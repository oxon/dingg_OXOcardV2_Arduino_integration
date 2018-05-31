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

#include "weather_animations.h"

float randomFloat(float min, float max) {
	return (max - min) * ((((float) rand()) / (float) RAND_MAX)) + min ;
}

void CloudAnimation::drawCycle(){
	if (this->particles.size() < 10){
		int newParticleCount = random(1,3);
		for (int i = 0; i <newParticleCount; i++){
			this->particles.push_back(Particle(
				{4,4}, // start-position
				{randomFloat(-0.03f, 0.03f),randomFloat(-0.03f, 0.03f)}, // velocity
				(float)(random(75, 100)) // lifetime
			));
		}
	}

	this->matrix->clear();
	this->matrix->disableAutoUpdate();
	for(uint8_t i=0; i< this->particles.size(); i++){
		Particle *current = &this->particles.at(i);
		if(current->isDead() == 0){
			vector2d v = current->getxy();
			uint8_t brightness = 6*current->getLifespan();
			this->matrix->drawPixel(v.x, v.y, rgb(brightness, brightness, brightness));
			current->step();
		}else{
			// he's dead, jimmy!
			this->particles.erase(this->particles.begin()+i);
			i--;
		}
	}
	this->matrix->update();
	this->matrix->enableAutoUpdate();
	this->matrix->clear();
}

Particle::Particle(vector2d location, vector2d velocity, int lifespan) {
	_location = {location.x, location.y};
	_acceleration = {0.0, 0.0};
	_velocity = velocity;
	_lifespan = lifespan;
	_initialLifespan = lifespan;
	_type = 0;
}

Particle::Particle(vector2d location, vector2d velocity, int lifespan, int type) {
	_location = {location.x, location.y};
	_acceleration = {0.0, 0.0};
	_velocity = velocity;
	_lifespan = lifespan;
	_initialLifespan = lifespan;
	_type = type;
}

void Particle::applyAcceleration(vector2d acceleration) {
	_acceleration = {_acceleration.x + acceleration.x, _acceleration.y + acceleration.y};
}

void Particle::step() {
	if (isDead()) return;
	_velocity = {_velocity.x + _acceleration.x, _velocity.y + _acceleration.y};
	_location = {_location.x + _velocity.x, _location.y + _velocity.y};
	_acceleration = {0, 0};
	_lifespan--;
}

uint8_t Particle::isDead() {
	return (_lifespan <= 0) ? 1 : 0;
}

vector2d Particle::getxy() {
	return _location;
}

void Particle::reanimate() {
	_lifespan = _initialLifespan;
}

float Particle::fractionOfLife() {
	return (float)_lifespan / (float)_initialLifespan;
}

int Particle::getType() {
	return _type;
}

int Particle::getLifespan() {
	return _lifespan;
}
