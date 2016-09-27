#include <StandardCplusplus.h>
#include <map>
#include <utility.h>
#include <Event.h>
#include <Timer.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
using namespace std;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "192.168.0.3";
IPAddress ip(192, 168, 0, 177);
EthernetClient client;
int testPin = 7;
int pinVal = 0;
#define CONN_PORT 8000
Timer t;
std::map<const char *, uint8_t> cmd_map;
void set_up_cmds() {
  cmd_map["volume"] = 1;
  cmd_map["channel"] = 2;
  cmd_map["timbre"] = 3;
  cmd_map["tempo"] = 4;
  cmd_map["note"] = 5;
  cmd_map["panport"] = 6;
  cmd_map["reverb"] = 7;
  cmd_map["play_note"] = 8;
  cmd_map["led_on_pos"] = 9;
  cmd_map["led_off_pos"] = 10;
  cmd_map["cur_seq"] = 11;
}
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
#define EVT_CUR_SEQ 11

void setup() {
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
  // t.every(500, doSend);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, CONN_PORT)) {
    Serial.println("connected");
  } else {
    Serial.println("connection failed");
    while(1) {}
  }
  pinMode(testPin, OUTPUT);
  // Wire.begin(8);                // join i2c bus with address #8
  // Wire.onRequest(requestEvent); // register event
  set_up_cmds();
  // jsonexample();

}

void doSend() {
  client.print('a');
}
char readingJsonBuffer[100] = "";
uint8_t bidx = 0;
// String readingJsonBuffer(100);
bool beginJson = false;
bool send_to_mega_flg = false;

void loop() {
  
//
//  x++;
// if there are incoming bytes available
  // from the server, read them and print them:
  if(client.available()) {
    char c = client.read();
    if(c == '{') {
      beginJson = true;
    } 

    if(beginJson) {
      readingJsonBuffer[bidx++] = c;
    } else Serial.print(c);
    if(c == '}') {
      beginJson = false;
      // Serial.println(readingJsonBuffer);
      parsingJson();
      strcpy(readingJsonBuffer, ""); bidx = 0;
    }
  }
  digitalWrite(testPin, pinVal);
}
void requestEvent() {
  Wire.write("hello "); // respond with message of 6 bytes
  // as expected by master
}


void parsingJson() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(readingJsonBuffer);
  if (!root.success()) {
    Serial.println("\nparseObject() failed");
    while(1) {}
  }
  const char* name = root["name"];
  const char* value = root["value"];
  // if(send_to_mega_flg) {

    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(name);
    Wire.write(" ");
    Wire.write(value);
    Wire.write("$");
    Wire.endTransmission();    // stop transmitting
  // }

  Serial.print(name);
  Serial.print(" "); Serial.println(value);
}
void jsonexample() {
  StaticJsonBuffer<200> jsonBuffer;

  char json[] =
      "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

  JsonObject& root = jsonBuffer.parseObject(json);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  const char* sensor = root["sensor"];
  long time = root["time"];
  double latitude = root["data"][0];
  double longitude = root["data"][1];

  Serial.println(sensor);
  Serial.println(time);
  Serial.println(latitude, 6);
  Serial.println(longitude, 6);
}
