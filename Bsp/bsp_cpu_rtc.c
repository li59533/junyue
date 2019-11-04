/*
*********************************************************************************************************
*
*	ģ������ : STM32�ڲ�RTCģ��
*	�ļ����� : bsp_cpu_rtc.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2015-08-08 armfly  �װ�.����������ԭ��
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

#define RTC_INIT_FLAG	0xA5A8

#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x00FF

RTC_T g_tRTC;
RTC_T send_tRTC;
/* ƽ���ÿ�������� */
const uint8_t mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static void RTC_Config(void);

#define SECONDS_IN_A_DAY (86400U)
#define SECONDS_IN_A_HOUR (3600U)
#define SECONDS_IN_A_MINUTE (60U)
#define DAYS_IN_A_YEAR (365U)
#define YEAR_RANGE_START (1970U)
#define YEAR_RANGE_END (2099U)

RTC_HandleTypeDef RtcHandle;	/* ������ȫ�ֱ��� */

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitRTC
*	����˵��: ��ʼ��CPU�ڲ�RTC
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitRTC(void)
{	
	uint32_t back_reg;

	RtcHandle.Instance = RTC;	
	
	/* ���ڼ���Ƿ��Ѿ����ù�RTC��������ù��Ļ����������ý���ʱ 
	����RTC���ݼĴ���Ϊ0xA5A5 ��������RTC���ݼĴ�������0xA5A5   ��ô��ʾû�����ù�����Ҫ����RTC.   */
	back_reg = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1); 
	if (back_reg != RTC_INIT_FLAG) 
	{ 		
		RTC_Config(); 	/* RTC ���� */ 
		
		RTC_WriteClock(2019, 7, 19, 0, 2, 0);	/* ���ó�ʼʱ�� */
		
		/* ���ñ��ݼĴ�������ʾ�Ѿ����ù�RTC */
		HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, RTC_INIT_FLAG);		
	} 
	else 
	{ 
		/* ����ϵ縴λ��־�Ƿ����� */ 
//		if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != 0)
//		{ 
//			/* �����ϵ縴λ */ 
//		} 
		/* ������Ÿ�λ��־�Ƿ����� */ 
//		else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != 0) 
//		{ 
//			/* �������Ÿ�λ */  
//		} 
		
		RTC_Config(); 	/* RTC ���� */ 
	}
}

/*
*********************************************************************************************************
*	�� �� ��: RTC_Config
*	����˵��: 1. ѡ��ͬ��RTCʱ��ԴLSI����LSE��
*             2. ����RTCʱ�ӡ�
*	��    �Σ���
*	�� �� ֵ: ��
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
*	�� �� ��: RTC_WriteClock
*	����˵��: ����RTCʱ��
*	��    �Σ���
*	�� �� ֵ: 1��ʾ�ɹ� 0��ʾ����
*********************************************************************************************************
*/
uint8_t RTC_WriteClock(uint16_t _year, uint8_t _mon, uint8_t _day, uint8_t _hour, uint8_t _min, uint8_t _sec)
{
	RTC_DateTypeDef  date;
	RTC_TimeTypeDef  time;

	RtcHandle.Instance = RTC;
	
	/* ���������պ����� */
	date.Year = _year - 2000;
	date.Month = _mon;
	date.Date = _day;
	date.WeekDay = RTC_CalcWeek(_year, _mon, _day);	/* ��5=5�� ����=7 */
	if (HAL_RTC_SetDate(&RtcHandle, &date, FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	} 

	/* ����ʱ���룬�Լ���ʾ��ʽ */
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
*	�� �� ��: RTC_ReadClock
*	����˵��: �õ���ǰʱ�ӡ��������� g_tRTC��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RTC_ReadClock(void)
{
	RTC_DateTypeDef  date;
	RTC_TimeTypeDef  time;

	RtcHandle.Instance = RTC;
	
	/* CPU BUG: �����ȶ�ȡʱ�䣬�ٶ�ȡ���� */
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
	
	g_tRTC.Hour = time.Hours;	/* Сʱ */
	g_tRTC.Min = time.Minutes; 	/* ���� */
	g_tRTC.Sec = time.Seconds;	/* �� */

	g_tRTC.Week = RTC_CalcWeek(g_tRTC.Year, g_tRTC.Mon, g_tRTC.Day);	/* �������� */
}   


/*
*********************************************************************************************************
*	�� �� ��: bsp_CalcWeek
*	����˵��: �������ڼ������ڼ�
*	��    ��: _year _mon _day  ������  (����2�ֽ��������º������ֽ�������
*	�� �� ֵ: �ܼ� ��1-7�� 7��ʾ����
*********************************************************************************************************
*/
uint8_t RTC_CalcWeek(uint16_t _year, uint8_t _mon, uint8_t _day)
{
	/*
	���գ�Zeller����ʽ
		��ʷ�ϵ�ĳһ�������ڼ���δ����ĳһ�������ڼ�������������⣬�кܶ���㹫ʽ������ͨ�ü��㹫ʽ��
	һЩ�ֶμ��㹫ʽ�����������������ǲ��գ�Zeller����ʽ��
	    ��w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1

		��ʽ�еķ��ź������£�
	     w�����ڣ�
	     c����ĸ�2λ��������-1
	     y���꣨��λ������
	     m���£�m���ڵ���3��С�ڵ���14�����ڲ��չ�ʽ�У�ĳ���1��2��Ҫ������һ���13��14�������㣬
	  	    ����2003��1��1��Ҫ����2002���13��1�������㣩��
	     d���գ�
	     [ ]����ȡ������ֻҪ�������֡�

	    �������W����7�������Ǽ��������ڼ������������0����Ϊ�����ա�
        �������Ǹ�������������������Ҫ���⴦��
            �������ܰ�ϰ�ߵ������ĸ�����������ֻ�ܰ������е������Ķ������ࡣΪ�˷���
        ���㣬���ǿ��Ը�������һ��7����������ʹ����Ϊһ��������Ȼ����������

		��2049��10��1�գ�100������죩Ϊ�����ò��գ�Zeller����ʽ���м��㣬�������£�
		���գ�Zeller����ʽ��w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1
		=49+[49/4]+[20/4]-2��20+[26�� (10+1)/10]+1-1
		=49+[12.25]+5-40+[28.6]
		=49+12+5-40+28
		=54 (����7��5)
		��2049��10��1�գ�100������죩������5��
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
	else	/* ĳ���1��2��Ҫ������һ���13��14�������� */
	{
		m = _mon + 12;
		y = (_year - 1) % 100;
		c = (_year - 1) / 100;
		d = _day;
	}

	w = y + y / 4 +  c / 4 - 2 * c + ((uint16_t)26*(m+1))/10 + d - 1;
	if (w == 0)
	{
		w = 7;	/* ��ʾ���� */
	}
	else if (w < 0)	/* ���w�Ǹ����������������ʽ��ͬ */
	{
		w = 7 - (-w) % 7;
	}
	else
	{
		w = w % 7;
	}
	
	/* 2018-10-20 else��������棬���������w�����0�� */
	if (w == 0)
	{
		w = 7;	/* ��ʾ���� */
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


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
