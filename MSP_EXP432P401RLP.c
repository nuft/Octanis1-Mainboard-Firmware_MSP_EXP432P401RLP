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

/*
 *  ======== MSP_EXP432P401RLP.c ========
 *  This file is responsible for setting up the board specific items for the
 *  MSP_EXP432P401RLP board.
 */

#include <stdbool.h>

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/family/arm/m3/Hwi.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>

#include <msp.h>
#include <rom.h>
#include <rom_map.h>
#include <dma.h>
#include <gpio.h>
#include <i2c.h>
#include <pmap.h>
#include <spi.h>
#include <timer_a.h>
#include <uart.h>
#include <wdt_a.h>

#include "MSP_EXP432P401RLP.h"

/*
 *  =============================== DMA ===============================
 */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_ALIGN(dmaControlTable, 256)
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=256
#elif defined(__GNUC__)
__attribute__ ((aligned (256)))
#endif
static DMA_ControlTable dmaControlTable[8];
static bool dmaInitialized = false;

/* Hwi_Struct used in the initDMA Hwi_construct call */
static Hwi_Struct dmaHwiStruct;

/*
 *  ======== dmaErrorHwi ========
 */
static Void dmaErrorHwi(UArg arg)
{
    System_printf("DMA error code: %d\n", MAP_DMA_getErrorStatus());
    MAP_DMA_clearErrorStatus();
    System_abort("DMA error!!");
}

/*
 *  ======== MSP_EXP432P401RLP_initDMA ========
 */
void MSP_EXP432P401RLP_initDMA(void)
{
    Error_Block eb;
    Hwi_Params  hwiParams;

    if (!dmaInitialized) {
        Error_init(&eb);
        Hwi_Params_init(&hwiParams);
        Hwi_construct(&(dmaHwiStruct), INT_DMA_ERR, dmaErrorHwi, &hwiParams,
            &eb);
        if (Error_check(&eb)) {
            System_abort("Couldn't construct DMA error hwi");
        }
        MAP_DMA_enableModule();
        MAP_DMA_setControlBase(dmaControlTable);

        dmaInitialized = true;
    }
}

/*
 *  =============================== General ===============================
 */

/*
 *  ======== MSP_EXP432P401RLP_initGeneral ========
 */
void MSP_EXP432P401RLP_initGeneral(void)
{
    Power_init();
}

/*
 *  =============================== GPIO ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(GPIOMSP432_config, ".const:GPIOMSP432_config")
#endif

#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOMSP432.h>

/*
 * Array of Pin configurations
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in MSP_EXP432P401RLP.h
 * NOTE: Pins not used for interrupts should be placed at the end of the
 *       array.  Callback entries can be omitted from callbacks array to
 *       reduce memory usage.
 */
