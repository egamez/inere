/*
 * Copyright (c) 2012, Enrique Gamez Flores <egamez@edisson.com.mx>,
 *                     Lae
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

/**
 * Pequeno program para traducir numeros arabigos a sus numerales.
 *
 * http://buscon.rae.es/dpdI/SrvltConsulta?lema=n%C3%BAmeros
 * http://es.wikipedia.org/wiki/Nombres_de_los_n%C3%BAmeros_en_espa%C3%B1ol
 * http://es.wikipedia.org/wiki/Escalas_num%C3%A9ricas_corta_y_larga
 *
 * Importante. Los numerales que terminan con 'uno', apocopan el 'uno' en 'un'
 * _solo_ si la palabra que la siguen es un sustantivo masculino o un
 * sustantivo femenino comenzando en 'a' o en 'ha' _tonica_. Los casos para
 * los cuales aplica esta regla son:
 *
 *	21	veintiuno	veintiun	veintiuna
 *	31	treinta y uno	treinta y un	treinte y una
 *	41	cuarenta y uno	cuarenta y un	cuarenta y una
 *	51	cincuenta y uno	cincuenta y un	cincuenta y una
 *	61	sesenta y uno	sesenta y un	sesenta y una
 *	71	setenta y uno	setenta y un	setenta y una
 *	81	ochenta y uno	ochenta y un	ochenta y una
 *	91	noventa y uno	noventa y un	noventa y una
 *
 * Debido a que este programa esta pensado para numerales que representan
 * cantidades de dinero en peso mexicanos, y el peso es un sustantivo masculino
 * se utilizaran las cantidades terminadas con apocope 'un'.
 *
 * La cantidad '0' en castellano es zero, pero en espanol de Mexico es cero.
 *
 * El numeral singular mas grande en castellano tiene longitud 14 (incluyendo
 * NUL).
 *
 * El numeral mas grande, actualmente disponible, corresponde al numero 54454454
 * y su numeral tiene longitud 97 ("cincuenta y cuatro millones cuatrocientos
 * cincuenta y cuatro mil cuatrocientos cincuenta y cuatro")
 *
 * BUGS:
 *  - Plurales
 *  - Sustantivos masculino y femenino
 *  - Numero: 0000001
 *
 */
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#ifndef INERE_NUMERALES_INCLUDED_H
#include "inere/numerales.h"
#endif

char*
unidades(const unsigned int numero)
{
  if ( numero == 0 ) return "cero";
  else if ( numero == 1 ) return "un";
  else if ( numero == 2 ) return "dos";
  else if ( numero == 3 ) return "tres";
  else if ( numero == 4 ) return "cuatro";
  else if ( numero == 5 ) return "cinco";
  else if ( numero == 6 ) return "seis";
  else if ( numero == 7 ) return "siete";
  else if ( numero == 8 ) return "ocho";
  else if ( numero == 9 ) return "nueve";
  return NULL;
}

char*
decenas(const unsigned int numero)
{
  if ( numero == 3 ) return "treinta";
  else if ( numero == 4 ) return "cuarenta";
  else if ( numero == 5 ) return "cincuenta";
  else if ( numero == 6 ) return "sesenta";
  else if ( numero == 7 ) return "setenta";
  else if ( numero == 8 ) return "ochenta";
  else if ( numero == 9 ) return "noventa";
  return NULL;
}

char*
centenas(const unsigned int numero)
{
  if ( numero == 1 ) return "ciento";
  else if ( numero == 2 ) return "doscientos";
  else if ( numero == 3 ) return "trescientos";
  else if ( numero == 4 ) return "cuatrocientos";
  else if ( numero == 5 ) return "quinientos";
  else if ( numero == 6 ) return "seiscientos";
  else if ( numero == 7 ) return "setecientos";
  else if ( numero == 8 ) return "ochocientos";
  else if ( numero == 9 ) return "novecientos";
  return NULL;
}

