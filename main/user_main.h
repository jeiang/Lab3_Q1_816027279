/* gpio example
 *
 * This example code is in the Public Domain.
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 *
 * Brief:
 * This test code shows how to configure gpio and how to use gpio interrupt.
 *
 * GPIO status:
 * GPIO0: output
 * GPIO2: input, pulled up, interrupt from falling edge
 *
 * Test:
 * Connect GPIO0 with GPIO2
 * Generate pulses on GPIO0, that triggers interrupt on GPIO2
 *
 */

#ifndef USER_MAIN_H
#define USER_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_system.h"

#define GPIO_OUTPUT_IO      0
#define GPIO_OUTPUT_PIN_SEL (1ULL << GPIO_OUTPUT_IO)
#define GPIO_INPUT_IO       2
#define GPIO_INPUT_PIN_SEL  (1ULL << GPIO_INPUT_IO)

/**
 * @brief Handle interrupts from GPIO2, sends arg as a uint32_t over an
 * inter-task queue to gpio_task_example
 *
 * @param arg Context pointer passed by xTaskCreate (pvParameters)
 */
static void gpio_isr_handler(void *arg);

/**
 * @brief Whenever an item is received in the queue, print the gpio number, the
 * current input (should be 0), and the number of times called.
 *
 * @param arg Context pointer passed by xTaskCreate (pvParameters)
 */
static void gpio_task_example(void *arg);

/**
 * @brief Initalize GPIO pins, interrupts, tasks and toggles GPIO0
 */
void app_main(void);

#endif
