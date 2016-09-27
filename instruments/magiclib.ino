#include "magiclib.h"

#define MAX_OCTAVE 7
#define MIN_OCTAVE 1
#define DRUM_SIGNATURE 250 
long modi_map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline void read_mpr121() {
  for(int i = 0 ; i < 3 ; ++i) {
    read_touch_inputs(touch_states[i], touch_toggled[i], 0x5C - i);
    if(i == 2)
      process_touch(touch_states[i], touch_toggled[i], DRUM_SIGNATURE);
    else
      process_touch(touch_states[i], touch_toggled[i], base_octave + i * 12);

  }
}
// make sound
inline void process_touch(bool touch_states[TOUCH_STATE],\
                   bool touch_toggled[TOUCH_STATE], \
                   uint8_t start_note) {
  int i;
  uint8_t chan_idx; 
  if(selected_channel == DRUM_CHANNEL) chan_idx = 5;
  else chan_idx = selected_channel - 5;

  for(i = 0 ; i < TOUCH_STATE ; ++i) {
    if(touch_toggled[i] && touch_states[i]) { // when touched
      
      if(start_note == DRUM_SIGNATURE) {
        MIDI_BOARD.sendNoteOn(freq_used_drum[i], MAX_VELOCITY, DRUM_CHANNEL);
        sprintf(out_buf, "drum pad:%d   ", freq_used_drum[i]);
        Serial.println(out_buf);
        print_lcd(2, 0, out_buf);
        just_pressed_key = i;
        if(is_drum_machine) {
            drum_bits_sample[drum_sample_num][drum_row][drum_note_pos] = freq_used_drum[i];
        }
      } else {

        just_pressed_key = i + start_note;
        if(just_pressed_key > 127 || just_pressed_key < 0) {
          sprintf(out_buf, "only 0~127      ");
          print_lcd(1,0, out_buf);
          return;
        }
        if(selected_channel == DRUM_CHANNEL) {
          MIDI_BOARD.sendNoteOn(just_pressed_key, MAX_VELOCITY, DRUM_CHANNEL);
          sprintf(out_buf, "drum key:%d   ", just_pressed_key);
          print_lcd(2, 0, out_buf);
          Serial.println(just_pressed_key);

          if(is_drum_machine) {
            drum_bits_sample[drum_sample_num][drum_row][drum_note_pos] = just_pressed_key;
          }
          return;
        }

        if(selected_channel == DRUM_CHANNEL && is_timbre_setting_mode) {
          freq_used_drum[drum_timbre_idx] = just_pressed_key;
          sprintf(out_buf, "%dth(%d)->%d", drum_timbre_idx, freq_used_drum[drum_timbre_idx], just_pressed_key);
          print_lcd(1, 0, out_buf, true);        
        }
        if(selected_channel != KEYBOARD_CHANNEL &&
           selected_channel != DRUM_CHANNEL) {
          // if selected channel is in 0 to 3 and timbre set mode is active,
          // change the timbre value to new value from keyboard just hitted.
          if(is_timbre_setting_mode) {
            uint8_t tmp = allocated_timbre_list_per_channel[selected_channel-5][configure_timbre];
            allocated_timbre_list_per_channel[selected_channel-5][configure_timbre] = just_pressed_key;
   
            MIDI_BOARD.sendProgramChange(just_pressed_key, KEYBOARD_CHANNEL);
            MIDI_BOARD.sendNoteOn(MIDDLE_C5, MAX_VELOCITY, KEYBOARD_CHANNEL);
            // MIDI_BOARD.sendProgramChange(just_pressed_key, 0);
            sprintf(out_buf, "%d->%d in ch%d", tmp, just_pressed_key, selected_channel);
            print_lcd(2, 0, out_buf);  
            return;
          }

        }
        
 
        
        if(is_record_mode[chan_idx]) {
          if(record_manual) {
            if(selected_channel == DRUM_CHANNEL)
              drum_bits_sample[drum_sample_num][drum_row][drum_note_pos_man] = just_pressed_key;
            record_temp[record_man_pos] = just_pressed_key;
            Serial.println("manual added");
          } else {
            record_temp[record_pos] = just_pressed_key;
            Serial.println("natural added");
          }

          sprintf(out_buf, "key>%d in %dth pos", just_pressed_key,
            record_manual ? record_man_pos+1: record_pos+1);
          Serial.println(out_buf);
          sprintf(out_buf, "%d added at %d   ",just_pressed_key, 
            record_manual ? record_man_pos+1: record_pos+1);
          print_lcd(2, 0, out_buf);

          MIDI_BOARD.sendNoteOn(just_pressed_key, MAX_VELOCITY, selected_channel);
          
        }

        if(!is_keyboard_off)
          MIDI_BOARD.sendNoteOn(i + start_note, MAX_VELOCITY, KEYBOARD_CHANNEL);
        
        sprintf(out_buf, "key:%3d(%3s)", just_pressed_key, note_to_its_name(i+start_note).c_str());
        Serial.println(out_buf);
        print_lcd(2, 0, out_buf);

      }
    } else if(touch_toggled[i] && !touch_states[i]) { // when touched off
      if(start_note == DRUM_SIGNATURE) {
        MIDI_BOARD.sendNoteOff(freq_used_drum[i], MAX_VELOCITY, DRUM_CHANNEL);
      } else if(is_timbre_setting_mode) {
        MIDI_BOARD.sendControlChange(AllNotesOff, 127, KEYBOARD_CHANNEL);
      } else if(is_record_mode[chan_idx]) {
        
        MIDI_BOARD.sendNoteOff(i + start_note, MAX_VELOCITY, selected_channel);
        MIDI_BOARD.sendNoteOff(i + start_note, MAX_VELOCITY, KEYBOARD_CHANNEL);
      } else {
        MIDI_BOARD.sendNoteOff(i + start_note, MAX_VELOCITY, selected_channel);
        MIDI_BOARD.sendNoteOff(i + start_note, MAX_VELOCITY, KEYBOARD_CHANNEL);
      }
    }
  }
}


