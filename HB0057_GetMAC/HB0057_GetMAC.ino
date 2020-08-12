/* HB0057 Comms Example
 * Author: Miswired
 * 
 * This sketch Displays the MAC of the ESP32 board.
 * This is handy for setting up the ESP-NOW pairing.
 * 
 * Keep it free, share knowelege. 
 * 
 * This code is released under the Creative Commons
 * Attribution-ShareAlike 4.0 International 
 * (CC BY-SA 4.0)
 * 
 * You are free to:
 *  Share — copy and redistribute the material in any medium or format
 *  Adapt — remix, transform, and build upon the material for any purpose, 
 *          even commercially. 
 *          
 * You must:
 * Attribution — You must give appropriate credit, provide a link to 
 *               the license, and indicate if changes were made. You 
 *               may do so in any reasonable manner, but not in any 
 *               way that suggests the licensor endorses you or your use.    
 * ShareAlike — If you remix, transform, or build upon the material, you 
 *               must distribute your contributions under the same license 
 *               as the original.
 * No additional restrictions — You may not apply legal terms or technological 
 *               measures that legally restrict others from doing anything the 
 *               license permits.
 */


#include <SPI.h>
#include <TFT_eSPI.h> 
#include "WiFi.h"


//Badge Pinouts
#define PIN_BTN_A     21
#define PIN_BTN_B     22
#define PIN_JOY_UP    15
#define PIN_JOY_DOWN  27
#define PIN_JOY_RIGHT 17
#define PIN_JOY_LEFT  12
#define PIN_JOY_PRESS 13 //Center press
#define PIN_BUZZER    32
#define PIN_IR_RX     33 //TSOP4838
#define PIN_IR_TX      2
#define PIN_RCA_VIDEO 25
#define PIN_RCA_AUDIO 26


TFT_eSPI tft = TFT_eSPI(); 

void setup(void) 
{
  pinMode(PIN_BTN_A, INPUT_PULLUP);
  pinMode(PIN_BTN_B, INPUT_PULLUP);
  pinMode(PIN_JOY_UP, INPUT_PULLUP);
  pinMode(PIN_JOY_DOWN, INPUT_PULLUP);
  pinMode(PIN_JOY_RIGHT, INPUT_PULLUP);
  pinMode(PIN_JOY_LEFT, INPUT_PULLUP);
  pinMode(PIN_JOY_PRESS, INPUT_PULLUP);

  //Setup the screen
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  //top left corner of display (0,0) and font 4
  tft.setCursor(0, 0, 4); 
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.println("\nESP32 MAC:\n");
  
  //Setup the radio so we can get the MAC Address
  WiFi.mode(WIFI_MODE_STA);
  tft.println(WiFi.macAddress());
}

void loop() 
{
  //Do nothing here, just show the mac on the screen from setup
}
