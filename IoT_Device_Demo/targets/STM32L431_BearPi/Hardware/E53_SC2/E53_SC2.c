/********************************************************************************
    * 文件名称 ：E53_SC2.c
    * 作     者：小熊派开源社区
    * 版     本：V1.0
    * 编写日期 ：2019-5-27
    * 功     能：E53_SC2扩展板驱动
*********************************************************************************
    * 说    明 ：本例程配套小熊派开源社区开发板使用
    *
    * 淘     宝：https://shop336827451.taobao.com/
    * 论     坛：https://bbs.huaweicloud.com/forum/forum-734-1.html
    * 微信公众号：小熊派开源社区
*********************************************************************************/
#include "E53_SC2.h"
#include "i2c.h"
#include "stm32l4xx.h"
#include "dwt.h"

#include<math.h>

#define EVAL_I2Cx_TIMEOUT_MAX                   3000
uint32_t I2cxTimeout = EVAL_I2Cx_TIMEOUT_MAX;
/***************************************************************
  * 函数功能: 写数据到MPU6050寄存器
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  ***************************************************************/
void MPU6050_WriteReg(uint8_t reg_add,uint8_t reg_dat,short iicno)
{
  I2C_MPU6050_WriteData(MPU6050_SLAVE_ADDRESS,reg_add,reg_dat,iicno);
}

/***************************************************************
  * 函数功能: 从MPU6050寄存器读取数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
***************************************************************/ 
void MPU6050_ReadData(uint8_t reg_add,unsigned char *Read,uint8_t num, short iicno)
{
  I2C_MPU6050_ReadBuffer(MPU6050_SLAVE_ADDRESS,reg_add,I2C_MEMADD_SIZE_8BIT,Read,num,iicno);
}

/***************************************************************
  * 函数功能: 初始化MPU6050芯片
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  ***************************************************************/ 
void MPU6050_Init(short iicno)
{
  int i=0,j=0;
  //在初始化之前要延时一段时间，若没有延时，则断电后再上电数据可能会出错
  for(i=0;i<1000;i++)
  {
    for(j=0;j<1000;j++)
    {
      ;
    }
  }
	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1,0X80,iicno);       //复位MPU6050
	delay10ms(10);
	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1,0X00,iicno);       //唤醒MPU6050
	MPU6050_WriteReg(MPU6050_RA_INT_ENABLE,0X00,iicno);       //关闭所有中断
	MPU6050_WriteReg(MPU6050_RA_USER_CTRL,0X00,iicno);        //I2C主模式关闭
	MPU6050_WriteReg(MPU6050_RA_FIFO_EN,0X00,iicno);          //关闭FIFO
	MPU6050_WriteReg(MPU6050_RA_INT_PIN_CFG,0X80,iicno);      //中断的逻辑电平模式,设置为0，中断信号为高电；设置为1，中断信号为低电平时。
	Motion_Interrupt(iicno);           					           	//运动中断
	MPU6050_WriteReg(MPU6050_RA_CONFIG,0x04,iicno);           //配置外部引脚采样和DLPF数字低通滤波器
	MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG,0x00,iicno);     //加速度传感器量程和高通滤波器配置1c
  MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG,0x18,iicno);
	MPU6050_WriteReg(MPU6050_RA_INT_PIN_CFG,0X1C,iicno);      //INT引脚低电平平时
	MPU6050_WriteReg(MPU6050_RA_INT_ENABLE,0x40,iicno);       //中断使能寄存器
}

/***************************************************************
  * 函数功能: 读取MPU6050的ID
  * 输入参数: 需要读取的串行线标识符
  * 返 回 值: 无
  * 说    明: 无
  ***************************************************************/ 
uint8_t MPU6050ReadID(short iicno)
{
	unsigned char Re = 0;
	MPU6050_ReadData(MPU6050_RA_WHO_AM_I,&Re,1,iicno);    //读器件地址
	if(Re != 0x68)
	{
		printf("MPU6050 dectected error!\r\n");
		return 0;
	}
	else
	{
		//printf("MPU6050 ID = %d\r\n",Re);
		return 1;
	}
}

