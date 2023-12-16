// 마이크로컨트롤러의 레지스터와 설정정보를 정의하는 헤더파일을 포함시킵니다.
#include "device_registers.h"

/*
매크로 상수를 정의하는 부분으로,
PTD1부터 PTD5까지와 PTD10부터 PTD14까지의 상수를 정의합니다.
이를 통해 포트D의 다양한 핀을 이용하게 됩니다.
*/
// SWITCH
#define PTD1 1
#define PTD2 2
#define PTD3 3
#define PTD4 4
#define PTD5 5

// LED
#define PTD10 10
#define PTD11 11
#define PTD12 12
#define PTD13 13
#define PTD14 14

void PORT_init(void)
{
    /*
    <Clock Setting>
    -마이크로 컨트롤러는 내부 레지스터와 장치를 동작시키기 위해 클럭신호를 필요로 합니다.
    -클록세팅은 마이크로컨트롤러의 클럭소스 및 분주기를 설정하는 과정을 말합니다.
    -아래 코드는 포트D의 클럭게이트를 활성화하는 역할을 합니다. 이렇게 하면 해당포트의 기능을 사용합니다.
    -클럭설정은 마이크로컨트롤러의 동작 및 정확성에 중요한 영향을 미치므로 주의깊게 설정해야 합니다.
    */
    PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;

    /*
    <Direction Setting>
    -마이크로컨트롤러의 핀은 입력 또는 출력으로 동작할 수 있습니다.
    -디렉션세팅은 각 핀의 동작방향을 설정한다는 과정을 위와 같이 거쳐야 합니다.
    -아래 첫번째 코드는 PTD1핀을 출력으로 설정하는 것을 나타냅니다. 즉, 이 핀은 데이터를 출력하는 역할을 하게 됩니다.
    -입력방향으로 설정된 핀은 외부에서 데이터를 읽어들이고, 출력방향으로 설정된 핀은 외부로 데이터를 출력하게 됩니다.
    */
    // SWITCH
    PTD->PDDR |= 1 << PTD1;
    PTD->PDDR |= 1 << PTD2;
    PTD->PDDR |= 1 << PTD3;
    PTD->PDDR |= 1 << PTD4;
    PTD->PDDR |= 1 << PTD5;

    // LED
    PTD->PDDR &= ~((unsigned int)1 << PTD10);
    PTD->PDDR &= ~((unsigned int)1 << PTD11);
    PTD->PDDR &= ~((unsigned int)1 << PTD12);
    PTD->PDDR &= ~((unsigned int)1 << PTD13);
    PTD->PDDR &= ~((unsigned int)1 << PTD14);

    /*
    <GPIO Setting>
     -"GPIO"는 General Purpose Input/Output의 약어로, 일반적인 입출력포트를 나타냅니다.
     -GPIO세팅은 특정핀을 범용입출력으로 컴포즈하는 과정으로, 이렇게 설정된 핀으로
     입출력 configuration을 처리할 수 있습니다.
     -위 첫번째 코드는 PTD1 핀을 GPIO 핀으로 설정되는 것을 나타냅니다.
     -GPIO 핀 설정은 주로 디지털 신호를 다룰 때 사용되며, 특정 입출력 장치와 상호작용할 때 필요합니다.
    */
}

int main()
{
    PORT_init();

    while (1)
    {
        /*
        포트D 10번핀의 입력상태를 확인하고, 입력되지 않은 것을 조건으로 하는 비트연산결과가
        거짓인 경우(sw가 눌린 경우 [condition != 1]) PTD1에 대해 PCOR로 세팅합니다.
        참인 경우(sw가 눌리지 안흔 경우[condition = 1]) PTD1에 대해 PSOR로 세팅합니다.
        */
        if (!(PTD->PDIR & (1 << PTD10)))
            PTD->PCOR |= 1 << PTD1;
        else
            PTD->PSOR |= 1 << PTD1;

        if (!(PTD->PDIR & (1 << PTD11)))
            PTD->PCOR |= 1 << PTD2;
        else
            PTD->PSOR |= 1 << PTD2;

        if (!(PTD->PDIR & (1 << PTD12)))
            PTD->PCOR |= 1 << PTD3;
        else
            PTD->PSOR |= 1 << PTD3;

        if (!(PTD->PDIR & (1 << PTD13)))
            PTD->PCOR |= 1 << PTD4;
        else
            PTD->PSOR |= 1 << PTD4;

        if (!(PTD->PDIR & (1 << PTD14)))
            PTD->PCOR |= 1 << PTD5;
        else
            PTD->PSOR |= 1 << PTD5;
    }
}