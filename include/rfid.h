#ifndef RFID_H_
#define RFID_H_

#include <avr/io.h>
#include <util/delay.h>

enum PCD_Register {
    CommandReg    = 0x01,
    ComIEnReg     = 0x02,
    DivIEnReg     = 0x03,
    ComIrqReg     = 0x04,
    DivIrqReg     = 0x05,
    ErrorReg      = 0x06,
    Status1Reg    = 0x07,
    Status2Reg    = 0x08,
    FIFODataReg   = 0x09,
    FIFOLevelReg  = 0x0A,
    WaterLevelReg = 0x0B,
    ControlReg    = 0x0C,
    BitFramingReg = 0x0D,
    CollReg       = 0x0E,

    ModeReg       = 0x11,
    TxModeReg     = 0x12,
    RxModeReg     = 0x13,
    TxControlReg  = 0x14,
    TxASKReg      = 0x15,
    TxSelReg      = 0x16,
    RxSelReg      = 0x17,
    RxThresholdReg= 0x18,
    DemodReg      = 0x19,
    MfTxReg       = 0x1C,
    MfRxReg       = 0x1D,
    SerialSpeedReg= 0x1F,

    CRCResultRegH = 0x21,
    CRCResultRegL = 0x22,
    ModWidthReg   = 0x24,
    RFCfgReg      = 0x26,
    GsNReg        = 0x27,
    CWGsPReg      = 0x28,
    ModGsPReg     = 0x29,
    TModeReg      = 0x2A,
    TPrescalerReg = 0x2B,
    TReloadRegH   = 0x2C,
    TReloadRegL   = 0x2D,

    VersionReg    = 0x37,
};

enum PCD_Command {
    PCD_Idle        = 0x00,
    PCD_CalcCRC     = 0x03,
    PCD_Transmit    = 0x04,
    PCD_Transceive  = 0x0C,
    PCD_SoftReset   = 0x0F,
};

enum PICC_Command {
    PICC_CMD_REQA      = 0x26,
    PICC_CMD_SEL_CL1   = 0x93,
    PICC_CMD_HLTA      = 0x50,
};

extern uint8_t SS_PIN;
extern uint8_t RST_PIN;

void SPI_Init();
uint8_t SPI_Transceive(uint8_t data);

void SS_Low();
void SS_High();
void RST_Low();
void RST_High();

void RC522_Write(uint8_t reg, uint8_t val);
uint8_t RC522_Read(uint8_t reg);
void RC522_SetRegisterBitMask(uint8_t reg, uint8_t mask);
void RC522_ClearRegisterBitMask(uint8_t reg, uint8_t mask);
void RC522_Reset();
void RC522_Init();

uint8_t RC522_CommunicateWithPICC(uint8_t command, uint8_t *sendData,
                                  uint8_t sendLen, uint8_t *backData,
                                  uint8_t *backLen);
uint8_t RC522_Request();
uint8_t RC522_Anticoll(uint8_t *serNum);
void PICC_HaltA();

#endif