GPIO_PinConfig gpioPinConfigs[] = {
    /* Input pins */
    /* MSP_EXP432P401RLP_S1 */
//    GPIOMSP432_P1_1 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
    /* MSP_EXP432P401RLP_S2 */
//    GPIOMSP432_P1_4 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
	/* MSP_EXP432P401RLP_ROCKBLOCK_NET */
	GPIOMSP432_P4_7 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
	/* MSP_EXP432P401RLP_ULTRASONIC_IN_0 */
	//GPIOMSP432_P5_7 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_BOTH_EDGES,
	/* Octanis_LIGHTNING_INT */
	GPIOMSP432_P1_4 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING,
	/*Octanis_GEIGER_COUNTER*/
	GPIOMSP432_P1_0 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING,


	/* Output pins */
    /* Octanis_LED1 (green) */
    GPIOMSP432_P10_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

    /* Octanis_LED2 (red) */
    GPIOMSP432_P10_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

    /* Octanis_ROCKBLOCK_SLEEP */
    GPIOMSP432_P4_6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

    /* Octanis_LORA_RESET_N */
    GPIOMSP432_P9_4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,

	/* Octanis_ULTRASONIC_TRIGGER */
	GPIOMSP432_PJ_3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_ULTRASONIC_SLEEP */
	GPIOMSP432_PJ_2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_WINDSENSOR_SLEEP */
	GPIOMSP432_P7_2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,

#ifdef VERSION_0_5
	/* Octanis_M1234_SLEEP */
	GPIOMSP432_P2_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M1_PH */
	GPIOMSP432_P3_6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M2_PH */
	GPIOMSP432_P2_5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M3_PH */
	GPIOMSP432_P2_7 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M4_PH */
	GPIOMSP432_P10_5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M1_EN */
	GPIOMSP432_P2_4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M2_EN */
	GPIOMSP432_P2_6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M3_EN */
	GPIOMSP432_P10_4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M4_EN */
	GPIOMSP432_P7_4 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M5678_SLEEP */
	GPIOMSP432_P3_5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M5_PH */
	GPIOMSP432_P7_5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M6_PH */
	GPIOMSP432_P8_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M7_PH */
	GPIOMSP432_P8_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M8_PH */
	GPIOMSP432_P3_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

#else //new boards from version 0.6 onwards
	/* Octanis_M1234_SLEEP */
	GPIOMSP432_P8_7 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M1_PH */
	GPIOMSP432_P9_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M2_PH */
	GPIOMSP432_P6_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M3_PH */
	GPIOMSP432_P4_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M4_PH */
	GPIOMSP432_P4_2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M1_EN */
	GPIOMSP432_P9_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M2_EN */
	GPIOMSP432_P6_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M3_EN */
	GPIOMSP432_P4_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M4_EN */
	GPIOMSP432_P4_3 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M5678_ON */
	GPIOMSP432_P3_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M5_IN2 */
	GPIOMSP432_P8_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M6_IN2 */
	GPIOMSP432_P8_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M7_IN2 */
	GPIOMSP432_P3_5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M8_IN2 */
	GPIOMSP432_P3_7 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

#endif

	/* Octanis_M5678_CURR_SENS_EN */
	GPIOMSP432_P8_6 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/* Octanis_M1_ANGLE_ENCODER_CS */
	GPIOMSP432_P6_2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,

	/*UV_EN*/
	GPIOMSP432_P1_5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

	/*GEIGER_EN*/
	GPIOMSP432_P9_5 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW,

};

/*
 * Array of callback function pointers
 * NOTE: The order of the pin configurations must coincide with what was
 *       defined in MSP_EXP432P401RLP.h
 * NOTE: Pins not used for interrupts can be omitted from callbacks array to
 *       reduce memory usage (if placed at end of gpioPinConfigs array).
 */
GPIO_CallbackFxn gpioCallbackFunctions[] = {
    /* MSP_EXP432P401RLP_S1 */
    NULL,
    /* MSP_EXP432P401RLP_S2 */
    NULL
};

/* The device-specific GPIO_config structure */
const GPIOMSP432_Config GPIOMSP432_config = {
    .pinConfigs = (GPIO_PinConfig *) gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *) gpioCallbackFunctions,
    .numberOfPinConfigs = sizeof(gpioPinConfigs) / sizeof(GPIO_PinConfig),
    .numberOfCallbacks = sizeof(gpioCallbackFunctions)/sizeof(GPIO_CallbackFxn),
    .intPriority = (~0)
};

/*
 *  ======== MSP_EXP432P401RLP_initGPIO ========
 */
void MSP_EXP432P401RLP_initGPIO(void)
{
    /* Initialize peripheral and pins */
    GPIO_init();
}

/*
 *  =============================== I2C ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(I2C_config, ".const:I2C_config")
#pragma DATA_SECTION(i2cMSP432HWAttrs, ".const:i2cMSP432HWAttrs")
#endif

#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CMSP432.h>

/* I2C objects */
I2CMSP432_Object i2cMSP432Objects[MSP_EXP432P401RLP_I2CCOUNT];

/* I2C configuration structure */
const I2CMSP432_HWAttrs i2cMSP432HWAttrs[MSP_EXP432P401RLP_I2CCOUNT] = {
    {
        .baseAddr = EUSCI_B0_BASE, //P1.6, P1.7
        .intNum = INT_EUSCIB0,
        .intPriority = ~0,
        .clockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK
    }
};

const I2C_Config I2C_config[] = {
    {
        .fxnTablePtr = &I2CMSP432_fxnTable,
        .object = &i2cMSP432Objects[0],
        .hwAttrs = &i2cMSP432HWAttrs[0]
    },
    {NULL, NULL, NULL}
};

/*
 *  ======== MSP_EXP432P401RLP_initI2C ========
 */
