// Copyright 2022 Kyle McCreery (@kylemccreery)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "sugarglider.h"
#ifdef QMK_SETTINGS
#include "qmk_settings.h"
#endif

#ifndef GLIDEPOINT_DPI_OPTIONS
#    define GLIDEPOINT_DPI_OPTIONS \
        { 400, 800, 1200, 1600, 2000, 2400, 2800, 3200, 3600, 4000 }
#    ifndef GLIDEPOINT_DPI_DEFAULT
#        define GLIDEPOINT_DPI_DEFAULT 1
#    endif
#endif
#ifndef GLIDEPOINT_DPI_DEFAULT
#    define GLIDEPOINT_DPI_DEFAULT 1
#endif

keyboard_config_t keyboard_config;

#ifndef POINTING_DEVICE_ENABLE
#undef DYNAMIC_TOUCH_TAP_ENABLE
#endif

#ifdef POINTING_DEVICE_ENABLE
uint16_t          dpi_array[] = GLIDEPOINT_DPI_OPTIONS;
#define DPI_OPTION_SIZE (sizeof(dpi_array) / sizeof(uint16_t))
static void eeprom_settings_save(void) {
#if defined(QMK_SETTINGS) && !defined(DYNAMIC_TOUCH_TAP_ENABLE)
    for (size_t i = 0; i < sizeof(qmk_settings_t); ++i) {
        uint8_t old_byte, new_byte;
        old_byte = dynamic_keymap_get_qmk_settings(i);
        memcpy(&new_byte, (char*)&QS + i, 1);
        if (old_byte != new_byte)
            dynamic_keymap_set_qmk_settings(i, new_byte);
    }
#endif
    eeconfig_update_kb(keyboard_config.raw);
}

#endif

void board_init(void) {
    // B9 is configured as I2C1_SDA in the board file; that function must be
    // disabled before using B7 as I2C1_SDA.
    setPinInputHigh(B9);
    setPinOutput(B12);
    setPinOutput(B13);
    setPinOutput(B14);
    setPinOutput(C13);
}


#if (defined(QMK_SETTINGS) && defined(DYNAMIC_TOUCH_TAP_ENABLE))
report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {       // here to sync settings from VIAL GUI for tap term to our eeprom config
    if (keyboard_config.dt_term_config != QS.tapping_term && keyboard_config.dt_term_config > 0) {
            keyboard_config.dt_term_config = QS.tapping_term;
            eeprom_settings_save();
    }
    return pointing_device_task_user(mouse_report);
}
#endif

#ifdef DYNAMIC_TOUCH_TAP_ENABLE
void tap_modify(int change_value, bool tap_status) {
    if (keyboard_config.dt_term_config < 0) {
        keyboard_config.dt_term_config *= -1;
    }
    keyboard_config.dt_term_config += change_value;
    if (keyboard_config.dt_term_config < 0) {
        keyboard_config.dt_term_config = 0;
    }
    if (tap_status == false ) {
        keyboard_config.dt_term_config *= -1;
#if defined(QMK_SETTINGS) && !defined(DYNAMIC_TOUCH_TAP_ENABLE)
        QS.tapping_term = 0;
#else
        g_touch_tapping_term = 0;
#endif
    } else {
#if defined(QMK_SETTINGS) && !defined(DYNAMIC_TOUCH_TAP_ENABLE)
        QS.tapping_term = keyboard_config.dt_term_config;
#else
        g_touch_tapping_term = keyboard_config.dt_term_config;
#endif
    }
    eeprom_settings_save();
}

void tap_toggle(void) {
    keyboard_config.dt_term_config *= -1;
    if (keyboard_config.dt_term_config > 0) {
#if defined(QMK_SETTINGS) && !defined(DYNAMIC_TOUCH_TAP_ENABLE)
        QS.tapping_term = keyboard_config.dt_term_config;
#else
        g_touch_tapping_term = keyboard_config.dt_term_config;
#endif
    } else {
#if defined(QMK_SETTINGS) && !defined(DYNAMIC_TOUCH_TAP_ENABLE)
        QS.tapping_term = 0;
#else
        g_touch_tapping_term = 0;
#endif
    }
    eeprom_settings_save();
}
#endif

