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
#include "nvs_flash.h"
#include "esp_err.h"

#define NVS_NAMESPACE "nvs"

void testScale(void *pvParameters)
{
    // Load Cell Code
    hx711_t dev = {
        .dout = GPIO_NUM_16,
        .pd_sck = GPIO_NUM_4,
        .gain = HX711_GAIN_A_128};

    ESP_ERROR_CHECK(hx711_init(&dev));

    // Calibration
    nvs_handle_t nvs_handle;
    float scalingFactor = 1;
    int offset = 0;

    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err == ESP_OK)
    {
        printf("NVS opened successfully\n");
        size_t required_size = 0; // Initialize required size variable
        err = nvs_get_str(nvs_handle, "scaling_factor", NULL, &required_size);
        if (err == ESP_OK)
        {
            char *scaling_factor_str = malloc(required_size);
            err = nvs_get_str(nvs_handle, "scaling_factor", scaling_factor_str, &required_size);
            if (err == ESP_OK)
            {
                printf("Scaling factor: %s\n", scaling_factor_str);
                scalingFactor = atof(scaling_factor_str);
            }
            else
            {
                printf("Error reading scaling factor memory key (2)\n");
            }
        }
        else
        {
            printf("Error reading scaling factor memory key (1)\n");
        }
    }
    else
    {
        printf("Error opening NVS handle\n");
    }
    nvs_close(nvs_handle);

    esp_err_t r = hx711_wait(&dev, 500);
    if (r != ESP_OK)
    {
        // ESP_LOGE(TAG, "Device not found: %d (%s)\n", r, esp_err_to_name(r));
        printf("Device not found: %d (%s)\n", r, esp_err_to_name(r));
        return;
    }

    printf("Getting offset  ...\n");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    int32_t data;
    r = hx711_read_average(&dev, 10, &data);
    offset = data;

    if (err != ESP_OK) // If reading from NVS failed at any
    {

        printf("Place a known weight on the scale\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        int32_t knownWeight = 362; // Hardcoded known weight
        printf("Calibrating...\n");
        r = hx711_read_average(&dev, 10, &data);
        scalingFactor = (data - offset) / knownWeight;

        int len = snprintf(NULL, 0, "%f", scalingFactor);
        char *scalingStr = malloc(len + 1);
        snprintf(scalingStr, len + 1, "%f", scalingFactor);

        esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
        if (err == ESP_OK)
        {
            printf("NVS opened successfully for writing scale factor\n");
            err = nvs_set_str(nvs_handle, "scaling_factor", scalingStr);
            if (err == ESP_OK)
            {
                printf("Calibration factor saved successfully!\n");
            }
            else
            {
                printf("Failed to save calibration factor\n");
            }
        }
        else
        {
            printf("Error opening NVS handle for writing scale factor\n");
        }
        nvs_close(nvs_handle);
    }

    while (1)
    {
        printf("Weight reading loop!\n");
        esp_err_t r = hx711_wait(&dev, 500);
        if (r != ESP_OK)
        {
            // ESP_LOGE(TAG, "Device not found: %d (%s)\n", r, esp_err_to_name(r));
            printf("Device not found: %d (%s)\n", r, esp_err_to_name(r));
            continue;
        }

        int32_t data;
        r = hx711_read_average(&dev, 10, &data);
        if (r != ESP_OK)
        {
            // ESP_LOGE(TAG, "Could not read data: %d (%s)\n", r, esp_err_to_name(r));
            printf("Could not read data: %d (%s)\n", r, esp_err_to_name(r));
            continue;
        }

        // ESP_LOGI(TAG, "Raw data: %" PRIi32, data);
        printf("Raw data: %" PRIi32 "\n", data);
        float actual = (data - offset) / scalingFactor;
        printf("Actual mass: %fg\n", actual);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_main(void)
{
    printf("Hello world!\n");

    // Configure Memory
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    xTaskCreate(testScale, "testScale", configMINIMAL_STACK_SIZE * 5, NULL, 5, NULL);
}
