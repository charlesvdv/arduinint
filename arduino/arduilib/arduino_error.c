#include <stdio.h>
#include <stdlib.h>

#include "arduino_logger.h"

void arduino_error_throw(unsigned long time, char* message, int exit_status) {
    arduino_log_error(time, message);
    char* data_dump = arduino_log_dump();
    printf("%s", data_dump);
    free(data_dump);
    exit(exit_status);
}
