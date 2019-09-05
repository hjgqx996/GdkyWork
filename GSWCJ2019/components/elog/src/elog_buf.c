/*
 * Function: Logs buffered output.
 * Created on: 2016-11-09
 */

#include <elog.h>
#include <string.h>

#ifdef ELOG_BUF_OUTPUT_ENABLE
#if !defined(ELOG_BUF_OUTPUT_BUF_SIZE)
    #error "Please configure buffer size for buffered output mode (in elog_cfg.h)"
#endif

/* buffered output mode's buffer */
static char log_buf[ELOG_BUF_OUTPUT_BUF_SIZE] = { 0 };
/* log buffer current write size */
static size_t buf_write_size = 0;
/* buffered output mode enabled flag */
static bool is_enabled = false;

extern void elog_port_output(const char *log, size_t size);
extern void elog_output_lock(void);
extern void elog_output_unlock(void);

/**
 * output buffered logs when buffer is full
 *
 * @param log will be buffered line's log
 * @param size log size
 */
void elog_buf_output(const char *log, size_t size) {
    size_t write_size = 0, write_index = 0;

    if (!is_enabled) {
        elog_port_output(log, size);
        return;
    }

    while (true) {
        if (buf_write_size + size > ELOG_BUF_OUTPUT_BUF_SIZE) {
            write_size = ELOG_BUF_OUTPUT_BUF_SIZE - buf_write_size;
            memcpy(log_buf + buf_write_size, log + write_index, write_size);
            write_index += write_size;
            size -= write_size;
            buf_write_size += write_size;
            /* output log */
            elog_port_output(log_buf, buf_write_size);
            /* reset write index */
            buf_write_size = 0;
        } else {
            memcpy(log_buf + buf_write_size, log + write_index, size);
            buf_write_size += size;
            break;
        }
    }
}

/**
 * flush all buffered logs to output device
 */
void elog_flush(void) {
    /* lock output */
    elog_output_lock();
    /* output log */
    elog_port_output(log_buf, buf_write_size);
    /* reset write index */
    buf_write_size = 0;
    /* unlock output */
    elog_output_unlock();
}

/**
 * enable or disable buffered output mode
 * the log will be output directly when mode is disabled
 *
 * @param enabled true: enabled, false: disabled
 */
void elog_buf_enabled(bool enabled) {
    is_enabled = enabled;
}
#endif /* ELOG_BUF_OUTPUT_ENABLE */
