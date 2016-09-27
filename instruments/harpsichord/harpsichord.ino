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

USING_NAMESPACE_MIDI
// Declare global variables
SoftwareSerial s7s(0, S7S_RX_PIN); 
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7);   
SoftwareSerial MIDI(255, MIDI_PIN); 
MIDI_CREATE_INSTANCE(SoftwareSerial, MIDI, MIDI_BOARD);
char out_buf[BUF_SIZ];
int track_num;

#define TRACK_SIZE 66
bool touch_states[3][TOUCH_STATE] = {false}, touch_toggled[3][TOUCH_STATE] = {false};
// std::map<uint8_t, void (*) ()> melody_performer;

uint16_t analog_mux_inputs[MUX_MAX];

vector<hMeasure<TRACK_SIZE> > noteBank;
hMeasure<TRACK_SIZE> lovers_concerto;
// hNote note[TRACK_SIZE] = {
  // {hG6, HALF, NOTE_ON}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, \
  // {hC6, QUAVER, NOTE_ON}, {hD6, QUAVER, NOTE_ON}, {hE6, QUAVER, NOTE_ON}, {hF6, QUAVER, NOTE_ON}, \
  // {hG6, CROCHET, NOTE_ON}, {0,0,0}, {0,0,0}, {0,0,0}, \
  // {0,0,0}, {0,0,0}, {hG6, CROCHET, NOTE_ON}, {0,0,0} ,
  // {hA6,CROCHET,NOTE_ON}, {0,0,0}, {hG6, CROCHET, NOTE_ON}, {0,0,0},
  // {hF6, QUAVER, NOTE_ON}, {hG6, QUAVER, NOTE_ON},{hA6, QUAVER, NOTE_ON}, {hB6, QUAVER, NOTE_ON},
  // {hC7, CROCHET, NOTE_ON}, {0,0,0},{0,0,0}, {hC6, QUAVER, NOTE_ON},
  // {hC6, HALF, NOTE_ON}, {0,0,0},{0,0,0},{0,0,0},

  // {hF6, HALF, 1}, {0,0,0}, {0,0,0}, {0,0,0},
  // {hG6, QUAVER, 1}, {hF6, QUAVER, 1,},{hE6, QUAVER, 1,},{hD6, QUAVER, 1,},
  // {hE6, HALF, 1}, {0,0,0}, {0,0,0},{0,0,0},
  // {hF6, QUAVER, 1}, {hE6, QUAVER, 1,},{hD6, QUAVER, 1,},{hC6, QUAVER, 1,},
  // {hB5, HALF, 1}, {0,0,0}, {0,0,0}, {0,0,0},
  // {hC6, QUAVER, 1}, {hD6, QUAVER, 1,},{hE6, QUAVER, 1,},{hC6, QUAVER, 1,},
  // {hD6, HALF, 1}, {0,0,0}, {0,0,0}, {0,0,0},
  // {hA6,CROCHET,NOTE_ON}, {0,0,0}, {hG6, CROCHET, NOTE_ON}, {0,0,0},

  //                 };

  uint8_t prev_joys[3] = {0};
  uint8_t curr_joys[3] = {0};
  Timer track_timer;
  Timer seven_segment_timer;
  uint16_t segment_value = 10;
  uint16_t led_pos;
  void print_seven_segment() {
    clearDisplay();
    s7s.print(segment_value);
  }
  bool digital_inputs[MUX_MAX] = {false};
  bool digital_toggles[MUX_MAX] = {false};


  uint8_t encoder1_pos = 1, encoder2_pos = 1;
  bool encoder1_last = LOW, encoder2_last = LOW;
  bool encoder1_n = LOW, encoder2_n = LOW;
  uint8_t option_select;
  uint8_t current_channel = 1;
  uint8_t cur_timbre = 6;
  uint16_t multi_purpose_value;
  uint8_t current_bank, current_timbre;
  uint8_t current_volume;
  uint16_t current_tempo;

