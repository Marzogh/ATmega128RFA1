/* Arduino RFA1 Library v.2.2.0
 * Copyright (C) 2015 by Prajwal Bhattaram
 * Modified by Prajwal Bhattaram - 24/11/2015
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
 
#ifndef RFA1_H
#define RFA1_H

//#include <Arduino.h> //perhaps not needed?

class RFA1 {
public:
  //----------------------------------------------Constructor-----------------------------------------------//
  Radio(uint8_t cs = 10, bool overflow = true);
  //----------------------------------------Initial / Chip Functions----------------------------------------//
  void     begin();
  //------------------------------------------Transmit / Receive Anything-----------------------------------------//
  template <class T> bool transmit(const T& value);
  /*//-------------------------------------------Write / Read Bytes-------------------------------------------//
  bool     writeByte(uint32_t address, uint8_t data, bool errorCheck = true);
  bool     writeByte(uint16_t page_number, uint8_t offset, uint8_t data, bool errorCheck = true);
  uint8_t  readByte(uint16_t page_number, uint8_t offset, bool fastRead = false);
  uint8_t  readByte(uint32_t address, bool fastRead = false);
  //----------------------------------------Write / Read Byte Arrays----------------------------------------//
  bool     writeByteArray(uint32_t address, uint8_t *data_buffer, uint16_t bufferSize, bool errorCheck = true);
  bool     writeByteArray(uint16_t page_number, uint8_t offset, uint8_t *data_buffer, uint16_t bufferSize, bool errorCheck = true);
  uint8_t  readByteArray(uint32_t address, uint8_t *data_buffer, uint16_t bufferSize, bool fastRead = false);
  uint8_t  readByteArray(uint16_t page_number, uint8_t offset, uint8_t *data_buffer, uint16_t bufferSize, bool fastRead = false);
  //-------------------------------------------Write / Read Chars-------------------------------------------//
  bool     writeChar(uint32_t address, int8_t data, bool errorCheck = true);
  bool     writeChar(uint16_t page_number, uint8_t offset, int8_t data, bool errorCheck = true);
  int8_t   readChar(uint32_t address, bool fastRead = false);
  int8_t   readChar(uint16_t page_number, uint8_t offset, bool fastRead = false);
  //----------------------------------------Write / Read Char Arrays----------------------------------------//
  bool     writeCharArray(uint32_t address, char *data_buffer, uint16_t bufferSize, bool errorCheck = true); 
  bool     writeCharArray(uint16_t page_number, uint8_t offset, char *data_buffer, uint16_t bufferSize, bool errorCheck = true); 
  uint8_t  readCharArray(uint32_t address, char *data_buffer, uint16_t buffer_size, bool fastRead = false);
  uint8_t  readCharArray(uint16_t page_number, uint8_t offset, char *data_buffer, uint16_t buffer_size, bool fastRead = false);
  //------------------------------------------Write / Read Shorts------------------------------------------//
  bool     writeShort(uint32_t address, int16_t data, bool errorCheck = true);
  bool     writeShort(uint16_t page_number, uint8_t offset, int16_t data, bool errorCheck = true);
  int16_t  readShort(uint32_t address, bool fastRead = false);
  int16_t  readShort(uint16_t page_number, uint8_t offset, bool fastRead = false);
  //-------------------------------------------Write / Read Words-------------------------------------------//
  bool     writeWord(uint32_t address, uint16_t data, bool errorCheck = true);
  bool     writeWord(uint16_t page_number, uint8_t offset, uint16_t data, bool errorCheck = true);
  uint16_t readWord(uint32_t address, bool fastRead = false);
  uint16_t readWord(uint16_t page_number, uint8_t offset, bool fastRead = false);
  //-------------------------------------------Write / Read Longs-------------------------------------------//
  bool     writeLong(uint32_t address, int32_t data, bool errorCheck = true);
  bool     writeLong(uint16_t page_number, uint8_t offset, int32_t data, bool errorCheck = true);
  int32_t  readLong(uint32_t address, bool fastRead = false);
  int32_t  readLong(uint16_t page_number, uint8_t offset, bool fastRead = false);
  //--------------------------------------Write / Read Unsigned Longs---------------------------------------//
  bool     writeULong(uint32_t address, uint32_t data, bool errorCheck = true);
  bool     writeULong(uint16_t page_number, uint8_t offset, uint32_t data, bool errorCheck = true);
  uint32_t readULong(uint32_t address, bool fastRead = false);
  uint32_t readULong(uint16_t page_number, uint8_t offset, bool fastRead = false);
  //-------------------------------------------Write / Read Floats------------------------------------------//
  bool     writeFloat(uint32_t address, float data, bool errorCheck = true);
  bool     writeFloat(uint16_t page_number, uint8_t offset, float data, bool errorCheck = true);
  float    readFloat(uint32_t address, bool fastRead = false);
  float    readFloat(uint16_t page_number, uint8_t offset, bool fastRead = false);
  //------------------------------------------Write / Read Strings------------------------------------------//
  bool     writeStr(uint32_t address, String &inputStr, bool errorCheck = true);
  bool     writeStr(uint16_t page_number, uint8_t offset, String &inputStr, bool errorCheck = true);
  bool     readStr(uint32_t address, String &outStr, bool fastRead = false);
  bool     readStr(uint16_t page_number, uint8_t offset, String &outStr, bool fastRead = false);
  //-------------------------------------------Write / Read Pages-------------------------------------------//
  bool     writePage(uint16_t page_number, uint8_t *data_buffer, bool errorCheck = true);
  bool     readPage(uint16_t page_number, uint8_t *data_buffer, bool fastRead = false);
  //--------------------------------------------Erase functions---------------------------------------------//
  bool     eraseSector(uint32_t address);
  bool     eraseSector(uint16_t page_number, uint8_t offset);
  bool     eraseBlock32K(uint32_t address);
  bool     eraseBlock32K(uint16_t page_number, uint8_t offset);
  bool     eraseBlock64K(uint32_t address);
  bool     eraseBlock64K(uint16_t page_number, uint8_t offset);
  bool     eraseChip(void);
  //---------------------------------------------Power functions--------------------------------------------//
  bool     suspendProg(void);
  bool     resumeProg(void);
  bool     powerDown(void);
  bool     powerUp(void); */
  //--------------------------------------------Private functions-------------------------------------------//
