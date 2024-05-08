/* rs232_tut_regs.h
 *
 * Ensemble des registres et des masque utile a programmer le 
 * le port seriel.
 */

#ifndef __rs232_tut_regs
#define __rs232_tut_regs

/**
 * Selection du port seriel
 */
#define RS232_DEFAULT_IRQ_NO    3
#define RS232_DEFAULT_BASE_PORT 0x2F8
#define RS232_NB_PORT           7
extern ulong base_port;
extern int irq;

/**
 * THR - Transmitter Holding Register
 *  Registre servant a l'envoie de donnees
 */
#define RS232_THR(base) ((base) + 0x00)

/**
 * RBR - Receive Buffer Register
 *  Registre servant a la reception de donnes
 */
#define RS232_RBR(base) ((base) + 0x00)

/**
 * IER - Interrupt Enable Register
 *  Registre permettant l'activation ou la desactivation des
 *  interruptions sur le port seriel.
 */
#define RS232_IER(base)         ((base) + 0x01)
#define RS232_INT_ON            0x07
#define RS232_INT_OFF           0x00

/**
 * IIR - Interrupt Indentification Register
 * Registre servant a identifier les interruptions
 */
#define RS232_IIR(base)         ((base) + 0x02)
#define RS232_INT_PENDING       0x01
#define RS232_ERROR             0x06
#define RS232_EMPTY_THR         0x02
#define RS232_DATA_AVAIL        0x04
#define RS232_IIR_MASK          0x06

/**
 * FCR - FIFO Control Register
 */
#define RS232_FCR(base)         ((base) + 0x02)
#define RS232_CFG_BUFFER        0x07
#define RS232_CFG_NOBUFFER      0x00

/**
 * LCR - Line control register
 */
#define RS232_LCR(base)         ((base) + 0x03)
#define RS232_CMD_SPEED         0x80
#define RS232_MASK_8BIT         0x03
#define RS232_MASK_NO_PARITY    0x00
#define RS232_MASK_ONE_STOP_BIT 0x00
#define RS232_CFG_LINE          (RS232_MASK_8BIT | RS232_MASK_NO_PARITY | RS232_MASK_ONE_STOP_BIT)

/**
 * MCR - Modem Control Register
 */
#define RS232_MCR(base)         ((base) + 0x04)
#define RS232_HAND_SHAKE        0x0F

/**
 * LSR - Line Statuc Register
 *  Registre permettant de verifier l'etat de la ligne
 *  avant de proceder a un transfert
 */
#define RS232_LSR(base)         ((base) + 0x05)
#define RS232_DATA_AVAILABLE    0x01
#define RS232_READY_TO_SEND     0x10

/**
 * MSR - Modem Status Register
 *  Registre de status du modem
 */
#define RS232_MSR(base) ((base) + 0x06)

/**
 * DLL/DLM - Divisor latch Registers
 *  Registre utilise pour passer de information de deux
 *  octet au port seriel.  DLL etant l'octet de poid faible
 *  et DLM l'octer de poid fort.  (Utilise pour configurer
 *  la vitesse du lien)
 */
#define RS232_DLL(base)         ((base) + 0x00)
#define RS232_DLM(base)         ((base) + 0x01)

// 50 bps
#define RS232_DLL_50             0x00
#define RS232_DLM_50             0x09

// 300 bps
#define RS232_DLL_300            0x80
#define RS232_DLM_300            0x01

// 1200 bps
#define RS232_DLL_1K             0x60
#define RS232_DLM_1K             0x00

// 2400 bps
#define RS232_DLL_2K             0x30
#define RS232_DLM_2K             0x00

// 4800 bps
#define RS232_DLL_4K             0x18
#define RS232_DLM_4K             0x00

// 9600 bps
#define RS232_DLL_9K             0x0C
#define RS232_DLM_9K             0x00

// 19200 bps
#define RS232_DLL_19K            0x06
#define RS232_DLM_19K            0x00

// 38400 bps
#define RS232_DLL_38K            0x03
#define RS232_DLM_38K            0x00

// 57600 bps
#define RS232_DLL_57K            0x02
#define RS232_DLM_57K            0x00

// 115200 bps
#define RS232_DLL_115K           0x01
#define RS232_DLM_115K           0x00


#endif //__rs232_tut_regs
