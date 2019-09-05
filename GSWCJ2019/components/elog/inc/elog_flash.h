/*
 * Function: It is an head file for flash log plugin. You can see all be called functions.
 * Created on: 2015-06-05
 */

#ifndef __ELOG_FLASH_H__
#define __ELOG_FLASH_H__

#include <elog.h>
#include <elog_flash_cfg.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(ELOG_FLASH_BUF_SIZE)
    #error "Please configure RAM buffer size (in elog_flash_cfg.h)"
#endif

/* Elog flash log plugin's software version number */
#define ELOG_FLASH_SW_VERSION                "V2.0.1"

/* elog_flash.c */
ElogErrCode elog_flash_init(void);
void elog_flash_output(size_t pos, size_t size);
void elog_flash_output_all(void);
void elog_flash_output_recent(size_t size);
void elog_flash_set_filter(uint8_t level,const char *tag,const char *keyword);
void elog_flash_write(const char *log, size_t size);
void elog_flash_clean(void);
void elog_flash_lock_enabled(bool enabled);

#ifdef ELOG_FLASH_USING_BUF_MODE
void elog_flash_flush(void);
#endif

/* elog_flash_port.c */
ElogErrCode elog_flash_port_init(void);
void elog_flash_port_output(const char *log, size_t size);
void elog_flash_port_lock(void);
void elog_flash_port_unlock(void);

#ifdef __cplusplus
}
#endif

#endif /* __ELOG_FLASH_H__ */
