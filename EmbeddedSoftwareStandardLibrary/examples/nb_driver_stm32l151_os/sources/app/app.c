/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 ZouZH  All Rights Reserved
  *
  * @file    app.c
  * @author  ZouZH
  * @version V1.00
  * @date    29-Nov-2017
  * @brief   application for nb-iot collector
  ******************************************************************************
  */
#define LOG_TAG    "APP"

/* INCLUDES ----------------------------------------------------------------- */
#include "includes.h"         /* 工程头文件 */

#include "led.h"              /* 指示驱动 */
#include "key.h"              /* 按键驱动 */
#include "pwrmgr.h"           /* 电源管理 */

#include "nb_app.h"           /* NB-IoT */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

/* CONSTANTS  --------------------------------------------------------------- */

/**
 * device address and software version
 */
VAR_AT(0x8008000) const uint8_t APP_DEV_ADDR[APP_DEV_ADDR_SIZE] =
                  {APP_DEV_ADDR_0, APP_DEV_ADDR_1, APP_DEV_ADDR_2, APP_DEV_ADDR_3};
VAR_AT(0x8008008) const uint8_t APP_SOFT_VER[APP_SOFT_VER_SIZE] =
                  {APP_VER_TYPE, APP_VER_FUNC, APP_VER_DEV, APP_VER_CODE};

/**
 * IAP update flag
 */
VAR_AT(0x8008004) const uint32_t APP_IAP_FLAG = 0xAA55AA55;

/* GLOBAL VARIABLES --------------------------------------------------------- */


/**
 * task handle
 */
TaskHandle_t g_hTaskInit;
TaskHandle_t g_hTaskCom;

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* 应用程序运行对象 */
AppRunObj_t s_tAppObj = 
{
  .error = 0,
  .mode = 0,
  .powerOn = 0,
  .rdyAlloc = 1,
  .allocHour = 0,
  .allocMinute = UINT8_LO(APP_DEV_ADDR_3),
  .configTime = 0,
};

/* LOCAL FUNCTIONS ---------------------------------------------------------- */
#if APP_TASK_COM > 0
  static void vAppTaskCom(void *pvParameters);
#endif /* APP_TASK_COM */

void AppLedPoll(void);


/**
 * @brief  hardware initialise
 *
 * @param  none
 *
 * @retval none
 */
static void AppInitHard(void)
{
  // 上电延时
  SYSDelayMs(1000);

  // 指示灯
  LedInit();

  // 配置按键
  KeyInit();

  // 电源管理
  PMInit();

  // 实时时钟
  if (SysTimeInit())
  {
    BIT_SET(s_tAppObj.error, BIT(APP_ERR_CLOCK));
    log_e("Clock init failed!");
  }

  // NB-IoT
  if (NB_OK != nbAppInit())
  {
    BIT_SET(s_tAppObj.error, BIT(APP_ERR_NB));
    log_e("NB init failed!");
  }

  // 显示信息
  AppDispInfo();

  // 底层硬件
  BspInitHard();
}

/**
 * @brief  创建应用程序任务
 *
 * @param  无
 *
 * @retval 无
 */
static void AppCreateTask(void)
{
#if APP_TASK_COM > 0
  xTaskCreate(vAppTaskCom, "ComTask", APP_TASK_STK_COM, NULL, APP_TASK_PRIO_COM, &g_hTaskCom);
#endif /* APP_TASK_COM */
}


/**
 * @brief Init task function
 *
 * @param[in] task parameter pointer
 *
 * @retval none
 */
void vAppTaskInit(void *pvParameters)
{
  (void)pvParameters;

  AppInitHard();

  AppCreateTask();

  log_i("Init task start success!");
  
  while (1)
  {
    // 时间更新
    SysTimeUpdate();
    
    // NB-IoT
    nbAppPoll();

    SYSDelayMs(130);
  }
}

/**
 * @brief  通信检测任务
 *
 * @param  [IN]pvParameters 传给任务的参数
 *
 * @retval 无
 */
#if APP_TASK_COM > 0
static void vAppTaskCom(void *pvParameters)
{
  (void)pvParameters;

  while(1)
  {
    // NB接收处理
    nbUpdate();

    // 状态指示
    AppLedPoll();

    // 重置看门口
    BspReStartIWDG();

    SYSDelayMs(20);
  }
}
#endif /* APP_TASK_COM */



