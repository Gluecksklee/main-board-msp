#include <msp430.h>				
#include "i2c.h"
#include "uart.h"
#include "timer.h"

/**
 * blink.c
 */

void setup_clock(){
    //BCSCTL3 = LFXT1S_0 + XCAP_3;

    if(CALBC1_16MHZ == 0xFF || CALDCO_16MHZ == 0xFF){
        while(1);
    }

    DCOCTL = 0;
    BCSCTL1 = CALBC1_16MHZ;
    BCSCTL2 |= DIVS_1;
    DCOCTL = CALDCO_16MHZ;
}

void setup_io(){
    P1DIR |= 0x08;
}

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer
	setup_clock();

	setup_io();

	setup_timer();

	setup_i2c();
	setup_uart();

	while (1)
    {
	    __bis_SR_register(CPUOFF + GIE);
    }
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
    if(IFG2 & UCA0TXIFG && uart_tx_ready()){
        UCA0TXBUF = uart_tx_received();
    }
    if(IFG2 & UCB0TXIFG){
        UCB0TXBUF = i2c_read_received();
    }
    if(IFG2 & UCB0RXIFG){
        i2c_write_received(UCB0RXBUF);
    }

}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{
    if(IFG2 & UCA0RXIFG){
        uart_rx_received(UCA0RXBUF);
    }

    if(UCB0STAT & UCSTPIFG){
        i2c_stop_received();
    }
    UCB0STAT &= ~(UCSTPIFG + UCSTTIFG);
}

