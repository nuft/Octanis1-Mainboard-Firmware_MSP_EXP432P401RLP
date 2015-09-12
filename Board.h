/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __BOARD_H
#define __BOARD_H

#ifdef __cplusplus
extern "C" {
#endif


/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/utils/Load.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Seconds.h>
#include <ti/sysbios/hal/Timer.h>
#include <ti/sysbios/knl/Mailbox.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/PWM.h>

#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "MSP_EXP432P401RLP.h"

#include "fw/core/cli.h" //enable CLI printf everywhere
#include "fw/core/system.h" //provides useful general functions



/* Board specific I2C addresses */
#define Board_BMP180_I2CADDR					(0x77) //temp/pres
#define Board_BNO055_MAINBOARD_I2CADDR			(0x28) //IMU 1
#define Board_BNO055_WEATHERSTRIP_I2CADDR		(0x29) //IMU 2
#define Board_MCP3425AD_I2CADDR					(0x68) //ADC for UV
#define Board_SHT21_I2CADDR						(0x40) //temp/hygro
#define Board_BME280_I2CADDR					(0x76) //hygro/pres
#define Board_AS3935_I2CADDR					(0x03) //lightning

/* The default I�C address of the BNO055 device is (0x29).
 * The alternative address (0x28), can be selected by pulling COM3 down.
 */


/* General board mappings */
#define Board_initGeneral           MSP_EXP432P401RLP_initGeneral
#define Board_initGPIO              MSP_EXP432P401RLP_initGPIO
#define Board_initI2C               MSP_EXP432P401RLP_initI2C
#define Board_initPWM               MSP_EXP432P401RLP_initPWM
#define Board_initSDSPI             MSP_EXP432P401RLP_initSDSPI
#define Board_initSPI               MSP_EXP432P401RLP_initSPI
#define Board_initUART              MSP_EXP432P401RLP_initUART
#define Board_initWatchdog          MSP_EXP432P401RLP_initWatchdog

#define Board_LED_ON                MSP_EXP432P401RLP_LED_ON
#define Board_LED_OFF               MSP_EXP432P401RLP_LED_OFF

#define Board_BUTTON0               MSP_EXP432P401RLP_S1
#define Board_BUTTON1               MSP_EXP432P401RLP_S2
#define Board_LED0                  MSP_EXP432P401RLP_LED1
#define Board_LED1                  MSP_EXP432P401RLP_LED_RED
#define Board_LED2                  MSP_EXP432P401RLP_LED_RED

#define Board_ROCKBLOCK_SLEEP		MSP_EXP432P401RLP_ROCKBLOCK_SLEEP
#define Board_ROCKBLOCK_NET			MSP_EXP432P401RLP_ROCKBLOCK_NET

#define Board_HX1_EN				MSP_EXP432P401RLP_HX1_EN
#define Board_HX1_TX				MSP_EXP432P401RLP_HX1_TX



#define Board_I2C0                  MSP_EXP432P401RLP_I2CB1

#define Board_PWM0                  MSP_EXP432P401RLP_PWM_TA1_1

#define Board_SPI0                  MSP_EXP432P401RLP_SPIB0
#define Board_SPI1                  MSP_EXP432P401RLP_SPIB2

#define Board_UART0_DEBUG           MSP_EXP432P401RLP_UARTA0 //"backchannel UART"
#define Board_UART1_GPS             MSP_EXP432P401RLP_UARTA3 //P9.6,9.7
#define Board_UART2_COMM  			MSP_EXP432P401RLP_UARTA2 //P3.2,3.3
#define Board_UART3_LORACOMM		MSP_EXP432P401RLP_UARTA1 //P2.2=RX,2.3=TX

#define Board_WATCHDOG0             MSP_EXP432P401RLP_WATCHDOG



#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