void MSP_EXP432P401RLP_initI2C(void)
{
    /*
     * NOTE: TI-RTOS examples configure EUSCIB0 as either SPI or I2C.  Thus,
     * a conflict occurs when the I2C & SPI drivers are used simultaneously in
     * an application.  Modify the pin mux settings in this file and resolve the
     * conflict before running your the application.
     */
    /* Configure Pins 1.6 & 1.7 as SDA & SCL, respectively. */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
        GPIO_PIN6 | GPIO_PIN7	, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Initialize the I2C driver */
    I2C_init();
}

/*
 *  =============================== Power ===============================
 */
const PowerMSP432_Config PowerMSP432_config = {
    .policyInitFxn = &PowerMSP432_policyInitFxn,
    .policyFxn = &PowerMSP432_policyFxn,
    .initialPerfLevel = 2,
    .enablePolicy = false,
    .enablePerf = true
};

/*
 *  =============================== PWM ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(PWM_config, ".const:PWM_config")
#pragma DATA_SECTION(pwmTimerMSP432HWAttrs, ".const:pwmTimerMSP432HWAttrs")
#endif

#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMTimerMSP432.h>

PWMTimerMSP432_Object pwmTimerMSP432Objects[MSP_EXP432P401RLP_PWMCOUNT];

/* PWM configuration structure */
const PWMTimerMSP432_HWAttrs pwmTimerMSP432HWAttrs[MSP_EXP432P401RLP_PWMCOUNT] = {

#ifdef VERSION_0_5
	{
		.baseAddr = TIMER_A1_BASE,
		.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2
	},
	{
		.baseAddr = TIMER_A1_BASE,
		.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1
	},
	{
		.baseAddr = TIMER_A1_BASE,
		.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3
	},
	{
		.baseAddr = TIMER_A1_BASE,
		.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4
	}
#else
	{
		.baseAddr = TIMER_A1_BASE,
		.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1
	},
	{
		.baseAddr = TIMER_A1_BASE,
		.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2
	},
	{
		.baseAddr = TIMER_A1_BASE,
		.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3
	},
	{
		.baseAddr = TIMER_A1_BASE,
		.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4
	}
#endif

};

const PWM_Config PWM_config[] = {
    {
        .fxnTablePtr = &PWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[0],
        .hwAttrs = &pwmTimerMSP432HWAttrs[0]
    },
    {
        .fxnTablePtr = &PWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[1],
        .hwAttrs = &pwmTimerMSP432HWAttrs[1]
    },
    {
        .fxnTablePtr = &PWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[2],
        .hwAttrs = &pwmTimerMSP432HWAttrs[2]
    },
    {
        .fxnTablePtr = &PWMTimerMSP432_fxnTable,
        .object = &pwmTimerMSP432Objects[3],
        .hwAttrs = &pwmTimerMSP432HWAttrs[3]
    },
    {NULL, NULL, NULL}
};

/*
 *  ======== MSP_EXP432P401RLP_initPWM ========
 */
void MSP_EXP432P401RLP_initPWM(void)
{
    /* Use Port Map on Port 7 to test pwm */
	//TODO this is only for testing purposes
#ifdef VERSION_0_5
    const uint8_t port7Map [] = {
        PM_NONE, PM_NONE,  PM_NONE, PM_TA0CCR1A, //note: P7.3 was defined as windsensor input
        PM_NONE, PM_NONE,  PM_TA1CCR2A, PM_TA1CCR1A
    };

    const uint8_t port3Map [] = {
    		PM_TA1CCR3A, PM_NONE, PM_NONE, PM_NONE,
		PM_TA1CCR4A, PM_NONE, PM_NONE, PM_NONE
	};
#else
    const uint8_t port7Map [] = {
        PM_NONE, PM_NONE, PM_NONE, PM_TA0CCR1A, //note: P7.3 was defined as windsensor input
        PM_NONE, PM_NONE, PM_NONE, PM_TA1CCR1A
    };

    const uint8_t port3Map [] = {
    		PM_TA1CCR2A, PM_NONE, PM_NONE, PM_NONE,
		PM_TA1CCR3A, PM_NONE, PM_TA1CCR4A, PM_NONE
	};

#endif
    /* Mapping capture compare registers to Port 7 */
    MAP_PMAP_configurePorts((const uint8_t *) port7Map, P7MAP, 1,
        PMAP_ENABLE_RECONFIGURATION);

    /* Mapping capture compare registers to Port 3 */
    MAP_PMAP_configurePorts((const uint8_t *) port3Map, P3MAP, 1,
        PMAP_ENABLE_RECONFIGURATION);

    /* Enable PWM output on GPIO pins */
#ifdef VERSION_0_5
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7,
    		GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7,
    		GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3,
        GPIO_PIN0, GPIO_PRIMARY_MODULE_FUNCTION);

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3,
		GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
