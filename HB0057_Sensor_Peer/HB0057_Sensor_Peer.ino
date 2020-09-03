/* HB0057 Comms Peer Example
 * Author: Miswired
 * 
 * Goes into deep sleep
 * Wakes up and pings the main board it's status
 * Goes back to deep sleep
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

#include <esp_now.h>
#include <WiFi.h>


//Global Variables

/* Update these! */
/*===============*/

/* This is this boards ID */
/* Each board should have a unique ID */
#define BOARD_ID_NUMBER   0x0000

/* This is the MAC address of the board you want to connect to */
// When you are done, you can remove the error line below
//#error Update the MAC Address Here
//uint8_t sendtoAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
uint8_t sendtoAddress[] = {0x24, 0x62, 0xAB, 0xF1, 0x9E, 0x58}; 

/*These two lines taken from TimerWakeUp example from the esp32 examples
 * Public Domain License
 * Author: Pranav Cherukupalli <cherukupallip@gmail.com>
 */
/**************************************************************/
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5           /* Time ESP32 will go to sleep (in seconds) */
/**************************************************************/

/*These are common definitions for the communication protocol*/
#define COMM_CMD_PING      0x0001
#define COMM_CMD_PING_ACK  0x0002

#define COMM_STATUS_GOOD   0x00AA
#define COMM_STATUS_BAD    0x0055

#define PING_NOT_RECEIVED   0x55
#define PING_RECEIVED       0xAA

/* This is the data packet structure sent over the radios */
/* Both devices have to have the same packet structure */
typedef struct packet_t {
  uint16_t sender_id;
  uint16_t command;
  uint16_t current_status;
} packet_t;

packet_t g_outgoing_message;
packet_t g_incomming_message;

uint8_t g_ping_received=PING_NOT_RECEIVED;

#define PACKET_DONE 0x01
#define PACKET_BUSY 0x02

volatile uint8_t g_packet_sending = PACKET_DONE;

/* There seems to be a bug in the ESP Now library. Randomly it seems
 * you will get an invalid peer error when the following variable has 
 * local scope in the radio setup as it does in the example. The solution
 * Was to make it a global and it seems to eliminate this bug. No idea why.
 */
esp_now_peer_info_t peerInfo;

/* Timer variables
 * Make sure these are marked volatile!
 */
volatile unsigned long g_timer_clock_current = 0;    //used for tracking time
volatile unsigned long g_timer_clock_previous = 0;
volatile unsigned long g_timer_ping_ms = 0;


#define STATE_BOOTUP            0x00
#define STATE_SEND_PING         0x01
#define STATE_PING_SENDING      0x02
#define STATE_DONE_SENDING      0x03
#define STATE_WAIT_FOR_COMMAND  0x04
#define STATE_SLEEP             0x05
volatile uint8_t g_system_state= STATE_BOOTUP;

/* Function Prototypes */
void timers_service(void);
void radio_setup(void);
void OnPacketReceved(const uint8_t * mac, const uint8_t *packetData, int packetLength);
void OnPacketSent(const uint8_t *mac, esp_now_send_status_t packetStatus);
void radio_ping_controller(void);
void sleep(void);

void sleep(void)
{
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");

  Serial.flush(); 
  esp_deep_sleep_start();
}

void setup(void) 
{
  Serial.begin(115200);
  radio_setup();
}

void loop() 
{
  timers_service();
  service_state();
}
