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
 *  - Cantidad: 123456789012
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
 * Helper function to add the corresponding class and period name
 * for the numeral.
 */
char*
class_name(unsigned int class_number)
{
  if ( class_number % 2 ) return " mil ";
  else if ( class_number == 2  ) return " millon ";
  else if ( class_number == 4  ) return " billon ";
  else if ( class_number == 6  ) return " trillon ";
  else if ( class_number == 8  ) return " cuatrillon ";
  else if ( class_number == 10 ) return " quintillon ";
  else if ( class_number == 12 ) return " sextillon ";
  else if ( class_number == 14 ) return " septillon ";
  else if ( class_number == 16 ) return " octillon ";
  else if ( class_number == 18 ) return " nonillon ";
  return NULL;
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
  unsigned int free_digits = 0;
  unsigned int classes = 0;
  div_t division;
  unsigned int unidad = 0;
  unsigned int decena = 0;
  unsigned int centena = 0;
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
  /* Remove any character that it isn't a decimal digit one or a leading 0s */
  while ( *numero ) {
    if ( isdigit(*numero) ) {
      if ( *numero == '0' && i == 0 ) {
	/* Avoid passing leading zeroes. */
	numero++;
	continue;
      }

      *copy++ = *numero;
      i++;
    }
    numero++;
  }
  *copy = 0; /* Set the null character at the end, and move back one more */

  /* Rewind the pointers */
  copy -= i;
  numero -= number_length;

  /* Compute a division on the 'copy' string to extract the number
   * of classes in which the numeral will be splitted
   */
  division = div(strlen(copy), 3);
  free_digits = division.rem;
  classes = division.quot;

  /* Now, start creating the numeral for the digits that no complete
   * a full class
   */
  if ( free_digits ) {
    if ( free_digits == 2 ) decena = (unsigned int)(*copy++ - 48);
    unidad = (unsigned int)(*copy++ - 48);

    if ( buffer == NULL ) buffer = (char*)calloc(MAXNUMERAL, sizeof(char));
    memset(buffer, 0, MAXNUMERAL);
    construye_clase(unidad, decena, 0, buffer, debug);
    /* add buffer to nombre */
#if _MSC_VER
    strncat_s(nombre, _countof(nombre), buffer, _TRUNCATE);
#else
    strncat(nombre, buffer, strlen(buffer));
#endif

  }

  /* Check if we need to add a suffix to the previous quantity */
  if ( classes && division.rem ) {
#if _MSC_VER
    strncat_s(nombre, _countof(nombre), class_name(classes), _TRUNCATE);
#else
    strncat(nombre, class_name(classes), strlen(class_name(classes)));
#endif
  }

  /* Now convert the remaining digits to its numeral */
  while ( classes ) {

    centena = (unsigned int)(*copy++ - 48);
    decena  = (unsigned int)(*copy++ - 48);
    unidad  = (unsigned int)(*copy++ - 48);

    if ( buffer == NULL ) buffer = (char*)calloc(MAXNUMERAL, sizeof(char));
    memset(buffer, 0, MAXNUMERAL);
    construye_clase(unidad, decena, centena, buffer, debug);
    /* add buffer to nombre */
#if _MSC_VER
    strncat_s(nombre, _countof(nombre), buffer, _TRUNCATE);
#else
    strncat(nombre, buffer, strlen(buffer));
#endif

    /* Check if you need to add some suffix */
    if ( classes > 1 ) {
#if _MSC_VER
      strncat_s(nombre, _countof(nombre), class_name(classes-1), _TRUNCATE);
#else
      strncat(nombre, class_name(classes-1), strlen(class_name(classes-1)));
#endif

    }
    classes--;
  }

  /* Freed space */
  if ( buffer != NULL ) free(buffer);

  copy -= i;
  free(copy); /* You need to add because you decrement one extra location */

  return nombre;
}