#else
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7,
    		GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3,
    		GPIO_PIN0, GPIO_PRIMARY_MODULE_FUNCTION);

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3,
        GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3,
		GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
#endif
    PWM_init();
}

/*
// *  =============================== SDSPI ===============================
// */
///* Place into subsections to allow the TI linker to remove items properly */
//#if defined(__TI_COMPILER_VERSION__)
//#pragma DATA_SECTION(SDSPI_config, ".const:SDSPI_config")
//#pragma DATA_SECTION(sdspiMSP432HWAttrs, ".const:sdspiMSP432HWAttrs")
//#endif
//
//#include <ti/drivers/SDSPI.h>
//#include <ti/drivers/sdspi/SDSPIMSP432.h>
//
/////* SDSPI objects */
////SDSPIMSP432_Object sdspiMSP432Objects[MSP_EXP432P401RLP_SDSPICOUNT];
////
///* SDSPI configuration structure, describing which pins are to be used */
//const SDSPIMSP432_HWAttrs sdspiMSP432HWAttrs[MSP_EXP432P401RLP_SDSPICOUNT] = {
//    {
//        .baseAddr = EUSCI_B1_BASE,
//        .clockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
//
//        /* CLK, MOSI & MISO ports & pins */
//        .portSCK = GPIO_PORT_P1,
//        .pinSCK = GPIO_PIN5,
//        .sckMode = GPIO_PRIMARY_MODULE_FUNCTION,
//
//        .portMISO = GPIO_PORT_P1,
//        .pinMISO = GPIO_PIN7,
//        .misoMode = GPIO_PRIMARY_MODULE_FUNCTION,
//
//        .portMOSI = GPIO_PORT_P1,
//        .pinMOSI = GPIO_PIN6,
//        .mosiMode = GPIO_PRIMARY_MODULE_FUNCTION,
//
//        /* Chip select port & pin */
//        .portCS = GPIO_PORT_P4,
//        .pinCS = GPIO_PIN6
//    }
//};
//
//const SDSPI_Config SDSPI_config[] = {
//    {
//        .fxnTablePtr = &SDSPIMSP432_fxnTable,
//        .object = &sdspiMSP432Objects[0],
//        .hwAttrs = &sdspiMSP432HWAttrs[0]
//    },
//    {NULL, NULL, NULL}
//};
//
///*
// *  ======== MSP_EXP432P401RLP_initSDSPI ========
// */
//void MSP_EXP432P401RLP_initSDSPI(void)
//{
//    SDSPI_init();
//}

/*
 *  =============================== SPI ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(SPI_config, ".const:SPI_config")
#pragma DATA_SECTION(spiMSP432DMAHWAttrs, ".const:spiMSP432DMAHWAttrs")
#endif

#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPIMSP432DMA.h>

/* SPI objects */
SPIMSP432DMA_Object spiMSP432DMAObjects[Octanis_SPICOUNT];

/* SPI configuration structure */
const SPIMSP432DMA_HWAttrs spiMSP432DMAHWAttrs[Octanis_SPICOUNT] = {
    {
        .baseAddr = EUSCI_B1_BASE,
        .bitOrder = EUSCI_B_SPI_MSB_FIRST,
        .clockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK,

        .defaultTxBufValue = 0,

        .dmaIntNum = INT_DMA_INT1,
		/* note: DMA_INT1, DMA_INT2, DMA_INT3: Can be mapped to the DMA completion
		 * event of any of the eight channels
		 */
        .intPriority = ~0,
        .rxDMAChannelIndex = DMA_CH3_EUSCIB1RX0,
        .txDMAChannelIndex = DMA_CH2_EUSCIB1TX0
    },
//    {
//        .baseAddr = EUSCI_B2_BASE,
//        .bitOrder = EUSCI_B_SPI_MSB_FIRST,
//        .clockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
//
//        .defaultTxBufValue = 0,
//
//        .dmaIntNum = INT_DMA_INT2,
//        .intPriority = ~0,
//        .rxDMAChannelIndex = DMA_CH5_EUSCIB2RX0,
//        .txDMAChannelIndex = DMA_CH4_EUSCIB2TX0
//    }
};

