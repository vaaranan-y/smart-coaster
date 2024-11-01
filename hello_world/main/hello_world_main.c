/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "hx711.h"
#include "esp_log.h"

void test(void *pvParameters)
{
    // Load Cell Code
    hx711_t dev = {
        .dout = GPIO_NUM_16,
        .pd_sck = GPIO_NUM_4,
        .gain = HX711_GAIN_A_128};

    ESP_ERROR_CHECK(hx711_init(&dev));

    while (1)
    {
        printf("Hello world loop!\n");
        esp_err_t r = hx711_wait(&dev, 500);
        if (r != ESP_OK)
        {
            // ESP_LOGE(TAG, "Device not found: %d (%s)\n", r, esp_err_to_name(r));
            printf("Device not found: %d (%s)\n", r, esp_err_to_name(r));
            continue;
        }

        int32_t data;
        r = hx711_read_average(&dev, 1, &data);
        if (r != ESP_OK)
        {
            // ESP_LOGE(TAG, "Could not read data: %d (%s)\n", r, esp_err_to_name(r));
            printf("Could not read data: %d (%s)\n", r, esp_err_to_name(r));
            continue;
        }

        // ESP_LOGI(TAG, "Raw data: %" PRIi32, data);
        printf("Raw data: %" PRIi32 "\n", data);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main(void)
{
    printf("Hello world!\n");

    xTaskCreate(test, "test", configMINIMAL_STACK_SIZE * 5, NULL, 5, NULL);

    // for (int i = 10; i >= 0; i--)
    // {
    //     printf("Restarting in %d seconds...\n", i);
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
    // printf("Restarting now.\n");
    // fflush(stdout);
    // esp_restart();
}