// Timer track_timers[MAX_MELODY];
// void (*melody_performer[8])(void);
hNote mommy_sister[TRACK_SIZE] {
  {hF4, CROCHET, 1}, {0,0,0}, {0,0,0}, {0,0,0},
  {hA4, QUAVER, 1}, {0,0,0}, {hG4, QUAVER, 1}, {0,0,0},
  {hA4, CROCHET, 1}, {0,0,0}, {0,0,0}, {0,0,0},
  
  {hG4, QUAVER, 1}, {0,0,0}, {hA4, QUAVER, 1}, {0,0,0},  
  {hE4, CROCHET, 1}, {0,0,0}, {0,0,0}, {0,0,0}, 
  {hE4, CROCHET, 1}, {0,0,0}, {0,0,0}, {0,0,0}, 

  {hD4, CROCHET+2, 1}, {0,0,0}, {0,0,0}, {0,0,0},

  {0,0,0},{0,0,0}, {hE4, QUAVER, 1}, {0,0,0},  
  {hG4, QUAVER, 1},  {0,0,0},  {hD4,QUAVER,1}, {0,0,0}, 

  {hE4, HALF+1, 1} , {0,0,0}, {0,0,0}, {0,0,0},
  {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},
  {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},

  {hA4, CROCHET, 1},{0,0,0}, {0,0,0}, {0,0,0},
  {hC5, QUAVER, 1}, {0,0,0},  {hB4, QUAVER, 1},{0,0,0}, 
  {hA4, CROCHET, 1},{0,0,0}, {0,0,0}, {0,0,0},
  {hD5, QUAVER, 1}, {0,0,0},{hC5, QUAVER, 1}, {0,0,0},

  // {hD5, CROCHET, 1}, {0,0,0},{0,0,0},{0,0,0},
  // {hA4, CROCHET, 1}, {0,0,0},{0,0,0},{0,0,0},
  // {hG4, CROCHET, 1}, {0,0,0},{0,0,0},{0,0,0},
  // {0,0,0},{0,0,0},{hE4, QUAVER, 1}, {0,0,0},

  // {hG4, QUAVER, 1}, {0,0,0}, {hA4, QUAVER, 1}, {0,0,0},
  // {hA4, HALF, 1}, {0,0,0},{0,0,0},{0,0,0},
  // {0,0,0},{0,0,0},{0,0,0},{0,0,0},
  // {0,0,0},{0,0,0},{0,0,0},{0,0,0}

  // {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, 
  // {0,0,0}, {0,0,0}, {0,0,0}, {hA4, CROCHET, 1},
  // {hC4, CROCHET, 1}, {0,0,0}, {0,0,0}, {0,0,0}, 
  // {hB4, CROCHET, 1}, {0,0,0}, {0,0,0}, {0,0,0}
};

Timer lcd_timer;

string lcd_first_line = "Have a good day!";
string lcd_second_line = "none";
void print_lcd_lines() {
  static string f, l; 
  f = l = "";
  if(!f.compare(lcd_first_line)) {
    f = lcd_first_line;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(lcd_first_line.c_str());
    } else if(!f.compare(lcd_second_line)) {
      l = lcd_second_line;
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.write(lcd_second_line.c_str());
    }
  }

  vector<string> option_str = {
    "Bank", "Volume", "Tempo", "Channel"
  };
  vector<string> bank_str = {
    "Piano", "Chromatic", "Organ", "Guitar", 
    "Bass", "String", "Ensemble", "Brass", 
    "Reed", "Pipe ", "Synth Lead", "Synth Pad",
    "Synth Effects", "Ethnic", "Sound Effects1",
    "Sound Effects2" 
  };

  std::map<string, uint8_t> option_finder;
  void set_option_finder() {
    int cnt = 1;
    for(const auto &p : option_str) {
      option_finder[p] = cnt++;
    }
  }

  void setup(){

    wrapper_setups();
    init_lovers_concerto();
  // noteBank[0].print_note_list();
  set_option_finder();

}

void set_track_tempo(long const tempo) {
  track_timer._events[track_num].period = tempo;
}

