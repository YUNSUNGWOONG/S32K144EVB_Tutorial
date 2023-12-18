#include "clocks_and_modes.h"
#include "device_registers.h"

#define PTD1 1
#define PTD2 2
#define PTD3 3
#define PTD4 4
#define PTD5 5
#define PTD6 6
#define PTD7 7
#define PTD8 8
#define PTD9 9
#define PTD10 10
#define PTD11 11
#define PTD13 13

int lpit0_ch0_flag_counter = 0; /*< LPIT0 timeout counter */

unsigned int num = 0;
unsigned int num0, num1, num2, num3 = 0;

unsigned int j = 0; /*FND select pin index */

unsigned int FND_DATA[10] = {0x7E, 0x0C, 0xB6, 0x9E, 0xCC, 0xDA, 0xFA, 0x4E, 0xFE, 0xCE}; // 0~9 number
unsigned int Delaytime = 0; /* Delay Time Setting Variable*/

void LPIT0_init(uint32_t delay)
{
    /*
    이 코드는 LPIT0 모듈을 사용하여 주기적으로 타이머 인터럽트를 생성하는 간단한 설정을 수행합니다.
    delay 값과 timeout 값을 조정하여 원하는 타이머 주기를 설정할 수 있습니다.
    */

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

    timeout = delay * 40000;
    LPIT0->TMR[0].TVAL = timeout; /* Chan 0 Timeout period: 40M clocks */
    LPIT0->TMR[0].TCTRL |= 0x11;  //.................LPIT_TMR_TCTRL_T_EN_MASK;
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
    while (0 == (LPIT0->MSR & 0x01 /*LPIT_MSR_TIF0_MASK*/))
    {
    }                         /* Wait for LPIT0 CH0 Flag */
    lpit0_ch0_flag_counter++; /* Increment LPIT0 timeout counter */
    LPIT0->MSR |= 0x00;       //............LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}

void PORT_init(void)
{
    /*
     * ===============세그먼트 포트 초기화=====================
     */

    PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT D */
    /*
    <Clock Setting>
    -마이크로 컨트롤러는 내부 레지스터와 장치를 동작시키기 위해 클럭신호를 필요로 합니다.
    -"Clock Setting"은 마이크로컨트롤러의 클럭소스 및 분주비를 설정하는 과정을 말합니다.
    -위 코드는 포트 D(PTD)의 클럭게이트를 활성화하는 역할을 합니다. 이렇게 하면 해당 포트의 기능을 사용할 수 있습니다.
    -클럭설정은 마이크로 컨트롤러의 동작 및 정확성에 중요한 영향을 미치므로 주의깊게 설정해야 합니다.
    */

    PTD->PDDR |= 1 << PTD1 | 1 << PTD2 | 1 << PTD3 | 1 << PTD4 | 1 << PTD5 | 1 << PTD6 | 1 << PTD7 | 1 << PTD8 |
                 1 << PTD9 | 1 << PTD10 | 1 << PTD11; /* Port D1-D7: Data Direction= output (default) */
    PTD->PDDR &= ~((unsigned int)1 << PTD13);
    /*
    <Driection Setting>
    -마이크로컨트롤러의 핀은 입력 또는 출력으로 동작할 수 있는데,
    "Direction Setting"은 각 핀의 동작 방향을 설정한다는 과정을 위와 같이 거쳐야 합니다.
    -위 첫번째 코드는 PTD1~7핀을  세그먼트 제어설정하는 것을 나타냅니다.즉, 이 핀은 세그먼트에 데이터를 출력하는 역할을
    하게 됩니다.
    -위 두번째 코드는 PTD8~11핀을 FND 제어설정하는 것을 나타냅니다. 즉, 이 핀든 FND에 데이터를 출력하는
    역할을 하게 됩니다.
    -위 세번째 코드는 PTD13핀을 SW 제어설정하는 것을 나타냅니다. 즉, 이 핀은 SW로 데이터를 입력하는 역할을 하게 됩니다.
    */

    PORTD->PCR[1] = PORT_PCR_MUX(1); /* Port D1: MUX = GPIO */
    PORTD->PCR[2] = PORT_PCR_MUX(1); /* Port D2: MUX = GPIO */
    PORTD->PCR[3] = PORT_PCR_MUX(1); /* Port D3: MUX = GPIO */
    PORTD->PCR[4] = PORT_PCR_MUX(1); /* Port D4: MUX = GPIO */
    PORTD->PCR[5] = PORT_PCR_MUX(1); /* Port D5: MUX = GPIO */
    PORTD->PCR[6] = PORT_PCR_MUX(1); /* Port D6: MUX = GPIO */
    PORTD->PCR[7] = PORT_PCR_MUX(1); /* Port D7: MUX = GPIO */

    PORTD->PCR[8] = PORT_PCR_MUX(1);  /* Port D8: MUX = GPIO */
    PORTD->PCR[9] = PORT_PCR_MUX(1);  /* Port D9: MUX = GPIO */
    PORTD->PCR[10] = PORT_PCR_MUX(1); /* Port D10: MUX = GPIO */
    PORTD->PCR[11] = PORT_PCR_MUX(1); /* Port D11: MUX = GPIO */

    PORTD->PCR[13] = PORT_PCR_MUX(1);
    /*
   <GPIO Setting>
   -"GPIO"는 eneral Puerpose Input/Output의 약어로, 일반적인 입출력포트를 나타냅니다.
   -"GPIO Setting"은 특정핀을 GPIO로 구성하는 과정으로, 이렇게 설정된 핀으로 입출력 configuration을 처리할 수 있습니다.
   -GPIO 핀 설정은 주로 디지털 신호를 다룰때 사용되며, 특정 입출력 장치와 상호작용할 때 필요합니다.
   */
}

void Seg_out(int number)
{

    Delaytime = 1;

    num3 = (number / 1000) % 10;
    num2 = (number / 100) % 10;
    num1 = (number / 10) % 10;
    num0 = number % 10;

    // 1000??? ??
    PTD->PSOR = FND_SEL[j];
    PTD->PCOR = 0x7f;
    PTD->PSOR = FND_DATA[num3];
    delay_ms(Delaytime);
    PTD->PCOR = 0xfff;
    j++;

    // 100??? ??
    PTD->PSOR = FND_SEL[j];
    PTD->PCOR = 0x7f;
    PTD->PSOR = FND_DATA[num2];
    delay_ms(Delaytime);
    PTD->PCOR = 0xfff;
    j++;

    // 10??? ??
    PTD->PSOR = FND_SEL[j];
    PTD->PCOR = 0x7f;
    PTD->PSOR = FND_DATA[num1];
    delay_ms(Delaytime);
    PTD->PCOR = 0xfff;
    j++;

    // 1??? ??
    PTD->PSOR = FND_SEL[j];
    PTD->PCOR = 0x7f;
    PTD->PSOR = FND_DATA[num0];
    delay_ms(Delaytime);
    PTD->PCOR = 0xfff;
    j = 0;
}

int main(void)
{
    int count = 94;

    PORT_init();           /* Configure ports */
    SOSC_init_8MHz();      /* Initialize system oscilator for 8 MHz xtal */
    SPLL_init_160MHz();    /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
    NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */

    while (1)
    {
        if (!(PTD->PDIR & (1 << PTD13)))
        {
            Seg_out(00); // TURN-ON
        }
        else
        {
            Seg_out(94); // TURN-OFF
        }
    }
}
