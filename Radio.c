#include <avr/io.h>
#include <util/delay.h>
#include "radio.h"

//Defines

//GENERAL DEFINES
#define FALSE								0x00
#define TRUE								0X01

//TRXPR RELATED DEFINES
#define TRX_SLEEP							0x01
#define TRX_RESET							0x02

//TRX_STATUS RELATED DEFINES
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

//TRX_STATE RELATED DEFINES
//TRAC_STATUS RELATED DEFINES
#define STAT_TRX_SUCCESS					0x00
#define STAT_SUCCESS_DATA_PENDING			0x20
#define STAT_SUCCESS_WAIT_FOR_ACK			0x40
#define STAT_CHANNEL_ACCESS_FAILURE		0x60
#define STAT_NO_ACK							0xA0
#define STAT_INVALID						0xE0
//TRX_CMD RELATED DEFINES
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

//INTERRUPT ENABLE RELATED DEFINES
#define TRX_INT								0XFF
#define AWAKE_INT							0x80
#define TX_END_INT							0x40
#define AMI_INT								0x20
#define CCA_ED_DONE_INT						0x10
#define RX_END_INT							0x08
#define RX_START_INT						0x04
#define PLL_UNLOCK_INT						0x02
#define PLL_LOCK_INT						0x01

//Macros

//Interrupt related Macros
#define TRX_INT_EN				IRQ_MASK |= TRX_INT
#define TRX_INT_DIS				IRQ_MASK &= ~(TRX_INT)
#define AWAKE_INT_EN			IRQ_MASK |= AWAKE_INT
#define AWAKE_INT_DIS			IRQ_MASK &= ~(AWAKE_INT)
#define TX_END_INT_EN			IRQ_MASK |= TX_END_INT
#define TX_END_INT_DIS			IRQ_MASK &= ~(TX_END_INT)
#define AMI_INT_EN				IRQ_MASK |= AMI_INT
#define AMI_INT_DIS				IRQ_MASK &= ~(AMI_INT)
#define CCA_ED_DONE_INT	_EN		IRQ_MASK |= CCA_ED_DONE_INT
#define CCA_ED_DONE_INT_DIS		IRQ_MASK & ~(CCA_ED_DONE_INT)
#define RX_END_INT_EN			IRQ_MASK |= RX_END_INT
#define RX_END_INT_DIS			IRQ_MASK &= ~(RX_END_INT)
#define RX_START_INT_EN			IRQ_MASK |= RX_START_INT
#define RX_START_INT_DIS		IRQ_MASK &= ~(RX_START_INT)
#define PLL_UNLOCK_INT_EN		IRQ_MASK |= PLL_UNLOCK_INT
#define PLL_UNLOCK_INT_DIS		IRQ_MASK &= ~(PLL_UNLOCK_INT)
#define PLL_LOCK_INT_EN			IRQ_MASK |= PLL_LOCK_INT
#define PLL_LOCK_INT_DIS		IRQ_MASK &= ~(PLL_LOCK_INT)


//Global variables
uint8_t rawRSSI
const int RF_BUFFER_SIZE = 127

struct ringBuffer {
	unsigned char buffer[RF_BUFFER_SIZE];
	volatile uint16_t head;
	volatile uint16_t tail;
} radioRXBuffer;

//Private Functions

uint8_t _trxBusy() {
	if (TRX_STATUS & 0x1F)
		return TRUE;
	else
		return FALSE;
}

void _trxReset() {
	TRXPR |= TRX_RESET;
}

uint8_t _trxWake() {
	TRX_STATE &= CMD_TRX_CLEAR;
	TRX_STATE |= CMD_TRX_OFF; //Wakes the transciever (i.e. in mode TRX_OFF)
	_delay_ms(1);
	
	if (TRX_STATUS & STAT_TRX_OFF) //Checks to see that the transciever is awake
		return TRUE
	else
		return FALSE
}

//Public Functions
uint8_t radioBegin(uint8_t channel) {

	//If the transciever is currently in STATE_TRANSITION_IN_PROGRESS do nothing
	if (_trxBusy())
		return FALSE;

	//Clear the ringBuffer
	for (int i=0; i<128; i++)
	{
		radioRXBuffer.buffer[i] = 0;
	}
	radioRXBuffer.tail = 0;
	radioRXBuffer.head = 0;

	_trxReset();						//Causes the transciever to reset
	TRX_INT_DIS;						//Disable all TRX interrupts by setting IRQ_MASK to 0x00

	if (!_trxWake())
		return FALSE;

	TRX_CTRL_1 |= (1<<TX_AUTO_CRC_ON);

	IRQ_MASK = (1<<RX_START_EN) | (1<<RX_END_EN) | (1<<TX_END_EN);

	if ((channel < 11) || (channel > 26))
	channel = 11;

	PHY_CC_CCA = (PHY_CC_CCA & 0xE0) | 11;

	TRX_STATE = (TRX_STATE & 0xE0) | RX_ON;

	return TRUE;
}