#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "radio.h"
#include "radioDefines.h"


//Global variables
uint8_t rawRSSI;
const uint16_t RF_BUFFER_SIZE = 127;

struct ringBuffer {
	unsigned char buffer[RF_BUFFER_SIZE];
	volatile uint16_t head;
	volatile uint16_t tail;
} radioRXBuffer;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//Checks to see if TRX is busy
bool _trxBusy() {
	if (TRX_STATUS & 0x1F)
		return true;
	else
		return false;
}

//Resets TRX
void _trxReset() {
	TRXPR |= TRX_RESET;
}

//Wakes TRX (sets mode to TRX_OFF)
bool _trxWake() {
	TRX_STATE &= CMD_TRX_CLEAR;
	TRX_STATE |= CMD_TRX_OFF; //Wakes the transciever (i.e. in mode TRX_OFF)
	_delay_ms(1);
	
	if (TRX_STATUS & STAT_TRX_OFF) //Checks to see that the transciever is awake
		return true
	else
		return false
}

//Clears ring buffer by writing all variables to 0
void _clearRingBuffer() {
	for (int i=0; i<128; i++)
	{
		radioRXBuffer.buffer[i] = 0;
	}
	radioRXBuffer.tail = 0;
	radioRXBuffer.head = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/* Initializes the RFA1's low-power 2.4GHz transciever and sets up the state machine. This function puts the radio
into the RX_ON state with interrupts enabled for RX_START, RX_END &vTX_END*/
bool radioBegin(uint8_t channel) {

	//If the transciever is currently in STATE_TRANSITION_IN_PROGRESS do nothing
	if (_trxBusy()) {
		return false;
	}

	_clearRingBuffer();//Clear the ringBuffer

	_trxReset();							//Causes the transciever to reset
	INT_TRX_DISABLE;						//Disable all TRX interrupts by setting IRQ_MASK to 0x00

	if (!_trxWake())
		return false;						//Check to se if the current TRX state is TRX_OFF. If not, return false

	TRX_CTRL_1 |= CTRL_TX_AUTO_CRC_ON;

	//Enables the RX_START, RX_END and TX_END interrupts
	INT_RX_START_ENABLE;
	INT_RX_END_ENABLE;
	INT_TX_END_ENABLE;

	if ((channel < 11) || (channel > 26)){
		channel = DEFAULT_CHANNEL;
	}										//If the channel declared is out of bounds, sets the channel to default - 11 (0x0B)

	PHY_CC_CCA &= PHY_RESET_CHANNELS;
	PHY_CC_CCA |= channel;

	TRX_STATE &= CMD_TRX_CLEAR;
	TRX_STATE|=CMD_RX_ON;					//Set the TRX state to Receiving (RX_ON)

	return true;
}

/* This function puts the radio into the PLL_ON state  followed by TX_ON after which it transmits the byte of data */
void radioTxByte(uint8_t data)
{
  uint8_t length = 3;

  // Transceiver State Control Register -- TRX_STATE
  // This regiseter controls the states of the radio.
  // Set to the PLL_ON state - this state begins the TX.
  TRX_STATE &= CMD_TRX_CLEAR;
  TRX_STATE |= CMD_PLL_ON;  // Set to TX start state

  while(!(TRX_STATUS & STAT_PLL_ON)) {
  	// Wait for PLL to lock
  }

  // Start of frame buffer - TRXFBST
  // This is the first byte of the 128 byte frame. It should contain
  // the length of the transmission.
  TRXFBST = length;
  // Now copy the byte-to-send into the address directly after TRXFBST.
  memcpy((void *)(&TRXFBST+1), &data, 1);

  // Transceiver Pin Register -- TRXPR.
  // From the PLL_ON state, setting SLPTR high will initiate the TX.
  TRX_STATE |= CMD_TX_START;

  // After sending the byte, set the radio back into the RX waiting state.
  TRX_STATE &= CMD_TRX_CLEAR);
  TRX_STATE |= CMD_RX_ON;
}