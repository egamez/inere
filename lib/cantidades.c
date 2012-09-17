/*
 * Copyright (c) 2012, Enrique Gamez Flores <egamez@edisson.com.mx>,
 *		       L.A.E.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __OpenBSD__
# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef INERE_CANTIDADES_INCLUDED_H
#include "inere/cantidades.h"
#endif
#ifndef INERE_NUMERALES_INCLUDED_H
#include "inere/numerales.h"
#endif

char*
cantidad(char* nombre, const char* numero, const int debug)
{
  char* cents = 0;
  char* integral = 0;
  char buffer[MAXNUMERAL];
  int cents_len = 0;

  /* Split the given 'numero' onto its integral part and its fractional part */
  /* The fractional part first */
  cents = strstr(numero, ".");

  if ( cents != NULL ) cents_len = strlen(cents);

  /* And now, the integral part */
#ifndef __OpenBSD__
  integral = strndupa(numero, strlen(numero)-cents_len);
#else
  integral = (char*)alloca(strlen(numero)-cents_len+1);
  memset(integral, 0, strlen(numero)-cents_len+1);
  integral = strncpy(integral, numero, strlen(numero)-cents_len);
#endif

  /* Now remove the period from the fractional part */
  if ( cents_len <= 1 ) {
#ifndef __OpenBSD__
    cents = strndupa("00", 2);
#else
    cents = (char*)alloca(3);
    memset(cents, 0, 3);
    cents = strncpy(cents, "00", 2);
#endif

  } else {

    cents = strtok(cents, ".");
    /* And make sure that the fractionail part has only two digits */
    if ( strlen(cents) == 1 ) {
      *(cents+1) = '0';
      *(cents+2) = 0;

    } else if ( strlen(cents) > 2 ) {
      if ( debug ) printf("Cantidad de centavos mayor a 100. Eliminando cantidades menores que centesimos...");
      *(cents+2) = 0;
      if ( debug ) printf("eliminados.\n");
    }

  }

  /* Now build up the full quantity name */
  memset(buffer, 0, MAXNUMERAL);
  numeral(buffer, integral, debug);

  /* Capitalize the first letter */
  *buffer -= 32;

  /* And add the fractional part we need to add " pesos xx/100 M.N." */
  snprintf(nombre, strlen(buffer)+19, "%s pesos %02d/100 M.N.", buffer, atoi(cents));

  return nombre;
}