char*
diez_y_veinte(const unsigned int numero)
{
  if ( numero == 10 ) return "diez";
  else if ( numero == 11 ) return "once";
  else if ( numero == 12 ) return "doce";
  else if ( numero == 13 ) return "trece";
  else if ( numero == 14 ) return "catorce";
  else if ( numero == 15 ) return "quince";
  else if ( numero == 16 ) return "dieciseis";
  else if ( numero == 17 ) return "diecisiete";
  else if ( numero == 18 ) return "dieciocho";
  else if ( numero == 19 ) return "diecinueve";
  else if ( numero == 20 ) return "veinte";
  else if ( numero == 21 ) return "veintiun";
  else if ( numero == 22 ) return "veintidos";
  else if ( numero == 23 ) return "veintitres";
  else if ( numero == 24 ) return "veinticuatro";
  else if ( numero == 25 ) return "veinticinco";
  else if ( numero == 26 ) return "veintiseis";
  else if ( numero == 27 ) return "veintisiete";
  else if ( numero == 28 ) return "veintiocho";
  else if ( numero == 29 ) return "veintinueve";
  return NULL;
}

char*
cien(void)
{
  return "cien";
}

char*
mil(void)
{
  return "mil";
}

char*
millones(const unsigned int numero)
{
  if ( numero == 1 ) return "millon";
  else if ( numero == 2 ) return "billon";
  else if ( numero == 3 ) return "trillon";
  else if ( numero == 4 ) return "cuatrillon";
  else if ( numero == 5 ) return "quintillon";
  else if ( numero == 6 ) return "sextillon";
  else if ( numero == 7 ) return "septillon";
  else if ( numero == 8 ) return "octillon";
  else if ( numero == 9 ) return "nonillon";
  return NULL;
}

/*
 * Function to swap strings
 * dst must always have length = MAXNUMERAL
 */
char*
swap_and_add(char* dst, char* buffer)
{
#if _MSC_VER
  char* tmp = _strdup(dst);
#else
  char* tmp = strndup(dst, strlen(dst)+1);
#endif

  memset(dst, 0, MAXNUMERAL);
#if _MSC_VER
  strncpy_s(dst, strlen(buffer)+1, buffer, _TRUNCATE);
  if ( strlen(tmp) ) strncat_s(dst, strlen(buffer)+1, " ", 1);
  strncat_s(dst, strlen(buffer)+1, tmp, strlen(tmp));
#else
  strncpy(dst, buffer, strlen(buffer));
  if ( strlen(tmp) ) strncat(dst, " ", 1);
  strncat(dst, tmp, strlen(tmp));
#endif
  free(tmp);
  return dst;
}

char*
construye_clase(const unsigned int unidad, const unsigned int decena, const unsigned int centena, char* buffer, const int debug)
{
  char* unidad_str = NULL;
  char* decena_str = NULL;
  char* centena_str = NULL;

  if ( unidad > 0 ) unidad_str = unidades(unidad);
  if ( decena == 1 || decena == 2 ) {
    decena_str = diez_y_veinte(decena*10 + unidad);
    unidad_str = NULL;

  } else if ( decena > 2 ) {
    decena_str = decenas(decena);

  }

  if ( centena > 0 ) centena_str = centenas(centena);

  /* Build the name */
  if ( centena_str != NULL ) {
#if _MSC_VER
    if ( decena_str == NULL && unidad_str == NULL ) strncpy_s(buffer, strlen(cien())+1, cien(), _TRUNCATE);
    else					    strncpy_s(buffer, strlen(centena_str)+1, centena_str, _TRUNCATE);
#else
    if ( decena_str == NULL && unidad_str == NULL ) strncpy(buffer, cien(), strlen(cien()));
    else					    strncpy(buffer, centena_str, strlen(centena_str));
#endif

  }

  if ( decena_str != NULL ) {
#if _MSC_VER
    if ( centena_str != NULL ) strncat_s(buffer, _countof(buffer), " ", 1);
    strncat_s(buffer, _countof(buffer), decena_str, strlen(decena_str));
#else
    if ( centena_str != NULL ) strncat(buffer, " ", 1);
    strncat(buffer, decena_str, strlen(decena_str));
#endif

  }

  if ( unidad_str != NULL ) {
#if _MSC_VER
    if ( decena_str != NULL ) 	    strncat_s(buffer, _countof(buffer), " y ", 3);
    else if ( centena_str != NULL ) strncat_s(buffer, _countof(buffer), " ", 1);
    strncat_s(buffer, _countof(buffer), unidad_str, strlen(unidad_str));
#else
    if ( decena_str != NULL ) 	    strncat(buffer, " y ", 3);
    else if ( centena_str != NULL ) strncat(buffer, " ", 1);
    strncat(buffer, unidad_str, strlen(unidad_str));
#endif
  }

  if ( debug ) printf("construye_clase: Numero [%d] = [%s]\n", centena*100+decena*10+unidad, buffer);

  return buffer;
}