#ifdef DIP_SWITCH_ENABLE
bool dip_switch_update_kb(uint8_t index, bool active) {
    if (!dip_switch_update_user(index, active)) { return false; }
    switch (index) {
        case 0:
            if(active) { tap_code(KC_CLCK); }
            break;
        break;
    }
    return true;
}
#endif

#ifdef ENCODER_ENABLE
bool encoder_update_kb(uint8_t index, bool clockwise) {
    if (!encoder_update_user(index, clockwise)) { return false; }
    switch (index) {
        case 0:
            if (clockwise) {
                tap_code(KC_VOLU);
            } else {
                tap_code(KC_VOLD);
            }
        break;
        case 1:
            if (clockwise) {
                tap_code(KC_VOLU);
            } else {
                tap_code(KC_VOLD);
            }
        break;
        case 2:
            if (clockwise) {
                tap_code(KC_PGUP);
            } else {
                tap_code(KC_PGDN);
            }
        break;
    }
    return true;
}
#endif

bool led_update_kb(led_t led_state) {
    bool res = led_update_user(led_state);
    if(res) {
        // writePin sets the pin high for 1 and low for 0.
        // In this example the pins are inverted, setting
        // it low/0 turns it on, and high/1 turns the LED off.
        // This behavior depends on whether the LED is between the pin
        // and VCC or the pin and GND.
        writePin(B12, !led_state.num_lock);
        writePin(B13, !led_state.caps_lock);
        writePin(B14, !led_state.scroll_lock);
        writePin(C13, !led_state.caps_lock);
    }
    return res;
}

#ifdef OLED_ENABLE
#ifndef OLED_DISPLAY_128X64
	oled_rotation_t oled_init_user(oled_rotation_t rotation) {
		return OLED_ROTATION_270;       // flips the display 270 degrees
	}

	static void render_logo(void) {     // Render MechWild "MW" Logo
		static const char PROGMEM logo_1[] = {0x8A, 0x8B, 0x8C, 0x8D, 0x00};
		static const char PROGMEM logo_2[] = {0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0x00};
		static const char PROGMEM logo_3[] = {0xCA, 0xCB, 0xCC, 0xCD, 0x00};
		static const char PROGMEM logo_4[] = {0x20, 0x8E, 0x8F, 0x90, 0x00};
		oled_set_cursor(0,0);
		oled_write_P(logo_1, false);
		oled_set_cursor(0,1);
		oled_write_P(logo_2, false);
		oled_set_cursor(0,2);
		oled_write_P(logo_3, false);
		oled_set_cursor(0,3);
		oled_write_P(logo_4, false);
	}

	bool oled_task_kb(void) {
        if(!oled_task_user()) {
            return false;
        }
		render_logo();
		oled_set_cursor(0,6);

		oled_write_ln_P(PSTR("Layer"), false);

    switch (get_highest_layer(layer_state)) {
        case 0:
            oled_write_ln_P(PSTR("Base "), false);
            break;
        case 1:
            oled_write_ln_P(PSTR("FN 1 "), false);
            break;
        case 2:
            oled_write_ln_P(PSTR("FN 2 "), false);
            break;
        case 3:
            oled_write_ln_P(PSTR("FN 3 "), false);
            break;
        default:
            oled_write_ln_P(PSTR("Undef"), false);
    }
	oled_write_ln_P(PSTR(""), false);
    // Host Keyboard LED Status
    led_t led_state = host_keyboard_led_state();
    oled_write_ln_P(led_state.num_lock ? PSTR("NUM  ") : PSTR("     "), false);
    oled_write_ln_P(led_state.caps_lock ? PSTR("CAP  ") : PSTR("     "), false);
    oled_write_ln_P(led_state.scroll_lock ? PSTR("SCR  ") : PSTR("     "), false);
    oled_write_ln_P(PSTR("     "), false);
#ifdef POINTING_DEVICE_ENABLE
    oled_write_P(PSTR("DPI: "), false);
    oled_write(get_u16_str(dpi_array[keyboard_config.dpi_config], ' '), false);
#endif
#ifdef DYNAMIC_TOUCH_TAP_ENABLE 
    oled_write_P(PSTR("TAP: "), false);
    if (keyboard_config.dt_term_config < 0) {
        oled_write_P(PSTR("Off  "), false);
    } else {
#if defined(QMK_SETTINGS) && !defined(DYNAMIC_TOUCH_TAP_ENABLE)
        oled_write(get_u16_str(QS.tapping_term, ' '), false);
#else
        oled_write(get_u16_str(g_touch_tapping_term, ' '), false);
#endif
    }
#endif

    return true;
	}
