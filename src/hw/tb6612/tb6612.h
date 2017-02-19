/*
 * file:	tb6612.h
 * author:	GAndaLF
 * brief:	Driver for TB6612 motor driver.
 */

#ifndef _TB6612_H_
#define _TB6612_H_

/**
 * Initialize TB6612 motor driver.
 */
void
tb6612_init(void);

/**
 * Set given PWM period on both motors.
 *
 * Function sets period on both motors with duty cycle 12.5%.
 *
 * @param period
 * @return
 */
int32_t
tb6612_motors_period_set(int32_t period);

#endif /* _TB6612_H_ */
