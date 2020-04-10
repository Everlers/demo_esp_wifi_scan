#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "esp_spi_flash.h"
#include "./st7789v/st7789v.h"
#include "string.h"

#define DEFAULT_SCAN_LIST_SIZE      20//最大的扫描数量

static const char *TAG = "scan";

void wifi_scan(void)
{
    ESP_ERROR_CHECK(esp_netif_init());//初始化底层TCP/IP堆栈
    ESP_ERROR_CHECK(esp_event_loop_create_default());//创建默认的事件循环
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();//创建默认的wifista。 如果有任何init错误，此API将中止。
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();//初始化默认的WiFi配置
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));//配置WiFi

    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);
    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {
        ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
        ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
        print_auth_mode(ap_info[i].authmode);
        if (ap_info[i].authmode != WIFI_AUTH_WEP) {
            print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
        }
        ESP_LOGI(TAG, "Channel \t\t%d\n", ap_info[i].primary);
    }
}

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    lcd_init();
    lcd_show_string(0,0,"ESP32",WHITE,BLACK);
    lcd_show_string(0,8,"wifi scan..",WHITE,BLACK);

    wifi_init_sta();
    while (1)
    {
        vTaskDelay(100);
    }
}