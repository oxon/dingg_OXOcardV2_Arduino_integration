/*******************************************************************************
* \file    matrix.cpp
********************************************************************************
* \author		Jascha Haldemann			jh@oxon.ch
* \date			31.01.2018
* \version 1.0
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
*******************************************************************************/

/* ================================= Imports ================================ */
#include <esp_log.h>
#include "driver/gpio.h"
#include "matrix.h"
#include "font_3x5.h"
#include "font_5x7.h"

/* FreeRTSO includes */
#include <freertos/FreeRTOS.h>

const uint8_t Matrix::GAMMA_CORRECTION[] = {
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
	  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
	  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
	 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
	 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
	 25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
	 37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
	 51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
	 69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
	 90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
	115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
	144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
	177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
	215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};


/* ======================= Module constant declaration ====================== */
#define LIMIT_COLORS_WHEN_MAX_CURRENT	// uncomment to limit the colors

/* ======================== Module macro declaration ======================== */
//...

/* ======================== Module global variables ========================= */
//...

/* ====================== Module class instantiations ======================= */
//...

/* ====================== Constructor & Deconstructor ======================= */
Matrix::Matrix(uint8_t dataPin, uint8_t enablePin, uint8_t numberOfRows,
	uint8_t numberOfColumns)
{
	dataPin_ = dataPin;
	enablePin_ = enablePin;
	numberOfRows_ = numberOfRows;
	numberOfColumns_ = numberOfColumns;
	totalNumberOfLeds = numberOfRows_ * numberOfColumns_;
	useGammaCorrection = 1;
}

Matrix::~Matrix()
{
	frameBufferList.clear();
	activeFrameBuffer = NULL;
	outputFrameBuffer = NULL;
	delete(esp32_sk6812_driven_leds); esp32_sk6812_driven_leds = NULL;
	delete(rmt_driven_leds); rmt_driven_leds = NULL;
	delete(ledsTiming); ledsTiming = NULL;
}

/* ======================== Public member Functions ========================= */
void Matrix::init()	// do not call this in the consturctor!
{
	ESP_LOGI(this->LOG_TAG, "Initializing");
	pinMode(enablePin_, OUTPUT);
	digitalWrite(enablePin_, HIGH);	// disable
	pinMode(dataPin_, OUTPUT);
	updateSemaphore = xSemaphoreCreateMutex();
	ledsTiming = new WS2812B();
	rmt_driven_leds = new RGBLeds(rmt_channel_t(RMT_CHANNEL_NR),
		gpio_num_t(dataPin_), totalNumberOfLeds, RGBLedsTiming(*ledsTiming));
	//esp32_sk6812_driven_leds = new ESP32_SK6812(dataPin_, totalNumberOfLeds);
	// create some frame buffers
	for (uint8_t n = 0; n < DEFAULT_NUMBER_OF_FRAME_BUFFERS; n++) {
		addFrameBuffer();
	}
	ESP_LOGI(this->LOG_TAG, "Created a FramBufferList with %d FrameBuffers.",
		getFrameBufferListLength());
	setActiveFrameBuffer(1);
	outputFrameBuffer = &frameBufferList.front();
}

/* HW control functions */
void Matrix::enableDisplay() {
	digitalWrite(enablePin_, LOW);
	vTaskDelay(1/portTICK_PERIOD_MS);
}

void Matrix::disableDisplay() {
	clear();
	update();
	digitalWrite(enablePin_, HIGH);
}

/* Buffer content control functions */
void Matrix::setPixel(uint8_t x, uint8_t y, rgbColor_t color) {
	if (x >= numberOfColumns_ || y >= numberOfRows_) return;
	setPixel(x+y*numberOfRows_, color);
}

void Matrix::setPixel(uint16_t n, rgbColor_t color) {
	activeFrameBuffer->setPixel(n, color);

}

void Matrix::clear() {
	activeFrameBuffer->clear();
}

