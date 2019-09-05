/*
 * Function: Portable interface for Elog's flash log pulgin.
 * Created on: 2015-07-28
 */

#include "elog_flash.h"

/**
 * Elog flash log pulgin port initialize
 *
 * @return result
 */
ElogErrCode elog_flash_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    /* add your code here */

    return result;
}

/**
 * output flash saved log port interface
 *
 * @param log flash saved log
 * @param size log size
 */
void elog_flash_port_output(const char *log, size_t size) {

    /* add your code here */

}

/**
 * flash log lock
 */
void elog_flash_port_lock(void) {

    /* add your code here */

}

/**
 * flash log unlock
 */
void elog_flash_port_unlock(void) {

    /* add your code here */

}