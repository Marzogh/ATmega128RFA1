/* Arduino RFA1 Library v.0.0.1
 * Copyright (C) 2015 by Prajwal Bhattaram
 * Modified by Prajwal Bhattaram - 26/05/2016
 *
 * This file is part of the Arduino RFA1 Library. This library is for
 * the ATMega128RFA1 microcontroller. In its current form it enables basic
 * Tx/Rx functions of the low-power 2.4 GHz transciever built into the
 * ATMega128RFA1.
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License v3.0
 * along with the Arduino RFA1 Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
 
#include "RFA1.h"
#include <stdbool.h>
 #include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "RFA1.h"
#include "radioDefines.h"

uint8_t rawRSSI
const int RF_BUFFER_SIZE = 127

struct ringBuffer {
	unsigned char buffer[RF_BUFFER_SIZE];
	volatile uint16_t head;
	volatile uint16_t tail;
} RFRXBuffer;

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

// Initialize the RFA1's low-power 2.4GHz transciever.
// Sets up the state machine, and gets the radio into
// the RX_ON state. Interrupts are enabled for RX
// begin and end, as well as TX end.
uint8_t begin(uint8_t channel) {

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

template <class T> bool tx(const T& value) {
	const byte* p = (const byte*)(const void*)&value;
	int len = sizeof(value);

	if (len < 128) {
		uint8_t frame[len];

		for (uint16_t i = 0; i < len; i++) {
			frame[i] = *p++
		}
		 //The Transceiver State Control Register -- TRX_STATE controls the states of the radio.
		 //Setting the PLL state to PLL_ON begins the TX.
		 TRX_STATE &= CMD_TRX_CLEAR;
		 TRX_STATE |= CMD_PLL_ON;  // Set to TX start state

		 while(!(TRX_STATUS & STAT_PLL_ON)) {
		 	// Wait for PLL to lock
		 }

		 //digitalWrite(TX_LED, HIGH);

		 // The start of frame buffer - TRXFBST is the first byte of the 128 byte frame. It should contain the length of the transmission.
		 TRXFBST = length + 2;
		 memcpy((void *)(&TRXFBST+1), frame, len); //If this doesn't work change frame to &frame
		 
		 TRX_STATE |= CMD_TX_START;
		 //Setting SLPTR low will end the TX.
		 TRXPR &= ~(1<<SLPTR);

		 // After the byte is sent the radio is set back into the RX waiting state.
		 TRX_STATE &= CMD_TRX_CLEAR;
		 TRX_STATE |= CMD_RX_ON;
		}

		else {
			throwError();
		}
}

// Returns how many unread bytes remain in the radio RX buffer.
// 0 means the buffer is empty. Maxes out at RF_BUFFER_SIZE.
unsigned int rfAvailable()
{
  return (unsigned int)(RF_BUFFER_SIZE + radioRXBuffer.head - radioRXBuffer.tail) % RF_BUFFER_SIZE;
}

// This function reads the oldest data in the radio RX buffer.
// If the buffer is emtpy, it'll return a 255.
char rfRead()
{
  if (radioRXBuffer.head == radioRXBuffer.tail)
  {
    return -1;
  }
  else
  {
    // Read from the buffer tail, and update the tail pointer.
    char c = radioRXBuffer.buffer[radioRXBuffer.tail];
    radioRXBuffer.tail = (unsigned int)(radioRXBuffer.tail + 1) % RF_BUFFER_SIZE;
    return c;
  }
}

// This interrupt is called when radio TX is complete. We'll just
// use it to turn off our TX LED.
ISR(TRX24_TX_END_vect)
{
  digitalWrite(TX_LED, LOW);
}

// This interrupt is called the moment data is received by the radio.
// We'll use it to gather information about RSSI -- signal strength --
// and we'll turn on the RX LED.
ISR(TRX24_RX_START_vect)
{
  digitalWrite(RX_LED, HIGH);  // Turn receive LED on
  rssiRaw = PHY_RSSI;  // Read in the received signal strength
}

// This interrupt is called at the end of data receipt. Here we'll gather
// up the data received. And store it into a global variable. We'll
// also turn off the RX LED.
ISR(TRX24_RX_END_vect)
{
  uint8_t length;
  // Maximum transmission is 128 bytes
  uint8_t tempFrame[RF_BUFFER_SIZE];

  // The received signal must be above a certain threshold.
  if (rssiRaw & RX_CRC_VALID)
  {
    // The length of the message will be the first byte received.
    length = TST_RX_LENGTH;
    // The remaining bytes will be our received data.
    memcpy(&tempFrame[0], (void*)&TRXFBST, length);
    
    // Now we need to collect the frame into our receive buffer.
    //  k will be used to make sure we don't go above the length
    //  i will make sure we don't overflow our buffer.
    unsigned int k = 0;
    unsigned int i = (radioRXBuffer.head + 1) % RF_BUFFER_SIZE; // Read buffer head pos and increment;
    while ((i != radioRXBuffer.tail) && (k < length-2))
    {
      // First, we update the buffer with the first byte in the frame
      radioRXBuffer.buffer[radioRXBuffer.head] = tempFrame[k++];
      radioRXBuffer.head = i; // Update the head
      i = (i + 1) % RF_BUFFER_SIZE; // Increment i % BUFFER_SIZE
    }
  }

  digitalWrite(RX_LED, LOW);  // Turn receive LED off, and we're out
}

void throwError() {

}