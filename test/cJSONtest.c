#include "cJSON/cJSON.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{
    char str[100];
    scanf("%s", str);

    cJSON *item = cJSON_Parse(str);
    if (!item)
    {
        perror("Json parse failed");
    }
    // 给JSON Object添加自定义属性
    cJSON_AddNumberToObject(item, "prop-01", 12);
    char * output = cJSON_PrintUnformatted(item);
    
    // 使用完毕的JSON Object一定要释放
    cJSON_Delete(item);
    printf("result: %s\n", output);

    // 就是标准库的free
    cJSON_free(output);
}