#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "microclib.h"
#include "arduino.h"

#include "int_arduino_code.h"
#include "int_jsonify_log.h"

static int max_time = 20000;

void int_parse_io_event_cli(char *arg) {
    char *str;
    char *token;
    int event[3];

    str = malloc(strlen(arg) + 1);
    strncpy(str, arg, strlen(arg));

    for(int i = 0; i < 3; str = NULL, i++) {
        token = strtok(str, ":");
        event[i] = atoi(token);
        if (token == NULL) {
            fprintf(stderr, "IO event config parsing error.");
            exit(1);
        }
    }
    free(str);
    if(mc_add_io_event(event[0], event[1], event[2]) != MC_SUCCESS) {
        fprintf(stderr, "IO event config parsing error.");
        exit(1);
    }
}

void int_handle_config(int argc, char *argv[]) {
    int c;
    while ((c = getopt(argc, argv, "e:i:r:")) != -1) {
        switch (c) {
            case 'e':
                // External event IO.
                int_parse_io_event_cli(optarg);
                break;
            case 'r':
                // Max time running.
                if ((max_time = atoi(optarg)) == 0 || max_time <= 0) {
                    fprintf(stderr, "Max time config error.");
                    exit(1);
                }
                break;
            case 'i':
                // Increment time.
                ;
                int incr;
                if ((incr = atoi(optarg)) == 0 || incr <= 0) {
                    fprintf(stderr, "Incrementation time config error.");
                    exit(1);
                }
                if (mc_set_increment_time(incr) != MC_SUCCESS) {
                    fprintf(stderr, "Incrementation time config error.");
                    exit(1);
                }
                break;
            default:
                fprintf(stderr, "Unable to parse correctly the config.");
                exit(1);
        }
    }
}

void int_loop_update(int time) {
    // Handle digital IO event.
    int prev_time = mc_calculate_previous_time(time);
    int size = mc_get_io_events_size(prev_time, time);
    mc_io_event_data io_events[size];
    mc_get_io_events(prev_time, time, io_events, size);
    for (int i = 0; i < size; i++) {
        mc_force_digital_io_value(io_events[i].pin, io_events[i].value);
    }

    mc_save_digital_io_state(time);
}

void int_exit(int status) {
    // Transform the IO states in JSON
    size_t io_logs_size = mc_get_digital_io_history_size();
    mc_io_log io_logs[io_logs_size];
    mc_get_digital_io_history(io_logs, io_logs_size);

    int_jsonifier_io_states(io_logs, io_logs_size);
    mc_free_digital_io_history();

    // Display log before exiting.
    char *s = int_get_log();
    printf("%s\n", s);
    int_free_log(s);

    exit(status);
}

void int_error(int time, char *msg) {
    int_log_error(time, msg);
    int_exit(EXIT_FAILURE);
}

void int_init() {
    int_jsonifier_init();
    ardno_register_delay_callback(int_loop_update);
    ardno_register_exit_failure_callback(int_error);
}

int main(int argc, char *argv[]) {
    int time = 0;

    int_handle_config(argc, argv);
    int_init();
    setup();
    int_loop_update(mc_get_time());
    while ((time = mc_get_time()) <= max_time) {
        loop();
        int_loop_update(mc_get_time());

        // Only increment time if we don't have delai in the arduino code.
        if (time == mc_calculate_previous_time(time)) {
            mc_increment_time();
        }
    }
    int_exit(EXIT_SUCCESS);
}