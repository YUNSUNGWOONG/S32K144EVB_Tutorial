#include "clocks_and_modes.h"
#include "device_registers.h" /* include peripheral declarations S32K144 */

int lpit0_ch0_flag_counter = 0; /* LPIT0 timeout counter */

void PORT_init(void)
{
    /* Clock setting */
    PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT D */
    PCC->PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT E */
    /* Direction setting */
    PTD->PDDR |= 1 << 11 | 1 << 15;                            // output
    PTE->PDDR |= 1 << 12 | 1 << 14 | 1 << 15;                  // output
    PTE->PDDR &= ~((1 << 0) & (1 << 1) & (1 << 2) & (1 << 3)); // input
    /* GPIO setting */
    PORTE->PCR[12] = PORT_PCR_MUX(1); /* Port E12: MUX = GPIO  */
    PORTE->PCR[14] = PORT_PCR_MUX(1); /* Port E14: MUX = GPIO  */
    PORTE->PCR[15] = PORT_PCR_MUX(1); /* Port E15: MUX = GPIO  */

    PORTD->PCR[11] = PORT_PCR_MUX(1); /* Port D11: MUX = GPIO */
    PORTD->PCR[15] = PORT_PCR_MUX(1); /* Port D15: MUX = GPIO */

    PORTE->PCR[0] = PORT_PCR_MUX(1) | PORT_PCR_PFE(1) | PORT_PCR_PE(1) |
                    PORT_PCR_PS(0); /* Port E0: MUX = GPIO, input filter enabled, internal pull-down enabled */
    PORTE->PCR[1] = PORT_PCR_MUX(1) | PORT_PCR_PFE(1) | PORT_PCR_PE(1) |
                    PORT_PCR_PS(0); /* Port E0: MUX = GPIO, input filter enabled, internal pull-down enabled */
    PORTE->PCR[2] = PORT_PCR_MUX(1) | PORT_PCR_PFE(1) | PORT_PCR_PE(1) |
                    PORT_PCR_PS(0); /* Port E0: MUX = GPIO, input filter enabled, internal pull-down enabled */
    PORTE->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PFE(1) | PORT_PCR_PE(1) |
                    PORT_PCR_PS(0); /* Port E0: MUX = GPIO, input filter enabled, internal pull-down enabled */
}

void WDOG_disable(void)
{
    WDOG->CNT = 0xD928C520;   /* Unlock watchdog       */
    WDOG->TOVAL = 0x0000FFFF; /* Maximum timeout value    */
    WDOG->CS = 0x00002100;    /* Disable watchdog       */
}

void LPIT0_init(uint32_t delay)
{
    uint32_t timeout;
    /*!
     * LPIT Clocking:
     * ==============================
     */
    PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* Clock Src = 6 (SPLL2_DIV2_CLK)*/
    PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs       */

    /*!
     * LPIT Initialization:
     */
    LPIT0->MCR |= LPIT_MCR_M_CEN_MASK; /* DBG_EN-0: Timer chans stop in Debug mode */
                                       /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
                                       /* SW_RST=0: SW reset does not reset timer chans, regs */
                                       /* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */

    timeout = delay * 40;         // ms setting*40000 us setting *40
    LPIT0->TMR[0].TVAL = timeout; /* Chan 0 Timeout period: 40M clocks */
    LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;
    /* T_EN=1: Timer channel is enabled */
    /* CHAIN=0: channel chaining is disabled */
    /* MODE=0: 32 periodic counter mode */
    /* TSOT=0: Timer decrements immediately based on restart */
    /* TSOI=0: Timer does not stop after timeout */
    /* TROT=0 Timer will not reload on trigger */
    /* TRG_SRC=0: External trigger soruce */
    /* TRG_SEL=0: Timer chan 0 trigger source is selected*/
}

void delay_ms(volatile int ms)
{
    LPIT0_init(ms); /* Initialize PIT0 for 1 second timeout  */
    while (0 == (LPIT0->MSR & LPIT_MSR_TIF0_MASK))
    {
    }                                 /* Wait for LPIT0 CH0 Flag */
    lpit0_ch0_flag_counter++;         /* Increment LPIT0 timeout counter */
    LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}

int KeyScan(void)
{
    int Dtime = 10;
    int Kbuff = 0;

    PTE->PCOR |= 1 << 12 | 1 << 14 | 1 << 15;

    PTE->PSOR |= 1 << 12;
    delay_ms(Dtime);
    if (PTE->PDIR & (1 << 0))
        Kbuff = 1; // 1
    if (PTE->PDIR & (1 << 1))
        Kbuff = 4; // 4
    if (PTE->PDIR & (1 << 2))
        Kbuff = 7; // 7
    if (PTE->PDIR & (1 << 3))
        Kbuff = 11; //*
    PTE->PCOR |= 1 << 12;

    PTE->PSOR |= 1 << 14;
    delay_ms(Dtime);
    if (PTE->PDIR & (1 << 0))
        Kbuff = 2; // 2
    if (PTE->PDIR & (1 << 1))
        Kbuff = 5; // 5
    if (PTE->PDIR & (1 << 2))
        Kbuff = 8; // 8
    if (PTE->PDIR & (1 << 3))
        Kbuff = 0; // 0
    PTE->PCOR |= 1 << 14;

    PTE->PSOR |= 1 << 15;
    delay_ms(Dtime);
    if (PTE->PDIR & (1 << 0))
        Kbuff = 3; // 3
    if (PTE->PDIR & (1 << 1))
        Kbuff = 6; // 6
    if (PTE->PDIR & (1 << 2))
        Kbuff = 9; // 9
    if (PTE->PDIR & (1 << 3))
        Kbuff = 12; // #
    PTE->PCOR |= 1 << 15;

    return Kbuff;
}

int main(void)
{
    WDOG_disable();        /* Disable Watchdog in case it is not done in startup code */
    PORT_init();           /* Configure ports */
    SOSC_init_8MHz();      /* Initialize system oscilator for 8 MHz xtal */
    SPLL_init_160MHz();    /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
    NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */

    int key;

    while (1)
    {
        //(1) GPIO? Keypad? ????,
        key = KeyScan();

        if (key == 5)
        {
            PTD->PSOR |= 1 << 11; //(2) '5'??? ??? PIEZO[Buzzor] 'On'
            delay_ms(2000);
        }
        else if (key == 3)
        {
            PTD->PCOR |= 1 << 15; // '3'? ?? ??? ??? LED 'On'
            delay_ms(2000);
        }
        else
        {
            PTD->PCOR |= 1 << 11; /* PIEZO[Buzzor] off */
            PTD->PSOR |= 1 << 15; /* Red LED off */
            delay_ms(2000);
        }
    }
}
