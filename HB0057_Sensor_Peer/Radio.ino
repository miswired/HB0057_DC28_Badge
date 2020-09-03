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
  g_packet_sending = PACKET_DONE;
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
void radio_setup(void)
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

void radio_ping_controller(void)
{
  /* Build Packet */
  g_outgoing_message.sender_id = BOARD_ID_NUMBER;
  g_outgoing_message.command = COMM_CMD_PING;
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
