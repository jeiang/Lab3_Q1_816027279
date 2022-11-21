/* gpio example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "user_main.h"
#include "rom/uart.h"

static const char  *TAG            = "main";
static const char  *TEST_TAG       = "test";
static xQueueHandle gpio_evt_queue = NULL;

static void
gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void
gpio_task_example(void *arg)
{
    uint32_t        io_num;
    static uint32_t counter = 0;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            counter++;
            ESP_LOGI(TAG,
                     "GPIO[%d] intr, val: %d, times called: %d\n",
                     io_num,
                     gpio_get_level(io_num),
                     counter);
        }
    }
}

// Launches and runs the app as normal
static void
app_run(void *arg)
{
    gpio_config_t io_conf;
    // disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    // bit mask to select GPIO0
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    // disable pull-down mode
    io_conf.pull_down_en = 0;
    // disable pull-up mode
    io_conf.pull_up_en = 0;
    // configure GPIO0 with the given settings
    gpio_config(&io_conf);

    // interrupt of falling edge
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    // bit mask to select GPIO0
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    // set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    // enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    // start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    // install gpio isr service
    gpio_install_isr_service(0);
    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(
        GPIO_INPUT_IO, gpio_isr_handler, (void *)GPIO_INPUT_IO);

    int count = 0;

    while (1)
    {
        count = count + 1;
        ESP_LOGI(TAG, "cnt: %d\n", count);
        vTaskDelay(1000 / portTICK_RATE_MS);
        gpio_set_level(GPIO_OUTPUT_IO, count % 2);
    }
}

// Tests to see if the isr handler is working
static void
app_test(void *arg)
{
    gpio_config_t io_conf;
    // disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    // bit mask to select GPIO0
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    // disable pull-down mode
    io_conf.pull_down_en = 0;
    // disable pull-up mode
    io_conf.pull_up_en = 0;
    // configure GPIO0 with the given settings
    gpio_config(&io_conf);

    // interrupt of falling edge
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    // bit mask to select GPIO0
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    // set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    // enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    // install gpio isr service
    gpio_install_isr_service(0);
    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(
        GPIO_INPUT_IO, gpio_isr_handler, (void *)GPIO_INPUT_IO);

    ESP_LOGI(TEST_TAG, "Starting test...");
    gpio_set_level(GPIO_OUTPUT_IO, 1);

    // ISR should be handled as this tells the scheduler to switch to another
    // task
    taskYIELD();
    vTaskDelay(100 / portTICK_RATE_MS);

    uint32_t gpio_pin;
    if (xQueueReceive(gpio_evt_queue, &gpio_pin, 1000/portTICK_RATE_MS))
    {
        ESP_LOGE(TEST_TAG, "GPIO Interrupt Test FAILED: Received interrupt on rising edge instead of falling edge.");
        return;
    }
    
    gpio_set_level(GPIO_OUTPUT_IO, 0);
    // ISR should be handled as this tells the scheduler to switch to another
    // task
    taskYIELD();
    vTaskDelay(100 / portTICK_RATE_MS);

    // GPIO_INPUT_IO should be the first thing in the queue
    if (!xQueueReceive(gpio_evt_queue, &gpio_pin, portMAX_DELAY))
    {
        ESP_LOGE(
            TEST_TAG,
            "GPIO Interrupt Test FAILED: Failed to receive data over queue.");
    }
    else if (gpio_pin != GPIO_INPUT_IO)
    {
        ESP_LOGE(TEST_TAG,
                 "GPIO Interrupt Test FAILED: Received incorrect data, got %d "
                 "instead of %d.",
                 gpio_pin,
                 GPIO_OUTPUT_IO);
    }
    else
    {
        ESP_LOGI(TEST_TAG, "GPIO Interrupt Test PASSED.");
    }

    ESP_LOGI(TEST_TAG, "Reset to redo the test or run normal operation.");

    // Sleep until reset.
    esp_deep_sleep(0);
}

void
app_main(void)
{
    bool run_tests      = false;
    bool valid_input    = false;
    char input_chars[2] = "\0";
    while (!valid_input)
    {
        ESP_LOGI(
            "INIT",
            "Do you want to run tests instead of the default operation? y/N");

        while (strlen(input_chars) == 0)
        {
            while (uart_rx_one_char((uint8_t *)input_chars) == OK)
            {
            }
            UartRxString((uint8_t *)input_chars, sizeof(input_chars) - 1);
        }
        // echo the character back to the sender to confirm character pressed
        if (input_chars[0] != '\r')
        {
            printf("%c\n", input_chars[0]);
        }

        if (input_chars[0] == 'Y' || input_chars[0] == 'y')
        {
            run_tests   = true;
            valid_input = true;
        }
        else if (input_chars[0] == 'N' || input_chars[0] == 'n'
                 || input_chars[0] == '\r')
        {
            valid_input = true;
        }
        else
        {
            ESP_LOGW("INIT",
                     "Please enter a valid character (Y/y for yes, N/n for no, "
                     "or enter for default).");
            input_chars[0] = '\0';
        }
    }
    if (run_tests)
    {
        xTaskCreate(app_test, "app_test", 2048, NULL, 10, NULL);
    }
    else
    {
        xTaskCreate(app_run, "app_run", 2048, NULL, 10, NULL);
    }
}
