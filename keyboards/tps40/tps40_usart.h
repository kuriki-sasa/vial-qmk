#pragma once

void tps40_usart_driver_start(void);
bool tps40_usart_send(const uint8_t* source, const size_t size);
size_t tps40_usart_receive(uint8_t* destination, const size_t size);
