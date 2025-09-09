#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <MQTTClient.h>
#include <assert.h>

#include "logc/log.h"

#define ADDRESS "tcp://192.168.196.131:1883"
#define CLIENTID "MqttTest"
#define PUSH_TOPIC "test/MqttPushTest"
#define PULL_TOPIC "test/MqttPullTest"
#define QOS 1
#define TIMEOUT 10000L

static MQTTClient client;
static MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
static MQTTClient_deliveryToken deliveredtoken;

// 消息发送回调
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    assert(context == NULL);
    log_trace("Message with token value %d delivery confirmed", dt);
    deliveredtoken = dt;
}

// 消息到达回调
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    assert(context == NULL);
    log_trace("Message from topic %.*s arrived, content %.*s", strlen(topicName), topicName, message->payloadlen, (char *)message->payload);
    // 消息处理完毕必须释放资源
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

// 链接丢失回调
void connlost(void *context, char *cause)
{
    assert(context == NULL);
    log_fatal("MQTT Connection lost because of %s, closing...", cause);
    exit(EXIT_FAILURE);
}

void mqtt_close()
{
    // 断开连接
    MQTTClient_disconnect(client, TIMEOUT);
    // 清理客户端
    MQTTClient_destroy(&client);
}

static void error_code_resolver(int error_code)
{
    switch (error_code)
    {
    case MQTTCLIENT_SUCCESS:
        break;
    case MQTTCLIENT_FAILURE:
        log_fatal("MQTTClient_create failed: General failure\n");
        break;
    case MQTTCLIENT_BAD_PROTOCOL:
        log_fatal("MQTTClient_create failed: Unsupported protocol\n");
        break;
    case MQTTCLIENT_BAD_MQTT_VERSION:
        log_fatal("MQTTClient_create failed: Unsupported MQTT version\n");
        break;
    case MQTTCLIENT_BAD_UTF8_STRING:
        log_fatal("MQTTClient_create failed: Invalid UTF-8 string\n");
        break;
    case MQTTCLIENT_NULL_PARAMETER:
        log_fatal("MQTTClient_create failed: NULL parameter\n");
        break;
    default:
        log_fatal("MQTTClient_create failed: Unknown error code %d\n", error_code);
    }
}

int mqtt_init()
{
    int res;
    // 创建MQTT客户端
    if ((res = MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        log_fatal("MQTT Client creation fails");
        error_code_resolver(res);

        return -1;
    }

    // 设置回调函数
    if ((res = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)) != MQTTCLIENT_SUCCESS)
    {
        mqtt_close();
        log_fatal("MQTT set callback fail");
        error_code_resolver(res);

        return -1;
    }

    // 连接MQTT服务器
    conn_opts.connectTimeout = TIMEOUT;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((res = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        mqtt_close();
        log_fatal("MQTT connect fail");
        error_code_resolver(res);

        return -1;
    }

    // 订阅话题
    if ((res = MQTTClient_subscribe(client, PULL_TOPIC, QOS)) != MQTTCLIENT_SUCCESS)
    {
        mqtt_close();
        log_warn("MQTT subscribe fail");
        error_code_resolver(res);

        return -1;
    }
    return 0;
}

int mqtt_send(void *ptr, int len)
{
    log_trace("Message: %s, len: %d", ptr, len);
    int result = 0, res;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    pubmsg.payload = ptr;
    pubmsg.payloadlen = len;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;

    if ((res = MQTTClient_publishMessage(client, PUSH_TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
    {
        log_warn("Message send fail");
        error_code_resolver(res);

        result = -1;
    }
    else
    {
        log_info("Message send success");
        error_code_resolver(res);

        result = 0;
    }

    log_info("token is %d", token);
    while (token != deliveredtoken)
        ;
    log_info("message has been acknowledged~, deliveredtoken is %d", deliveredtoken);

    return result;
}

int main()
{
    mqtt_init();

    char buf[128];

    while (1)
    {
        memset(buf, 0, 128);
        read(0, buf, 128);
        log_trace("Recieved message from console: %s", buf);
        mqtt_send(buf, strlen(buf));
    }

    mqtt_close();
}