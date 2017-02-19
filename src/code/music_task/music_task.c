/*
 * @file	music_task.c
 * @author	GAndaLF
 * @brief	Header for task that plays music on DC motors.
 */

#include "platform_specific.h"
#include "stm32f4xx.h"
#include "tb6612/tb6612.h"
#include "FreeRTOS.h"
#include "task.h"

#include "tone_freqs.h"
#include "music_task.h"

/** Size of music task stack. */
#define MUSIC_TASK_STACKSIZE			(configMINIMAL_STACK_SIZE * 2)
/** Music task priority. */
#define MUSIC_TASK_PRIORITY				(tskIDLE_PRIORITY + 2)

/** Speed in ms of one whole note. */
#define PLAY_SPEED_MS					4000

/** Size of Imperial March note table. */
#define IMPERIAL_MARCH_SIZE	(sizeof(imperial_march)/sizeof(imperial_march[0]))
/** Size of Wsrod nocnej ciszy note table. */
#define WSROD_NOCNEJ_CISZY_SIZE (sizeof(wsrod_nocnej_ciszy)/sizeof(wsrod_nocnej_ciszy[0]))
/** Size of We are the champions note table. */
#define WE_ARE_THE_CHAMPIONS_SIZE (sizeof(we_are_the_champions)/sizeof(we_are_the_champions[0]))

/** Structure describing single note. */
struct note
{
    int32_t tone;		/** Tone frequency. */
    int32_t len;		/** Tone length eg. quarter, half note etc. */
};

/** Notes for Imperial March. */
static const struct note imperial_march[] = {
        {TONE_G4, NOTE_QUARTER},
        {TONE_G4, NOTE_QUARTER},
        {TONE_G4, NOTE_QUARTER},
        {TONE_DIS4, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_EIGHTH},
        {TONE_G4, NOTE_QUARTER},
        {TONE_DIS4, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_EIGHTH},
        {TONE_G4, NOTE_HALF},

        {TONE_D5, NOTE_QUARTER},
        {TONE_D5, NOTE_QUARTER},
        {TONE_D5, NOTE_QUARTER},
        {TONE_DIS5, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_EIGHTH},
        {TONE_FIS4, NOTE_QUARTER},
        {TONE_DIS4, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_EIGHTH},
        {TONE_G4, NOTE_HALF},

        {TONE_G5, NOTE_QUARTER},
        {TONE_G4, NOTE_EIGHTH},
        {TONE_G4, NOTE_EIGHTH},
        {TONE_G5, NOTE_QUARTER},
        {TONE_FIS5, NOTE_EIGHTH},
        {TONE_F5, NOTE_EIGHTH},
        {TONE_E5, NOTE_SIXTEENTH},
        {TONE_DIS5, NOTE_EIGHTH},
        {TONE_E5, NOTE_QUARTER},
        {TONE_GIS4, NOTE_EIGHTH},
        {TONE_CIS5, NOTE_QUARTER},
        {TONE_C5, NOTE_EIGHTH},
        {TONE_H4, NOTE_SIXTEENTH},

        {TONE_AIS4, NOTE_SIXTEENTH},
        {TONE_A4, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_QUARTER},
        {TONE_DIS4, NOTE_EIGHTH},
        {TONE_FIS4, NOTE_QUARTER},
        {TONE_DIS4, NOTE_EIGHTH},
        {TONE_FIS4, NOTE_SIXTEENTH},

        {TONE_AIS4, NOTE_QUARTER},
        {TONE_G4, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_EIGHTH},
        {TONE_D5, NOTE_HALF},

        {TONE_G5, NOTE_QUARTER},
        {TONE_G4, NOTE_EIGHTH},
        {TONE_G4, NOTE_EIGHTH},
        {TONE_G5, NOTE_QUARTER},
        {TONE_FIS5, NOTE_EIGHTH},
        {TONE_F5, NOTE_EIGHTH},
        {TONE_E5, NOTE_SIXTEENTH},
        {TONE_DIS5, NOTE_EIGHTH},
        {TONE_E5, NOTE_QUARTER},
        {TONE_GIS4, NOTE_EIGHTH},
        {TONE_CIS5, NOTE_QUARTER},
        {TONE_C5, NOTE_EIGHTH},
        {TONE_H4, NOTE_SIXTEENTH},

        {TONE_AIS4, NOTE_SIXTEENTH},
        {TONE_A4, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_QUARTER},
        {TONE_DIS4, NOTE_EIGHTH},
        {TONE_FIS4, NOTE_QUARTER},
        {TONE_DIS4, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_SIXTEENTH},

        {TONE_G4, NOTE_QUARTER},
        {TONE_DIS4, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_EIGHTH},
        {TONE_G4, NOTE_HALF},
};

