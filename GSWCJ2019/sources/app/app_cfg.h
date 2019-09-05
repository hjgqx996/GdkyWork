/**
  ******************************************************************************
  *               Copyright(C) 2017-2027 GDKY  All Rights Reserved
  *
  * @file    app_cfg.h
  * @author  ZouZH
  * @version V1.00
  * @date    29-Nov-2017
  * @brief   application configuration for user
  ******************************************************************************
  */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __APP_CFG_H
#define __APP_CFG_H

#ifdef __cplusplus
 extern "C" {
#endif  /* __cplusplus */

/* INCLUDES ----------------------------------------------------------------- */

/* TYPEDEFS ----------------------------------------------------------------- */

/* MACROS  ------------------------------------------------------------------ */

#define CAP_VOLTAGE_ADC_CYCLE     60000 //开关型室温采集器法拉电容电压检测周期,60s,
#define VOLT_LEVEL_POWER_OFF      2700  //电源电压阈值，如果低于阈值，则模块断电,单位：0.001v
#define VOLT_LEVEL_SEND           3300  //电源电压阈值，如果低于阈值，则不发送数据,单位：0.001v
#define VOLT_LEVEL_MODULE_INIT    4500  //NB模块可以初始化时法拉电容的最小电压,单位：0.001v
   
/********************************配置信息*************************************************************/
#define DEBUG                     0   //0、正式程序    1、调试模式，序列号不同    2、调试模式EEPROM存储LOG
   
#define DEVICE_TYPE               3   // 设备类型  1、插座   2、锂电池（碱性电池外壳）    3、开关型（注意：开关型室温采集屏幕分两种，需要根据实际情况更改屏幕的配置）

//GPRS模块型号 1、MG2639   2、MG8332   3、SIM7600   4、NB UDP协议 BC95、ME5310 ML5515  
// 5、U9507W   6、EC20   7、ME3630  8、NB CoAP协议BC95、BC35   10、NB CoAP协议NB05、WT208   
//11、有方N21 UDP  12、骐俊ML5515   13、NB UDP协议NB05、NB86  14、NB OneNet NB86-G   15、NB OneNet M5310
#define GPRS_MODEM_TYPE           10  
#define TEMPERATURE_SENSOR_TYPE   2   //温度传感器型号  1、18B20    2、HDC1080
  
//用户协议  1、2017.8月份以前FE开头的协议  2、2015安阳定制MODBUS协议   
//          3、20170922工大科雅整合后协议  4、NB网络CoAP协议20170922工大科雅整合后协议    
//          5、NB网络OneNet协议20170922工大科雅整合后协议
#define USER_AGREEMENT            3    
   
//网络类型  1、移动    2、联通     3、电信    //此配置只有利尔达NB模块、和骐俊NB模块COAP工作方式有效。
#define NETWORK_TYPE              3   
#define NB_COAP_MODEM_POWER_OFF   1   //NB模块CoAP通信时，发送完数据是否断电。    0、不断电   1、断电
   
#if(DEVICE_TYPE == 1)                 // 设备类型  1、插座   2、锂电池
  #define DISPLAY_TYPE            1   // 显示类型  1、插座dg14093液晶    2、锂电池zx21455a液晶    
#elif(DEVICE_TYPE == 2)
  #define DISPLAY_TYPE            2   // 显示类型  1、插座dg14093液晶    2、锂电池zx21455a液晶  
#elif(DEVICE_TYPE == 3)
  #define DISPLAY_TYPE            3   // 显示类型  1、插座dg14093液晶    2、锂电池zx21455a液晶     
                                      //3、机械开关型CLD1712SYU     4、继电器开关型GTK-B21-P
#endif
   
   
#define NON_HEATING_SEASON_TX_CYCLE  360     //非采暖季数据发送周期  360分钟
#define NON_HEATING_SEASON_TEMP_NUM  6        //非采暖季上传温度个数

   
#if((GPRS_MODEM_TYPE == 4) || (GPRS_MODEM_TYPE == 8) || (GPRS_MODEM_TYPE == 10) ||(GPRS_MODEM_TYPE == 11) || (GPRS_MODEM_TYPE == 12) || (GPRS_MODEM_TYPE == 13)||(GPRS_MODEM_TYPE == 14) || (GPRS_MODEM_TYPE == 15) )  //如果为NB模块
  #define NB_MODEM   1                            //NB模块
  #define GPRS_DATA_ESCAPE     0                  //GPRS数据是否转义  0不转义   1转义
  #define INIT_TX_TIME         60                 //温度上传周期
  #define TEMP_NUM             3                  //上传温度个数 
#else
  #define NB_MODEM   0                            //非NB模块
  #define GPRS_DATA_ESCAPE     1                  //GPRS数据是否转义  0不转义   1转义
  #define INIT_TX_TIME         60                 //温度上传周期
  #define TEMP_NUM             3                  //上传温度个数  
#endif
   
/*********************************************************************************************/ 
   
   
/********************************默认网络参数配置*********************************************/ 
   
#define INIT_IP              "117.60.157.137"   //IP  电信正式平台117.60.157.137 ;电信测试平台180.101.147.115  
                                                //IP  联通OC平台58.240.96.46
                                                //IP  移动oneNet平台183.230.40.40
   
#define INIT_PORT            "5683"                   //端口  5683  32100
#define INIT_TELNUM          "13888888888"            //电话号码

#define INIT_APN             ""                       //APN
#define INIT_USER            ""                       //APN用户名
#define INIT_PWD             ""                       //APN密码

#define DIS_TEMP             0                        //是否显示温度    0、显示    1、不显示
#define DOWN_DATA_TIME       0                        //默认每天0点下发数据
#define SIGNAL_THRESHOLD     32                       //信号阈值（高于此值则不重复发送数据）16进制数据


/*********************************************************************************************/ 
   

/* CONSTANTS  --------------------------------------------------------------- */

/**
 * device address and software version bytes
 */
#define APP_DEV_ADDR_SIZE         4
#define APP_SOFT_VER_SIZE         2
#define APP_PROTOCOL_VER_SIZE     6
#define   VersNum 0x0203////????
/**
 * define device address
 */
#define APP_DEV_ADDR_0            0xFF  /* highest */
#define APP_DEV_ADDR_1            0xFF
#define APP_DEV_ADDR_2            0xFF
#define APP_DEV_ADDR_3            0xFF  /* lowest */
#define APP_DEV_ADDR_ALL          0xFFFFFFFF

/**
 * define software version
 */
#define APP_VER_TYPE              0x01  /* product type */
#define APP_VER_FUNC              0x01  /* product function */
#define APP_VER_DEV               0x01
#define APP_VER_CODE              0x03  /* software update */
   
/**
 * 协议版本号定义
 * 其中PROTOCOL_VER_NUM_ADDR3，表示运营商、模组厂商 ，具体如下：
 * (1)11利尔达模组,Coap电信运营商 ;13利尔达模组,Coap联通运营商；14利尔达模组,Coap移动运营商，15利尔达模组,UDP协议;
 * (2)16移远模组,Coap电信运营商  ;17移远模组,Coap联通运营商  ；18移远模组,Coap移动运营商 ；19移远模组,UDP协议;
 * (3)20骐俊模组,Coap电信运营商 ;21骐俊模组,Coap联通运营商；22骐俊模组,Coap移动运营商 ；23骐俊模组,UDP协议 
 */
#define  PROTOCOL_VER_NUM_ADDR0        0x17
#define  PROTOCOL_VER_NUM_ADDR1        0x09
#define  PROTOCOL_VER_NUM_ADDR2        0x22
#define  PROTOCOL_VER_NUM_ADDR3        0x16
#define  PROTOCOL_VER_NUM_ADDR4        0x00
#define  PROTOCOL_VER_NUM_ADDR5        0x85


/* GLOBAL VARIABLES --------------------------------------------------------- */

/* GLOBAL FUNCTIONS --------------------------------------------------------- */

/* LOCAL VARIABLES ---------------------------------------------------------- */

/* LOCAL FUNCTIONS ---------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __APP_CFG_H */

/***************************** END OF FILE ************************************/


