#ifndef _SOUND_EFFECTS_H_
#define _SOUND_EFFECTS_H_

/* ============================== Global defines ============================ */
extern const uint8_t alarm_wav_start[] asm("_binary_alarm_wav_start");
extern const uint8_t alarm_wav_end[]   asm("_binary_alarm_wav_end");
#define alarm_wav_size	(alarm_wav_end - alarm_wav_start)
#define alarm_wav				alarm_wav_start

extern const uint8_t baby_sneeze_wav_start[] asm("_binary_baby_sneeze_wav_start");
extern const uint8_t baby_sneeze_wav_end[]   asm("_binary_baby_sneeze_wav_end");
#define baby_sneeze_wav_size	(baby_sneeze_wav_end - baby_sneeze_wav_start)
#define baby_sneeze_wav				baby_sneeze_wav_start

extern const uint8_t bee_wav_start[] asm("_binary_bee_wav_start");
extern const uint8_t bee_wav_end[]   asm("_binary_bee_wav_end");
#define bee_wav_size	(bee_wav_end - bee_wav_start)
#define bee_wav				bee_wav_start

extern const uint8_t cannon_wav_start[] asm("_binary_cannon_wav_start");
extern const uint8_t cannon_wav_end[]   asm("_binary_cannon_wav_end");
#define cannon_wav_size	(cannon_wav_end - cannon_wav_start)
#define cannon_wav				cannon_wav_start

extern const uint8_t choo_choo_wav_start[] asm("_binary_choo_choo_wav_start");
extern const uint8_t choo_choo_wav_end[]   asm("_binary_choo_choo_wav_end");
#define choo_choo_wav_size	(choo_choo_wav_end - choo_choo_wav_start)
#define choo_choo_wav				choo_choo_wav_start

extern const uint8_t cling_wav_start[] asm("_binary_cling_wav_start");
extern const uint8_t cling_wav_end[]   asm("_binary_cling_wav_end");
#define cling_wav_size	(cling_wav_end - cling_wav_start)
#define cling_wav				cling_wav_start

extern const uint8_t crash_wav_start[] asm("_binary_crash_wav_start");
extern const uint8_t crash_wav_end[]   asm("_binary_crash_wav_end");
#define crash_wav_size	(crash_wav_end - crash_wav_start)
#define crash_wav				crash_wav_start

extern const uint8_t crickets_wav_start[] asm("_binary_crickets_wav_start");
extern const uint8_t crickets_wav_end[]   asm("_binary_crickets_wav_end");
#define crickets_wav_size	(crickets_wav_end - crickets_wav_start)
#define crickets_wav				crickets_wav_start

extern const uint8_t cuckoo_clock_wav_start[] asm("_binary_cuckoo_clock_wav_start");
extern const uint8_t cuckoo_clock_wav_end[]   asm("_binary_cuckoo_clock_wav_end");
#define cuckoo_clock_wav_size	(cuckoo_clock_wav_end - cuckoo_clock_wav_start)
#define cuckoo_clock_wav				cuckoo_clock_wav_start

extern const uint8_t firecrackers_wav_start[] asm("_binary_firecrackers_wav_start");
extern const uint8_t firecrackers_wav_end[]   asm("_binary_firecrackers_wav_end");
#define firecrackers_wav_size	(firecrackers_wav_end - firecrackers_wav_start)
#define firecrackers_wav				firecrackers_wav_start

extern const uint8_t flute_wav_start[] asm("_binary_flute_wav_start");
extern const uint8_t flute_wav_end[]   asm("_binary_flute_wav_end");
#define flute_wav_size	(flute_wav_end - flute_wav_start)
#define flute_wav				flute_wav_start

extern const uint8_t jungle_wav_start[] asm("_binary_jungle_wav_start");
extern const uint8_t jungle_wav_end[]   asm("_binary_jungle_wav_end");
#define jungle_wav_size	(jungle_wav_end - jungle_wav_start)
#define jungle_wav				jungle_wav_start

