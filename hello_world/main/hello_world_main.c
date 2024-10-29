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

// Define HX711 pins and constants
#define HX711_DOUT_PIN GPIO_NUM_16
#define HX711_SCK_PIN GPIO_NUM_4

int32_t hx711_read()
{
    int32_t count = 0;
    gpio_set_level(HX711_SCK_PIN, 0); // Ensure SCK is low

    // Wait until DOUT goes low, indicating data is ready
    while (gpio_get_level(HX711_DOUT_PIN))
        ;

    // Read 24-bit data from HX711
    for (int i = 0; i < 24; i++)
    {
        gpio_set_level(HX711_SCK_PIN, 1);
        count = count << 1 | gpio_get_level(HX711_DOUT_PIN);
        gpio_set_level(HX711_SCK_PIN, 0);
    }

    // Set gain to 128 by sending one more clock pulse
    gpio_set_level(HX711_SCK_PIN, 1);
    count ^= 0x800000; // Convert from signed 24-bit to signed 32-bit
    gpio_set_level(HX711_SCK_PIN, 0);

    return count;
}

float calibration_factor = -7050.0;

void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK)
    {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    while (true)
    {
        printf("Reading HX711...\n");
        int32_t raw_value = hx711_read();
        // float weight = raw_value / calibration_factor; // Convert raw value to weight
        printf("raw_value: %ld\n", raw_value);
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1-second delay
    }

    // for (int i = 10; i >= 0; i--)
    // {
    //     printf("Restarting in %d seconds...\n", i);
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
    // printf("Restarting now.\n");
    // fflush(stdout);
    // esp_restart();
}