#else
    oled_rotation_t oled_init_user(oled_rotation_t rotation) {
		return OLED_ROTATION_180;       // flips the display 270 degrees
	}
	static void render_logo(void) {     // Render MechWild "MW" Logo
		static const char PROGMEM logo_1[] = {0x8A, 0x8B, 0x8C, 0x8D, 0x00};
		static const char PROGMEM logo_2[] = {0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0x00};
		static const char PROGMEM logo_3[] = {0xCA, 0xCB, 0xCC, 0xCD, 0x00};
		static const char PROGMEM logo_4[] = {0x20, 0x8E, 0x8F, 0x90, 0x00};
		oled_set_cursor(0,0);
		oled_write_P(logo_1, false);
		oled_set_cursor(0,1);
		oled_write_P(logo_2, false);
		oled_set_cursor(0,2);
		oled_write_P(logo_3, false);
		oled_set_cursor(0,3);
		oled_write_P(logo_4, false);
	}

	bool oled_task_kb(void) {
        if(!oled_task_user()) {
            return false;
        }
        render_logo();
        oled_set_cursor(8,1);
        switch (get_highest_layer(layer_state)) {
            case 0:
                oled_write_P(PSTR("Layer 0"), false);
                break;
            case 1:
                oled_write_P(PSTR("Layer 1"), false);
                break;
            case 2:
                oled_write_P(PSTR("Layer 2"), false);
                break;
            case 3:
                oled_write_P(PSTR("Layer 3"), false);
                break;
            default:
                oled_write_P(PSTR("Layer ?"), false);    // Should never display, here as a catchall
        }
        led_t led_state = host_keyboard_led_state();
        oled_set_cursor(8,0);
        oled_write_P(led_state.num_lock ? PSTR("NUM ") : PSTR("    "), false);
        oled_write_P(led_state.caps_lock ? PSTR("CAP ") : PSTR("    "), false);
        oled_write_P(led_state.scroll_lock ? PSTR("SCR") : PSTR("    "), false);
#ifdef POINTING_DEVICE_ENABLE
        oled_set_cursor(8,2);
        oled_write_P(PSTR("DPI:"), false);
        oled_write(get_u16_str(dpi_array[keyboard_config.dpi_config], ' '), false);
#endif
#ifdef DYNAMIC_TOUCH_TAP_ENABLE              // only display tap info if it is being configured dynamically
        oled_set_cursor(8,3);
        oled_write_P(PSTR("TAP:"), false);
        if (keyboard_config.dt_term_config < 0) {
            oled_write_P(PSTR("Off  "), false);
        } else {
#if defined(QMK_SETTINGS) && !defined(DYNAMIC_TOUCH_TAP_ENABLE)
            oled_write(get_u16_str(QS.tapping_term, ' '), false);
#else
            oled_write(get_u16_str(g_touch_tapping_term, ' '), false);
#endif
        }
#endif
    return true;
}
#endif
#endif
bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
    switch(keycode) {
#ifdef POINTING_DEVICE_ENABLE
        case DPI_UP:
            if (record->event.pressed) {
                keyboard_config.dpi_config = (keyboard_config.dpi_config + 1) % DPI_OPTION_SIZE;
                eeconfig_update_kb(keyboard_config.raw);
                pointing_device_set_cpi(dpi_array[keyboard_config.dpi_config]);
            }
            return false;
        case DPI_DN:
            if (record->event.pressed) {
                if (keyboard_config.dpi_config > 0) {
                    keyboard_config.dpi_config = (keyboard_config.dpi_config - 1) % DPI_OPTION_SIZE;
                } else {
                    keyboard_config.dpi_config = DPI_OPTION_SIZE - 1;
                }
                eeconfig_update_kb(keyboard_config.raw);
                pointing_device_set_cpi(dpi_array[keyboard_config.dpi_config]);
            }
            return false;
        case DPI_FINE:
            if (record->event.pressed) {
                pointing_device_set_cpi(dpi_array[0]);
            } else {
                pointing_device_set_cpi(dpi_array[keyboard_config.dpi_config]);
            }
            return false;
#endif
#ifdef DYNAMIC_TOUCH_TAP_ENABLE // only include tap info keycodes if it is being configured dynamically
        case TAP_UP:
            if (record->event.pressed) {
                tap_modify(DYNAMIC_TAPPING_TERM_INCREMENT, true);
            }
            return false;
        case TAP_DN:
            if (record->event.pressed) {
                if (keyboard_config.dt_term_config > 0) {
                    tap_modify(-1 * DYNAMIC_TAPPING_TERM_INCREMENT, true);
                }
            }
            return false;
        case TAP_ON:
            if (record->event.pressed) {
                tap_modify(0, true);
            }
            return false;
        case TAP_OFF:
            if (record->event.pressed) {
                tap_modify(0, false);
            }
            return false;
        case TAP_TOG:
            if (record->event.pressed) {
                tap_toggle();
            }
            return false;
#endif
    }
    return process_record_user(keycode, record);
}