inline void clearDisplay()
{
  s7s.write(0x76);  // Clear display command
}
inline void setBrightness(byte value)
{
  s7s.write(0x7A);  // Set brightness command byte
  s7s.write(value);  // brightness data byte
}
inline void setDecimals(byte decimals)
{
  s7s.write(0x77);
  s7s.write(decimals);
}

inline void blink_led_for_tempo(bool is_reset) {
  static byte shcmd = 0x01;
  static byte dir = 0;

  if(is_reset) {
    shcmd = 0x01;
    dir = 0;
    return;
  }

  digitalWrite(SR_RCK, LOW);
  if(dir % 2 == 0) 
    shiftOut(SR_SER1, SR_SCK, MSBFIRST, 0xff & shcmd);
  else 
    shiftOut(SR_SER2, SR_SCK, MSBFIRST, 0xff & shcmd);
  digitalWrite(SR_RCK, HIGH);

  if(shcmd == 0x80) {
    shcmd = 0x1;
    dir++; dir %= 2;
  } else 
      shcmd = shcmd << 1; 
}
// Shift Registor 1, 2 : 16 leds according with the callback function by timer.
inline void move_led_by_tempo(bool is_reset) {
  static byte shcmd = 0x01;
  static bool next = false;
  if(is_reset) {
    shcmd = 0x01;
    next = false;
    return;
  }
  digitalWrite(SR_SCLR, LOW);
  digitalWrite(SR_SCLR, HIGH);
  digitalWrite(SR_RCK2, LOW);
  if(next) {
    shiftOut(SR_SER3, SR_SCK2, MSBFIRST, shcmd);
  } else {
    shiftOut(SR_SER4, SR_SCK2, MSBFIRST, shcmd);
  }
  digitalWrite(SR_RCK2, HIGH);
  if(shcmd == 0x80) {
    shcmd = 0x1;
    next = !next;
  } else {
    shcmd <<= 0x1;
  }
}
struct shift_res {
  uint8_t ser;
  uint8_t sck;
  uint8_t rck;
};
void move_led_test() {
  static uint8_t shcmd = 0x01;
  static uint8_t flwr = 0;
  static shift_res shifts[4] = {
    {SR_SER1, SR_SCK , SR_RCK} , {SR_SER2, SR_SCK, SR_RCK},
    {SR_SER4, SR_SCK2, SR_RCK2}, {SR_SER3, SR_SCK2, SR_RCK2},
  };

  digitalWrite(shifts[flwr].rck, LOW);
  shiftOut(shifts[flwr].ser, shifts[flwr].sck, MSBFIRST, shcmd);
  digitalWrite(shifts[flwr].rck, HIGH);

  if(shcmd == 0x80) {
    shcmd = 0x1;
    flwr++; flwr %= 4;
  } else {
    shcmd <<= 0x1;
  }
}
// Shift Registor 3, 4 : 16 leds according with rotary switch encoder. for selecting note position
// receives value range of 1~16
inline void move_led_pos(int16_t pos) {
  digitalWrite(SR_RCK, LOW);
  if(pos > 8) {
    pos -= 8;
    shiftOut(SR_SER1, SR_SCK, MSBFIRST, 0);
    digitalWrite(SR_RCK, HIGH);
    digitalWrite(SR_RCK, LOW);
    shiftOut(SR_SER2, SR_SCK, MSBFIRST, 0x1 << (pos-1));

  } else {
    shiftOut(SR_SER2, SR_SCK, MSBFIRST, 0);
    digitalWrite(SR_RCK, HIGH);
    digitalWrite(SR_RCK, LOW);
    shiftOut(SR_SER1, SR_SCK, MSBFIRST, 0x1 << (pos-1));
  }
  digitalWrite(SR_RCK, HIGH);
}
void read_tacts(bool tact_btns_state[TACT_BTN_NUM], bool tact_btns_toggle[TACT_BTN_NUM]) {
  uint8_t pin_num, i;
  for(i = 0, pin_num = TACT_START_PIN ; pin_num <= TACT_END_PIN ; ++pin_num, ++i) {
    if(tact_btns_state[i] != digitalRead(pin_num)) {
      tact_btns_toggle[i] = true;
      tact_btns_state[i] = digitalRead(pin_num);
    } else tact_btns_toggle[i] = false;
  }
}


