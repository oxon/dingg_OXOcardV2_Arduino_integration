#ifndef _LIS3DE_H_
#define _LIS3DE_H_
/*******************************************************************************
* \file    LIS3DE.h
********************************************************************************
* \author  Jascha Haldemann jh@oxon.ch
* \date    01.11.2017
* \version 1.0
*
* \brief   The LIS3DE is an ultra low power 3-axis 8-bit accelerometer
*
* \license LGPL-V2.1
* Copyright (c) 2017 OXON AG. All rights reserved.
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, see 'http://www.gnu.org/licenses/'
********************************************************************************
* Accelerometer Library
*******************************************************************************/

/* ============================== Global imports ============================ */
#include <Arduino.h>
#include <Wire.h>

/* ==================== Global module constant declaration ================== */

/* ========================= Global macro declaration ======================= */

/* ============================ Class declaration =========================== */
class LIS3DE
{
public:
  /* Public member typedefs */
  typedef enum : uint8_t
  {
    MODE_NORMAL    = 0,
    MODE_LOW_POWER = 1
  } mode_t;

  typedef enum : uint8_t
  {
    RANGE_2G  = 0b00,   // +/- 2g (default value)
    RANGE_4G  = 0b01,   // +/- 4g
    RANGE_6G  = 0b10,   // +/- 8g
    RANGE_16G = 0b11    // +/- 16g
  } range_t;

  typedef enum : uint8_t
  {
    DATA_RATE_0HZ     = 0x00,   // Power-down mode
    DATA_RATE_1HZ     = 0x01,
    DATA_RATE_10HZ    = 0x02,
    DATA_RATE_25HZ    = 0x03,
    DATA_RATE_50HZ    = 0x04,
    DATA_RATE_100HZ   = 0x05,
    DATA_RATE_200HZ   = 0x06,
    DATA_RATE_400HZ   = 0x07,
    DATA_RATE_1620HZ  = 0x08,   // works only in LOW_POWER mode
    DATA_RATE_1344HZ  = 0x09,   // works only in NORMAL mode
    DATA_RATE_5376HZ  = 0x09    // works only in LOW_POWER mode
  } dataRate_t;

  typedef enum : uint8_t
  {
    UNKNOWN = 0,
    UP = 1,
    DOWN = 2,
    HORIZONTALLY = 3,
    VERTICALLY = 4,
    HALF_UP = 5,
    HALF_DOWN = 6,
    HALF_LEFT = 7,
    HALF_RIGHT = 8
  } orientation_t;

  /* Public member data */
  //...

  /* Constructor(s) and  Destructor*/
  //LIS3DE(WireProxy& wireRef, uint8_t i2cAddress = DEFAULT_I2C_ADDRESS) : wireRef_(wireRef), i2cAddress_(i2cAddress) {};
  LIS3DE(TwoWire& wireRef, uint8_t int1Pin = -1, uint8_t int2Pin = -1,
    uint8_t i2cAddress = DEFAULT_I2C_ADDRESS) :
    wireRef_(wireRef), int1Pin_(int1Pin), int2Pin_(int2Pin),
    i2cAddress_(i2cAddress) {};
  ~LIS3DE() {};

  /* Public member functions */
  bool begin(mode_t mode = MODE_NORMAL, range_t range = RANGE_2G, dataRate_t dr = DATA_RATE_10HZ);
  void end();
  uint8_t getChipID();
  void setMode(LIS3DE::mode_t mode);
  mode_t getMode();
  void setRange(range_t range);
  range_t getRange();
  void setDataRate(dataRate_t dr);
  dataRate_t getDataRate();
  int8_t getVectorX();
  int8_t getVectorY();
  int8_t getVectorZ();
  void getAccelerationVector(int16_t accelVector[3]);
  void getAccelerationVector(float accelVector[3], bool convert = true);
  void mgScale(int16_t accelVector[3]);
  void gScale(float accelVector[3]);
  orientation_t getOrientation();
  void enableMovementDetectionINT1(bool ig1 = true, bool ig2 = false, uint8_t threshold = 16, uint8_t duration = 2);  //TODO: test
  void enableMovementDetectionINT2(bool ig1 = true, bool ig2 = false, uint8_t threshold = 16, uint8_t duration = 2);  //TODO: test
  // Example: accel.enableMovementDetectionINT1();  // INT1 becomes HIGH when dx, dy or dz are over/under +/-125mg (range = +/-2g) for 2s (dr = 1Hz)
  uint8_t getIG1Source();
  uint8_t getIG2Source();

private:
  /*  Private constant declerations (static) */
  static const uint8_t DEFAULT_I2C_ADDRESS = 0x28;  // 0x28 (SA0 = GND) or 0x29 (SA0 = VDD)
  static const int8_t ACCEL_THRESHOLD  = 60;        // found empirically

  /* Private member data */
  //WireProxy& wireRef_;
  TwoWire& wireRef_;
  uint8_t int1Pin_, int2Pin_;
  uint8_t i2cAddress_;

  /* Private member functions */
  uint8_t readReg(uint8_t regAddress);
  void readReg(uint8_t regAddress, uint8_t *regValue, uint8_t quanity, bool autoIncrement = true);
  void writeReg(uint8_t regAddress, uint8_t regValue);
  void writeReg(uint8_t regAddress, uint8_t *regValue, size_t quanity, bool autoIncrement = true);
};

#endif
