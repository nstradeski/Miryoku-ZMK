/*
 * Parameters for the zmk-ble-midi behaviors.
 *
 *   &midi_note MIDI_N(MN_C, 4)   note (MIDI number, shifted by octave/transpose)
 *   &midi_cc 64                  CC 127 while held, 0 on release (e.g. sustain)
 *   &midi_cc_tog 20              CC toggles 127 / 0 on each press
 *   &midi_ctl MIDI_OCT_UP        one of the commands below
 */

#pragma once

/* Note names, in semitones above C. */
#define MN_C 0
#define MN_CS 1
#define MN_D 2
#define MN_DS 3
#define MN_E 4
#define MN_F 5
#define MN_FS 6
#define MN_G 7
#define MN_GS 8
#define MN_A 9
#define MN_AS 10
#define MN_B 11

/* MIDI note number, using the "middle C = C4 = 60" convention. Apple apps
 * (GarageBand, Logic) call the same note C3. */
#define MIDI_N(note, octave) (((octave) + 1) * 12 + (note))

/* &midi_ctl commands */
#define MIDI_OCT_DN 0   /* octave down (held notes still release correctly) */
#define MIDI_OCT_UP 1
#define MIDI_OCT_RST 2  /* octave and transpose back to 0 */
#define MIDI_SEMI_DN 3  /* transpose one semitone */
#define MIDI_SEMI_UP 4
#define MIDI_VEL_DN 5   /* velocity -16 */
#define MIDI_VEL_UP 6   /* velocity +16 */
#define MIDI_CH_DN 7    /* MIDI channel */
#define MIDI_CH_UP 8
#define MIDI_PC_DN 9    /* program change to previous / next program */
#define MIDI_PC_UP 10
#define MIDI_PANIC 11   /* release every held note, All Notes Off, sustain off */
#define MIDI_PLAY 12    /* MIDI Start + MMC Play */
#define MIDI_STOP 13    /* MIDI Stop + MMC Stop */
#define MIDI_CONT 14    /* MIDI Continue */
#define MIDI_REC 15     /* MMC Record Strobe */
#define MIDI_PAIR 16    /* advertise the MIDI service so the iPad can find it */