const SPI_Config SPI_config[] = {
    {
        .fxnTablePtr = &SPIMSP432DMA_fxnTable,
        .object = &spiMSP432DMAObjects[0],
        .hwAttrs = &spiMSP432DMAHWAttrs[0]
    },
//    {
//        .fxnTablePtr = &SPIMSP432DMA_fxnTable,
//        .object = &spiMSP432DMAObjects[1],
//        .hwAttrs = &spiMSP432DMAHWAttrs[1]
//    },
    {NULL, NULL, NULL},
};

/*
 *  ======== MSP_EXP432P401RLP_initSPI ========
 */
void MSP_EXP432P401RLP_initSPI(void)
{
    /*
     * NOTE: TI-RTOS examples configure EUSCIB0 as either SPI or I2C.  Thus,
     * a conflict occurs when the I2C & SPI drivers are used simultaneously in
     * an application.  Modify the pin mux settings in this file and resolve the
     * conflict before running your the application.
     */
    /* Configure CLK, MOSI & MISO for SPI0 (EUSCI_B1) */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P6,
        GPIO_PIN3 | GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);		//SCLK, MOSI
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN5,
        GPIO_PRIMARY_MODULE_FUNCTION);							// MISO

//    /* Configure CLK, MOSI & MISO for SPI1 (EUSCI_B2) */
//    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3,
//        GPIO_PIN5 | GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
//    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN7,
//        GPIO_PRIMARY_MODULE_FUNCTION);

    MSP_EXP432P401RLP_initDMA();
    SPI_init();
}

/*
 *  =============================== UART ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(UART_config, ".const:UART_config")
#pragma DATA_SECTION(uartMSP432HWAttrs, ".const:uartMSP432HWAttrs")
#endif

#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTMSP432.h>

/* UART objects */
UARTMSP432_Object uartMSP432Objects[MSP_EXP432P401RLP_UARTCOUNT];
unsigned char uartMSP432RingBuffer[32];

/*
 * The baudrate dividers were determined by using the MSP430 baudrate
 * calculator
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const UARTMSP432_BaudrateConfig uartMSP432Baudrates[] = {
    /* {baudrate, input clock, prescalar, UCBRFx, UCBRSx, oversampling} - EUSCI */
    {115200, 12000000,  6,  8,  32, 1},
    {115200, 6000000,   3,  4,   2, 1},
    {115200, 3000000,   1, 10,   0, 1},

    {57600, 12000000,  13,  0,  37, 1},
    {57600, 6000000,    6,  8,  32, 1},
    {57600, 3000000,  3, 4, 2, 1},


    {19200, 12000000,  39,  1,  0, 1},
    {19200, 6000000,   19,  8,   85, 1},
    {19200, 3000000,   9, 12,   34, 1},

    {9600,   12000000, 78,  2,   0, 1},
    {9600,   6000000,  39,  1,   0, 1},
    {9600,   3000000,  19,  8,  85, 1},
    {9600,   32768,     3,  0, 146, 0}
};