/***************************************************************
  * 函数功能: 读取MPU6050的加速度数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
***************************************************************/ 
void MPU6050ReadAcc(short *accData, short iicno)
{
    uint8_t buf[6];
    MPU6050_ReadData(MPU6050_ACC_OUT, buf, 6, iicno);
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
}


/***************************************************************
  * 函数功能: 读取MPU6050的角速度数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
 ***************************************************************/ 
void MPU6050ReadGyro(short *gyroData, short iicno)
{
    uint8_t buf[6];
    MPU6050_ReadData(MPU6050_GYRO_OUT,buf,6,iicno);
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5];
}


/***************************************************************
  * 函数功能: 读取MPU6050的原始温度数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  ***************************************************************/
void MPU6050ReadTemp(short *tempData,short iicno)
{
	uint8_t buf[2];
    MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2,iicno);     //读取温度值
    *tempData = (buf[0] << 8) | buf[1];
}

/***************************************************************
  * 函数功能: 读取MPU6050的温度数据，转化成摄氏度
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  **************************************************************/ 
void MPU6050_ReturnTemp(short*Temperature, short iicno)
{
	short temp3;
	uint8_t buf[2];
	
	MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2,iicno);     //读取温度值
	temp3= (buf[0] << 8) | buf[1];
	*Temperature=(((double) (temp3 + 13200)) / 280)-13;
}

/***************************************************************
  * 函数功能: 自由落体中断
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  **************************************************************/
void Free_Fall_Interrupt(short iicno)          //自由落体中断
{
    MPU6050_WriteReg(MPU6050_RA_FF_THR,0x01,iicno);             //自由落体阈值
    MPU6050_WriteReg(MPU6050_RA_FF_DUR,0x01,iicno);             //自由落体检测时间20ms 单位1ms 寄存器0X20
}
void Motion_Interrupt(short iicno)             //运动中断
{
    MPU6050_WriteReg(MPU6050_RA_MOT_THR,0x03,iicno);            //运动阈值
    MPU6050_WriteReg(MPU6050_RA_MOT_DUR,0x14,iicno);            //检测时间20ms 单位1ms 寄存器0X20
}
void Zero_Motion_Interrupt(short iicno)        //静止中断
{
    MPU6050_WriteReg(MPU6050_RA_ZRMOT_THR,0x20,iicno);          //静止阈值
    MPU6050_WriteReg(MPU6050_RA_ZRMOT_DUR,0x20,iicno);          //静止检测时间20ms 单位1ms 寄存器0X20
}

/***************************************************************
  * 函数功能: I2C通信错误处理函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 一般在I2C通信超时时调用该函数
  **************************************************************/
static void I2C_MPU6050_Error (short iicno)
{
  /* 反初始化I2C通信总线 */
  if(iicno == 1)
  {
    HAL_I2C_DeInit(&hi2c1);
  }
  if(iicno == 3)
  {
    HAL_I2C_DeInit(&hi2c3);
  }
  
  /* 重新初始化I2C通信总线*/
  printf("MPU6050 I2C Delay！！！ Reset I2C...\n");
}

/***************************************************************
  * 函数功能: 通过I2C写入一个值到指定寄存器内
  * 输入参数: Addr：I2C设备地址
  *           Reg：目标寄存器
  *           Value：值
  * 返 回 值: 无
  * 说    明: 无
  **************************************************************/
void I2C_MPU6050_WriteData(uint16_t Addr, uint8_t Reg, uint8_t Value, short iicno)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  if(iicno == 1)
  {
    status = HAL_I2C_Mem_Write(&hi2c1, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, I2cxTimeout);
  }
  if(iicno == 3)
  {
    status = HAL_I2C_Mem_Write(&hi2c3, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, I2cxTimeout);
  }
  
  /* 检测I2C通信状态 */
  if(status != HAL_OK)
  {
    /* 调用I2C通信错误处理函数 */
    I2C_MPU6050_Error(iicno);
  }
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
HAL_StatusTypeDef I2C_MPU6050_WriteBuffer(uint16_t Addr, uint8_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length, short iicno)
{
  HAL_StatusTypeDef status = HAL_OK;
  
  if(iicno == 1)
  {
    status = HAL_I2C_Mem_Write(&hi2c1, Addr, (uint16_t)Reg, RegSize, pBuffer, Length, I2cxTimeout); 

  }
  if(iicno == 3)
  {
    status = HAL_I2C_Mem_Write(&hi2c3, Addr, (uint16_t)Reg, RegSize, pBuffer, Length, I2cxTimeout); 

  }
  
  /* 检测I2C通信状态 */
  if(status != HAL_OK)
  {
    /* 调用I2C通信错误处理函数 */
    I2C_MPU6050_Error(iicno);
  }        
  return status;
}


