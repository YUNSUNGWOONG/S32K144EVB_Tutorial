/** PORT - Size of Registers Arrays */
#define PORT_PCR_COUNT 32u

/** PORT - Register Layout Typedef */
typedef struct
{
    __IO uint32_t PCR[PORT_PCR_COUNT]; /**< Pin Control Register n, array offset:
                                          0x0, array step: 0x4*/
    __O uint32_t GPCLR;                /**< Global Pin Control Low Register, offset: 0x80 */
    __O uint32_t GPCHR;                /**< Global Pin Control High Register, offset: 0x84 */
    __O uint32_t GICLR;                /**< Global Interrupt Control Low Register, offset: 0x88 */
    __O uint32_t GICHR;                /**< Global Interrupt Control High Register, offset: 0x8C */
    uint8_t RESERVED_0[16];

    __IO uint32_t ISFR; /**< Interrupt Status Flag Register, offset: 0xA0 */

    uint8_t RESERVED_1[28];

    __IO uint32_t DFER; /**< Digital Filter Enable Register, offset: 0xA0 */

    __IO uint32_t DFCR; /**< Digital Filter Clock Register, offset: 0xC4 */

    __IO uint32_t DFWR; /**< Digital Filter Width Register, offset: 0xC8 */

} PORT_Type, *PORT_MemMapPtr;

/** Number of instances of the PORT module. */
#define PORT_INSTANCE_COUNT (5u)

/* PORT - Peripheral instance base addresses */
/** Peripheral PORTA base address */
#define PORTA_BASE (0x40049000u)
/** Peripheral PORTA base register */
#define PORTA ((PORT_Type *)PORTA_BASE)
/** Peripheral PORTB base address */
#define PORTB_BASE (0x4004A000u)
/** Peripheral PORTB base pointer */
#define PORTB ((PORT_Type *)PORTB_BASE)
/** Peripheral PORTC base address */
#define PORTC_BASE (0x4004B000u)
/** Peripheral PORTC base pointer */
#define PORTC ((PORT_Type *)PORTC_BASE)
/** Peripheral PORTD base address */
#define PORTD_BASE (0x4004C000u)
/** Peripheral PORTD base pointer */
#define PORTD ((PORT_Type *)PORTD_BASE)

/** PCC - Size of Registers Arrays */
#define PCC_PCCn_COUNT 116u

/** PCC - Register Layout Typedef */
typedef struct
{
    __IO uint32_t PCCn[PCC_PCCn_COUNT];
} PCC_Type, *PCC_MemMapPtr;

/** Number of instances of the PCC module. */
#define PCC_INSTANCE_COUNT (1u)

/* PCC - Peripheral instance base addresses */
/** Peripheral PCC base address */
#define PCC_BASE (0x40065000u)
/** Peripheral PCC base pointer */
#define PCC ((PCC_Type *)PCC_BASE)
/** Array initializer of PCC peripheral base addresses */
#define PCC_BASE_ADDRS                                                                                                 \
    {                                                                                                                  \
        PCC_BASE                                                                                                       \
    }
/** Array initializer of PCC peripheral base pointers */
#define PCC_BASE_PTRS                                                                                                  \
    {                                                                                                                  \
        PCC                                                                                                            \
    }