void process_tacts(bool tact_btns_state[TACT_BTN_NUM], bool tact_btns_toggle[TACT_BTN_NUM]) {
  for(int i = 0; i < TACT_BTN_NUM ; ++i) {
    if(tact_btns_toggle[i] && tact_btns_state[i]) {
      switch(i) {
        case 0:
          octave_shift(false);
          break;
        case 1:
          octave_shift(true);
          break;
        case 2: // previous timbre in the same bank
          prev_timbre();
          break;
        case 3: // next timbre in the same bank
          next_timbre();
          break;
        case 4: // set timbre list.
          configure_timbre_list();
          break;
        case 5: // record pattern on selected channel
          // add_measure();
          record_pattern();
          break;
        case 6: // delete all measures in selected channel
          init_measure_channel();
          break;
        case 7: // activate selected channel
          activate_channel();
          break;

      }
      sprintf(out_buf, "tact_%d", i);
      Serial.println(out_buf);
    }
  }
}
void activate_channel() {
  uint8_t chan_idx = selected_channel - 5;
  bool is_active;
  if(is_record_mode[chan_idx]) {
    if(record_manual) {
      record_temp[record_man_pos] = 0;

      sprintf(out_buf, "remove pos %d  ", record_man_pos+1);
    } else {
      record_temp[record_pos] = 0;
      sprintf(out_buf, "remove pos %d  ", record_pos+1);
    }
    print_lcd(2,0,out_buf);
    return;
  }
  is_active = channel_activation[chan_idx] = !channel_activation[chan_idx];
  if(is_active) {
    sprintf(out_buf, "ch%d activated", chan_idx);
  } else {
    sprintf(out_buf, "ch%d inactivated", chan_idx);
  }
  print_lcd(1, 0, out_buf, true);

}
void init_measure_channel() {
  uint8_t chan_idx;
  if(selected_channel == DRUM_CHANNEL) chan_idx = 5;
  else chan_idx = selected_channel - 5;

  if(!channel_activation[chan_idx]) {
    sprintf(out_buf, "ch%d>inactive",chan_idx);
    print_lcd(1,0,out_buf, true);
    return;
  }
  if(is_play_mode[chan_idx]) {
    for(int i = 0 ;  i < REC_SIZE; ++i) {
      record_space[chan_idx][i] = 0;
    }
    sprintf(out_buf, "ch%d>init play",chan_idx);
    print_lcd(1,0,out_buf, true);
    return;
  }
  if(is_record_mode[chan_idx]) {
    if(selected_channel == DRUM_CHANNEL) {
      for(int i = 0 ; i < 16 ; ++i)
        drum_bits_sample[drum_sample_num][drum_row][i] = 0;

    }
    for(int i = 0 ; i < sizeof(record_temp) / sizeof(record_temp[0]) ; ++i) {
      record_temp[i] = 0;
    }
    record_pos = 0;
    move_led_by_tempo(true);
    sprintf(out_buf, "ch%d>init record", chan_idx);
    print_lcd(1,0,out_buf, true);
    return;
  }
}
void record_pattern() {
  uint8_t chan_idx = selected_channel - 5;
  if(!channel_activation[chan_idx]) {
    sprintf(out_buf, "ch%d:inactive", chan_idx);
    print_lcd(1, 0, out_buf, true);
    return;
  }
  if(is_play_mode[chan_idx]) {
    sprintf(out_buf, "ch%d:alreadyplay",chan_idx);
    print_lcd(1, 0, out_buf, true);
    return;
  }


  if(!is_record_mode[chan_idx]) {
    print_recording_start();
    if(selected_channel == DRUM_CHANNEL) {

      is_drum_machine = true;
      drum_note_pos = 0;
      sprintf(out_buf, "Drum sequencing  ");
      print_lcd(1, 0, out_buf);
      print_seven_segment(drum_note_pos);
    }

    is_record_mode[chan_idx] = true;
    if(!already_play)
      record_id = record_tmr.every((unsigned long)curr_tempo, record_performer);

    // sprintf(out_buf, "tempo:%d,rec_performer", curr_tempo);
    Serial.println((unsigned long)curr_tempo);
    prev_rec_siz = record_sizes[chan_idx];
    for(int i = 0 ; i < REC_SIZE ; ++i) {
      Serial.print(record_temp[i]);
      Serial.print(", ");
      if(i % 20 == 0)Serial.println();
    }
  } else { 
    
    is_record_mode[chan_idx] = false;
    record_tmr.stop(record_id);
    sprintf(out_buf, "Recorded on ch%d", selected_channel-5);
    print_lcd(1, 0, out_buf);
    Serial.println(out_buf);
    record_pos = 0;
    record_man_pos = 0;
    print_seven_segment(record_pos);
    move_led_by_tempo(true);

    // memcpy(&record_space[chan_idx], record_temp, REC_SIZE);
    for(int i = 0 ; i < REC_SIZE ; ++i) {
      record_space[chan_idx][i] = record_temp[i];
      record_temp[i] = 0;
    }
    if(selected_channel == DRUM_CHANNEL && is_drum_machine) {
      return;
    }
 
  }
}

