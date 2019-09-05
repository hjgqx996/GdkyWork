/*
 * Function: Portable interface for FreeRTOS.
 * Created on: 2018-07-11
 */

#include <elog.h>

#include <stdio.h>
#include "SEGGER_RTT.h"


#ifdef ELOG_OS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static SemaphoreHandle_t output_lock;
#endif

#ifdef ELOG_ASYNC_OUTPUT_ENABLE

#define  ELOG_TASK_STK (configMINIMAL_STACK_SIZE * 2)

static SemaphoreHandle_t output_notice;


static void async_output(void *arg);
#endif

/**
 * Elog port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    // Jlink-RTT
    if (SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP) < 0) {
        result = ELOG_ERR_IO;
    }

    // elog lock
#ifdef ELOG_OS
    output_lock = xSemaphoreCreateMutex();
    if (NULL == output_lock) {
        result = ELOG_ERR_NORAM;
    }
#endif
    
#ifdef ELOG_ASYNC_OUTPUT_ENABLE
    TaskHandle_t async_thread = NULL;
    output_notice = xSemaphoreCreateBinary();
    if (NULL == output_notice) {
        result = ELOG_ERR_NORAM;
    }

    xTaskCreate(async_output, "elog_tsk", ELOG_TASK_STK, NULL, configMAX_PRIORITIES - 1, &async_thread);
    if (NULL == async_thread) {
        result = ELOG_ERR_NORAM;
    }
#endif

    return result;
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) {
    /* output to terminal */
    SEGGER_RTT_Write(0, log, size);

    /* output to flash */
    //elog_flash_write(log, size);
}

/**
 * output lock
 */
void elog_port_output_lock(void) {
#ifdef ELOG_OS
    xSemaphoreTake(output_lock, portMAX_DELAY);
#endif
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
#ifdef ELOG_OS
    xSemaphoreGive(output_lock);
#endif
}

/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {
    #include "systime.h"

    static char cur_system_time[9] = { 0 };
    //snprintf(cur_system_time, 12, "%010d", xTaskGetTickCount());
    snprintf(cur_system_time, 9, "%02X:%02X:%02X", g_tSysTime.hour, g_tSysTime.minute, g_tSysTime.second);
    return cur_system_time;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    return "";
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
#ifdef ELOG_OS
    return pcTaskGetName(xTaskGetCurrentTaskHandle());
#else
    return "";
#endif
}

#ifdef ELOG_ASYNC_OUTPUT_ENABLE
void elog_async_output_notice(void) {
    xSemaphoreGive(output_notice);
}

static void async_output(void *arg) {
    size_t get_log_size = 0;
    static char poll_get_buf[ELOG_LINE_BUF_SIZE - 4];

    while(true) {
        /* waiting log */
        xSemaphoreTake(output_notice, portMAX_DELAY);
        /* polling gets and outputs the log */
        while(true) {

#ifdef ELOG_ASYNC_LINE_OUTPUT
            get_log_size = elog_async_get_line_log(poll_get_buf, sizeof(poll_get_buf));
#else
            get_log_size = elog_async_get_log(poll_get_buf, sizeof(poll_get_buf));
#endif

            if (get_log_size) {
                elog_port_output(poll_get_buf, get_log_size);
            } else {
                break;
            }
        }
    }
}
#endif
