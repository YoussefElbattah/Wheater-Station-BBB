#include "mqtt.h"
#include "bme280.h"

#include <mosquitto.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

volatile int mqtt_connected = 0;

void on_disconnect(struct mosquitto *mosq, void *obj, int reason_code){
	
	printf("on_disconnect : %s\n", mosquitto_connack_string(reason_code));
	if((reason_code != 0)){
		mosquitto_reconnect(mosq);
	}else
		mqtt_connected = 0;

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
		mqtt_connected = 1;

}

void trim(char *s){
	char *p = s;

	while((*p) && (*p != '\n') && (*p != '\r'))
		p++;

	*p = 0;
}
int mqtt_load_config(struct mqtt_config_t *cfg)
{
	FILE *fd = fopen(CONFIG_FILE, "r");

	if(!fd){
		printf("Couldn't open config file\n, err : %d", -errno);
		return -errno;
	}	

	char line[256];
	while(fgets(line, sizeof(line), fd)){
		
		trim(line); // remove '\n' from the buffer and convert it to a buffer with '\0'
		
		char key[64] , value[124];

		if(sscanf(line, "%63[^=]=%123s", key, value) != 2)
			continue;

		if(!strcmp(key , "host")) strncpy(cfg->host, value, sizeof(cfg->host));
		
		if(!strcmp(key , "port")) cfg->port = atoi(value);
		
		if(!strcmp(key , "user")) strncpy(cfg->user, value, sizeof(cfg->user));

		if(!strcmp(key , "pass")) strncpy(cfg->pass, value, sizeof(cfg->pass));
		
		if(!strcmp(key , "topic")) strncpy(cfg->topic, value, sizeof(cfg->topic));

		if(!strcmp(key , "keepalive")) cfg->keepalive = atoi(value);

	}

	fclose(fd);

	return 0;

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


int mqtt_connect(struct mosquitto *mosq, struct mqtt_config_t *cfg){
	
	int ret = mosquitto_username_pw_set(mosq, cfg->user, cfg->pass);
	if(ret != MOSQ_ERR_SUCCESS){
		printf("Couldn't set username & passwd, ERROR : %s\n", mosquitto_strerror(ret));
		return ret;
	}

	ret = mosquitto_connect(mosq, cfg->host, cfg->port, cfg->keepalive);
	
	if(ret != MOSQ_ERR_SUCCESS){
		printf("Couldn't connect to the broker, Error: %s\n", mosquitto_strerror(ret));
		return ret;
	}
	
	return ret;

}

void mqtt_publish(struct mosquitto *mosq, char *topic, sensor_attr attr){
	
	int ret;
	char payload[100];

	snprintf(payload, sizeof(payload), "{\"current_temp\":%.2f,\"min\":%.2f,\"max\":%.2f," 
	"\"status_code\":%d,\"status\":\"%s\"}", attr.current_val,
	attr.min_value, attr.max_value, STATUS_TO_CODE(attr.stat), attr.stat);
	
	ret = mosquitto_publish(mosq, NULL, topic, strlen(payload), payload, 2, false);
	if(ret != MOSQ_ERR_SUCCESS){
		printf("Error while publishing : %s\n", mosquitto_strerror(ret));
	}

}
