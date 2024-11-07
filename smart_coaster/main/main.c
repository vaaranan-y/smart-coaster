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
#include "esp_http_client.h"
#include "esp_tls.h"
#include "esp_wifi.h"

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
static const char *TAG = "HTTP_CLIENT";
#define NVS_NAMESPACE "nvs"
static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer; // Buffer to store response of http request from event handler
    static int output_len;      // Stores number of bytes read
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        /*
         *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
         *  However, event handler can also be used in case chunked encoding is used.
         */
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            // If user_data buffer is configured, copy the response into the buffer
            if (evt->user_data)
            {
                memcpy(evt->user_data + output_len, evt->data, evt->data_len);
            }
            else
            {
                if (output_buffer == NULL)
                {
                    output_buffer = (char *)malloc(esp_http_client_get_content_length(evt->client));
                    output_len = 0;
                    if (output_buffer == NULL)
                    {
                        ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                        return ESP_FAIL;
                    }
                }
                memcpy(output_buffer + output_len, evt->data, evt->data_len);
            }
            output_len += evt->data_len;
        }

        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        if (output_buffer != NULL)
        {
            // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
            // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        int mbedtls_err = 0;
        esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
        if (err != 0)
        {
            ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
            ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
        }
        if (output_buffer != NULL)
        {
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        esp_http_client_set_header(evt->client, "From", "user@example.com");
        esp_http_client_set_header(evt->client, "Accept", "text/html");
        break;
    }
    return ESP_OK;
}

static void http_rest_with_hostname_path(void)
{

    esp_netif_init();                // Initialize network interface
    esp_event_loop_create_default(); // Create default event loop

    esp_netif_create_default_wifi_sta(); // Create a default STA interface

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); // Default wifi config
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH)); // Set storage type
    ESP_ERROR_CHECK(esp_wifi_start());                         // Start WiFi

    // Wait for WiFi connection
    wifi_event_group = xEventGroupCreate();
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    // Once connected, proceed with the HTTP request
    ESP_LOGI(TAG, "WiFi connected! Starting HTTP client...");

    esp_http_client_config_t config = {
        .host = "httpbin.org",
        .path = "/get",
        .transport_type = HTTP_TRANSPORT_OVER_TCP,
        .event_handler = _http_event_handler,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET
    esp_err_t err = esp_http_client_perform(client);
    // if (err == ESP_OK)
    // {
    //     ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld",
    //              esp_http_client_get_status_code(client),
    //              esp_http_client_get_content_length(client));
    // }
    // else
    // {
    //     ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    // }

    // // POST
    // const char *post_data = "field1=value1&field2=value2";
    // esp_http_client_set_url(client, "/post");
    // esp_http_client_set_method(client, HTTP_METHOD_POST);
    // esp_http_client_set_post_field(client, post_data, strlen(post_data));
    // err = esp_http_client_perform(client);
    // if (err == ESP_OK)
    // {
    //     ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %lld",
    //              esp_http_client_get_status_code(client),
    //              esp_http_client_get_content_length(client));
    // }
    // else
    // {
    //     ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    // }

    // // PUT
    // esp_http_client_set_url(client, "/put");
    // esp_http_client_set_method(client, HTTP_METHOD_PUT);
    // err = esp_http_client_perform(client);
    // if (err == ESP_OK)
    // {
    //     ESP_LOGI(TAG, "HTTP PUT Status = %d, content_length = %lld",
    //              esp_http_client_get_status_code(client),
    //              esp_http_client_get_content_length(client));
    // }
    // else
    // {
    //     ESP_LOGE(TAG, "HTTP PUT request failed: %s", esp_err_to_name(err));
    // }

    // // PATCH
    // esp_http_client_set_url(client, "/patch");
    // esp_http_client_set_method(client, HTTP_METHOD_PATCH);
    // esp_http_client_set_post_field(client, NULL, 0);
    // err = esp_http_client_perform(client);
    // if (err == ESP_OK)
    // {
    //     ESP_LOGI(TAG, "HTTP PATCH Status = %d, content_length = %lld",
    //              esp_http_client_get_status_code(client),
    //              esp_http_client_get_content_length(client));
    // }
    // else
    // {
    //     ESP_LOGE(TAG, "HTTP PATCH request failed: %s", esp_err_to_name(err));
    // }

    // // DELETE
    // esp_http_client_set_url(client, "/delete");
    // esp_http_client_set_method(client, HTTP_METHOD_DELETE);
    // err = esp_http_client_perform(client);
    // if (err == ESP_OK)
    // {
    //     ESP_LOGI(TAG, "HTTP DELETE Status = %d, content_length = %lld",
    //              esp_http_client_get_status_code(client),
    //              esp_http_client_get_content_length(client));
    // }
    // else
    // {
    //     ESP_LOGE(TAG, "HTTP DELETE request failed: %s", esp_err_to_name(err));
    // }

    // // HEAD
    // esp_http_client_set_url(client, "/get");
    // esp_http_client_set_method(client, HTTP_METHOD_HEAD);
    // err = esp_http_client_perform(client);
    // if (err == ESP_OK)
    // {
    //     ESP_LOGI(TAG, "HTTP HEAD Status = %d, content_length = %lld",
    //              esp_http_client_get_status_code(client),
    //              esp_http_client_get_content_length(client));
    // }
    // else
    // {
    //     ESP_LOGE(TAG, "HTTP HEAD request failed: %s", esp_err_to_name(err));
    // }

    // esp_http_client_cleanup(client);
}

static void http_test_task(void *pvParameters)
{
    printf("HTTP test task\n");
    http_rest_with_hostname_path();
    vTaskDelete(NULL);
}

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
        xTaskCreate(&http_test_task, "http_test_task", 8192, NULL, 5, NULL);
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