void record_performer() {
  uint8_t chan, chan_idx;
  chan_idx = selected_channel - 5;
  move_led_by_tempo(false);
  for(int i = 0 ; i < 6 ; ++i) {
    if(channel_activation[i]) {
      chan = freq_channels[i];
      if(chan == DRUM_CHANNEL) {
        for(int j = 0 ; j < 4 ; ++j) {
          MIDI_BOARD.sendNoteOn(
            drum_bits_sample[drum_sample_num][j][drum_note_pos],
            MAX_VELOCITY, DRUM_CHANNEL);
          sprintf(out_buf, "%d, %d, %d, %d",drum_bits_sample[drum_sample_num][j][drum_note_pos], drum_sample_num, j, drum_note_pos);
          Serial.println(out_buf);
        }
        print_seven_segment(drum_note_pos+1);
        ++drum_note_pos; drum_note_pos %= 16;

      } else {
        if(is_record_mode[i]) {
          
          if(is_change_of_measure_size) {
            record_pos = 0;
            is_change_of_measure_size = false;
            move_led_by_tempo(true);
          }
          if(record_pos >= record_sizes[i]) {
            Serial.print("m");
            record_pos = 0;
          } 

          
          print_seven_segment(record_pos+1);
          // sprintf(out_buf, "mpos:%d, npos:%d", record_man_pos, record_pos);
          // Serial.println(out_buf);
          MIDI_BOARD.sendControlChange(AllNotesOff, 127, selected_channel);
          if(record_temp[record_pos] != 0) {
            MIDI_BOARD.sendNoteOn(record_temp[record_pos], 127, selected_channel);
            Serial.println("note");
          }

          ++record_pos;
        }
        for(int i = 0 ; i < 6 ; ++i) {
          if(is_play_mode[i]) {
            uint8_t play_note;
            chan = freq_channels[i];
            play_note = record_space[i][play_note_pos[i]];
            MIDI_BOARD.sendControlChange(AllNotesOff, 127, chan);
            if(play_note != 0) 
              MIDI_BOARD.sendNoteOn(play_note, MAX_VELOCITY, chan);
            play_note_pos[i]++;
            if(play_note_pos[i] >= record_sizes[i]) {
              play_note_pos[i] = 0;
            }
          }
        }

      }

    }
  }
}
inline void update_record_tmr() {
  record_tmr.update();
}
inline void set_record_tempo(long const tempo) {
   record_tmr._events[record_id].period = tempo;
}
inline void print_recording_start() {
    sprintf(out_buf, "Recording(ch%d)", selected_channel-5);
    print_lcd(1, 0, out_buf);
}
// add current to melody and cursor to 

// delete current measure and cursor to the next measure 
// or if currrent is end of melody it points to the end