void Matrix::update()
{
	// rgb_leds (rmt) driver
	if (rmt_driven_leds == NULL)
	{
		delete(esp32_sk6812_driven_leds);
		esp32_sk6812_driven_leds = NULL;
		rmt_driven_leds = new RGBLeds(rmt_channel_t(RMT_CHANNEL_NR),
			gpio_num_t(dataPin_), totalNumberOfLeds, RGBLedsTiming(*ledsTiming));
	}
	float scale = 1.0;
	#ifdef LIMIT_COLORS_WHEN_MAX_CURRENT
		/* get the current color sum */
		uint16_t currentColorSum = 0;
		for (uint8_t n = 0; n < totalNumberOfLeds; n++)
		{
			rgbColor_t color = activeFrameBuffer->getPixel(n);
			currentColorSum += GAMMA_CORRECTION[color.r];
			currentColorSum += GAMMA_CORRECTION[color.g];
			currentColorSum += GAMMA_CORRECTION[color.b];
		}
		/* check if max will be reached */
		if (currentColorSum > MAX_BRIGHTNESS_SUM_VALUE)
		{
			scale = (float)(MAX_BRIGHTNESS_SUM_VALUE)/currentColorSum;
		}
	#endif
	xSemaphoreTake(updateSemaphore, portMAX_DELAY);
	for (uint8_t n = 0; n < totalNumberOfLeds; n++)
	{
		rgbColor_t color = activeFrameBuffer->getPixel(n);
		if(useGammaCorrection)
		{
			color.r = GAMMA_CORRECTION[color.r]*scale;
			color.g = GAMMA_CORRECTION[color.g]*scale;
			color.b = GAMMA_CORRECTION[color.b]*scale;
		}
		else
		{
			color.r *= scale;
			color.g *= scale;
			color.b *= scale;
		}
		outputFrameBuffer->setPixel(n, color);
		rmt_driven_leds->set_pixel(n, color.r, color.g, color.b);
	}
	rmt_driven_leds->apply();
	xSemaphoreGive(updateSemaphore);
}

void Matrix::update_esp32_sk6812()
{
	// esp32_sk6812 driver
	if (esp32_sk6812_driven_leds == NULL)
	{
		delete(rmt_driven_leds);
		rmt_driven_leds = NULL;
		esp32_sk6812_driven_leds = new ESP32_SK6812(dataPin_, totalNumberOfLeds);
	}
	float scale = 1.0;
	#ifdef LIMIT_COLORS_WHEN_MAX_CURRENT
		/* get the current color sum */
		uint16_t currentColorSum = 0;
		for (uint8_t n = 0; n < totalNumberOfLeds; n++)
		{
			rgbColor_t color = activeFrameBuffer->getPixel(n);
			currentColorSum += GAMMA_CORRECTION[color.r];
			currentColorSum += GAMMA_CORRECTION[color.g];
			currentColorSum += GAMMA_CORRECTION[color.b];
		}
		/* check if max will be reached */
		if (currentColorSum > MAX_BRIGHTNESS_SUM_VALUE)
		{
			scale = (float)(MAX_BRIGHTNESS_SUM_VALUE)/currentColorSum;
		}
	#endif
	for (uint8_t n = 0; n < totalNumberOfLeds; n++)
	{
		rgbColor_t color = activeFrameBuffer->getPixel(n);
		if(useGammaCorrection)
		{
			color.r = GAMMA_CORRECTION[color.r]*scale;
			color.g = GAMMA_CORRECTION[color.g]*scale;
			color.b = GAMMA_CORRECTION[color.b]*scale;
		}
		else
		{
			color.r *= scale;
			color.g *= scale;
			color.b *= scale;
		}
		outputFrameBuffer->setPixel(n, color);
	}
	esp32_sk6812_driven_leds->apply(outputFrameBuffer);
}

/* Pixel functions */
void Matrix::drawPixel(uint8_t x, uint8_t y) {drawPixel(x, y, foreColor);}
void Matrix::drawPixel(uint8_t x, uint8_t y, rgbColor_t color)
{
	setPixel(x, y, color);
	if (autoUpdate) update();
}

void Matrix::clearPixel(uint8_t x, uint8_t y)
{
	setPixel(x, y, rgb(0, 0, 0));
	if (autoUpdate) update();
}

rgbColor_t Matrix::readPixel(uint8_t x, uint8_t y)
{
	return activeFrameBuffer->getPixel(x, y);
}

bool Matrix::isPixel(uint8_t x, uint8_t y)
{
	rgbColor_t pixel = activeFrameBuffer->getPixel(x, y);
	return (pixel.r || pixel.g || pixel.b);
}

/* Screen functions */
void Matrix::fillScreen() {fillScreen(foreColor);}
void Matrix::fillScreen(rgbColor_t color)
{
	for (uint16_t n = 0; n < totalNumberOfLeds; n++) setPixel(n, color);
	if (autoUpdate) update();
}

void Matrix::clearScreen()
{
	clear();
	if (autoUpdate) update();
}

