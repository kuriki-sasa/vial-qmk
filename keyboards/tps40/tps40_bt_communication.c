#include "tps40_bt_communication.h"

#include <string.h>
#include <stdio.h>

#include "tps40_usart.h"
#include "print.h"

static uint8_t received_command_buffer[512] = { 0 };
static int last_index = 0;

size_t read_from_uart(void) {
    size_t read_size = tps40_usart_receive(&received_command_buffer[last_index], 64);
    last_index += read_size;
    if (last_index != strlen((char*)received_command_buffer)) {
        for (int find_index = 0; find_index < last_index; find_index++) {
            if (received_command_buffer[find_index] != '\0') {
                int new_length = last_index - find_index;
                memmove(received_command_buffer, &received_command_buffer[find_index], new_length);
                memset(&received_command_buffer[new_length], '\0', last_index - new_length);
                read_size -= find_index;
                last_index = new_length;
                break;
            }
        }
    }
    return read_size;
}

bool read_command_from_buffer(uint8_t* dest) {
    int received_command_bufferLength = strlen((char*)received_command_buffer);
    for (int find_index = 0; find_index < received_command_bufferLength; find_index++) {
        dest[find_index] = received_command_buffer[find_index];
        if (received_command_buffer[find_index] == '\n') {
            int new_head_index = find_index + 1;
            int new_length = received_command_bufferLength - new_head_index;
            memmove(received_command_buffer, &received_command_buffer[new_head_index], new_length);
            memset(&received_command_buffer[new_length], '\0', received_command_bufferLength - new_length);
            last_index = new_length;
            dest[find_index + 1] = '\0';
            return true;
        }
    }
    return false;
}

void start_communication(void) {
    tps40_usart_driver_start();
}

bool send_text(const uint8_t* command) {
    size_t length = strlen((char*)command);
    return send_buffer(command, length);
}

bool send_buffer(const uint8_t* command, const uint8_t length) {
    tps40_usart_send(command, length);
    //uprintf("send: %s", command);
    return true;
}

bool is_expected_text(const uint8_t* received, const uint8_t* text) {
    return strcasecmp((char*)received, (char*)text) == 0;
}

void print_buffer(void) {
    print("buffer: ");
    for (int i = 0; i < last_index; i++) {
        uprintf("%02x ", received_command_buffer[i]);
    }
    print("\n");
}

bool send_wakeup(void) {
    return tps40_wakeup_frame_send();
}