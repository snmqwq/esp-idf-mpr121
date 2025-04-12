#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_err.h"
#include "mpr121.h"

#define CONFIG_SCL_GPIO GPIO_NUM_9
#define CONFIG_SDA_GPIO GPIO_NUM_10
#define CONFIG_IRQ_GPIO GPIO_NUM_8

static const char *TAG = "MPR121";

void app_main(void)
{

    ESP_LOGI(TAG, "CONFIG_SCL_GPIO=%d", CONFIG_SCL_GPIO);
    ESP_LOGI(TAG, "CONFIG_SDA_GPIO=%d", CONFIG_SDA_GPIO);
    ESP_LOGI(TAG, "CONFIG_IRQ_GPIO=%d", CONFIG_IRQ_GPIO);

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,  // 使用I2C端口0
        .scl_io_num = GPIO_NUM_9,  // SCL引脚
        .sda_io_num = GPIO_NUM_10,  // SDA引脚
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    MPR121_t dev;
    bool ret = MPR121_begin(&dev, MPR121_ADDR_0x5A, 40, 25, CONFIG_IRQ_GPIO, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO);
    
    if (!ret) {
        ESP_LOGE(TAG, "MPR121 initialization failed, error: %d", MPR121_getError(&dev));
        while(1) vTaskDelay(1); // Wait forever on error
    }

    // MPR121 设置
    MPR121_setFFI(&dev, FFI_10);  // 设置 AFE 配置 1
    MPR121_setSFI(&dev, SFI_10);  // 设置 AFE 配置 2
    MPR121_setGlobalCDT(&dev, CDT_4US);  // 设置全局采样时间
    MPR121_autoSetElectrodesDefault(&dev, true);  // 自动设置电极配置

    // 循环检测触摸事件
    while(1) {
		MPR121_updateAll(&dev);
		for (int i = 0; i < 12; i++) {
			if (MPR121_isNewTouch(&dev, i)) {
				ESP_LOGI(TAG, "electrode %d was just touched", i);
			} else if (MPR121_isNewRelease(&dev, i)) {
				ESP_LOGI(TAG, "electrode %d was just released", i);
			}
		}
		vTaskDelay(10);
	}
}

