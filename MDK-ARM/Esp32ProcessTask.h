/**
 **************************************************************************************************
 * @file        Esp32ProcessTask.h
 * @author
 * @Esp32ProcessTask.h
 * @date        
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */
#ifndef _ESP32PROCESSTASK_H_
#define _ESP32PROCESSTASK_H_

/**
 * @addtogroup    XXX 
 * @{ 
 */
#include "self_def.h"
/**
 * @addtogroup    Esp32ProcessTask.h_Modules 
 * @{  
 */

/**
 * @defgroup      Esp32ProcessTask.h_Exported_Macros 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      Esp32ProcessTask.h_Exported_Constants
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      Esp32ProcessTask.h_Exported_Types 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      Esp32ProcessTask.h_Exported_Variables 
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      Esp32ProcessTask.h_Exported_Functions 
 * @{  
 */
void Esp32_SetSendDeviceInfo_Flag(void);
void Esp32_ClearSendDeviceInfo_Flag(void);
uint8_t Esp32_GetSendDeivceInfo_Flag(void);

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
