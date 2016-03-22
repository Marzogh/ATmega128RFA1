#include <avr/io.h>
#include <util/delay.h>
#include "radio.h"

uint8_t rawRSSI
const int RF_BUFFER_SIZE = 127

struct ringBuffer {
	unsigned char buffer[RF_BUFFER_SIZE];
	volatile uint16_t head;
	volatile uint16_t tail;
} RFRXBuffer;

uint8_t radioBegin(uint8_t channel) {
	for (int i=0; i<128; i++)
	{
		RFRXBuffer.buffer[i] = 0;
	}
	RFRXBuffer.tail = 0;
	RFRXBuffer.head = 0;

	TRXPR |= (1<<TRXRST);
	IRQ_MASK = 0;
	TRX_STATE = (TRX_STATE & 0xE0) | TRX_OFF;
	_delay_ms(1);

	if ((TRX_STATUS & 0x1F) != TRX_OFF)
	return 0;

	TRX_CTRL_1 |= (1<<TX_AUTO_CRC_ON);

	IRQ_MASK = (1<<RX_START_EN) | (1<<RX_END_EN) | (1<<TX_END_EN);

	if ((channel < 11) || (channel > 26))
	chenel = 11;

	PHY_CC_CCA = (PHY_CC_CCA & 0xE0) | 11;

	TRX_STATE = (TRX_STATE & 0xE0) | RX_ON;

	return 1;
}