/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-07-04     Yifang       the first version
 */
#include "car_stabilize.h"
#include "./pid_lib/pid.h"
#include "encoder_read.h"

#define DBG_LEVEL DBG_INFO
#define DBG_SECTION_NAME  "car_stab"
#include <rtdbg.h>

//PID(PID_TypeDef *uPID, double *Input, double *Output, double *Setpoint, double Kp, double Ki, double Kd, PIDPON_TypeDef POn, PIDCD_TypeDef ControllerDirection);
//
//PID_SetMode(&TPID, _PID_MODE_AUTOMATIC);
//PID_SetSampleTime(&TPID, 500);
//PID_SetOutputLimits(&TPID, 1, 100);
//
//PID_Compute(&TPID);

PID_TypeDef TPID;

char OutBuf[50];
double speed, PIDOut, speed_set_point;

void pid_thread_entry(void)
{
    while(1)
    {
        speed = Get_Vercel_Encoder('A');
        PID_Compute(&TPID);

//        sprintf(OutBuf, "speed%3.2f : %u\n", speed, (uint16_t)PIDOut);
//        UART_Transmit((uint8_t *)OutBuf, strlen(OutBuf));
        rt_kprintf("PIDOut: %lf",PIDOut);

        rt_thread_mdelay(1000);
    }
}

void pid_test(void)
{
    speed_set_point = 1.0;

    PID(&TPID, &speed, &PIDOut, &speed_set_point, 2, 5, 1, _PID_P_ON_E, _PID_CD_DIRECT);

    PID_SetMode(&TPID, _PID_MODE_AUTOMATIC);
    PID_SetSampleTime(&TPID, 500);
    PID_SetOutputLimits(&TPID, 1, 100);

    rt_thread_t ret = rt_thread_create("pid_thread", pid_thread_entry, RT_NULL, 2048, 19, 100);
    if(ret == RT_NULL)
    {
        LOG_E("encoder_read_thread create failed!\n");
    }
    rt_thread_startup(ret);

    return RT_EOK;
}
MSH_CMD_EXPORT(pid_test,pid_test);
