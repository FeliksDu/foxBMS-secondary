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
 * @file    com.c
 * @author  foxBMS Team
 * @date    28.08.2015 (date of creation)
 * @ingroup DRIVERS
 * @prefix  COM
 *
 * @brief Driver for the COM module.
 *
 * Generic communication module which handles pre-defined user input and output.
 * In its current state it only uses UART/RS232 for communication
 */



/*================== Includes =============================================*/
#include "com.h"
#include "misc.h"
#include <string.h>
#include "os.h"
#include "bkpsram.h"


/*================== Macros and Definitions ===============================*/

#define TESTMODE_TIMEOUT 30000


/*================== Constant and Variable Definitions ====================*/

char com_receivedbyte[256];
uint8_t com_receive_slot = 0;

static uint8_t com_testmode_enabled = 0;
static uint32_t com_tickcount = 0;

static RTC_Time_s com_Time;
static RTC_Date_s com_Date;

static uint8_t com_buf[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

/*================== Function Prototypes ==================================*/


/*================== Function Implementations =============================*/

/**
 * Prints date and time info on serial interface
 *
 * @return none
 */
void COM_printTimeAndDate(void) {

    /* Get time and date */
    RTC_getTime(&com_Time);
    RTC_getDate(&com_Date);

    DEBUG_PRINTF((const uint8_t * )"Date and Time: 20");
    DEBUG_PRINTF(U8ToDecascii(com_buf, &com_Date.Year, 2));
    DEBUG_PRINTF((const uint8_t * )".");
    DEBUG_PRINTF(U8ToDecascii(com_buf, &com_Date.Month, 2));
    DEBUG_PRINTF((const uint8_t * )".");
    DEBUG_PRINTF(U8ToDecascii(com_buf, &com_Date.Date, 2));
    DEBUG_PRINTF((const uint8_t * )" - ");
    DEBUG_PRINTF(U8ToDecascii(com_buf, &com_Time.Hours, 2));
    DEBUG_PRINTF((const uint8_t * )":");
    DEBUG_PRINTF(U8ToDecascii(com_buf, &com_Time.Minutes, 2));
    DEBUG_PRINTF((const uint8_t * )":");
    DEBUG_PRINTF(U8ToDecascii(com_buf, &com_Time.Seconds, 2));
    DEBUG_PRINTF((const uint8_t * )" \r\n");
}


/**
 * @brief COM_StartupInfo prints some startup related informations (time & date).
 *
 * This function needs proper initialized RTC and UART module to work.
 * Called during startup process in main function.
 *
 * @return (type: void)
 */
void COM_StartupInfo(void) {
    /* Get RCC Core Reset Register */
    uint32_t tmp = main_state.CSR;

    DEBUG_PRINTF((const uint8_t * )"System starting...\r\n");
    DEBUG_PRINTF((const uint8_t * )"RCC Core Reset Register: 0x");
    DEBUG_PRINTF((const uint8_t * )"RCC Core Reset Register: 0x");
    DEBUG_PRINTF((const uint8_t * )U32ToHexascii(com_buf, &tmp));
    DEBUG_PRINTF((const uint8_t * )"\r\n");

    /* Print time and date */
    COM_printTimeAndDate();

    /* Print runtime */
    DEBUG_PRINTF((const uint8_t * )"Runtime: ");
    DEBUG_PRINTF(U8ToDecascii(com_buf, &os_timer.Timer_h, 2));
    DEBUG_PRINTF((const uint8_t * )"h ");
    DEBUG_PRINTF(U8ToDecascii(com_buf, &os_timer.Timer_min, 2));
    DEBUG_PRINTF((const uint8_t * )"m ");
    DEBUG_PRINTF(U8ToDecascii(com_buf, &os_timer.Timer_sec, 2));
    DEBUG_PRINTF((const uint8_t * )"s");
    DEBUG_PRINTF((const uint8_t * )"\r\n");
}


/**
 * @brief COM_Decoder parses and interprets user defined input data.
 *
 * This function needs to be called within a cyclic engtask e.g. ENG_TSK_Cyclic_10ms
 * It processes input data received by the UART module asynchronly.
 *
 * @return (type: void)
 *
 */
void COM_Decoder(void) {

    /* Command Received - Replace Carrier Return with null character */
    if(com_receivedbyte[com_receive_slot - 1] == '\r') {
        com_receivedbyte[com_receive_slot - 1] = '\0';

        /* ENABLE TESTMODE */
        if (strcmp(com_receivedbyte, "teston") == 0) {

            /* Set timeout */
            com_tickcount = osKernelSysTick();

            if(!com_testmode_enabled) {

                /* Enable testmode */
                com_testmode_enabled = 1;

                DEBUG_PRINTF((const uint8_t * )"Testmode enabled!");
                DEBUG_PRINTF((const uint8_t * )"\r\n");
            }
            else {

                /* Testmode already enabled */
                DEBUG_PRINTF((const uint8_t * )"Testmode already enabled!");
                DEBUG_PRINTF((const uint8_t * )"\r\n");
            }

            /* Clear received command */
            memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
            com_receive_slot = 0;

            /* Reset timeout to TESTMODE_TIMEOUT */
            com_tickcount = osKernelSysTick();

            return;
        }

        /* PRINT CONTACTOR INFO */
        if(strcmp(com_receivedbyte, "printcontactorinfo") == 0) {

            /* Print contactor info */
            DIAG_PrintContactorInfo();

            /* Clear received command */
            memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
            com_receive_slot = 0;

            /* Reset timeout to TESTMODE_TIMEOUT */
            com_tickcount = osKernelSysTick();

            return;
        }

        /* PRINT DIAG INFO */
        if(strcmp(com_receivedbyte, "printdiaginfo") == 0) {

            /* Print diag info */
            DIAG_PrintErrors();

            /* Clear received command */
            memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
            com_receive_slot = 0;

            /* Reset timeout to TESTMODE_TIMEOUT */
            com_tickcount = osKernelSysTick();

            return;
        }

        /* GETTIME */
        if (strcmp(com_receivedbyte, "gettime") == 0) {

            /* Print time and date */
            COM_printTimeAndDate();

            /* Clear received command */
            memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
            com_receive_slot = 0;

            /* Reset timeout to TESTMODE_TIMEOUT */
            com_tickcount = osKernelSysTick();

            return;
        }

        /* Command received and testmode enabled */
        if (com_testmode_enabled) {

            /* DISABLE TESTMODE */
            if (strcmp(com_receivedbyte, "testoff") == 0) {
                com_testmode_enabled = 0;
                DEBUG_PRINTF((const uint8_t * )"Testmode disabled on request!");
                DEBUG_PRINTF((const uint8_t * )"\r\n");

                /* Clear received command */
                memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
                com_receive_slot = 0;
                return;
            }

            /* SETTIME */
//            if(strcmp(com_receivedbyte, "settime") == 0) {
            if(com_receivedbyte[0] == 's' && com_receivedbyte[1] == 'e' && com_receivedbyte[2] == 't'
                    && com_receivedbyte[3] == 't' && com_receivedbyte[4] == 'i' && com_receivedbyte[5] == 'm'
                            && com_receivedbyte[6] == 'e') {

                /* Command length = 24, +1 for \0, +1 cause index always one step ahead */
                if(com_receive_slot == 26) {

                    /* Set time and date only if command length is like expected */
                    uint8_t value, tmp10, tmp1, retVal = 0;

                    /* Calculate year */
                    tmp10 = com_receivedbyte[8] - '0';
                    tmp1 = com_receivedbyte[9] - '0';
                    value = (tmp10 * 10) + tmp1;

                    if(IS_RTC_YEAR(value))  /* Check if value is valid */
                        com_Date.Year = value;
                    else
                        retVal = 0xFF;

                    /* Calculate month */
                    tmp10 = com_receivedbyte[11] - '0';
                    tmp1 = com_receivedbyte[12] - '0';
                    value = (tmp10 * 10) + tmp1;

                    if(IS_RTC_MONTH(value))  /* Check if value is valid */
                        com_Date.Month = value;
                    else
                        retVal = 0xFF;

                    /* Calculate day */
                    tmp10 = com_receivedbyte[14] - '0';
                    tmp1 = com_receivedbyte[15] - '0';
                    value = (tmp10 * 10) + tmp1;

                    if(IS_RTC_DATE(value))  /* Check if value is valid */
                        com_Date.Date = value;
                    else
                        retVal = 0xFF;

                    /* Calculate hours */
                    tmp10 = com_receivedbyte[17] - '0';
                    tmp1 = com_receivedbyte[18] - '0';
                    value = (tmp10 * 10) + tmp1;

                    if(IS_RTC_HOUR24(value))  /* Check if value is valid */
                        com_Time.Hours = value;
                     else
                        retVal = 0xFF;

                    /* Calculate minutes */
                    tmp10 = com_receivedbyte[20] - '0';
                    tmp1 = com_receivedbyte[21] - '0';
                    value = (tmp10 * 10) + tmp1;

                    if(IS_RTC_MINUTES(value))  /* Check if value is valid */
                        com_Time.Minutes = value;
                     else
                        retVal = 0xFF;

                    /* Calculate seconds */
                    tmp10 = com_receivedbyte[23] - '0';
                    tmp1 = com_receivedbyte[24] - '0';
                    value = (tmp10 * 10) + tmp1;

                    if(IS_RTC_SECONDS(value))  /* Check if value is valid */
                        com_Time.Seconds = value;
                     else
                        retVal = 0xFF;

                    if(retVal == 0) {

                        /* Set time and date */
                        RTC_setTime(&com_Time);
                        RTC_setDate(&com_Date);

                        DEBUG_PRINTF((const uint8_t * )"Time and date set!");
                        DEBUG_PRINTF((const uint8_t * )"\r\n");
                    }
                    else {

                        /* Print error message */
                        DEBUG_PRINTF((const uint8_t * )"Invalid parameter!");
                        DEBUG_PRINTF((const uint8_t * )"\r\n");

                        for(int i = 0; i < 25; i++) {
                            DEBUG_PRINTF((const uint8_t * )(uintptr_t)&com_receivedbyte[i]);
                        }

                        DEBUG_PRINTF((const uint8_t * )"\r\n");
                    }

                }
                else {

                    /* Print error message */
                    DEBUG_PRINTF((const uint8_t * )"Invalid parameter length!");
                    DEBUG_PRINTF((const uint8_t * )"\r\n");

                    for(int i = 0; i < com_receive_slot; i++) {
                        DEBUG_PRINTF((const uint8_t * )(uintptr_t)&com_receivedbyte[i]);
                    }

                    DEBUG_PRINTF((const uint8_t * )"\r\n");
                }

                /* Clear received command */
                memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
                com_receive_slot = 0;

                /* Reset timeout to TESTMODE_TIMEOUT */
                com_tickcount = osKernelSysTick();

                return;
            }

            /* RESET */
            if (strcmp(com_receivedbyte, "reset") == 0) {
                DEBUG_PRINTF((const uint8_t * )"Software reset!");
                DEBUG_PRINTF((const uint8_t * )"\r\n");

                /* Clear received command */
                memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
                com_receive_slot = 0;

                HAL_NVIC_SystemReset();
                return;
            }

            if (strcmp(com_receivedbyte, "watchdogtest") == 0) {
                DEBUG_PRINTF((const uint8_t * )"WDG");
              //  DEBUG_PRINTF((const uint8_t * )"\r\n");

                /* Clear received command */
                memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
                com_receive_slot = 0;
                MCU_Wait_us(1000);

                /* disable global interrupt, no saving of interrupt status because of follwing reset */
                (void)MCU_DisableINT();

                while(1) {                       /* stop system and wait for watchdog reset*/
                    ;
                }
                return;
            }


            /* SWITCH CONTACTORS */
            switch (com_receivedbyte[0]) {

            case 'c':
                ;   // added to get rid of compile error
                /* Convert ascii number to int */
                uint8_t contNumber;
                contNumber = com_receivedbyte[2] - '0';

                /* Switch contactor */
                if((0 <= contNumber) && (contNumber <= NR_OF_CONTACTORS)) {

                    /* Close contactor */
                    if (com_receivedbyte[1] == 'e') {
                        DEBUG_PRINTF((const uint8_t * )"Contactor ");
                        DEBUG_PRINTF((const uint8_t * )(uintptr_t)&com_receivedbyte[2]);

                        CONT_SetContactorState(contNumber, CONT_SWITCH_ON);

                        DEBUG_PRINTF((const uint8_t * )" enabled\r\n");
                    }
                    /* Open contactor */
                    else if (com_receivedbyte[1] == 'd') {
                        DEBUG_PRINTF((const uint8_t * )"Contactor ");
                        DEBUG_PRINTF((const uint8_t * )(uintptr_t)&com_receivedbyte[2]);

                        CONT_SetContactorState(contNumber, CONT_SWITCH_OFF);

                        DEBUG_PRINTF((const uint8_t * )" disabled\r\n");
                    }
                    else {

                        /* Invalid command */
                        DEBUG_PRINTF((const uint8_t * )"Invalid command!\r\n");
                    }
                }
                else {
                    /* Invalid contactor number */

                    DEBUG_PRINTF((const uint8_t * )"Invalid contactor number! Only ");
                    uint8_t a = NR_OF_CONTACTORS;
                    DEBUG_PRINTF(U8ToDecascii(com_buf, &a, 0));
                    DEBUG_PRINTF((const uint8_t * )" contactors are connected! \r\n");
                }
                break;

            default:
                DEBUG_PRINTF((const uint8_t * )"Invalid command!\r\n");
                DEBUG_PRINTF((const uint8_t * )com_receivedbyte);
                DEBUG_PRINTF((const uint8_t * )"\r\n");
                break;

            }

            /* Reset timeout to TESTMODE_TIMEOUT */
            com_tickcount = osKernelSysTick();

            /* Clear received command */
            memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
            com_receive_slot = 0;

            return;
        }


        /* Invalid command */
        DEBUG_PRINTF((const uint8_t * )"Invalid command!\r\n");
        DEBUG_PRINTF((const uint8_t * )com_receivedbyte);
        DEBUG_PRINTF((const uint8_t * )"\r\n");

        /* Clear received command */
        memset(com_receivedbyte, 0, sizeof(com_receivedbyte));
        com_receive_slot = 0;

    }

    /* Timed out --> disable testmode */
    if (com_tickcount + TESTMODE_TIMEOUT < osKernelSysTick() && com_testmode_enabled) {
        com_testmode_enabled = 0;
        DEBUG_PRINTF((const uint8_t * )"Testmode disabled on timeout!");
        DEBUG_PRINTF((const uint8_t * )"\r\n");

    }

}
