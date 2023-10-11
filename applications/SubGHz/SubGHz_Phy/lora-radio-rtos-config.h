/*!
 * \file      lora-radio-rtos-config.h
 *
 * \brief     adapter to different RTOS implementation
 *
 * \copyright SPDX-License-Identifier: Apache-2.0
 *
 * \author    Forest-Rain
 */

#ifndef __LORA_RADIO_RTOS_CONFIG_H_
#define __LORA_RADIO_RTOS_CONFIG_H_

#define LORA_RADIO_DRIVER_USING_RTOS_RT_THREAD
//#define LORA_RADIO_DRIVER_USING_RTOS_RT_THREAD_NANO
//#define USING_RTOS_CONTIKI_NG
//#define USING_RTOS_ALIOS_THINGS

#ifdef LORA_RADIO_DRIVER_USING_RTOS_RT_THREAD
#elif defined LORA_RADIO_DRIVER_USING_RTOS_RT_THREAD_NANO
#elif defined USING_RTOS_CONTIKI_NG
#elif defined USING_RTOS_ALIOS_THINGS
#else
    #error "Please Choose your RTOS setup!"
#endif

#ifdef LORA_RADIO_DRIVER_USING_RTOS_RT_THREAD
#include <rtthread.h>
#include <rtdevice.h>
//#include "drv_spi.h"

#define LOG_LVL_ASSERT                 0
#define LOG_LVL_ERROR                  3
#define LOG_LVL_WARNING                4
#define LOG_LVL_INFO                   6
#define LOG_LVL_DBG                    7

#endif

#endif // end of __LORA_RADIO_RTOS_CONFIG_H_