/**
 * @brief  print app info
 *
 * @param  none
 *
 * @retval none
 */
void AppDispInfo(void)
{
  xprintf("\r\n");
  xprintf("Device   %02X%02X%02X%02X\r\n", APP_DEV_ADDR[0], APP_DEV_ADDR[1], APP_DEV_ADDR[2], APP_DEV_ADDR[3]);
  xprintf("Version  %X.%X.%X.%X\r\n", APP_SOFT_VER[0], APP_SOFT_VER[1], APP_SOFT_VER[2], APP_SOFT_VER[3]);
  xprintf("Time     %04X-%02X-%02X %02X:%02X:%02X Week-%d\r\n", g_tSysTime.year, g_tSysTime.month, g_tSysTime.day, \
                                                                g_tSysTime.hour, g_tSysTime.minute, g_tSysTime.second, \
                                                                g_tSysTime.week);
  xprintf("APPErr   %02X\r\n", s_tAppObj.error);

  if (nbAppGetRunState() > NB_State_Idle)
  {
    const NB_t *pnb = nbGetObj();
    xprintf("NBErr    %02X\r\n", nbGetLastError());
    xprintf("NBAPPErr %02X\r\n", nbAppGetLastError());
    xprintf("IMEI     %s\r\n", pnb->imei);
    xprintf("IMSI     %s\r\n", pnb->imsi);
    xprintf("RSSI     %d\r\n", pnb->rssi);
    xprintf("NETSTAT  %d\r\n", pnb->netStat);
    xprintf("IP       %d.%d.%d.%d\r\n", pnb->ipAddr[0], pnb->ipAddr[1], pnb->ipAddr[2], pnb->ipAddr[3]);
    xprintf("CMEE     %d\r\n", pnb->cme);
  }
  
  xprintf("\r\n");
}

/**
 * @brief  指示状态轮询
 *
 * @param  无
 *
 * @retval 无
 */
void AppLedPoll(void)
{
  static uint32_t runtick = 0;
  static uint32_t runperiod = 0;

  static uint32_t nettick = 0;
  static uint32_t netperiod = 0;

  static uint8_t  runoncycle = 0;
  static uint8_t  netoncycle = 0;

  uint32_t curtick = SYSGetTickCount();

  // Run LED Status
  if (s_tAppObj.error)
    runperiod = 200;
  else if (BIT_READ(s_tAppObj.mode, BIT(APP_MODE_CONFIG)))
    runperiod = 500;
  else if (BIT_READ(s_tAppObj.mode, BIT(APP_MODE_READ_UM)))
    runperiod = 1000;
  else if (BIT_READ(s_tAppObj.mode, BIT(APP_MODE_WAIT)))
    runperiod = 2000;
  else
    runperiod = 5000;


  if (runperiod && (curtick - runtick > pdMS_TO_TICKS(runperiod)))
  {
    runoncycle = 0;
    runtick = curtick;
    LedSet(LED_RUN, LED_MODE_ON);
  }

  if (++runoncycle == 2)
    LedSet(LED_RUN, LED_MODE_OFF);

  // Net LED Status
  if (nbAppGetRunState() < NB_State_NetConfig)
    netperiod = 5000;
  else if (nbAppGetRunState() < NB_State_WaitAttach)
    netperiod = 4000;
  else if (nbAppGetRunState() < NB_State_SendData)
    netperiod = 3000;
  else if (nbAppGetRunState() < NB_State_Running)
    netperiod = 2000;
  else if (nbAppGetRunState() < NB_State_Suspend)
    netperiod = 1000;
  else if (nbAppGetRunState() == NB_State_Error)
    netperiod = 200;
  else
    netperiod = 0;

  curtick = SYSGetTickCount();
  if (netperiod && (curtick - nettick > pdMS_TO_TICKS(netperiod)))
  {
    netoncycle = 0;
    nettick = curtick;
    LedSet(LED_NET, LED_MODE_ON);
  }

  if (++netoncycle == 2)
    LedSet(LED_NET, LED_MODE_OFF);
}
