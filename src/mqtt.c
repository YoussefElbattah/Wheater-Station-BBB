#include "mqtt.h"
#include "bme280.h"

#include <mosquitto.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

static int connected = 0;
void on_disconnect(struct mosquitto *mosq, void *obj, int reason_code){
	
	printf("on_disconnect : %s\n", mosquitto_connack_string(reason_code));
	if((reason_code != 0)){
		mosquitto_reconnect(mosq);
	}else
		connected = 0;

}

void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
	printf("Message with mid %d has been published.\n", mid);
}

void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
{
	printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
	if(reason_code != 0){
		mosquitto_disconnect(mosq);				
	}else 
		connected = 1;

}
int mqtt_init(struct mosquitto **mosq){
	
	int rc;
	mosquitto_lib_init();

	*mosq = mosquitto_new(NULL, true, NULL);
		
	if(*mosq == NULL){
		printf("Cannot create a new mosquitto client instance, error : %d\n", -errno);
		return -errno;
	}
	/* Configure callbacks */
	mosquitto_connect_callback_set(*mosq, on_connect);
	mosquitto_publish_callback_set(*mosq, on_publish);
	mosquitto_disconnect_callback_set(*mosq, on_disconnect);

	return 0;
	
}


int mqtt_connect(struct mosquitto *mosq){
	
	int ret = mosquitto_username_pw_set(mosq, USER_NAME, PASSWORD);
	if(ret != MOSQ_ERR_SUCCESS){
		printf("Couldn't set username & passwd, ERROR : %s\n", mosquitto_strerror(ret));
		return ret;
	}

	ret = mosquitto_connect(mosq, HOST, PORT, KEEP_ALIVE);
	
	if(ret != MOSQ_ERR_SUCCESS){
		printf("Couldn't connect to the broker, Error: %s\n", mosquitto_strerror(ret));
		return ret;
	}
	
	return ret;

}

int mqtt_publish(struct mosquitto *mosq, sensor_attr attr){
	
	int ret;
	char payload[100];

	snprintf(payload, sizeof(payload), "{\"current_temp\":%.2f,\"min\":%.2f,\"max\":%.2f," 
	"\"status_code\":%d,\"status\":\"%s\"}", attr.current_val,
	attr.min_value, attr.max_value, STATUS_TO_CODE(attr.stat), attr.stat);
	
	ret = mosquitto_publish(mosq, NULL, "bbb/weather", strlen(payload), payload, 2, false);
	if(ret != MOSQ_ERR_SUCCESS){
		printf("Error while publishing : %s\n", mosquitto_strerror(ret));
	}

	return ret;
}
