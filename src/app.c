#include <stdio.h>
#include "lcd.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <mosquitto.h>

#include "bme280.h"
#include "mqtt.h"

#define RETRY_MAX	4

int main(int argc, char **argv){
	const char path[] = PATH TEMP_VALUE;
	int ret;
	struct mosquitto *mosq;
	struct mqtt_config_t *cfg_ctx = malloc(sizeof(struct mqtt_config_t));
	int retry_conn = 0;
	/*ret = lcd_init();
	if(ret < 0){
		printf("Couldn't initialize LCD\n");
		return ret;
	}

	sleep(2);

	ret = lcd_clear();
	if(ret < 0)
		return ret;
	*/

	ret = mqtt_init(&mosq);
	
	if(ret < 0)
		return ret;

	ret = mqtt_load_config(cfg_ctx);
	
	if(ret < 0)
		return ret;

	do{
		printf("trying to connect %d time \n", retry_conn + 1);
		ret = mqtt_connect(mosq, cfg_ctx);
		retry_conn++;
	}while((ret != MOSQ_ERR_SUCCESS) && (retry_conn < RETRY_MAX));

	if(retry_conn >= RETRY_MAX){
		printf("Connecting to Broker failed retried %d times\n", retry_conn);
		mosquitto_destroy(mosq);
		mosquitto_lib_cleanup();
		return ret;
	}

	ret = mosquitto_loop_start(mosq);
	if(ret != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(mosq);
		printf("Error while start mosquitto loop: %s\n", mosquitto_strerror(ret));
		return ret;
	}

	sensor_attr temp_attr;
	char msg[SOME_BYTES];
	while(1){
		
		//lcd_set_cursor(1,1);
		int ret = read_sysfs_long(path, &temp_attr.current_val);
		if(ret){
			printf("Couldn't read temperature value from sysfs , error %d", ret);
			return ret;
		}

		//printf("current_val = %4.1f \n", temp_attr.current_val);
		ret = max_min_check(&temp_attr);
		if(ret){
			printf("Couldn't check max/min values \n");
			return ret;
		}
		//printf("max/min = %4.1f/%4.1f \n", temp_attr.max_value, temp_attr.min_value);
		ret = status_update(&temp_attr);
		if(ret)
			return ret;

		snprintf(msg, sizeof(msg), "T:%4.1fC ST:%s  ", temp_attr.current_val,temp_attr.stat);

		printf("msg = %s\n", msg);
		/*ret = lcd_send_message(msg);
		if(ret < 0)
			return ret;

		lcd_set_cursor(2,1);*/
		
		snprintf(msg, sizeof(msg), "LO/HI: %4.1f/%4.1f", temp_attr.min_value, temp_attr.max_value);
		      
		printf("msg = %s\n", msg);
		/*ret = lcd_send_message(msg);
		if(ret < 0)
			return ret;
		*/

		mqtt_publish(mosq, cfg_ctx->topic ,temp_attr);

		sleep(1);
	}


}

