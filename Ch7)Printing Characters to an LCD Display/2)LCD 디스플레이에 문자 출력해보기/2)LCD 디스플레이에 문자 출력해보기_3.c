#include "clocks_and_modes.h"
#include "device_registers.h"
#include "lcd1602A.h"
/* 마이크로컨트롤러의 레지스터와 설정정보를 정의하는 헤더파일을 포함시킵니다. */
void NVIC_init_IRQs(void)
{
    S32_NVIC->ICPR[1] = 1 << (48 % 32); /* IRQ48-LPIT0 ch0: clr any pending IRQ Set to FSL_ for S32K144.h */
    S32_NVIC->ISER[1] = 1 << (48 % 32); /* IRQ48-LPIT0 ch0: enable IRQ */
    S32_NVIC->IP[48] = 0x00;            /* IRQ48-LPIT0 ch0: priority 10 of 0~15 */

    S32_NVIC->ICPR[1] = 1 << (49 % 32); /* IRQ48-LPIT1 ch0: clr any pending IRQ Set to FSL_ for S32K144.h */
    S32_NVIC->ISER[1] = 1 << (49 % 32); /* IRQ48-LPIT1 ch0: enable IRQ */
    S32_NVIC->IP[49] = 0x0B;            /* IRQ48-LPIT1 ch0: priority 10 of 0~15 */
}

void LPIT0_init(uint32_t Dtime)
{
    PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* Clock Src = 6 (SPLL2_DIV2_CLK)*/
    PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs */
    LPIT0->MCR = 0x00000001;                        /* DBG_EN-0: Timer chans stop in Debug mode */
                                                    /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
                                                    /* SW_RST=0: SW reset does not reset timer chans, regs */
                                                    /* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */
    LPIT0->MIER = 0x03;                             /* TIE0=1: Timer Interrupt Enabled fot Chan 0,1 */
    LPIT0->TMR[0].TVAL = 40000000;                  /* Chan 0 Timeout period: 40M clocks */
    LPIT0->TMR[0].TCTRL = 0x00000001;               /* T_EN=1: Timer channel is enabled */
                                                    /* CHAIN=0: channel chaining is disabled */
                                                    /* MODE=0: 32 periodic counter mode */
                                                    /* TSOT=0: Timer decrements immediately based on restart */
                                                    /* TSOI=0: Timer does not stop after timeout */
                                                    /* TROT=0 Timer will not reload on trigger */
                                                    /* TRG_SRC=0: External trigger soruce */
                                                    /* TRG_SEL=0: Timer chan 0 trigger source is selected*/
    LPIT0->TMR[1].TVAL = Dtime * 40000;             /* Chan 1 Timeout period: 40M clocks */
    LPIT0->TMR[1].TCTRL = 0x00000001;               /* T_EN=1: Timer channel is enabled */
                                                    /* CHAIN=0: channel chaining is disabled */
                                                    /* MODE=0: 32 periodic counter mode */
                                                    /* TSOT=0: Timer decrements immediately based on restart */
                                                    /* TSOI=0: Timer does not stop after timeout */
                                                    /* TROT=0 Timer will not reload on trigger */
                                                    /* TRG_SRC=0: External trigger soruce */
                                                    /* TRG_SEL=0: Timer chan 0 trigger source is selected*/
}

void PORT_init(void)
{
    // clock setting
    PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT D */

    // direction setting
    PTD->PDDR |=
        1 << 9 | 1 << 10 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14 | 1 << 15; /* Port D9~D15: Data Direction = output */

    // gpio setting
    PORTD->PCR[9] = PORT_PCR_MUX(1);
    PORTD->PCR[10] = PORT_PCR_MUX(1);
    PORTD->PCR[11] = PORT_PCR_MUX(1);
    PORTD->PCR[12] = PORT_PCR_MUX(1);
    PORTD->PCR[13] = PORT_PCR_MUX(1);
    PORTD->PCR[14] = PORT_PCR_MUX(1);
    PORTD->PCR[15] = PORT_PCR_MUX(1);
}

void delay_ms(volatile int ms) /* ms: delay Time Control Variable */
{
    LPIT0_init(ms);
    /* Initialize PIT0 for delay time, timeout */
    while (0 == (LPIT0->MSR & LPIT_MSR_TIF1_MASK))
    {
    }
    /* Wait for LPIT0 CH1 Flag */
    lpit0_ch0_flag_counter++;
    /* Increment LPIT0 timeout counter */
    LPIT0->MSR |= LPIT_MSR_TIF0_MASK;
    /* Clear LPIT0 timer flag 0 */
}

char msg_array[] = "HELLO_WORLD";     // 첫 번째 줄에 출력할 문자열
char msg_array2[] = "HELLO_STUDENTS"; // 두 번째 줄에 출력할 문자열

int main(void)
{
    PORT_init();           /* Configure ports */
    SOSC_init_8MHz();      /* Initialize system oscilator for 8 MHz xtal */
    SPLL_init_160MHz();    /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
    NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */

    NVIC_init_IRQs(); // 인터럽트 초기화
    lcdinit();        // 초기화(초기 DDRAM값 설정 완료)

    // 문자 출력 loop =========================================
    while (msg_array[i] != '\0')
    {
        lcdcharinput(msg_array[i]); // 1번째 줄에 출력할 문자열 LCD 모듈로 전송 (CGRAM 값 설정)
        delay_ms(800);              // 안정화(DDRAM값은 Busy Flag가 바뀌면서 자동으로 바뀐다)
        i++;
    }

    lcdinput(0x80 + 0x40); // 두번째 줄로(DDRAM 값 설정)
    delay_ms(200);
    i = 0;
    while (msg_array2[i] != '\0')
    {
        lcdcharinput(msg_array2[i]); // 2번쨰 줄에 출력할 문자열 LCD모듈로 전송 ( CGRAM 값 설정 )
        delay_ms(800);               // 안정화(DDRAM값은 Busy Flag가 바뀌면서 자동으로 바뀐다)
        i++;
    }
    //=========================================================

    // 문자 출력 종료 및 화면 초기화=============================
    delay_ms(2000);
    lcdinput(0x08); // lcd display off
    delay_ms(400);
    lcdinput(0x01); // Clear display
    delay_ms(200);
    //=========================================================

    return 0;
}