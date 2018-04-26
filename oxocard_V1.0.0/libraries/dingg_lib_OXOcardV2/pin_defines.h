#ifndef _LIB_PIN_DEFINES_H_
#define _LIB_PIN_DEFINES_H_

/* select your hardware version here: */
#define USE_V24

/* ==================== Global module constant declaration ================== */
#ifdef USE_V24
	/* Microphone */
	constexpr uint8_t M_DATA_IN		= 36;
	constexpr uint8_t M_CLK				= 4;

	/* Battery measurement */
	constexpr uint8_t CHARGED			= 39;
	constexpr uint8_t SCAN_BAT		= 34;

	/* Accelerometer */
	constexpr uint8_t ACCEL_INT		= 35;

	/* Buttons */
	constexpr uint8_t BUTTON_L1		= 0;
	constexpr uint8_t BUTTON_L2		= 32;
	constexpr uint8_t BUTTON_L3		= 33;
	constexpr uint8_t BUTTON_R1		= 14;
	constexpr uint8_t BUTTON_R2		= 13;
	constexpr uint8_t BUTTON_R3		= 27;

	/* LEDs */
	constexpr uint8_t LEDS_PIN		= 12;
	constexpr uint8_t EN_LEDS_PIN	= 15;

	/* Audio */
	constexpr uint8_t A_SHUTDOWN	= 2;

	/* GPIOs */
	constexpr uint8_t IO16				= 16;
	constexpr uint8_t IO17				= 17;

	/* UART */
	constexpr uint8_t TXD         = 1;
	constexpr uint8_t RXD         = 3;

#endif

/* ========================= Global macro declaration ======================= */
/* button makros */
#define button1Pressed (!digitalRead(BUTTON_L1))	// L1
#define button2Pressed (digitalRead(BUTTON_L2))		// L2
#define button3Pressed (digitalRead(BUTTON_L3))		// L3
#define button4Pressed (digitalRead(BUTTON_R1))		// R1
#define button5Pressed (digitalRead(BUTTON_R2))		// R2
#define button6Pressed (digitalRead(BUTTON_R3))		// R3

#endif // GLOBALS_H
