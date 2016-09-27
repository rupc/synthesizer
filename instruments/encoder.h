#ifndef __ENCODER_H__
#define __ENCODER_H__

#include <Arduino.h>

class Encoder {
  /*  
    wraps encoder setup and update functions in a class

    !!! NOTE : user must call the encoders update method from an
    interrupt function himself! i.e. user must attach an interrupt to the
    encoder pin A and call the encoder update method from within the 
    interrupt

    uses Arduino pullups on A & B channel outputs
    turning on the pullups saves having to hook up resistors 
    to the A & B channel outputs 

    // ------------------------------------------------------------------------------------------------
    // Example usage :
    // ------------------------------------------------------------------------------------------------
        #include "Encoder.h"

        Encoder encoder(2, 4);

        void setup() { 
            attachInterrupt(0, doEncoder, CHANGE); 
            Serial.begin (115200);
            Serial.println("start");
        } 

        void loop(){
            // do some stuff here - the joy of interrupts is that they take care of themselves
        }

        void doEncoder(){
            encoder.update();
            Serial.println( encoder.getPosition() );
        }    
    // ------------------------------------------------------------------------------------------------
    // Example usage end
    // ------------------------------------------------------------------------------------------------
  */
public:

    // constructor : sets pins as inputs and turns on pullup resistors

    Encoder( int8_t PinA, int8_t PinB, int b, int t) : pin_a ( PinA), pin_b( PinB ), e_bottom(b), e_top(t) {
        // set pin a and b to be input 
        pinMode(pin_a, INPUT); 
        pinMode(pin_b, INPUT); 
        // and turn on pullup resistors
        digitalWrite(pin_a, HIGH);    
        digitalWrite(pin_b, HIGH);       
        curr = prev = LOW;          
        position = b;

    };

    // call this from your interrupt function

    void update () {
        // if (digitalRead(pin_a)) digitalRead(pin_b) ? position++ : position--;
        // else digitalRead(pin_b) ? position-- : position++;
        // need to hack this code to simplify it.

        // curr = digitalRead(pin_a);

        // if(curr == HIGH && prev == LOW) {
        //     if(digitalRead(pin_b) == LOW) {
        //         --position; if(position < e_bottom) position = e_top;
        //     } else {
        //         ++position; if(position > e_top) position = e_bottom;
        //     }
        // }

        if(digitalRead(pin_a)) {
            if(digitalRead(pin_b))  { // a = HIGH, b = HIGH
                curr = 1;
                if(prev == 2) {
                        position++;
                        if(position > e_top) position = e_top;
                    }
            } else { // a = HIGH, b = LOW
                 curr = 2;
            }
        } else {
            if(digitalRead(pin_b)) {
            // a = LOW, b = HIGH
            curr = 3;
        }
            else {
                 // a = LOW, b = LOW
                if(prev == 2) {
                    position--;
                    if(position < e_bottom) position = e_bottom;
                } 
                curr = 4;
            }
        }
        // prev = curr;
        // I'll try this code on machine. on afternoon

        if(prev != curr) {
            prev = curr;
            // Serial.print("/");
            // Serial.println(prev);
        }

    };

    // returns current position

    inline long int getPosition () { return position; };
    inline void set_bottom(int b) {if(b > e_top) return; e_bottom = b; if(position < b) position = b;}
    inline void set_top(int t) {if(t < e_bottom) return;e_top = t; if(position > t) position = t;}
    inline void set_btmNtop(int b, int t) { 
        if(b > t) return;
        e_bottom = t; e_top = t; 
        if(position < b) position = b;
        if(position > t) position = t;
    }
    // set the position value

    inline void setPosition ( const long int p) { position = p; };

private:
    long int position;
    int8_t pin_a;
    int8_t pin_b;
    uint8_t prev;
    uint8_t curr;
    int e_bottom;
    int e_top;
};

#endif // __ENCODER_H__