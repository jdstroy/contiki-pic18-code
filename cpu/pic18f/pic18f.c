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

#include "io.h"
#include <signal.h>

#include "pic18fdef.h"
#include "dev/watchdog.h"
#include "net/uip.h"

#include "sys/energest.h"
#include "sys/etimer.h"
#include "sys/rtimer.h"

/*---------------------------------------------------------------------------*/
static void
init_ports(void)
{
#define init_port(tris_reg, port_reg, tris_val, port_val) do { \
  port_reg = port_val; \
  tris_reg = tris_val; \
} while (0)

#ifdef TRISA_CONF
#ifdef PORTA_INIT
	init_port(TRISA, PORTA, TRISA_CONF, PORTA_INIT);
#else
#error TRISA_CONF/ PORTA_INIT not defined!
#endif
#endif

#ifdef TRISB_CONF
#ifdef PORTB_INIT
	 init_port(TRISB, PORTB, TRISB_CONF, PORTB_INIT);
#else
#error TRISB_CONF / PORTB_INIT not defined!
#endif
#endif

#ifdef TRISC_CONF
#ifdef PORTC_INIT
	 init_port(TRISC, PORTC, TRISC_CONF, PORTC_INIT);
#else
#error TRISC_CONF / PORTC_INIT not defined!
#endif
#endif

#ifdef TRISD_DONF
#ifdef PORTD_INIT
	 init_port(TRISD, PORTD, TRISD_CONF, PORTD_INIT);
#else
#error TRISD_CONF / PORTD_INIT not defined!
#endif
#endif

#ifdef TRISE_DONF
#ifdef PORTE_INIT
	 init_port(TRISE, PORTE, TRISE_CONF, PORTE_INIT);
#else
#error TRISE_CONF / PORTE_INIT not defined!
#endif
#endif

#ifndef ANSEL_CONF
#error ANSEL_CONF not defined!
#else
  ANSEL = ANSEL_CONF;
#endif

#ifndef ANSELH_CONF
#error ANSELH_CONF not defined!
#else
  ANSELH = ANSELH_CONF;
#endif
#undef init_port

#ifndef WPUB_CONF
#error WPUB_CONF not defined!
#else
  WPUB = WPUB_CONF;
  /* Weak pull-ups enabled */
#endif

  INTCON2bits.NOT_RBPU = 0;
}
/*---------------------------------------------------------------------------*/
static void
init_spi(void)
{
  if (SSPCON1bits.SSPEN == 1) {
    return;
  }

#ifndef SSPSTAT_CONF
#error SSPSTAT_CONF not defined!
#endif
#ifndef SSPCON_CONF
#error SSPCON_CONF not defined!
#endif
  SSPSTAT = SSPSTAT_CONF;
  SSPCON1 = SSPCON_CONF;
}
/*---------------------------------------------------------------------------*/
void
pic18f_cpu_init(void)
{
  di();
  INTCON2bits.TMR0IP = 0; /* low priority for system tick  */
  RCONbits.IPEN = 1; /* enable interrupt priority */
  watchdog_init();
//  init_ports();
#if defined SPI
  #warning "*** spi support enabled ***"
  init_spi();
#else
  #warning "*** SPI not defined, init_spi() not called ***"
#endif

#if defined RTIMER
  #warning "*** rtimer support enabled ***"
  rtimer_arch_init();
#else
  #warning "*** RTIMER not defined, rtimer_arch_init() not called ***"
#endif
  ei();
}
/*---------------------------------------------------------------------------*/
/*
 * Mask all interrupts that can be masked.
 */
spl_t
splhigh_(void)
{
  /* Clear the GIE (General Interrupt Enable) flag. */
  spl_t sr;
  sr = INTCON;
  return sr & INTCONbits.GIE;		/* Ignore other sr bits. */
}
/*---------------------------------------------------------------------------*/
/*
 * Restore previous interrupt mask.
 */
void
splx_(spl_t sr)
{
  /* If GIE was set, restore it. */
  INTCON |= sr;
}
/*---------------------------------------------------------------------------*/

/* Declare handler to be the handler function for the given signal.
 * sig should be one of SIG_xxx from above,
 * sig2 should also be a signal (probably SIG_xxxIE from below) and
 * handler should be a function defined using SIGHANDLER(handler)
 * or SIGHANDLERNAKED(handler).
 * To be used together with DEF_INTHIGH and DEF_INTLOW.
 *
 * Added extern definition, since the handler may be defined in another 
 * module. 
 */
#undef DEF_HANDLER2
#define DEF_HANDLER2(sig1,sig2,handler)         \
    __asm btfss sig1 __endasm;                  \
    __asm bra   $+8 __endasm;                   \
    __asm btfsc sig2 __endasm;                  \
    __asm extern _ ## handler __endasm;         \
    __asm goto  _ ## handler __endasm;

DEF_INTHIGH(high_handler)
  DEF_HANDLER2(SIG_RC, SIG_RCIE, uart1_rx)
/*  DEF_HANDLER2(SIG_TMR0, SIG_TMR0IE, tick_it) */
#if defined(TMR2_HANDLER)
  DEF_HANDLER2(SIG_TMR2, SIG_TMR2IE, tmr2_tick)
#endif
#if BUTTON_PIN == 0
#warning "*** INT0 IRQ handler: button_interrupt ***" 
  DEF_HANDLER2(SIG_INT0, SIG_INT0IE, button_interrupt)
#elif BUTTON_PIN == 1
#warning "*** INT0 IRQ handler: button_interrupt ***" 
  DEF_HANDLER2(SIG_INT1, SIG_INT1IE, button_interrupt)
#elif BUTTON_PIN == 2
#warning "*** INT0 IRQ handler: button_interrupt ***" 
  DEF_HANDLER2(SIG_INT2, SIG_INT2IE, button_interrupt)
#endif
END_DEF

DEF_INTLOW(low_handler)
  DEF_HANDLER2(SIG_TMR0, SIG_TMR0IE, tick_it)
END_DEF
/*---------------------------------------------------------------------------*/
