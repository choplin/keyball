/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

#include "quantum.h"
#include "avr/pgmspace.h"
#include "report.h"
#include "pointing_device.h"
#include "keyball44/keyball44.h"
#include "keyball44/config.h"
#include "stdint.h"


enum layers {
    BASE = 0,
    NUM,
    MOUSE,
    ARROW,
};

// clang-format off
// const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [BASE] = LAYOUT_universal(
    LT(ARROW, KC_ESC) , KC_Q    , KC_W    , KC_E     , KC_R     , KC_T     ,                                  KC_Y     , KC_U     , KC_I     , KC_O     , KC_P           , LT(ARROW, KC_LBRC),
    LCTL_T(KC_TAB)    , KC_A    , KC_S    , KC_D     , KC_F     , KC_G     ,                                  KC_H     , KC_J     , KC_K     , KC_L     , RCTL_T(KC_SCLN), C_S_T(KC_RBRC) ,
    LSFT_T(KC_MINS)   , KC_Z    , KC_X    , KC_C     , KC_V     , KC_B     ,                                  KC_N     , KC_M     , KC_COMM  , KC_DOT   , RSFT_T(KC_SLSH), RALT_T(KC_BSLS),
                                  TO(BASE), KC_F12,     KC_BSPC ,KC_SPC, KC_LCTL,                   KC_RCTL, LT(NUM,KC_ENT)       , _______  , _______  , _______
  ),

  [NUM] = LAYOUT_universal(
    KC_EQL            ,  S(KC_1)   , S(KC_2)   , S(KC_3)   , S(KC_4)   , S(KC_5)   ,                                       S(KC_6)   , S(KC_7)   , S(KC_8)   , S(KC_9)   , S(KC_0) , KC_QUOT ,
    LCTL_T(S(KC_EQL)) ,  KC_1      , KC_2      , KC_3      , KC_4      , KC_5      ,                                       KC_6      , KC_7      , KC_8      , KC_9      , KC_0    , S(KC_QUOT) ,
    LSFT_T(KC_GRAVE)  ,  RCS(KC_1) , RCS(KC_2) , RCS(KC_3) , RCS(KC_4) , RCS(KC_5) ,                                       RCS(KC_6) , RCS(KC_7) , RCS(KC_8) , RCS(KC_9) , KC_DEL  , S(KC_GRAVE) ,
                                     _______   , _______       , _______ , _______ , _______ ,                      _______  , _______           , _______   , _______   , _______
  ),

  [MOUSE] = LAYOUT_universal(
    _______ , _______ , _______ , _______ , _______ , _______ ,                                        _______  , _______ , _______ , _______ , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,                                        _______  , KC_BTN1 , SCRL_TO , KC_BTN2 , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,                                        _______  , SCRL_MO , _______ , _______ , _______ , _______ ,
                        _______ , _______  ,    _______  , _______  , _______  ,                _______  , _______        , _______ , _______ , _______
  ),

  [ARROW] = LAYOUT_universal(
    RGB_TOG  , _______  , KBC_RST , KBC_SAVE , SCRL_DVD , SCRL_DVI ,                                       KC_HOME    , KC_PGDN    , KC_PGUP  , KC_END      , _______ , _______ ,
    RGB_MOD  , RGB_HUI  , RGB_SAI , RGB_VAI  , CPI_D1K  , CPI_I1K  ,                                       KC_LEFT    , KC_DOWN    , KC_UP    , KC_RIGHT    , _______ , _______ ,
    RGB_RMOD , RGB_HUD  , RGB_SAD , RGB_VAD  , CPI_D100 , CPI_I100 ,                                       C(KC_LEFT) , C(KC_DOWN) , C(KC_UP) , C(KC_RIGHT) , _______ , _______ ,
                         _______ , _______    , _______ , _______ , QK_BOOT ,                      _______  , _______              , _______  , _______     , _______
  ),
};
// clang-format on

int current_layer = BASE;
layer_state_t layer_state_set_user(layer_state_t state) {
    current_layer = get_highest_layer(state);
    keyball_set_scroll_mode(get_highest_layer(state) == ARROW);
    return state;
}

struct mouse_state_t {
    enum {
        NORMAL = 0,
        MOUSE_MOVED,
        MOUSE_LAYER_ENABLED,
        CLICKING,
    } state;
    uint16_t mouse_moved_timer;
    int16_t mouse_movement; // The amount of movement during WATING state
    int16_t after_click_mouse_movement; // The amount of movement after a click
};

struct mouse_state_t mouse_state = {
    .state = NORMAL,
    .mouse_movement = 0,
    .after_click_mouse_movement = 0,
};

union user_config_t{
  uint32_t raw;
  struct {
    int16_t mouse_movement_threshold;
  };
} ;
union user_config_t user_config;

void eeconfig_init_user(void) {
    user_config.raw = 0;
    user_config.mouse_movement_threshold = 50;
    eeconfig_update_user(user_config.raw);
}

void keyboard_post_init_user(void) {
    user_config.raw = eeconfig_read_user();
}

#define MOUSE_LAYER_TIMEOUT 500
#define MOUSE_MOVED_STATE_TIMEOUT 500
#define MOUSE_MOVE_LOCK_AFTER_CLICK 30

