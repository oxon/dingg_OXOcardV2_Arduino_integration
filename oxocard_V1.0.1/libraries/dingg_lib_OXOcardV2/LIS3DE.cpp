/*******************************************************************************
* \file    LIS3DE.cpp
********************************************************************************
* \author  Jascha Haldemann jh@oxon.ch
* \date    06.04.2017
* \version 1.0
*******************************************************************************/

/* ================================= Imports ================================ */
#include "LIS3DE.h"

/* FreeRTOS includes */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/* ======================= Module constant declaration ====================== */
constexpr uint8_t CHIP_ID         = 0x33;

/* Register definitions */
constexpr uint8_t STATUS_REG_AUX  = 0x07;
constexpr uint8_t OUT_ADC1_L      = 0x08;
constexpr uint8_t OUT_ADC1_H      = 0x09;
constexpr uint8_t OUT_ADC2_L      = 0x0A;
constexpr uint8_t OUT_ADC2_H      = 0x0B;
constexpr uint8_t OUT_ADC3_L      = 0x0C;
constexpr uint8_t OUT_ADC3_H      = 0x0D;
constexpr uint8_t INT_COUNTER_REG = 0x0E;
constexpr uint8_t WHO_AM_I        = 0x0F;
constexpr uint8_t TEMP_CFG_REG    = 0x1F;
constexpr uint8_t CTRL_REG1       = 0x20;
constexpr uint8_t CTRL_REG2       = 0x21;
constexpr uint8_t CTRL_REG3       = 0x22;
constexpr uint8_t CTRL_REG4       = 0x23;
constexpr uint8_t CTRL_REG5       = 0x24;
constexpr uint8_t CTRL_REG6       = 0x25;
constexpr uint8_t REFERENCE       = 0x26;
constexpr uint8_t STATUS_REG2     = 0x27;
constexpr uint8_t ACCELERATIONS   = 0x28;
constexpr uint8_t OUT_X           = 0x29;
constexpr uint8_t OUT_Y           = 0x2B;
constexpr uint8_t OUT_Z           = 0x2D;
constexpr uint8_t FIFO_CTRL_REG   = 0x2E;
constexpr uint8_t FIFO_SRC_REG    = 0x2F;
constexpr uint8_t IG1_CFG         = 0x30;
constexpr uint8_t IG1_SOURCE      = 0x31;
constexpr uint8_t IG1_THS         = 0x32;
constexpr uint8_t IG1_DURATION    = 0x33;
constexpr uint8_t IG2_CFG         = 0x34;
constexpr uint8_t IG2_SOURCE      = 0x35;
constexpr uint8_t IG2_THS         = 0x36;
constexpr uint8_t IG2_DURATION    = 0x37;
constexpr uint8_t CLICK_CFG       = 0x38;
constexpr uint8_t CLICK_SRC       = 0x39;
constexpr uint8_t CLICK_THS       = 0x3A;
constexpr uint8_t TIME_LIMIT      = 0x3B;
constexpr uint8_t TIME_LATENCY    = 0x3C;
constexpr uint8_t TIME_WINDOW     = 0x3D;
constexpr uint8_t Act_THS         = 0x3E;
constexpr uint8_t Act_DUR         = 0x3F;

/* Register masks */

/* Debug */
// #define DEBUG_ACCEL                    // define to activate the SoftwareSerial Debug prints
#define DEBUG_ACCEL_BAUDRATE    115200    // in Baud

/* ======================== Private macro declaration ======================= */
#ifdef DEBUG_ACCEL
	#define DebugAccel_begin(...)     Serial.begin(__VA_ARGS__)
  #define DebugAccel_print(...)     Serial.print(__VA_ARGS__)
  #define DebugAccel_println(...)   Serial.println(__VA_ARGS__)
#else
	#define DebugAccel_begin(...)
  #define DebugAccel_print(...)
  #define DebugAccel_println(...)
#endif

