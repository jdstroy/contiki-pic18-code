/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include <pic18fregs.h>
#include <signal.h>

#include "pic18f.h"

#include "sys/energest.h"
#include "sys/clock.h"
#include "sys/etimer.h"

#include "dev/leds.h"

#define INTERVAL (RTIMER_ARCH_SECOND / CLOCK_SECOND)
#undef INTERVAL
#define INTERVAL ((PIC18F_CPU_SPEED / (256 * 4)) / CLOCK_SECOND)

static volatile unsigned long seconds;
static volatile clock_time_t count = 0;

/*---------------------------------------------------------------------------*/
clock_time_t
clock_time(void)
{
  clock_time_t got_count;
  do {
    got_count = count;
  } while (got_count != count);

  /* return tick counter */
  return count;
}
/*---------------------------------------------------------------------------*/
void
clock_init(void)
{
  di();
  /* Setup Timer 1 as 32768Hz Xtal */
  T1CONbits.RD16 = 1;
  T1CONbits.T1CKPS1 = 1;
  T1CONbits.T1CKPS0 = 1;
  T1CONbits.T1OSCEN = 1;
  T1CONbits.T1SYNC = 1; /* async mode */
  T1CONbits.TMR1CS = 1;
#if 0
  TMR1H = (uint8_t)((0xffff - INTERVAL) >> 8);
  TMR1L = (uint8_t)((0xffff - INTERVAL) & 0xff);
  T1CONbits.TMR1ON = 1;
  PIR1bits.TMR1IF = 0;
  PIE1bits.TMR1IE = 1;
  INTCONbits.PEIE = 1;
#else
  T0CON = 0x07; /* disable TMR0, 16bit, prescaler 1:256,  */
  TMR0H = (uint8_t)((0xffff - INTERVAL) >> 8);
  TMR0L = (uint8_t)((0xffff - INTERVAL) & 0xff);
  T0CONbits.TMR0ON = 1;
  INTCON |= 0x60; /* PEIE, TMR0IE */
#endif 
  /* Enable interrupts. */
  ei();
}

/*---------------------------------------------------------------------------*/
void
clock_delay(unsigned int len)
{
  if(len) {
    while(--len) {
    }
  }
}
/*---------------------------------------------------------------------------*/
unsigned long
clock_seconds(void)
{
  static unsigned long t1;
  do {
    t1 = seconds;
  } while(t1 != seconds);
  return t1;
}
/*---------------------------------------------------------------------------*/
SIGHANDLER(tick_it)
{
  /* Preset */
#if 0
  T1CONbits.TMR1ON = 0;
  TMR1H = (uint8_t)((0xffff - INTERVAL) >> 8);
  TMR1L = (uint8_t)((0xffff - INTERVAL) & 0xff);
  T1CONbits.TMR1ON = 1;
  /* clear flag */
  PIR1bits.TMR1IF = 0;
#else
//  T0CONbits.TMR0ON = 0;
  TMR0H = (uint8_t)((0xffff - INTERVAL) >> 8);
  TMR0L = (uint8_t)((0xffff - INTERVAL) & 0xff); //  + TMR0L;
//  T0CONbits.TMR0ON = 1;
  /* clear flag */
  INTCONbits.TMR0IF = 0;
#endif  

#ifdef DEBUG_TOGGLE_TICK
  leds_toggle(LEDS_GREEN);
#endif

  /* increase tick timer */
  ++count;

  /* Make sure the CLOCK_CONF_SECOND is a power of two, to ensure
   that the modulo operation below becomes a logical and and not
   an expensive divide. Algorithm from Wikipedia:
   http://en.wikipedia.org/wiki/Power_of_two */
  #if (CLOCK_CONF_SECOND & (CLOCK_CONF_SECOND - 1)) != 0
  #error CLOCK_CONF_SECOND must be a power of two (i.e., 1, 2, 4, 8, 16, ...).
  #error Change CLOCK_CONF_SECOND in contiki-conf.h.
  #endif

  if(count % CLOCK_CONF_SECOND == 0) {
    ++seconds;
  }

  if(etimer_pending()) {
    etimer_request_poll();
  }
}