/** Notes for Wsrod nocnej ciszy. */
static const struct note wsrod_nocnej_ciszy[] = {
        {TONE_G4, NOTE_HALF},
        {TONE_A4, NOTE_QUARTER},
        {TONE_FIS4, NOTE_QUARTER},
        {TONE_G4, NOTE_HALF},
        {TONE_D4, NOTE_HALF},
        {TONE_H4, NOTE_QUARTER},
        {TONE_H4, NOTE_QUARTER},
        {TONE_C5, NOTE_QUARTER},
        {TONE_A4, NOTE_QUARTER},
        {TONE_H4, NOTE_FULL},

        {TONE_G4, NOTE_HALF},
        {TONE_A4, NOTE_QUARTER},
        {TONE_FIS4, NOTE_QUARTER},
        {TONE_G4, NOTE_HALF},
        {TONE_D4, NOTE_HALF},
        {TONE_H4, NOTE_QUARTER},
        {TONE_H4, NOTE_QUARTER},
        {TONE_C5, NOTE_QUARTER},
        {TONE_A4, NOTE_QUARTER},
        {TONE_H4, NOTE_FULL},

        {TONE_G4, NOTE_QUARTER},
        {TONE_H4, NOTE_QUARTER},
        {TONE_G4, NOTE_QUARTER},
        {TONE_H4, NOTE_QUARTER},
        {TONE_C5, NOTE_QUARTER},
        {TONE_A4, NOTE_QUARTER},
        {TONE_FIS4, NOTE_QUARTER},
        {TONE_D4, NOTE_QUARTER},

        {TONE_G4, NOTE_QUARTER},
        {TONE_H4, NOTE_QUARTER},
        {TONE_G4, NOTE_QUARTER},
        {TONE_H4, NOTE_QUARTER},
        {TONE_C5, NOTE_QUARTER},
        {TONE_A4, NOTE_QUARTER},
        {TONE_FIS4, NOTE_QUARTER},
        {TONE_D4, NOTE_QUARTER},

        {TONE_G4, NOTE_QUARTER},
        {TONE_G4, NOTE_QUARTER},
        {TONE_A4, NOTE_QUARTER},
        {TONE_A4, NOTE_QUARTER},
        {TONE_H4, NOTE_FULL},

        {TONE_G4, NOTE_QUARTER},
        {TONE_G4, NOTE_QUARTER},
        {TONE_A4, NOTE_QUARTER},
        {TONE_A4, NOTE_QUARTER},
        {TONE_G4, NOTE_FULL},
};

