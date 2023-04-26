/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-04-25     Yifang       the first version
 */
#ifndef APPLICATIONS_GP2Y_H_
#define APPLICATIONS_GP2Y_H_

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define GP2Y_PIN    GET_PIN(A,12)

#define GP2Y_High rt_pin_write(GP2Y_PIN,PIN_HIGH)
#define GP2Y_Low  rt_pin_write(GP2Y_PIN,PIN_LOW)

void GP2Y_Value_read(void);

#endif /* APPLICATIONS_GP2Y_H_ */
