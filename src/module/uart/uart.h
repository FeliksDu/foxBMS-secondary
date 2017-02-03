/**
 *
 * @copyright &copy; 2010 - 2016, Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V. All rights reserved.
 *
 * BSD 3-Clause License
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * We kindly request you to use one or more of the following phrases to refer to foxBMS in your hardware, software, documentation or advertising materials:
 *
 * &Prime;This product uses parts of foxBMS&reg;&Prime;
 *
 * &Prime;This product includes parts of foxBMS&reg;&Prime;
 *
 * &Prime;This product is derived from foxBMS&reg;&Prime;
 *
 */

/**
 * @file    uart.h
 * @author  foxBMS Team
 * @date    26.08.2015
 * @ingroup DRIVERS
 * @prefix  UART
 *
 * @brief Headers for the driver for the UART.
 *
 */

#ifndef UART_H_
#define UART_H_

/*================== Includes =============================================*/
#include "uart_cfg.h"
#include "syscontrol.h"


/*================== Macros and Definitions ===============================*/
#define RXBUF_LENGTH    0x100    /* 256 Byte */


#define ACK_SYMBOL  0x06
#define NAK_SYMBOL  0x15
#define STX_SYMBOL  0x02
#define ETX_SYMBOL  0x03

#if(BUILD_MODULE_DEBUGPRINTF)
#if (!BUILD_MODULE_ENABLE_UART)
#error ERROR: wrong combination of module enable keys in general.h !
#error BUILD_MODULE_DEBUGPRINTF could only be used when UART module (BUILD_MODULE_ENABLE_UART) is activated
#endif
    #define DEBUG_PRINTF(x)             UART_vWrite(x)
#else
    #define DEBUG_PRINTF(x)             /* keep macro empty */
#endif


/*================== Constant and Variable Definitions ====================*/
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart2;

/*================== Function Prototypes ==================================*/

extern void UART_Init(void);
extern void HAL_UART_CustomIRQHandler(UART_HandleTypeDef *huart);
extern void UART_vWrite(const uint8_t *source);
extern void COM_Decoder(void);
/*================== Function Implementations =============================*/


#endif /* UART_H_ */
