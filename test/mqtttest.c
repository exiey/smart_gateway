#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <MQTTClient.h>
#include <assert.h>

#include "/home/xhy/getway/app/app_mqtt.h"
#include "/home/xhy/getway/utils/logc/log.h"

static int receive_flag = 0;

int receCallBack(char * str,int len)
{
    receive_flag = 1;
    assert(memcmp(str,"Hello world!",len));
    return 0;
}

int main() {
    // 1. 初始化MQTT，必须检查返回值
    int init_ret = app_mqtt_init();
    if (init_ret != 0) {
        log_error("MQTT init failed! ret: %d", init_ret);
        return -1; // 初始化失败直接退出，避免后续错误
    }

    // 2. 注册回调（此时回调函数参数已改为void*，无警告）
    app_mqtt_registerRecvCallback(receCallBack);

    // 3. 发送消息（你的现有逻辑）
    char *msg = "hello world!";
    app_mqtt_send(msg, strlen(msg));

    // 4. 关键：增加「等待消息接收」的逻辑
    // 原因：MQTT接收是异步的（回调在底层线程执行），主线程跑得太快，断言会提前执行
    int wait_cnt = 0;
    while (receive_flag == 0 && wait_cnt < 10) { // 等待10秒（每1秒检查一次）
        log_trace("Waiting for message... (%d/10)", wait_cnt);
        sleep(1); // 暂停1秒，给回调线程时间执行
        wait_cnt++;
    }

    // 5. 断言检查（此时若收到消息，receive_flag已设为1）
    assert(receive_flag == 1);

    // 6. 清理资源
    app_mqtt_close();
    return 0;
}
