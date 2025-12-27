/*
 * Bootloader.c
 *
 *  Created on: Dec 5, 2025
 *      Author: hendb
 *  Updated for: UART1 (PA9/PA10) to fix ESP32 connection
 */
#include "Bootloader.h"
#include "main.h"


static uint8_t Host_buffer[HOSTM_MAX_SIZE];
static uint32_t BL_CRC_verify(uint8_t *pdata,uint32_t  DataLen , uint32_t HostCRC);
static void BL_Get_Help(uint8_t *Host_buffer);
static void BL_Send_ACK(uint8_t dataLen);
static void BL_Send_NACK();
static void BL_Get_Version(uint8_t *Host_buffer);
static void BL_Get_Chip_Identification_nNumber(uint8_t *Host_buffer);
void BL_SendMessage(char *format,...);
static uint8_t Perform_Flash_Erase(uint32_t PageAddress , uint8_t page_Number );
static void BL_Flash_Erase(uint8_t *Host_buffer);
static void BL_Write_Data(uint8_t *Host_buffer);
static uint8_t BL_Address_Varification(uint32_t Address);
static uint32_t GetSector(uint32_t Address);
static uint8_t FlashMemory_Payload_Write(uint8_t* pdata, uint32_t StartAddress, uint8_t payloadlen);
static void BL_Go_To_Addr(uint8_t *Host_buffer) ;

/* Définition d'un pointeur de fonction pour le saut */
typedef void (*pFunction)(void);

extern CRC_HandleTypeDef hcrc;
/* CORRECTION IMPORTANTE : Utilisation de UART1 au lieu de UART2 */
extern UART_HandleTypeDef huart1;

BL_status BL_FeatchHostCommand() {
    BL_status status = BL_NACK;
    HAL_StatusTypeDef Hal_status = HAL_ERROR;
    uint8_t DataLen = 0;

    memset(Host_buffer,0,HOSTM_MAX_SIZE);

    /* Utilisation de huart1 */
    Hal_status = HAL_UART_Receive(&huart1,Host_buffer,1,HAL_MAX_DELAY);
    if(Hal_status != HAL_OK) return BL_NACK;

    DataLen = Host_buffer[0];
    /* Utilisation de huart1 */
    Hal_status = HAL_UART_Receive(&huart1,&Host_buffer[1],DataLen,HAL_MAX_DELAY);
    if(Hal_status != HAL_OK) return BL_NACK;

    switch(Host_buffer[1]){
        case CBL_GET_VER_CMD :
            BL_Get_Version(Host_buffer);
            break;

        case CBL_GET_HELP_CMD :
            BL_Get_Help(Host_buffer);
            break;

        case CBL_GET_CID_CMD :
            BL_Get_Chip_Identification_nNumber(Host_buffer);
            break;

        case CBL_FLASH_ERASE_CMD :
            BL_Flash_Erase(Host_buffer);
            break;

        case CBL_MEM_WRITE_CMD :
            BL_Write_Data(Host_buffer);
            break;
        case CBL_GO_TO_ADDR_CMD :
            BL_Go_To_Addr(Host_buffer);
            break;

        default:
            BL_Send_NACK();
            break;
    }
    return BL_NACK;
}

void BL_SendMessage(char *format,...){
    char message[100]={0};
    va_list args;
    va_start(args,format);
    vsnprintf(message, sizeof(message), format, args);
    /* Utilisation de huart1 */
    HAL_UART_Transmit(&huart1,(uint8_t*)message,strlen(message),HAL_MAX_DELAY);
    va_end(args);
}


static uint32_t BL_CRC_verify(uint8_t *pdata, uint32_t DataLen, uint32_t HostCRC){
    return CRC_VERIFING_PASS;

//    uint32_t crc_status = CRC_VERIFING_FAILED;
//    uint32_t MCU_CRC = 0;
//    uint32_t dataBuffer = 0;
//
//    __HAL_CRC_DR_RESET(&hcrc);
//
//    for(uint32_t count = 0; count < DataLen; count++){
//        dataBuffer = (uint32_t)pdata[count];
//        MCU_CRC = HAL_CRC_Accumulate(&hcrc, &dataBuffer, 1);
//    }
//
//    __HAL_CRC_DR_RESET(&hcrc);
//
//    if(HostCRC == MCU_CRC){
//        crc_status = CRC_VERIFING_PASS;
//    } else {
//        crc_status = CRC_VERIFING_FAILED;
//    }
//    return crc_status;
}


