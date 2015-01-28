/*
 * Copyright (c) 2012-2015, L3a,
 *			    Enrique Gamez Flores <egamez@edisson.com.mx>
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef INERE_CANTIDADCL_ALLOC_H_
#include "inere/cantidadcl_alloc.h"
#endif
#ifndef INERE_NUMERAL_ALLOC_H_
#include "inere/numeral_alloc.h"
#endif

char*
cantidadcl_alloc(const char* numero, const int debug)
{
  char* cents = 0;
  char* integral = 0;
  char *buffer = NULL;
  size_t buffer_len = 0;
  int cents_len = 0;
  int must_freed = 0;
  char *nombre = NULL;
#if _MSC_VER
  char *next_token = 0;
#endif

  /* Split the given 'numero' onto its integral part and its fractional part */
  /* The fractional part first */
#if _MSC_VER
  cents = (char *)strstr(numero, ".");
#else
  cents = (char *)strstr(numero, ".");
#endif

  if ( cents != NULL ) cents_len = strlen(cents);

  /* And now, the integral part */
  integral = (char*)malloc(strlen(numero)-cents_len+1);
  memset(integral, 0, strlen(numero)-cents_len+1);
#if _MSC_VER
  strncpy_s(integral, strlen(numero)-cents_len+1, numero, _TRUNCATE);
#else
  integral = strncpy(integral, numero, strlen(numero)-cents_len);
#endif

  /* Now remove the period from the fractional part */
  if ( cents_len <= 1 ) {
    cents = NULL;
    cents = (char*)malloc(3);
    memset(cents, 0, 3);
#if _MSC_VER
    strncpy_s(cents, 3, "00", 2);
#else
    cents = strncpy(cents, "00", 2);
#endif
    must_freed = 1;

  } else {

#if _MSC_VER
    cents = strtok_s(cents, ".", &next_token);
#else
    cents = strtok(cents, ".");
#endif
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
  buffer = numeral_alloc(integral, debug);

  free(integral);

  /* Capitalize the first letter */
  *buffer -= 32;

  /* Reserva la memoria para esta cantidad */
  buffer_len = strlen(buffer);
  nombre = (char *)calloc(buffer_len + 19, sizeof(char));

  /* And add the fractional part we need to add " pesos xx/100 M.N." */
#if _MSC_VER
  _snprintf_s(nombre, strlen(buffer)+19, _TRUNCATE, "%s pesos %02d/100 M.N.", buffer, atoi(cents));
#else
  snprintf(nombre, strlen(buffer)+19, "%s pesos %02d/100 M.N.", buffer, atoi(cents));
#endif
  if ( must_freed ) free(cents);
  free(buffer);

  return nombre;
}
