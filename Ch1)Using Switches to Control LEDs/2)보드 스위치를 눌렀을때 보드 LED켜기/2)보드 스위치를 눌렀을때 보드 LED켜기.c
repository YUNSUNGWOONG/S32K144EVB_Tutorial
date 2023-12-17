// 레지스터 및 라이브러리 정의를 제공하는 헤더파일
#include "device_registers.h"

/*
매크로 상수를 정의하는 부분으로,
PTD0과 PTD12의 상수를 정의합니다.
이를 통해 포트D의 다양한 핀을 이용하게 됩니다.
*/
#define PTD0 0
#define PTC12 12

void PORT_init()
{
    /*
    <Clock Setting>
    -마이크로 컨트롤러는 내부 레지스터와 장치를 동작시키기 위해 클럭신호를 필요로 합니다.
    -클록세팅은 마이크로컨트롤러의 클럭소스 및 분주기를 설정하는 과정을 말합니다.
    -아래 코드는 포트D의 클럭게이트를 활성화하는 역할을 합니다. 이렇게 하면 해당포트의 기능을 사용합니다.
    -클럭설정은 마이크로컨트롤러의 동작 및 정확성에 중요한 영향을 미치므로 주의깊게 설정해야 합니다.
    */
    PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;

    /*
    <Direction Setting>
    -마이크로컨트롤러의 핀은 입력 또는 출력으로 동작할 있습니다.
    -디렉션세팅은 각 판의 동작 방향을 설정한다는 과정을 위와 같이 거쳐야 합니다.
    -아래 첫번째 코드는 PTD0핀을 출력으롤 설정하는 것을 의미합니다. 즉, 이 핀은 데이터를 출력하는 역할을 하게 됩니다.
    -두번쨰 코드는 PTC12 핀을 입력으로 설정하는 것을 의미합니다. 즉, 이 핀은 데이터를 입력하는 역할을 하게 됩니다.
    (OR연산자를 쓰면 출력이고, AND연산자를 쓰면 입력인 것으로 생각할 수 있습니다.)
    -입력방향으로 설정된 핀은 외부에서 데이터를 읽어들이고, 출력방향으로 설정된 핀은 외부로 데이터를
    출력하게 됩니다.
    */
    PTD->PDDR |= 1 << PTD0;
    PTC->PDDR &= ~((unsigned int)1 << PTC12);

    /*
    <GPIO Setting>
    -범용입출력, GPIO는 일반적인 입출력포트를 나타냅니다.
    -GPIO세팅은 특정핀을 범용입출력으로 컴포즈하는 과정으로, 이렇게 설정된 핀으로 입출력 configuration을 처리할 수
    있습니다. -아래 첫번째 코드는 PTD0핀을 GPIO핀으로 설정되는 것을 나타냅니다. -두번째 코드는 PTC12핀을 GPIO핀으로
    설정되는 것을 나타냅니다. -GPIO핀 설정은 주로 디지털 신호를 다룰때 사용되며, 특정 입출력 장치와 상호작용을 할때
    필요합니다.
    */
    PORTD->PCR[0] = PORT_PCR_MUX(1);
    PORTD->PCR[12] = PORT_PCR_MUX(1);
}

int main()
{
    PORT_init();

    while (1)
    {
        /*
        포트C 12번 핀의 입력상태를
        */
        if (!(PTC->PDIR & (1 << PTC12)))
        {
            PTD->PSOR |= 1 << PTD0; // 출력핀(0)-TURN_ON
        }
        else
        {
            PTD->PCOR |= 1 << PTD0; // 출력핀(0)-TURN_OFF
        }
    }
}



#include "device_registers.h"
#define PTD0 0
int main(void)
{
    PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK;
    PORTD->PCR[0] = PORT_PCR_MUX(1);
    PTD->PDDR |= 1 << PTD0;
    for(;;)
    {
        int cycles = 720000;
        while(cycles--);
        PTD-> PTOR |= 1<<PTD0;
    }
}