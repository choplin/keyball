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
#include "stdint.h"
#include "avr/pgmspace.h"
#include "keyball44/keyball44.h"
#include "keyball44/config.h"

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
    KC_ESC          , KC_Q         , KC_W    , KC_E     , KC_R           , KC_T     ,                                  KC_Y     , KC_U     , KC_I     , KC_O     , KC_P            , KC_LBRC ,
    C_S_T(KC_TAB)   , LCTL_T(KC_A) , KC_S    , KC_D     , LT(MOUSE,KC_F) , KC_G     ,                                  KC_H     , KC_J     , KC_K     , KC_L     , RCTL_T(KC_SCLN) , KC_RBRC ,
    LALT_T(KC_MINS) , LSFT_T(KC_Z) , KC_X    , KC_C     , LT(ARROW,KC_V) , KC_B     ,                                  KC_N     , KC_M     , KC_COMM  , KC_DOT   , RSFT_T(KC_SLSH) , RALT_T(KC_BSLS) ,
                                     TO(BASE), KC_LGUI,     LALT_T(KC_BSPC) ,KC_SPC, KC_LCTL,           KC_RCTL, LT(NUM,KC_ENT)       , _______  , _______  , SCRL_TO
  ),

  [NUM] = LAYOUT_universal(
    KC_EQL    ,  S(KC_1)      , S(KC_2)   , S(KC_3)   , S(KC_4)   , S(KC_5)   ,                                       S(KC_6)   , S(KC_7)   , S(KC_8)   , S(KC_9)   , S(KC_0)         , KC_QUOT ,
    S(KC_EQL) ,  LCTL_T(KC_1) , KC_2      , KC_3      , KC_4      , KC_5      ,                                       KC_6      , KC_7      , KC_8      , KC_9      , KC_0            , S(KC_QUOT) ,
    KC_GRAVE  ,  LSFT_T(KC_1) , RCS(KC_2) , RCS(KC_3) , RCS(KC_4) , RCS(KC_5) ,                                       RCS(KC_6) , RCS(KC_7) , RCS(KC_8) , RCS(KC_9) , RSFT_T(KC_BSLS) , KC_BSLS ,
                                _______   , _______             , _______ , _______ , _______ ,                  _______  , _______         , _______   , _______   , TO(MOUSE)
  ),

  [MOUSE] = LAYOUT_universal(
    RGB_TOG  , _______  , KBC_RST , KBC_SAVE , SCRL_DVD , SCRL_DVI ,                                        _______  , _______ , SCRL_TO , _______ , _______ , _______ ,
    RGB_MOD  , RGB_HUI  , RGB_SAI , RGB_VAI  , CPI_D1K  , CPI_I1K  ,                                        _______  , KC_BTN1 , SCRL_MO , KC_BTN2 , _______ , _______ ,
    RGB_RMOD , RGB_HUD  , RGB_SAD , RGB_VAD  , CPI_D100 , CPI_I100 ,                                        _______  , _______ , _______ , _______ , _______ , _______ ,
                          _______ , _______  ,    _______  , _______  , QK_BOOT  ,                   _______  , _______        , _______ , _______ , _______
  ),

  [ARROW] = LAYOUT_universal(
    _______ , _______  , _______ , _______ , _______ , _______ ,                                        KC_HOME    , KC_PGDN    , KC_PGUP  , KC_END      , _______ , _______ ,
    _______ , _______  , _______ , _______ , _______ , _______ ,                                        KC_LEFT    , KC_DOWN    , KC_UP    , KC_RIGHT    , _______ , _______ ,
    _______ , _______  , _______ , _______ , _______ , _______ ,                                        C(KC_LEFT) , C(KC_DOWN) , C(KC_UP) , C(KC_RIGHT) , _______ , _______ ,
                         _______ , _______    , _______ , _______ , _______ ,                   _______  , _______              , _______  , _______     , _______
  ),
};
// clang-format on

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case LSFT_T(KC_1):
      if (record->event.pressed) {
        register_code(KC_LSFT);
      } else {
        unregister_code(KC_LSFT);
        tap_code16(RCS(KC_1));
      }
      return false; // Skip all further processing of this key
    case RSFT_T(KC_BSLS):
      if (record->event.pressed) {
        register_code(KC_RSFT);
      } else {
        unregister_code(KC_RSFT);
        tap_code16(S(KC_BSLS));
      }
      return false; // Skip all further processing of this key
    default:
      return true; // Process all other keycodes normally
  }
}


layer_state_t layer_state_set_user(layer_state_t state) {
    keyball_set_scroll_mode(get_highest_layer(state) == ARROW);
    return state;
}

#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
}
#endif