/* Geometry functions */
void Matrix::drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
	drawLine(x0, y0, x1, y1, foreColor);
}
void Matrix::drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
	rgbColor_t color)
{
	int16_t dx = x1 - x0;
	int16_t dy = y1 - y0;
	int16_t dx_abs = abs(dx);
	int16_t dy_abs = abs(dy);
	int16_t s_dx = sgn(dx);
	int16_t s_dy = sgn(dy);
	int16_t x = dy_abs >> 1;
	int16_t y = dx_abs >> 1;
	int16_t px = x0;
	int16_t py = y0;

	setPixel(x0, y0, color);	// draw first pixel
	if (dx_abs >= dy_abs)
	{
		// the line is more horizontal than vertical
		int16_t i;
		for (i = 0; i < dx_abs; i++)
		{
			y += dy_abs;
			if (y >= dx_abs)
			{
				y -= dx_abs;
				py += s_dy;
			}
			px += s_dx;
			setPixel(px, py, color);
		}
	}
	else
	{
		// the line is more vertical than horizontal
		int16_t i;
		for (i = 0; i < dy_abs; i++)
		{
			x += dx_abs;
			if (x >= dy_abs)
			{
				x -= dy_abs;
				px += s_dx;
			}
			py += s_dy;
			setPixel(px, py, color);
		}
	}
	if (autoUpdate) update();
}

void Matrix::drawHLine(uint8_t x, uint8_t y, uint8_t l, rgbColor_t color) {
	drawLine(x, y, x + l - 1, y, color);
}

void Matrix::drawVLine(uint8_t x, uint8_t y, uint8_t h, rgbColor_t color) {
	drawLine(x, y, x, y + h - 1, color);
}

void Matrix::drawRectangle(uint8_t x, uint8_t y, uint8_t l, uint8_t h) {
	drawRectangle(x, y, l, h, foreColor);
}
void Matrix::drawRectangle(uint8_t x, uint8_t y, uint8_t l, uint8_t h,
	rgbColor_t color)
{
	bool oldAutoUpdate = autoUpdate;
	autoUpdate = false;
  if (fillMode) drawFilledRectangle(x, y, l, h, color);
	else
	{
		drawHLine(x, y, l, color);
		drawVLine(x, y, h, color);
		drawHLine(x, y + h - 1, l, color);
		drawVLine(x + l - 1, y, h, color);
	}
	autoUpdate = oldAutoUpdate;
  if (autoUpdate) update();
}

void Matrix::drawCircle(uint8_t x0, uint8_t y0, uint8_t r) {
	drawCircle(x0, y0, r, foreColor);
}
void Matrix::drawCircle(uint8_t x0, uint8_t y0, uint8_t r, rgbColor_t color)
{
	bool oldAutoUpdate = autoUpdate;
	autoUpdate = false;
	if (fillMode) drawFilledCircle(x0, y0, r, color);
	else
	{
		int16_t f = 1 - r;
		int16_t ddF_x = 1;
		int16_t ddF_y = -2 * r;
		int16_t x = 0;
		int16_t y = r;

		setPixel(x0, y0 + r, color);
		setPixel(x0, y0 - r, color);
		setPixel(x0 + r, y0, color);
		setPixel(x0 - r, y0, color);
		while (x < y)
		{
			if (f >= 0)
			{
				y--;
				ddF_y += 2;
				f += ddF_y;
			}
			x++;
			ddF_x += 2;
			f += ddF_x;
			setPixel(x0 + x, y0 + y, color);
			setPixel(x0 - x, y0 + y, color);
			setPixel(x0 + x, y0 - y, color);
			setPixel(x0 - x, y0 - y, color);
			setPixel(x0 + y, y0 + x, color);
			setPixel(x0 - y, y0 + x, color);
			setPixel(x0 + y, y0 - x, color);
			setPixel(x0 - y, y0 - x, color);
	  }
	}
	autoUpdate = oldAutoUpdate;
	if (autoUpdate) update();
}

void Matrix::drawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
	uint8_t x2, uint8_t y2) {
	drawTriangle(x0, y0, x1, y1, x2, y2, foreColor);
}
void Matrix::drawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
	uint8_t x2, uint8_t y2, rgbColor_t color)
{
	bool oldAutoUpdate = autoUpdate;
	autoUpdate = false;
	if (fillMode) drawFilledTriangle(x0, y0, x1, y1, x2, y2, color);
	else
	{
		drawLine(x0, y0, x1, y1, color);
		drawLine(x0, y0, x2, y2, color);
		drawLine(x1, y1, x2, y2, color);
	}
	autoUpdate = oldAutoUpdate;
	if (autoUpdate) update();
}

