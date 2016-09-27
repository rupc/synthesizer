#include <pinmap.h>
#include <SoftwareSerial.h>

#define BUF_SIZ 256
#define MAX_VELOCITY 127

#define MUX_MAX 16
#define CHANNEL_SIZ 6
#define FREQ_TIMBRE_SIZ 8

#define TACT_BTN_NUM 8
#define TACT_START_PIN 32
#define TACT_END_PIN 39

#define KEYBOARD_CHANNEL 9
#define DRUM_CHANNEL 10
#define DRUM_MIN_KEY 35
#define DRUM_MAX_KEY 81
#define DRUM_KIT_SIZ 12


void move_led_test();

inline void map_to_joys(uint8_t curr[3], uint16_t analog[3]);
inline void read_many_thing();
inline void read_mux_inputs(uint16_t ins[MUX_MAX]);
inline void print_mux_intpus(uint16_t ins[MUX_MAX]) ;
long modi_map(long x, long in_min, long in_max, long out_min, long out_max);

inline void read_mpr121();
inline void process_touch(bool touch_states[TOUCH_STATE], bool touch_toggled[TOUCH_STATE], uint8_t octave);

void mux_test();
inline void clearDisplay();
inline void setBrightness(byte value);
inline void setDecimals(byte decimals);
inline void blink_led_for_tempo(uint8_t);
inline void led_seq() ;
inline void update_rotary_encoder();

inline void read_gbtns(bool gbtn_inputs[3], bool gbtn_toggles[3]);
inline void process_gbtns(bool gbtn_inputs[3], bool gbtn_toggles[3]);

void read_tacts(bool tact_btns_state[TACT_BTN_NUM], bool tact_btns_toggle[TACT_BTN_NUM]);
void process_tacts(bool tact_btns_state[TACT_BTN_NUM], bool tact_btns_toggle[TACT_BTN_NUM]);

void set_measure_tempo(long const tempo);
inline void set_reverb(uint8_t rev);
inline void set_panport(uint8_t pan);
inline void set_channel_volume(uint8_t vol);

inline void print_lcd(uint8_t line, String s);
inline void print_lcd(uint8_t line, uint8_t cur, String s);
inline void print_lcd(uint8_t line, uint8_t cur, String s, bool clr);
inline void print_lcd(uint8_t line, uint8_t cur, const char *s);
inline void clear_lcd();
void print_seven_segment(uint8_t segment_value);

inline void update_encoders();
inline void print_enc_val();
inline void get_encoder_values();

inline void octave_shift(bool dir);
inline void read_pots();

void prev_timbre(); 
void next_timbre();
void select_next_channel();
void configure_timbre_list();
void add_measure();
void del_measure();
void init_measure_channel();
void record_performer();