#include "/home/xhy/getway/app/app_buffer.c"
#include <string.h>
#include <stdio.h>
#include "/home/xhy/getway/utils/logc/log.h"

int main() {
    log_set_level(LOG_INFO);
    Buffer buf;
    if (app_buffer_init(&buf, 128) == -1) {
        log_error("缓存初始化失败");
    }
    char *str = "hello, this is app buffer test~";
    char read_buf[128];

    app_buffer_write(&buf, str, strlen(str));
    app_buffer_read(&buf, read_buf, 128);

    printf("写入缓存的数据是: %s\n", read_buf);

    app_buffer_close(&buf);
}