/* ======================== Public member Functions ========================= */
/** -------------------------------------------------------------------------
  * \fn     begin
  * \brief  init the HW
  *
  * \param  mode    operation mode (see enumerator in the header-file)
  * \param  range   full scale range (see enumerator in the header-file)
  * \param  dr      data rate (see enumerator in the header-file)
  * \return true if the sensor is accessible
  --------------------------------------------------------------------------- */
  bool LIS3DE::begin(mode_t mode, range_t range, dataRate_t dr)
  {
    bool accessible = false;
    DebugAccel_begin(DEBUG_ACCEL_BAUDRATE);
    vTaskDelay(5/portTICK_PERIOD_MS);	// necessary startup time after power-up
    wireRef_.begin();
		if (int1Pin_ != (uint8_t)(-1)) pinMode(int1Pin_, INPUT);
		if (int2Pin_ != (uint8_t)(-1)) pinMode(int2Pin_, INPUT);
    if (getChipID() == CHIP_ID)
    {
      accessible = true;
      setDataRate(dr);
      setMode(mode);
      setRange(range);
      /* HPF */
      //writeReg(int regAddress, int regValue)
    }
    return accessible;
  }

/** -------------------------------------------------------------------------
  * \fn     end
  * \brief  deinit the HW
  --------------------------------------------------------------------------- */
  void LIS3DE::end()
  {
    setDataRate(DATA_RATE_0HZ);  // Power-down mode
    #ifndef ESP32
      wireRef_.end();
    #endif
  }

/** -------------------------------------------------------------------------
  * \fn     getChipID
  * \brief  get chip ID byte
  *
  * \return chip ID byte
  --------------------------------------------------------------------------- */
  uint8_t LIS3DE::getChipID()
  {
    return readReg(WHO_AM_I);
  }

/** -------------------------------------------------------------------------
  * \fn     setMode
  * \brief  set operation mode
  *
  * \param  mode  operation mode (see enumerator in the header-file)
  * \return None
  --------------------------------------------------------------------------- */
  void LIS3DE::setMode(LIS3DE::mode_t mode)
  {
    DebugAccel_println(F("setMode..."));
    uint8_t reg1 = readReg(CTRL_REG1);
    reg1 &= ~0x08;
    reg1 |= (mode << 3);
    writeReg(CTRL_REG1, reg1);
  }

/** -------------------------------------------------------------------------
  * \fn     getMode
  * \brief  get operation mode
  *
  * \return operation mode (see enumerator in the header-file)
  --------------------------------------------------------------------------- */
  LIS3DE::mode_t LIS3DE::getMode()
  {
    return mode_t((readReg(CTRL_REG1) & 0x08) >> 3);
  }

/** -------------------------------------------------------------------------
  * \fn     setRange
  * \brief  set full scale range
  *
  * \param  range  full scale range (see enumerator in the header-file)
  * \return None
  --------------------------------------------------------------------------- */
  void LIS3DE::setRange(LIS3DE::range_t range)
  {
    DebugAccel_println(F("setRange..."));
    uint8_t reg4 = readReg(CTRL_REG4);
    reg4 &= ~0x30;
    reg4 |= (range << 4);
    writeReg(CTRL_REG4, reg4);
  }

/** -------------------------------------------------------------------------
  * \fn     getRange
  * \brief  get full scale range
  *
  * \return full scale range (see enumerator in the header-file)
  --------------------------------------------------------------------------- */
  LIS3DE::range_t LIS3DE::getRange()
  {
    return range_t((readReg(CTRL_REG4) & 0x30) >> 4);
  }

/** -------------------------------------------------------------------------
  * \fn     setDataRate
  * \brief  set data rate
  *
  * \param  mode  data rate (see enumerator in the header-file)
  * \return None
  --------------------------------------------------------------------------- */
  void LIS3DE::setDataRate(dataRate_t dr)
  {
    DebugAccel_println(F("setDataRate..."));
    uint8_t reg1 = readReg(CTRL_REG1);
    reg1 &= ~0xF0;
    reg1 |= (dr << 4);
    writeReg(CTRL_REG1, reg1);
  }

/** -------------------------------------------------------------------------
  * \fn     getDataRate
  * \brief  get data rate
  *
  * \return data rate (see enumerator in the header-file)
  --------------------------------------------------------------------------- */
  LIS3DE::dataRate_t LIS3DE::getDataRate()
  {
    return dataRate_t((readReg(CTRL_REG1) & 0xF0) >> 4);
  }

