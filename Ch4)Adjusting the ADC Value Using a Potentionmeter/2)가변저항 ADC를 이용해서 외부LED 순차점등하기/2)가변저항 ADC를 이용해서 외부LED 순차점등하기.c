#include "ADC.h"
#include "clocks_and_modes.h"
#include "device_registers.h"

void PORT_init(void)
{

    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORTD */
    PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clock for PORTC */

    PORTC->PCR[15] = PORT_PCR_MUX(1);
    PORTD->PCR[1] = PORT_PCR_MUX(1);
    PORTD->PCR[2] = PORT_PCR_MUX(1);
    PORTD->PCR[3] = PORT_PCR_MUX(1);
    PORTD->PCR[4] = PORT_PCR_MUX(1);
    PORTD->PCR[5] = PORT_PCR_MUX(1);
    PORTD->PCR[6] = PORT_PCR_MUX(1);
    PORTD->PCR[7] = PORT_PCR_MUX(1);
    PORTD->PCR[8] = PORT_PCR_MUX(1);

    PTD->PDDR |= 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7 | 1 << 8;
    PTC->PDDR |= 1 << 15;
}

void WDOG_disable(void)
{
    WDOG->CNT = 0xD928C520;   /* Unlock watchdog 		*/
    WDOG->TOVAL = 0x0000FFFF; /* Maximum timeout value 	*/
    WDOG->CS = 0x00002100;    /* Disable watchdog 		*/
}

int main(void)
{
    uint32_t adcResultInMv = 0; /*< ADC0 Result in miliVolts */

    /*!
     * Initialization:
     * =======================
     */
    WDOG_disable();        /* Disable WDOG												*/
    SOSC_init_8MHz();      /* Initialize system oscillator for 8 MHz xtal 				*/
    SPLL_init_160MHz();    /* Initialize SPLL to 160 MHz with 8 MHz SOSC 				*/
    NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash*/
    PORT_init();           /* Init  port clocks and gpio outputs 						*/
    ADC_init();            /* Init ADC resolution 12 bit									*/

    /*!
     * Infinite for:
     * ========================
     */
    for (;;)
    {
        convertAdcChan(13); /* Convert Channel AD13 to pot on EVB 	*/
        while (adc_complete() == 0)
        {
        }                               /* Wait for conversion complete flag 	*/
        adcResultInMv = read_adc_chx(); /* Get channel's conversion results in mv */

        if (adcResultInMv > 4000)
        { /* If result > 3.75V 		*/
            PTD->PCOR |= 1 << 1;
            PTD->PSOR |= 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7 | 1 << 8;
        }
        else if (adcResultInMv > 3500)
        { /* If result > 1.25V 		*/
            PTD->PCOR |= 1 << 2;
            PTD->PSOR |= 1 << 1 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7 | 1 << 8;
        }
        else if (adcResultInMv > 3000)
        { /* If result > 1.25V 		*/
            PTD->PCOR |= 1 << 3;
            PTD->PSOR |= 1 << 1 | 1 << 2 | 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7 | 1 << 8;
        }
        else if (adcResultInMv > 2500)
        { /* If result > 1.25V 		*/
            PTD->PCOR |= 1 << 4;
            PTD->PSOR |= 1 << 1 | 1 << 2 | 1 << 3 | 1 << 5 | 1 << 6 | 1 << 7 | 1 << 8;
        }
        else if (adcResultInMv > 2000)
        { /* If result > 1.25V 		*/
            PTD->PCOR |= 1 << 5;
            PTD->PSOR |= 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 6 | 1 << 7 | 1 << 8;
        }
        else if (adcResultInMv > 1500)
        { /* If result > 1.25V 		*/
            PTD->PCOR |= 1 << 6;
            PTD->PSOR |= 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 7 | 1 << 8;
        }
        else if (adcResultInMv > 1000)
        { /* If result > 1.25V 		*/
            PTD->PCOR |= 1 << 7;
            PTD->PSOR |= 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6 | 1 << 8;
        }
        else if (adcResultInMv > 500)
        { /* If result > 1.25V 		*/
            PTD->PCOR |= 1 << 8;
            PTD->PSOR |= 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7;
        }
        else
        {
            PTD->PSOR |= 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7 | 1 << 8;
        }
    }
}