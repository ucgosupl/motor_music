/*
 * file:	tb6612.c
 * author:	GAndaLF
 * brief:   Driver for TB6612 motor driver.
 */

#include <stdint.h>
#include <errno.h>

#include "stm32f4xx.h"
#include "platform_specific.h"
#include "gpio_f4/gpio_f4.h"

#include "tb6612.h"

/** Motor 1 PWM pin number - PA00. */
#define MOTOR1_PWM_PIN                  0
/** Motor 1 dir 1 pin number - PC02. */
#define MOTOR1_CH1_PIN                  2
/** Motor 1 dir 2 pin number - PC00. */
#define MOTOR1_CH2_PIN                  0

/** Motor 2 PWM pin number - PA01. */
#define MOTOR2_PWM_PIN                  1
/** Motor 2 dir 1 pin number - PC03. */
#define MOTOR2_CH1_PIN                  3
/** Motor 2 dir 2 pin number - PC01. */
#define MOTOR2_CH2_PIN                  1

/** Offset of bitfield CMS MODE in TIM_CR1 register. */
#define TIM_CR1_CMS_MODE_CA3_BIT        5
/** Offset of bitfield OC1M in TIM_CCMR1 register. */
#define TIM_CCMR1_OC1M_BIT              4
/** Offset of bitfield OC2M in TIM_CCMR1 register. */
#define TIM_CCMR1_OC2M_BIT              12

#define PERIOD_MAX_US					1000000

/**
 * Initialize GPIOs for timer and direction pins.
 */
static void
gpio_init(void);

/**
 * Initialize hardware timer to PWM mode.
 */
static void
timer_init(void);

/**
 * Set motor left direction to forward.
 */
static void
tb6612_motor_left_forward(void);

/**
 * Set motor left direction to backward.
 */
static void
tb6612_motor_left_backward(void);

/**
 * Stop motor left.
 */
static void
tb6612_motor_left_stop(void);

/**
 * Set motor right direction to forward.
 */
static void
tb6612_motor_right_forward(void);

/**
 * Set motor right direction to backward.
 */
static void
tb6612_motor_right_backward(void);

/**
 * Stop motor right.
 */
static void
tb6612_motor_right_stop(void);

void
tb6612_init(void)
{
    gpio_init();
    timer_init();
}

int32_t
tb6612_motors_period_set(int32_t period)
{
	if ((period < 0) || (period > PERIOD_MAX_US))
	{
		return -EINVAL;
	}

    TIM2->ARR = period;

    /* Set duty cycle to 12.5%. */
    TIM2->CCR1 = period >> 3;
    TIM2->CCR2 = period >> 3;

    TIM2->CNT = 0;

    if (period == 0)
    {
    	tb6612_motor_left_stop();
    	tb6612_motor_right_stop();
    }
    else
    {
        tb6612_motor_left_forward();
        tb6612_motor_right_forward();
    }

    return 0;
}

static void
gpio_init(void)
{
    /*
     * Motor 1:
     *    * CH1 - PC02
     *    * CH2 - PC00
     *    * PWM - PA00
     *
     * Motor 2:
     *    * CH1 - PC03
     *    * CH2 - PC01
     *    * PWM - PA01
     */

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

    /* PWM pins */
    gpio_mode_config(GPIOA, MOTOR1_PWM_PIN, GPIO_MODE_AF);
    gpio_mode_config(GPIOA, MOTOR2_PWM_PIN, GPIO_MODE_AF);
    gpio_af_config(GPIOA, MOTOR1_PWM_PIN, GPIO_AF_TIM2);
    gpio_af_config(GPIOA, MOTOR2_PWM_PIN, GPIO_AF_TIM2);

    /* GPIO pins */
    gpio_mode_config(GPIOC, MOTOR1_CH1_PIN, GPIO_MODE_OUTPUT);
    gpio_mode_config(GPIOC, MOTOR1_CH2_PIN, GPIO_MODE_OUTPUT);
    gpio_mode_config(GPIOC, MOTOR2_CH1_PIN, GPIO_MODE_OUTPUT);
    gpio_mode_config(GPIOC, MOTOR2_CH2_PIN, GPIO_MODE_OUTPUT);

    /* GPIO initial state - all low */
    GPIOC->ODR &= 0xFFF0;
}

static void
timer_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->CR1 = (3 << TIM_CR1_CMS_MODE_CA3_BIT);    /* Center aligned mode 3 */
    TIM2->CCMR1 = (6 << TIM_CCMR1_OC1M_BIT) |       /* Channel 1 PWM mode 1 */
            (6 << TIM_CCMR1_OC2M_BIT);              /* Channel 2 PWM mode 1 */
    TIM2->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E;     /* Output 1 and 2 enabled */

    /* Single timer tick is 1us, full cycle is 100us */
    TIM2->PSC = APB1_CLOCK_FREQ / 1000000 - 1;
    TIM2->ARR = 100;

    /* Set initial duty cycle */
    TIM2->CCR1 = 0;
    TIM2->CCR2 = 0;

    /* Enable timer */
    TIM2->CR1 |= TIM_CR1_CEN;
}

static void
tb6612_motor_left_forward(void)
{
    GPIOC->BSRRH = (1 << MOTOR1_CH1_PIN);
    GPIOC->BSRRL = (1 << MOTOR1_CH2_PIN);
}

static void
tb6612_motor_left_backward(void)
{
    GPIOC->BSRRL = (1 << MOTOR1_CH1_PIN);
    GPIOC->BSRRH = (1 << MOTOR1_CH2_PIN);
}

static void
tb6612_motor_left_stop(void)
{
    GPIOC->BSRRH = (1 << MOTOR1_CH1_PIN);
    GPIOC->BSRRH = (1 << MOTOR1_CH2_PIN);
}

static void
tb6612_motor_right_forward(void)
{
    GPIOC->BSRRH = (1 << MOTOR2_CH1_PIN);
    GPIOC->BSRRL = (1 << MOTOR2_CH2_PIN);
}

static void
tb6612_motor_right_backward(void)
{
    GPIOC->BSRRL = (1 << MOTOR2_CH1_PIN);
    GPIOC->BSRRH = (1 << MOTOR2_CH2_PIN);
}

static void
tb6612_motor_right_stop(void)
{
    GPIOC->BSRRH = (1 << MOTOR2_CH1_PIN);
    GPIOC->BSRRH = (1 << MOTOR2_CH2_PIN);
}
