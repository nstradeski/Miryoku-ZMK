/*
 * MIDI key behaviors: notes, drums, chords, CCs and performance controls.
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_midi

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <drivers/behavior.h>

#include <zmk/behavior.h>
#include <zmk_ble_midi/ble_midi.h>
#include <zmk_ble_midi/midi_state.h>
#include <dt-bindings/zmk/midi.h>

LOG_MODULE_DECLARE(zmk_ble_midi, CONFIG_ZMK_LOG_LEVEL);

/* Order matches the midi-type enum in zmk,behavior-midi.yaml. */
enum midi_type {
    MIDI_TYPE_NOTE,
    MIDI_TYPE_CC,
    MIDI_TYPE_CC_TOGGLE,
    MIDI_TYPE_CONTROL,
    MIDI_TYPE_FIXED_NOTE,
    MIDI_TYPE_CHORD,
};

struct behavior_midi_config {
    enum midi_type type;
};

#define OCTAVE_MIN -4
#define OCTAVE_MAX 4
#define TRANSPOSE_MAX 11
#define VELOCITY_STEP 16
#define VELOCITY_MIN 8
#define MAX_HELD_NOTES 32

/* Chords are built around middle C (plus key and octave); bass notes sit an
 * octave below. */
#define CHORD_BASE_NOTE 60
#define BASS_BASE_NOTE 48

/* State shared by every MIDI key. */
static int8_t octave;
static int8_t transpose;
static uint8_t velocity = CONFIG_ZMK_BLE_MIDI_DEFAULT_VELOCITY;
static uint8_t program;
static uint8_t scale = ZMK_MIDI_SCALE_MAJOR;
static uint8_t inversion;
static uint32_t cc_toggled[128 / 32];

static const uint8_t scale_steps[][7] = {
    [ZMK_MIDI_SCALE_MAJOR] = {0, 2, 4, 5, 7, 9, 11},
    [ZMK_MIDI_SCALE_MINOR] = {0, 2, 3, 5, 7, 8, 10}, /* natural minor */
};

/* Notes are remembered per key so a release always turns off exactly the
 * notes that key started, even if the octave, key, scale or channel changed
 * while it was held. A chord key owns several entries. */
struct held_note {
    bool used;
    uint32_t position;
    uint8_t channel;
    uint8_t note;
};
static struct held_note held[MAX_HELD_NOTES];

static void send3(uint8_t status, uint8_t d1, uint8_t d2) {
    const uint8_t msg[] = {status, d1 & 0x7F, d2 & 0x7F};
    zmk_ble_midi_send(msg, sizeof(msg));
}

static void send2(uint8_t status, uint8_t d1) {
    const uint8_t msg[] = {status, d1 & 0x7F};
    zmk_ble_midi_send(msg, sizeof(msg));
}

static void send1(uint8_t status) { zmk_ble_midi_send(&status, 1); }

static void send_mmc(uint8_t command) {
    const uint8_t msg[] = {0xF0, 0x7F, 0x7F, 0x06, command, 0xF7};
    zmk_ble_midi_send(msg, sizeof(msg));
}

static void start_note(uint32_t position, int note) {
    if (note < 0 || note > 127) {
        return;
    }

    for (int i = 0; i < MAX_HELD_NOTES; i++) {
        if (!held[i].used) {
            uint8_t ch = zmk_ble_midi_channel();
            held[i] = (struct held_note){
                .used = true, .position = position, .channel = ch, .note = note};
            send3(0x90 | ch, note, velocity);
            return;
        }
    }
    LOG_WRN("Too many held notes, ignoring note %d", note);
}

static void stop_notes(uint32_t position) {
    for (int i = 0; i < MAX_HELD_NOTES; i++) {
        if (held[i].used && held[i].position == position) {
            send3(0x80 | held[i].channel, held[i].note, 0x40);
            held[i].used = false;
        }
    }
}

/* Semitones above the key's root for scale step `step` (may exceed 7). */
static int scale_offset(int step) { return scale_steps[scale][step % 7] + 12 * (step / 7); }

static void start_chord(uint32_t position, uint32_t param) {
    int degree = param & 0x07;
    int shift = 12 * octave + transpose + ((param & MC_8VA) ? 12 : 0);

    if (degree > 6) {
        return;
    }

    if (param & MC_BASS) {
        start_note(position, BASS_BASE_NOTE + shift + scale_offset(degree));
        return;
    }

    /* Stack thirds within the scale: degree, +2 steps, +4 (+6 for a 7th). */
    int count = (param & MC_7TH) ? 4 : 3;
    int notes[4];
    for (int i = 0; i < count; i++) {
        notes[i] = CHORD_BASE_NOTE + shift + scale_offset(degree + 2 * i);
    }
    /* Inversions lift the lowest notes an octave, keeping chord changes
     * closer together on the keyboard. */
    for (int i = 0; i < inversion && i < count; i++) {
        notes[i] += 12;
    }
    for (int i = 0; i < count; i++) {
        start_note(position, notes[i]);
    }
}

static void panic(void) {
    for (int i = 0; i < MAX_HELD_NOTES; i++) {
        if (held[i].used) {
            send3(0x80 | held[i].channel, held[i].note, 0x40);
            held[i].used = false;
        }
    }
    uint8_t ch = zmk_ble_midi_channel();
    send3(0xB0 | ch, 64, 0);  /* sustain off */
    send3(0xB0 | ch, 123, 0); /* all notes off */
}

