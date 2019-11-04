/**
 **************************************************************************************************
 * @file        bsp_uniqueid.c
 * @author
 * @bsp_uniqueid
 * @date        
 * @brief
 **************************************************************************************************
 * @attention
 *
 **************************************************************************************************
 */

/**
 * @addtogroup    XXX 
 * @{  
 */
#include "bsp_uniqueid.h"
/**
 * @addtogroup    bsp_uniqueid_Modules 
 * @{  
 */
#include "crc.h"
/**
 * @defgroup      bsp_uniqueid_IO_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup       bsp_uniqueid_Macros_Defines 
 * @brief         
 * @{  
 */
#define BSP_UNIQUE_DEVICE_ID_ADDRESS 0x1FFF7590
/**
 * @}
 */

/**
 * @defgroup      bsp_uniqueid_Constants_Defines 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup       bsp_uniqueid_Private_Types
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_uniqueid_Private_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_uniqueid_Public_Variables 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_uniqueid_Private_FunctionPrototypes 
 * @brief         
 * @{  
 */

/**
 * @}
 */

/**
 * @defgroup      bsp_uniqueid_Functions 
 * @brief         
 * @{  
 */

uint8_t bsp_uniqueID_space[12] = { 0 };

uint8_t * BSP_GetUniqueID(void)
{
	uint8_t i = 0;
	uint8_t uniqueid[12] = { 0 };
	uint8_t buf_temp[4] = { 0 };
	uint16_t crc_temp = 0;
	for( i = 0 ; i < 12; i ++)
	{
		uniqueid[i] =  *(uint8_t *)(BSP_UNIQUE_DEVICE_ID_ADDRESS + i);
	}
	
	buf_temp[0] = uniqueid[0];
	buf_temp[1] = uniqueid[1];
	buf_temp[2] = uniqueid[10];
	buf_temp[3] = uniqueid[11];
	
	crc_temp =  CRC16_Modbus(buf_temp,4);
	
	bsp_uniqueID_space[0] = crc_temp & 0x00ff;
	bsp_uniqueID_space[1] = crc_temp >> 8 ;
	
	buf_temp[0] = uniqueid[2];
	buf_temp[1] = uniqueid[3];
	buf_temp[2] = uniqueid[8];
	buf_temp[3] = uniqueid[9];
	
	crc_temp =  CRC16_Modbus(buf_temp,4);
	
	bsp_uniqueID_space[2] = crc_temp & 0x00ff;
	bsp_uniqueID_space[3] = crc_temp >> 8 ;
	
	buf_temp[0] = uniqueid[4];
	buf_temp[1] = uniqueid[5];
	buf_temp[2] = uniqueid[6];
	buf_temp[3] = uniqueid[7];
	
	crc_temp =  CRC16_Modbus(buf_temp,4);
	
	bsp_uniqueID_space[4] = crc_temp & 0x00ff;
	bsp_uniqueID_space[5] = crc_temp >> 8 ;
	
	buf_temp[0] = uniqueid[3];
	buf_temp[1] = uniqueid[4];
	buf_temp[2] = uniqueid[7];
	buf_temp[3] = uniqueid[8];
	
	crc_temp =  CRC16_Modbus(buf_temp,4);
	
	bsp_uniqueID_space[6] = crc_temp & 0x00ff;
	bsp_uniqueID_space[7] = crc_temp >> 8 ;	
	
	return bsp_uniqueID_space;
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