/***************************************************************
  * 函数功能: 通过I2C读取一个指定寄存器内容
  * 输入参数: Addr：I2C设备地址
  *           Reg：目标寄存器
  * 返 回 值: uint8_t：寄存器内容
  * 说    明: 无
  **************************************************************/
uint8_t I2C_MPU6050_ReadData(uint16_t Addr, uint8_t Reg, short iicno)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t value = 0;
  
  if(iicno == 1)
  {
    status = HAL_I2C_Mem_Read(&hi2c1, Addr, Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, I2cxTimeout);
  }
  if(iicno == 3)
  {
    status = HAL_I2C_Mem_Read(&hi2c3, Addr, Reg, I2C_MEMADD_SIZE_8BIT, &value, 1, I2cxTimeout);
  }
  
 
  /* 检测I2C通信状态 */
  if(status != HAL_OK)
  {
    /* 调用I2C通信错误处理函数 */
    I2C_MPU6050_Error(iicno);
  }
  return value;
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
HAL_StatusTypeDef I2C_MPU6050_ReadBuffer(uint16_t Addr, uint8_t Reg, uint16_t RegSize, uint8_t *pBuffer, uint16_t Length, short iicno)
{
  HAL_StatusTypeDef status = HAL_OK;
  if(iicno == 1)
  {
    status = HAL_I2C_Mem_Read(&hi2c1, Addr, (uint16_t)Reg, RegSize, pBuffer, Length, I2cxTimeout);
  }
  if(iicno == 3)
  {
    status = HAL_I2C_Mem_Read(&hi2c3, Addr, (uint16_t)Reg, RegSize, pBuffer, Length, I2cxTimeout);
  }
  /* 检测I2C通信状态 */
  if(status != HAL_OK)
  {
    /* 调用I2C通信错误处理函数 */
    I2C_MPU6050_Error(iicno);
  }        
  return status;
}

/***************************************************************
  * 函数功能: 检测I2C设备是否处于准备好可以通信状态
  * 输入参数: DevAddress：I2C设备地址
  *           Trials：尝试测试次数
  * 返 回 值: HAL_StatusTypeDef：操作结果
  * 说    明: 无
  **************************************************************/
HAL_StatusTypeDef I2C_MPU6050_IsDeviceReady(uint16_t DevAddress, uint32_t Trials, short iicno)
{ 
  if(iicno == 1)
  {
    return (HAL_I2C_IsDeviceReady(&hi2c1, DevAddress, Trials, I2cxTimeout));
  }
  if(iicno == 3)
  {
    return (HAL_I2C_IsDeviceReady(&hi2c3, DevAddress, Trials, I2cxTimeout));
  }

}

/***************************************************************
* 函数名称: Init_E53_SC2
* 说    明: 初始化Init_E53_SC2
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_E53_SC2(void)
{
    MX_I2C1_Init();
    MX_I2C3_Init();
    MPU6050_Init(1);
    MPU6050_Init(3);
    Init_Light();
}
/***************************************************************
* 函数名称: E53_SC2_Read_Data
* 说    明: 读取数据
* 参    数: 无
* 返 回 值: 无
***************************************************************/

#define cn_app_Manhole_Cover 0x0

float fRad2Deg = 57.295779513f;//将弧度转为角度的乘数

float GetRoll(short *pRealVals, float fNorm) {
  float fNormXZ = sqrt(pRealVals[0] * pRealVals[0] + pRealVals[2] * pRealVals[2]);
  float fCos = fNormXZ / fNorm;
  return acos(fCos) * fRad2Deg;
}