int8_t LIS3DE::getVectorX()
{
	return (int8_t)(readReg(OUT_X));
}

int8_t LIS3DE::getVectorY()
{
	return (int8_t)(readReg(OUT_Y));
}

int8_t LIS3DE::getVectorZ()
{
	return (int8_t)(readReg(OUT_Z));
}

/** -------------------------------------------------------------------------
  * \fn     getAccelerationVector
  * \brief  reads the x, y, and z accelerations
  *
  * \param  accelVector   pointer to an acceleration vector (x, y, z)
  * \return None
  --------------------------------------------------------------------------- */
  void LIS3DE::getAccelerationVector(int16_t accelVector[3])
  {
	accelVector[0] = (int8_t)(readReg(OUT_X));
	accelVector[1] = (int8_t)(readReg(OUT_Y));
	accelVector[2] = (int8_t)(readReg(OUT_Z));
  }

/** -------------------------------------------------------------------------
  * \fn     getAccelerationVector
  * \brief  reads the x, y, and z accelerations
  *
  * \param  accelVector		pointer to an acceleration vector (x, y, z)
	* \param  convert				convert to g if true
  * \return None
  --------------------------------------------------------------------------- */
	void LIS3DE::getAccelerationVector(float accelVector[3], bool convert)
	{
		int16_t tempVector[3];
		getAccelerationVector(tempVector);
		accelVector[0] = tempVector[0];
		accelVector[1] = tempVector[1];
		accelVector[2] = tempVector[2];
		if (convert) gScale(accelVector);
	}

/** -------------------------------------------------------------------------
  * \fn     mgScale
  * \brief  scales the given accelerations to mg (milli-g)
  *
  * \param  accelVector   pointer to an acceleration vector (x, y, z)
  * \return None
  --------------------------------------------------------------------------- */
	void LIS3DE::mgScale(int16_t accelVector[3])
	{
		uint8_t range = 2 * (2 * pow(2, getRange()));   // convert to g (+/- 2g -> 4g, +/- 4g -> 8g, ...)
		float scale = (float)(range)*1000/256;            // calculate scale to get mg
		accelVector[0] = (int16_t)((float)(accelVector[0]) * scale);
		accelVector[1] = (int16_t)((float)(accelVector[1]) * scale);
		accelVector[2] = (int16_t)((float)(accelVector[2]) * scale);
	}

/** -------------------------------------------------------------------------
  * \fn     gScale
  * \brief  scales the given accelerations to g
  *
  * \param  accelVector   pointer to an acceleration vector (x, y, z)
  * \return None
  --------------------------------------------------------------------------- */
	void LIS3DE::gScale(float accelVector[3])
	{
		uint8_t range = 2 * (2 * pow(2, getRange()));	// convert to g (+/- 2g -> 4g, +/- 4g -> 8g, ...)
		float scale = (float)(range)/256;								// calculate scale to get g
		accelVector[0] *= scale;
		accelVector[1] *= scale;
		accelVector[2] *= scale;
	}

/** -------------------------------------------------------------------------
  * \fn     getOrientation
  * \brief  gets the orientation
  *
  * \return orientation (see enumerator in the header file)
  --------------------------------------------------------------------------- */
	LIS3DE::orientation_t LIS3DE::getOrientation()
	{
		orientation_t orientation = UNKNOWN;
		int16_t accelVector[3];
		getAccelerationVector(accelVector);
		if      (accelVector[2] <= -ACCEL_THRESHOLD) orientation = DOWN;
		else if (accelVector[2] >=  ACCEL_THRESHOLD) orientation = UP;
		else if (accelVector[0] <= -ACCEL_THRESHOLD) orientation = HORIZONTALLY;
		else if (accelVector[0] <= -ACCEL_THRESHOLD/2) orientation = HALF_LEFT;
		else if (accelVector[0] >=  ACCEL_THRESHOLD) orientation = HORIZONTALLY;
		else if (accelVector[0] >=  ACCEL_THRESHOLD/2) orientation = HALF_RIGHT;
		else if (accelVector[1] <= -ACCEL_THRESHOLD) orientation = VERTICALLY;
		else if (accelVector[1] <= -ACCEL_THRESHOLD/2) orientation = HALF_DOWN;
		else if (accelVector[1] >=  ACCEL_THRESHOLD) orientation = VERTICALLY;
		else if (accelVector[1] >=  ACCEL_THRESHOLD/2) orientation = HALF_UP;
		return orientation;
	}

