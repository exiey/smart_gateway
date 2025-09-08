#include "/home/xhy/getway/utils/logc/log.h"

int main(int argc, char const *argv[])
{
    log_set_level(LOG_TRACE);
    log_trace("Test trace");
    log_debug("Test debug");
    log_info("Test info");
    log_warn("Test warn");
    log_error("Test error");
    log_fatal("Test fatal");
}