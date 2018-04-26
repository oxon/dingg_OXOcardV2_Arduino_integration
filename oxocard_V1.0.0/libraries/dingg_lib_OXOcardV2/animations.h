#ifndef ANIMATION_HELPER_H_
#define ANIMATION_HELPER_H_

#include <esp_event_loop.h>
#include <esp_system.h>
#include "matrix.h"

class Animation{
	public:
		virtual void drawCycle() = 0;
		void setMatrix(Matrix *matrix);
		void setImage(byte bytes[], int size);
		void setImage(byte bytes[]);
		void setRGBVal(rgbColor_t color);

	protected:
		Matrix *matrix;
		byte *image;
		rgbColor_t color;
};

class WaveAnimation: public Animation{
	public:
		WaveAnimation();
		void drawCycle();

	private:
		float currentRow = 0.0;
		float amplification = 2.0;
		int fadeLength = 2;
};

class GlowAnimation: public Animation{
	public:
		GlowAnimation();
		void drawCycle();

	private:
		float currentAmplification = 1.0;
		bool decreasing = false;
		float step = 0.02;
};

class AnimationHelper
{
	public:
		static void startTask(void *animation);
		static void stop();
	private:
		static const uint8_t delayTime=20;
};

#endif
