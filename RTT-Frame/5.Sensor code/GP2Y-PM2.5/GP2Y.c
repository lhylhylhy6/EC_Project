/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-04-25     Yifang       the first version
 */
#include "GP2Y.h"
#include "rtdef.h"
#include "stdint.h"

static uint16_t AD_PM;

#define ADC_DEV_NAME        "adc1"      /* ADC 设备名称 */
#define ADC_DEV_CHANNEL     1           /* ADC 通道 */       //PA1

void GP2Y_Value_read(void)
{
    rt_adc_device_t adc_dev;
    rt_uint32_t value;
    rt_err_t ret = RT_EOK;

    float pm;

    /* 查找设备 */
    adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);
    if (adc_dev == RT_NULL)
    {
        rt_kprintf("adc sample run failed! can't find %s device!\n", ADC_DEV_NAME);
        return RT_ERROR;
    }

    /* 使能设备 */
    ret = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL);

    /* 读取采样值 */
    value = rt_adc_read(adc_dev, ADC_DEV_CHANNEL);
    rt_kprintf("the value is :%d \n", value);

    GP2Y_Low;
    rt_thread_mdelay(280);
    AD_PM = rt_adc_read(adc_dev,ADC_DEV_CHANNEL); //PA0
    rt_thread_mdelay(40);
    GP2Y_High;
    rt_thread_mdelay(9680);
    pm = 0.17*AD_PM-0.1; //电压-灰尘转换
    rt_kprintf("%f\n",pm);
}
MSH_CMD_EXPORT(GP2Y_Value_read,GP2Y_Value_read)
