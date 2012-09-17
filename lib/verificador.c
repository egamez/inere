/*
 * Copyright (c) 2012, Enrique Gamez Flores <egamez@edisson.com.mx>,
 *                     L.A.E.
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
 * PROCEDIMIENTO PARA CALCULAR EL DIGITO VERIFICADOR DEL REGISTRO FEDERAL DE
 * CONTRIBUYENTES
 *
 * 1. Se asignaran los valores del Anexo III a las letras y numeros del
 *    registro federal de contribuyentes formado a 12 posiciones
 *    Ejemplo:
 *
 *	GODE561231GR
 *
 *		G = 16
 *		O = 25
 *		D = 13
 *		E = 14
 *		5 = 05
 *		6 = 06
 *		1 = 01
 *		2 = 02
 *		3 = 03
 *		1 = 01
 *		G = 16
 *		R = 28
 *
 * 2. Una vez asignados los valores se aplicara la siguiente forma tomando como
 *    base el factor 13 en orden descendente a cada letra y numero del R.F.C.
 *    para su multiplicacion, de acuerdo a la siguiente formula:
 *
 *	(V_1 * (P_1 + 1)) + (V_2 * (P_2 + 1)) + ... + (V_12 * (P_12 + 1)) MOD11
 *
 *	V_i es el valor asociado al caracter de acuerdo a la tabla del Anexo III
 *	P_i es la posicion que ocupa el i-esimo caracter tomando de derecha a
 *	izquierda, es decir, P toma los valores 1 a 12.
 *
 * Ejemplo:
 *
 *	D = (16*13 + 25*12 + 13*11 + 14*10 + 05*9 + 06*8 + 01*7 + 02*6 +
 *	     03*5 + 01*4 + 16*3 + 28*2 = 1026
 *
 *	Donde el resultado sera: 1026
 *
 * 3. El resultado de la suma se divide entre el factor 11
 *
 *	En el ejemplo, el cociente sera 93, y el residuo 03
 *
 *    y se le asigna el digito verificador segun el siguiente criterio:
 *
 *	- Si el residuo es igual a cero, este sera el valor que se le asignara
 *	  al digito verificador.
 *	- Si el residuo es mayor a cero se restara este numero al factor 11:
 *		11 - 3 = 8
 *	- Si el residuo es igual a 10, o el resultado de la substraccion
 *	  anterior lo es, el digito verificador sera "A".
 *
 * Por lo tanto, "8" es el digito verificador del RFC del ejemplo. Quedando el
 * R.F.C. como: GODE561231GR8
 *
 *
 * ANEXO III
 * Tabla de valores para la generacion del codigo verificador del registro
 * federal de contribuyentes.
 *
 *	0	00		D	13		P	26
 *	1	01		E	14		Q	27
 *	2	02		F	15		R	28
 *	3	03		G	16		S	29
 *	4	04		H	17		T	30
 *	5	05		I	18		U	31
 *	6	06		J	19		V	32
 *	7	07		K	20		W	33
 *	8	08		L	21		X	34
 *	9	09		M	22		Y	35
 *	A	10		N	23		Z	36
 *	B	11		&	24		ESPACIO	37
 *	C	12		O	25		Ñ	38
 *
 * Todos los demas caracteres que no se encuentren en la tabla, les sera
 * asignado el valor cero.
 *
 * BUG in https://www.recaudanet.gob.mx/recaudanet/rfc.jsp
 * Input was 06/074/197 when the correct one should be 06/04/1970
 */
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#ifndef INERE_VERIFICADOR_INCLUDED_H
#include "inere/verificador.h"
#endif

int
anexo3(const char caracter)
{
  /* Convert the caracter given to uppercase */
  char c = toupper(caracter);
  if      ( c == '0' ) return 0;
  else if ( c == '1' ) return 1;
  else if ( c == '2' ) return 2;
  else if ( c == '3' ) return 3;
  else if ( c == '4' ) return 4;
  else if ( c == '5' ) return 5;
  else if ( c == '6' ) return 6;
  else if ( c == '7' ) return 7;
  else if ( c == '8' ) return 8;
  else if ( c == '9' ) return 9;
  else if ( c == 'A' ) return 10;
  else if ( c == 'B' ) return 11;
  else if ( c == 'C' ) return 12;
  else if ( c == 'D' ) return 13;
  else if ( c == 'E' ) return 14;
  else if ( c == 'F' ) return 15;
  else if ( c == 'G' ) return 16;
  else if ( c == 'H' ) return 17;
  else if ( c == 'I' ) return 18;
  else if ( c == 'J' ) return 19;
  else if ( c == 'K' ) return 20;
  else if ( c == 'L' ) return 21;
  else if ( c == 'M' ) return 22;
  else if ( c == 'N' ) return 23;
  else if ( c == '&' ) return 24;
  else if ( c == 'O' ) return 25;
  else if ( c == 'P' ) return 26;
  else if ( c == 'Q' ) return 27;
  else if ( c == 'R' ) return 28;
  else if ( c == 'S' ) return 29;
  else if ( c == 'T' ) return 30;
  else if ( c == 'U' ) return 31;
  else if ( c == 'V' ) return 32;
  else if ( c == 'W' ) return 33;
  else if ( c == 'X' ) return 34;
  else if ( c == 'Y' ) return 35;
  else if ( c == 'Z' ) return 36;
  else if ( c == ' ' ) return 37;
  /*else if ( c == 'Ñ' ) return 40;*/
  else return 0;
  return 0;
}

/**
 * Convierte los caracteres de la clave del RFC en los valores definidos
 * en el Anexo III. Debes verificar que la clave del RFC tenga exactamente
 * 12 caracteres, y en caso de que aparezcan solo 11 caracteres (caso en el
 * que posiblemente se trate de un RFC de una persona moral, agrega el valor
 * correspondiente a un espacio al inicio de la secuencia de valores.
 */
char
digito_verificador(const char* rfc, const int debug)
{
  unsigned int suma = 0;
  unsigned int index = 13;
  unsigned int remainder = 0;
  char result = 0;
  size_t length = strlen(rfc);

  if ( length > 12 ) {
    /* The rfc length shouldn't be larger than 12 characters */
    fprintf(stderr, "Error. The RFC length shouldn't be larger than 12 characters. The number of characters passed were: %ld.\n", length);
    return 0;
  } else if ( length < 11 ) {
    fprintf(stderr, "Error. The RFC length must be of at least 11 characters long (for personas morales. The number of characters passed was: %ld.\n", length);
    return 0;
  }

  if ( length == 11 ) {
    /*You need to add a zero value for this character, and advance the counter*/
    printf("Adding an extra space at the beggining of the RFC.\n");
    suma += anexo3(' ') * index--;
  }
  while ( *rfc ) {
    suma += anexo3(*rfc++) * index--;
  }

  /* Now get the residual of dividing 'suma' over 11 */
  remainder = suma % 11;
  /* Now, apply the crieria to determine the caracter diferenciador */
  if ( remainder == 0 ) result = '0';
  else if ( remainder == 1 ) result = 'A'; /* 11 - 10 = 1 */
  else result = (11 - remainder) + 48; /* 48 is the '0' in ascii */

  if ( debug ) printf("Suma [%d], remainder [%d], digit [%c]\n", suma, remainder, result);
  return result;
}
