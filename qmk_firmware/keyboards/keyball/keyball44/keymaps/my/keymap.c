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

#include "action.h"
#include "avr/pgmspace.h"
#include "default_keyboard.h"
#include "drivers/oled/oled_driver.h"
#include "info_config.h"
#include "keyball44/config.h"
#include "keyball44/keyball44.h"
#include "pointing_device/pointing_device.h"
#include "pointing_device/pointing_device_auto_mouse.h"
#include "quantum.h"
#include "report.h"
#include "rgblight/rgblight.h"
#include "stdint.h"

enum layers {
    BASE = 0,
    NUM,
    MOUSE,
    SYSTEM,
    ARROW,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [BASE] = LAYOUT_right_ball(
    LT(ARROW, KC_ESC) , KC_Q    , KC_W    , KC_E     , KC_R     , KC_T     ,                                  KC_Y     , KC_U     , KC_I     , KC_O     , KC_P           , KC_LBRC,
    LCTL_T(KC_TAB)    , KC_A    , KC_S    , KC_D     , KC_F     , KC_G     ,                                  KC_H     , KC_J     , KC_K     , KC_L     , RCTL_T(KC_SCLN), C_S_T(KC_RBRC) ,
    LSFT_T(KC_MINS)   , KC_Z    , KC_X    , KC_C     , KC_V     , KC_B     ,                                  KC_N     , KC_M     , KC_COMM  , KC_DOT   , RSFT_T(KC_SLSH), RALT_T(KC_BSLS),
                                  MO(SYSTEM), KC_APP, LSFT_T(KC_BSPC) ,KC_SPC, KC_LGUI,                   KC_RGUI, LT(NUM,KC_ENT)                             , KC_MEH
  ),

  [NUM] = LAYOUT_right_ball(
    KC_EQL      ,  S(KC_1)   , S(KC_2)   , S(KC_3)   , S(KC_4)   , S(KC_5)   ,                                       S(KC_6)   , S(KC_7)   , S(KC_8)   , S(KC_9)   , S(KC_0) , KC_QUOT ,
    S(KC_EQL)   ,  KC_1      , KC_2      , KC_3      , KC_4      , KC_5      ,                                       KC_6      , KC_7      , KC_8      , KC_9      , KC_0    , S(KC_QUOT) ,
    S(KC_GRAVE) ,  MEH(KC_1) , MEH(KC_2) , MEH(KC_3) , MEH(KC_4) , MEH(KC_5) ,                                       MEH(KC_6) , MEH(KC_7) , MEH(KC_8) , MEH(KC_9) , KC_DEL  , KC_GRAVE ,
                               _______   , _______       , LAG(KC_NO) , _______ , _______ ,              _______  , _______                                , _______
  ),

  [MOUSE] = LAYOUT_right_ball(
    _______ , _______ , _______ , _______ , _______ , _______ ,                                        _______  , _______ , _______ , _______ , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,                                        SCRL_TO  , KC_BTN1 , SCRL_MO , KC_BTN2 , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,                                        _______  , _______ , _______ , _______ , _______ , _______ ,
                        _______ , _______  ,    _______  , _______  , _______  ,                _______  , _______                            , _______
  ),

  [SYSTEM] = LAYOUT_right_ball(
    RGB_TOG  , _______  , _______ , _______ , _______ , _______ ,                                       KBC_SAVE , _______ , _______  , _______ , _______ , _______ ,
    RGB_MOD  , RGB_HUI  , RGB_SAI , RGB_VAI , _______ , _______ ,                                       SCRL_DVI , CPI_I1K , CPI_I100 , _______ , _______ , _______ ,
    RGB_RMOD , RGB_HUD  , RGB_SAD , RGB_VAD , _______ , _______ ,                                       SCRL_DVD , CPI_D1K , CPI_D100 , _______ , _______ , _______ ,
                       _______ , _______              , EE_CLR , _______ , QK_BOOT ,       _______  , _______                                   , _______
  ),

  [ARROW] = LAYOUT_right_ball(
    _______ , _______  , _______ , _______ , _______ , _______ ,                                       KC_HOME    , KC_PGDN    , KC_PGUP  , KC_END      , _______ , _______ ,
    _______ , _______  , _______ , _______ , _______ , _______ ,                                       KC_LEFT    , KC_DOWN    , KC_UP    , KC_RIGHT    , _______ , _______ ,
    _______ , _______  , _______ , _______ , _______ , _______ ,                                       A(KC_LEFT) , A(KC_DOWN) , A(KC_UP) , A(KC_RIGHT) , _______ , _______ ,
                         _______ , _______              , _______ , _______ , _______ ,       _______  , _______                                        , _______
  ),
};
// clang-format on

int current_layer = BASE;
layer_state_t layer_state_set_user(layer_state_t state) {
    current_layer = get_highest_layer(state);
    keyball_set_scroll_mode(get_highest_layer(state) == ARROW);
    return state;
}

void pointing_device_init_user(void) {
    set_auto_mouse_layer(MOUSE);
    set_auto_mouse_enable(true);
}

bool is_mouse_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case SCRL_MO:
    case SCRL_TO:
        return true;
    default:
        return false;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keyball_get_scroll_mode() && keycode != SCRL_TO &&
        record->event.pressed) {
        keyball_set_scroll_mode(false);
    }
    return true;
}

#ifdef OLED_ENABLE

#include "lib/oledkit/oledkit.h"

void my_oled_render_info(void) {
    oled_write_P(PSTR("L:"), false);
    switch (current_layer) {
    case BASE:
        oled_write_P(PSTR("Base "), false);
        break;
    case NUM:
        oled_write_P(PSTR("Num  "), false);
        break;
    case MOUSE:
        oled_write_P(PSTR("Mouse"), false);
        break;
    case ARROW:
        oled_write_P(PSTR("Arrow"), false);
        break;
    case SYSTEM:
        oled_write_P(PSTR("Sys  "), false);
        break;
    default:
        break;
    }
}

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    my_oled_render_info();
}
#endif
