/*
 * Copyright (c) 2014, L3a,
 *			Enrique Gámez <egamez@edisson.com.mx>
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
#ifndef INERE_EXTRAEFECHA_INCLUDED_H
#include "inere/extrae_fecha.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * Esta función es para extraer de la clave del R.F.C. la fecha
 *
 * La clave puede no estar en su forma completa, pero si se espera que
 * contenga la fecha completa, que es la finalidad de esta funcion.
 *
 * Los argumentos de la función son:
 *
 *	- La clave del R.F.C. (completa o semicompleta, pero debe tener
 *	  la fecha),
 *	- Un arreglo de longitud 3 (incluyendo el caracter NUL) para el dia,
 *	- Un arreglo de longitud 3 (incluyendo el caracter NUL) para el mes,
 *	- Un arreglo de longitud 3 (incluyendo el caracter NUL) para el año,
 *	- Un int para imprimir mensajes de informacion.
 *
 * La función regresa:
 *
 *	 0	Si la función logro extraer la fecha
 *
 * y en caso de no ejecutar la extracción:
 *
 *	-1	Si la longitud de la cadena de caracteres suministrada no es
 *		lo suficientemente larga como para extraer la fecha.
 *	-2	Si la cantidad de digitos, que siguen a las letras que
 *		del nombre del contribuyente, que forman la clave, no es
 *		suficiente para extraer la fecha.
 *	-3	Si la cantidad de letras, que corresponden al nombre (en
 *		la clave) excede a 4, con lo cual identificamos una clave
 *		mal formada, y la extracción de alguna fecha no seria confiable
 *	-4	Un error improbable que ocurriria en caso de no fuesen
 *		detectados ninguna de las situaciones previas.
 *	-5	Cuando el proceso de extracción no pudo completarse.
 */
int
extrae_fecha(const char *semiclave, char *day, char *month, char *year, int verbose)
{
  int res = 0;
  char siglas[5];
  int letras = 0;
  int digitos = 0;
  int i = 0;
  const size_t len = strlen(semiclave);

  memset(day,    0, 3);
  memset(month,  0, 3);
  memset(year,   0, 3);
  memset(siglas, 0, 5);

  if ( len > 13 || len < 9 ) {
    /* No es posible realizar extraccion alguna */
    if ( verbose ) printf("%s:%d : Error -1. Longitud de la clave no suficiente para realizar la extracción (longitud = %d.)\n", __FILE__, __LINE__, len);
    return -1;
  }

  /* Haz un chequeo sobre el tipo de clave y su calidad */
  while ( *semiclave ) {
    if      ( isalpha(*semiclave) ) letras++;
    else if ( *semiclave == '^'   ) letras++; /* Para la conversion en inere */
    else if ( *semiclave == '&'   ) letras++;
    else if ( isdigit(*semiclave) ) digitos++;

    if ( letras > 4 ) {
      /* Esta no puede ser una clave del R.F.C. */
      if ( verbose ) printf("%s:%d : Se ha completado la cantidad maxima de carácteres para este test.\n", __FILE__, __LINE__);
      break;
    }

    if ( digitos == 6 ) {
      /* Se ha llegado el limite de este test */
      if ( verbose ) printf("%s:%d : Se ha completado la cantidad máxima para los digitos que representan la fecha en la clave.\n", __FILE__, __LINE__);
      break;
    }

    semiclave++;
    i++;
  }

  /* Rewind the string */
  semiclave -= i;

  /* Analiza ahora el tipo de clave del cual se quiere extraer las partes. */
  if ( digitos != 6 ) {
    /* No es posible hacer extraccion alguna, la clave no esta bien formada */
    if ( verbose ) printf("%s:%d : Error 2. No es suficiente la cantidad de digitos para la fecha (digitos = %d.)\n", __FILE__, __LINE__, digitos);
    return -2;

  } else if ( letras > 4 || letras < 3 ) {
    /* Esta es la misma situacion que arriba. */
    if ( verbose ) printf("%s:%d : Error 4. Cantidad de letras mayor a la esperada (letras = %d.)\n", __FILE__, __LINE__, letras);
    return -3;

  }

  /* Ahora, prepara la conversion */
  if ( letras == 4 ) {
    /* Este es el caso para una persona fisica */
    if ( verbose ) printf("%s:%d : Aplicando sscanf con el siguiente formato \"%%4s%%2s%%2s%%2s\" para la clave de una persona fisica.\n", __FILE__, __LINE__);
    res = sscanf(semiclave, "%4s%2s%2s%2s", siglas, year, month, day);

  } else if ( letras == 3 ) {
    /* Este es al caso de una persona moral */
    if ( verbose ) printf("%s:%d : Aplicando sscanf con el siguiente formato \"%%3s%%2s%%2s%%2s\" para la clave de una persona moral.\n", __FILE__, __LINE__);
    res = sscanf(semiclave, "%3s%2s%2s%2s", siglas, year, month, day);

  } else {
    /* No hay razon por la cual uno deba estar aqui! */
    if ( verbose ) printf("%s:%d : Error 4. Error inesperador (letras = %d, digitos = %d.)\n", __FILE__, __LINE__, letras, digitos);
    return -4;
  }

  /* Verifica que se hallan hecho las sustituciones correctas */
  if ( res != 4 ) {
    if ( verbose ) printf("%s:%d : Error 5. Cantidad de sustituciones mayor a la esperada (sustituciones = %d.)\n", __FILE__, __LINE__, res);
    return -5;

  }

  if ( verbose ) printf("%s:%d : Input [%s], letras de la clave [%s], año [%s], mes [%s], día [%s]\n", __FILE__, __LINE__, semiclave, siglas, year, month, day);
  return 0;
}
