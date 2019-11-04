/*
*********************************************************************************************************
*
*	模块名称 : STM32内部RTC模块
*	文件名称 : bsp_cpu_rtc.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2015-08-08 armfly  首版.安富莱电子原创
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

#define RTC_INIT_FLAG	0xA5A8

#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00FF

RTC_T g_tRTC;
RTC_T send_tRTC;
/* 平年的每月天数表 */
const uint8_t mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static void RTC_Config(void);

#define SECONDS_IN_A_DAY (86400U)
#define SECONDS_IN_A_HOUR (3600U)
#define SECONDS_IN_A_MINUTE (60U)
#define DAYS_IN_A_YEAR (365U)
#define YEAR_RANGE_START (1970U)
#define YEAR_RANGE_END (2099U)

RTC_HandleTypeDef RtcHandle;	/* 必须用全局变量 */

/*
*********************************************************************************************************
*	函 数 名: bsp_InitRTC
*	功能说明: 初始化CPU内部RTC
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitRTC(void)
{	
	uint32_t back_reg;

	RtcHandle.Instance = RTC;	
	
	/* 用于检测是否已经配置过RTC，如果配置过的话，会在配置结束时 
	设置RTC备份寄存器为0xA5A5 。如果检测RTC备份寄存器不是0xA5A5   那么表示没有配置过，需要配置RTC.   */
	back_reg = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1); 
	if (back_reg != RTC_INIT_FLAG) 
	{ 		
		RTC_Config(); 	/* RTC 配置 */ 
		
		RTC_WriteClock(2019, 7, 19, 0, 2, 0);	/* 设置初始时间 */
		
		/* 配置备份寄存器，表示已经设置过RTC */
		HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, RTC_INIT_FLAG);		
	} 
	else 
	{ 
		/* 检测上电复位标志是否设置 */ 
//		if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != 0)
//		{ 
//			/* 发生上电复位 */ 
//		} 
		/* 检测引脚复位标志是否设置 */ 
//		else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != 0) 
//		{ 
//			/* 发生引脚复位 */  
//		} 
		
		RTC_Config(); 	/* RTC 配置 */ 
	}
}

/*
*********************************************************************************************************
*	函 数 名: RTC_Config
*	功能说明: 1. 选择不同的RTC时钟源LSI或者LSE。
*             2. 配置RTC时钟。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void RTC_Config(void)
{
	RCC_OscInitTypeDef        RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

	/* To enable access on RTC registers */
	HAL_PWR_EnableBkUpAccess();
	
	/* Configure LSE/LSI as RTC clock source ###############################*/
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{ 
		Error_Handler();
	}

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{ 
		Error_Handler();
	}
	
	/* Configures the External Low Speed oscillator (LSE) drive capability */
	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);	
	
	/* Enable RTC Clock */ 
	__HAL_RCC_RTC_ENABLE(); 
	
	 /*##-3- Configure the NVIC for RTC Alarm ###################################*/
  HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0x0, 0);
  HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
  
	/* Configure RTC prescaler and RTC data registers */	
	{
	 
		RtcHandle.Instance = RTC;

		/* RTC configured as follows:
		  - Hour Format    = Format 24
		  - Asynch Prediv  = Value according to source clock
		  - Synch Prediv   = Value according to source clock
		  - OutPut         = Output Disable
		  - OutPutPolarity = High Polarity
		  - OutPutType     = Open Drain */
		RtcHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
		RtcHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
		RtcHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
		RtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
		RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
		RtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
		if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
		{
			/* Initialization Error */
			Error_Handler();
		}	
	}
	
}

