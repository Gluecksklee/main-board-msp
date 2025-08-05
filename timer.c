#include "timer.h"
#include <msp430.h>

void setup_timer(){

    P2DIR |= TIMER_PWM_LED;
    P2SEL |= TIMER_PWM_LED;
    P2SEL2 &= ~TIMER_PWM_LED;

    P2DIR |= TIMER_FAN_PWM;
    P2OUT |= TIMER_FAN_PWM;

    P3DIR &= ~TIMER_FAN_TACHO;
    P3SEL |= TIMER_FAN_TACHO;
    P3SEL2 &= ~TIMER_FAN_TACHO;

    // Fan Tacho Timer
    // smlck clock, count up to 0xFFFF, clear timer => 1MHz
    TA0CTL = TASSEL_2 | MC_1 | TACLR | ID_3;
    TA0CCR0 = 0xFFFF;
    TA0CCTL0 = CCIE;
    TA0CCTL2 = CAP | CM_2 | CCIS_0 | SCS | CCIE;

    // smclk, count up to TA0CCR0, clear timer, enable interrupt
    //TA1CTL = TASSEL_2 | MC_1 | TACLR;
    // count up to value and reset timer
    //TA1CCR0 = 0x400;
    // set output when reached, reset when overflow
    //TA1CCTL1 = OUTMOD_6;
    // set initial pwm value
    //TA1CCR1 = 0x200;

    P1DIR &= ~HEARTBEAT;
    P1IES &= ~HEARTBEAT;
    P1IE |= HEARTBEAT;

    P1DIR |= REBOOT;
    P1OUT &= ~ REBOOT;

    reboot_state = REBOOT_STATE_INIT;
    reboot_cycles = 0;
    reboot_control = 1;
}


int timer_get_fan_tacho(){
    if(timer_fan_cycles == TIMER_RESET_CYCLES){
        return 0xFFFF;
    }
    return timer_fan_tacho - timer_fan_tacho_last;
}

void timer_set_fan_pwm(int value){
    // TODO: implement
}

void timer_set_led_enable(int value){
    // TODO: implement
}

void timer_set_led_pwm(int value){
    TA1CCR1 = value;
}

inline unsigned int timer_get_reboot_status(){
    return (reboot_state | (reboot_control << 7)) & 0xFF;
}

/**
 * Bits:
 * 7 : active
 * 6
 * 5
 * 4
 * 3
 * 2 : overwrite state
 * 1 : state bit 1
 * 0 : state bit 0
 */
inline void timer_set_reboot_control(unsigned int value){
    reboot_control = (value >> 7) & 1;
    if(reboot_state != (value & 3) && reboot_state & 4){
        reboot_state = value & 3;
    }
}

inline unsigned int timer_get_reboot_counter(){
    return reboot_cycles;
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
    if(P1IFG |= HEARTBEAT){
        cycles_since_last_heartbeat_last = cycles_since_last_heartbeat;
        cycles_since_last_heartbeat = 0;
        P1IFG &= ~ HEARTBEAT;
    }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    cycles_since_last_heartbeat += 1;
    if(timer_fan_cycles < TIMER_RESET_CYCLES){
        timer_fan_cycles += 1;
    }

    if(reboot_control == 0){
        reboot_state = REBOOT_STATE_INIT;
    }

    if(reboot_state == REBOOT_STATE_INIT){
        reboot_cycles += 1;
        if(reboot_cycles >= REBOOT_TIMEOUT_INITIAL){
            reboot_cycles = 0;
            reboot_state = REBOOT_STATE_WAIT;
        }
    }else if(reboot_state == REBOOT_STATE_WAIT){
        if(cycles_since_last_heartbeat >= REBOOT_ACTIVATION_DELAY){
            reboot_state = REBOOT_STATE_REBOOT;
        }
    }else if(reboot_state == REBOOT_STATE_REBOOT){
        P1OUT |= REBOOT;
        reboot_cycles += 1;
        if (reboot_cycles >= REBOOT_REBOOT){
            reboot_cycles = 0;
            reboot_state = REBOOT_STATE_TIMEOUT;
            P1OUT &= ~REBOOT;
        }
    }else if (reboot_state == REBOOT_STATE_TIMEOUT){
        reboot_cycles += 1;
        if(reboot_cycles >= REBOOT_TIMEOUT){
            reboot_cycles = 0;
            reboot_state = REBOOT_STATE_WAIT;
        }
    }else{
        reboot_state = REBOOT_STATE_INIT;
    }
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    if(TA0CCTL2 & CCIFG){
        timer_fan_tacho_last = timer_fan_tacho;
        timer_fan_tacho = TA0CCR2;
        timer_fan_cycles = 0;
        TA0CCTL2 &= ~ CCIFG;
    }
}