/* UART configuration structure */
const UARTMSP432_HWAttrs uartMSP432HWAttrs[MSP_EXP432P401RLP_UARTCOUNT] = {
    {
        .baseAddr = EUSCI_A0_BASE,
        .intNum = INT_EUSCIA0,
        .intPriority = ~0,
        .clockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        .bitOrder = EUSCI_A_UART_LSB_FIRST,
        .numBaudrateEntries = sizeof(uartMSP432Baudrates) /
            sizeof(UARTMSP432_BaudrateConfig),
        .baudrateLUT = uartMSP432Baudrates,
        .ringBufPtr  = uartMSP432RingBuffer,
        .ringBufSize = sizeof(uartMSP432RingBuffer)
    },
	{
            .baseAddr = EUSCI_A1_BASE,
            .intNum = INT_EUSCIA1,
            .intPriority = ~0,
            .clockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK,
            .bitOrder = EUSCI_A_UART_LSB_FIRST,
            .numBaudrateEntries = sizeof(uartMSP432Baudrates) /
                sizeof(UARTMSP432_BaudrateConfig),
            .baudrateLUT = uartMSP432Baudrates,
            .ringBufPtr  = uartMSP432RingBuffer,
            .ringBufSize = sizeof(uartMSP432RingBuffer)
    },
    {
        .baseAddr = EUSCI_A2_BASE,
        .intNum = INT_EUSCIA2,
        .intPriority = ~0,
        .clockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        .bitOrder = EUSCI_A_UART_LSB_FIRST,
        .numBaudrateEntries = sizeof(uartMSP432Baudrates) /
            sizeof(UARTMSP432_BaudrateConfig),
        .baudrateLUT = uartMSP432Baudrates,
        .ringBufPtr  = uartMSP432RingBuffer,
        .ringBufSize = sizeof(uartMSP432RingBuffer)
    },
    {
        .baseAddr = EUSCI_A3_BASE,
        .intNum = INT_EUSCIA3,
        .intPriority = ~0,
        .clockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        .bitOrder = EUSCI_A_UART_LSB_FIRST,
        .numBaudrateEntries = sizeof(uartMSP432Baudrates) /
            sizeof(UARTMSP432_BaudrateConfig),
        .baudrateLUT = uartMSP432Baudrates,
        .ringBufPtr  = uartMSP432RingBuffer,
        .ringBufSize = sizeof(uartMSP432RingBuffer)
    }
};

const UART_Config UART_config[] = {
    {
        .fxnTablePtr = &UARTMSP432_fxnTable,
        .object = &uartMSP432Objects[0],
        .hwAttrs = &uartMSP432HWAttrs[0]
    },
	{
            .fxnTablePtr = &UARTMSP432_fxnTable,
            .object = &uartMSP432Objects[1],
            .hwAttrs = &uartMSP432HWAttrs[1]
    },
    {
        .fxnTablePtr = &UARTMSP432_fxnTable,
        .object = &uartMSP432Objects[2],
        .hwAttrs = &uartMSP432HWAttrs[2]
    },
    {
        .fxnTablePtr = &UARTMSP432_fxnTable,
        .object = &uartMSP432Objects[3],
        .hwAttrs = &uartMSP432HWAttrs[3]
    },
    {NULL, NULL, NULL}
};

/*
 *  ======== MSP_EXP432P401RLP_initUART ========
 */
void MSP_EXP432P401RLP_initUART(void)
{
    /* Set P1.2 & P1.3 in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
        GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);


    /* Set P2.2 & P2.3 in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2,
        GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);


    /* Set P3.2 & P3.3 in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
        GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Set P9.6 & P9.7 in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P9,
        GPIO_PIN6 | GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Initialize the UART driver */
    UART_init();
}

/*
 *  =============================== Watchdog ===============================
 */
/* Place into subsections to allow the TI linker to remove items properly */
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(Watchdog_config, ".const:Watchdog_config")
#pragma DATA_SECTION(watchdogMSP432HWAttrs, ".const:watchdogMSP432HWAttrs")
#endif

#include <ti/drivers/Watchdog.h>
#include <ti/drivers/watchdog/WatchdogMSP432.h>

/* Watchdog objects */
WatchdogMSP432_Object watchdogMSP432Objects[MSP_EXP432P401RLP_WATCHDOGCOUNT];

/* Watchdog configuration structure */
const WatchdogMSP432_HWAttrs
    watchdogMSP432HWAttrs[MSP_EXP432P401RLP_WATCHDOGCOUNT] = {
    {
        .baseAddr = WDT_A_BASE,
        .intNum = INT_WDT_A,
        .intPriority = ~0,
        .clockSource = WDT_A_CLOCKSOURCE_SMCLK,
        .clockDivider = WDT_A_CLOCKDIVIDER_8192K
    },
};

const Watchdog_Config Watchdog_config[] = {
    {
        .fxnTablePtr = &WatchdogMSP432_fxnTable,
        .object = &watchdogMSP432Objects[0],
        .hwAttrs = &watchdogMSP432HWAttrs[0]
    },
    {NULL, NULL, NULL}
};

/*
 *  ======== MSP_EXP432P401RLP_initWatchdog ========
 */
void MSP_EXP432P401RLP_initWatchdog(void)
{
    /* Initialize the Watchdog driver */
    Watchdog_init();
}