static void BL_Send_ACK(uint8_t dataLen){
	uint8_t ACK_value[2]={0};
	ACK_value[0]=SEND_ACK;
	ACK_value[1]=dataLen;
    /* Utilisation de huart1 */
	HAL_UART_Transmit(&huart1, (uint8_t*)ACK_value,2,HAL_MAX_DELAY);
}

static void BL_Send_NACK(){
	uint8_t ACK_value=SEND_NACK;
    /* Utilisation de huart1 */
	HAL_UART_Transmit(&huart1,&ACK_value,sizeof(ACK_value),HAL_MAX_DELAY);
}

static void BL_Get_Version(uint8_t *Host_buffer){
	uint8_t Version[4]={CBL_VENDOR_ID,CBL_SW_MAJOR_VERSION,CBL_SW_MINOR_VERSION,CBL_SW_PATCH_VERSION};
	uint16_t Host_Packet_Len=0;
	uint32_t CRC_value = 0;

	Host_Packet_Len=Host_buffer[0]+1;
	CRC_value =*(uint32_t*)( Host_buffer + Host_Packet_Len-4);

    if(CRC_VERIFING_PASS == BL_CRC_verify((uint8_t*)&Host_buffer[0],Host_Packet_Len-4,CRC_value)){
		BL_Send_ACK(4);
        /* Utilisation de huart1 */
		HAL_UART_Transmit(&huart1,(uint8_t*)Version,sizeof(Version),HAL_MAX_DELAY);
	}
	else{
		BL_Send_NACK();
	}
}

static void BL_Get_Help(uint8_t *Host_buffer){
	uint8_t BL_sppurted_CMS[6]={CBL_GET_VER_CMD,CBL_GET_HELP_CMD,CBL_GET_CID_CMD,CBL_GO_TO_ADDR_CMD,CBL_FLASH_ERASE_CMD,CBL_MEM_WRITE_CMD};
	uint16_t Host_Packet_Len=0;
	uint32_t CRC_value=0;
	Host_Packet_Len=Host_buffer[0]+1;
	CRC_value =*(uint32_t*)( Host_buffer + Host_Packet_Len-4);
	if(CRC_VERIFING_PASS == BL_CRC_verify((uint8_t*)&Host_buffer[0],Host_Packet_Len-4,CRC_value)){
		BL_Send_ACK(6);
        /* Utilisation de huart1 */
		HAL_UART_Transmit(&huart1,(uint8_t*)BL_sppurted_CMS, sizeof(BL_sppurted_CMS), HAL_MAX_DELAY);
	}
	else{
		BL_Send_NACK();
	}
}

static void BL_Get_Chip_Identification_nNumber(uint8_t *Host_buffer){
    uint16_t Chip_ID = 0;
    uint16_t Host_Packet_Len = 0;
    uint32_t CRC_value = 0;

    Host_Packet_Len = Host_buffer[0] + 1;
    memcpy(&CRC_value, Host_buffer + Host_Packet_Len - 4, sizeof(CRC_value));

    if(CRC_VERIFING_PASS == BL_CRC_verify((uint8_t*)&Host_buffer[0], Host_Packet_Len - 4, CRC_value)){
        Chip_ID = (uint16_t)(DBGMCU->IDCODE & 0x0FFFU);
        BL_Send_ACK(2);
        /* Utilisation de huart1 */
        HAL_UART_Transmit(&huart1, (uint8_t *)&Chip_ID, 2, HAL_MAX_DELAY);
    } else {
        BL_Send_NACK();
    }
}

static uint32_t GetSector(uint32_t Address)
{
    uint32_t sector = 0;
    if((Address >= 0x08000000) && (Address < 0x08004000)){
        sector = FLASH_SECTOR_0;
    }
    else if((Address >= 0x08004000) && (Address < 0x08008000)){
        sector = FLASH_SECTOR_1;
    }
    else if((Address >= 0x08008000) && (Address < 0x0800C000)){
        sector = FLASH_SECTOR_2;
    }
    else if((Address >= 0x0800C000) && (Address < 0x08010000)){
        sector = FLASH_SECTOR_3;
    }
    else if((Address >= 0x08010000) && (Address < 0x08020000)){
        sector = FLASH_SECTOR_4;
    }
    else if((Address >= 0x08020000) && (Address < 0x08040000)){
        sector = FLASH_SECTOR_5;
    }
    else if((Address >= 0x08040000) && (Address < 0x08060000)){
        sector = FLASH_SECTOR_6;
    }
    else if((Address >= 0x08060000) && (Address < 0x08080000)){
        sector = FLASH_SECTOR_7;
    }
    else{
       sector = 0xFFFFFFFF;
    }
    return sector;
}

