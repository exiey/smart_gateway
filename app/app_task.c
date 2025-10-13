#include "app_task.h"
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h>
#include "/home/xhy/getway/utils/logc/log.h"

#define MSG_LEN sizeof(Task)
#define RECV_MQ "/atguigu_gateway_recv_mq"
#define SEND_MQ "/atguigu_gateway_send_mq"

typedef struct TaskStruct
{
    void (*callback)(void *);
    void *argv;
} Task;

static mqd_t send_mq = -1;
static mqd_t recv_mq = -1;
static pthread_t send_thread = 0;
static pthread_t recv_thread = 0;
static TaskType send_type = SEND_TYPE;
static TaskType recv_type = RECV_TYPE;

void *app_task_executor(void *argv)
{
    // 负责处理特定任务类型的线程
    TaskType *type_p = argv;
    log_info("Task type %d executor starting...", *type_p);
    int result;

    // 声明任务结构体
    Task task;
    while (1)
    {
        result = -1;
        switch (*type_p)
        {
        case SEND_TYPE:
            result = mq_receive(send_mq, (char *)&task, MSG_LEN, NULL);
            break;
        case RECV_TYPE:
            result = mq_receive(recv_mq, (char *)&task, MSG_LEN, NULL);
            break;
        default:
            result = -2;
            break;
        }
        if (result == -2)
        {
            log_error("No such task type %d", *type_p);
            break;
        }
        if (result < 0)
        {
            perror("mq_recv:");
            continue;
        }

        log_trace("Received task %p", task.callback);
        task.callback(task.argv);
        log_trace("Task %p executed", task.callback);
    }

    return NULL;
}

int app_task_init()
{
    log_debug("Task Manager Initializing...");
    mq_unlink(SEND_MQ);
    mq_unlink(RECV_MQ);
    // 申请消息队列
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_msgsize = MSG_LEN;
    attr.mq_maxmsg = 1000;
    attr.mq_curmsgs = 0;
    send_mq = mq_open(SEND_MQ, O_RDWR | O_CREAT, 0666, &attr);
    if (send_mq < 0)
    {
        perror("sendmq:");
        return -1;
    }
    log_trace("Send task queue id %d created", send_mq);

    recv_mq = mq_open(RECV_MQ, O_RDWR | O_CREAT, 0666, &attr);
    if (recv_mq < 0)
    {
        perror("recvmq:");
        mq_close(send_mq);
        mq_unlink(SEND_MQ);
        return -1;
    }

    log_trace("Recv task queue id %d created", recv_mq);

    // 启动两个后台线程执行任务
    if (pthread_create(&send_thread, NULL, app_task_executor, &send_type) < 0)
    {
        return -1;
    }
    if (pthread_create(&recv_thread, NULL, app_task_executor, &recv_type) < 0)
    {
        pthread_cancel(send_thread);
        pthread_join(send_thread, NULL);
        return -1;
    }

    return 0;
}

int app_task_registerTask(TaskType type, void (*callback)(void *), void *argv)
{
    log_trace("Regitering task %p", callback);
    Task task;
    int result = -1;
    task.callback = callback;
    task.argv = argv;
    switch (type)
    {
    case SEND_TYPE:
        result = mq_send(send_mq, (const char *)&task, MSG_LEN, 0);
        break;
    case RECV_TYPE:
        result = mq_send(recv_mq, (const char *)&task, MSG_LEN, 0);
        break;
    default:
        break;
    }

    return result;
}

void app_task_wait()
{
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);
}

int app_task_close()
{
    log_info("Task manager closing...");
    // 首先关闭两个后台线程
    if (send_thread)
    {
        pthread_cancel(send_thread);
    }
    if (recv_thread)
    {
        pthread_cancel(recv_thread);
    }

    app_task_wait();

    // 关闭消息队列
    if (send_mq > 0)
    {
        mq_close(send_mq);
    }

    if (recv_mq < 0)
    {
        mq_close(recv_mq);
    }

    // 删除文件
    mq_unlink(SEND_MQ);
    mq_unlink(RECV_MQ);

    return 0;
}