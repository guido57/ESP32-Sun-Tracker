#include <Arduino.h>
#include <SPIFFS.h>
#include <esp_spiffs.h>


void InitSPIFFS(){

Serial.println("Initializing SPIFFS");
    
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };
    
    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    const char * TAG = "SPIFFS_init";

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            Serial.println("Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            Serial.println("Failed to find SPIFFS partition");
        } else {
            Serial.printf("Failed to initialize SPIFFS (%s)\r\n", esp_err_to_name(ret));
        }
        return;
    }
    
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        Serial.printf("Failed to get SPIFFS partition information (%s)\r\n", esp_err_to_name(ret));
    } else {
        Serial.printf("Partition size: total: %d, used: %d\r\n", total, used);
    }

}