// 마이크로컨트롤러에서 Delay를 활용하기 위한 헤더파일을 추가합니다.
#include "clocks_and_modes.h"
// 마이크로컨트롤러의 레지스터와 설정정보를 정의하는 헤더파일을 포함시킵니다.
#include "device_registers.h"

#define PTD1 1
#define PTD2 2
#define PTD3 3
#define PTD4 4
#define PTD5 5
#define PTD6 6
#define PTD7 7
#define PTD8 8

// Delay_ms 함수에 사용될 전역변수를 선언합니다.
int lpit0_ch0_flag_counter = 0;

void PORT_init(void)
{
    /*
    <Clock Setting>
    -마이크로컨트롤러는 내부 레지스터와 장치를 동작시키기 위해 클럭신호를 필요로 합니다.
    -클록세팅은 마이크로컨트롤러의 클럭소스 및 분주비를 설정하는 과정을 말합니다.
    -아래 코드는 포트 D(PTD)의 클럭게이트를 활성화하는 역할을 합니다. 이렇게 하면 해당 포트의 기능을 사용합니다.
    -클럭설정은 마이크로컨트롤러의 동작 및 정확성에 중요한 영향을 미치므로 주의깊게 설정해야 합니다.
    */
    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;

    /*
    <Direction Setting>
    -마이크로컨트롤러의 핀은 입력 또는 출력으로 동작할 수 있다.
    -디렉션세팅은 각 핀의 동작 방향을 설정한다는 과정을 위와 같이 거쳐야 합니다.
    -첫번쨰 코드는 PTD1~~8핀을 출력으로 설정하는 것을 나타냅니다. 즉, 이 핀은 데이터를 출력하는
    역할을 하게 됩니다.
    -출력방향으로 설정된 핀이므로, 외부로 데이터를 출력하게 됩니다.
    */
    PTD->PDDR |= 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6 | 1 << 7 | 1 << 8;

    /*
    <GPIO Setting>
    -"GPIO"는 "범용입출력"으로, 일반적인 입출력포트를 나타냅니다
    -GPIO세팅은 특정 핀을 GPIO로 구성하는 과정으로, 이렇게 설정된 핀으로
     Configuration을 처리할 수 있습니다.
    -GPIO핀 설정은 주로 디지털 신호를 다룰때 사용되며, 특정 입출력 장치와 상호작용할 때 필요합니다.
    */
    for (int i = 1; i < 9; i++)
    {
        PORTD->PCR[i] = PORT_PCR_MUX(1);
    }
}

/*
시스템이 정상동작하지 않는다고 판단되면,
일정시간이 지난 후 와치독 타이머를 비활성화하여, 시스템이 재시작하지 않도록 합니다.
*/
void WDOG_disable(void)
{
    /**
     * CNT 레지스터를 특정한 용도로 사용하고자 초기화합니다.
     */
    WDOG->CNT = 0xD928C520; /* Unlock Watchdog */

    /**
     * TOVAL 레지스터를 최대타임아웃값으로 초기화합니다.
     */
    WDOG->TOVAL = 0x0000FFFF; /* Maximum timeout value */

    /**
     * CS 레지스터에 플래그값을 넣어 비활성화합니다.
     */
    WDOG->CS = 0x00002100;
}

/**
 * LPIT0 채널0을 초기화합니다
 * -SPLL_DIV2_CLK의 클록소스를 활성화합니다.
 * LPIT0 레지스터에 대한 클럭 활성화
 * LPIT0 모듈 활성화
 * 채널 0 초기화: (시간 초과) = (시계의 1초)
 * 모드를 32비트 카운터로 설정하고 채널0을 활성화합니다.
 */
void LPII0_init(uint32_t delay)
{
    uint32_t timeout;
    /**
     * LPIT Clocking:
     * ========================
     */
    PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* CLock Src = 6 (SPLL2_DIV2_CLK) */
    PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs*/

    /**
     * LPIT Initialization
     */
    LPIT0->MCR |= LPIT_MCR_M_CEN_MASK; /* DBG_en-0: Timer chans stop in Debug mode */
                                       /* DOZE_EN=0: Timer chans are stopped in DOZE mode*/
                                       /* SW_RST=0: SW reset does not reset timer chans, regs */
                                       /* M_CEN=1: enable module clk (allows writing other LPIT0 regs)*/

    timeout = delay * 40000;
    LPIT0->TMR[0].TVAL = timeout; /* Chan 0 Timeout period: 40Mm clocks */
    LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;
}

