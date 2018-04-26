/*
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
 */

#include "animations.h"

void Animation::setMatrix(Matrix *matrix){
	this->matrix = matrix;
	this->matrix->disableAutoUpdate();
}

void Animation::setImage(byte bytes[], int size){
	this->image = (byte*)malloc(sizeof(byte)*size);
	for(int i=0; i<size; i++)
		this->image[i] = bytes[i];
}

void Animation::setImage(byte bytes[]){
	this->setImage(bytes, 8);
}

void Animation::setRGBVal(rgbColor_t color){
	this->color = color;
}

WaveAnimation::WaveAnimation(){

}

void WaveAnimation::drawCycle(){

	for(int y=0; y<8; y++){

	float distance = y-(this->currentRow-this->fadeLength);
			if(distance < 0) distance *= -1.0;

		for(int x=0; x<8; x++){
			float val = ((this->image[y] & (1<<(7-x))) > 0) ? 1.0 : 0.0;
			if(distance <= this->fadeLength){
				val*=1.0+(this->fadeLength-distance)/this->fadeLength*this->amplification;
			}
			this->matrix->drawPixel(x, y, rgb(this->color.r*val, this->color.g*val, this->color.b*val));
		}
	}

	this->matrix->update();
	this->currentRow = (this->currentRow+0.2);
	if(this->currentRow >= fadeLength*2+8){
		this->currentRow = 0.0;
	}
}


GlowAnimation::GlowAnimation(){

}

void GlowAnimation::drawCycle(){
	if(this->currentAmplification <= 0.5f || this->currentAmplification >= 1.0f){
		this->decreasing = !this->decreasing;
	}

	this->currentAmplification += (this->decreasing) ? (-this->step) : (this->step);

	rgbColor_t currentColor = rgb(
		this->color.r*this->currentAmplification,
		this->color.g*this->currentAmplification,
		this->color.b*this->currentAmplification
	);

	for(int y=0; y<8; y++){
		for(int x=0; x<8; x++){
			if((this->image[y] & (1<<(7-x))) > 0){
				this->matrix->drawPixel(x, y, currentColor);
			}
		}
	}
	this->matrix->update();
}

bool running = true;

void AnimationHelper::startTask(void *pvParameter){
	Animation *a = (Animation*)pvParameter;
	while(running){
		a->drawCycle();
		delay(AnimationHelper::delayTime);
	}
}

void AnimationHelper::stop(){
	running = false;
}