/* Draw image functions */
void Matrix::drawImage(uint8_t *image, uint8_t ox, uint8_t oy) {
	drawImage(image, ox, oy, foreColor);
}
void Matrix::drawImage(uint8_t *image, uint8_t ox, uint8_t oy, rgbColor_t color)
{
	for (uint8_t y = 0; y < numberOfRows_; y++)
	{
		uint8_t rowByte = image[y];
		for (uint8_t x = 0; x < numberOfColumns_; x++)
		{
			if (rowByte & (1 << (7 - x)))
				setPixel(x+ox, y+oy, color);
		}
	}
	if (autoUpdate) update();
}

void Matrix::drawImage(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3,
	uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7, uint8_t ox, uint8_t oy) {
	drawImage(b0, b1, b2, b3, b4, b5, b6, b7, ox, oy, foreColor);
}
void Matrix::drawImage(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3,
	uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7, uint8_t ox, uint8_t oy,
	rgbColor_t color)
{
	uint8_t image[8] = {b0, b1, b2, b3, b4, b5, b6, b7};
	drawImage(image, ox, oy, color);
}

void Matrix::drawImage(uint64_t image, uint8_t ox, uint8_t oy) {
	drawImage(image, ox, oy, foreColor);
}
void Matrix::drawImage(uint64_t image, uint8_t ox, uint8_t oy, rgbColor_t color)
{
  byteInt_t bi;
  bi.int64 = swap_uint64(image);
  drawImage(bi.bytes, ox, oy, color);
}

void Matrix::drawImage(rgbColor_t *pixels, uint8_t ox, uint8_t oy)
{
	uint8_t x = 0, y = 0;
	for (uint16_t i = 0; i < totalNumberOfLeds; i++)
	{
		setPixel(x+ox, y+oy, pixels[i]);
		x++;
		if (x >= numberOfColumns_)
		{
			x = 0;
			y++;
		}
	}
	if (autoUpdate) update();
}

/* Draw char functions */
void Matrix::drawCharSmallFont(char c, uint8_t ox, uint8_t oy) {
	drawCharSmallFont(c, ox, oy, foreColor);
}
void Matrix::drawCharSmallFont(char c, uint8_t ox, uint8_t oy, rgbColor_t color)
{
	if ((c < 32) || (c > 127)) return;	// printable character?
	for (uint8_t cy = 0; cy < FONT_3X5_HEIGHT; cy++)
	{
		for (uint8_t cx = 0; cx < FONT_3X5_WIDTH; cx++)
		{
			if (pgm_read_byte(&(font3x5[c - 32][cy][cx])) > 0)
				setPixel(ox + cx, oy + cy, color);
		}
	}
	if (autoUpdate) update();
}

void Matrix::drawCharBigFont(char c, uint8_t ox, uint8_t oy) {
	drawCharBigFont(c, ox, oy, foreColor);
}
void Matrix::drawCharBigFont(char c, uint8_t ox, uint8_t oy,
	rgbColor_t color)
{
	if ((c < 32) || (c > 127)) return;	// printable character?
	for (uint8_t cx = 0; cx < FONT_5X7_WIDTH; cx++)
	{
		uint8_t columnByte = pgm_read_byte(&(font5x7[((c-32)*FONT_5X7_WIDTH)+cx]));
		for (uint8_t cy = 1; cy <= FONT_5X7_HEIGHT; cy++)	// start with 1 since the last bit is not set
		{
			if (columnByte & (1 << (7 - cy))) setPixel(ox+cx, oy+(7-cy), color);
		}
	}
	if (autoUpdate) update();
}

