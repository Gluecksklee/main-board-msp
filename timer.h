#ifndef __TIMER_H
#define __TIMER_H

// PWM LED 2.1
#define TIMER_PWM_LED BIT1
// FAN TACHO 3.0
#define TIMER_FAN_TACHO BIT0
// FAN PWM 2.4
#define TIMER_FAN_PWM BIT4

#define HEARTBEAT BIT4
#define REBOOT BIT5

#define TIMER_RESET_CYCLES 3

#define TICKS_PER_SECOND 15
#define REBOOT_TIMEOUT_INITIAL TICKS_PER_SECOND * 60 * 30
#define REBOOT_ACTIVATION_DELAY TICKS_PER_SECOND * 60
#define REBOOT_REBOOT TICKS_PER_SECOND
#define REBOOT_TIMEOUT TICKS_PER_SECOND * 60 * 30

#if REBOOT_TIMEOUT_INITIAL > 65536
#error REBOOT_TIMEOUT_INITIAL needs to be smaller than 65536
#endif

#if REBOOT_ACTIVATION_DELAY > 65536
#error REBOOT_ACTIVATION_DELAY needs to be smaller than 65536
#endif

#if REBOOT_TIMEOUT > 65536
#error REBOOT_TIMEOUT needs to be smaller than 65536
#endif

#define REBOOT_STATE_INIT 0
#define REBOOT_STATE_WAIT 1
#define REBOOT_STATE_REBOOT 2
#define REBOOT_STATE_TIMEOUT 3

unsigned int reboot_control;

unsigned char reboot_state;
unsigned int reboot_cycles;

unsigned int cycles_since_last_heartbeat;
unsigned int cycles_since_last_heartbeat_last;

unsigned int timer_fan_tacho_last;
unsigned int timer_fan_tacho;
unsigned char timer_fan_cycles;

void setup_timer();

int timer_get_fan_tacho();

void timer_set_fan_pwm(int value);

void timer_set_led_enable(int value);

void timer_set_led_pwm(int value);

unsigned int timer_get_reboot_status();

void timer_set_reboot_control(unsigned int value);

unsigned int timer_get_reboot_counter();

#endif
