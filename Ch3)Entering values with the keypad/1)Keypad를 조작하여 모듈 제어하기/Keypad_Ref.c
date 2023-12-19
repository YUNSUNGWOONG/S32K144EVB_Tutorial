
/**
 * 아래와 같이 포트E(키패드 핀) GPIO 설정을 0, 1, 2, 3(입력) // 12,14,15(출력)로 설정
 * - E4 핀이 항상 켜지는 문제로 사용하지 않음 - E13 핀은 GPIO로 설정해도 작동이 되지않아 E12,E14, E15를 출력으로 설정함
 */

PTE->PDDR |= 1 << 12 | 1 << 14 | 1 << 15; /* Port E12,E14-E15:  Data Direction = output */
PTE->PDDR &= ~(1 << 0);                   /* Port E0: Data Direction= input (default) */
PTE->PDDR &= ~(1 << 1);                   /* Port E1: Data Direction= input (default) */
PTE->PDDR &= ~(1 << 2);                   /* Port E2: Data Direction= input (default) */
PTE->PDDR &= ~(1 << 3);                   /* Port E3: Data Direction= input (default) */

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
