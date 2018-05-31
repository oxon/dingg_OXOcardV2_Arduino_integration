#include <esp_log.h>
#include <string.h>
#include "frame_buffer.h"

void FrameBuffer::setPixel(uint16_t n, element_t value){
	this->buffer[n] = value;
}

void FrameBuffer::setPixel(uint8_t x, uint8_t y, element_t value){
	this->buffer[x+y*FrameBuffer::HEIGHT] = value;
}

element_t FrameBuffer::getPixel(uint16_t n){
	return this->buffer[n];
}

element_t FrameBuffer::getPixel(uint8_t x, uint8_t y){
	return this->buffer[x+y*FrameBuffer::HEIGHT];
}

void FrameBuffer::clear(){
	memset(buffer, 0, sizeof(element_t)*FrameBuffer::HEIGHT*FrameBuffer::WIDTH);
}