private:
  void     _troubleshoot(uint8_t error);
  void     _cmd(uint8_t cmd, bool _continue = true, uint8_t cs = 0);
  void     _endProcess(void);
  void     _errorCodeCheck(void);
  void     _beginRead(uint32_t address);
  void     _beginFastRead(uint32_t address);
  bool     _noSuspend(void);
  bool     _notBusy(uint32_t timeout = 10L);
  bool     _notPrevWritten(uint32_t address, uint8_t size = 1);
  bool     _addressCheck(uint32_t address);
  bool     _beginWrite(uint32_t address);
  bool     _writeNextByte(uint8_t c, bool _continue = true);
  bool     _writeEnable(void);
  bool     _writeDisable(void);
  bool     _getJedecId(uint8_t *b1, uint8_t *b2, uint8_t *b3);
  bool     _getManId(uint8_t *b1, uint8_t *b2);
  bool     _chipID(void);
  uint8_t  _readStat1(void);
  uint8_t  _readNextByte(bool _continue = true);
  uint32_t _getAddress(uint16_t page_number, uint8_t offset = 0);
  uint32_t _prepRead(uint32_t address);
  uint32_t _prepRead(uint16_t page_number, uint8_t offset = 0);
  uint32_t _prepWrite(uint32_t address);
  uint32_t _prepWrite(uint16_t page_number, uint8_t offset = 0);
  template <class T> bool _writeErrorCheck(uint32_t address, const T& value);
  //-------------------------------------------Private variables------------------------------------------//
  bool        pageOverflow;
  volatile uint8_t *cs_port;
  uint8_t     cs_mask, csPin, errorcode;
  uint16_t    name;
  uint32_t    capacity, maxPage;
  uint32_t    currentAddress = 1;
  const uint8_t devType[10]   = {0x5, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  const uint32_t memSize[10]  = {64L * 1024L, 128L * 1024L, 256L * 1024L, 512L * 1024L, 1L * 1024L * 1024L,
                                2L * 1024L * 1024L, 4L * 1024L * 1024L, 8L * 1024L * 1024L, 16L * 1024L * 1024L};
  const uint16_t chipName[10] = {05, 10, 20, 40, 80, 16, 32, 64, 128, 256};
};