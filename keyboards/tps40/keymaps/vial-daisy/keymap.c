// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "tps40_led.h"
#include "tps40_battery_monitoring.h"

#define _QWERTY 0
#define _LWR 1
#define _RSE 2
#define _ADJ 3
#define TAB_CRL LCTL_T(KC_TAB)
#define LWR_SPC LT(_LWR, KC_SPC)
#define RSE_SPC LT(_RSE, KC_SPC)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY] = LAYOUT(
        KC_MUTE,
        KC_ESC,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
        TAB_CRL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENT ,
        KC_CAPS, KC_LCTL, KC_LALT, KC_LGUI, LWR_SPC,          RSE_SPC,          KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT,
        KC_PGUP, KC_PGDN
    ),
    [_LWR] = LAYOUT(
        KC_MPLY,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        KC_CAPS, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______,          _______,          _______, _______, _______, _______,
        _______, _______
    ),
    [_RSE] = LAYOUT(
        _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______,          _______,          _______, _______, _______, _______,
        _______, _______
    ),
    [_ADJ] = LAYOUT(
        XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,          XXXXXXX,          XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, XXXXXXX
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [_QWERTY] =   {
        ENCODER_CCW_CW(KC_VOLD, KC_VOLU)
    },
    [_LWR] =   {
        ENCODER_CCW_CW(KC_MPRV, KC_MNXT)
    },
    [_RSE] =   {
        ENCODER_CCW_CW(_______, _______)
    },
    [_ADJ] =   {
        ENCODER_CCW_CW(_______, _______)
    }
};
#endif

layer_state_t layer_state_set_user(layer_state_t state) {
    layer_state_t computed = update_tri_layer_state(state, _LWR, _RSE, _ADJ);
    switch (get_highest_layer(computed)) {
        case _LWR:
            set_led_state(1, true);
            set_led_state(2, false);
            break;
        case _RSE:
            set_led_state(1, false);
            set_led_state(2, true);
            break;
        case _ADJ:
            set_led_state(1, true);
            set_led_state(2, true);
            break;
        default:
            set_led_state(1, false);
            set_led_state(2, false);
            break;
    }
    return computed;
}

bool led_update_user(led_t led_state) {
    //set_led_state(0, led_state.caps_lock);
    return false;
}

void battery_state_updated(enum BatteryState state) {
    uprintf("battery state: %d\n", state);
    switch (state) {
        case LEVEL_LOW:
        case LEVEL_MID:
        case LEVEL_HIGH:
            set_led_state(0, false);
            break;
        case CHARGING:
            set_led_state(0, true);
            break;
    }
}