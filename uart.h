#ifndef __UART_H
#define __UART_H

#include <msp430.h>

#define UART_BUFFER_SIZE 13

#define UART_RX BIT1
#define UART_TX BIT2

void setup_uart();

void uart_send_bytes(char* buffer, char size);

void uart_await_bytes(char size);

char uart_receive_bytes(char* buffer);

char uart_receive_ready();

char uart_tx_received();

char uart_tx_ready();

void uart_rx_received(char rx_data);

void uart_reset();

#endif