void loop() {
  // // blink_REV_for_tempo();

  update_rotary_encoder();
  read_many_thing();
  process_joys(prev_joys, curr_joys);

  // read_digital_mux_inputs(digital_inputs, digital_toggles);
  // for(int i = 0; i < MUX_MAX; ++i) {
  //   if(digital_toggles[i]) {
  //     Serial.println(i);
  //   }
  // }
} 
inline void read_options_from_mux() {
  option_select = modi_map(analog_mux_inputs[MUX_OPTION_SELECT], 0, 1023, 1, option_str.size()+1);
  switch(option_select) {
    case SEL_BANK:

    multi_purpose_value = modi_map(analog_mux_inputs[MUX_BANK_SELECT], 0, 1023, 1, 17);
      // Serial.println(bank_str.at(multi_purpose_value-1).c_str());
      sprintf((char *)lcd_second_line.c_str(), "%s", bank_str.at(multi_purpose_value-1).c_str());
      multi_purpose_value = modi_map(analog_mux_inputs[MUX_TIMBRE_SELECT], 0, 1023, 1, 9);
      // segment_value = multi_purpose_value;
      break;
      case SEL_CHAN:
      sprintf((char *)lcd_second_line.c_str(), "%d", current_channel);
      break;
      case SEL_VOLUME:
      sprintf((char *)lcd_second_line.c_str(), "%d", current_volume);
      break;
      case SEL_TEMPO:
      sprintf((char *)lcd_second_line.c_str(), "%d", current_tempo);
      break;
      default:
      break;

    }
    current_bank = modi_map(analog_mux_inputs[MUX_BANK_SELECT], 0, 1023, 1, 17);
    current_timbre = modi_map(analog_mux_inputs[MUX_TIMBRE_SELECT], 0, 1023, 1, 9);
    current_channel = modi_map(analog_mux_inputs[MUX_CHAN_SELECT], 0, 1023, 1, 17);
    current_volume = modi_map(analog_mux_inputs[MUX_MASTER_VOLUME], 0, 1023, 0, 127);
    current_tempo = modi_map(analog_mux_inputs[MUX_BPM], 0, 1023, 50, 500);
    set_track_tempo(current_tempo);
    sprintf((char *)lcd_first_line.c_str(), "%d. %s", option_select, option_str[option_select-1].c_str());

}
inline void locate_led_pos() {
  led_pos = modi_map(analog_mux_inputs[MUX_LED_POS], 0, 1023, 1, 17);
  move_led_pos(led_pos);
  segment_value = led_pos;
}
inline void update_timers() {
  lcd_timer.update();
  seven_segment_timer.update();
  // track_timer.update();
}
inline void map_to_joys(uint8_t curr[3], uint16_t analog[MUX_MAX]) {
  if(analog[MUX_JOY_X] < 100) curr[JOY_X] = X_LEFT;
  else if(analog[MUX_JOY_X] > 800) curr[JOY_X] = X_RIGHT;
  else curr[JOY_X] = X_FLOAT;

  if(analog[MUX_JOY_Y] < 100) curr[JOY_Y] = Y_UP;
  else if(analog[MUX_JOY_Y] > 800) curr[JOY_Y] = Y_DOWN;
  else curr[JOY_Y] = Y_FLOAT;

  if(analog[MUX_JOY_SW] < 100) curr[JOY_SW] = SW_PUSHED;
  else curr[JOY_SW] = SW_FLOAT;
}
inline void event_joystick(uint8_t e) {
  switch(e) {
    case X_RIGHT:
    break;
    case X_LEFT:
    break;
    case X_FLOAT:
    break;
    case Y_UP:
    break;
    case Y_DOWN:
    break;
    case Y_FLOAT:
    break;
    case SW_PUSHED:
    break;
    case SW_FLOAT:
    break;
  }
}

inline void process_joys(uint8_t prev_joys[3], uint8_t curr_joys[3]) {
  if(prev_joys[JOY_X] != curr_joys[JOY_X]) {
    event_joystick(curr_joys[JOY_X]);
    prev_joys[JOY_X] = curr_joys[JOY_X];
  }
  if(prev_joys[JOY_Y] != curr_joys[JOY_Y]) {
    event_joystick(curr_joys[JOY_Y]);
    prev_joys[JOY_Y] = curr_joys[JOY_Y];
  }
  if(prev_joys[JOY_SW] != curr_joys[JOY_SW]) {
    event_joystick(curr_joys[JOY_SW]);
    prev_joys[JOY_SW] = curr_joys[JOY_SW];
  }
}

inline void read_many_thing() {
  for(int i = 0 ; i < 3 ; ++i) {
    read_touch_inputs(touch_states[i], touch_toggled[i], 0x5A + i);
    process_touch(touch_states[i], touch_toggled[i], 5+i);
  }
  read_analog_mux_inputs(analog_mux_inputs);
  map_to_joys(curr_joys, analog_mux_inputs);
}
inline void read_digital_mux_inputs(bool d_ins[MUX_MAX], bool d_toggles[MUX_MAX]) {
 for(uint8_t i = 0 ; i < MUX_MAX ; ++i) {
  digitalWrite(MUX2_S0, i & 0x1);
  digitalWrite(MUX2_S1, i & 0x2);
  digitalWrite(MUX2_S2, i & 0x4);
  digitalWrite(MUX2_S3, i & 0x8);

  if(d_ins[i] != digitalRead(MUX2_OUT)) {
    d_toggles[i] = true;
    } else {
      d_toggles[i] = false;
    }
    d_ins[i] = digitalRead(MUX2_OUT);
  }
}


