/*
 * File: main.c
 * Author: GAndaLF
 * Brief: Main file for main_program target.
 */

#include "platform_specific.h"
#include "FreeRTOS.h"
#include "task.h"

#include "core_init/core_init.h"
#include "music_task/music_task.h"

int
main(void)
{
    core_init();

    /* Place your initialisation code here. */
    music_task_init();

    /* Place your application code here. */
    vTaskStartScheduler();

    while (1)
        ;
}
