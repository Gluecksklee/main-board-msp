#ifndef __I2C_H
#define __I2C_H

#include <msp430.h>

#define VERSION 22121

#define I2C_BUFFER_SIZE 13

#define I2C_COM_STATUS 0x00
#define I2C_COM_WRITE_TO_BUFFER 0x01
#define I2C_COM_READ_FROM_BUFFER 0x02
#define I2C_COM_SEND_TO_UART 0x03
#define I2C_COM_RECEIVE_FROM_UART 0x04
#define I2C_COM_FAN_TACHO 0x05
#define I2C_COM_FAN_PWM 0x06
#define I2C_COM_LED_ENABLE 0x07
#define I2C_COM_LED_PWM 0x08
#define I2C_COM_LED_DEBUG 0x09
#define I2C_COM_RESET_UART 0x0A

#define I2C_COM_DEBUG_LED_SET 0x10
#define I2C_COM_DEBUG_LED_GET 0x11
#define I2C_COM_DEBUG_LED_TOGGLE 0x12

#define I2C_COM_REBOOT_STATUS 0x20
#define I2C_COM_REBOOT_CONTROL 0x21
#define I2C_COM_REBOOT_GET_COUNTER 0x22

#define I2C_COM_VERSION 0xFF

char i2c_rx_buffer[I2C_BUFFER_SIZE];
char i2c_tx_buffer[I2C_BUFFER_SIZE];

void setup_i2c();

void i2c_stop_received();

void i2c_write_received(char rx_data);

char i2c_read_received();

#endif
