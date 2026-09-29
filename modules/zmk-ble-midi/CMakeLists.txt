# Only the central half talks to the host, so only it gets the MIDI service.
# On the peripheral half ZMK_BLE_MIDI is off and nothing here is compiled; the
# devicetree nodes from dts/behaviors/midi.dtsi still exist but have no driver,
# which is harmless because nothing on that half references them.
if(CONFIG_ZMK_BLE_MIDI)
  zephyr_library()
  zephyr_library_include_directories(${APPLICATION_SOURCE_DIR}/include)
  # Global, so a display module can include <zmk_ble_midi/midi_state.h>.
  zephyr_include_directories(include)
  zephyr_library_sources(
    src/ble_midi.c
    src/behavior_midi.c
    src/input_processor_midi_xy.c
    src/midi_state.c
  )
endif()
