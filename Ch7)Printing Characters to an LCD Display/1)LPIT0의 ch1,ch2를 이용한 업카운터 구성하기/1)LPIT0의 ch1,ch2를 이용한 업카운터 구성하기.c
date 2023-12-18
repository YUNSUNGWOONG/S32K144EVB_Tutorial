#include "clocks_and_modes.h"
#include "device_registers.h" /* include peripheral declarations S32K144 */

// SEGMENT
#define PTE1 1
#define PTE2 2
#define PTE3 3
#define PTE4 4
#define PTE5 5
#define PTE6 6
#define PTE7 7
#define PTE8 8
#define PTE9 9
#define PTE10 10
#define PTE11 11

int lpit0_ch0_flag_counter = 0; /*< LPIT0 timeout counter */
int lpit0_ch1_flag_counter = 0; /*< LPIT0 timeout counter */

unsigned int num = 0;
unsigned int num0, num1, num2, num3 = 0;

unsigned int j = 0; /*FND select pin index */

unsigned int FND_DATA[10] = {0x7E, 0x0C, 0xB6, 0x9E, 0xCC, 0xDA, 0xFA, 0x4E, 0xFE, 0xCE}; // 0~9 number
unsigned int Delaytime = 0; /* Delay Time Setting Variable*/

unsigned int FND_SEL[4] = {0x0100, 0x0200, 0x0400, 0x0800};

void NVIC_init_IRQs(void)
{
    /*LPIT ch0 overflow set*/
    S32_NVIC->ICPR[1] = 1 << (48 % 32);
    S32_NVIC->ISER[1] = 1 << (48 % 32);
    S32_NVIC->IP[48] = 0x00;
    /*LPIT ch1 overflow set*/
    S32_NVIC->ICPR[1] = 1 << (49 % 32);
    S32_NVIC->ISER[1] = 1 << (49 % 32);
    S32_NVIC->IP[49] = 0x0B;
}

void PORT_init(void)
{
    /*
     * ===============PORT-E SEGMENT=====================
     */
    PCC->PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT D */

    PTE->PDDR |= 1 << PTE1 | 1 << PTE2 | 1 << PTE3 | 1 << PTE4 | 1 << PTE5 | 1 << PTE6 | 1 << PTE7 | 1 << PTE8 |
                 1 << PTE9 | 1 << PTE9 | 1 << PTE10 | 1 << PTE11;

    PORTE->PCR[1] = PORT_PCR_MUX(1);  /* Port D1: MUX = GPIO */
    PORTE->PCR[2] = PORT_PCR_MUX(1);  /* Port D2: MUX = GPIO */
    PORTE->PCR[3] = PORT_PCR_MUX(1);  /* Port D3: MUX = GPIO */
    PORTE->PCR[4] = PORT_PCR_MUX(1);  /* Port D4: MUX = GPIO */
    PORTE->PCR[5] = PORT_PCR_MUX(1);  /* Port D5: MUX = GPIO */
    PORTE->PCR[6] = PORT_PCR_MUX(1);  /* Port D6: MUX = GPIO */
    PORTE->PCR[7] = PORT_PCR_MUX(1);  /* Port D7: MUX = GPIO */
    PORTE->PCR[8] = PORT_PCR_MUX(1);  /* Port D8: MUX = GPIO */
    PORTE->PCR[9] = PORT_PCR_MUX(1);  /* Port D9: MUX = GPIO */
    PORTE->PCR[10] = PORT_PCR_MUX(1); /* Port D10: MUX = GPIO */
    PORTE->PCR[11] = PORT_PCR_MUX(1); /* Port D11: MUX = GPIO */
}

void WDOG_disable(void)
{
    WDOG->CNT = 0xD928C520;   /* Unlock watchdog       */
    WDOG->TOVAL = 0x0000FFFF; /* Maximum timeout value    */
    WDOG->CS = 0x00002100;    /* Disable watchdog       */
}

