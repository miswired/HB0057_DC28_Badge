/*
Simple Deep Sleep with Timer Wake Up
=====================================
ESP32 offers a deep sleep mode for effective power
saving as power is an important factor for IoT
applications. In this mode CPUs, most of the RAM,
and all the digital peripherals which are clocked
from APB_CLK are powered off. The only parts of
the chip which can still be powered on are:
RTC controller, RTC peripherals ,and RTC memories

This code displays the most basic deep sleep with
a timer to wake it up and how to store data in
RTC memory to use it over reboots

This code is under Public Domain License.

Author:
Pranav Cherukupalli <cherukupallip@gmail.com>
*/

#include <esp_now.h>
#include <WiFi.h>

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */


RTC_DATA_ATTR int bootCount = 0;


typedef struct packet_t {
  uint16_t sender_id;
  uint16_t command;
  uint16_t current_status;
} packet_t;
packet_t g_outgoing_message;
esp_now_peer_info_t peerInfo;
uint8_t sendtoAddress[] = {0x24, 0x62, 0xAB, 0xF1, 0x9E, 0x58}; 

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

//This is the callback function for when data has been sent
void OnPacketSent(const uint8_t *mac, esp_now_send_status_t packetStatus)
{
  if(packetStatus == ESP_NOW_SEND_SUCCESS)
  {
    Serial.println("Packet received by peer");
  }
  else
  {
    Serial.println("Packet not received by peer");
  }
  
}

//This is the callback function for when data is received
void OnPacketReceved(const uint8_t * mac, const uint8_t *packetData, int packetLength)
{
  
}

// This function is the radio setup
// It is placed here just for organization
// the setup() function calls this on boot
void setup_radio(void)
{
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Setup the sending callback function
  esp_now_register_send_cb(OnPacketSent);

  // Setup peer info
  // Here we initalize the container for our peer
  // Then copy our peers address
  // Set the channel to 0, and set as unencrypted
  
  memcpy(peerInfo.peer_addr, sendtoAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add the peer we just created
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
  }

  //Register the receiving callback function
  esp_now_register_recv_cb(OnPacketReceved);
}

void setup(){
  Serial.begin(115200);
  delay(1000); //Take some time to open up the Serial Monitor
  setup_radio();
  
  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 5 seconds
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");

  /*
  Next we decide what all peripherals to shut down/keep on
  By default, ESP32 will automatically power down the peripherals
  not needed by the wakeup source, but if you want to be a poweruser
  this is for you. Read in detail at the API docs
  http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
  Left the line commented as an example of how to configure peripherals.
  The line below turns off all RTC peripherals in deep sleep.
  */
  //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  //Serial.println("Configured all RTC Peripherals to be powered down in sleep");

  /*
  Now that we have setup a wake cause and if needed setup the
  peripherals state in deep sleep, we can now start going to
  deep sleep.
  In the case that no wake up sources were provided but deep
  sleep was started, it will sleep forever unless hardware
  reset occurs.
  */
  radio_ping();
  Serial.println("Going to sleep now");
  Serial.flush(); 
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void radio_ping(void)
{
  /* Build Packet */
  g_outgoing_message.sender_id = 0x01;
  g_outgoing_message.command = 0x02;
  g_outgoing_message.current_status = 0x03;

  /* Send message */
  esp_err_t send_result = esp_now_send(sendtoAddress, (uint8_t *) &g_outgoing_message, sizeof(g_outgoing_message));

  if (send_result == ESP_OK) {
    Serial.println("Packet Sent");
  }
  else {
    Serial.println("Sending Failed");
  }
}

void loop(){
  //This is not going to be called
}