float GetPitch(short *pRealVals, float fNorm) {
  float fNormYZ = sqrt(pRealVals[1] * pRealVals[1] + pRealVals[2] * pRealVals[2]);
  float fCos = fNormYZ / fNorm;
  return acos(fCos) * fRad2Deg;
}

void E53_SC2_Read_Data(void)
{
	short  Accel[3];
  short  Gyro[3];
	short  Temp;
  float fNorm;
  short readouts[7];
  short  Accel3[3];
  short  Gyro3[3];
	short  Temp3;
  float fNorm3;
  short readouts3[7];
	if (MPU6050ReadID(1) == 0||MPU6050ReadID(3) == 0)
	{	
	  while(1);
  }
      MPU6050ReadAcc(Accel,1);
      MPU6050ReadGyro(Gyro,1);			
      MPU6050_ReturnTemp(&Temp,1);
      E53_SC2_Data.Temperature = Temp;
      E53_SC2_Data.Accel[0] = Accel[0];
      E53_SC2_Data.Accel[1] = Accel[1];
      E53_SC2_Data.Accel[2] = Accel[2];
      E53_SC2_Data.Gyro[0] = Gyro[0];
      E53_SC2_Data.Gyro[1] = Gyro[1];
      E53_SC2_Data.Gyro[2] = Gyro[2];
      readouts[0] = E53_SC2_Data.Accel[0];
      readouts[1] = E53_SC2_Data.Accel[1];
      readouts[2] = E53_SC2_Data.Accel[2];
      readouts[3] = E53_SC2_Data.Temperature;
      readouts[4] = E53_SC2_Data.Gyro[0];
      readouts[5] = E53_SC2_Data.Gyro[1];
      readouts[6] = E53_SC2_Data.Gyro[2];
      fNorm = sqrt(E53_SC2_Data.Accel[0] * E53_SC2_Data.Accel[0] + E53_SC2_Data.Accel[1] * E53_SC2_Data.Accel[1] + E53_SC2_Data.Accel[2] * E53_SC2_Data.Accel[2]);
      E53_SC2_Data.roll = (short)GetRoll(readouts, fNorm);
      E53_SC2_Data.pitch =  (short)GetPitch(readouts, fNorm);

      MPU6050ReadAcc(Accel3,3);
      MPU6050ReadGyro(Gyro3,3);			
      MPU6050_ReturnTemp(&Temp3,3);
      E53_SC2_Data3.Temperature = Temp3;
      E53_SC2_Data3.Accel[0] = Accel3[0];
      E53_SC2_Data3.Accel[1] = Accel3[1];
      E53_SC2_Data3.Accel[2] = Accel3[2];
      E53_SC2_Data3.Gyro[0] = Gyro3[0];
      E53_SC2_Data3.Gyro[1] = Gyro3[1];
      E53_SC2_Data3.Gyro[2] = Gyro3[2];
      readouts3[0] = E53_SC2_Data3.Accel[0];
      readouts3[1] = E53_SC2_Data3.Accel[1];
      readouts3[2] = E53_SC2_Data3.Accel[2];
      readouts3[3] = E53_SC2_Data3.Temperature;
      readouts3[4] = E53_SC2_Data3.Gyro[0];
      readouts3[5] = E53_SC2_Data3.Gyro[1];
      readouts3[6] = E53_SC2_Data3.Gyro[2];
      fNorm3 = sqrt(E53_SC2_Data3.Accel[0] * E53_SC2_Data3.Accel[0] + E53_SC2_Data3.Accel[1] * E53_SC2_Data3.Accel[1] + E53_SC2_Data3.Accel[2] * E53_SC2_Data3.Accel[2]);
      E53_SC2_Data3.roll = (short)GetRoll(readouts3, fNorm3);
      E53_SC2_Data3.pitch =  (short)GetPitch(readouts3, fNorm3);
      delay10ms(50);
}





/***************************************************************
* 函数名称: Init_Light
* 说    明: 初始化Init_E53_SC2的提示灯
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void Init_Light(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}



