/* RadioFunctions.h
  A handful of sending and receiving functions for the 
  ATmega128RFA1.
  by: Jim Lindblom
      SparkFun Electronics
  date: July 8, 2013
  license: Beerware. Use, distribut, and modify this code freely
  however you please. If you find it useful, you can buy me a beer
  some day.
  
  Functions in here:
    rfBegin(uint8_t channel) - Initializes the radio on a channel
                                between 11 and 26.
    rfWrite(uint8_t b) - Sends a byte over the radio.
    rfPrint(String toPrint) - Sends a string over the radio.
    int rfAvailable() - Returns number of characters currently
                        in receive buffer, ready to be read.
    char rfRead() - Reads a character out of the buffer.
    
  Interrupt Sub-Routines (ISRs) in here:
    TRX24_TX_END_vect - End of radio transfer.
    TRX24_RX_START_vect - Beginning of radio receive.
    TRX24_RX_END_vect - End of radio receive. Characters are 
                        collected into a buffer here.
*/
// This function sends a string of characters out of the radio.
// Given a string, it'll format a frame, and send it out.
void rfPrint(String toPrint)
{
  uint8_t frame[127];  // We'll need to turn the string into an arry
  int length = toPrint.length();  // Get the length of the string
  for (int i=0; i<length; i++)  // Fill our array with bytes in the string
  {
    frame[i] = toPrint.charAt(i);
  }
  
  // Transceiver State Control Register -- TRX_STATE
  // This regiseter controls the states of the radio.
  // Set to the PLL_ON state - this state begins the TX.
  TRX_STATE = (TRX_STATE & 0xE0) | PLL_ON;  // Set to TX start state
  while(!(TRX_STATUS & PLL_ON))
    ;  // Wait for PLL to lock
    
  digitalWrite(TX_LED, HIGH);
  
  // Start of frame buffer - TRXFBST
  // This is the first byte of the 128 byte frame. It should contain
  // the length of the transmission.
  TRXFBST = length + 2;
  memcpy((void *)(&TRXFBST+1), frame, length);
  // Transceiver Pin Register -- TRXPR.
  // From the PLL_ON state, setting SLPTR high will initiate the TX.
  TRXPR |= (1<<SLPTR);   // SLPTR high
  TRXPR &= ~(1<<SLPTR);  // SLPTR low
  
  // After sending the byte, set the radio back into the RX waiting state.
  TRX_STATE = (TRX_STATE & 0xE0) | RX_ON;
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
