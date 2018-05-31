#ifndef _LIB_MATRIX_H_
#define _LIB_MATRIX_H_
/*******************************************************************************
* \file		matrix.h
********************************************************************************
* \author		Jascha Haldemann			jh@oxon.ch
* \date			31.01.2018
* \version	1.0
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
* OXOcard2
*******************************************************************************/

/* ============================== Global imports ============================ */
#include <Arduino.h>
#include <list>
#include "esp32_sk6812.h"
#include "frame_buffer.h"
#include "rgb_leds.h"
#include "rgb_leds_timing.h"

/* ========================== Global module typedefs ======================== */
//typedef rgbColor_t matrixColor_t;

/* ==================== Global module constant declaration ================== */
#define DEFAULT_FORE_COLOR		rgb(0, 161, 167)

/* ========================= Global macro declaration ======================= */
//...

/* ============================ Class declaration =========================== */
class Matrix
{
public:
	static const uint8_t GAMMA_CORRECTION[];

	Matrix(uint8_t dataPin, uint8_t enablePin,
		uint8_t numberOfRows = DEFAULT_NUMBER_OF_ROWS,
		uint8_t numberOfColumns = DEFAULT_NUMBER_OF_COLUMNS);
	~Matrix();

	/* Public member functions */
	void init();

	/*   HW control functions */
	void enableDisplay();
	void disableDisplay();

	/*   Buffer content control functions */
	void setPixel(uint8_t x, uint8_t y, rgbColor_t color);
	void setPixel(uint16_t n, rgbColor_t color);
	rgbColor_t readPixel(uint8_t x, uint8_t y);
	void clear();
	void update();
	void update_esp32_sk6812();

	/*   Pixel functions */
	void drawPixel(uint8_t x, uint8_t y);
	void drawPixel(uint8_t x, uint8_t y, rgbColor_t color);
	void clearPixel(uint8_t x, uint8_t y);
	bool isPixel(uint8_t x, uint8_t y);

	/*   Screen functions */
	void fillScreen();
	void fillScreen(rgbColor_t color);
	void clearScreen();

	/*   Geometry functions */
	void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
	void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
		rgbColor_t color);
	void drawHLine(uint8_t x, uint8_t y, uint8_t l, rgbColor_t color);
	void drawVLine(uint8_t x, uint8_t y, uint8_t h, rgbColor_t color);
	void drawRectangle(uint8_t x, uint8_t y, uint8_t l, uint8_t h);
	void drawRectangle(uint8_t x, uint8_t y, uint8_t l, uint8_t h,
		rgbColor_t color);
	void drawCircle(uint8_t x0, uint8_t y0, uint8_t r);
	void drawCircle(uint8_t x0, uint8_t y0, uint8_t r, rgbColor_t color);
	void drawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
		uint8_t x2, uint8_t y2);
	void drawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
		uint8_t x2, uint8_t y2, rgbColor_t color);

	/*   Draw image functions */
	void drawImage(uint8_t *image, uint8_t ox = 0, uint8_t oy = 0);
	void drawImage(uint8_t *image, uint8_t ox, uint8_t oy, rgbColor_t color);
	void drawImage(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4,
		uint8_t b5, uint8_t b6, uint8_t b7, uint8_t ox = 0, uint8_t oy = 0);
	void drawImage(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4,
		uint8_t b5, uint8_t b6, uint8_t b7, uint8_t ox, uint8_t oy,
		rgbColor_t color);
	void drawImage(uint64_t image, uint8_t ox = 0, uint8_t oy = 0);
	void drawImage(uint64_t image, uint8_t ox, uint8_t oy, rgbColor_t color);
	void drawImage(rgbColor_t *pixels, uint8_t ox = 0, uint8_t oy = 0);

	/*   Draw char functions */
	void drawCharSmallFont(char c, uint8_t ox, uint8_t oy = 1);
	void drawCharSmallFont(char c, uint8_t ox, uint8_t oy, rgbColor_t color);
	void drawCharBigFont(char c, uint8_t ox = 1, uint8_t oy = 0);
	void drawCharBigFont(char c, uint8_t ox, uint8_t oy, rgbColor_t color);

	/*   Draw text functions */
	void drawNumber(double number);
	void drawNumber(double number, rgbColor_t color);
	void drawText(String text, bool bigFont, bool backwards = false,
		uint8_t speed = 200);
	void drawText(String text, bool bigFont, bool backwards, uint8_t speed,
		rgbColor_t color);
	void drawText(const char *text, bool bigFont, bool backwards = false,
		uint8_t speed = 200);
	void drawText(const char *text, bool bigFont, bool backwards, uint8_t speed,
		rgbColor_t color);

	/*   Configuration functions */
	void setForeColor(rgbColor_t color) {foreColor = color;}
	rgbColor_t getForeColor() {return foreColor;}
	void fill() {fillMode = true;}
	void noFill() {fillMode = false;}
	void enableAutoUpdate() {autoUpdate = true;}
	void disableAutoUpdate() {autoUpdate = false;}
	bool isAutoUpdate() {return autoUpdate;}

	/*   FrameBuffer functions */
	void addFrameBuffer() {frameBufferList.push_back(FrameBuffer());}
	void removeFrameBuffer() {frameBufferList.pop_back();}
	size_t getFrameBufferListLength() {return frameBufferList.size();}
	void setActiveFrameBuffer(uint8_t nr, bool log = true);
	uint8_t getActiveFrameBufferNr() {return activeFrameBufferNr;}
	void copyFrameBufferContent(uint8_t destinationNr, uint8_t sourceNr);
	void copyFrameBufferContent(FrameBuffer *destinationFrameBuffer,
		FrameBuffer *sourceFrameBuffer);

	/*   general matrix functions */
	uint8_t getNumberOfRows() {return numberOfRows_;}
	uint8_t getNumberOfColumns() {return numberOfColumns_;}

	/* Public class functions (static) */
	//...

	/* Public variables */
	volatile SemaphoreHandle_t updateSemaphore;

