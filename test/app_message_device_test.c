#include "../app/app_message.h"
#include "/home/xhy/getway/utils/logc/log.h"
#include <stdio.h>
#include "../app/app_device.h"
#include <string.h>
#include <strings.h>

int main() {
    Message message;
    log_set_level(LOG_INFO);

    // 1. 测试 普通字节序列 -> Message结构体实例 -> JSON字符串 的转换
    log_info("测试 普通字节序列 -> Message结构体实例 -> JSON字符串 的转换");
    // 用于测试的字节序列
    char initMessagePtr[] = {2, 3, '0', '1', 'O', 'K', 0};
    // 用于存储 JSON 字符串
    char messageJsonPtr[64];
    memset(messageJsonPtr, 0, 64);

    app_message_init(&message, BLUETOOTH, initMessagePtr, strlen(initMessagePtr) + 1);
    log_info("{2, 3, '0', '1', 'O', 'K', 0} 经过app_message_init()处理之后得到的message信息如下:\nmessage.id_len: %d\nmessage.msg_len: %d\nmessage.payload: %s", message.id_len, message.msg_len, message.payload);

    app_message_printJson(&message, messageJsonPtr, 64);
    log_info("上述message实例经过app_message_saveBinary()处理后得到的JSON字符串如下: \n%s\n", messageJsonPtr);

    // 2. 测试 JSON字符串 -> Message结构体实例 -> 普通字符序列 的转换
    log_info("测试 JSON字符串 -> Message结构体实例 -> 普通字符序列 的转换");
    // 用于测试的 JSON 字符串
    char *initJsonStr = "{\"connection_type\":1,\"id\":\"3031\",\"message\":\"4F4B00\"}";
    char binarySeq[32];
    memset(binarySeq, 0, 32);

    // 清零 message
    bzero(&message, sizeof(Message));

    app_message_initByJson(&message, initJsonStr);
    log_info("{\"connection_type\":1,\"id\":\"3031\",\"message\":\"4F4B00\"} 经过app_message_initByJson()处理之后得到的message信息如下:\nmessage.id_len: %d\nmessage.msg_len: %d\nmessage.payload: %s", message.id_len, message.msg_len, message.payload);

    app_message_saveBinary(&message, binarySeq, 32);
    log_info("字节序列的前两个字节数值分别为: %d\t%d", binarySeq[0], binarySeq[1]);
    log_info("字节序列从第三个字节开始的字符表示为: %s", (binarySeq + 2));
}