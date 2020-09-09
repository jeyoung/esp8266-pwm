#include "eagle_soc.h"
#include "ets_sys.h"
#include "gpio.h"
#include "osapi.h"
#include "pwm.h"
#include "user_interface.h"

/*
 * This program pulses an LED by using PWM.
 *
 * It uses the software timer instead of a loop to prevent the watchdog from
 * resetting the processor.
 */

/* A period of 20ms */
#define PWM_PERIOD 20000

/* The maximum duty cycle equation: (PERIOD * 1000/45) */
#define MAX_DUTY_CYCLE 444444

static const int pwm_pin = 4;

static os_timer_t timer;

static volatile int variable_duty_cycle = MAX_DUTY_CYCLE;
static volatile int duty_cycle_step = -10000;

int duty_cycle_out_of_bounds()
{
	return (variable_duty_cycle < 0 || variable_duty_cycle > MAX_DUTY_CYCLE);
}

void reverse_duty_cycle_step()
{
	duty_cycle_step *= -1;
}

void change_duty_cycle()
{
	variable_duty_cycle += duty_cycle_step;

	if (duty_cycle_out_of_bounds())
		reverse_duty_cycle_step();

	if (variable_duty_cycle < 0)
		variable_duty_cycle = 0;

	pwm_set_duty(variable_duty_cycle, 0);
	pwm_start();
}

void ICACHE_FLASH_ATTR timer_func(void *arg)
{
	change_duty_cycle();
}

void ICACHE_FLASH_ATTR user_init(void)
{
	gpio_init();

	uint32 io_info[][3] = {
		{ PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4, 4 }
	};
	uint32 initial_duty_cycle[1] = { MAX_DUTY_CYCLE };
	pwm_init(PWM_PERIOD, initial_duty_cycle, 1, io_info);

	os_timer_disarm(&timer);
	os_timer_setfn(&timer, (os_timer_func_t *)timer_func, (void *)NULL);
	os_timer_arm(&timer, 125, 1);
}