void pointing_device_init_kb(void) {
#ifdef POINTING_DEVICE_ENABLE
    pointing_device_set_cpi(dpi_array[keyboard_config.dpi_config]);
#endif
}

void eeconfig_init_kb(void) {
#ifdef POINTING_DEVICE_ENABLE
    keyboard_config.dpi_config = GLIDEPOINT_DPI_DEFAULT;
#endif
#ifdef DYNAMIC_TOUCH_TAP_ENABLE // only set tap term from eeprom if it is being configured dynamically
    keyboard_config.dt_term_config = TAPPING_TERM;
#endif
    eeconfig_update_kb(keyboard_config.raw);
    eeconfig_init_user();
}

void matrix_init_kb(void) {
    // is safe to just read DPI setting since matrix init
    // comes before pointing device init.
    keyboard_config.raw = eeconfig_read_kb();
#ifdef POINTING_DEVICE_ENABLE
    if (keyboard_config.dpi_config > DPI_OPTION_SIZE) {
        eeconfig_init_kb();
    }
#endif
    matrix_init_user();
}

void keyboard_post_init_kb(void) {
#ifdef POINTING_DEVICE_ENABLE
    pointing_device_set_cpi(dpi_array[keyboard_config.dpi_config]);
#endif
#ifdef RGBLIGHT_ENABLE
    rgblight_toggle_noeeprom();     //double toggle post init removes the weirdness with rgb strips having a yellow first LED
    rgblight_toggle_noeeprom();
#endif
#ifdef DYNAMIC_TOUCH_TAP_ENABLE
    tap_toggle();   // Need it to reevaluate this setting after initiating so that it is current after init
    tap_toggle();
#endif
    keyboard_post_init_user();
#ifdef OLED_ENABLE  // purposefully after user post init to allow the RGB to startup first
    wait_ms(200);   // Avoids a startup issue where the oled renders and then turns off with blackpill
    oled_on();
#endif
}
