/**
  ******************************************************************************
  *               Copyright(C) 2018-2028 GDKY All Rights Reserved
  *
  * @file     nb_os_freertos.c
  * @author   ZouZH
  * @version  V1.00
  * @date     31-July-2018
  * @brief    NB-IoT os port of freertos.
  ******************************************************************************
  * @history
  */


/* INCLUDES ----------------------------------------------------------------- */
#include "nb_os.h"

#include "includes.h"


/** 
 * @addtogroup NB-IoT
 * @{
 */

/** 
 * @defgroup NB-OS
 * @brief NB-IoT 多任务系统接口
 * @{
 */


/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

#if NB_CFG_OS
static SemaphoreHandle_t st_nbMutex = NULL;
#endif

/* LOCAL FUNCTIONS ---------------------------------------------------------- */

/**
 * @brief  Retrieve system time
 * @param  None
 * @return Number of milliseconds elapsed since the system was last started
 * @retval Milliseconds
 */
uint32_t nbOsTimeGet(void)
{
#if NB_CFG_OS
    uint32_t tick;

    tick = xTaskGetTickCount();

    // Convert system ticks to milliseconds
    return  (tick * (1000u / configTICK_RATE_HZ));
#else
    return 0;
#endif
}


/**
 * @brief  Initialize the mutex
 * @param  None
 * @return Operation result
 * @retval 0 Success
 */
uint8_t nbOsMutexInit(void)
{
#if NB_CFG_OS
    uint8_t ret = 0;

    if(st_nbMutex != NULL)
        return 0;

    st_nbMutex = xSemaphoreCreateBinary();
    if(st_nbMutex == NULL)
    {
        ret = 1;
    }

    return ret;
#else
    return 0;
#endif
}


/**
 * @brief  Uninitialize the mutex
 * @param  None
 * @return Operation result
 * @retval 0 Success
 */
uint8_t nbOsMutexUninit(void)
{
#if NB_CFG_OS
    vSemaphoreDelete(st_nbMutex);
    st_nbMutex = NULL;
#endif
    return 0;
}


/**
 * @brief  Acquire the mutex.
 * @param  timout: Wait time milliseconds
 * @return Operation result
 * @retval 0-Success
 */
uint8_t nbOsMutexAcquire(uint32_t timout)
{
#if NB_CFG_OS
    uint8_t ret = 0;

    if(xSemaphoreTake(st_nbMutex, timout) != pdTRUE)
    {
        ret = 1;
    }

    return ret;
#else
    return 0;
#endif
}

/**
 * @brief  Release the mutex.
 * @param  None
 * @return Operation result
 * @retval 0-Success
 */
void nbOsMutexRelease(void)
{
#if NB_CFG_OS
    xSemaphoreGive(st_nbMutex);
#endif
}


/**
 * @brief      Allocate a memory block
 * @param[in]  size: size Bytes to allocate
 * @return     A pointer to the allocated memory block or NULL if
 *             there is insufficient memory available
 */
void *nbOsMemAlloc(size_t size)
{
   void *p;

   // Allocate a memory block
   p = SYSMalloc(size);

   return p;
}


/**
 * @brief Release a previously allocated memory block
 * @param[in] p Previously allocated memory block to be freed
 **/

/**
 * @brief      Release a previously allocated memory block
 * @param[in]  p: Previously allocated memory block to be freed
 * @return     None
 */
void nbOsMemFree(void *p)
{
   //Make sure the pointer is valid
   if(p != NULL)
   {
      //Free memory block
      SYSFree(p);
   }
}

/**
 * @}
 */

/**
 * @}
 */

