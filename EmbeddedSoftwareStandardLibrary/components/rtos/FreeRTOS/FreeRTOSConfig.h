#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/


#define configCPU_CLOCK_HZ                          ( SystemCoreClock )
#define configTICK_RATE_HZ                          ( ( TickType_t ) 100 )
#define configUSE_PREEMPTION                        1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION     1
#define configMAX_PRIORITIES                        ( 3 )
#define configMINIMAL_STACK_SIZE                    ( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE                       ( ( size_t ) ( 16 * 1024 ) )
#define configMAX_TASK_NAME_LEN                     ( 16 )
#define configUSE_16_BIT_TICKS                      0
#define configIDLE_SHOULD_YIELD                     1

#define configUSE_MUTEXES                           1
#define configUSE_RECURSIVE_MUTEXES                 0
#define configUSE_COUNTING_SEMAPHORES               1
#define configENABLE_BACKWARD_COMPATIBILITY         0


/* Hook function related definitions. */
#define configUSE_IDLE_HOOK                         1
#define configUSE_TICK_HOOK                         1
#define configCHECK_FOR_STACK_OVERFLOW              2
#define configUSE_MALLOC_FAILED_HOOK                0

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS               0
#define configUSE_TRACE_FACILITY                    0
#define configUSE_STATS_FORMATTING_FUNCTIONS        0
#define configQUEUE_REGISTRY_SIZE                   10

/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES                       0
#define configMAX_CO_ROUTINE_PRIORITIES             1

/* Software timer related definitions. */
#define configUSE_TIMERS                            0
#define configTIMER_TASK_PRIORITY                   ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH                    10
#define configTIMER_TASK_STACK_DEPTH                ( configMINIMAL_STACK_SIZE * 2 )

/* Low power related definitions. */
#define configUSE_TICKLESS_IDLE                     1


/* FreeRTOS MPU specific definitions. */
#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS 0

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           0
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          0
#define INCLUDE_xTaskGetCurrentTaskHandle       0
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_xTaskGetIdleTaskHandle          0
#define INCLUDE_xTimerGetTimerDaemonTaskHandle  0
#define INCLUDE_pcTaskGetTaskName               0
#define INCLUDE_eTaskGetState                   0
#define INCLUDE_xEventGroupSetBitFromISR        0
#define INCLUDE_xTimerPendFunctionCall          0


#if configUSE_TICKLESS_IDLE == 1
/* configPRE_SLEEP_PROCESSING() and configPOST_SLEEP_PROCESSING() macros, which are
   called pre and post the low power SLEEP mode being entered and exited.  These
   macros can be used to turn turn off and on IO, clocks, the Flash etc. to obtain
   the lowest power possible while the tick is off.  */
  void vApplicationPreStopHook( void );
  void vApplicationPostStopHook( void );
  #define configPRE_STOP_PROCESSING()               vApplicationPreStopHook()
  #define configPOST_STOP_PROCESSING()              vApplicationPostStopHook()
  
  #define configEXPECTED_IDLE_TIME_BEFORE_SLEEP     ( 1 + 1 )
  #define configOVERRIDE_DEFAULT_TICK_CONFIGURATION 1

  #define configPRE_SLEEP_PROCESSING( x )           (x=0)
  //#define configPOST_SLEEP_PROCESSING( x )          printf("Sleep End...\r\n")
#endif /* configUSE_TICKLESS_IDLE */


/* Include the FreeRTOS+Trace FreeRTOS trace macro definitions. */
#if ( configUSE_TRACE_FACILITY == 1 )
  #include "trcRecorder.h"
#endif /* configUSE_TRACE_FACILITY */


/* Define to trap errors during development. */
#ifdef USE_FULL_ASSERT
  void assert_failed(const char* file, uint32_t line);
  #define configASSERT( x )     if( ( x ) == 0 ) assert_failed( __FILE__, __LINE__ )
#endif /* USE_FULL_ASSERT */



/* Use the system definition, if there is one */
#ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS       __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS       4        /* 15 priority levels */
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5

/* The lowest priority. */
#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Priority 5, or 95 as only the top four bits are implemented. */
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler
extern uint32_t SystemCoreClock;

#endif /* FREERTOS_CONFIG_H */