void delay_ms(volatile int ms)
{
    LPII0_init(ms); /* LPIT0를 사용한 지연 초기화 */

    while ((LPIT0->MSR & LPIT_MSR_TIF0_MASK) == 0)
    {
        lpit0_ch0_flag_counter++; /* LPIT0 타이머가 만료될 때까지 대기 */
    }
    LPIT0->MSR = LPIT_MSR_TIF0_MASK; /* 타이머 인터럽트 플래그 클리어 */
}

/* Function Definition Section (start)*/
void shiftRight();
void shiftLeft();
/* Function Definition Section (end)*/
int main()
{
    WDOG_disable();        /* Watchdog 비활성화 */
    PORT_init();           /* 포트 초기화 */
    SOSC_init_8MHz();      /* 8MHz 시스템 오실레이터 초기화 */
    SPLL_init_160MHz();    /* 160MHz 시스템 PLL 초기화 */
    NormalRUNmode_80MHz(); /* 80MHz sysclk & core, 40 Mhz, 20Mhz 초기화*/

    int Dtime = 500; /* 딜레이 시간 설정 */

    while (1)
    {
        shiftRight();
        shiftLeft();
    }
}

/* Function Implementation Section (start)*/
void shiftRight()
{
    PTD->PSOR |= 1 << PTD1; /* 포트 D1을 끕니다. */
    PTD->PCOR |= 1 << PTD2; /* 포트 D2을 켭니다. */
    delay_ms(Dtime);

    PTD->PSOR |= 1 << PTD2; /* 포트 D2을 끕니다. */
    PTD->PCOR |= 1 << PTD3; /* 포트 D3을 켭니다. */
    delay_ms(Dtime);

    PTD->PSOR |= 1 << PTD3; /* 포트 D3을 끕니다. */
    PTD->PCOR |= 1 << PTD4; /* 포트 D4을 켭니다. */
    delay_ms(Dtime);

    PTD->PSOR |= 1 << PTD4; /* 포트 D4을 끕니다. */
    PTD->PCOR |= 1 << PTD5; /* 포트 D5을 켭니다. */
    delay_ms(Dtime);

    PTD->PSOR |= 1 << PTD5; /* 포트 D5를 끕니다. */
    PTD->PCOR |= 1 << PTD6; /* 포트 D6을 켭니다. */
    delay_ms(Dtime);

    PTD->PSOR |= 1 << PTD6; /* 포트 D5를 끕니다. */
    PTD->PCOR |= 1 << PTD7; /* 포트 D6을 켭니다. */
    delay_ms(Dtime);

    PTD->PSOR |= 1 << PTD7; /* 포트 D7를 끕니다. */
    PTD->PCOR |= 1 << PTD8; /* 포트 D8을 켭니다. */
    delay_ms(Dtime);
}
void shiftLeft()
{
    PTD->PSOR |= 1 << PTD8; /* 포트 D8를 끕니다. */
    PTD->PCOR |= 1 << PTD7; /* 포트 D7을 켭니다. */
    delay_ms(Dtime);

    PTD->PSOR |= 1 << PTD7; /* 포트 D7를 끕니다. */
    PTD->PCOR |= 1 << PTD6; /* 포트 D6을 켭니다. */
    delay_ms(Dtime);

    PTD->PSOR |= 1 << PTD6; /* 포트 D6를 끕니다. */
    PTD->PCOR |= 1 << PTD5; /* 포트 D5을 켭니다. */
    delay_ms(Dtime);

    PTD->PSOR |= 1 << PTD5; /* 포트 D5를 끕니다. */
    PTD->PCOR |= 1 << PTD4; /* 포트 D4을 켭니다. */
    delay_ms(Dtime);

    PTD->PSOR |= 1 << PTD4; /* 포트 D4를 끕니다. */
    PTD->PCOR |= 1 << PTD3; /* 포트 D3을 켭니다. */
    delay_ms(Dtime);

    PTD->PSOR |= 1 << PTD3; /* 포트 D3를 끕니다. */
    PTD->PCOR |= 1 << PTD2; /* 포트 D2을 켭니다. */
    delay_ms(Dtime);

    PTD->PSOR |= 1 << PTD2; /* 포트 D2를 끕니다. */
    PTD->PCOR |= 1 << PTD1; /* 포트 D1을 켭니다. */
    delay_ms(Dtime);
}
/* Function Implementation Section (end)*/