/** -------------------------------------------------------------------------
  * \fn     enableMovementDetectionINT1
  * \brief  enables the movement detection on INT1
  *         INT1 becomes high when accelerations are over/under given treshold
  *         for given time
  *
  * \param  ig1       select interrupt IG1 for Int1
  * \param  ig2       select interrupt IG2 for Int1
  * \param  treshold  threshold in mg (value = treshold * range/256); 0... 127
  * \param  duration  in s (value = duration / dataRate)
  * \return None
  --------------------------------------------------------------------------- */
  void LIS3DE::enableMovementDetectionINT1(bool ig1, bool ig2, uint8_t threshold, uint8_t duration)
  {
    /* configure the behavior of the interrupt recognition */
    if (ig1)
    {
      writeReg(IG1_THS, threshold);      // set threshold
      writeReg(IG1_DURATION, duration);  // set duration
    }

    if (ig2)
    {
      writeReg(IG2_THS, threshold);      // set threshold
      writeReg(IG2_DURATION, duration);  // set duration
    }

    /* configure which interrupt (IG1 o. IG2) will be on the INT1 pin */
    uint8_t reg3 = readReg(CTRL_REG3);
    reg3 &= ~0x60;
    if (ig1) reg3 |= (1 << 6);  // enable IG1
    if (ig2) reg3 |= (1 << 5);  // enable IG2
    writeReg(CTRL_REG3, reg3);

    /* enable HPF for defined interrupts */
    uint8_t reg2 = readReg(CTRL_REG2);
    reg2 &= ~0x03;
    if (ig1) reg2 |= (1 << 0);  // HPF enable for IG1
    if (ig2) reg2 |= (1 << 1);  // HPF enable for IG2
    writeReg(CTRL_REG2, reg2);

    /* configure on which event the interrupt will be fired */
    if (ig1) writeReg(IG1_CFG, 0b00101111); // OR combintation of interrupt events (everything but ZLIE)
    if (ig2) writeReg(IG2_CFG, 0b00101111); // OR combintation of interrupt events (everything but ZLIE)
  }

/** -------------------------------------------------------------------------
  * \fn     getIG1Source
  * \brief  get source byte of IG1
  *
  * \return source byte of IG1
  --------------------------------------------------------------------------- */
  uint8_t LIS3DE::getIG1Source()
  {
    return readReg(IG1_SOURCE);
  }

/** -------------------------------------------------------------------------
  * \fn     enableMovementDetectionINT2
  * \brief  enables the movement detection on INT2
  *         INT2 becomes high when accelerations are over/under given treshold
  *         for given time
  *
  * \param  ig1       select interrupt IG1 for Int2
  * \param  ig2       select interrupt IG2 for Int2
  * \param  treshold  threshold in mg (value = treshold * range/256); 0... 127
  * \param  duration  in s (value = duration / dataRate)
  * \return None
  --------------------------------------------------------------------------- */
  void LIS3DE::enableMovementDetectionINT2(bool ig1, bool ig2, uint8_t threshold, uint8_t duration)
  {
    /* configure the behavior of the interrupt recognition */
    if (ig1)
    {
      writeReg(IG1_THS, threshold);      // set threshold
      writeReg(IG1_DURATION, duration);  // set duration
    }

    if (ig2)
    {
      writeReg(IG2_THS, threshold);      // set threshold
      writeReg(IG2_DURATION, duration);  // set duration
    }

    /* configure which interrupt (IG1 o. IG2) will be on the INT2 pin */
    uint8_t reg6 = readReg(CTRL_REG6);
    reg6 &= ~0x60;
    if (ig1) reg6 |= (1 << 6);  // enable IG1
    if (ig2) reg6 |= (1 << 5);  // enable IG2
    writeReg(CTRL_REG6, reg6);

    /* enable HPF for defined interrupts */
    uint8_t reg2 = readReg(CTRL_REG2);
    reg2 &= ~0x03;
    if (ig1) reg2 |= (1 << 0);  // HPF enable for IG1
    if (ig2) reg2 |= (1 << 1);  // HPF enable for IG2
    writeReg(CTRL_REG2, reg2);

    /* configure on which event the interrupt will be fired */
    if (ig1) writeReg(IG1_CFG, 0b00101111); // OR combintation of interrupt events (everything but ZLIE)
    if (ig2) writeReg(IG2_CFG, 0b00101111); // OR combintation of interrupt events (everything but ZLIE)
  }

