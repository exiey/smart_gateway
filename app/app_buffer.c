#include "app_buffer.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "logc/log.h"

static pthread_mutex_t lock_initializer = PTHREAD_MUTEX_INITIALIZER;

int app_buffer_init(Buffer *buffer, int total_len)
{
    log_trace("Buffer %p initializing...", buffer);
    bzero(buffer, sizeof(Buffer));
    if (!buffer)
    {
        log_error("Buffer not valid");
        return -1;
    }

    buffer->ptr = malloc(total_len);
    if (!buffer->ptr)
    {
        log_error("Not enough memory for buffer %p", buffer);
        // 申请失败
        return -1;
    }
    // 初始化buffer同步锁
    memcpy(&buffer->lock, &lock_initializer, sizeof(pthread_mutex_t));

    buffer->total_len = total_len;
    log_trace("Buffer %p initialized", buffer);
    return 0;
}

