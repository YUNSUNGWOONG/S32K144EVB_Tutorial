/**
 * <!-- Paraphrasing -->
 */
/*! Port PTD0, bit 0: FROM EVB output to blue LED
 */
#define PTD0 0 // LED

/*! Port PTC12, bit 12: FROM EVB input from BTN0 [SW2]*/

#define PTC12 12 // SWTICH

int main(void)
{
    int counter = 0;

    WDOG_disable(); /* Disable Watchod in case it is not done in startup code */

    PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clocks to peripherals (PORT modules)*/
                                                    /* Enable clock to PORT C*/
    PORTC->PCR[12] = PORT_PCR_MUX(1);
    /* Configure port C12 as GPIO input (BTN 0 [SW2] on EVB) */
    PTC->PDDR &= ~(1 << PTC12) /* Port C12: Data Direction= input (default) */

                  PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clocks to peripherals (PORT modules) */
                                                                  /* Enable clock to PORT D*/
    PORTD->PCR[0] = PORT_PCR_MUX(1);                              /* Port D0: MUX = GPIO */

    /* Configure port D0 as GPIO output (LED on EVB) */
    PTD->PDDR |= 1 << PTD0; /* Port D0: Data Direction= output */

    for (;;)
    {
        /*! -If Pad Data Input = 1 (BTN0 [SW2] pushed)
         *
         * Clear Output on port D0 (LED on)
         *
         */
        if (PTC->PDIR & (1 << PTC12))
        {
            PTD->PCOR |= 1 << PTD0;
        }
        else
        {
            PTD->PSOR |= 1 << PTD0;
        }
    }
}
