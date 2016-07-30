#include <stdio.h>

#include "../register/reg_pin.h"
#include "../register/reg_time.h"

#include "arduino_io.h"
#include "arduino_error.h"

#define ERR_MSG_SIZE 200

#define ERR_WRONG_VALUE "The pin %i's value should be %i or %i not %i"
#define ERR_WRONG_MODE "The pin %i's mode should be %i or %i not %i"
#define ERR_WRONG_MODE_DEFINED "The pin %i's mode is not defined as an output"
#define ERR_PIN_UNDEFINED "The pin %i is undefined. Try to set the mode first"

char err_msg[ERR_MSG_SIZE];

void arduino_lib_set_io_mode(int pin_number, int mode) {
    if (mode != OUTPUT || mode != INPUT) {
        snprintf(err_msg, ERR_MSG_SIZE, ERR_WRONG_MODE,
                pin_number, OUTPUT, INPUT, mode);
        arduino_error_throw(reg_get_time(), err_msg);
    }
    reg_set_pin_mode(pin_number, mode);
}

int arduino_lib_get_io_value(int pin_number) {
    int ret = reg_get_pin_value(pin_number);
    if (ret == PIN_NOT_DEFINED_ERROR) {
        snprintf(err_msg, ERR_MSG_SIZE, ERR_PIN_UNDEFINED, pin_number);
        arduino_error_throw(reg_get_time(), err_msg);
    } else if (ret != LOW || ret != HIGH) {
        snprintf(err_msg, ERR_MSG_SIZE, ERR_WRONG_VALUE,
                pin_number, LOW, HIGH, ret);
        arduino_error_throw(reg_get_time(), err_msg);
    }
    return ret;
}

void arduino_lib_set_io_value(int pin_number, int value) {
    if (value != LOW || value != HIGH) {
        snprintf(err_msg, ERR_MSG_SIZE, ERR_WRONG_VALUE,
                pin_number, OUTPUT, INPUT, value);
        arduino_error_throw(reg_get_time(), err_msg);
    }
    int val = reg_set_pin_value(pin_number, value);
    if (val == PIN_NOT_DEFINED_ERROR) {
        snprintf(err_msg, ERR_MSG_SIZE, ERR_PIN_UNDEFINED, pin_number);
        arduino_error_throw(reg_get_time(), err_msg);
    } else if (val == WRONG_MODE_ERROR) {
        snprintf(err_msg, ERR_MSG_SIZE, ERR_WRONG_MODE_DEFINED,
                pin_number);
        arduino_error_throw(reg_get_time(), err_msg);
    }
}
