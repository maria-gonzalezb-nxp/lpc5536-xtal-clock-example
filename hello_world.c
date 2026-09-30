/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_clock.h"
#include "fsl_power.h"
#include "board.h"
#include "app.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define XTAL_FREQ_HZ    16000000U   /* 16 MHz crystal on LPC5536 EVK */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 *
 * Clock-switch sequence:
 *   1. Run BOARD_InitHardware() first.
 *      BOARD_BootClockPLL150M (called internally) sets MAIN_CLK to PLL0 at
 *      150 MHz and enables the XTAL as the PLL0 reference, so EXT_CLK is
 *      valid and stable when BOARD_InitHardware() returns.
 *      Note: kPDRUNCFG_PD_XTAL32M does NOT exist on LPC55S36 -- the XTAL
 *      is enabled entirely by the board boot clock function via ANACTRL.
 *   2. Re-attach MAIN_CLK directly to EXT_CLK (XTAL, 16 MHz), bypassing PLL0.
 *      The debug UART (FlexComm) is clocked from its own source (FRO12M),
 *      independent of MAIN_CLK, so serial output remains intact.
 *   3. Verify with CLOCK_GetClockAttachId() and CLOCK_GetCoreSysClkFreq().
 */
int main(void)
{
    /* ------------------------------------------------------------------
     * Step 1: Board init -- runs BOARD_BootClockPLL150M which enables
     * the XTAL as the PLL0 reference.  EXT_CLK is valid after this call.
     * ------------------------------------------------------------------ */
    BOARD_InitHardware();

    /* ------------------------------------------------------------------
     * Step 2: Re-attach MAIN_CLK to EXT_CLK (XTAL 16 MHz), bypassing
     * PLL0.  The XTAL is already powered and gated by board init, so no
     * ANACTRL write is needed here.
     * ------------------------------------------------------------------ */
    CLOCK_AttachClk(kEXT_CLK_to_MAIN_CLK);
    SystemCoreClock = CLOCK_GetCoreSysClkFreq();

    /* ------------------------------------------------------------------
     * Step 3: Software verification -- read back the actual SYSCON mux
     * register state with CLOCK_GetClockAttachId() and confirm frequency.
     * ------------------------------------------------------------------ */
    clock_attach_id_t actual = CLOCK_GetClockAttachId(kEXT_CLK_to_MAIN_CLK);

    PRINTF("--- LPC5536 EVK Clock Verification ---\r\n");
    PRINTF("MAIN_CLK mux    : %s\r\n",
           (actual == kEXT_CLK_to_MAIN_CLK) ? "EXT_CLK (XTAL) OK" : "MISMATCH");
    PRINTF("SystemCoreClock : %u Hz\r\n", SystemCoreClock);
    PRINTF("Expected        : %u Hz\r\n",  XTAL_FREQ_HZ);
    PRINTF("Result          : %s\r\n",
           (SystemCoreClock == XTAL_FREQ_HZ) ? "PASS" : "FAIL");
    PRINTF("--------------------------------------\r\n");

    PRINTF("\r\nhello world.\r\n");

    while (1)
    {
        /* idle */
    }
}
