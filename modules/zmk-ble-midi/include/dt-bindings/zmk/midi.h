/*
 * Parameters for the zmk-ble-midi behaviors.
 *
 *   &midi_note MIDI_N(MN_C, 4)   note (MIDI number, shifted by octave and key)
 *   &midi_drum GM_KICK           fixed note, never shifted (drums, samples)
 *   &midi_chord MC_V             diatonic chord in the current key and scale
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

/* &midi_drum MIDI_ON_CH(16, 5): a fixed note on a fixed MIDI channel (1-16)
 * instead of the current one. Used by the DJ mode so its buttons never
 * collide with notes from the other modes. */
#define MIDI_ON_CH(channel, note) (((channel) << 8) | (note))

/* &midi_ctl commands */
#define MIDI_OCT_DN 0   /* octave down (held notes still release correctly) */
#define MIDI_OCT_UP 1
#define MIDI_OCT_RST 2  /* octave back to 0 */
#define MIDI_KEY_DN 3   /* key (transpose) down one semitone: notes and chords */
#define MIDI_KEY_UP 4
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
#define MIDI_KEY_RST 17 /* key back to C major, root position, octave 0 */
#define MIDI_SCALE 18   /* toggle major / minor (chords) */
#define MIDI_INV 19     /* cycle chord inversion: root, 1st, 2nd */

/* Older names for the key commands. */
#define MIDI_SEMI_DN MIDI_KEY_DN
#define MIDI_SEMI_UP MIDI_KEY_UP

/* &midi_chord: scale degree, optionally OR-ed with flags.
 *   MC_IV           triad on the 4th degree (F in C major, Fm... in minor)
 *   MC_V | MC_7TH   four-note seventh chord (G7 in C major)
 *   MC_I | MC_BASS  just the root, an octave below the chords (bass line)
 *   MC_II | MC_8VA  an octave higher */
#define MC_I 0
#define MC_II 1
#define MC_III 2
#define MC_IV 3
#define MC_V 4
#define MC_VI 5
#define MC_VII 6
#define MC_7TH 0x08
#define MC_BASS 0x10
#define MC_8VA 0x20

/* General MIDI drum map (use with &midi_drum). */
#define GM_KICK2 35
#define GM_KICK 36
#define GM_SIDESTICK 37
#define GM_SNARE 38
#define GM_CLAP 39
#define GM_SNARE2 40
#define GM_FLOOR_TOM_LO 41
#define GM_HIHAT_CLOSED 42
#define GM_FLOOR_TOM_HI 43
#define GM_HIHAT_PEDAL 44
#define GM_TOM_LO 45
#define GM_HIHAT_OPEN 46
#define GM_TOM_LOMID 47
#define GM_TOM_HIMID 48
#define GM_CRASH 49
#define GM_TOM_HI 50
#define GM_RIDE 51
#define GM_CHINA 52
#define GM_RIDE_BELL 53
#define GM_TAMBOURINE 54
#define GM_SPLASH 55
#define GM_COWBELL 56
#define GM_CRASH2 57
#define GM_RIDE2 59
#define GM_BONGO_HI 60
#define GM_BONGO_LO 61
#define GM_CONGA_HI_MUTE 62
#define GM_CONGA_HI_OPEN 63
#define GM_CONGA_LO 64
#define GM_TIMBALE_HI 65
#define GM_TIMBALE_LO 66
#define GM_CABASA 69
#define GM_MARACAS 70
#define GM_CLAVES 75
#define GM_TRIANGLE_OPEN 81
