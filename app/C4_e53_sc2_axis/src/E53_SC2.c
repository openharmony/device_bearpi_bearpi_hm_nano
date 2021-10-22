/*
 * Copyright (c) 2020 Nanjing Xiaoxiongpai Intelligent Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "E53_SC2.h"
#include "cmsis_os2.h"
#include "iot_errno.h"
#include "iot_gpio.h"
#include "iot_gpio_ex.h"
#include "iot_i2c.h"
#include "iot_i2c_ex.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define WIFI_IOT_IO_FUNC_GPIO_0_I2C1_SDA 6
#define WIFI_IOT_IO_FUNC_GPIO_1_I2C1_SCL 6
#define WIFI_IOT_IO_FUNC_GPIO_8_GPIO 0
#define WIFI_IOT_IO_FUNC_GPIO_7_GPIO 0
#define WIFI_IOT_I2C_IDX_1 1
#define RESET_DELAY_US 20000

/***************************************************************
 * 函数名称: E53SC2IoInit
 * 说    明: E53_SC2 GPIO初始化
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
static void E53SC2IoInit(void)
{
    IoTGpioInit(8);
    IoTGpioSetFunc(8, WIFI_IOT_IO_FUNC_GPIO_8_GPIO);
    IoTGpioSetDir(8, IOT_GPIO_DIR_OUT); // 设置GPIO_8为输出模式

    IoTGpioInit(7);
    IoTGpioSetFunc(7, WIFI_IOT_IO_FUNC_GPIO_7_GPIO);
    IoTGpioSetDir(7, IOT_GPIO_DIR_OUT); // 设置GPIO_7为输出模式

    IoTGpioInit(0);
    IoTGpioSetFunc(0, WIFI_IOT_IO_FUNC_GPIO_0_I2C1_SDA); // GPIO_0复用为I2C1_SDA
    IoTGpioInit(1);
    IoTGpioSetFunc(1, WIFI_IOT_IO_FUNC_GPIO_1_I2C1_SCL); // GPIO_1复用为I2C1_SCL
    IoTI2cInit(WIFI_IOT_I2C_IDX_1, 400000);              /* baudrate: 400kbps */
}
/***************************************************************
 * 函数功能: 通过I2C写入一个值到指定寄存器内
 * 输入参数: Addr：I2C设备地址
 *           Reg：目标寄存器
 *           Value：值
 * 返 回 值: 无
 * 说    明: 无
 **************************************************************/
static int MPU6050WriteData(uint8_t Reg, uint8_t Value)
{
    uint32_t ret;
    uint8_t send_data[2] = {Reg, Value};
    ret = IoTI2cWrite(WIFI_IOT_I2C_IDX_1, (MPU6050_SLAVE_ADDRESS << 1) | 0x00, send_data, 2);
    if (ret != 0) {
        printf("===== Error: I2C write ret = 0x%x! =====\r\n", ret);
        return -1;
    }
    return 0;
}
/***************************************************************
 * 函数功能: 通过I2C写入一段数据到指定寄存器内
 * 输入参数: Addr：I2C设备地址
 *           Reg：目标寄存器
 *           RegSize：寄存器尺寸(8位或者16位)
 *           pBuffer：缓冲区指针
 *           Length：缓冲区长度
 * 返 回 值: HAL_StatusTypeDef：操作结果
 * 说    明: 在循环调用是需加一定延时时间
 **************************************************************/
static int MPU6050WriteBuffer(uint8_t Reg, uint8_t* pBuffer, uint16_t Length)
{
    uint32_t ret = 0;
    uint8_t send_data[256] = {0};

    send_data[0] = Reg;
    for (int j = 0; j < Length; j++) {
        send_data[j + 1] = pBuffer[j];
    }

    ret = IoTI2cWrite(WIFI_IOT_I2C_IDX_1, (MPU6050_SLAVE_ADDRESS << 1) | 0x00, send_data, Length + 1);
    if (ret != 0) {
        printf("===== Error: I2C write ret = 0x%x! =====\r\n", ret);
        return -1;
    }
    return 0;
}

/***************************************************************
 * 函数功能: 通过I2C读取一段寄存器内容存放到指定的缓冲区内
 * 输入参数: Addr：I2C设备地址
 *           Reg：目标寄存器
 *           RegSize：寄存器尺寸(8位或者16位)
 *           pBuffer：缓冲区指针
 *           Length：缓冲区长度
 * 返 回 值: HAL_StatusTypeDef：操作结果
 * 说    明: 无
 **************************************************************/
