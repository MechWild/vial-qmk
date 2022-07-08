// Copyright 2022 Kyle McCreery
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "keymap_steno.h"

// Defines names for use in layer keycodes and the keymap

void keyboard_post_init_user(void) {
  // Customise these values to desired behaviour
  //debug_enable=true;
  //debug_matrix=true;
  //debug_keyboard=true;
  //debug_mouse=true;
}

void eeconfig_init_user() {
    steno_set_mode(STENO_MODE_GEMINI); // or STENO_MODE_BOLT
}

enum layer_names {
    _QWERTY,
	_LOWER,
	_RAISE,
    _ADJUST,
    _STENO
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
                                                                                 
  [_QWERTY] = LAYOUT(                                                            
    KC_GESC, KC_Q, KC_W,    KC_E,   KC_R, KC_T,                      KC_NLCK, KC_Y, KC_U, KC_I,    KC_O,    KC_P,    KC_BSPC, 
    KC_TAB,  KC_A, KC_S,    KC_D,   KC_F, KC_G,             KC_MUTE, KC_CLCK, KC_H, KC_J, KC_K,    KC_L,    KC_SCLN, KC_ENT,
    KC_LSFT, KC_Z, KC_X,    KC_C,   KC_V, KC_B,                      KC_SLCK, KC_N, KC_M, KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
    KC_MUTE, KC_LCTL, KC_LALT, LT(_LOWER, KC_SPC), KC_BTN1, TG(_STENO), KC_BTN2, LT(_RAISE, KC_SPC), KC_SPC,  KC_RGUI,  KC_MUTE
  ),

  [_LOWER] = LAYOUT(                                                 
    KC_1,        KC_2,    KC_3,    KC_4,    KC_5,    KC_6,         KC_TRNS,  KC_7,     KC_8,    KC_9,    KC_0,    KC_MINS,   KC_EQL,         
    KC_TRNS,     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS, KC_LBRC, KC_RBRC, KC_BSLS,   KC_QUOT,
    KC_TRNS,     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS,  KC_DEL,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT,   KC_TRNS,
                       KC_TRNS,            KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS,KC_TRNS,KC_TRNS,     KC_TRNS,  KC_TRNS, KC_TRNS,              KC_TRNS            
  ),                                                                                                                 
                                                                                                                     
  [_RAISE] = LAYOUT(                                                                                                 
    KC_EXLM,     KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC,       KC_TRNS, KC_AMPR,  KC_ASTR, KC_LPRN, KC_RPRN, KC_UNDS,   KC_PLUS,           
    KC_TRNS,     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS,   KC_TRNS, KC_TRNS,  KC_TRNS, KC_LCBR, KC_RCBR, KC_PIPE,   KC_DQUO,
    KC_TRNS,     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,       KC_TRNS, KC_TAB,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS,
                       KC_TRNS,            KC_TRNS, TG(_QWERTY), KC_TRNS,   KC_TRNS,KC_TRNS,KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS,          KC_TRNS       
  ),                                                                                                                 
                                                                                                                     
  [_ADJUST] = LAYOUT(                                                                                                
    KC_F1,       KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,        KC_TRNS,  KC_F7,    KC_F8,   KC_F9,   KC_F10,  KC_F11,    KC_F12,         
    KC_TRNS,     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS,  KC_TRNS,  KC_TRNS,  KC_TRNS, KC_TRNS, RGB_VAD, RGB_RMOD,  RGB_TOG,
    RESET,       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS,  KC_TRNS,  KC_TRNS, KC_TRNS, RGB_VAI, RGB_MOD,   KC_TRNS,
                         KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS,KC_TRNS,KC_TRNS,    KC_TRNS,  KC_TRNS, KC_TRNS,      KC_TRNS
  ),
  [_STENO] = LAYOUT(
    KC_ESC, STN_N2, STN_N3,    STN_N4,   STN_N5, STN_N6,                      KC_NLCK, STN_N7, STN_N8, STN_N9, STN_NA,    STN_NB,    STN_NC,
    KC_TRNS, STN_S1, STN_TL,    STN_PL,   STN_HL, STN_ST1,              KC_MUTE, KC_CLCK, STN_ST3, STN_FR, STN_PR, STN_LR, STN_TR, STN_DR, 
    KC_TRNS, STN_S2, STN_KL,    STN_WL,   STN_RL, STN_ST2,                        KC_SLCK, STN_ST4, STN_RR, STN_BR, STN_GR, STN_SR, STN_ZR,
    KC_TRNS, KC_TRNS, STN_A, STN_O, KC_TRNS, TG(_STENO), KC_TRNS, STN_E, STN_U,  KC_TRNS, KC_TRNS
  )

};
  
layer_state_t layer_state_set_user(layer_state_t state) {
  return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}


