LPC5536 EVK — XTAL Clock Switch Example

This example demonstrates how to detach the default FRO12M internal oscillator and attach the on-board 16 MHz crystal oscillator (XTAL) as the MAIN_CLK source on the LPC5536 EVK, using the MCUXpresso SDK clock driver APIs.

Hardware
LPCXpresso55S36 EVK
On-board 16 MHz crystal (XTALIN/XTALOUT pins)

Software
MCUXpresso SDK 26.6.0
MCUXpresso for VS Code
Based on the hello_world SDK demo app

What It Does
By default the LPC5536 boots on FRO12M (12 MHz internal oscillator). This example:
Calls BOARD_InitHardware() which enables the XTAL as the PLL0 reference and brings the system up at 150 MHz

Re-attaches MAIN_CLK directly to EXT_CLK (XTAL), bypassing PLL0, to run at 16 MHz

Verifies the switch using CLOCK_GetClockAttachId() and prints the result over UART

Expected Serial Output
Open a serial terminal at 115200 baud on the MCU-LINK COM port. After flashing you should see:

--- LPC5536 EVK Clock Verification ---
MAIN_CLK mux    : EXT_CLK (XTAL) OK
SystemCoreClock : 16000000 Hz
Expected        : 16000000 Hz
Result          : PASS
--------------------------------------

hello world.

Key Code Changes in hello_world.c

/* Let board init run first — this enables XTAL as PLL0 reference */
BOARD_InitHardware();

/* Re-attach MAIN_CLK directly to EXT_CLK (XTAL 16 MHz), bypassing PLL0 */
CLOCK_AttachClk(kEXT_CLK_to_MAIN_CLK);
SystemCoreClock = CLOCK_GetCoreSysClkFreq();

/* Verify using the actual mux register state */
clock_attach_id_t actual = CLOCK_GetClockAttachId(kEXT_CLK_to_MAIN_CLK);
PRINTF("MAIN_CLK mux : %s\r\n",
    (actual == kEXT_CLK_to_MAIN_CLK) ? "EXT_CLK (XTAL) OK" : "MISMATCH");

How to Use
Clone this repo:
git clone https://github.com/maria-gonzalezb-nxp/lpc5536-xtal-clock-example.git

Copy hello_world.c into your local SDK at:
 <SDK_ROOT>/examples/demo_apps/hello_world/
Build and flash using MCUXpresso for VS Code or your preferred IDE
Open a serial terminal at 115200 baud to see the verification output

Important Notes
kPDRUNCFG_PD_XTAL32M does not exist on the LPC55S36 — the XTAL is enabled via BOARD_InitHardware() and the ANACTRL->XO32M_CTRL register, not through the power management API

The debug UART (FlexComm) is clocked independently from MAIN_CLK, so serial output remains functional after the switch

Place the clock switch after BOARD_InitHardware() — placing it before will be overridden by the board's default boot clock function (BOARD_BootClockPLL150M)

License
BSD-3-Clause — see LICENSE
