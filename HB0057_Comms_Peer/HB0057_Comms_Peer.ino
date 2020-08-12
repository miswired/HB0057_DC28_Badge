/* HB0057 Comms Peer Example
 * Author: Miswired
 * 
 * Waits for a ping from the main board and responds when it's received.
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
#define BOARD_ID_NUMBER   0x0001

/* This is the MAC address of the board you want to connect to */
// When you are done, you can remove the error line below
//#error Update the MAC Address Here
//uint8_t sendtoAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
uint8_t sendtoAddress[] = {0x24, 0x62, 0xAB, 0xF1, 0x9E, 0x58}; 

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
  //Notice we are copying based on the size of storrage data type, NOT the sending packet
  memcpy(&g_incomming_message, packetData, sizeof(g_incomming_message));

  Serial.println("Data Received!");
  Serial.print("Bytes:");
  Serial.println(packetLength);
  Serial.print("sender_id=");
  Serial.println(g_incomming_message.sender_id);
  Serial.print("command=");
  Serial.println(g_incomming_message.command);
  Serial.print("current_status=");
  Serial.println(g_incomming_message.current_status);

  if((g_incomming_message.sender_id == 0x0000) &&
     (g_incomming_message.command == COMM_CMD_PING))
  {
    g_ping_received = PING_RECEIVED;
    Serial.println("Ping Received!");
  }
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
  esp_now_peer_info_t peerInfo;
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

void setup(void) 
{
  Serial.begin(115200);
  /*  Be sure this setup happens first thing at boot
   *  I had errors if there was a delay like setting
   *  up the screen and printing first.
   */
  setup_radio();

}

void loop() 
{
  if(g_ping_received == PING_RECEIVED)
  {
    /* Reset ping status */
    g_ping_received = PING_NOT_RECEIVED;

    /* Build Packet */
    g_outgoing_message.sender_id = BOARD_ID_NUMBER;
    g_outgoing_message.command = COMM_CMD_PING_ACK;
    g_outgoing_message.current_status = COMM_STATUS_GOOD;
  
    /* Send message */
    esp_err_t send_result = esp_now_send(sendtoAddress, (uint8_t *) &g_outgoing_message, sizeof(g_outgoing_message));
  
    if (send_result == ESP_OK) {
      Serial.println("Packet Sent");
    }
    else {
      Serial.println("Sending Failed");
    }
  }
}