const uint16_t keys_to_ignore_for_disabling_mouse_layer[] = {KC_LGUI, KC_LCTL};
uint16_t keys_to_ignore_for_disabling_mouse_layer_length = sizeof(keys_to_ignore_for_disabling_mouse_layer) / sizeof(keys_to_ignore_for_disabling_mouse_layer[0]);

void enable_mouse_layer(void) {
    layer_on(MOUSE);
    mouse_state.mouse_moved_timer = timer_read();
    mouse_state.state = MOUSE_LAYER_ENABLED;
}

void disable_mouse_layer(void) {
    mouse_state.state = NORMAL;
    layer_off(MOUSE);
}

int16_t my_abs(int16_t num) {
    if (num < 0) {
        return -num;
    }
    return num;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_BTN1:
        case KC_BTN2:
        case KC_BTN3:
        {
            report_mouse_t currentReport = pointing_device_get_report();

            uint8_t btn = 1 << (keycode - KC_BTN1);
            if (record->event.pressed) {
                currentReport.buttons |= btn;
                mouse_state.state = CLICKING;
                mouse_state.after_click_mouse_movement = 0;
            } else {
                currentReport.buttons &= ~btn;
                mouse_state.state = MOUSE_LAYER_ENABLED;
            }

            mouse_state.mouse_moved_timer = timer_read();

            pointing_device_set_report(currentReport);
            pointing_device_send();
            return false;
        }

        case SCRL_TO:
        case SCRL_MO:
            return true;

        default:
            if  (record->event.pressed) {
                if (mouse_state.state == CLICKING) {
                    return true;
                } else if (mouse_state.state == MOUSE_LAYER_ENABLED) {
                    for (int i = 0; i < keys_to_ignore_for_disabling_mouse_layer_length; i++) {
                        if (keycode == keys_to_ignore_for_disabling_mouse_layer[i]) {
                            return false;
                        }
                    }
                }
                disable_mouse_layer();
            }
            return true;
    }
}


report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    int16_t current_x;
    int16_t current_y;

    if (mouse_report.x != 0 || mouse_report.y != 0) {
        switch (mouse_state.state) {
            case MOUSE_LAYER_ENABLED:
                mouse_state.mouse_moved_timer = timer_read();
                break;

            case CLICKING:
                current_x = mouse_report.x;
                current_y = mouse_report.y;
                mouse_state.after_click_mouse_movement -= my_abs(current_x) + my_abs(current_y);
                if (mouse_state.after_click_mouse_movement > MOUSE_MOVE_LOCK_AFTER_CLICK) {
                    current_x = 0;
                    current_y = 0;
                }
                mouse_report.x = current_x;
                mouse_report.y = current_y;

                break;

            case MOUSE_MOVED:
                mouse_state.mouse_movement += my_abs(current_x) + my_abs(current_y);
                if (mouse_state.mouse_movement >= user_config.mouse_movement_threshold) {
                    mouse_state.mouse_movement = 0;
                    enable_mouse_layer();
                }
                break;

            case NORMAL:
                mouse_state.mouse_moved_timer = timer_read();
                mouse_state.state = MOUSE_MOVED;
                mouse_state.mouse_movement = 0;
                break;

            default:
                break;
        }
    } else {
        switch (mouse_state.state) {
            case CLICKING:
                break;

            case MOUSE_LAYER_ENABLED:
                if (!keyball_get_scroll_mode() ) {
                    if (timer_elapsed(mouse_state.mouse_moved_timer) > MOUSE_LAYER_TIMEOUT) {
                        disable_mouse_layer();
                    }
                }
                break;

             case MOUSE_MOVED:
                if (timer_elapsed(mouse_state.mouse_moved_timer) > MOUSE_MOVED_STATE_TIMEOUT) {
                    mouse_state.mouse_movement = 0;
                    mouse_state.state = NORMAL;
                }
                break;

            case NORMAL:
                mouse_state.mouse_movement = 0;
                mouse_state.state = NORMAL;

            default:
                break;
        }
    }

    return mouse_report;
}

#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"

void my_oled_render_mouse_info(void) {
    oled_write_P(PSTR("L:"), false);
    switch (current_layer) {
        case BASE:
            oled_write_P(PSTR("BASE "), false);
            break;
        case NUM:
            oled_write_P(PSTR("NUM  "), false);
            break;
        case MOUSE:
            oled_write_P(PSTR("ARROW"), false);
            break;
        case ARROW:
            oled_write_P(PSTR("ARROW"), false);
            break;
        default:
            break;
    }
    oled_write_P(PSTR("  M:"), false);
    switch (mouse_state.state) {
        case MOUSE_MOVED:
            oled_write_P(PSTR("Moved  "), false);
            break;
        case CLICKING:
            oled_write_P(PSTR("Click  "), false);
            break;
        case MOUSE_LAYER_ENABLED:
            oled_write_P(PSTR("Enabled"), false);
            break;
        case NORMAL:
            oled_write_P(PSTR("Normal "), false);
            break;
        default:
            break;
    }
}

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    my_oled_render_mouse_info();
}
#endif
