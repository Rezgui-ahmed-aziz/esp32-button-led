#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_GPIO 2        // Built-in LED on ESP-WROOM-32 (e.g., ESP32-DevKitC)
#define BUTTON_GPIO 0     // BOOT button on ESP-WROOM-32
#define DEBOUNCE_TIME_MS 50 // Debounce time in milliseconds

static const char *TAG = "button_led";
static QueueHandle_t button_queue; // Queue for button events

// Interrupt Service Routine (ISR) for BOOT button press
static void IRAM_ATTR button_isr_handler(void *arg)
{
    static uint32_t last_isr_time = 0;
    uint32_t current_time = xTaskGetTickCountFromISR() * portTICK_PERIOD_MS;
    
    // Debouncing: ignore interrupts within 50ms
    if (current_time - last_isr_time > DEBOUNCE_TIME_MS) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        uint32_t gpio_num = (uint32_t)arg;
        xQueueSendFromISR(button_queue, &gpio_num, &xHigherPriorityTaskWoken);
        last_isr_time = current_time;
        if (xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR();
        }
    }
}

// Task to handle button events and toggle LED
static void button_task(void *arg)
{
    uint32_t gpio_num;
    static bool led_state = false;

    while (1) {
        if (xQueueReceive(button_queue, &gpio_num, portMAX_DELAY)) {
            // Toggle LED state
            led_state = !led_state;
            // Use !led_state for active-low LED (common on some boards)
            gpio_set_level(LED_GPIO, led_state); // Try led_state or !led_state
            ESP_LOGI(TAG, "BOOT button pressed, LED is now %s", led_state ? "ON" : "OFF");
        }
    }
}

// Configure GPIO for LED and BOOT button
static void configure_gpio(void)
{
    // Configure LED GPIO as output
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    // Configure BOOT button GPIO as input with pull-up
    gpio_config_t button_config = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,    // Internal pull-up for BOOT button
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE       // Falling edge (BOOT button to GND)
    };
    gpio_config(&button_config);

    // Install ISR service
    gpio_install_isr_service(0);
    // Attach ISR to BOOT button GPIO
    gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, (void *)BUTTON_GPIO);
}

void app_main(void)
{
    // Create queue for button events
    button_queue = xQueueCreate(10, sizeof(uint32_t));
    if (button_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create button queue");
        return;
    }

    // Configure GPIOs
    configure_gpio();

    // Create button handling task
    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);

    ESP_LOGI(TAG, "BOOT button-controlled LED initialized on ESP-WROOM-32");
}