private:
	/* Private member typedefs */
	typedef union {
		uint8_t bytes[8];
		uint64_t int64;
	} byteInt_t;

	/* Private constant declerations (static) */
	const char *LOG_TAG = "Matrix";
	static const uint8_t RMT_CHANNEL_NR = 0;
	static const uint8_t DEFAULT_NUMBER_OF_ROWS = 8;
	static const uint8_t DEFAULT_NUMBER_OF_COLUMNS = 8;
	static const uint8_t DEFAULT_NUMBER_OF_FRAME_BUFFERS = 4;
	static const uint16_t MAX_BRIGHTNESS_SUM_VALUE = 5000;	// max USB current at this value is ~0.46A (including an accu and wifi)

	/* Private member data */
	uint8_t dataPin_, enablePin_;
	uint8_t numberOfRows_, numberOfColumns_;
	uint8_t useGammaCorrection;
	std::list<FrameBuffer> frameBufferList = {};
	FrameBuffer *outputFrameBuffer = NULL;
	FrameBuffer *activeFrameBuffer = NULL;
	RGBLedsTiming *ledsTiming = NULL;
	RGBLeds *rmt_driven_leds = NULL;
	ESP32_SK6812 *esp32_sk6812_driven_leds = NULL;

	uint8_t activeFrameBufferNr = 0;
	uint16_t totalNumberOfLeds = 0;
	rgbColor_t foreColor = DEFAULT_FORE_COLOR;
	bool fillMode = false;
	bool autoUpdate = true;

	/* Private member functions */
	void drawFilledRectangle(uint8_t x, uint8_t y, uint8_t l, uint8_t h,
		rgbColor_t color);
	void drawFilledCircle(uint8_t x0, uint8_t y0, uint8_t r, rgbColor_t color);
	void drawFilledTriangle(uint8_t x0, uint8_t y0, uint8_t x1,
		uint8_t y1, uint8_t x2, uint8_t y2, rgbColor_t color);

	/* Private class functions (static) */
	static int8_t sgn(int16_t value);
	static uint64_t swap_uint64(uint64_t val);

	void dumpCurrentFrameBufferToLog();
};

#endif
