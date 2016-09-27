#include <SPI.h>
#include <SD.h>
#include "encoder.h"
#include <Wire.h>
// #include <SimpleTimer.h>
#include <StandardCplusplus.h>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <map>
#include <memory>

#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <iostream>

#include "./notelist/noteList.h"
#include "./notelist/pitches.h"

#include <midi_Settings.h>
#include <SoftwareSerial.h> 
#include <Event.h>
#include <Timer.h>
#include <LiquidCrystal.h>
#include "mpr121.h"
#include "pinmap.h"
#include "magiclib.h"
#include "playTrack.h"
#include "harpsichord.h"
bool communication_mode = false;
USING_NAMESPACE_MIDI
// Declare global variables
SoftwareSerial s7s(0, S7S_RX_PIN); 
SoftwareSerial MIDI(255, MIDI_PIN); 
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7);   
MIDI_CREATE_INSTANCE(SoftwareSerial, MIDI, MIDI_BOARD);

// declare variables
bool already_play = false;
bool just_pressed = false;
bool is_drum_machine = false;
bool is_change_of_measure_size = false;
int8_t drum_sample_num;
uint8_t drum_note_pos;
uint8_t drum_note_pos_man;
uint8_t drum_row;

uint8_t drum_space[5][16] = {0};
Timer drum_machine_tmr;
int   drum_machine_id;
uint8_t drum_machine_pos;
uint8_t drum_machine_line;
uint8_t drum_machine_timbre[5];
uint8_t drum_bits_sample[4][4][16] = {
  {
    {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
    {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
    {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
    {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}
  },
  {
    {47,0,0,0, 47,0,0,0, 47,0,0,0, 47,0,0,0},
    {0,0,0,0, 39,0,0,0, 0,0,0,0, 39,0,0,0},
    {0,46,46,0, 0,46,46,0, 0,46,46,0, 0,46,46,0},
    {0,0,0,38, 0,0,0,38, 0,0,0,38, 0,0,0,38}
  },

  {
    {0,0,0,0, 0,35,0,0, 0,0,0,0, 0,35,0,0},
    {0,0,0,0, 39,0,0,0, 0,39,0,0, 0,0,0,0},
    {44,0,44,0, 44,0,44,0, 44,0,44,0, 44,0,44,0},
    {0,0,0,0, 0,0,40,0, 0,0,0,0, 40,0,0,0},  
  },

  {
    {47,0,0,0, 47,0,0,0, 0,0,0,0, 0,0,0,0},
    {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,39,0},
    {0,0,0,0, 46,0,46,0, 0,46,0,46, 46,0,46,0},
    {0,0,38,0, 0,0,38,0, 0,0,0,0, 0,0,0,0},  
  },

};

Timer record_tmr;
int   record_id;
int   play_id;
bool  is_record_mode[6] = {false,};
int record_note_pos = 0; 
nMeasure record_measure;
int record_man_pos = 0;
bool  record_manual = false;
uint8_t play_note_pos[6];

uint8_t record_space[6][REC_SIZE];
uint8_t record_temp[REC_SIZE];
int record_sizes[6] = {8,8,8,8,8,8};
int record_pos;
int prev_rec_siz;

char out_buf[BUF_SIZ];
Timer exit_msg_tmr;
int exit_msg_tmr_id;
// indexing timbre list per channel when timbre configure is on
uint8_t configure_timbre = 0;
// predefine channel used. Some channel would be enough.
// drum channel should be treated separately.
uint8_t freq_channels[CHANNEL_SIZ] = {5, 6, 7, 8, KEYBOARD_CHANNEL, DRUM_CHANNEL};
bool channel_activation[6] = {false,};
uint8_t allocated_timbre_list_per_channel[CHANNEL_SIZ][FREQ_TIMBRE_SIZ] = {
  {25, 26, 27, 28, 29, 30, 31, 32},
  {41, 42, 43, 44, 45, 46, 47, 48},
  {57, 58, 59, 60, 61, 62, 63, 64},
  {89, 90, 91, 92, 93, 94, 95, 96}, 
  {0 , 1 , 2 , 3 , 4 , 5 , 6 , 7} // last item is keyboard channel
};
uint8_t prev_key_timbre;
int8_t selected_timbre, timbre_idx, drum_timbre_idx;
// drum instrument list
uint8_t freq_used_drum[DRUM_KIT_SIZ] = {
  35, 38, 43, 46,
  47, 48, 49, 50,
  51, 54, 55, 60
};

nMeasure measures_per_channel[NUM_OF_CHAN][NUM_OF_MEASURE];
// 채널 별 measure 갯수 저장
uint8_t num_of_measure[NUM_OF_CHAN] = {0};

uint8_t freq_channel_idx, freq_timbre_idx;
uint8_t selected_channel = KEYBOARD_CHANNEL;
bool is_timbre_setting_mode = false;
bool isDrum = false;


vector<Encoder> encoders;
int selected_note_pos;
uint8_t just_pressed_key;

bool is_keyboard_off = false;
Timer melody_timer;
Timer measure_timer;
int mea_tmr_id;

uint8_t curr_bank;
uint8_t curr_timbre = curr_bank * 8 - 1;
uint8_t curr_channel = 0;

unsigned long curr_tempo = 500;
uint8_t curr_volume;
uint8_t panport_val;
uint8_t reverb_val;
bool meldoy_start_flag = false;

bool touch_states[3][TOUCH_STATE] = {false}, touch_toggled[3][TOUCH_STATE] = {false};
bool tact_btns_toggle[TACT_BTN_NUM] = {false}, tact_btns_state[TACT_BTN_NUM] = {false};

uint8_t prev_encoder_vals[4] = {false};
bool togg_encoder_vals[4] = {false};

bool gbtn_inputs[3] = {false};
bool gbtn_toggles[3] = {false};
int mel_tmr_id;

bool i2c_flg = false;

bool is_play_mode[6] = {false,};

bool measure_start_flag = false;
uint8_t tim_ctr = 0;
Timer led_timer;

char json_name[16];
char json_value[8];
int json_char_cnt;
void setup(){

  wrapped_setups();

  MIDI_BOARD.sendControlChange(BankSelect, 0x78, DRUM_CHANNEL);
  MIDI_BOARD.sendControlChange(ChannelVolume, 127, DRUM_CHANNEL);
  MIDI_BOARD.sendControlChange(ChannelVolume, 127, KEYBOARD_CHANNEL);
  print_lcd(1,0,"Have a good day!", true);
  print_seven_segment(1111);
  selected_channel = freq_channels[freq_channel_idx];
  Serial.println("Initialzing... done");
  // led_timer.every(100, move_led_test);

}
void loop() {
  if(!communication_mode) {
    read_mpr121();
    // led_timer.update();
    read_tacts(tact_btns_state, tact_btns_toggle);
    process_tacts(tact_btns_state, tact_btns_toggle);

    read_gbtns(gbtn_inputs, gbtn_toggles);
    process_gbtns(gbtn_inputs, gbtn_toggles);

    update_encoders();
    get_encoder_values();
    print_enc_val();

    read_pots();

    // measure_timer.update();
    // exit_msg_tmr.update();

    for(int i = 0 ; i < 6 ; ++i) {
      if(is_record_mode[i] || is_play_mode[i]) {
        record_tmr.update();
        break;
      }
    }
  } else {
    // get control values from the ic2 communication 
    read_gbtns(gbtn_inputs, gbtn_toggles);
    process_gbtns(gbtn_inputs, gbtn_toggles);
  }
} 

int json_nv_flg = false;
int json_int_val;
void receiveEvent(int howMany) {
  if(!communication_mode) {
    Serial.println("attempt to deliver msg through i2c...");
    Serial.println("but it would be enough to be solo mode");
    return;
  }
  while (Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    if(c == ' ') {
      json_nv_flg = true;
      json_char_cnt = 0;
    }
    if(c == '$') {
      json_nv_flg = false;
      json_char_cnt = 0;
      json_int_val = atoi(json_value);
      strncpy(json_value, "\0", strlen(json_value));
      break;
    }
    if(!json_nv_flg) {
      json_name[json_char_cnt++] = c;
    } else {
      json_value[json_char_cnt++] = c;
    }
    Serial.print(c);         // print the character
  }
  Serial.println();
  // sprintf(out_buf, "converted value:%d", json_int_val);
  // Serial.println(out_buf);
  
  Serial.println(out_buf);

  if(strcmp(json_name, "note") == 0) {
    // sprintf(out_buf, "note event:%d", json_value);
    MIDI_BOARD.sendControlChange(AllNotesOff, 127, KEYBOARD_CHANNEL);
    MIDI_BOARD.sendNoteOn((uint8_t)json_int_val, MAX_VELOCITY, KEYBOARD_CHANNEL);
    sprintf(out_buf, "key>%d       ", json_int_val);
  } else if(strcmp(json_name, "timbre") == 0) {
    MIDI_BOARD.sendProgramChange((uint8_t)json_int_val, KEYBOARD_CHANNEL);
    sprintf(out_buf, "timbre>%d     ", json_int_val);
  } else if(strcmp(json_name, "panport") == 0) {
    MIDI_BOARD.sendControlChange(Pan, (uint8_t)json_int_val, KEYBOARD_CHANNEL);
    MIDI_BOARD.sendControlChange(Pan, (uint8_t)json_int_val, DRUM_CHANNEL);
    sprintf(out_buf, "panport>%d     ", json_int_val);
  } else if(strcmp(json_name, "reverb") == 0) {
    MIDI_BOARD.sendControlChange(Effects1, (uint8_t)json_int_val, KEYBOARD_CHANNEL);
    MIDI_BOARD.sendControlChange(Effects1, (uint8_t)json_int_val, DRUM_CHANNEL);
    sprintf(out_buf, "reverb>%d     ", json_int_val);
  } else if(strcmp(json_name, "volume") == 0) {
    MIDI_BOARD.sendControlChange(ChannelVolume, (uint8_t)json_int_val, KEYBOARD_CHANNEL);
    MIDI_BOARD.sendControlChange(ChannelVolume, (uint8_t)json_int_val, DRUM_CHANNEL);
    sprintf(out_buf, "volume>%d     ", json_int_val);
  } else if(strcmp(json_name, "play_note") == 0) {
    if(json_int_val == 1) sprintf(out_buf, "play drum bit");
    else sprintf(out_buf, "stop drum bit");
  } else if(strcmp(json_name, "cur_seq") == 0) {
    for(int i = 0 ; i < 5 ; ++i) {
      if(drum_space[i][(uint8_t)json_int_val-1] != 0)
        MIDI_BOARD.sendNoteOn(drum_space[i][(uint8_t)json_int_val-1], MAX_VELOCITY, DRUM_CHANNEL);
    }
    // ++drum_machine_pos;
    // drum_machine_pos %= 16;
    move_led_pos((uint8_t)json_int_val+1);
    // move_led_by_tempo(false);
  } else if(strcmp(json_name, "drumline") == 0) {
    drum_machine_line = (uint8_t)json_int_val - 1;
    sprintf(out_buf, "drumline>%d  ", json_int_val);
  } else if(strcmp(json_name, "drumtimbre") == 0) {
    MIDI_BOARD.sendNoteOn((uint8_t)json_int_val, MAX_VELOCITY,DRUM_CHANNEL);
    drum_machine_timbre[drum_machine_line] = (uint8_t)json_int_val;
    for(int i = 0 ; i < 16 ; ++i) {
      if(drum_space[drum_machine_line][i] != 0) {
        drum_space[drum_machine_line][i] = (uint8_t)json_int_val;

      } 
    }
    sprintf(out_buf, "drumtimbre>%d  ", json_int_val);
  } else if(strcmp(json_name, "led_on_pos") == 0) {
    uint8_t col = (uint8_t)json_int_val % 16;
    uint8_t row = (uint8_t)json_int_val / 16;
    drum_space[row][col] = drum_machine_timbre[row];
    // sprintf(out_buf,"ledposon:row,col,tim:%d,%d,%d", row,col,drum_machine_timbre[row]);
    // Serial.println(out_buf);
    sprintf(out_buf, "on pos>%d  ", json_int_val);
  } else if(strcmp(json_name, "led_off_pos") == 0) {
    uint8_t col = (uint8_t)json_int_val % 16;
    uint8_t row = (uint8_t)json_int_val / 16;
    drum_space[row][col] = 0;
    sprintf(out_buf, "off pos>%d  ", json_int_val);
  } else if(strcmp(json_name, "all_note_off") == 0) {
    for(int i = 0 ; i < 5 ; ++i) 
      for(int j = 0 ; j < 16 ; ++j) drum_space[i][j] = 0;
    sprintf(out_buf, "all note off");
  } else if(strcmp(json_name, "sync") == 0) {
    print_drum_space();
  } else {
    return;
  }
  // print_lcd(2,0,out_buf,true);
  strncpy(json_name, "\0", strlen(json_name));

  // int x = Wire.read();    // receive byte as an integer
  // Serial.println(x);         // print the integer

}
void print_drum_space() {
  for(int i = 0 ; i < 5 ; ++i) {
    for(int j = 0; j < 16; ++j) {
      Serial.print(drum_space[i][j]);
      Serial.print(", ");
    }
    Serial.println();
  }
}

