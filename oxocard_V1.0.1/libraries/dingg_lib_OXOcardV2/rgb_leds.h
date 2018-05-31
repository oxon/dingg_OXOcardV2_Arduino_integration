#ifndef _RGBLEDS_H_
#define _RGBLEDS_H_

#include <driver/rmt.h>
#include <vector>
#include "rgb_leds_timing.h"

typedef union {
  struct __attribute__ ((packed)) {
    uint8_t r, g, b, w;
  };
  uint32_t num;
} rgbColor_t;

inline rgbColor_t rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0) {
	rgbColor_t c = {r, g, b, w};
	return c;
}

class RGBLeds
{
public:
		RGBLeds(rmt_channel_t channel, gpio_num_t io_pin, uint16_t led_count,
			const RGBLedsTiming& timing);
		virtual ~RGBLeds();

		void set_pixel(uint16_t ix, uint8_t red, uint8_t green, uint8_t blue);
		void clear();
		void apply();

private:
		static constexpr double pulse_width = 12.5; // ns, i.e. 1 / 80MHz

		static constexpr uint16_t t0h = 400; // ns
		static constexpr uint16_t t0l = 850; // ns
		static constexpr uint16_t t1h = 800; // ns
		static constexpr uint16_t t1l = 450; // ns
		static constexpr uint16_t reset_time = 55000; // ns = 50us

		static constexpr uint8_t clock_divider = 4;
		static constexpr uint8_t bits_per_pixel = 24;

		uint16_t get_rmt_duration(uint16_t duration_in_ns);
		void add_color(std::vector<rmt_item32_t>::iterator& pixel, uint8_t color);

		std::vector<rmt_item32_t> led_data;
		rmt_channel_t channel;
		uint16_t led_count;
		rmt_item32_t hi;
		rmt_item32_t low;
		rmt_item32_t reset;
};

#include <bitset>
class ByteSet : public std::bitset<8>
{
public:
	ByteSet(uint8_t value) : std::bitset<8>(value) {}

	operator uint8_t() const {return static_cast<uint8_t>(to_ulong());}

	/// Gets the value of the specified bits aligned to LSB.
	/// \param lsb Least Significant Bit, >= 0
	/// \param msb Most Significant Bit <= 7
	/// \return The resulting value
	uint8_t get_bits_as_byte(uint8_t lsb, uint8_t msb)
	{
		uint8_t res = 0;
		int ix = 0;
		for (uint8_t i = lsb; i <= msb; ++i)
		{
			if (test(i)) res |= 1 << ix;
			++ix;
		}
		return res;
	}
};

#endif
