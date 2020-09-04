/* service_timers()
 *  Calculates the delta between calls and decrements system timers
 *  This is used to provide a non-blocking timer function in arduino. 
 *  Idealy this would be implmented with a timer inturrupt routine but 
 *  this seems to be the best way in this framework
 */
void service_timers(void)
{
  uint32_t delta = 0;
  /* Update time */
  g_timer_clock_current = millis();

  /* Check for wrap-around of timer variable (~49.7 days) */
  if(g_timer_clock_previous <= g_timer_clock_current)
  {
    /* no rollover, subtract times */
    delta = g_timer_clock_current - g_timer_clock_previous;
  }
  else
  {
    /* rolled over, calculate delta with rollover point */
    delta = (4294967295 - g_timer_clock_previous) + g_timer_clock_current;
  }

  /* Record current time for next time */
  g_timer_clock_previous = g_timer_clock_current;

  /* Decrement system counters */
  /*===========================*/
  /* g_timer_ping_ms */
  if(g_timer_ping_ms > delta)
  {
    g_timer_ping_ms = g_timer_ping_ms - delta;
  }
  else
  {
    g_timer_ping_ms = 0;
  }
  
  /* g_timer_screen_ms */
  if(g_timer_screen_ms > delta)
  {
    g_timer_screen_ms = g_timer_screen_ms - delta;
  }
  else
  {
    g_timer_screen_ms = 0;
  }

  /* g_timer_screen_refresh_ms */
  if(g_timer_screen_refresh_ms > delta)
  {
    g_timer_screen_refresh_ms = g_timer_screen_refresh_ms - delta;
  }
  else
  {
    g_timer_screen_refresh_ms = 0;
  }

  /* g_timer_ping_timeout_ms */
  if(g_timer_ping_timeout_ms > delta)
  {
    g_timer_ping_timeout_ms = g_timer_ping_timeout_ms - delta;
  }
  else
  {
    g_timer_ping_timeout_ms = 0;
  }
  
}
