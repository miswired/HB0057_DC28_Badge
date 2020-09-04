/* setup_screen()
 *  This should be called once on startup to setup the screen
 */
void setup_screen(void)
{
  /* Inital config of screen */
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
}


/* service_screen()
 *  This function manages the screen display
 *  It does this by using a global state variable
 *  and a state machine to choose what to show. 
 *  
 *  I really should implment some kind of check to 
 *  see if the screen needs updating instead of just
 *  blindly writing at the refresh rate. But it seems
 *  as though the tft library being used checks for this
 *  already? I don't see screen flicker when the values are
 *  constant.
 *  
 * Screen formating notes
 * tft.fillScreen(TFT_BLACK); //Clears screen with desired color
 * tft.setCursor(0, 0, 4);    //Moves to x,y,fontsize
 * tft.setTextColor(TFT_RED, TFT_BLACK); //Set font color,background color
 * Color examples:
 * TFT_RED
 * TFT_BLUE
 * TFT_GREEN
 *
 */
void service_screen(void)
{
  uint8_t i=0;

     
  switch(g_screen_state)
  {
    case SCREEN_BOOTUP:
      /*top left corner of display (0,0) and font 4*/
      tft.setCursor(0, 0, 4); 
      tft.setTextColor(TFT_BLUE, TFT_BLACK);
      tft.println("ESP-NOW Test\nBy:Miswired");
      tft.println("This board's MAC:");
      tft.println(WiFi.macAddress());
      g_timer_screen_ms = TIMER_SCREEN_BOOT_MS;
      g_screen_state = SCREEN_BOOT_TIMEOUT;
    break;

    case SCREEN_BOOT_TIMEOUT:
      if(g_timer_screen_ms == 0)
      {
        tft.fillScreen(TFT_BLACK);
        g_screen_state = SCREEN_LOGO;
      }
    break;

    case SCREEN_LOGO:
      tft.pushImage(0, 0,  240, 135, logo);
      g_timer_screen_ms = TIMER_SCREEN_LOGO_MS;
      g_screen_state = SCREEN_LOGO_TIMEOUT;
    break;

    case SCREEN_LOGO_TIMEOUT:
      if(g_timer_screen_ms == 0)
      {
        tft.fillScreen(TFT_BLACK);
        g_screen_state = SCREEN_MAIN;
      }
    break;

    case SCREEN_MAIN:

    for(i=0; i<NUMBER_OF_PEERS; i++)
    {
      if(g_peers[i].activity_status == PEER_ACTIVE)
      {
        tft.setCursor(0, 25*i, 4);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.print("Peer ");
        tft.print(i);
        tft.print(" Peer Alive ");
      }
      else
      {
        tft.setCursor(0, 25*i, 4);
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.print("Peer ");
        tft.print(i);
        tft.print(" Peer Dead  ");
      }
    }
    
/*
      //tft.fillScreen(TFT_BLACK);
      //Serial.print("*");
      if(g_peer_a.ping_sent == STATE_SUCCESS)
      {
        tft.setCursor(0, 0, 4);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.print("Ping--->");
      }

      if(g_peer_a.ping_sent == STATE_FAILED)
      {
        tft.setCursor(0, 0, 4);
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.print("Ping FAIL");
      }
      

      if(g_peer_a.mac_found == STATE_SUCCESS)
      {
        tft.setCursor(100, 0, 4);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.print("RX");
      }

      if(g_peer_a.mac_found == STATE_FAILED)
      {
        tft.setCursor(100, 0, 4);
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.print("RX FAIL");
      }

      if(g_peer_a.response_received == STATE_SUCCESS)
      {
        tft.setCursor(0, 25, 4);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.print("RX<----PONG");
      }

      if(g_peer_a.response_received == STATE_FAILED)
      {
        tft.setCursor(0, 25, 4);
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.print("NO RESPONSE");
      }
      */
    break;
  }
}