/** Notes for We are the champions. */
static const struct note we_are_the_champions[] = {
        {TONE_PAUSE, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_FIS4, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_EIGHTH},
        {TONE_C5, NOTE_EIGHTH},
        {TONE_C5, NOTE_HALF},
        {TONE_PAUSE, NOTE_QUARTER},

        {TONE_PAUSE, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_FIS4, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_EIGHTH},
        {TONE_C5, NOTE_EIGHTH},
        {TONE_C5, NOTE_HALF},
        {TONE_PAUSE, NOTE_QUARTER},

        {TONE_PAUSE, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_FIS4, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_EIGHTH},
        {TONE_C5, NOTE_EIGHTH},
        {TONE_C5, NOTE_EIGHTH},
        {TONE_C5, NOTE_EIGHTH},
		{TONE_FIS4, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_QUARTER},

        {TONE_PAUSE, NOTE_QUARTER},
        {TONE_FIS4, NOTE_SIXTEENTH},
        {TONE_FIS4, NOTE_SIXTEENTH},
        {TONE_D5, NOTE_SIXTEENTH},
        {TONE_C5, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_SIXTEENTH},
        {TONE_C5, NOTE_EIGHTH},
		{TONE_C5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_QUARTER},
		{TONE_PAUSE, NOTE_QUARTER},

        {TONE_PAUSE, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_EIGHTH},
        {TONE_D5, NOTE_EIGHTH},
        {TONE_DIS5, NOTE_EIGHTH},
        {TONE_DIS5, NOTE_HALF},
        {TONE_PAUSE, NOTE_QUARTER},

        {TONE_PAUSE, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_EIGHTH},
        {TONE_D5, NOTE_SIXTEENTH},
        {TONE_DIS5, NOTE_EIGHTH},
        {TONE_DIS5, NOTE_SIXTEENTH},
		{TONE_DIS5, NOTE_HALF},
        {TONE_PAUSE, NOTE_QUARTER},

        {TONE_PAUSE, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_EIGHTH},
        {TONE_D5, NOTE_EIGHTH},
        {TONE_DIS5, NOTE_EIGHTH},
        {TONE_DIS5, NOTE_SIXTEENTH},
        {TONE_DIS5, NOTE_EIGHTH},
        {TONE_F5, NOTE_EIGHTH},
        {TONE_F5, NOTE_SIXTEENTH},
        {TONE_F5, NOTE_SIXTEENTH},
        {TONE_F5, NOTE_EIGHTH},
        {TONE_G5, NOTE_EIGHTH},
        {TONE_G5, NOTE_SIXTEENTH},

		{TONE_G5, NOTE_QUARTER},
		{TONE_PAUSE, NOTE_SIXTEENTH},
		{TONE_G5, NOTE_SIXTEENTH},
        {TONE_AIS5, NOTE_QUARTER},
        {TONE_A5, NOTE_EIGHTH},
        {TONE_H5, NOTE_EIGHTH},
        {TONE_H5, NOTE_QUARTER},
        {TONE_H5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},

        {TONE_DIS5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_E5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_F5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_G5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},

        {TONE_F5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_F5, NOTE_EIGHTH},
        {TONE_E5, NOTE_EIGHTH},
        {TONE_F5, NOTE_EIGHTH},
        {TONE_E5, NOTE_QUARTER},
        {TONE_C5, NOTE_EIGHTH},
        {TONE_C5, NOTE_EIGHTH},
		{TONE_PAUSE, NOTE_EIGHTH},
        {TONE_A4, NOTE_EIGHTH},

        {TONE_D5, NOTE_QUARTER},
        {TONE_A4, NOTE_EIGHTH},
        {TONE_GIS4, NOTE_EIGHTH},
        {TONE_PAUSE, NOTE_QUARTER},

        {TONE_PAUSE, NOTE_HALF},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_C5, NOTE_EIGHTH},
        {TONE_F5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_F5, NOTE_EIGHTH},
        {TONE_G5, NOTE_EIGHTH},
        {TONE_AIS5, NOTE_EIGHTH},
        {TONE_C6, NOTE_QUARTER},
        {TONE_AIS5, NOTE_EIGHTH},
        {TONE_A5, NOTE_EIGHTH},
        {TONE_D5, NOTE_EIGHTH},
        {TONE_F5, NOTE_EIGHTH},

        {TONE_D5, NOTE_HALF},
        {TONE_PAUSE, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_HALF},
        {TONE_PAUSE, NOTE_QUARTER},

        {TONE_D5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_C5, NOTE_QUARTER},
        {TONE_D5, NOTE_EIGHTH},
        {TONE_C5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_AIS4, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},

        {TONE_H5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_AIS5, NOTE_QUARTER},
        {TONE_H5, NOTE_EIGHTH},
        {TONE_AIS5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_G5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},

        {TONE_AIS5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_F5, NOTE_QUARTER},
        {TONE_H5, NOTE_EIGHTH},
        {TONE_AIS5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_F5, NOTE_EIGHTH},
		{TONE_PAUSE, NOTE_EIGHTH},
        {TONE_H5, NOTE_EIGHTH},

        {TONE_A5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_F5, NOTE_QUARTER},
        {TONE_H5, NOTE_EIGHTH},
        {TONE_A5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},
        {TONE_F5, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_EIGHTH},

        {TONE_PAUSE, NOTE_HALF},
        {TONE_DIS5, NOTE_EIGHTH},
        {TONE_C5, NOTE_SIXTEENTH},
        {TONE_F5, NOTE_SIXTEENTH},
        {TONE_F5, NOTE_FULL},
		{TONE_PAUSE, NOTE_QUARTER},
		{TONE_PAUSE, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_HALF},
		{TONE_PAUSE, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_HALF},
		{TONE_PAUSE, NOTE_QUARTER},
        {TONE_PAUSE, NOTE_HALF},
		{TONE_PAUSE, NOTE_QUARTER},
};

/**
 * Task performing playing music on DC motors.
 *
 * @param params			Task params - unused.
 */
static void
music_task(void *params);

/**
 * Set frequency on DC motors.
 *
 * @param freq				Frequency to be set on motors.
 */
static void
freq_set(int32_t freq);

/**
 * Play music from note table.
 *
 * @param note_table		Note table for melody to be played.
 * @param len				Length of note table.
 */
static void
play(struct note *note_table, int32_t len);

void
music_task_init(void)
{
    rtos_task_create(music_task, "music", MUSIC_TASK_STACKSIZE, MUSIC_TASK_PRIORITY);
}

static void
music_task(void *params)
{
	(void) params;

    tb6612_init();

    while (1)
    {
    	/* Pick your melody here. */
        //play(imperial_march, IMPERIAL_MARCH_SIZE);
        //play(wsrod_nocnej_ciszy, WSROD_NOCNEJ_CISZY_SIZE);
        play(we_are_the_champions, WE_ARE_THE_CHAMPIONS_SIZE);
        rtos_delay(1000);
    }
}

static void
freq_set(int32_t freq)
{
    int32_t val;

    if (freq == 0)
    {
    	val = 0;
    }
    else
    {
        /* Convert frequency to period in us */
        val = 1000000ULL / freq;
    }

    tb6612_motors_period_set(val);
}

static void
play(struct note *note_table, int32_t len)
{
    int32_t i;

    for (i = 0; i < len; i++)
    {
    	/*
    	 * Play single note for a given time.
    	 *
    	 * Between two notes there should be small delay, where no sound is played.
    	 * So 1/32 of time is subtracted from note length for delay.
    	 */
        freq_set(note_table[i].tone);
        rtos_delay(PLAY_SPEED_MS / note_table[i].len - (PLAY_SPEED_MS / 32));
        freq_set(TONE_PAUSE);
        rtos_delay(PLAY_SPEED_MS / 32);
    }
}
