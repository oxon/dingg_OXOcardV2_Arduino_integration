#ifndef _LIB_FRAME_BUFFER_H_
#define _LIB_FRAME_BUFFER_H_

#include <stdint.h>
#include "rgb_leds.h"

typedef rgbColor_t element_t;	// accessible from "rgb_leds.h"

class FrameBuffer{

	public:
		static const uint8_t HEIGHT = 8;
		static const uint8_t WIDTH = 8;

		FrameBuffer(){};
		~FrameBuffer(){};

		void setPixel(uint16_t n, element_t value);
		void setPixel(uint8_t x, uint8_t y, element_t value);
		element_t getPixel(uint16_t n);
		element_t getPixel(uint8_t x, uint8_t y);
		void clear();

	private:
		const char *LOG_TAG = "FrameBuffer";
		element_t buffer[FrameBuffer::WIDTH*FrameBuffer::HEIGHT];

};

#endif