/* Draw text functions */
void Matrix::drawNumber(double number) {drawNumber(number, foreColor);}
void Matrix::drawNumber(double number, rgbColor_t color)
{
	String text = String(number);
	double diff = number - long(number);
	if (diff == 0.0) text = String(long(number));
	const uint8_t textLength = text.length();
	const uint8_t y0 =  1;
	clear();
	if (textLength == 1)
	{
		drawCharSmallFont('0', 0, y0, color);
		drawCharSmallFont(text.charAt(0), 4, y0, color);
	}
	else if (textLength == 2)
	{
		drawCharSmallFont(text.charAt(0), 0, y0, color);
		drawCharSmallFont(text.charAt(1), 4, y0, color);
	}
	else
	{
		/* scrolling text */
		const uint8_t width = FONT_3X5_WIDTH + 1;	// width + space
		uint8_t x = 0;
		bool oldAutoUpdate = autoUpdate;
		autoUpdate = false;
		do
		{
			if (x == 1) delay(800);  // we wait a little bit, until the scrolling starts
			clear();
			for (uint8_t i = 0; i < textLength; i++) {
				drawCharSmallFont(text.charAt(i), (i*width)-x, y0, color);
			}
			update();
			delay(75);
			x++;
		} while (x < ((textLength-2)*width));
		autoUpdate = oldAutoUpdate;
	}
}

void Matrix::drawText(
		String text,
		bool bigFont,
		bool backwards,
		uint8_t speed
	){
	this->drawText(text, bigFont, backwards, speed, foreColor);
}

void Matrix::drawText(
		String text,
		bool bigFont,
		bool backwards,
		uint8_t speed,
		rgbColor_t color
	){
	char charText[text.length()+1] = {0};
	text.toCharArray(charText, text.length());
	this->drawText(charText, bigFont, backwards, speed, color);
}

void Matrix::drawText(
		const char* text,
		bool bigFont,
		bool backwards,
		uint8_t speed
	){
	this->drawText(text, bigFont, backwards, speed, foreColor);
}

void Matrix::drawText(
		const char *text,
		bool bigFont,
		bool backwards,
		uint8_t speed,
		rgbColor_t color
	){
	const uint8_t FONT_WIDTH = bigFont ? FONT_5X7_WIDTH : FONT_3X5_WIDTH;
	const uint8_t CHAR_SPACING_OFFSET = 1;
	const uint8_t x0 = 8;
	const uint8_t y0 = bigFont ? 0 : 1;
	const uint8_t textLength = strlen(text);
	const uint16_t textLengthInPixels = ((FONT_WIDTH+CHAR_SPACING_OFFSET)*(textLength));
	const uint16_t delayTime = 255-speed+10; // minimum sleep of 10
	bool oldAutoUpdate = autoUpdate;
	autoUpdate = false;
	if (backwards)
	{
		/* scroll (backwards) from x = -textLengthInPixels to x = x0-8 */
		for(int8_t x = -textLengthInPixels; x <= x0-DEFAULT_NUMBER_OF_COLUMNS; x++)
		{
			/* loop throug each char in text */
			for(int8_t i = 0; i < textLength; i++)
			{
				/* draw char if it will be visible */
				int8_t xNew = (x + (FONT_WIDTH + CHAR_SPACING_OFFSET)*i);
				if(xNew <= 7 && xNew >= -(FONT_WIDTH-1))
				{
					if(bigFont) drawCharBigFont(text[i], xNew, y0, color);
					else drawCharSmallFont(text[i], xNew, y0, color);
				}
			}
			update();
			vTaskDelay(delayTime/portTICK_PERIOD_MS);
			clear();
		}
	}
	else
	{
		/* scroll from x = x0 to x = -textLengthInPixels+8 */
		for(int8_t x = x0; x >= -textLengthInPixels+DEFAULT_NUMBER_OF_COLUMNS; x--)
		{
			for(int8_t i = 0; i < textLength; i++)
			{
				int8_t xNew = (x + (FONT_WIDTH + CHAR_SPACING_OFFSET)*i);
				if(xNew <= 7 && xNew >= -(FONT_WIDTH-1))
				{
					if(bigFont) drawCharBigFont(text[i], xNew, y0, color);
					else drawCharSmallFont(text[i], xNew, y0, color);
				}
			}
			update();
			vTaskDelay(delayTime/portTICK_PERIOD_MS);
			clear();
		}
	}
	autoUpdate = oldAutoUpdate;
}

void Matrix::setActiveFrameBuffer(uint8_t nr, bool log)
{
	if (nr >= getFrameBufferListLength()) nr = 0;
	activeFrameBuffer = &frameBufferList.front()+nr;
	if (log) {ESP_LOGI(this->LOG_TAG, "FrameBuffer_%d is now active.", nr);}
	activeFrameBufferNr = nr;
}

