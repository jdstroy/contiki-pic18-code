/*
 * Copyright (c) 2007, Swedish Institute of Computer Science
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
 * @(#)$Id: msp430def.h,v 1.1 2007/08/16 13:09:06 bg- Exp $
 */

#ifndef PIC18FDEF_H
#define PIC18FDEF_H

/* partially derived from 8051def.h */

#include <stdint.h>

/* These names are deprecated, use C99 names. */
typedef  uint8_t    u8_t;
typedef uint16_t   u16_t;
typedef uint32_t   u32_t;
typedef  int32_t   s32_t;

/* Compiler configurations */
#define CCIF
#define CLIF
#define CC_CONF_CONST_FUNCTION_BUG

/*
* Enable architecture-depend checksum calculation
* for uIP configuration.
* @see uip_arch.h
* @see uip_arch-asm.S
*/
/*
* DO NOT USE UIP_ARCH flags!
* uip_arch code was copied from z80 directory but NOT ported
*/

#define UIP_ARCH_ADD32      0
#define UIP_ARCH_CHKSUM 0
#define UIP_ARCH_IPCHKSUM

#define CC_CONF_ASSIGN_AGGREGATE(dest, src) \
    memcpy(dest, src, sizeof(*dest))

#define uip_ipaddr_copy(dest, src) \
    memcpy(dest, src, sizeof(*dest))

/* ensure that memcpy is available */
#include <string.h>

void pic18f_cpu_init(void);	/* Rename to cpu_init() later! */
#define cpu_init() pic18f_cpu_init()

typedef unsigned char spl_t;
#define splhigh() splhigh_()
#define splx(sr) splx_()

#define READ_AND_DISCARD(r) do { __asm movf _##r __endasm; } while (0)
#define di() do { INTCONbits.GIEH = INTCONbits.GIEL = 0; } while (0)
#define ei() do { INTCONbits.GIEH = INTCONbits.GIEL = 1; } while (0)

#endif /* PIC18FDEF_H */
