#include <stdio.h>
#include "mqtt.h"
#include "wireless.h"

#define RETRY_MAX	4

int wireless_init(struct mosquitto **mosq, struct mqtt_config_t *cfg_ctx){
	
	int ret = mqtt_init(mosq);
	int retry_conn = 0;
	if(ret < 0)
		return ret;

	ret = mqtt_load_config(cfg_ctx);
	if(ret < 0)
		return ret;
	
	ret = mqtt_tls_set(*mosq, cfg_ctx);
	if(ret != MOSQ_ERR_SUCCESS)
		return ret;

	do{
		printf("trying to connect %d time \n", retry_conn + 1);
		ret = mqtt_connect(*mosq, cfg_ctx);
		retry_conn++;
	}while((ret != MOSQ_ERR_SUCCESS) && (retry_conn < RETRY_MAX));

	if(retry_conn >= RETRY_MAX){
		printf("Connecting to Broker failed retried %d times\n", retry_conn);
		mosquitto_destroy(*mosq);
		mosquitto_lib_cleanup();
		return ret;
	}
	
	ret = mosquitto_loop_start(*mosq);
	if(ret != MOSQ_ERR_SUCCESS){
		mosquitto_destroy(*mosq);
		printf("Error while start mosquitto loop: %s\n", mosquitto_strerror(ret));
		return ret;
	}
	
	while (!mqtt_connected) {
    		mosquitto_loop(*mosq, -1, 1);
		usleep(200000);
	}

	return 0;

}

void wireless_send(struct mosquitto *mosq, char *topic, sensor_attr attr){
	
	mqtt_publish(mosq, topic ,attr);
}
