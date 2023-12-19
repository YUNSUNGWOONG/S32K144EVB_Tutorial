#include "clocks_and_modes.h"
#include "device_registers.h"

int lpit0_ch0_flag_counter = 0; /*< LPIT0 timeout counter */
unsigned int FND_DATA[10] = {0x7E, 0x0C, 0xB6, 0x9E, 0xCC, 0xDA, 0xFA, 0x4E, 0xFE, 0xCE};
unsigned int FND_SEL[4] = {0x0100, 0x0200, 0x0400, 0x0800};
unsigned int j = 0; /*FND select pin index */
unsigned int num, num0, num1, num2, num3 = 0;
/*num is Counting value, num0 is '1', num2 is '10', num2 is '100', num3 is '1000'*/
unsigned int Dtime = 0; /* Delay Time Setting Variable*/

void PORT_init(void)
{
    /*
     * ===============세그먼트 포트 초기화=====================
     */
    PCC->PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT E */
    PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT D */
    /*
    <Clock Setting>
    -마이크로 컨트롤러는 내부 레지스터와 장치를 동작시키기 위해 클럭신호를 필요로 합니다.
    -"Clock Setting"은 마이크로컨트롤러의 클럭소스 및 분주비를 설정하는 과정을 말합니다.
    -위 코드는 포트 D(PTD)의 클럭게이트를 활성화하는 역할을 합니다. 이렇게 하면 해당 포트의 기능을 사용할 수 있습니다.
    -클럭설정은 마이크로 컨트롤러의 동작 및 정확성에 중요한 영향을 미치므로 주의깊게 설정해야 합니다.
    */

    PTE->PDDR |= 1 << 12 | 1 << 14 | 1 << 15;                  /* Port E12,E14,E15:  Data Direction = output */
    PTE->PDDR &= ~((1 << 0) & (1 << 1) & (1 << 2) & (1 << 3)); /* Port E0,E1,E2,E3:  Data Direction = input */
    /*
    <Driection Setting>
    -마이크로컨트롤러의 핀은 입력 또는 출력으로 동작할 수 있는데,
    "Direction Setting"은 각 핀의 동작 방향을 설정한다는 과정을 위와 같이 거쳐야 합니다.
    */

    PORTE->PCR[0] = PORT_PCR_MUX(1) | PORT_PCR_PFE(1) | PORT_PCR_PE(1) |
                    PORT_PCR_PS(0); /* Port E0: MUX = GPIO, input filter enabled, internal pull-down enabled */
    PORTE->PCR[1] = PORT_PCR_MUX(1) | PORT_PCR_PFE(1) | PORT_PCR_PE(1) |
                    PORT_PCR_PS(0); /* Port E1: MUX = GPIO, input filter enabled, internal pull-down enabled */
    PORTE->PCR[2] = PORT_PCR_MUX(1) | PORT_PCR_PFE(1) | PORT_PCR_PE(1) |
                    PORT_PCR_PS(0); /* Port E2: MUX = GPIO, input filter enabled, internal pull-down enabled */
    PORTE->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PFE(1) | PORT_PCR_PE(1) |
                    PORT_PCR_PS(0); /* Port E3: MUX = GPIO, input filter enabled, internal pull-down enabled */

    PORTE->PCR[12] = PORT_PCR_MUX(1); /* Port E12: MUX = GPIO  */
    PORTE->PCR[14] = PORT_PCR_MUX(1); /* Port E14: MUX = GPIO  */
    PORTE->PCR[15] = PORT_PCR_MUX(1); /* Port E15: MUX = GPIO  */

    /* ===============PORT-D SEGMENT=====================  */
    PTD->PDDR |=
        1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7; /* Port D1-D7: Data Direction= output (default) */

    PORTD->PCR[1] = PORT_PCR_MUX(1); /* Port D1: MUX = GPIO */
    PORTD->PCR[2] = PORT_PCR_MUX(1); /* Port D2: MUX = GPIO */
    PORTD->PCR[3] = PORT_PCR_MUX(1); /* Port D3: MUX = GPIO */
    PORTD->PCR[4] = PORT_PCR_MUX(1); /* Port D4: MUX = GPIO */
    PORTD->PCR[5] = PORT_PCR_MUX(1); /* Port D5: MUX = GPIO */
    PORTD->PCR[6] = PORT_PCR_MUX(1); /* Port D6: MUX = GPIO */
    PORTD->PCR[7] = PORT_PCR_MUX(1); /* Port D7: MUX = GPIO */

    PTD->PDDR |= 1 << 8 | 1 << 9 | 1 << 10 | 1 << 11;
    PORTD->PCR[8] = PORT_PCR_MUX(1);  /* Port D8: MUX = GPIO */
    PORTD->PCR[9] = PORT_PCR_MUX(1);  /* Port D9: MUX = GPIO */
    PORTD->PCR[10] = PORT_PCR_MUX(1); /* Port D10: MUX = GPIO */
    PORTD->PCR[11] = PORT_PCR_MUX(1); /* Port D11: MUX = GPIO */
    /*
    <GPIO Setting>
    -"GPIO"는 eneral Puerpose Input/Output의 약어로, 일반적인 입출력포트를 나타냅니다.
    -"GPIO Setting"은 특정핀을 GPIO로 구성하는 과정으로, 이렇게 설정된 핀으로 입출력 configuration을 처리할 수
    있습니다. -GPIO 핀 설정은 주로 디지털 신호를 다룰때 사용되며, 특정 입출력 장치와 상호작용할 때 필요합니다.
    */
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

void delay_us(volatile int us)
{
    LPIT0_init(us); /* Initialize PIT0 for 1 second timeout  */
    while (0 == (LPIT0->MSR & 0x01 /*LPIT_MSR_TIF0_MASK*/))
    {
    }                         /* Wait for LPIT0 CH0 Flag */
    lpit0_ch0_flag_counter++; /* Increment LPIT0 timeout counter */
    LPIT0->MSR |= 0x00;       //............LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}

void Seg_out(int number)
{
    Dtime = 1000;

    num3 = number / 1000;
    num2 = (number % 1000) / 100;
    num1 = (number % 100) / 10;
    num0 = number % 10;

    // 1000??? ??
    PTD->PSOR = FND_SEL[j];
    PTD->PCOR = 0x7f;
    PTD->PSOR = FND_DATA[num3];
    delay_us(Dtime);
    PTD->PCOR = 0xfff;
    j++;

    // 100??? ??
    PTD->PSOR = FND_SEL[j];
    PTD->PCOR = 0x7f;
    PTD->PSOR = FND_DATA[num2];
    delay_us(Dtime);
    PTD->PCOR = 0xfff;
    j++;

    // 10??? ??
    PTD->PSOR = FND_SEL[j];
    PTD->PCOR = 0x7f;
    PTD->PSOR = FND_DATA[num1];
    delay_us(Dtime);
    PTD->PCOR = 0xfff;
    j++;

    // 1??? ??
    PTD->PSOR = FND_SEL[j];
    PTD->PCOR = 0x7f;
    PTD->PSOR = FND_DATA[num0];
    delay_us(Dtime);
    PTD->PCOR = 0xfff;
    j = 0;
}

int KeyScan(void)
{
    int Kbuff = -1;

    Dtime = 1000;

    PTE->PSOR |= 1 << 12;
    delay_us(Dtime);
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
    delay_us(Dtime);
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
    delay_us(Dtime);
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

    int key = 0;

    for (;;)
    {
        key = KeyScan();
        Seg_out(key);
    }
}
