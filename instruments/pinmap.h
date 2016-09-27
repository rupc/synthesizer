#define TEMPO_PIN 0
#define VOLUME_PIN 1
#define REVERB_PIN 2
#define PANPORT_PIN 3

#define ON_BOARD_LED 13
#define MIDI_RESET 13
#define MIDI_PIN 12


#define irqpin 2  // Digital 2 for MPR121 irq signal
#define TOUCH_STATE 12
// pin3 is used by MIDI

// game buttons
#define RED_BTN_PIN 14
#define YLW_BTN_PIN 15
#define BLU_BTN_PIN 16

#define COMM_CHECK_PIN 17

#define TACT_CWHI_PIN 32
#define TACT_CBLK_PIN 33
#define TACT_BLU_PIN 34
#define TACT_RED_PIN 35
#define TACT_GRE_PIN 36
#define TACT_WHI_PIN 37
#define TACT_YLW_PIN 38
#define TACT_CBLU_PIN 39



/*						
	LCD 			| RS(4) | EN(6) | DB7(14) | DB6(13) | DB5(12) | DB4(11) |
	Arduino(Mega)   |  11	|  10	|   9	  |  8		|  7	  | 6		|
*/

#define LCD_RS	11
#define LCD_EN	10
#define LCD_DB7	9
#define LCD_DB6	8
#define LCD_DB5	7
#define LCD_DB4	6
// sevent segmnet by serial pin
#define S7S_RX_PIN	5

// shift registor pin assignment
#define SR_SER1 25
#define SR_SER2 26
#define SR_SCK 23
#define SR_RCK 22
#define SR_SCLR 24
	
#define SR_SER3 27
#define SR_SER4 28
#define SR_SCLR2 31
#define SR_RCK2 29
#define SR_SCK2 30

#define SR_SER5 40
#define SR_SER6 41
#define SR_SCLR3 42
#define SR_RCK3 43
#define SR_SCK3 44

// encoder pins 

#define LED_ROTARY_PIN1 46
#define LED_ROTARY_PIN2 47


void setPinMaps();
inline void setup_rotary();
inline void I2C_connection_setup();
inline void lcd_setup() ;
inline void wrapped_setups();