/*
*********************************************************************************************************
*	函 数 名: RTC_WriteClock
*	功能说明: 设置RTC时钟
*	形    参：无
*	返 回 值: 1表示成功 0表示错误
*********************************************************************************************************
*/
uint8_t RTC_WriteClock(uint16_t _year, uint8_t _mon, uint8_t _day, uint8_t _hour, uint8_t _min, uint8_t _sec)
{
	RTC_DateTypeDef  date;
	RTC_TimeTypeDef  time;

	RtcHandle.Instance = RTC;
	
	/* 设置年月日和星期 */
	date.Year = _year - 2000;
	date.Month = _mon;
	date.Date = _day;
	date.WeekDay = RTC_CalcWeek(_year, _mon, _day);	/* 周5=5， 周日=7 */
	if (HAL_RTC_SetDate(&RtcHandle, &date, FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	} 

	/* 设置时分秒，以及显示格式 */
	time.Hours   = _hour;
	time.Minutes = _min;
	time.Seconds = _sec; 
	time.TimeFormat = RTC_HOURFORMAT12_AM;
	time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
	time.StoreOperation = RTC_STOREOPERATION_RESET;	
	if(HAL_RTC_SetTime(&RtcHandle, &time, FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}  

	return 1;      
}

/*
*********************************************************************************************************
*	函 数 名: RTC_ReadClock
*	功能说明: 得到当前时钟。结果存放在 g_tRTC。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void RTC_ReadClock(void)
{
	RTC_DateTypeDef  date;
	RTC_TimeTypeDef  time;

	RtcHandle.Instance = RTC;
	
	/* CPU BUG: 必须先读取时间，再读取日期 */
	if (HAL_RTC_GetTime(&RtcHandle, &time, FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	} 	
	
	if (HAL_RTC_GetDate(&RtcHandle, &date, FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	} 
	
	g_tRTC.Year = date.Year + 2000;
	g_tRTC.Mon = date.Month;
	g_tRTC.Day = date.Date;
	
	g_tRTC.Hour = time.Hours;	/* 小时 */
	g_tRTC.Min = time.Minutes; 	/* 分钟 */
	g_tRTC.Sec = time.Seconds;	/* 秒 */

	g_tRTC.Week = RTC_CalcWeek(g_tRTC.Year, g_tRTC.Mon, g_tRTC.Day);	/* 计算星期 */
}   


/*
*********************************************************************************************************
*	函 数 名: bsp_CalcWeek
*	功能说明: 根据日期计算星期几
*	形    参: _year _mon _day  年月日  (年是2字节整数，月和日是字节整数）
*	返 回 值: 周几 （1-7） 7表示周日
*********************************************************************************************************
*/
uint8_t RTC_CalcWeek(uint16_t _year, uint8_t _mon, uint8_t _day)
{
	/*
	蔡勒（Zeller）公式
		历史上的某一天是星期几？未来的某一天是星期几？关于这个问题，有很多计算公式（两个通用计算公式和
	一些分段计算公式），其中最著名的是蔡勒（Zeller）公式。
	    即w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1

		公式中的符号含义如下，
	     w：星期；
	     c：年的高2位，即世纪-1
	     y：年（两位数）；
	     m：月（m大于等于3，小于等于14，即在蔡勒公式中，某年的1、2月要看作上一年的13、14月来计算，
	  	    比如2003年1月1日要看作2002年的13月1日来计算）；
	     d：日；
	     [ ]代表取整，即只要整数部分。

	    算出来的W除以7，余数是几就是星期几。如果余数是0，则为星期日。
        如果结果是负数，负数求余数则需要特殊处理：
            负数不能按习惯的余数的概念求余数，只能按数论中的余数的定义求余。为了方便
        计算，我们可以给它加上一个7的整数倍，使它变为一个正数，然后再求余数

		以2049年10月1日（100周年国庆）为例，用蔡勒（Zeller）公式进行计算，过程如下：
		蔡勒（Zeller）公式：w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1
		=49+[49/4]+[20/4]-2×20+[26× (10+1)/10]+1-1
		=49+[12.25]+5-40+[28.6]
		=49+12+5-40+28
		=54 (除以7余5)
		即2049年10月1日（100周年国庆）是星期5。
	*/
	uint8_t y, c, m, d;
	int16_t w;

	if (_mon >= 3)
	{
		m = _mon;
		y = _year % 100;
		c = _year / 100;
		d = _day;
	}
	else	/* 某年的1、2月要看作上一年的13、14月来计算 */
	{
		m = _mon + 12;
		y = (_year - 1) % 100;
		c = (_year - 1) / 100;
		d = _day;
	}

	w = y + y / 4 +  c / 4 - 2 * c + ((uint16_t)26*(m+1))/10 + d - 1;
	if (w == 0)
	{
		w = 7;	/* 表示周日 */
	}
	else if (w < 0)	/* 如果w是负数，则计算余数方式不同 */
	{
		w = 7 - (-w) % 7;
	}
	else
	{
		w = w % 7;
	}
	
	/* 2018-10-20 else的情况里面，会有情况把w计算成0的 */
	if (w == 0)
	{
		w = 7;	/* 表示周日 */
	}
	
	return w;
}

static bool RTC_CheckDatetimeFormat(const RTC_T *datetime)
{
//    assert(datetime);

    /* Table of days in a Mon for a non leap Year. First entry in the table is not used,
     * valid months start from 1
     */
    uint8_t daysPerMonth[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

    /* Check Year, Mon, Hour, Min, seconds */
    if ((datetime->Year < YEAR_RANGE_START) || (datetime->Year > YEAR_RANGE_END) || (datetime->Mon > 12U) ||
        (datetime->Mon < 1U) || (datetime->Hour >= 24U) || (datetime->Min >= 60U) || (datetime->Sec >= 60U))
    {
        /* If not correct then error*/
        return false;
    }

    /* Adjust the days in February for a leap Year */
    if ((((datetime->Year & 3U) == 0) && (datetime->Year % 100 != 0)) || (datetime->Year % 400 == 0))
    {
        daysPerMonth[2] = 29U;
    }

    /* Check the validity of the Day */
    if ((datetime->Day > daysPerMonth[datetime->Mon]) || (datetime->Day < 1U))
    {
        return false;
    }

    return true;
}


uint32_t RTC_ConvertDatetimeToSeconds(const RTC_T *datetime)
{
//    assert(datetime);

    /* Number of days from begin of the non Leap-Year*/
    /* Number of days from begin of the non Leap-Year*/
    uint16_t monthDays[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};
    uint32_t seconds;

    /* Compute number of days from 1970 till given Year*/
    seconds = (datetime->Year - 1970U) * DAYS_IN_A_YEAR;
    /* Add leap Year days */
    seconds += ((datetime->Year / 4) - (1970U / 4));
    /* Add number of days till given Mon*/
    seconds += monthDays[datetime->Mon];
    /* Add days in given Mon. We subtract the current Day as it is
     * represented in the hours, minutes and seconds field*/
    seconds += (datetime->Day - 1);
    /* For leap Year if Mon less than or equal to Febraury, decrement Day counter*/
    if ((!(datetime->Year & 3U)) && (datetime->Mon <= 2U))
    {
        seconds--;
    }

    seconds = (seconds * SECONDS_IN_A_DAY) + (datetime->Hour * SECONDS_IN_A_HOUR) +
              (datetime->Min * SECONDS_IN_A_MINUTE) + datetime->Sec;

    return seconds;
}

void RTC_ConvertSecondsToDatetime(uint32_t seconds, RTC_T *datetime)
{
//    assert(datetime);

    uint32_t x;
    uint32_t secondsRemaining, days;
    uint16_t daysInYear;
    /* Table of days in a Mon for a non leap Year. First entry in the table is not used,
     * valid months start from 1
     */
    uint8_t daysPerMonth[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

    /* Start with the seconds value that is passed in to be converted to date time format */
    secondsRemaining = seconds;

    /* Calcuate the number of days, we add 1 for the current Day which is represented in the
     * hours and seconds field
     */
    days = secondsRemaining / SECONDS_IN_A_DAY + 1;

    /* Update seconds left*/
    secondsRemaining = secondsRemaining % SECONDS_IN_A_DAY;

    /* Calculate the datetime Hour, Min and Sec fields */
    datetime->Hour = secondsRemaining / SECONDS_IN_A_HOUR;
    secondsRemaining = secondsRemaining % SECONDS_IN_A_HOUR;
    datetime->Min = secondsRemaining / 60U;
    datetime->Sec = secondsRemaining % SECONDS_IN_A_MINUTE;

    /* Calculate Year */
    daysInYear = DAYS_IN_A_YEAR;
    datetime->Year = YEAR_RANGE_START;
    while (days > daysInYear)
    {
        /* Decrease Day count by a Year and increment Year by 1 */
        days -= daysInYear;
        datetime->Year++;

        /* Adjust the number of days for a leap Year */
        if (datetime->Year & 3U)
        {
            daysInYear = DAYS_IN_A_YEAR;
        }
        else
        {
            daysInYear = DAYS_IN_A_YEAR + 1;
        }
    }

    /* Adjust the days in February for a leap Year */
    if (!(datetime->Year & 3U))
    {
        daysPerMonth[2] = 29U;
    }

    for (x = 1U; x <= 12U; x++)
    {
        if (days <= daysPerMonth[x])
        {
            datetime->Mon = x;
            break;
        }
        else
        {
            days -= daysPerMonth[x];
        }
    }

    datetime->Day = days;
}

void RTC_WKUP_IRQHandler(void)
{
  HAL_RTCEx_WakeUpTimerIRQHandler(&RtcHandle);
}


/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