bool comm_toggle = false;
bool comm_prev = false;
inline void read_gbtns(bool gbtn_inputs[3], bool gbtn_toggles[3]) {
  int j = 0;

  for(int i = 14; j < 3 ; ++j, ++i) {
    if(digitalRead(i) != gbtn_inputs[j]) {
      gbtn_toggles[j] = true;
      gbtn_inputs[j] = digitalRead(i);
    } else gbtn_toggles[j] = false;
  }


  if(comm_prev != (gbtn_inputs[1] && gbtn_inputs[2])) {
    comm_toggle = true;
    comm_prev = gbtn_inputs[1] && gbtn_inputs[2];
  } else {
    comm_toggle = false;
  }
}
inline void process_gbtns(bool gbtn_inputs[3], bool gbtn_toggles[3]) {
  if(!communication_mode) {
    for(int j = 0; j < 3 ; ++j) {
      if(gbtn_toggles[j] && gbtn_inputs[j]) {
        // do something.
        switch (j) {
            case 0: // RED. play current saved measure
              play_measure();
              break;
            case 1: // YLW. stop current activated measure
              stop_measure();
              // do something
              break;
            case 2: // BLU. select channel.
              select_next_channel();
              break;
        }
        sprintf(out_buf, "gbtn%d", j);
        Serial.println(out_buf);
      }
    }
  }
  if(comm_toggle && gbtn_inputs[1] && gbtn_inputs[2]) {
    communication_mode = !communication_mode;
    if(communication_mode) {
      sprintf(out_buf, "comm mode");
      print_lcd(1,0,out_buf,true);
      Serial.println("Communication mode");
      MIDI_BOARD.sendProgramChange(0, KEYBOARD_CHANNEL);
    } else {
      sprintf(out_buf, "solo mode");
      print_lcd(1,0,out_buf, true);
      Serial.println("Solo mode");
    } 
  } 
}
void select_next_channel() {
  // if current channel is being recorded, then
  // stop the recording
  uint8_t chan_idx = selected_channel - 5;
  if(is_record_mode[chan_idx]) {
      sprintf(out_buf, "stop recording  ");
      print_lcd(1,0,out_buf,true);
      return;
      // record_tmr.stop(record_id);
      // is_record_mode[chan_idx] = false;
      // record_pos = 0;
      // move_led_by_tempo(true);
      // sprintf(out_buf, "stop rec(ch%d)", chan_idx);
      // print_lcd(2, 0, out_buf);
      // Serial.println(out_buf);
      // // restore previous size
      // record_sizes[chan_idx] = prev_rec_siz;
  }
  MIDI_BOARD.sendControlChange(AllNotesOff, 127, selected_channel-5);

  timbre_idx = 0;
  ++freq_channel_idx;
  freq_channel_idx %= CHANNEL_SIZ;
  selected_channel = freq_channels[freq_channel_idx];
  
  if(selected_channel == KEYBOARD_CHANNEL) {
    selected_timbre = allocated_timbre_list_per_channel[4][0];
    // sprintf(out_buf, "ch%d>%3d(key)", selected_channel-5, selected_timbre);
    print_lcd(1, 0, "ch4>0(key)", true);
  } else if(selected_channel == DRUM_CHANNEL) {
    // selected_timbre = allocated_timbre_list_per_channel[5][0];
    // sprintf(out_buf, "ch%d>drum", selected_channel-5);
    print_lcd(1, 0, "ch5>drum", true);
  } else {
    selected_timbre = allocated_timbre_list_per_channel[selected_channel-5][0];
    sprintf(out_buf, "ch%d>%3d", selected_channel-5, selected_timbre);
    print_lcd(1, 0, out_buf, true);
  }
  MIDI_BOARD.sendProgramChange(selected_timbre, selected_channel);
  MIDI_BOARD.sendProgramChange(selected_timbre, KEYBOARD_CHANNEL);
  Serial.println(out_buf);
  print_seven_segment(selected_channel-5);

  
}
inline void set_reverb(uint8_t rev) {
  for(int i = 0 ; i < CHANNEL_SIZ ; ++i){
    MIDI_BOARD.sendControlChange(Effects1, rev, freq_channels[i]);
  }
}
inline void set_panport(uint8_t pan) {
  for(int i = 0 ; i < CHANNEL_SIZ ; ++i){
    MIDI_BOARD.sendControlChange(Pan, pan, freq_channels[i]);
  }
}
inline void set_channel_volume(uint8_t vol) {
  for(int i = 0 ; i < CHANNEL_SIZ ; ++i){
    MIDI_BOARD.sendControlChange(ChannelVolume, vol, freq_channels[i]);
  } 
}
// which way gonna fit?
inline void set_master_volume(uint8_t vol) {

}

