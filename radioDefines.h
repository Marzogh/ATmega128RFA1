//Defines for ATmega128RFA1 TRX registers

//GENERAL DEFINES
#define DEFAULT_CHANNEL						0X0B		//Channels 11 (0x0B) through to 26 (0x1A) are supported

//TRXPR MASK DEFINES
#define TRX_SLEEP							0x01
#define TRX_RESET							0x02

//TRX_STATUS MASK DEFINES
#define STAT_BUSY_RX						0x01
#define STAT_BUSY_TX						0x02
#define STAT_RX_ON							0X06
#define STAT_TRX_OFF						0X08
#define STAT_PLL_ON							0X09
#define STAT_SLEEP							0x0F
#define STAT_BUSY_RX_AACK					0X11
#define STAT_BUSY_TX_ARET					0X12
#define STAT_RX_AACK_ON						0X16
#define STAT_TX_ARET_ON						0X19
#define STAT_STATE_TRANSITION_IN_PROGRESS	0X1F
#define STAT_CCA_STATUS_IDLE				0X40
#define STAT_CCA_DONE						0x80

//TRX_STATE REGISTER MASK DEFINES
//TRAC_STATUS BITS MASK DEFINES
#define STAT_TRX_SUCCESS					0x00
#define STAT_SUCCESS_DATA_PENDING			0x20
#define STAT_SUCCESS_WAIT_FOR_ACK			0x40
#define STAT_CHANNEL_ACCESS_FAILURE			0x60
#define STAT_NO_ACK							0xA0
#define STAT_INVALID						0xE0
//TRX_CMD BITS MASK DEFINES
#define CMD_TRX_CLEAR						0xE0
#define CMD_NOP								0x00
#define CMD_TX_START						0x02
#define CMD_FORCE_TRX_OFF					0x03
#define CMD_FORCE_PLL_ON					0x04
#define CMD_RX_ON							0x06
#define CMD_TRX_OFF							0x08
#define CMD_PLL_ON							0x09
#define CMD_TX_ON							0x09
#define CMD_RX_AACK_ON						0x16
#define CMD_TX_ARET_ON						0x19

//IRQ_MASK REGISTER (ENABLE) MASK DEFINES
#define INT_TRX								0XFF
#define INT_AWAKE							0x80
#define INT_TX_END							0x40
#define INT_AMI								0x20
#define INT_CCA_ED_DONE						0x10
#define INT_RX_END							0x08
#define INT_RX_START						0x04
#define INT_PLL_UNLOCK						0x02
#define INT_PLL_LOCK						0x01

//TRX_CTRL_1 REGISTER MASK DEFINES
#define CTRL_PA_EXT_EN						0x80
#define CTRL_IRQ_2_EXT_EN					0x40 		//This bit is not to be enabled (set to 1) if Antenna diversity os being used as they share a pin
#define CTRL_TX_AUTO_CRC_ON					0x20

//PHY_CC_CCA REGISTER MASK DEFINES
#define PHY_CCA_REQUEST						0x80
#define PHY_CCA_MODE1						0X40
#define PHY_CCA_MODE0						0x20
#define PHY_RESET_CHANNELS					0xE0

//Macros

//Interrupt related macros
#define INT_TRX_ENABLE				IRQ_MASK |= INT_TRX
#define INT_TRX_DISABLE				IRQ_MASK &= ~(INT_TRX)
#define INT_AWAKE_ENABLE			IRQ_MASK |= INT_AWAKE
#define INT_AWAKE_DISABLE			IRQ_MASK &= ~(INT_AWAKE)
#define INT_TX_END_ENABLE			IRQ_MASK |= INT_TX_END
#define INT_TX_END_DISABLE			IRQ_MASK &= ~(INT_TX_END)
#define INT_AMI_ENABLE				IRQ_MASK |= INT_AMI
#define INT_AMI_DISABLE				IRQ_MASK &= ~(INT_AMI)
#define INT_CCA_ED_DONE_ENABLE		IRQ_MASK |= INT_CCA_ED_DONE
#define INT_CCA_ED_DONE_DISABLE		IRQ_MASK & ~(INT_CCA_ED_DONE)
#define INT_RX_END_ENABLE			IRQ_MASK |= INT_RX_END
#define INT_RX_END_DISABLE			IRQ_MASK &= ~(INT_RX_END)
#define INT_RX_START_ENABLE			IRQ_MASK |= INT_RX_START
#define INT_RX_START_DISABLE		IRQ_MASK &= ~(INT_RX_START)
#define INT_PLL_UNLOCK_ENABLE		IRQ_MASK |= INT_PLL_UNLOCK
#define INT_PLL_UNLOCK_DISABLE		IRQ_MASK &= ~(INT_PLL_UNLOCK)
#define INT_PLL_LOCK_ENABLE			IRQ_MASK |= INT_PLL_LOCK
#define INT_PLL_LOCK_DISABLE		IRQ_MASK &= ~(INT_PLL_LOCK)