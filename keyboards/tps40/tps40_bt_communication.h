#pragma once

#include "tps40_bt_constants.h"

#define COMMAND_STR(str) (uint8_t*)str "\r\n"
#define TO_STR(num) #num
#define TO_STR_HELPER(num) TO_STR(num)
#define RUN_COMMAND(command) COMMAND_STR(AT_PREFIX command)
#define WRITE_COMMAND(command, value) COMMAND_STR(AT_PREFIX command "=" value)
#define READ_COMMAND(command) COMMAND_STR(AT_PREFIX command "?")
#define send_run_command(command) send_text(RUN_COMMAND(command))
#define send_write_command(command, value) send_text(WRITE_COMMAND(command, value))
#define send_read_command(command) send_text(READ_COMMAND(command))

#define is_expected_command(received, expected) is_expected_text(received, COMMAND_STR(expected))
#define is_expected_notification(received, expected, value) is_expected_text(received, COMMAND_STR(expected ":" value))
#define is_success_response(received) is_expected_text(received, COMMAND_STR(COMMAND_OK))

void start_communication(void);
bool send_text(const uint8_t* command);
bool send_buffer(const uint8_t* command, const uint8_t length);
bool is_expected_text(const uint8_t* received, const uint8_t* text);
void print_buffer(void);
size_t read_from_uart(void);
bool read_command_from_buffer(uint8_t* dest);
