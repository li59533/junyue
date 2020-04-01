/**
 **************************************************************************************************
 * @file        net_task.h
 * @author
 * @version
 * @date        
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */
#ifndef _NET_TASK_H_
#define _NET_TASK_H_

/**
 * @addtogroup    XXX 
 * @{ 
 */
#include "self_def.h"
/**
 * @addtogroup    net_task_Modules 
 * @{  
 */

/**
 * @defgroup      net_task_Exported_Macros 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      net_task_Exported_Constants
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      net_task_Exported_Types 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      net_task_Exported_Variables 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      net_task_Exported_Functions 
 * @{  
 */
void NetProcessFunction ( void *pvParameters );

void Net_Task_UpdataConnectFlag(void);
void Net_Task_CheckConnect(void);

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
#endif
