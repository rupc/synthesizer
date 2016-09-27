#include "pinmap.h"
File timbre_list_file;
void sd_card_setup() {
  pinMode(53, OUTPUT);
  Serial.print("Initializing SD card...");
   // SD카드를 사용하기 위해서는 꼭 10번 핀을 출력모드로 설정해야 한다.
   // 아두이노 mega를 쓰시는분은 53번핀으로 바꿔준다.
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return; 
    } else {
      Serial.println("Initialized SD card successfully");
  }

}

String read_timbre_name_fromSD(uint8_t tim_num) {
  char ch;
  timbre_list_file = SD.open("timbre_list.txt");

  if(timbre_list_file) {
    Serial.println("Open timbre_list.txt");
    while(timbre_list_file.available()) {

      
      if(tim_num == 1) {
        String line = timbre_list_file.readStringUntil('\n');
        strtok((char *)line.c_str(), " ");
        return String(strtok((char *)line.c_str(), " "));
      }
      timbre_list_file.readStringUntil('\n');
      tim_num--;
    }
  } else {
    Serial.println("timbre list file doesn't exist\nplz check it out!");
    return "";
  }
}

void write_measure_toSD(const list<nMeasure> &mea) {

}
void read_measrue_fromSD(uint8_t num, list<nMeasure> &mea) {

}
void write_melody_toSD(const uint8_t num, const list<nMelody> &mel) {

}
void write_melody_toSD(char *m_name, const list<nMelody> &mel) {
  
}
#define I2C_UNO_ADRS 8
inline void I2C_connection_setup() {
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Serial.println("Enter I2C address as 8");
  mpr121_setup(0x5A); 
  Serial.println("MPR121:0x5A");
  mpr121_setup(0x5B); 
  Serial.println("MPR121:0x5B");
  mpr121_setup(0x5C); 
  Serial.println("MPR121:0x5C");
  Serial.println("I2C Set up successfully");
}
inline void get_from_uni() {
  Wire.requestFrom(I2C_UNO_ADRS, 2); 
}
inline void lcd_setup() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.write("lcd setup done");
}
inline void seven_segment_setup() {
  s7s.begin(9600);
  setBrightness(100);
  clearDisplay();
  s7s.write(1);

}

inline void wrapped_setups() {
  // MIDI communication
  MIDI_BOARD.begin();

  // Serial communication
  Serial.begin(9600); 
  Serial.println("Serial Initialzing...");

  // Initialize Liquid Crystal LCD 
  lcd_setup();

  Serial.println("LCD Initialzing...");
  // Initialze 7 Segment for Serial Communication
  seven_segment_setup();
  Serial.println("7Segment Initialzing...");
  /*
  set custom pin mapping
  Shift registor, Decoder
  */
  setPinMaps();
  Serial.println("Pin Mode Setting...");

  // sd_card_setup();
  setup_encoders();
  Serial.println("Rotary Encoder Switch Setting...");

    // I2C capacitive touch sensor
  I2C_connection_setup();

  Serial.println("I2C Initialzing...");

}
inline void setup_encoders() {
  // set up encoder moving led position 

  encoders.push_back(Encoder(LED_ROTARY_PIN1, LED_ROTARY_PIN2, 1, 16));
  // encoders.push_back(Encoder(REV_ROTARY_PIN1, REV_ROTARY_PIN2, 1, 16));


}
void setPinMaps() {
	pinMode(ON_BOARD_LED, OUTPUT);
	pinMode(MIDI_RESET, OUTPUT);
	digitalWrite(MIDI_RESET, HIGH);

  pinMode(TEMPO_PIN, INPUT);
  pinMode(VOLUME_PIN, INPUT);
  pinMode(REVERB_PIN, INPUT);
  pinMode(PANPORT_PIN, INPUT);

  pinMode(COMM_CHECK_PIN, INPUT);

  pinMode(BLU_BTN_PIN, INPUT);
  pinMode(YLW_BTN_PIN, INPUT);
  pinMode(RED_BTN_PIN, INPUT);

  pinMode(TACT_CWHI_PIN, INPUT);
  pinMode(TACT_CBLK_PIN, INPUT);
  pinMode(TACT_BLU_PIN, INPUT);
  pinMode(TACT_RED_PIN, INPUT);
  pinMode(TACT_GRE_PIN, INPUT);
  pinMode(TACT_WHI_PIN, INPUT);

  pinMode(TACT_YLW_PIN , INPUT);
  pinMode(TACT_CBLU_PIN, INPUT);

	pinMode(SR_SER1, OUTPUT);
	pinMode(SR_RCK, OUTPUT);
	pinMode(SR_SCLR, OUTPUT);
	pinMode(SR_SCK, OUTPUT);
  pinMode(SR_SER2, OUTPUT);
	digitalWrite(SR_SCLR, LOW);
	digitalWrite(SR_SCLR, HIGH);

  pinMode(SR_SER3, OUTPUT);
  pinMode(SR_SER4, OUTPUT);
  pinMode(SR_RCK2, OUTPUT); 
  pinMode(SR_SCK2, OUTPUT); 
  pinMode(SR_SCLR2, OUTPUT);
  digitalWrite(SR_SCLR2, LOW);
  digitalWrite(SR_SCLR2, HIGH);


  digitalWrite(SR_SCLR, HIGH);
  digitalWrite(SR_SCLR2, HIGH);

  digitalWrite(SR_RCK, LOW);
  shiftOut(SR_SER1, SR_SCK, MSBFIRST, 0);
  digitalWrite(SR_RCK, HIGH);
  digitalWrite(SR_RCK, LOW);
  shiftOut(SR_SER2, SR_SCK, MSBFIRST, 0);
  digitalWrite(SR_RCK, HIGH);

  digitalWrite(SR_RCK2, LOW);
  shiftOut(SR_SER3, SR_SCK2, MSBFIRST, 0);
  digitalWrite(SR_RCK2, HIGH);
  digitalWrite(SR_RCK2, LOW);
  shiftOut(SR_SER4, SR_SCK2, MSBFIRST, 0);
  digitalWrite(SR_RCK2, HIGH);
}

