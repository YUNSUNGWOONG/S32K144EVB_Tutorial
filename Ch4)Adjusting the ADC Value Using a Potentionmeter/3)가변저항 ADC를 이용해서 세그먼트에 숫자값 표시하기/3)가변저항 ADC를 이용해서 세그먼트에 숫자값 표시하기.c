#include "ADC.h"
#include "clocks_and_modes.h"
#include "device_registers.h"

int lpit0_ch0_flag_counter = 0;
void PORT_init(void)
{

    PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;
    PTC->PDDR |= 1 << 15;
    PORTC->PCR[15] = PORT_PCR_MUX(1);

    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;
    PTD->PDDR |= 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7 | 1 << 8 | 1 << 9 | 1 << 10 | 1 << 11;

    PORTD->PCR[1] = PORT_PCR_MUX(1);
    PORTD->PCR[2] = PORT_PCR_MUX(1);
    PORTD->PCR[3] = PORT_PCR_MUX(1);
    PORTD->PCR[4] = PORT_PCR_MUX(1);
    PORTD->PCR[5] = PORT_PCR_MUX(1);
    PORTD->PCR[6] = PORT_PCR_MUX(1);
    PORTD->PCR[7] = PORT_PCR_MUX(1);

    PORTD->PCR[10] = PORT_PCR_MUX(1); /* DIGIT_1000  */
    PORTD->PCR[11] = PORT_PCR_MUX(1); /* DIGIT_100  */
    PORTD->PCR[9] = PORT_PCR_MUX(1);  /* DIGIT_10  */
    PORTD->PCR[8] = PORT_PCR_MUX(1);  /* DIGIT_1  */
}
void WDOG_disable(void)
{
    WDOG->CNT = 0xD928C520;
    WDOG->TOVAL = 0x0000FFFF;
    WDOG->CS = 0x00002100;
}

void LPIT0_init(uint32_t delay)
{
    uint32_t timeout;
    PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);
    PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK;
    LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;
    timeout = delay * 40000;
    LPIT0->TMR[0].TVAL = timeout;
    LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;
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

void num(int nom)
{
    switch (nom)
    {
    case 0:
        PTD->PSOR |= 1 << 1;
        PTD->PSOR |= 1 << 2;
        PTD->PSOR |= 1 << 3;
        PTD->PSOR |= 1 << 4;
        PTD->PSOR |= 1 << 5;
        PTD->PSOR |= 1 << 6;
        PTD->PCOR |= 1 << 7;
        break;
    case 1:
        PTD->PCOR |= 1 << 1;
        PTD->PSOR |= 1 << 2;
        PTD->PSOR |= 1 << 3;
        PTD->PCOR |= 1 << 4;
        PTD->PCOR |= 1 << 5;
        PTD->PCOR |= 1 << 6;
        PTD->PCOR |= 1 << 7;
        break;
    case 2:
        PTD->PSOR |= 1 << 1;
        PTD->PSOR |= 1 << 2;
        PTD->PCOR |= 1 << 3;
        PTD->PSOR |= 1 << 4;
        PTD->PSOR |= 1 << 5;
        PTD->PCOR |= 1 << 6;
        PTD->PSOR |= 1 << 7;
        break;
    case 3:
        PTD->PSOR |= 1 << 1;
        PTD->PSOR |= 1 << 2;
        PTD->PSOR |= 1 << 3;
        PTD->PSOR |= 1 << 4;
        PTD->PCOR |= 1 << 5;
        PTD->PCOR |= 1 << 6;
        PTD->PSOR |= 1 << 7;
        break;
    case 4:
        PTD->PCOR |= 1 << 1;
        PTD->PSOR |= 1 << 2;
        PTD->PSOR |= 1 << 3;
        PTD->PCOR |= 1 << 4;
        PTD->PCOR |= 1 << 5;
        PTD->PSOR |= 1 << 6;
        PTD->PSOR |= 1 << 7;
        break;
    case 5:
        PTD->PSOR |= 1 << 1;
        PTD->PCOR |= 1 << 2;
        PTD->PSOR |= 1 << 3;
        PTD->PSOR |= 1 << 4;
        PTD->PCOR |= 1 << 5;
        PTD->PSOR |= 1 << 6;
        PTD->PSOR |= 1 << 7;
        break;
    case 6:
        PTD->PSOR |= 1 << 1;
        PTD->PCOR |= 1 << 2;
        PTD->PSOR |= 1 << 3;
        PTD->PSOR |= 1 << 4;
        PTD->PSOR |= 1 << 5;
        PTD->PSOR |= 1 << 6;
        PTD->PSOR |= 1 << 7;
        break;
    case 7:
        PTD->PSOR |= 1 << 1;
        PTD->PSOR |= 1 << 2;
        PTD->PSOR |= 1 << 3;
        PTD->PCOR |= 1 << 4;
        PTD->PCOR |= 1 << 5;
        PTD->PSOR |= 1 << 6;
        PTD->PCOR |= 1 << 7;
        break;
    case 8:
        PTD->PSOR |= 1 << 1;
        PTD->PSOR |= 1 << 2;
        PTD->PSOR |= 1 << 3;
        PTD->PSOR |= 1 << 4;
        PTD->PSOR |= 1 << 5;
        PTD->PSOR |= 1 << 6;
        PTD->PSOR |= 1 << 7;
        break;
    case 9:
        PTD->PSOR |= 1 << 1;
        PTD->PSOR |= 1 << 2;
        PTD->PSOR |= 1 << 3;
        PTD->PSOR |= 1 << 4;
        PTD->PCOR |= 1 << 5;
        PTD->PSOR |= 1 << 6;
        PTD->PSOR |= 1 << 7;
        break;
    }
}

void Seg_out(int number)
{
    int d1000 = number % 10000 / 1000;
    int d100 = number % 1000 / 100;
    int d10 = number % 100 / 10;
    int d1 = number % 10;

    num(d1000);
    PTD->PCOR |= 1 << 11;
    PTD->PCOR |= 1 << 10;
    PTD->PCOR |= 1 << 9;
    PTD->PSOR |= 1 << 8;
    delay_ms(2);

    num(d100);
    PTD->PCOR |= 1 << 11;
    PTD->PCOR |= 1 << 10;
    PTD->PSOR |= 1 << 9;
    PTD->PCOR |= 1 << 8;
    delay_ms(2);

    num(d10);
    PTD->PCOR |= 1 << 11;
    PTD->PSOR |= 1 << 10;
    PTD->PCOR |= 1 << 9;
    PTD->PCOR |= 1 << 8;
    delay_ms(2);

    num(d1);
    PTD->PSOR |= 1 << 11;
    PTD->PCOR |= 1 << 10;
    PTD->PCOR |= 1 << 9;
    PTD->PCOR |= 1 << 8;
    delay_ms(2);
}

int main(void)
{
    WDOG_disable();
    PORT_init();
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    ADC_init();

    while (1)
    {
        convertAdcChan(13); /* Convert Channel AD13 to pot on EVB 	*/
        while (adc_complete() == 0)
        {
        }
        int adcResultInMv = read_adc_chx();
        Seg_out(adcResultInMv);
    }
}