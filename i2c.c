#include <msp430.h>
#include "i2c.h"
#include "uart.h"
#include "timer.h"

char command = 0;
char state = 0;
char counter = 0;
unsigned int temp_uint;

void setup_i2c(){
    P1SEL |= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
    P1SEL2|= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0

    UCB0CTL1 |= UCSWRST;           // Enable SW reset
    UCB0CTL0 = UCMODE_3 + UCSYNC;  // I2C Slave, synchronous mode
    UCB0I2COA = 0x24;              // Own Address is 024h
    UCB0I2CIE |= UCSTPIE;
    UCB0CTL1 &= ~UCSWRST;          // Clear SW reset, resume operation
    IE2 |= UCB0RXIE | UCB0TXIE;    // Enable TX interrupt
}

inline void i2c_stop_received(){
    counter = 0;
}

inline void i2c_write_received(char rx_data){
    switch (state){
       case 0:
           // IDLE: select follow-on state
           if(rx_data == I2C_COM_STATUS){
               state = 5;
           }else if(rx_data == I2C_COM_WRITE_TO_BUFFER){
               state = 1;
               counter = 0;
           }else if(rx_data == I2C_COM_READ_FROM_BUFFER){
               uart_receive_bytes(i2c_tx_buffer);
               state = 2;
               counter = 0;
           }else if(rx_data == I2C_COM_SEND_TO_UART){
               state = 3;
           }else if(rx_data == I2C_COM_RECEIVE_FROM_UART){
               state = 4;
           }else if(rx_data == I2C_COM_FAN_TACHO){
               state = 6;
               temp_uint = timer_get_fan_tacho();
           }else if(rx_data == I2C_COM_FAN_PWM){
               state = 8;
               command = I2C_COM_FAN_PWM;
           }else if(rx_data == I2C_COM_LED_ENABLE){
               state = 8;
               command = I2C_COM_LED_ENABLE;
           }else if(rx_data == I2C_COM_LED_PWM){
               state = 8;
               command = I2C_COM_LED_PWM;
           }else if(rx_data == I2C_COM_RESET_UART){
               state = 0;
               uart_reset();
               counter = 0;
           }else if(rx_data == I2C_COM_DEBUG_LED_SET){
               state = 8;
               command = I2C_COM_DEBUG_LED_SET;
           }else if(rx_data == I2C_COM_DEBUG_LED_GET){
               state = 6;
               temp_uint = (P1OUT & 0x08) >> 3;
           }else if(rx_data == I2C_COM_DEBUG_LED_TOGGLE){
               state = 0;
               P1OUT ^= 0x08;
           }else if(rx_data == I2C_COM_REBOOT_CONTROL){
               state = 8;
               command = I2C_COM_REBOOT_CONTROL;
           }else if(rx_data == I2C_COM_REBOOT_STATUS){
               state = 6;
               temp_uint = timer_get_reboot_status();
           }else if(rx_data == I2C_COM_REBOOT_GET_COUNTER){
               state = 6;
               temp_uint = timer_get_reboot_counter();
           }else if(rx_data == I2C_COM_VERSION){
               state = 6;
               temp_uint = VERSION;
           }else{
               state = 0;
           }
           break;
       case 1:
           // WRITE_TO_BUFFER: write to i2c buffer
           i2c_rx_buffer[counter] = rx_data;
           ++counter;
           if(counter == UART_BUFFER_SIZE){
               state = 0;
           }
           break;
       case 2:
           // READ_FROM_BUFFER: read from i2c buffer
           break;
       case 3:
           // SEND_TO_UART: number of bytes to send to uart
           uart_send_bytes(i2c_rx_buffer, rx_data);
           state = 0;
           break;
       case 4:
           // RECEIVE_FROM_UART: number of bits to store in buffer
           uart_await_bytes(rx_data);
           state = 0;
           break;
       case 5:
           // STATUS
           break;
       case 6:
           // SEND uint Low
           break;
       case 7:
           // SEND uint High
           break;
       case 8:
           // WRITE uint low
           temp_uint = rx_data;
           state = 9;
           break;
       case 9:
           // WRITE uint high
           temp_uint |= rx_data << 8;

           if(command == I2C_COM_FAN_PWM){
               //timer_set_fan_pwm(temp_uint);
           }else if(command == I2C_COM_LED_ENABLE){
               //timer_set_led_enable(temp_uint);
           }else if(command == I2C_COM_LED_PWM){
               //timer_set_led_pwm(temp_uint);
           }else if(command == I2C_COM_REBOOT_CONTROL){
               timer_set_reboot_control(temp_uint & 0xFF);
           }else if(command == I2C_COM_DEBUG_LED_SET){
               if(temp_uint){
                   P1OUT |= 0x08;
               }else{
                   P1OUT &= ~0x08;
               }
           }
           state = 0;
           break;
    }
}

inline char i2c_read_received(){
    char tx_data = 0;
    if(state == 2){
        tx_data = i2c_tx_buffer[counter];
        ++counter;
        if(counter == UART_BUFFER_SIZE){
            state = 0;
        }
    }else if(state == 5){
        if(uart_receive_ready()){
            tx_data |= 0x01;
        }
        state = 0;
    }else if(state == 6){
        tx_data = temp_uint & 0xFF;
        state = 7;
    }else if(state == 7){
        tx_data = (temp_uint >> 8) & 0xFF;
        state = 0;
    }
    return tx_data;
}
