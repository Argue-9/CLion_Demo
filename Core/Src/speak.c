/*
 * @Author: huangwenjun
 * @Date: 2022-07-22 21:02:11
 * @FilePath: \selfpractice\Usart\Core\Src\speak.c
 */
#include "speak.h"

static __IO uint32_t I2CTimeout = I2CT_LONG_TIMEOUT;

/**
 * @brief  Basic management of the timeout situation.
 * @param  None.
 * @retval None.
 */
static uint8_t I2C_TIMEOUT_UserCallback(void)
{
    /* Block communication and all processes */
    printf("I2C Timeout error!");

    return 0;
}

/**
 * @brief   写一个字节到I2C设备中
 * @param
 *		@arg pBuffer:写的值
 * @retval  正常返回1，异常返回0
 */
uint8_t I2C_ByteWrite(uint8_t pBuffer)
{
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback();
    }

    /* Send STRAT condition */
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    while (HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR, &pBuffer, 1, 0xffff) != HAL_OK)
    {
        printf("write error\r\n");
        HAL_Delay(500);
    }
    HAL_Delay(5);
    return 1; //正常返回1
}

void I2C_Writes_Bytes(uint8_t *buff, int number)
{
    int i;
    for (i = 0; i < number; i++)
    {
        I2C_ByteWrite(buff[i]);
    }
}

/**
 * @brief   获取芯片状态值
 * @param
 * @retval  返回状态值
 */
uint8_t GetChipStatus(void)
{
    uint8_t pBuffer = 0xff;
    uint8_t AskState[4] = {0xFD, 0x00, 0x01, 0x21};

    I2C_Writes_Bytes(AskState, 4);
    // printf("ASK write ok\r\n");

    while ((HAL_I2C_Master_Receive(&hi2c1, I2C_ADDR, &pBuffer, 1, 0xffff)) != HAL_OK)
    {
        printf("receive error\r\n");
    }
    // printf("receive!\r\n");
    // printf("%x \r\n", pBuffer);
    return pBuffer;
}

void speech_text(uint8_t *str, uint8_t encoding_format)
{
    uint16_t size = strlen(str) + 2;

    XFS_Protocol_TypeDef DataPack;

    DataPack.DataHead = DATAHEAD;
    DataPack.Length_HH = size >> 8;
    DataPack.Length_LL = size;
    DataPack.Commond = 0x01;
    DataPack.EncodingFormat = encoding_format;
    DataPack.Text = str;

    I2C_ByteWrite(DataPack.DataHead);
    I2C_ByteWrite(DataPack.Length_HH);
    I2C_ByteWrite(DataPack.Length_LL);
    I2C_ByteWrite(DataPack.Commond);
    I2C_ByteWrite(DataPack.EncodingFormat);

    I2C_Writes_Bytes(DataPack.Text, strlen(DataPack.Text));
}

void SetBase(uint8_t *str)
{
    uint16_t size = strlen(str) + 2;

    XFS_Protocol_TypeDef DataPack;

    DataPack.DataHead = DATAHEAD;
    DataPack.Length_HH = size >> 8;
    DataPack.Length_LL = size;
    DataPack.Commond = 0x01;
    DataPack.EncodingFormat = 0x00;
    DataPack.Text = str;

    I2C_ByteWrite(DataPack.DataHead);
    I2C_ByteWrite(DataPack.Length_HH);
    I2C_ByteWrite(DataPack.Length_LL);
    I2C_ByteWrite(DataPack.Commond);
    I2C_ByteWrite(DataPack.EncodingFormat);
    I2C_Writes_Bytes(DataPack.Text, strlen(DataPack.Text));
}

void TextCtrl(char c, int d)
{
    char str[10];
    if (d != -1)
        sprintf(str, "[%c%d]", c, d);
    else
        sprintf(str, "[%c]", c);
    // printf("str:%s\r\n", str);
    SetBase(str);
}

void SetStyle(Style_Type style)
{
    TextCtrl('f', style);
    while (GetChipStatus() != ChipStatus_Idle)
    {
        HAL_Delay(10);
    }
}

void SetLanguage(Language_Type language)
{
    TextCtrl('g', language);
    while (GetChipStatus() != ChipStatus_Idle)
    {
        HAL_Delay(10);
    }
}

void SetArticulation(Articulation_Type articulation)
{
    TextCtrl('h', articulation);
    while (GetChipStatus() != ChipStatus_Idle)
    {
        HAL_Delay(10);
    }
}

void SetSpell(Spell_Type spell)
{
    TextCtrl('i', spell);
    while (GetChipStatus() != ChipStatus_Idle)
    {
        HAL_Delay(10);
    }
}

void SetReader(Reader_Type reader)
{
    TextCtrl('m', reader);
    while (GetChipStatus() != ChipStatus_Idle)
    {
        HAL_Delay(10);
    }
}

void SetNumberHandle(NumberHandle_Type numberHandle)
{
    TextCtrl('n', numberHandle);
    while (GetChipStatus() != ChipStatus_Idle)
    {
        HAL_Delay(10);
    }
}

void SetZeroPronunciation(ZeroPronunciation_Type zeroPronunciation)
{
    TextCtrl('o', zeroPronunciation);
    while (GetChipStatus() != ChipStatus_Idle)
    {
        HAL_Delay(10);
    }
}

void SetNamePronunciation(NamePronunciation_Type namePronunciation)
{
    TextCtrl('r', namePronunciation);
    while (GetChipStatus() != ChipStatus_Idle)
    {
        HAL_Delay(10);
    }
}

void SetSpeed(int speed)
{
    TextCtrl('s', speed);
    while (GetChipStatus() != ChipStatus_Idle)
    {
        HAL_Delay(10);
    }
}

void SetIntonation(int intonation)
{
    TextCtrl('t', intonation);
    while (GetChipStatus() != ChipStatus_Idle)
    {
        HAL_Delay(10);
    }
}

void SetVolume(int volume)
{
    printf("TextCtrl('v', volume);\r\n");
    TextCtrl('v', volume);
    while (GetChipStatus() != ChipStatus_Idle)
    {
        HAL_Delay(10);
    }
    printf("done\r\n");
}

void SetOnePronunciation(OnePronunciation_Type onePronunciation)
{
    TextCtrl('y', onePronunciation);
    while (GetChipStatus() != ChipStatus_Idle)
    {
        HAL_Delay(10);
    }
}

void SetRhythm(Rhythm_Type rhythm)
{
    TextCtrl('z', rhythm);
    while (GetChipStatus() != ChipStatus_Idle)
    {
        HAL_Delay(10);
    }
}

void SetRestoreDefault()
{
    TextCtrl('d', -1);
    while (GetChipStatus() != ChipStatus_Idle)
    {
        HAL_Delay(10);
    }
}