// String type is convinient to use. because it is more easier to convert int to char.
inline void print_lcd(uint8_t line, String s) {
  lcd.setCursor(0, line - 1);
  lcd.write(s.c_str());
}
inline void print_lcd(uint8_t line, uint8_t cur, String s) {
  lcd.setCursor(cur, line -1);
  lcd.write(s.c_str());
}
inline void print_lcd(uint8_t line, uint8_t cur, const char *s) {
  lcd.setCursor(cur, line-1);
  lcd.write(s);
}
inline void print_lcd(uint8_t line, uint8_t cur, int s) {
  lcd.setCursor(cur, line-1);
  lcd.write(String(s).c_str());
}
inline void print_lcd(uint8_t line, uint8_t cur, String s, bool clr) {
  if(clr) lcd.clear();
  print_lcd(line, cur, s);
}
inline void clear_lcd() {
  lcd.clear();
}
void print_seven_segment(uint8_t segment_value) {
  clearDisplay();
  s7s.print(segment_value);
}
inline void update_encoders() {
  for(auto &p : encoders) {
    p.update();    
  }
}

inline void print_enc_val() {
  uint8_t chan_idx = selected_channel-5;
  // if(togg_encoder_vals[0]) {

  // }  
  for(int i = 0 ; i < sizeof(togg_encoder_vals) / sizeof(bool) ; ++i) {
    if(togg_encoder_vals[i]) {
      sprintf(out_buf, "%dth enc : %d", i+1, prev_encoder_vals[i]);
      Serial.println(out_buf);
      if(is_record_mode[chan_idx]) {
        record_sizes[chan_idx] = prev_encoder_vals[i]*8;
        print_seven_segment(record_sizes[i]);
        sprintf(out_buf, "ch%d>size:%d", chan_idx, record_sizes[chan_idx]);
        Serial.println(out_buf);
        print_lcd(1, 0, out_buf, true);
        is_change_of_measure_size = true;
        move_led_by_tempo(true);
        record_pos = 0;
      }
      
    }
  }
}

inline void get_encoder_values() {
  for(int i = 0 ; i < encoders.size(); ++i) {
    if(prev_encoder_vals[i] != encoders[i].getPosition()) {
      prev_encoder_vals[i] = encoders[i].getPosition();
      togg_encoder_vals[i] = true;
      
    } else {
      togg_encoder_vals[i] = false;
    }

  }

  // selected_note_pos = encoders[0].getPosition();

  // curr_bank = encoders[3].getPosition();
  // move_led_pos(selected_note_pos); 
} 

