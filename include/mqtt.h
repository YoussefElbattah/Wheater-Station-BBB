#ifndef __MQTT_H
#define __MQTT_H

#include <mosquitto.h>
#include "bme280.h"

#define CONFIG_FILE	"/etc/bbb_mqtt.conf"

struct mqtt_config_t {
	    char host[64];
	    int  port;
	    char user[64];
	    char pass[64];
	    char topic[64];
	    int  keepalive;
};

int mqtt_load_config(struct mqtt_config_t *cfg); 
int mqtt_connect(struct mosquitto *mosq, struct mqtt_config_t *cfg);
int mqtt_init(struct mosquitto **mosq);
int mqtt_publish(struct mosquitto *mosq, char *topic, sensor_attr attr);

#endif 
