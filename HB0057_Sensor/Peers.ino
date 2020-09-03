
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

  g_peers[1].id = 1;
  //0x24, 0x62, 0xAB, 0xD5, 0x10, 0xD0
  g_peers[1].mac[0] = 0x24;
  g_peers[1].mac[1] = 0x62;
  g_peers[1].mac[2] = 0xAB;
  g_peers[1].mac[3] = 0xD5;
  g_peers[1].mac[4] = 0x10;
  g_peers[1].mac[5] = 0xD0;
  
}

void peers_serivce(void)
{
  
}
