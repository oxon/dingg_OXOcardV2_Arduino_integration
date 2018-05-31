#ifndef LIB_ANIMATION_WEATHER_H_
#define LIB_ANIMATION_WEATHER_H_

#include <esp_event_loop.h>
#include <esp_system.h>
#include <vector>
#include "animations.h"
#include "matrix.h"

float randomFloat(float min, float max);

struct vector2d {
	float x;
	float y;
};

class Particle {
	public:
		Particle(vector2d location, vector2d velocity, int lifespan);
		Particle(vector2d location, vector2d velocity, int lifespan, int type);
		void applyAcceleration(vector2d acceleration);
		void step();
		uint8_t isDead();
		vector2d getxy();
		int getLifespan();
		void reanimate();
		int getType();
		float fractionOfLife();
	private:
		vector2d _location;
		vector2d _velocity;
		vector2d _acceleration;
		int _initialLifespan;
		int _lifespan;
		int _type;
};

class CloudAnimation :public Animation{
	public:
		CloudAnimation(){};
		void drawCycle();

	private:
		uint32_t counter;
		uint32_t stepper;
		std::vector<Particle> particles;
};

#endif
