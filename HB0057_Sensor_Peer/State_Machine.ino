

void service_state(void)
{
  switch(g_system_state)
  {
    case STATE_BOOTUP:
      g_system_state = STATE_SEND_PING;
    break;

    case STATE_SEND_PING:
      g_packet_sending = PACKET_BUSY;
      radio_ping_controller();
      g_system_state = STATE_PING_SENDING;
    break;

    case STATE_PING_SENDING:
      if(g_packet_sending == PACKET_DONE)
      {
        g_system_state = STATE_DONE_SENDING;
      }
    break;

    case STATE_DONE_SENDING:
      //Set delay for a command response after the ping was sent
      g_timer_responce_timeout_ms = RESPONSE_TIMEOUT_MS;
      g_system_state = STATE_WAIT_FOR_COMMAND;
    break;

    case STATE_WAIT_FOR_COMMAND:
      //Wait for a timeout or the command response
      if(g_timer_responce_timeout_ms == 0)
      {
        g_system_state = STATE_SLEEP;
      }
    break;

    case STATE_SLEEP:
      sleep();
    break;
  }
  
}
