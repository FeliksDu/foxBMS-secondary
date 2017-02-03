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
 * @file    bkpsram_cfg.h
 * @author  foxBMS Team
 * @date    02.03.2016
 * @ingroup DRIVERS_CONF
 * @prefix  BKP
 *
 * @brief   Headers for the configuration for static backup RAM.
 *
 */
#ifndef BKSPSRAM_CFG_H_
#define BKSPSRAM_CFG_H_
/*================== Includes =============================================*/
#include "general.h"
#include "sox.h"
#include "contactor.h"
#include "diag.h"
#include "main.h"

/*================== Macros and Definitions ===============================*/



/**
 * Enables the usage of the backup SRAM
 */
#define BKP_SRAM_ENABLE

/**
 * state of charge voltages, since soc is voltage dependent, three different values are calculated, min, max and mean,
 * number of opening/closing events of the contactors,
 * checksum over soc data and switching events
 */
typedef struct {
    SOX_SOC_s nvsoc;
    DIAG_CONTACTOR_s contactors_count;
    uint32_t checksum;
} BKPSRAM_CH_1_s;

/*================== Constant and Variable Definitions ====================*/
extern BKPSRAM_CH_1_s BKP_SRAM bkpsram_ch_1;
extern MAIN_STATUS_s BKP_SRAM main_state;

/*================== Function Prototypes ==================================*/

/**
 * @brief  Sets the number of opening/closing events of the contactors saved in the backup SRAM
 *
 * @param  ptr pointer where the contactor events are stored
 * @return void
*/
extern void BKPSRAM_Set_contactorcnt(DIAG_CONTACTOR_s *ptr);

/**
 * @brief  Gets the number of opening/closing events of the contactors saved in the backup SRAM
 *
 * @param  ptr pointer where the contactor event data should be stored to
 * @return void
*/
extern void BKPSRAM_Get_contactorcnt(DIAG_CONTACTOR_s *ptr);

/*================== Function Implementations =============================*/


#endif /* BKSPSRAM_CFG_H_ */