void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    // Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  // Serial.println(x);         // print the integer
}
inline void init_lovers_concerto() {
  for(int i = 0; i < sizeof(mommy_sister)/ sizeof(hNote); ++i)
  lovers_concerto.add_note_event(mommy_sister[i]);

  // lovers_concerto.print_note_list();
  lovers_concerto.enable_measure();  
  noteBank.push_back(lovers_concerto);

  track_num = track_timer.every(300, measure_performer);

}
// hMelody<4, 1> measureBank;
#define MEL_CHAN 15
void melody_performer() {
  // static auto measure_tracker = measureBank.melody.begin();
  // static auto note_tracker = measureBank.melody.begin()->measure.begin();

  // if(measure_tracker->is_active()) {
  //   if(measure_tracker == measureBank.melody.end()) {
  //     measure_tracker = measureBank.melody.begin();
  //     note_tracker = measure_tracker->measure.begin();
  //     MIDI_BOARD.sendControlChange(AllNotesOff, 127, MEL_CHAN);
  //   } else {
  //     for(auto inNote : *note_tracker) {
  //       if(inNote.on_off == NOTE_ON) {
  //         MIDI_BOARD.sendNoteOn(inNote.pitch, 127, MEL_CHAN);
  //       } else {
  //         MIDI_BOARD.sendNoteOff(inNote.pitch, 127, MEL_CHAN);
  //       }
  //     }
  //     ++note_tracker;
  //     if(note_tracker == measure_tracker->measure.end()) {
  //       note_tracker = (++measure_tracker)->measure.begin();
  //     }
  //   }
  // }
}

void measure_performer() {
  static int cnt = 0;
  for(int mldy_idx = 0 ; mldy_idx < noteBank.size() ; ++mldy_idx) {
    if(noteBank[mldy_idx].is_active()) {
     if(noteBank[mldy_idx].get_current_pos() == noteBank[mldy_idx].get_length()) {
      MIDI_BOARD.sendControlChange(AllNotesOff, 127 ,noteBank[mldy_idx].get_channel());
    }
    vector<hNote> tmp_vs = noteBank[mldy_idx].get_current_notes();
    for(int i = 0 ; i < tmp_vs.size(); ++i) {
     sprintf(out_buf, "<%d, %d, %d>, ", tmp_vs[i].pitch, tmp_vs[i].duration, tmp_vs[i].on_off);
     Serial.print(out_buf);
     if(tmp_vs[i].on_off == NOTE_ON)
       MIDI_BOARD.sendNoteOn(tmp_vs[i].pitch, 127, noteBank[mldy_idx].get_channel());
     else if(tmp_vs[i].on_off == NOTE_OFF && tmp_vs[i].pitch != mldy_idx) {
      MIDI_BOARD.sendNoteOff(tmp_vs[i].pitch, 127, noteBank[mldy_idx].get_channel());
    }
  }
  Serial.println();
  noteBank[mldy_idx].inc_current();
  // blink_led_for_tempo(noteBank[mldy_idx].get_channel());
      // cnt %= sizeof(note) / sizeof(hNote);
    }
  }
}

void track_performer() {

}
inline void read_analog_mux_inputs(uint16_t ins[MUX_MAX])
{
  for(uint8_t i = 0 ; i < MUX_MAX ; ++i) {
    digitalWrite(MUX_S0, i & 0x1);
    digitalWrite(MUX_S1, i & 0x2);
    digitalWrite(MUX_S2, i & 0x4);
    digitalWrite(MUX_S3, i & 0x8);
    ins[i] = analogRead(MUX_OUT);     
  }
}
inline void print_mux_intpus(uint16_t ins[MUX_MAX]) {

  Serial.println();
  for(int i = 0 ; i < MUX_MAX ; ++i) {
    sprintf(out_buf, "%3d ", ins[i]);
    Serial.print(out_buf);
  }
}