static int MPU6050ReadBuffer(uint8_t Reg, uint8_t* pBuffer, uint16_t Length)
{
    uint32_t ret = 0;
    IotI2cData mpu6050_i2c_data = {0};
    uint8_t buffer[1] = {Reg};
    mpu6050_i2c_data.sendBuf = buffer;
    mpu6050_i2c_data.sendLen = 1;
    mpu6050_i2c_data.receiveBuf = pBuffer;
    mpu6050_i2c_data.receiveLen = Length;
    ret = IoTI2cWriteread(WIFI_IOT_I2C_IDX_1, (MPU6050_SLAVE_ADDRESS << 1) | 0x00, &mpu6050_i2c_data);
    if (ret != 0) {
        printf("===== Error: I2C writeread ret = 0x%x! =====\r\n", ret);
        return -1;
    }
    return 0;
}
/***************************************************************
 * 函数功能: 写数据到MPU6050寄存器
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明: 无
 ***************************************************************/
static void MPU6050WriteReg(uint8_t reg_add, uint8_t reg_dat)
{
    MPU6050WriteData(reg_add, reg_dat);
}

/***************************************************************
 * 函数功能: 从MPU6050寄存器读取数据
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明: 无
 ***************************************************************/
static int MPU6050ReadData(uint8_t reg_add, unsigned char* read, uint8_t num)
{
    return MPU6050ReadBuffer(reg_add, read, num);
}

/***************************************************************
 * 函数功能: 读取MPU6050的加速度数据
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明: 无
 ***************************************************************/
static int MPU6050ReadAcc(short* accData)
{
    int ret;
    uint8_t buf[6];
    ret = MPU6050ReadData(MPU6050_ACC_OUT, buf, 6);
    if (ret != 0) {
        return -1;
    }
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
    return 0;
}

/***************************************************************
 * 函数功能: 读取MPU6050的角速度数据
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明: 无
 ***************************************************************/
static int MPU6050ReadGyro(short* gyroData)
{
    int ret;
    uint8_t buf[6];
    ret = MPU6050ReadData(MPU6050_GYRO_OUT, buf, 6);
    if (ret != 0) {
        return -1;
    }
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5];
    return 0;
}

/***************************************************************
 * 函数功能: 读取MPU6050的原始温度数据
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明: 无
 ***************************************************************/
static int MPU6050ReadTemp(short* tempData)
{
    int ret;
    uint8_t buf[2];
    ret = MPU6050ReadData(MPU6050_RA_TEMP_OUT_H, buf, 2); // 读取温度值
    if (ret != 0) {
        return -1;
    }
    *tempData = (buf[0] << 8) | buf[1];
    return 0;
}

/***************************************************************
 * 函数功能: 读取MPU6050的温度数据，转化成摄氏度
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明: 无
 **************************************************************/
static int MPU6050ReturnTemp(short* Temperature)
{
    int ret;
    short temp3;
    uint8_t buf[2];

    ret = MPU6050ReadData(MPU6050_RA_TEMP_OUT_H, buf, 2); // 读取温度值
    if (ret != 0) {
        return -1;
    }
    temp3 = (buf[0] << 8) | buf[1];
    *Temperature = (((double)(temp3 + 13200)) / 280) - 13;
    return 0;
}

/***************************************************************
 * 函数功能: 自由落体中断
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明: 无
 **************************************************************/
void FreeFallInterrupt(void) // 自由落体中断
{
    MPU6050WriteReg(MPU6050_RA_FF_THR, 0x01); // 自由落体阈值
    MPU6050WriteReg(MPU6050_RA_FF_DUR, 0x01); // 自由落体检测时间20ms 单位1ms 寄存器0X20
}
void MotionInterrupt(void) // 运动中断
{
    MPU6050WriteReg(MPU6050_RA_MOT_THR, 0x03); // 运动阈值
    MPU6050WriteReg(MPU6050_RA_MOT_DUR, 0x14); // 检测时间20ms 单位1ms 寄存器0X20
}
void ZeroMotionInterrupt(void) // 静止中断
{
    MPU6050WriteReg(MPU6050_RA_ZRMOT_THR, 0x20); // 静止阈值
    MPU6050WriteReg(MPU6050_RA_ZRMOT_DUR, 0x20); // 静止检测时间20ms 单位1ms 寄存器0X20
}

/***************************************************************
 * 函数功能: 初始化MPU6050芯片
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明: 无
 ***************************************************************/
