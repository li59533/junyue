#include "bsp.h"
#include "app.h"
#include <string.h>  
#include <stdbool.h>
#include <stdarg.h>
#include "bsp_flash.h"
#include "bsp_uniqueID.h"

//#define IAP_ADDRESS 0x08100000

#define EEPROM_START  0x08086000 

//#define EEPROM_START  0X08020000 

/*************************************************************************/
/*************************本代码仅仅实现了BANK1****************************/
/*************************************************************************/

#define FLASH_USER_START_ADDR   ADDR_FLASH_PAGE_16   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ADDR_FLASH_PAGE_255 + FLASH_PAGE_SIZE - 1   /* End @ of user Flash area */

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;

uint32_t FirstPage = 0, NbOfPages = 0, BankNumber = 0;
uint32_t Address = 0, PAGEError = 0;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;


//读取指定地址的字(32位数据) 
//faddr:读地址 
//返回值:对应数据.
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
	return *(__IO uint32_t *)faddr; 
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;
  
  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }
  
  return page;
}

/**
  * @brief  Gets the bank of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The bank of a given address
  */
static uint32_t GetBank(uint32_t Addr)
{
  uint32_t bank = 0;
  
  if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
  {
  	/* No Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_1;
    }
    else
    {
      bank = FLASH_BANK_2;
    }
  }
  else
  {
  	/* Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_2;
    }
    else
    {
      bank = FLASH_BANK_1;
    }
  }
  
  return bank;
}

uint64_t ssssss;
void STMFLASH_Write(uint32_t Address,uint64_t *pBuffer,uint32_t NumToWrite)	
{
  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();
	uint64_t *DATA64;
	DATA64=pBuffer;
	uint32_t write_addr=Address;
  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Clear OPTVERR bit set on virgin samples */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 
  /* Get the 1st page to erase */
  FirstPage = GetPage(Address);
  /* Get the number of pages to erase from 1st page */
  NbOfPages = GetPage(Address+NumToWrite) - FirstPage + 1;
  /* Get the bank */
  BankNumber = GetBank(Address);
  /* Fill EraseInit structure*/
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks       = BankNumber;
  EraseInitStruct.Page        = FirstPage;
  EraseInitStruct.NbPages     = NbOfPages;

  /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
     you have to make sure that these data are rewritten before they are accessed during code
     execution. If this cannot be done safely, it is recommended to flush the caches by setting the
     DCRST and ICRST bits in the FLASH_CR register. */
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
  {
    /*
      Error occurred while page erase.
      User can add here some code to deal with this error.
      PAGEError will contain the faulty page and then to know the code error on this page,
      user can call function 'HAL_FLASH_GetError()'
    */
    /* Infinite loop */

  }

  /* Program the user Flash area word by word
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  while (write_addr < (Address+NumToWrite))
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, write_addr, *DATA64) == HAL_OK)
    {
      write_addr = write_addr + 8;
			DATA64=DATA64+1;
    }
   else
    {
 
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();
}

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(32位)数
void STMFLASH_Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead)   	
{
	uint32_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//读取4个字节.
		ReadAddr+=4;//偏移4个字节.	
	}
}



void EarsePage(uint32_t address)  //
{

}


void EarseFlash(uint32_t address,uint32_t length)  //
{
	uint32_t write_addr=Address;
	/* Erase the user Flash area
	(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

	/* Clear OPTVERR bit set on virgin samples */
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR); 
	/* Get the 1st page to erase */
	FirstPage = GetPage(Address);
	/* Get the number of pages to erase from 1st page */
	NbOfPages = GetPage(Address+length) - FirstPage + 1;
	/* Get the bank */
	BankNumber = GetBank(Address);
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks       = BankNumber;
	EraseInitStruct.Page        = FirstPage;
	EraseInitStruct.NbPages     = NbOfPages;

	/* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
	you have to make sure that these data are rewritten before they are accessed during code
	execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	DCRST and ICRST bits in the FLASH_CR register. */
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{}
}

void RDBYTES(uint32_t address,uint32_t num, uint8_t *Data)
{
	uint32_t count;
	for(count=0;count<num/2;count++)
	{
		*((uint16_t *)Data+count)=*((uint16_t *)address+count);
  }
}

uint32_t temp_num = 0;
void WRBYTES(uint32_t address,uint32_t num, uint8_t *Data)
{
	num=(num+7)&0xfffffff8;
//	temp_num = num;
//		__disable_irq();
	STMFLASH_Write(address,(uint64_t*)Data,num);
//		__enable_irq();
}		

void readProtect(void){
	uint32_t t=0xfffffBf4;
	uint32_t rt=*((uint32_t*)(unsigned char*)(0x040c));
	if((rt &0x0f )!=4)	WRBYTES(0x040C,4,(unsigned char*)&t);
}

uint32_t llength;
void saveConfig()
{
	llength=sizeof(struct CONFIG);
	EarsePage(EEPROM_START);
//		__disable_irq();
//	STMFLASH_Write(EEPROM_START,(uint32_t*)&config,sizeof(struct CONFIG));
//		__enable_irq();
	WRBYTES(EEPROM_START,sizeof(struct CONFIG),(uint8_t*)&config);
}


void loadConfig(void)
{
//	STMFLASH_Read(EEPROM_START,(uint32_t*)&config,sizeof(config.vaildsign));
	RDBYTES(EEPROM_START,sizeof(config.vaildsign),(unsigned char*)&config);
	if(config.vaildsign!=0x45aa)
	{	
		uint8_t * uniqueid_ptr = 0;
		config.vaildsign=0x45aa;
//		uniqueid_ptr = BSP_GetUniqueID();
//		memcpy(&config.SNnumber,uniqueid_ptr,8);

		saveConfig();
		return;
	}
//	STMFLASH_Read(EEPROM_START,(uint32_t*)&config,sizeof(config.vaildsign));
	RDBYTES(EEPROM_START,sizeof(struct CONFIG),((unsigned char*)&config));
}

void initEEPROM()
{

	loadConfig();

}
