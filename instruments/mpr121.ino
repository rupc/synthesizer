/*Copyright (c) 2010 bildr community

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#include "mpr121.h"
#include <Wire.h>


// #define PRINT_TOUCH

void read_touch_inputs(bool touchStates[TOUCH_STATE], bool touch_toggled[TOUCH_STATE], int address){
  if(!checkInterrupt()){
    
    //read the touch state from the MPR121
    Wire.requestFrom(address,2); 
    
    unsigned char LSB = Wire.read();
    unsigned char MSB = Wire.read();
    
    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states

    
    for (int i=0; i < 12; i++){  // Check what electrodes were pressed
      if(touched & (1<<i)){
      
        if(touchStates[i] == 0){
          //pin i was just touched
          #ifdef PRINT_TOUCH
            Serial.print("address : ");
            Serial.print(address);
            Serial.print("pin ");
            Serial.print(i);
            Serial.println(" was just touched");
          #endif

          touch_toggled[i] = true;
        } else if(touchStates[i] == 1){
          //pin i is still being touched
          touch_toggled[i] = false;
        }  
      
        touchStates[i] = 1;      
      } else {
        if(touchStates[i] == 1){
          #ifdef PRINT_TOUCH
            Serial.print("address : ");
            Serial.print(address);
            Serial.print("pin ");
            Serial.print(i);
            Serial.println(" is no longer being touched");
          #endif

          touch_toggled[i] = true;
          //pin i is no longer being touched
        } else {
          touch_toggled[i] = false;
        }
        
        touchStates[i] = 0;
      }
    
    }
    
  }
}




void mpr121_setup(int address){
  // Wire.begin();
  set_register(address, ELE_CFG, 0x00); 
  
  // Section A - Controls filtering when data is > baseline.
  set_register(address, MHD_R, 0x01);
  set_register(address, NHD_R, 0x01);
  set_register(address, NCL_R, 0x00);
  set_register(address, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(address, MHD_F, 0x01);
  set_register(address, NHD_F, 0x01);
  set_register(address, NCL_F, 0xFF);
  set_register(address, FDL_F, 0x02);
  
  // Section C - Sets touch and release thresholds for each electrode
  set_register(address, ELE0_T, TOU_THRESH);
  set_register(address, ELE0_R, REL_THRESH);
 
  set_register(address, ELE1_T, TOU_THRESH);
  set_register(address, ELE1_R, REL_THRESH);
  
  set_register(address, ELE2_T, TOU_THRESH);
  set_register(address, ELE2_R, REL_THRESH);
  
  set_register(address, ELE3_T, TOU_THRESH);
  set_register(address, ELE3_R, REL_THRESH);
  
  set_register(address, ELE4_T, TOU_THRESH);
  set_register(address, ELE4_R, REL_THRESH);
  
  set_register(address, ELE5_T, TOU_THRESH);
  set_register(address, ELE5_R, REL_THRESH);
  
  set_register(address, ELE6_T, TOU_THRESH);
  set_register(address, ELE6_R, REL_THRESH);
  
  set_register(address, ELE7_T, TOU_THRESH);
  set_register(address, ELE7_R, REL_THRESH);
  
  set_register(address, ELE8_T, TOU_THRESH);
  set_register(address, ELE8_R, REL_THRESH);
  
  set_register(address, ELE9_T, TOU_THRESH);
  set_register(address, ELE9_R, REL_THRESH);
  
  set_register(address, ELE10_T, TOU_THRESH);
  set_register(address, ELE10_R, REL_THRESH);
  
  set_register(address, ELE11_T, TOU_THRESH);
  set_register(address, ELE11_R, REL_THRESH);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(address, FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(address, ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  
  // Section F
  // Enable Auto Config and auto Reconfig
  /*set_register(address, ATO_CFG0, 0x0B);
  set_register(address, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(address, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(address, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V
  
  set_register(address, ELE_CFG, 0x0C);
  
}


boolean checkInterrupt(void){
  return digitalRead(irqpin);
}


void set_register(int address, unsigned char r, unsigned char v){
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}