void MPU6050Init(void)
{
    MPU6050WriteReg(MPU6050_RA_PWR_MGMT_1, 0X80); // 复位MPU6050
    usleep(RESET_DELAY_US);
    MPU6050WriteReg(MPU6050_RA_PWR_MGMT_1, 0X00); // 唤醒MPU6050
    MPU6050WriteReg(MPU6050_RA_INT_ENABLE, 0X00); // 关闭所有中断
    MPU6050WriteReg(MPU6050_RA_USER_CTRL, 0X00);  // I2C主模式关闭
    MPU6050WriteReg(MPU6050_RA_FIFO_EN, 0X00);    // 关闭FIFO
    MPU6050WriteReg(MPU6050_RA_INT_PIN_CFG,
                    0X80); // 中断的逻辑电平模式,设置为0，中断信号为高电；设置为1，中断信号为低电平时。
    MotionInterrupt();                              // 运动中断
    MPU6050WriteReg(MPU6050_RA_CONFIG, 0x04);       // 配置外部引脚采样和DLPF数字低通滤波器
    MPU6050WriteReg(MPU6050_RA_ACCEL_CONFIG, 0x1C); // 加速度传感器量程和高通滤波器配置
    MPU6050WriteReg(MPU6050_RA_INT_PIN_CFG, 0X1C);  // INT引脚低电平平时
    MPU6050WriteReg(MPU6050_RA_INT_ENABLE, 0x40);   // 中断使能寄存器
}

/***************************************************************
 * 函数功能: 读取MPU6050的ID
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明: 无
 ***************************************************************/
int MPU6050ReadID(void)
{
    unsigned char Re = 0;
    MPU6050ReadData(MPU6050_RA_WHO_AM_I, &Re, 1); // 读器件地址
    if (Re != 0x68) {
        printf("MPU6050 dectected error!\r\n");
        return -1;
    } else {
        return 0;
    }
}
/***************************************************************
 * 函数名称: E53SC2Init
 * 说    明: 初始化E53_SC2
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
int E53SC2Init(void)
{
    uint32_t ret = 0;
    E53SC2IoInit();
    MPU6050Init();
    ret = MPU6050ReadID();
    if (ret != 0) {
        return -1;
    }
    osDelay(100);
    return 0;
}
/***************************************************************
 * 函数名称: E53SC2ReadData
 * 说    明: 读取数据
 * 参    数: 无
 * 返 回 值: 无
 ***************************************************************/
int E53SC2ReadData(E53SC2Data* ReadData)
{
    int ret;
    short Accel[3];
    short Temp;
    if (MPU6050ReadID() != 0) {
        return -1;
    }
    ret = MPU6050ReadAcc(Accel);
    if (ret != 0) {
        return -1;
    }

    ret = MPU6050ReturnTemp(&Temp);
    if (ret != 0) {
        return -1;
    }
    ReadData->Temperature = Temp;
    ReadData->Accel[ACCEL_X_AXIS] = Accel[ACCEL_X_AXIS];
    ReadData->Accel[ACCEL_Y_AXIS] = Accel[ACCEL_Y_AXIS];
    ReadData->Accel[ACCEL_Z_AXIS] = Accel[ACCEL_Z_AXIS];
    usleep(50000);
    return 0;
}
/***************************************************************
 * 函数名称: LedD1StatusSet
 * 说    明: LED_D1状态设置
 * 参    数: status,ENUM枚举的数据
 *									OFF,关
 *									ON,开
 * 返 回 值: 无
 ***************************************************************/
void LedD1StatusSet(E53SC2Status status)
{
    if (status == ON) {
        IoTGpioSetOutputVal(7, 1); // 设置GPIO_7输出高电平点亮灯
    }
    if (status == OFF) {
        IoTGpioSetOutputVal(7, 0); // 设置GPIO_7输出低电平关闭灯
    }
}

/***************************************************************
 * 函数名称: LedD2StatusSet
 * 说    明: LED_D2状态设置
 * 参    数: status,ENUM枚举的数据
 *									OFF,关
 *									ON,开
 * 返 回 值: 无
 ***************************************************************/
void LedD2StatusSet(E53SC2Status status)
{
    if (status == ON) {
        IoTGpioSetOutputVal(8, 1); // 设置GPIO_8输出高电平点亮灯
    }

    if (status == OFF) {
        IoTGpioSetOutputVal(8, 0); // 设置GPIO_8输出低电平关闭灯
    }
}