void LPIT0_init(uint32_t Dtime)
{
    /*!
     * LPIT Clocking:
     * ==============================
     */
    PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* Clock Src = 6 (SPLL2_DIV2_CLK)*/
    PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs 		*/
                                                    /*!
                                                     * LPIT Initialization:
                                                     */
    LPIT0->MCR = 0x00000001;                        /* DBG_EN-0: Timer chans stop in Debug mode */
                                                    /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
                                                    /* SW_RST=0: SW reset does not reset timer chans, regs */
                                                    /* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */
    LPIT0->MIER = 0x03;                             /* TIE0=1: Timer Interrupt Enabled fot Chan 0,1 */

    LPIT0->TMR[0].TVAL = 40000000; /* Chan 0 Timeout period: 40M clocks */
    LPIT0->TMR[0].TCTRL = 0x00000001;
    /* T_EN=1: Timer channel is enabled */
    /* CHAIN=0: channel chaining is disabled */
    /* MODE=0: 32 periodic counter mode */
    /* TSOT=0: Timer decrements immediately based on restart */
    /* TSOI=0: Timer does not stop after timeout */
    /* TROT=0 Timer will not reload on trigger */
    /* TRG_SRC=0: External trigger soruce */
    /* TRG_SEL=0: Timer chan 0 trigger source is selected*/

    LPIT0->TMR[1].TVAL = Dtime * 40000; /* Chan 1 Timeout period: 40M clocks */
    LPIT0->TMR[1].TCTRL = 0x00000001;
    /* T_EN=1: Timer channel is enabled */
    /* CHAIN=0: channel chaining is disabled */
    /* MODE=0: 32 periodic counter mode */
    /* TSOT=0: Timer decrements immediately based on restart */
    /* TSOI=0: Timer does not stop after timeout */
    /* TROT=0 Timer will not reload on trigger */
    /* TRG_SRC=0: External trigger soruce */
    /* TRG_SEL=0: Timer chan 0 trigger source is selected*/
}

void delay_ms(volatile int us)
{
    LPIT0_init(us); /* Initialize PIT0 for 1 second timeout  */
    while (0 == (LPIT0->MSR & LPIT_MSR_TIF1_MASK))
        lpit0_ch1_flag_counter++;     /* Increment LPIT0 timeout counter */
    LPIT0->MSR |= LPIT_MSR_TIF1_MASK; //............LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}

void Seg_out(int number)
{

    Delaytime = 1;

    num3 = (number / 1000) % 10;
    num2 = (number / 100) % 10;
    num1 = (number / 10) % 10;
    num0 = number % 10;

    // 1000의 자리
    PTE->PSOR = FND_SEL[j];
    PTE->PCOR = 0x7f;
    PTE->PSOR = FND_DATA[num3];
    delay_ms(Delaytime);
    PTE->PCOR = 0xfff;
    j++;

    // 100의 자리
    PTE->PSOR = FND_SEL[j];
    PTE->PCOR = 0x7f;
    PTE->PSOR = FND_DATA[num2];
    delay_ms(Delaytime);
    PTE->PCOR = 0xfff;
    j++;

    // 10의 자리
    PTE->PSOR = FND_SEL[j];
    PTE->PCOR = 0x7f;
    PTE->PSOR = FND_DATA[num1];
    delay_ms(Delaytime);
    PTE->PCOR = 0xfff;
    j++;

    // 1의 자리
    PTE->PSOR = FND_SEL[j];
    PTE->PCOR = 0x7f;
    PTE->PSOR = FND_DATA[num0];
    delay_ms(Delaytime);
    PTE->PCOR = 0xfff;
    j = 0;
}

void LPIT0_Ch1_IRQHandler(void)
{                                     /* delay counter */
    lpit0_ch1_flag_counter++;         /* Increment LPIT1 timeout counter */
    LPIT0->MSR |= LPIT_MSR_TIF1_MASK; /* Clear LPIT0 timer flag 0 */
}

void LPIT0_Ch0_IRQHandler(void)
{
    lpit0_ch0_flag_counter++; /* Increment LPIT0 timeout counter */

    LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
    num++;
}

int main(void)
{
    WDOG_disable();        /* Disable Watchdog in case it is not done in startup code */
    PORT_init();           /* Configure ports */
    SOSC_init_8MHz();      /* Initialize system oscilator for 8 MHz xtal */
    SPLL_init_160MHz();    /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
    NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
    NVIC_init_IRQs();      /* Enable desired interrupts and priorities */
    LPIT0_init(1);

    for (;;)
    {
        Seg_out(num);
    }
}
