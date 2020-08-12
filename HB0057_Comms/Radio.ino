/* setup_radio()
 *  This function is the radio setup
 *  It is placed here just for organization
 *  the setup() function calls this on boot
 */
void setup_radio(void)
{
  /* Set device as a Wi-Fi Station */
  WiFi.mode(WIFI_STA);

  /* Init ESP-NOW */
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  /* Setup the sending callback function */
  esp_now_register_send_cb(OnPacketSent);

  /* Setup peer info
     Here we initalize the container for our peer
     Then copy our peers address
     Set the channel to 0, and set as unencrypted
  */
 
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, sendtoAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  /* Add the peer we just created */
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
  }

  /* Register the receiving callback function */
  esp_now_register_recv_cb(OnPacketReceved);
}




/* service_radio()
 *  This function manages the radio comms
 *  It is responsible for sending pings periodicly
 */
void service_radio(void)
{
  if(g_timer_ping_ms == 0)
  {
    g_timer_ping_ms = TIMER_PING_DELAY_MS;
    g_outgoing_message.sender_id = BOARD_ID_NUMBER;
    g_outgoing_message.command = COMM_CMD_PING;
    g_outgoing_message.current_status = COMM_STATUS_GOOD;

    //Send message
    esp_err_t send_result = esp_now_send(sendtoAddress, (uint8_t *) &g_outgoing_message, sizeof(g_outgoing_message));
  
    if (send_result == ESP_OK) {
      Serial.println("Packet Sent");
      g_timer_ping_timeout_ms = TIMER_PEER_PING_TIMEOUT_MS;
      g_peer_a.ping_sent=STATE_SUCCESS;
    }
    else {
      Serial.println("Sending Failed");
      g_peer_a.ping_sent=STATE_FAILED;
    }

  }
}

/* OnPacketSent()
 *  This is the callback function for when data has been sent
 */
void OnPacketSent(const uint8_t *mac, esp_now_send_status_t packetStatus)
{
  if(packetStatus == ESP_NOW_SEND_SUCCESS)
  {
    Serial.println("Packet received by peer");
    g_peer_a.mac_found = STATE_SUCCESS;
  }
  else
  {
    Serial.println("Packet not received by peer");
    g_peer_a.mac_found = STATE_FAILED;
  }
  
}

/* OnPacketReceived()
 *  This is the callback function for when data is received
 */
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

  /*Verify the right sender, command, status, and the ping
   * timeout did not occur
   */
  if((g_incomming_message.sender_id == 0x0001) &&
     (g_incomming_message.command == COMM_CMD_PING_ACK) &&
     (g_incomming_message.current_status == COMM_STATUS_GOOD)&&
     (g_timer_ping_timeout_ms > 0))
  {
    g_peer_a.response_received = STATE_SUCCESS;
  }
  else
  {
    g_peer_a.response_received = STATE_FAILED;
  }
  
}
