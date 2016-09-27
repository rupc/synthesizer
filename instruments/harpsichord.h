#define REC_SIZE 128

#define MIN_NOTE 12
#define MAX_NOTE 119

#define SEL_BANK 1
#define SEL_VOLUME 2
#define SEL_TEMPO 3
#define SEL_CHAN 4

#define MIDDLE_C5 60

#define OCTAVE_START_1 36
#define OCTAVE_START_2 48
#define OCTAVE_START_3 60
#define OCTAVE_START_4 70
#define OCTAVE_START_5 84
#define OCTAVE_START_6 96
#define OCTAVE_START_7 108

#define OCTAVE_MIN 0
#define OCTAVE_MAX 108

#define OCTAVE_UNIT 12

#define NUM_OF_CHAN 6
#define NUM_OF_MEASURE 8

#define EVT_VOLUME 1
#define EVT_CHANNEL 2
#define EVT_TIMBRE 3
#define EVT_TEMPO 4
#define EVT_NOTE 5
#define EVT_PANPORT 6
#define EVT_REVERB 7
#define EVT_PLAY_NOTE 8
#define EVT_LED_ON_POS 9
#define EVT_LED_OFF_POS 10
#define CUR_SEQ 11

int base_octave = MIDDLE_C5;

// why do i write this wasteful trash?
inline void map_to_joys(uint8_t curr[3], uint16_t analog[MUX_MAX]);
inline void process_joys(uint8_t prev_joys[3], uint8_t curr_joys[3]);
inline void print_mux_intpus(uint16_t ins[MUX_MAX]) ;
inline void read_mux_inputs(uint16_t ins[MUX_MAX]);
inline void map_to_joys(uint8_t curr[3], uint16_t analog[MUX_MAX]);
inline void process_joys(uint8_t prev_joys[3], uint8_t curr_joys[3]);

void track_performer();

