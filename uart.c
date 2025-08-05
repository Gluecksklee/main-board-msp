#include <msp430.h>
#include "uart.h"
#include <string.h>

char uart_rx_buffer[UART_BUFFER_SIZE];
char uart_tx_buffer[UART_BUFFER_SIZE];

char uart_rx_index;
char uart_tx_index;

char uart_rx_size;
char uart_tx_size;

void setup_uart(){
    P1OUT |= UART_RX | UART_TX;
    P1OUT &= ~(UART_RX | UART_TX);

    P1SEL |= UART_RX | UART_TX;
    P1SEL2 |= UART_RX | UART_TX;

    UCA0CTL1 |= UCSSEL_2;                     // CLK = SMLCK
    UCA0BR0 = 65;
    UCA0BR1 = 3;
    UCA0MCTL = UCBRS_2;                       // Modulation UCBRSx = 2
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

inline void uart_send_bytes(char* buffer, char size){
    if(size > UART_BUFFER_SIZE || size == 0){
        return;
    }

    uart_tx_index = 0;
    uart_tx_size = size;
    memset(uart_tx_buffer, 0, UART_BUFFER_SIZE);
    memcpy(uart_tx_buffer, buffer, uart_tx_size);

    UCA0TXBUF = uart_tx_buffer[uart_tx_index++];

    if(uart_tx_index == uart_tx_size){
        // transmission completed, reset counter
        uart_tx_index = 0;
        uart_tx_size = 0;
    }else{
        // transmission not completed yet, enable transmission interrupt
        IE2 |= UCA0TXIE;
    }
}

inline void uart_await_bytes(char size){
    memset(uart_rx_buffer, 0, UART_BUFFER_SIZE);
    uart_rx_index = 0;
    uart_rx_size = size;
}

inline char uart_receive_bytes(char* buffer){
    if(uart_rx_index == uart_rx_size){
        memcpy(buffer, uart_rx_buffer, uart_rx_size);
        return 1;
    }
    return 0;
}

inline char uart_receive_ready(){
    return uart_rx_index == uart_rx_size;
}

inline char uart_tx_received(){
    // transmit next byte
    const char tx_data = uart_tx_buffer[uart_tx_index++];

    if(uart_tx_index == uart_tx_size){
        // transmission completed, reset counter and disable interrupt
        uart_tx_index = 0;
        uart_tx_size = 0;
        IE2 &= ~UCA0TXIE;
    }
    return tx_data;
}

inline char uart_tx_ready(){
    return uart_tx_index != uart_tx_size && uart_tx_size != 0;
}

inline void uart_rx_received(char rx_data){
    if (uart_rx_index != uart_rx_size)
    {
        // receive next byte
        uart_rx_buffer[uart_rx_index++] = rx_data;
    }
}

inline void uart_reset(){
    uart_rx_index = 0;
    uart_tx_index = 0;
    uart_rx_size = 0;
    uart_tx_size = 0;
}
