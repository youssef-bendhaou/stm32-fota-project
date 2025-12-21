
/*
 * Bootloader.h
 *
 *  Created on: Dec 5, 2025
 *      Author: hendb
 *  Updated: Fix Mass Erase Value & Clean Prototypes
 */

#ifndef INC_BOOTLOADER_H_
#define INC_BOOTLOADER_H_

#include "string.h"
#include "stdio.h"
#include <stdint.h>
#include <stdarg.h>
#include "stm32f4xx_hal.h"

/* --- Commandes du Bootloader --- */
#define CBL_GET_VER_CMD       0x10
#define CBL_GET_HELP_CMD      0x11
#define CBL_GET_CID_CMD       0x12
#define CBL_GO_TO_ADDR_CMD    0x14
#define CBL_FLASH_ERASE_CMD   0x15
#define CBL_MEM_WRITE_CMD     0x16

/* --- Réponses ACK/NACK --- */
#define SEND_NACK   0xAB
#define SEND_ACK    0xCD

/* --- Status CRC --- */
#define CRC_VERIFING_FAILED   0x00
#define CRC_VERIFING_PASS     0X01

/* --- Paramètres Buffer --- */
#define HOSTM_MAX_SIZE        200

/* --- Version --- */
#define CBL_VENDOR_ID         100
#define CBL_SW_MAJOR_VERSION  1
#define CBL_SW_MINOR_VERSION  1
#define CBL_SW_PATCH_VERSION  0

/* --- Paramètres Flash STM32F401RE --- */
#define CBL_FLASH_MAX_PAGE_NUMBER   8

/*
 * CRITIQUE : Doit être 0xFFFFFFFF pour correspondre à l'adresse envoyée par l'ESP32
 */
#define CBL_FLASH_MASS_ERASE        0xFFFFFFFF

#define HAL_SUCCESSFUL_ERASE        0xFFFFFFFFU

#define INVALID_PAGE_NUMBER         0x00
#define VALID_PAGE_NUMBER           0x01
#define UNSUCCESSFUL_ERASE          0x02
#define SUCCESSFUL_ERASE            0x03

#define FLASH_PAYLOAD_WRITE_FAILED  0x00
#define FLASH_PAYLOAD_WRITE_PASSED  0x01

#define ADDRESS_IS_INVALID          0x00
#define ADDRESS_IS_VALID            0x01

/* --- Mapping Mémoire STM32F401 --- */
#define STM32F401_SRAM_SIZE     (96 * 1024)
#define STM32F401_FLASH_SIZE    (512 * 1024)
#define STM32F401_SRAM_END      (SRAM_BASE + STM32F401_SRAM_SIZE)
#define STM32F401_FLASH_END     (FLASH_BASE + STM32F401_FLASH_SIZE)

/* --- Enum Status --- */
typedef enum{
	BL_NACK = 0,
	BL_ACK
} BL_status;

/* --- Prototypes Publics (Appelés par main.c) --- */
void BL_SendMessage(char *format,...);
BL_status BL_FeatchHostCommand();

/*
 * Note : Les autres fonctions (Perform_Flash_Erase, BL_Write_Data, etc.)
 * sont déclarées en 'static' dans Bootloader.c et ne doivent pas apparaître ici
 * pour éviter des warnings/erreurs de compilation dans main.c
 */

#endif /* INC_BOOTLOADER_H_ */




///*
// * Bootloader.h
// *
// *  Created on: Dec 5, 2025
// *      Author: hendb
// */
//
//#ifndef INC_BOOTLOADER_H_
//#define INC_BOOTLOADER_H_
//
//#include "string.h"
//#include "stdio.h"
//#include <stdint.h>
//#include <stdarg.h>
//#include "stm32f4xx_hal.h"
//
//
//
//#define CBL_GET_VER_CMD    0x10
//#define CBL_GET_HELP_CMD    0x11
//#define CBL_GET_CID_CMD    0x12
//#define CBL_GO_TO_ADDR_CMD    0x14
//#define CBL_FLASH_ERASE_CMD    0x15
//#define CBL_MEM_WRITE_CMD    0x16
//
//#define SEND_NACK   0xAB
//#define SEND_ACK   0xCD
//
//
//
//
//
//
//
//#define CRC_VERIFING_FAILED   0x00
//#define CRC_VERIFING_PASS     0X01
//#define HOSTM_MAX_SIZE     200
//
//#define CBL_VENDOR_ID    	  100
//#define CBL_SW_MAJOR_VERSION  1
//#define CBL_SW_MINOR_VERSION  1
//#define CBL_SW_PATCH_VERSION  0
//
//
//#define CBL_FLASH_MAX_PAGE_NUMBER   8
//#define CBL_FLASH_MASS_ERASE        0xFF
//
//#define HAL_SUCCESSFUL_ERASE 0xFFFFFFFFU
//
//#define INVALID_PAGE_NUMBER           0x00
//#define VALID_PAGE_NUMBER             0x01
//#define UNSUCCESSFUL_ERASE            0x02
//#define SUCCESSFUL_ERASE              0x03
//
//#define FLASH_PAYLOAD_WRITE_FAILED    0x00
//#define FLASH_PAYLOAD_WRITE_PASSED    0x01
//
//#define  ADDRESS_IS_INVALID   0x00
//#define  ADDRESS_IS_VALID     0x01
//
//#define STM32F401_SRAM_SIZE     (96 * 1024)
//#define STM32F401_FLASH_SIZE     (512* 1024)
//#define STM32F401_SRAM_END      (	SRAM_BASE  + STM32F401_SRAM_SIZE)
//#define STM32F401_FLASH_END     (FLASH_BASE   + STM32F401_FLASH_SIZE)
//
//
//
//
//
//typedef enum{
//	BL_NACK=0,
//	BL_ACK
//}BL_status;
//
//
//
//
//void BL_SendMessage(char *format,...);
//static void BL_Go_To_Addr(uint8_t *Host_buffer) ;
//BL_status BL_FeatchHostCommand();
//static uint8_t Perform_Flash_Erase(uint32_t PageAddress, uint8_t page_Number);
///* Dans Bootloader.h */
//static uint8_t FlashMemory_Payload_Write(uint8_t* pdata, uint32_t StartAddress, uint8_t payloadlen);
//static uint32_t BL_CRC_verify(uint8_t *pdata,uint32_t  DataLen , uint32_t HostCRC);
//static void BL_Get_Help(uint8_t *Host_buffer);
//
//#endif /* INC_BOOTLOADER_H_ */
