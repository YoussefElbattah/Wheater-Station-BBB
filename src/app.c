#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <mosquitto.h>

#include "display.h"
#include "weather.h"
#include "wireless.h"
#include "mqtt.h"

#define RETRY_MAX	4

int main(void){

	int ret;
	struct mosquitto **mosq = malloc(sizeof(struct mosquitto *));
	struct mqtt_config_t *cfg_ctx = malloc(sizeof(struct mqtt_config_t));
	
	ret = display_init();
	if(ret < 0){
		printf("Couldn't initialize LCD\n");
		return ret;
	}
	
	ret = wireless_init(mosq, cfg_ctx);
	if(ret < 0){
		printf("Couldn't initialize wireless, err %d\n", ret);
		//return ret;
	}

	sensor_attr temp_attr;
	while(1){
		
		ret = weather_read(PATH TEMP_VALUE, &temp_attr);
		
		if(ret){
			printf("Couldn't read weather data, error %d\n", ret);
			return ret;
		}

		ret = display_result(temp_attr);
		
		if(ret < 0){
			printf("Couldn't display result, error %d\n", ret);
			return ret;
		}
		
		if(mqtt_connected)
			wireless_send(*mosq, cfg_ctx->topic ,temp_attr);

		sleep(1);
	}


}

