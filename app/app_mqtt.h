#if !defined(__APP_MQTT_H__)
#define __APP_MQTT_H__

#define ADDRESS "tcp://192.168.196.131:1883"
#define CLIENTID "e06d7035-16e0-41f3-a8b0-a7ff930deac9"
#define PUSH_TOPIC "ExamplePushTopic"
#define PULL_TOPIC "ExamplePullTopic"
#define QOS 1
#define TIMEOUT 10000L

int app_mqtt_init();

void app_mqtt_close();

void app_mqtt_registerRecvCallback(int (*callback)(void *ptr, int len));

int app_mqtt_send(void *ptr, int len);

#endif // __APP_MQTT_H__