struct zmk_midi_state_changed zmk_ble_midi_state(void) {
    return (struct zmk_midi_state_changed){
        .octave = octave,
        .transpose = transpose,
        .velocity = velocity,
        .channel = zmk_ble_midi_channel(),
        .scale = scale,
        .inversion = inversion,
        .connected = zmk_ble_midi_is_connected(),
    };
}

static void run_control(uint32_t command) {
    switch (command) {
    case MIDI_OCT_DN:
        octave = MAX(octave - 1, OCTAVE_MIN);
        break;
    case MIDI_OCT_UP:
        octave = MIN(octave + 1, OCTAVE_MAX);
        break;
    case MIDI_OCT_RST:
        octave = 0;
        break;
    case MIDI_KEY_DN:
        transpose = MAX(transpose - 1, -TRANSPOSE_MAX);
        break;
    case MIDI_KEY_UP:
        transpose = MIN(transpose + 1, TRANSPOSE_MAX);
        break;
    case MIDI_KEY_RST:
        transpose = 0;
        octave = 0;
        scale = ZMK_MIDI_SCALE_MAJOR;
        inversion = 0;
        break;
    case MIDI_SCALE:
        scale = scale == ZMK_MIDI_SCALE_MAJOR ? ZMK_MIDI_SCALE_MINOR : ZMK_MIDI_SCALE_MAJOR;
        break;
    case MIDI_INV:
        inversion = (inversion + 1) % 3;
        break;
    case MIDI_VEL_DN:
        velocity = MAX(velocity - VELOCITY_STEP, VELOCITY_MIN);
        break;
    case MIDI_VEL_UP:
        velocity = MIN(velocity + VELOCITY_STEP, 127);
        break;
    case MIDI_CH_DN:
        zmk_ble_midi_set_channel((zmk_ble_midi_channel() + 15) % 16);
        break;
    case MIDI_CH_UP:
        zmk_ble_midi_set_channel((zmk_ble_midi_channel() + 1) % 16);
        break;
    case MIDI_PC_DN:
        program = (program + 127) % 128;
        send2(0xC0 | zmk_ble_midi_channel(), program);
        break;
    case MIDI_PC_UP:
        program = (program + 1) % 128;
        send2(0xC0 | zmk_ble_midi_channel(), program);
        break;
    case MIDI_PANIC:
        panic();
        return;
    case MIDI_PLAY:
        send1(0xFA);
        send_mmc(0x02);
        return;
    case MIDI_STOP:
        send1(0xFC);
        send_mmc(0x01);
        return;
    case MIDI_CONT:
        send1(0xFB);
        return;
    case MIDI_REC:
        send_mmc(0x06);
        return;
    case MIDI_PAIR:
        zmk_ble_midi_pair();
        return;
    default:
        return;
    }
    LOG_INF("MIDI ch %d octave %d key %s %s inv %d velocity %d program %d",
            zmk_ble_midi_channel() + 1, octave, zmk_ble_midi_key_name(transpose),
            scale == ZMK_MIDI_SCALE_MAJOR ? "maj" : "min", inversion, velocity, program);
    zmk_ble_midi_state_notify();
}

static int on_midi_pressed(struct zmk_behavior_binding *binding,
                           struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_midi_config *cfg = dev->config;
    uint32_t param = binding->param1;

    switch (cfg->type) {
    case MIDI_TYPE_NOTE:
        start_note(event.position, (int)param + 12 * octave + transpose);
        break;
    case MIDI_TYPE_FIXED_NOTE:
        start_note(event.position, param);
        break;
    case MIDI_TYPE_CHORD:
        start_chord(event.position, param);
        break;
    case MIDI_TYPE_CC:
        zmk_ble_midi_send_cc(param, 127);
        break;
    case MIDI_TYPE_CC_TOGGLE: {
        uint8_t cc = param & 0x7F;
        cc_toggled[cc / 32] ^= BIT(cc % 32);
        zmk_ble_midi_send_cc(cc, (cc_toggled[cc / 32] & BIT(cc % 32)) ? 127 : 0);
        break;
    }
    case MIDI_TYPE_CONTROL:
        run_control(param);
        break;
    }
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_midi_released(struct zmk_behavior_binding *binding,
                            struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    const struct behavior_midi_config *cfg = dev->config;

    switch (cfg->type) {
    case MIDI_TYPE_NOTE:
    case MIDI_TYPE_FIXED_NOTE:
    case MIDI_TYPE_CHORD:
        stop_notes(event.position);
        break;
    case MIDI_TYPE_CC:
        zmk_ble_midi_send_cc(binding->param1, 0);
        break;
    default:
        break;
    }
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_midi_driver_api = {
    .binding_pressed = on_midi_pressed,
    .binding_released = on_midi_released,
};

#define MIDI_INST(n)                                                                               \
    static const struct behavior_midi_config behavior_midi_config_##n = {                          \
        .type = DT_INST_ENUM_IDX(n, midi_type),                                                    \
    };                                                                                             \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, &behavior_midi_config_##n, POST_KERNEL,           \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_midi_driver_api);

DT_INST_FOREACH_STATUS_OKAY(MIDI_INST)