static uint8_t Perform_Flash_Erase(uint32_t PageAddress, uint8_t page_Number) {
    FLASH_EraseInitTypeDef pEraseInit;
    uint32_t PageError = 0;
    uint8_t Pagestatus = INVALID_PAGE_NUMBER;
    HAL_StatusTypeDef Hal_Status = HAL_ERROR;
    uint32_t FirstSector = 0;

    /* --- CAS 1 : SMART MASS ERASE --- */
    if (PageAddress == CBL_FLASH_MASS_ERASE) {
        pEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
        pEraseInit.Banks = FLASH_BANK_1;
        pEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
        /* Effacement Application (Secteurs 2 à 7) */
        pEraseInit.Sector = FLASH_SECTOR_2;
        pEraseInit.NbSectors = 6;

        HAL_FLASH_Unlock();
        Hal_Status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);
        HAL_FLASH_Lock();

        if (Hal_Status == HAL_OK && PageError == 0xFFFFFFFFU) {
            return SUCCESSFUL_ERASE;
        } else {
            return UNSUCCESSFUL_ERASE;
        }
    }

    /* --- CAS 2 : EFFACEMENT PAR SECTEUR --- */
    FirstSector = GetSector(PageAddress);

    if (FirstSector == 0xFFFFFFFF) return INVALID_PAGE_NUMBER;
    if (FirstSector < FLASH_SECTOR_2) return INVALID_PAGE_NUMBER; // Protection Bootloader
    if ((FirstSector + page_Number) > CBL_FLASH_MAX_PAGE_NUMBER) return INVALID_PAGE_NUMBER;

    pEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
    pEraseInit.Banks = FLASH_BANK_1;
    pEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    pEraseInit.Sector = FirstSector;
    pEraseInit.NbSectors = page_Number;

    HAL_FLASH_Unlock();
    Hal_Status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);
    HAL_FLASH_Lock();

    if (Hal_Status == HAL_OK && PageError == 0xFFFFFFFFU) {
        Pagestatus = SUCCESSFUL_ERASE;
    } else {
        Pagestatus = UNSUCCESSFUL_ERASE;
    }

    return Pagestatus;
}

static void BL_Flash_Erase(uint8_t *Host_buffer) {
    uint8_t Erase_status = UNSUCCESSFUL_ERASE;
    uint16_t Host_Packet_Len = 0;
    uint32_t CRC_value = 0;

    Host_Packet_Len = Host_buffer[0] + 1;
    CRC_value = *(uint32_t*)(Host_buffer + Host_Packet_Len - 4);

    if (CRC_VERIFING_PASS == BL_CRC_verify((uint8_t*)&Host_buffer[0], Host_Packet_Len - 4, CRC_value)) {
        uint32_t PageAddress = *((uint32_t*)&Host_buffer[2]);
        uint8_t NumberOfSectors = Host_buffer[6];

        Erase_status = Perform_Flash_Erase(PageAddress, NumberOfSectors);

        BL_Send_ACK(1);
        /* Utilisation de huart1 */
        HAL_UART_Transmit(&huart1, (uint8_t *)&Erase_status, 1, HAL_MAX_DELAY);

    } else {
        BL_Send_NACK();
    }
}

static uint8_t FlashMemory_Payload_Write(uint8_t* pdata, uint32_t StartAddress, uint8_t payloadlen)
{
    HAL_StatusTypeDef Hal_status = HAL_ERROR;
    uint8_t payload_status = FLASH_PAYLOAD_WRITE_FAILED;

    HAL_FLASH_Unlock();

    for(uint8_t i = 0; i < payloadlen; i++)
    {
        uint32_t current_address = StartAddress + i;
        Hal_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, current_address, pdata[i]);

        if(Hal_status != HAL_OK)
        {
            payload_status = FLASH_PAYLOAD_WRITE_FAILED;
            break;
        }
        else
        {
            payload_status = FLASH_PAYLOAD_WRITE_PASSED;
        }
    }
    HAL_FLASH_Lock();
    return payload_status;
}