inline void octave_shift(bool dir) {
  if(dir) { // increase one ocatve
    base_octave += OCTAVE_UNIT;
    if(base_octave > OCTAVE_MAX) {
      base_octave = OCTAVE_MAX;
      print_lcd(1, 0, "Max Ocatve", true);

    }
  } else { // decrease one octave
    base_octave -= OCTAVE_UNIT;
    if(base_octave < OCTAVE_MIN) {
      base_octave = OCTAVE_MIN;
      print_lcd(1, 0, "Min Octave", true);
    }    
  }
  print_seven_segment(base_octave);
}
inline void read_pots() {
  curr_tempo = modi_map(analogRead(TEMPO_PIN), 0, 1023, 50, 1000);
  curr_volume = modi_map(analogRead(VOLUME_PIN),0 ,1023, 0, 127);
  reverb_val = modi_map(analogRead(REVERB_PIN), 0, 1023, 0, 127);
  panport_val = modi_map(analogRead(PANPORT_PIN), 0, 1023, 0, 127);

  // set_measure_tempo(curr_tempo);

  // Serial.println(curr_tempo);
  // sprintf(out_buf, "%d, %d, %d, %d", curr_tempo, curr_volume, reverb_val, panport_val);
  // Serial.println(out_buf);
  
  set_record_tempo(curr_tempo);

  set_channel_volume(curr_volume);
  set_reverb(reverb_val);
  set_panport(panport_val);
}
void prev_timbre() {
  uint8_t chan_idx = selected_channel-5;
  if(selected_channel != DRUM_CHANNEL) {

    if(is_record_mode[chan_idx] && record_manual) {
      record_man_pos--;
      if(record_man_pos < 0) record_man_pos = 0;
      sprintf(out_buf, "pos:%d         ",record_man_pos+1);
      print_lcd(2,0, out_buf);
      move_led_pos(record_man_pos % 16 + 1);
      return;
    }
    --timbre_idx; if(timbre_idx < 0) timbre_idx = 0;
    selected_timbre = allocated_timbre_list_per_channel[chan_idx][timbre_idx];
    if(timbre_idx == 0) {
      sprintf(out_buf, "ch%d>%3d(First) ",chan_idx, selected_timbre);
    } else {
      sprintf(out_buf, "ch%d>timbre:%3d ",chan_idx, selected_timbre);
    }
    print_lcd(1, 0, out_buf);


    if(is_play_mode[chan_idx]) {
      MIDI_BOARD.sendProgramChange(selected_timbre, selected_channel);
      return;
    }

    if(!is_timbre_setting_mode) {
      MIDI_BOARD.sendProgramChange(selected_timbre, selected_channel);
      MIDI_BOARD.sendProgramChange(selected_timbre, KEYBOARD_CHANNEL);
    } else {
      MIDI_BOARD.sendControlChange(AllNotesOff, MAX_VELOCITY, KEYBOARD_CHANNEL);
      --configure_timbre; if(configure_timbre < 0) configure_timbre = 0;

      sprintf(out_buf,"%d->?? in ch%d    ", allocated_timbre_list_per_channel[chan_idx][timbre_idx], selected_channel);
      print_lcd(2, 0, out_buf);
    }
    
  } else if(selected_channel == DRUM_CHANNEL && is_timbre_setting_mode) {
    drum_timbre_idx--; if(drum_timbre_idx < 0) drum_timbre_idx = 0;
    sprintf(out_buf, "%d->?? in drum  ", drum_timbre_idx, freq_used_drum[drum_timbre_idx]);
    print_lcd(1, 0, out_buf);
  } else if(selected_channel == DRUM_CHANNEL && is_drum_machine && !is_timbre_setting_mode) {
    --drum_sample_num; if(drum_sample_num < 0) drum_sample_num = 0;
    drum_note_pos = 0;
    sprintf(out_buf, "drum pattern :%d  ", drum_sample_num);
    print_seven_segment(drum_note_pos);
    print_lcd(1, 0, out_buf);
    move_led_by_tempo(true);
  }
}
void next_timbre() {
  uint8_t chan_idx = selected_channel - 5;
  if(selected_channel != DRUM_CHANNEL) {

    if(is_record_mode[chan_idx] && record_manual) {
      record_man_pos++;
      if(record_man_pos >= record_sizes[chan_idx]) record_man_pos = record_sizes[chan_idx] - 1;
      sprintf(out_buf, "pos:%d         ",record_man_pos+1);
      print_lcd(2,0, out_buf);
      move_led_pos(record_man_pos % 16 + 1);
      return;
    }

    timbre_idx++; if(timbre_idx == FREQ_TIMBRE_SIZ) timbre_idx--;

    selected_timbre = allocated_timbre_list_per_channel[selected_channel-5][timbre_idx];
    if(timbre_idx == FREQ_TIMBRE_SIZ - 1) {
      sprintf(out_buf, "ch%d>%3d(Last)   ",selected_channel-5, selected_timbre);
    } else {
      sprintf(out_buf, "ch%d>timbre:%3d", selected_channel-5, selected_timbre);
    }

    print_lcd(1, 0, out_buf);
    

    if(is_play_mode[chan_idx]) {
      MIDI_BOARD.sendProgramChange(selected_timbre, selected_channel);
      return;
    }
    if(!is_timbre_setting_mode) {
      MIDI_BOARD.sendProgramChange(selected_timbre, selected_channel);
      MIDI_BOARD.sendProgramChange(selected_timbre, KEYBOARD_CHANNEL);
    } else {
      MIDI_BOARD.sendControlChange(AllNotesOff, MAX_VELOCITY, KEYBOARD_CHANNEL);
      ++configure_timbre;
      configure_timbre %= FREQ_TIMBRE_SIZ;
      sprintf(out_buf,"%d->?? in ch%d", allocated_timbre_list_per_channel[selected_channel-5][timbre_idx], selected_channel);
      print_lcd(2, 0, out_buf);
    }
  } else if(selected_channel == DRUM_CHANNEL && is_timbre_setting_mode) {
    drum_timbre_idx++; if(drum_timbre_idx > DRUM_KIT_SIZ) drum_timbre_idx--;
    sprintf(out_buf, "%d->?? in drum   ", drum_timbre_idx, freq_used_drum[drum_timbre_idx]);
    print_lcd(2, 0, out_buf);
  } else if(selected_channel == DRUM_CHANNEL && is_drum_machine && !is_timbre_setting_mode) {
    ++drum_sample_num; drum_sample_num %= 4;
    drum_note_pos = 0;
    print_seven_segment(drum_note_pos);
    sprintf(out_buf, "drum pattern:%d  ", drum_sample_num);
    print_lcd(1, 0, out_buf);
    move_led_by_tempo(true);
  }

}