extern const uint8_t knife_throw_wav_start[] asm("_binary_knife_throw_wav_start");
extern const uint8_t knife_throw_wav_end[]   asm("_binary_knife_throw_wav_end");
#define knife_throw_wav_size	(knife_throw_wav_end - knife_throw_wav_start)
#define knife_throw_wav				knife_throw_wav_start

extern const uint8_t open_can_wav_start[] asm("_binary_open_can_wav_start");
extern const uint8_t open_can_wav_end[]   asm("_binary_open_can_wav_end");
#define open_can_wav_size	(open_can_wav_end - open_can_wav_start)
#define open_can_wav				open_can_wav_start

extern const uint8_t pew_wav_start[] asm("_binary_pew_wav_start");
extern const uint8_t pew_wav_end[]   asm("_binary_pew_wav_end");
#define pew_wav_size	(pew_wav_end - pew_wav_start)
#define pew_wav				pew_wav_start

extern const uint8_t phone_wav_start[] asm("_binary_phone_wav_start");
extern const uint8_t phone_wav_end[]   asm("_binary_phone_wav_end");
#define phone_wav_size	(phone_wav_end - phone_wav_start)
#define phone_wav				phone_wav_start

extern const uint8_t pling_wav_start[] asm("_binary_pling_wav_start");
extern const uint8_t pling_wav_end[]   asm("_binary_pling_wav_end");
#define pling_wav_size	(pling_wav_end - pling_wav_start)
#define pling_wav				pling_wav_start

extern const uint8_t pool_shot_wav_start[] asm("_binary_pool_shot_wav_start");
extern const uint8_t pool_shot_wav_end[]   asm("_binary_pool_shot_wav_end");
#define pool_shot_wav_size	(pool_shot_wav_end - pool_shot_wav_start)
#define pool_shot_wav				pool_shot_wav_start

extern const uint8_t rifle_wav_start[] asm("_binary_rifle_wav_start");
extern const uint8_t rifle_wav_end[]   asm("_binary_rifle_wav_end");
#define rifle_wav_size	(rifle_wav_end - rifle_wav_start)
#define rifle_wav				rifle_wav_start

extern const uint8_t sirene_wav_start[] asm("_binary_sirene_wav_start");
extern const uint8_t sirene_wav_end[]   asm("_binary_sirene_wav_end");
#define sirene_wav_size	(sirene_wav_end - sirene_wav_start)
#define sirene_wav				sirene_wav_start

extern const uint8_t slap_wav_start[] asm("_binary_slap_wav_start");
extern const uint8_t slap_wav_end[]   asm("_binary_slap_wav_end");
#define slap_wav_size	(slap_wav_end - slap_wav_start)
#define slap_wav				slap_wav_start

extern const uint8_t spin_jump_wav_start[] asm("_binary_spin_jump_wav_start");
extern const uint8_t spin_jump_wav_end[]   asm("_binary_spin_jump_wav_end");
#define spin_jump_wav_size	(spin_jump_wav_end - spin_jump_wav_start)
#define spin_jump_wav				spin_jump_wav_start

extern const uint8_t spurs_wav_start[] asm("_binary_spurs_wav_start");
extern const uint8_t spurs_wav_end[]   asm("_binary_spurs_wav_end");
#define spurs_wav_size	(spurs_wav_end - spurs_wav_start)
#define spurs_wav				spurs_wav_start

extern const uint8_t turkey_wav_start[] asm("_binary_turkey_wav_start");
extern const uint8_t turkey_wav_end[]   asm("_binary_turkey_wav_end");
#define turkey_wav_size	(turkey_wav_end - turkey_wav_start)
#define turkey_wav				turkey_wav_start

extern const uint8_t water_wav_start[] asm("_binary_water_wav_start");
extern const uint8_t water_wav_end[]   asm("_binary_water_wav_end");
#define water_wav_size	(water_wav_end - water_wav_start)
#define water_wav				water_wav_start

#endif
