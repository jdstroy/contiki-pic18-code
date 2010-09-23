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
 * @(#)$Id: hwconf.h,v 1.3 2010/01/14 20:05:58 nifi Exp $
 */
#ifndef __HWCONF_H__
#define __HWCONF_H__

#include "sys/cc.h"

#include <io.h>


#define ANSEL_REG(port, bit) ANS##port##bit##_REG
#define ANSEL_BIT(port, bit) ANS##port##bit##_POS

#define ANSB0_REG ANSELH
#define ANSB1_REG ANSELH
#define ANSB2_REG ANSELH
#define ANSB3_REG ANSELH
#define ANSB4_REG ANSELH

#define ANSA0_REG ANSEL
#define ANSA1_REG ANSEL
#define ANSA2_REG ANSEL
#define ANSA3_REG ANSEL
#define ANSA5_REG ANSEL
#define ANSA0_REG ANSEL
#define ANSE1_REG ANSEL
#define ANSE2_REG ANSEL

#define ANSB0_POS 4
#define ANSB1_POS 2
#define ANSB2_POS 0
#define ANSB3_POS 1
#define ANSB4_POS 3

#define ANSA0_POS 0
#define ANSA1_POS 1
#define ANSA2_POS 2
#define ANSA3_POS 3
#define ANSA5_POS 4
#define ANSE0_POS 5
#define ANSE1_POS 6
#define ANSE2_POS 7


#define HWCONF_PIN(name, port, bit) \
static inline void name##_SELECT_IO() {ANSEL_REG(port, bit) &= ~(1 << ANSEL_BIT(port, bit));} \
static inline void name##_SET() {PORT##port |= 1 << bit;} \
static inline void name##_CLEAR() {PORT##port &= ~(1 << bit);} \
static inline uint8_t name##_READ() {return (PORT##port & (1 << bit));} \
static inline void name##_MAKE_OUTPUT() {TRIS##port &= ~(1 << bit);} \
static inline void name##_MAKE_INPUT() {TRIS##port |= 1 << bit;} \
static inline void name##_TOGGLE() {__asm btg _##LAT##port, bit, 0 __endasm;} 

#define HWCONF_PIN_NO_MX(name, port, bit) \
static inline void name##_SET() {PORT##port |= 1 << bit;} \
static inline void name##_CLEAR() {PORT##port &= ~(1 << bit);} \
static inline uint8_t name##_READ() {return (PORT##port & (1 << bit));} \
static inline void name##_MAKE_OUTPUT() {TRIS##port &= ~(1 << bit);} \
static inline void name##_MAKE_INPUT() {TRIS##port |= 1 << bit;} \
static inline void name##_TOGGLE() {__asm btg _##LAT##port, bit, 0 __endasm;} 

#define INT_REG(port, bit) R##port##bit##I_REG
#define INT_IE(port, bit) R##port##bit##IE_POS 
#define INT_IF(port, bit) R##port##bit##IF_POS
#define INT_ED_REG(port, bit) R##port##bit##ED_REG
#define INT_ED_POS(port, bit) R##port##bit##ED_POS

#define RB0I_REG INTCON
#define RB1I_REG INTCON3
#define RB2I_REG INTCON3
#define RB0IE_POS 4
#define RB0IF_POS 1
#define RB1IE_POS 3
#define RB1IF_POS 0
#define RB2IE_POS 4
#define RB2IF_POS 1

#define RB0ED_REG INTCON2
#define RB1ED_REG INTCON2
#define RB2ED_REG INTCON2
#define RB0ED_POS 6
#define RB1ED_POS 5
#define RB2ED_POS 4

#define HWCONF_IRQ(name, port, bit) \
static inline void name##_ENABLE_IRQ() {INT_REG(port, bit) |= 1 << INT_IE(port, bit);} \
static inline void name##_DISABLE_IRQ() {INT_REG(port, bit) &= ~(1 << INT_IE(port, bit));} \
static inline uint8_t name##_IRQ_ENABLED() {return INT_REG(port, bit) & (1 << INT_IE(port, bit));} \
static inline void name##_IRQ_EDGE_SELECTD() {INT_ED_REG(port, bit) &= ~(1 << INT_ED_POS(port, bit));} \
static inline void name##_IRQ_EDGE_SELECTU() {INT_ED_REG(port, bit) |= 1 << INT_ED_POS(port, bit);}\
static inline uint8_t name##_CHECK_IRQ() {return INT_REG(port, bit) & (1 << INT_IF(port, bit));} \
static inline void name##_CLEAR_FLAG() {INT_REG(port, bit) &= ~(1 << INT_IF(port, bit));} \
static inline uint8_t name##_IRQ_PORT() {return PORT##port;}

#endif /* __HWCONF_H__ */