void configure_timbre_list() {
  if(selected_channel == DRUM_CHANNEL && is_drum_machine) {
    drum_row++; drum_row %= 4;
    sprintf(out_buf, "drum row:%d     ", drum_row);
    print_lcd(1, 0, out_buf);
    return;
  }
  is_timbre_setting_mode = !is_timbre_setting_mode;
  if(selected_channel == KEYBOARD_CHANNEL ||
     selected_channel == DRUM_CHANNEL) {
    sprintf(out_buf, "select ch0-3");
    Serial.println(out_buf);
    print_lcd(1, 0, out_buf, true);
    return;
  }

  if(is_timbre_setting_mode) {
    sprintf(out_buf, "Config ch%d", selected_channel-5);
  } else {
    configure_timbre = drum_timbre_idx = 0;
    sprintf(out_buf, "Exit Config ch%d", selected_channel-5);
    exit_msg_tmr_id = exit_msg_tmr.after(1500, exit_msg_func);
  }
  print_lcd(1, 0, out_buf, true);
}
void exit_msg_func() {
  if(timbre_idx == FREQ_TIMBRE_SIZ - 1) {
    sprintf(out_buf, "ch%d>%3d(Last)   ",selected_channel, selected_timbre);
  } else {
    sprintf(out_buf, "ch%d>%3d         ",selected_channel, selected_timbre);
  }
  if(timbre_idx == 0) {
    sprintf(out_buf, "ch%d>%3d(First) ",selected_channel, selected_timbre);
  } else {
    sprintf(out_buf, "ch%d>%3d        ",selected_channel, selected_timbre);
  }
  print_lcd(1, 0, out_buf);
}

void play_measure() {
  uint8_t chan_idx = selected_channel - 5;

  if(!channel_activation[chan_idx]) {
    sprintf(out_buf, "ch%d:inactive", chan_idx);
    print_lcd(1,0,out_buf, true);
    return;
  }

  if(is_record_mode[chan_idx]) {
    // sprintf(out_buf, "ch%d:recording");
    // print_lcd(1,0,out_buf, true);
    // sprintf(out_buf,"stop rec to play");
    // print_lcd(2,0,out_buf);
    record_manual = !record_manual;
    record_man_pos = 0;
    move_led_pos(1);
    move_led_by_tempo(true);
    if(record_manual) sprintf(out_buf, "record manually");
    else sprintf(out_buf, "record naturally");
    print_lcd(1,0,out_buf,true);
    return;
  }

  if(!is_play_mode[chan_idx]) {
    // play_id = re.every(curr_tempo, measure_performer);
    is_play_mode[chan_idx] = true;
    sprintf(out_buf,"----ch%d is playing----", chan_idx);
    Serial.println(out_buf);
    for(int i = 0 ; i < record_sizes[chan_idx] ; ++i) {
      Serial.print(record_space[chan_idx][i]);
      Serial.print(", ");
      if(i % 8 == 0) Serial.println();
    }
    if(already_play == false) {
      record_id = record_tmr.every(curr_tempo, record_performer);
      already_play = true;
    }

    sprintf(out_buf, "ch%d:playing", chan_idx);
    print_lcd(1,0,out_buf, true);
  } else {
    sprintf(out_buf, "ch%d:alreadyplay",chan_idx);
    print_lcd(1,0,out_buf,true);
  }


}
// If play button is pushed, then stop button will literally stop current played measure
// If not, stop button will shut the every note in every channel
void stop_measure() {
  uint8_t chan_idx = selected_channel - 5;
  MIDI_BOARD.sendControlChange(AllNotesOff, 127, selected_channel);
  if(!channel_activation[chan_idx]) {
    sprintf(out_buf, "ch%d:inactive", chan_idx);
    print_lcd(1,0,out_buf, true);
    return;
  }
  if(is_play_mode[chan_idx]) {
    is_play_mode[chan_idx] = false;
    record_tmr.stop(record_id);
    sprintf(out_buf, "ch%d>stop play", chan_idx);
    print_lcd(1,0,out_buf,true);
  }
  for(int i = 0; i < 6 ; ++i) {
    if(is_play_mode[i]) {
      already_play = true;
      break;
    } else already_play = false;
  }
  if(is_record_mode[chan_idx]) {
    is_record_mode[chan_idx] = false;
    record_sizes[chan_idx] = prev_rec_siz;
    record_tmr.stop(record_id);
    sprintf(out_buf, "ch%d:recording", chan_idx);
    print_lcd(1,0,out_buf, true);
    sprintf(out_buf,"not saved");
    print_lcd(2,0,out_buf);
    return;
  }
}