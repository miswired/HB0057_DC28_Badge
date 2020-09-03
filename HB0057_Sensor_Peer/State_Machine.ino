

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
      //This is where I plan on setting delays to wait on response
      g_system_state = STATE_WAIT_FOR_COMMAND;
    break;

    case STATE_WAIT_FOR_COMMAND:
      //This is where I plan on waiting for a command
      g_system_state = STATE_SLEEP;
    break;

    case STATE_SLEEP:
      sleep();
    break;
  }
  
}
