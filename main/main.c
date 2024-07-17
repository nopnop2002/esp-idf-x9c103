/* X9C10X Digitally Controlled Potentiometer example of ESP-IDF
 *
 * This sample code is in the public domain.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "x9c103.h"

static const char *TAG = "MAIN";

void app_main()
{
#if CONFIG_MODEL_X9C102
	ESP_LOGI(TAG, "X9C102");
	float total= KOHMS_X9C102;
#elif CONFIG_MODEL_X9C103
	ESP_LOGI(TAG, "X9C103");
	float total= KOHMS_X9C103;
#elif CONFIG_MODEL_X9C503
	ESP_LOGI(TAG, "X9C503");
	float total= KOHMS_X9C503;
#elif CONFIG_MODEL_X9C104
	ESP_LOGI(TAG, "X9C104");
	float total= KOHMS_X9C104;
#endif
	ESP_LOGI(TAG, "total=%f KOhms", total);
	ESP_LOGI(TAG, "CONFIG_INCREMENT_GPIO=%d", CONFIG_INCREMENT_GPIO);
	ESP_LOGI(TAG, "CONFIG_UPDOWN_GPIO=%d", CONFIG_UPDOWN_GPIO);
	ESP_LOGI(TAG, "CONFIG_DEVICE_SELECT_GPIO=%d", CONFIG_DEVICE_SELECT_GPIO);

	X9C_t dev;
	X9C_init(&dev, CONFIG_INCREMENT_GPIO, CONFIG_UPDOWN_GPIO, CONFIG_DEVICE_SELECT_GPIO, total);
	X9C_begin(&dev, 0);

	uint8_t counter = 0;
	uint8_t DIRECTION = 1;
	
	while(1) {
		X9C_set_counter(&dev, counter);
		float resistance = X9C_getK(&dev);
		ESP_LOGI(TAG, "counter=%d resistance=%f KOhms", counter, resistance);
		if(DIRECTION) {
			counter++;
			if(counter == 99) DIRECTION = 0;
		} else {
			counter--;
		 if(counter == 0) DIRECTION = 1;
		}
		vTaskDelay(20);
	} // end while

}