/** -------------------------------------------------------------------------
  * \fn     getIG2Source
  * \brief  get source byte of IG2
  *
  * \return source byte of IG2
  --------------------------------------------------------------------------- */
  uint8_t LIS3DE::getIG2Source()
  {
    return readReg(IG2_SOURCE);
  }

/** -------------------------------------------------------------------------
  * \fn     getTemperature
  * \brief  get temperature of the integratet temperature sensor (+/-2°C)
  *
  * \return temperature in °C
  --------------------------------------------------------------------------- */
	int8_t LIS3DE::getTemperature()
	{
		uint8_t tempReg = readReg(TEMP_CFG_REG);
		writeReg(TEMP_CFG_REG, tempReg | 0b11000000);
		uint8_t reg4 = readReg(CTRL_REG4);
		writeReg(CTRL_REG4, reg4 | 0b10000000);
		uint8_t adc3[2];
		readReg(OUT_ADC3_L, adc3, 2);		// both ADC3 registers must be read
		writeReg(CTRL_REG4, reg4 & ~0b11000000);
		return (int8_t)(adc3[1]) + 20;	// the temp data is in the OUT_ADC3_H register
	}

//TODO: read ADCs
/*
int16_t Adafruit_LIS3DH::readADC(uint8_t adc) {
  if ((adc < 1) || (adc > 3)) return 0;
  uint16_t value;

  adc--;

  uint8_t reg = LIS3DH_REG_OUTADC1_L + adc*2;

  if (_cs == -1) {
    // i2c
    wireRef_.beginTransmission(_i2caddr);
    wireRef_.write(reg | 0x80);   // 0x80 for autoincrement
    wireRef_.endTransmission();
    wireRef_.requestFrom(_i2caddr, 2);
    value = wireRef_.read();  value |= ((uint16_t)wireRef_.read()) << 8;
  }
  #ifndef __AVR_ATtiny85__
  else {
    if (_sck == -1)
      SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
    digitalWrite(_cs, LOW);
    spixfer(reg | 0x80 | 0x40); // read multiple, bit 7&6 high

    value = spixfer(); value |= ((uint16_t)spixfer()) << 8;

    digitalWrite(_cs, HIGH);
    if (_sck == -1)
      SPI.endTransaction();              // release the SPI bus
  }
  #endif

  return value;
}
*/

//TODO: other interrupts

//TODO: click
/*
void Adafruit_LIS3DH::setClick(uint8_t c, uint8_t clickthresh, uint8_t timelimit, uint8_t timelatency, uint8_t timewindow) {
  if (!c) {
    //disable int
    uint8_t r = readRegister8(LIS3DH_REG_CTRL3);
    r &= ~(0x80); // turn off I1_CLICK
    writeRegister8(LIS3DH_REG_CTRL3, r);
    writeRegister8(LIS3DH_REG_CLICKCFG, 0);
    return;
  }
  // else...

  writeRegister8(LIS3DH_REG_CTRL3, 0x80); // turn on int1 click
  writeRegister8(LIS3DH_REG_CTRL5, 0x08); // latch interrupt on int1

  if (c == 1)
    writeRegister8(LIS3DH_REG_CLICKCFG, 0x15); // turn on all axes & singletap
  if (c == 2)
    writeRegister8(LIS3DH_REG_CLICKCFG, 0x2A); // turn on all axes & doubletap


  writeRegister8(LIS3DH_REG_CLICKTHS, clickthresh); // arbitrary
  writeRegister8(LIS3DH_REG_TIMELIMIT, timelimit); // arbitrary
  writeRegister8(LIS3DH_REG_TIMELATENCY, timelatency); // arbitrary
  writeRegister8(LIS3DH_REG_TIMEWINDOW, timewindow); // arbitrary
}

uint8_t Adafruit_LIS3DH::getClick(void) {
  return readRegister8(LIS3DH_REG_CLICKSRC);
}
*/