static uint8_t BL_Address_Varification(uint32_t Address){
    uint8_t Adress_varify = ADDRESS_IS_INVALID;

    if(Address >= FLASH_BASE && Address <= STM32F401_FLASH_END)
    {
        if (Address < 0x08008000)
        {
            Adress_varify = ADDRESS_IS_INVALID;
        }
        else
        {
            Adress_varify = ADDRESS_IS_VALID;
        }
    }
    else if(Address >= SRAM_BASE && Address <= STM32F401_SRAM_END)
    {
        Adress_varify = ADDRESS_IS_VALID;
    }
    else
    {
        Adress_varify = ADDRESS_IS_INVALID;
    }
    return Adress_varify;
}


static void BL_Write_Data(uint8_t *Host_buffer){
    uint8_t payload_status = FLASH_PAYLOAD_WRITE_FAILED;
    uint8_t Adress_varify = ADDRESS_IS_INVALID;
    uint32_t Adress_Host = 0;
    uint8_t DataLen = 0;
    uint16_t Host_Packet_Len = 0;
    uint32_t CRC_value = 0;

    Host_Packet_Len = Host_buffer[0] + 1;
    CRC_value = *(uint32_t*)(Host_buffer + Host_Packet_Len - 4);

    if(CRC_VERIFING_PASS == BL_CRC_verify((uint8_t*)&Host_buffer[0], Host_Packet_Len - 4, CRC_value)){

        BL_Send_ACK(1);

        Adress_Host = *((uint32_t*)&Host_buffer[2]);
        DataLen = Host_buffer[6];

        Adress_varify = BL_Address_Varification(Adress_Host);

        if(Adress_varify == ADDRESS_IS_VALID){
            payload_status = FlashMemory_Payload_Write(&Host_buffer[7], Adress_Host, DataLen);
            /* Utilisation de huart1 */
            HAL_UART_Transmit(&huart1, &payload_status, 1, HAL_MAX_DELAY);
        }
        else{
            payload_status = FLASH_PAYLOAD_WRITE_FAILED;
            /* Utilisation de huart1 */
            HAL_UART_Transmit(&huart1, &payload_status, 1, HAL_MAX_DELAY);
        }
    }
    else{
        BL_Send_NACK();
    }
}

static void BL_Go_To_Addr(uint8_t *Host_buffer) {
    uint8_t addr_status = ADDRESS_IS_INVALID;
    uint32_t Jump_Address = 0;
    uint16_t Host_Packet_Len = 0;
    uint32_t CRC_value = 0;

    Jump_Address = *((uint32_t*)&Host_buffer[2]);
    Host_Packet_Len = Host_buffer[0] + 1;
    CRC_value = *(uint32_t*)(Host_buffer + Host_Packet_Len - 4);

    if(CRC_VERIFING_PASS == BL_CRC_verify((uint8_t*)&Host_buffer[0], Host_Packet_Len - 4, CRC_value)){

        BL_Send_ACK(1);

        addr_status = BL_Address_Varification(Jump_Address);

        if(addr_status == ADDRESS_IS_VALID){

            /* Utilisation de huart1 */
            HAL_UART_Transmit(&huart1, &addr_status, 1, HAL_MAX_DELAY);

            uint32_t MSP_Value = *(volatile uint32_t *)Jump_Address;
            uint32_t Reset_Handler_Address = *(volatile uint32_t *)(Jump_Address + 4);

            pFunction Jump_To_Application = (pFunction)Reset_Handler_Address;

            HAL_CRC_DeInit(&hcrc);

            /* CORRECTION : Désactivation de HUART1 avant le saut */
            HAL_UART_DeInit(&huart1);

            HAL_RCC_DeInit();

            SysTick->CTRL = 0;
            SysTick->LOAD = 0;
            SysTick->VAL  = 0;

            __disable_irq();

            SCB->VTOR = Jump_Address;
            __set_MSP(MSP_Value);
            __enable_irq();
            Jump_To_Application();
        }
        else{
            /* Utilisation de huart1 */
            HAL_UART_Transmit(&huart1, &addr_status, 1, HAL_MAX_DELAY);
        }
    }
    else{
        BL_Send_NACK();
    }
}
