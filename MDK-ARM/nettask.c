/**
 **************************************************************************************************
 * @file        net_task.c
 * @author
 * @version
 * @date        
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */

#include "nettask.h"

/**
 * @addtogroup    XXX 
 * @{  
 */
#include "main.h"
#include "cmsis_os.h"
#include "clog.h"
#include "app.h"
/**
 * @addtogroup    net_task_Modules 
 * @{  
 */

/**
 * @defgroup      net_task_IO_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup       net_task_Macros_Defines 
 * @brief         
 * @{  
 */
#define NET_TASK_CONNECT_TIMEOUT    30
/**
 * @}
 */

/**
 * @defgroup      net_task_Constants_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup       net_task_Private_Types
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      net_task_Private_Variables 
 * @brief         
 * @{  
 */
static uint32_t net_connect_flag = 0;
/**
 * @}
 */

/**
 * @defgroup      net_task_Public_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      net_task_Private_FunctionPrototypes 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      net_task_Functions 
 * @brief         
 * @{  
 */
void NetProcessFunction ( void *pvParameters )
{
	while(1)
	{
		DEBUG("NetProcessFunction Check Beacon\r\n");
		
		Net_Task_CheckConnect();
		osDelay (1000);
		
	}
}


void Net_Task_UpdataConnectFlag(void)
{
	net_connect_flag = 0;
}


void Net_Task_CheckConnect(void)
{
	if(config.Lowpower_Mode==normalmode)
	{
		net_connect_flag ++;	
		if(net_connect_flag >= NET_TASK_CONNECT_TIMEOUT)
		{
			DEBUG("Net_Task Time out \r\n");
			net_connect_flag = 0;
			software_reset();
		}		
	}

}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