/* ======================= Private member Functions ========================= */
/** -------------------------------------------------------------------------
  *   \fn     readReg
  *   \brief  read content of given register
  *
  *   \param  regAddress register address
  *   \return value of given register
  --------------------------------------------------------------------------- */
  uint8_t LIS3DE::readReg(uint8_t regAddress)
  {
    regAddress |= 0x80; // set MSB to enable auto increment

    uint8_t regValue;
    wireRef_.beginTransmission(i2cAddress_);
    wireRef_.write(regAddress);
    wireRef_.endTransmission();
    wireRef_.requestFrom(i2cAddress_, (uint8_t)(1));
    regValue = wireRef_.read();
    wireRef_.endTransmission();
    return regValue;
  }

/** -------------------------------------------------------------------------
 *   \fn     readReg
 *   \brief  read content of given register and continue reading
 *
 *   \param  regAddress    register address
 *   \param  regValue      variable pointer to store the register value
 *   \param  quantity      number of bytes to read
 *   \param  autoIncrement enable (true) or disable (false) the autoIncrement
 *   \return None
 ---------------------------------------------------------------------------- */
 // TODO: test this function before using anywhere
 void LIS3DE::readReg(uint8_t regAddress, uint8_t *regValue, uint8_t quantity, bool autoIncrement)
 {
    wireRef_.beginTransmission(i2cAddress_);
    if(autoIncrement)
    {
      regAddress |= 0x80; // set MSB to enable auto increment
      wireRef_.write(regAddress);
      wireRef_.transact(quantity);
      for(uint8_t i = 0; i < quantity; i++) regValue[i] = wireRef_.read();
    }
    else
    {
      for(uint8_t i = 0; i < quantity; i++)
      {
        wireRef_.write(regAddress+i);
        wireRef_.transact((uint8_t)(1));
        regValue[i] = wireRef_.read();
        if(i < (quantity-1))
        {
          wireRef_.endTransmission();
          wireRef_.beginTransmission(i2cAddress_);
        }
      }
    }
    wireRef_.endTransmission();
 }

 /** -------------------------------------------------------------------------
   *   \fn     writeReg
   *   \brief  read content of given register
   *
   *   \param  regAddress register address
   *   \param  regValue   value to write
   *   \return None
   --------------------------------------------------------------------------- */
  void LIS3DE::writeReg(uint8_t regAddress, uint8_t regValue)
  {
    wireRef_.beginTransmission(i2cAddress_);
    wireRef_.write(regAddress);
    wireRef_.write(regValue);
    wireRef_.endTransmission();
  }

  /** -------------------------------------------------------------------------
   *   \fn     writeReg
   *   \brief  write content to given register and continue writing
   *
   *   \param  regAddress    register address
   *   \param  regValue      variable pointer to value array
   *   \param  quantity      number of bytes to write
   *   \param  autoIncrement enable (true) or disable (false) the autoIncrement
   *   \return None
   ---------------------------------------------------------------------------- */
void LIS3DE::writeReg(uint8_t regAddress, uint8_t *regValue, size_t quantity, bool autoIncrement)
{
  wireRef_.beginTransmission(i2cAddress_);
  if(autoIncrement)
  {
    regAddress |= 0x80; // set MSB to enable auto increment
    wireRef_.write(regAddress);
    wireRef_.write(regValue, quantity);
  }
  else
  {
    for(uint8_t i = 0; i < quantity; i++)
    {
      wireRef_.write(regAddress+i);
      wireRef_.write(regValue[i]);
      if(i < (quantity-1))
      {
        wireRef_.endTransmission();
        wireRef_.beginTransmission(i2cAddress_);
      }
    }
  }
  wireRef_.endTransmission();
}
