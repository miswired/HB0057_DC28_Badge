
/* peers_setup()
 *  This function sets up the peer's ID, mac addresses, and
 *  the default command to respond with when a ping is received
 */
void peers_setup(void)
{
  g_peers[0].id = 0;
  //0xFC, 0xF5, 0xC4, 0x0E, 0x3F, 0xC4
  g_peers[0].mac[0] = 0xFC;
  g_peers[0].mac[1] = 0xF5;
  g_peers[0].mac[2] = 0xC4;
  g_peers[0].mac[3] = 0x0E;
  g_peers[0].mac[4] = 0x3F;
  g_peers[0].mac[5] = 0xC4;
  g_peers[0].queued_command = COMM_CMD_SLEEP;

  g_peers[1].id = 1;
  //0x24, 0x62, 0xAB, 0xD5, 0x10, 0xD0
  g_peers[1].mac[0] = 0x24;
  g_peers[1].mac[1] = 0x62;
  g_peers[1].mac[2] = 0xAB;
  g_peers[1].mac[3] = 0xD5;
  g_peers[1].mac[4] = 0x10;
  g_peers[1].mac[5] = 0xD0;
  g_peers[1].queued_command = COMM_CMD_SLEEP;
}

/* peers_time_set()
 *  This function sets the timestamp of a given peer id
 */
void peers_time_set(uint32_t ping_time, uint8_t id)
{
  uint8_t i=0;

  for(i=0; i<NUMBER_OF_PEERS; i++)
  {
    if(g_peers[i].id == id)
    {
      g_peers[i].last_ping_time = ping_time;
    }
  }
}

/* peers_command_get()
 *  This function gets queued command when provided
 *  the ID number. If no matching ID was found, it returns 
 *  0xFF by default which should be interpreted as error
 */
uint8_t peers_command_get(uint8_t id)
{
  uint8_t i=0;
  uint8_t command = 0xFF;

  for(i=0; i<NUMBER_OF_PEERS; i++)
  {
    if(g_peers[i].id == id)
    {
      command = g_peers[i].queued_command;
    }
  }

  return command;
}

/* peers_command_set()
 *  This function sets the next queued command for the
 *  the ID number given. If no matching ID was found, 
 *  nothing happens. I should probably make it return 
 *  a sucess for failure status but I don't feel like it.
 */
void peers_command_set(uint8_t id, uint8_t command)
{
  uint8_t i=0;

  for(i=0; i<NUMBER_OF_PEERS; i++)
  {
    if(g_peers[i].id == id)
    {
      g_peers[i].queued_command = command;
    }
  }
}

/* peer_id_found()
 *  Checks to see if the id is found in the array of peers
 */
uint8_t peer_id_found(uint8_t id)
{
  uint8_t i=0;
  uint8_t peer_found=0;

  for(i=0; i<NUMBER_OF_PEERS; i++)
  {
    if(g_peers[i].id == id)
    {
      peer_found = 1;
    }
  }

  return peer_found;
}


/* peers_command_set()
 *  This function finds the the first index in the array of peers
 *  that matches the given id, and returns that index. If no index
 *  is found it returns 0xFF to indicate an error.
 */
uint8_t peer_id_to_index(uint8_t id)
{
  uint8_t i=0;
  uint8_t peer_index=0xFF;

  for(i=0; i<NUMBER_OF_PEERS; i++)
  {
    if(g_peers[i].id == id)
    {
      peer_index = i;
    }
  }

  return peer_index;
}

/* peers_serivce()
 *  This is the service routine for the peers that 
 *  calculates if the peer is active or not
 */
void peers_serivce(void)
{
  uint8_t i=0;
  uint32_t current_time = 0;
  uint32_t delta = 0;
  
  current_time = millis();

  for(i=0; i<NUMBER_OF_PEERS; i++)
  {
    
    /* Check for wrap-around of timer variable (~49.7 days) */
    if(g_peers[i].last_ping_time <= current_time)
    {
      /* no rollover, subtract times */
      delta = current_time - g_peers[i].last_ping_time;
    }
    else
    {
      /* rolled over, calculate delta with rollover point */
      delta = (4294967295 - g_peers[i].last_ping_time) + current_time;
    }

    if(delta > MAX_PING_TIMEOUT_MS)
    {
      /*It's been too long, set status as inactive*/
      g_peers[i].activity_status=PEER_INACTIVE;
    }
    else
    {
      /*Received a pin in time, update status*/
      g_peers[i].activity_status=PEER_ACTIVE;
    }
  }
}