void Matrix::copyFrameBufferContent(uint8_t destinationNr, uint8_t sourceNr)
{
	if (destinationNr >= getFrameBufferListLength()) return;
	if (sourceNr >= getFrameBufferListLength()) return;
	FrameBuffer *destinationFrameBuffer = &frameBufferList.front()+destinationNr;
	FrameBuffer *sourceFrameBuffer = &frameBufferList.front()+sourceNr;
	copyFrameBufferContent(destinationFrameBuffer, sourceFrameBuffer);
}

void Matrix::copyFrameBufferContent(FrameBuffer *destinationFrameBuffer,
	FrameBuffer *sourceFrameBuffer)
{
	for (uint16_t n = 0; n < totalNumberOfLeds; n++) {
		destinationFrameBuffer->setPixel(n, sourceFrameBuffer->getPixel(n));
	}
}

/* ======================= Private member Functions ========================= */
void Matrix::drawFilledRectangle(uint8_t x, uint8_t y, uint8_t l, uint8_t h,
	rgbColor_t color) {
	for (uint8_t i = y; i < y + h; i++) drawHLine(x, i, l, color);
}

void Matrix::drawFilledCircle(uint8_t x0, uint8_t y0, uint8_t r,
	rgbColor_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	drawHLine(x0 - r, y0, 2 * r + 1, color);
	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		drawLine(x0 + x, y0 + y, x0 - x, y0 + y, color);
		drawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);
		drawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
		drawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
	}
}

void Matrix::drawFilledTriangle(uint8_t x0, uint8_t y0, uint8_t x1,
	uint8_t y1, uint8_t x2, uint8_t y2, rgbColor_t color)
{
	/* do the same as in the drawLine() function but draw lines to the static
		corner (x2,y2) instead of single pixels */
	int16_t dx = x1 - x0;
	int16_t dy = y1 - y0;
	int16_t dx_abs = abs(dx);
	int16_t dy_abs = abs(dy);
	int16_t s_dx = sgn(dx);
	int16_t s_dy = sgn(dy);
	int16_t x = dy_abs >> 1;
	int16_t y = dx_abs >> 1;
	int16_t px = x0;
	int16_t py = y0;

	if (dx_abs >= dy_abs)
	{
		// the line is more horizontal than vertical
		int16_t i;
		for (i = 0; i < dx_abs; i++)
		{
			y += dy_abs;
			if (y >= dx_abs)
			{
				y -= dx_abs;
				py += s_dy;
			}
			px += s_dx;
			drawLine(x2, y2, px, py, color);
		}
	}
	else
	{
		// the line is more vertical than horizontal
		int16_t i;
		for (i = 0; i < dy_abs; i++)
		{
			x += dx_abs;
			if (x >= dy_abs)
			{
				x -= dy_abs;
				px += s_dx;
			}
			py += s_dy;
			drawLine(x2, y2, px, py, color);
		}
	}

	/* do the same again, but with (x0,y0) as the static corner to get rid of some
		empty pixels */
	dx = x2 - x1;
	dy = y2 - y1;
	dx_abs = abs(dx);
	dy_abs = abs(dy);
	s_dx = sgn(dx);
	s_dy = sgn(dy);
	x = dy_abs >> 1;
	y = dx_abs >> 1;
	px = x1;
	py = y1;

	if (dx_abs >= dy_abs)
	{
		// the line is more horizontal than vertical
		int16_t i;
		for (i = 0; i < dx_abs; i++)
		{
			y += dy_abs;
			if (y >= dx_abs)
			{
				y -= dx_abs;
				py += s_dy;
			}
			px += s_dx;
			drawLine(x0, y0, px, py, color);
		}
	}
	else
	{
		// the line is more vertical than horizontal
		int16_t i;
		for (i = 0; i < dy_abs; i++)
		{
			x += dx_abs;
			if (x >= dy_abs)
			{
				x -= dy_abs;
				px += s_dx;
			}
			py += s_dy;
			drawLine(x0, y0, px, py, color);
		}
	}
}

/* ======================== Private class Functions ========================= */
int8_t Matrix::sgn(int16_t value)
{
	if (value > 0) return 1;
	if (value < 0) return -1;
	return 0;
}

uint64_t Matrix::swap_uint64(uint64_t val)
{
	val = ((val << 8) & 0xFF00FF00FF00FF00ULL) |
		((val >> 8) & 0x00FF00FF00FF00FFULL);
	val = ((val << 16) & 0xFFFF0000FFFF0000ULL) |
		((val >> 16) & 0x0000FFFF0000FFFFULL);
	return ((val << 32) | (val >> 32));
}