/**
 *
 */
char*
numeral(char* nombre, const char* numero, const int debug)
{
  int i = 0;
  char* copy = NULL;
  char* buffer = NULL;
  unsigned int unidad = 0;
  unsigned int decena = 0;
  unsigned int centena = 0;
  unsigned int orden = 0;
  unsigned int clase = 1; /* We have at least one clase */
  unsigned int periodo = 0;
  const size_t number_length = strlen(numero);

  /* There is one trivial option. The one in which the number given has
   * only one digit. If this is the case the translation is simple.
   */
  if ( strlen(numero) == 1 ) {
    buffer = unidades((unsigned int)(*numero-48));
#if _MSC_VER
    strncpy_s(nombre, strlen(buffer)+1, buffer, _TRUNCATE);
#else
    strncpy(nombre, buffer, strlen(buffer));
#endif
    return nombre;
  }

  /* This is the standard case in which you have a quantity with more than
   * one digit. Now, remove any non digit character, to do so, you need
   * to traverse all the array.
   */
  copy = (char*)calloc(number_length+1, sizeof(char));
  memset(copy, 0, number_length+1);
  /* Remove any character that it isn't a decimal digit one */
  while ( *numero ) {
    if ( isdigit(*numero) ) {
      *copy++ = *numero;
      i++;
    }
    numero++;
  }
  *copy-- = 0; /* Set the null character at the end, en move back one more */

  /* Now, you need to split the number onto its classes.
   * The copy pointer is currently at the beginning of the number, so you just
   * need to reverse the string.
   */
   while (i) {

    orden++;
    if ( debug ) printf("numeral: digito [%c], index = %d\n", *copy, i);

    /* Now, to generate the numeral, you need to build it from blocks
     * This blocks are collections of 3 digits, named 'clase'
     */
    if ( orden == 1 ) {
      unidad = (unsigned int)(*copy - 48);

    } else if ( orden == 2 ) {
      decena = (unsigned int)(*copy - 48);

    } else if ( orden == 3 ) {
      centena = (unsigned int)(*copy - 48);

      /* Reset the 'orden' counter. */
      orden = 0;

      if ( buffer == NULL ) buffer = (char*)calloc(MAXNUMERAL, sizeof(char));

      memset(buffer, 0, MAXNUMERAL);
      construye_clase(unidad, decena, centena, buffer, debug);

      /* And add one 'clase' more */
      clase++;
      if ( clase % 2 ) {
	if ( strlen(buffer) ) swap_and_add(nombre, "mil");
	periodo++; /* Add one periodo */
      }


      /* Check if we already have some previous periods */
      if ( nombre != NULL ) {
	/* You need to swap the names */
        swap_and_add(nombre, buffer);

      } else {
	/* Set the numeral */
#if _MSC_VER
	strncpy_s(nombre, strlen(buffer)+1, buffer, _TRUNCATE);
#else
	strncpy(nombre, buffer, strlen(buffer));
#endif

      }
      unidad = decena = centena = 0; /* Reset the current numbers */
      free(buffer);
      buffer = NULL;

    }

    copy--;
    i--;
  }

  /* Now, recover the last step in case that the number doesn't
   * complete a clase
   */
  if ( unidad || decena || centena ) {

    if ( buffer == NULL ) buffer = (char*)calloc(MAXNUMERAL, sizeof(char));

    memset(buffer, 0, MAXNUMERAL);
    construye_clase(unidad, decena, centena, buffer, debug);

    if ( periodo ) {
      if ( strcmp(buffer, "un") ) swap_and_add(nombre, "millones");
      else			  swap_and_add(nombre, "millon");
    }

    /* After splitting the numbers in groups of three, add ... */
    if ( !(clase % 2) ) {
      if ( strlen(buffer) ) swap_and_add(nombre, "mil");
    }

    if ( nombre != NULL ) {
      /* You need to swap the names */
      swap_and_add(nombre, buffer);

    } else {
      /* Set the numeral */
#if _MSC_VER
      strncpy_s(nombre, strlen(buffer)+1, buffer, strlen(buffer));
#else
      strncpy(nombre, buffer, strlen(buffer));
#endif

    }

  }

  /* Freed space */
  if ( buffer != NULL ) free(buffer);

  free(++copy); /* You need to add because you decrement one extra location */

  return nombre;
}
