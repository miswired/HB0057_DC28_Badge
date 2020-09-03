/* HB0057 Comms Example
 * Author: Miswired
 * 
 * This sketch is intended to be run on the Hackerbox #0057 badge.
 * It implments a simple framework for wireless communciations with
 * other ESP32 chips using the ESP-NOW libraries. 
 * 
 * Todo:
 * There are a lot fo improvments to be made here. Right now this just
 * connects to one board. In the future I plan to make improments to 
 * work with multiple boards and pass mroe information back and fourth.
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
#include <esp_now.h>
#include <WiFi.h>
#include "logo.h"

TFT_eSPI tft = TFT_eSPI(); 

/* Update this Section! */
/*====================================================================*/
/* When you are done, you can remove the error line below */
//#error Update this section!

/* This is this boards ID */
/* Each board should have a unique ID */
#define BOARD_ID_NUMBER   0x0000

/* This is the number of peers in the peers setup function */
#define NUMBER_OF_PEERS   2

/* This is the MAC address of the board you want to connect to */
//uint8_t sendtoAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
//uint8_t sendtoAddress[] = {0xFC, 0xF5, 0xC4, 0x0E, 0x3F, 0xC4}; 

/*======================================================================*/


/* Definitions */
/**************************************/

#define TIMER_PING_DELAY_MS           5000      //Rate to ping boards
#define TIMER_PEER_PING_TIMEOUT_MS    3000      //How long to wait for a ping response

#define TIMER_SCREEN_BOOT_MS          5000      //How long to show boot screen
#define TIMER_SCREEN_LOGO_MS          2000      //How long to show the logo on boot
#define TIMER_SCREEN_REFRESH_RATE_MS  500       //How quickly to refresh LCD

/* Screen state machine states */
#define SCREEN_BOOTUP         0x00
#define SCREEN_BOOT_TIMEOUT   0x01
#define SCREEN_MAIN           0x02
#define SCREEN_LOGO           0x03
#define SCREEN_LOGO_TIMEOUT   0x04

/* Badge hardware pinouts */
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

/* Protocol field deffinitions */
/*  Note, the value doesn't really matter
 *  But if you are wondering what's up 
 *  with the 0x55 and 0xAA, in binary they
 *  are one bit shifted from each other. 
 *  Using multibit success/fails like this
 *  prevents single bit flip errors from 
 *  changing the state of execution. It's 
 *  not likley, and not needed here, just
 *  habit on my part. 
 */
/*=============================*/
#define COMM_CMD_PING      0x0001
#define COMM_CMD_PING_ACK  0x0002

#define COMM_STATUS_GOOD   0x00AA
#define COMM_STATUS_BAD    0x0055

#define LINK_STATUS_GOOD  0xAA
#define LINK_STATUS_BAD   0x55

/* Radio state definitions */
#define STATE_RESET       0x00
#define STATE_FAILED      0xAA
#define STATE_SUCCESS     0x55

/* Global Variables
/**************************************/

/* Create struct to keep track of peers */
typedef struct peer_t {
  uint8_t   id;
  uint8_t   current_status;
  uint32_t  last_ping_time;
  uint8_t   mac[6]; 
} peer_t;

/* Array of peers to keep track of them */
peer_t g_peers[NUMBER_OF_PEERS];


/* Define packet structure
 *  Both devices have to have the same packet structure
 */
typedef struct packet_t {
  uint16_t sender_id;
  uint16_t command;
  uint16_t current_status;
} packet_t;

/* Instantiate an outgoing and incoming instance to hold data */
packet_t g_outgoing_message;
packet_t g_incomming_message;

/* define and initalize the state machine variable */
volatile unsigned int g_screen_state = SCREEN_BOOTUP;

/* Timer variables
 * Make sure these are marked volatile!
 */
volatile uint32_t g_timer_clock_current = 0;    //used for tracking time
volatile uint32_t g_timer_clock_previous = 0;
volatile uint32_t g_timer_ping_ms = 0;
volatile uint32_t g_timer_screen_ms = 0;
volatile uint32_t g_timer_screen_refresh_ms = 0;
volatile uint32_t g_timer_ping_timeout_ms = 0;

/* Counters to keep track of statistics */
uint16_t g_sucess_counter = 0;
uint16_t g_fail_coutner = 0;
uint8_t  g_sucess_ratio = 0;

/* Function Prototypes */
void service_timers(void);
void service_screen(void);
void service_radio(void);
void setup_radio(void);
void setup_screen(void);
void screen_print_stats(void);
void OnPacketSent(const uint8_t *mac, esp_now_send_status_t packetStatus);
void OnPacketReceved(const uint8_t * mac, const uint8_t *packetData, int packetLength);

/* There seems to be a bug in the ESP Now library. Randomly it seems
 * you will get an invalid peer error when the following variable has 
 * local scope in the radio setup as it does in the example. The solution
 * Was to make it a global and it seems to eliminate this bug. No idea why.
 */
esp_now_peer_info_t peerInfo;


/* setup()
 *  Mandatory arduino setup function
 *  Called once on bootup before loop()
 */
void setup(void) 
{
  Serial.begin(115200);
  peers_setup();
  setup_radio();

  pinMode(PIN_BTN_A, INPUT_PULLUP);
  pinMode(PIN_BTN_B, INPUT_PULLUP);
  pinMode(PIN_JOY_UP, INPUT_PULLUP);
  pinMode(PIN_JOY_DOWN, INPUT_PULLUP);
  pinMode(PIN_JOY_RIGHT, INPUT_PULLUP);
  pinMode(PIN_JOY_LEFT, INPUT_PULLUP);
  pinMode(PIN_JOY_PRESS, INPUT_PULLUP);

  setup_screen();
}

/* loop()
 *  This is the "main" function in most c programs
 *  except it loops automatically.
 */
void loop() 
{
  service_timers();
  service_radio();

  /* If screen timer is up, refresh screen */
  if(g_timer_screen_refresh_ms == 0)
  {
    g_timer_screen_refresh_ms = TIMER_SCREEN_REFRESH_RATE_MS;
    service_screen();
  }

}
