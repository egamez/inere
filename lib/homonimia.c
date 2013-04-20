/*
 * Copyright (c) 2012,	Lae,
 *			Enrique Gámez Flores <egamez@edisson.com.mx>
 * All rights reserved
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
 * PROCEDIMIENTO PARA OBTENER LA CLAVE DIFERENCIADORA DE HOMONIMIA
 *
 * 1. Se asignaran valores a las letras del nombre o razon social de acuerdo
 * a la tabla del Anexo 1.
 *
 * Ejemplo:
 *
 *	Gomez Diaz Emma
 *
 *	G = 17	Espacio = 00	Espacio = 00
 *	O = 26	D = 14		E = 15
 *	M = 24	I = 19		M = 24
 *	E = 15	A = 11		M = 24
 *	Z = 39	Z = 39		A = 11
 *
 * NOTA: No se debe permitir caracteres especiales en el nombre, excepto el &
 *
 * 2. Se ordenan los valores de la siguiente manera:
 *
 *	G   O  M  E  Z  _  D  I  A  Z _   E  M  M  A
 *	017 26 24 15 39 00 14 19 11 39 00 15 24 24 11
 *
 * Se agrega un cero al valor de la primera letra para uniformar el criterio
 * de los numeros a tomar de dos en dos.
 *
 * 3. Se efectuaran las multiplicaciones de los numeros tomados de dos en dos
 *    para la posicion de la pareja:
 *
 * 01*1	= 1	90*0 = 0	90*0 = 0
 * 17*7 = 115	00*0 = 0	00*0 = 0
 * 72*2 = 144	01*1 = 1	01*1 = 1
 * 26*6 = 156	14*4 = 56	15*5 = 75
 * 62*2 = 124	41*1 = 41	52*2 = 104
 * 24*4 = 96	19*9 = 171	24*4 = 96
 * 41*1 = 41	91*1 = 91	42*2 = 84
 * 15*5 = 75	11*1 = 11	24*4 = 96
 * 53*3 = 159	13*3 = 39	41*1 = 41
 * 39*9 = 351	39*9 = 351	11*1 = 11
 *
 * 4. Se suma el resultado de las multiplicaciones y del resultado obtenido,
 *    se tomaran las tres ultimas cifras y estas se dividen entre el factor
 *    34, ejemplo:
 *
 *	El resultado de la suma de las multiplicaciones es:
 *			2535
 *	15 el cociente, y el residuo 25
 *
 * 5. Con el cociente y el residuo se consulta en la table del Anexo II y se
 *    asignia la homonimia.
 *
 *	Obteniendose:
 *
 *			15 = G
 *			25 = R
 *
 *    por lo tanto la homonimia sera "GR".
 *
 *		RFC CON HOMONIMIA:	GODE561231GR
 *
 *
 * ANEXO I
 * Table para asignar valores a los caracteres qua aparecen en el nombre al que
 * se la calculara la clave diferenciadora de homonimo
 *
 *	ESPACIO = 00		B = 12			O = 26
 *	0 = 00			C = 13			P = 27
 *	1 = 01			D = 14			Q = 28
 *	2 = 02			E = 15			R = 29
 *	3 = 03			F = 16			S = 32
 *	4 = 04			G = 17			T = 33
 *	5 = 05			H = 18			U = 34
 *	6 = 06			I = 19			V = 35
 *	7 = 07			J = 21			W = 36
 *	8 = 08			K = 22			X = 37
 *	9 = 09			L = 23			Y = 38
 *	& = 10			M = 24			Z = 39
 *	A = 11			N = 25			Ñ = 40
 *
 *
 * ANEXO II
 * Table de valores que se asignan a la clave diferenciadora de homonimio en
 * base al coeficiente y al residuo.
 *
 *	0  = 1			17 = I
 *	1  = 2			18 = J
 *	2  = 3			19 = K
 *	3  = 4			20 = L
 *	4  = 5			21 = M
 *	5  = 6			22 = N
 *	6  = 7			23 = P
 *	7  = 8			24 = Q
 *	8  = 9			25 = R
 *	9  = A			26 = S
 *	10 = B			27 = T
 *	11 = C			28 = U
 *	12 = D			29 = V
 *	13 = E			30 = W
 *	14 = F			31 = X
 *	15 = G			32 = Y
 *	16 = H			33 = Z
 */

/**
 * OBSERVACIÓN:
 * Debido a que el RFC no cambia debido a un cambio en el tipo de sociedad
 * es posible que la clave sea diferente a la que se obtiene con el actual
 * nombre.
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef INERE_HOMONIMIA_INCLUDED_H
#include "inere/homonimia.h"
#endif

/**
 * Anexo I
 */
int
anexo1(const char caracter)
{
  char c = caracter;
  if ( c == ' ' || c == '0' ) return 0;
  else if ( c == '1' ) return 1;
  else if ( c == '2' ) return 2;
  else if ( c == '3' ) return 3;
  else if ( c == '4' ) return 4;
  else if ( c == '5' ) return 5;
  else if ( c == '6' ) return 6;
  else if ( c == '7' ) return 7;
  else if ( c == '8' ) return 8;
  else if ( c == '9' ) return 9;
  else if ( c == '&' ) return 10;
  else if ( c == 'A' || c == 'a' ) return 11;
  else if ( c == 'B' || c == 'b' ) return 12;
  else if ( c == 'C' || c == 'c' ) return 13;
  else if ( c == 'D' || c == 'd' ) return 14;
  else if ( c == 'E' || c == 'e' ) return 15;
  else if ( c == 'F' || c == 'f' ) return 16;
  else if ( c == 'G' || c == 'g' ) return 17;
  else if ( c == 'H' || c == 'h' ) return 18;
  else if ( c == 'I' || c == 'i' ) return 19;
  else if ( c == 'J' || c == 'j' ) return 21;
  else if ( c == 'K' || c == 'k' ) return 22;
  else if ( c == 'L' || c == 'l' ) return 23;
  else if ( c == 'M' || c == 'm' ) return 24;
  else if ( c == 'N' || c == 'n' ) return 25;
  else if ( c == 'O' || c == 'o' ) return 26;
  else if ( c == 'P' || c == 'p' ) return 27;
  else if ( c == 'Q' || c == 'q' ) return 28;
  else if ( c == 'R' || c == 'r' ) return 29;
  else if ( c == 'S' || c == 's' ) return 32;
  else if ( c == 'T' || c == 't' ) return 33;
  else if ( c == 'U' || c == 'u' ) return 34;
  else if ( c == 'V' || c == 'v' ) return 35;
  else if ( c == 'W' || c == 'w' ) return 36;
  else if ( c == 'X' || c == 'x' ) return 37;
  else if ( c == 'Y' || c == 'y' ) return 38;
  else if ( c == 'Z' || c == 'z' ) return 39;
  else if ( c == '^' ) return 40; /* This is for the Ñ which is not a single character */
  else return -1;
  return -1;
}

/**
 * Anexo II
 */
char
anexo2(const unsigned int n)
{
  if ( n == 0 ) return '1';
  else if ( n == 1 ) return '2';
  else if ( n == 2 ) return '3';
  else if ( n == 3 ) return '4';
  else if ( n == 4 ) return '5';
  else if ( n == 5 ) return '6';
  else if ( n == 6 ) return '7';
  else if ( n == 7 ) return '8';
  else if ( n == 8 ) return '9';
  else if ( n == 9 ) return 'A';
  else if ( n == 10 ) return 'B';
  else if ( n == 11 ) return 'C';
  else if ( n == 12 ) return 'D';
  else if ( n == 13 ) return 'E';
  else if ( n == 14 ) return 'F';
  else if ( n == 15 ) return 'G';
  else if ( n == 16 ) return 'H';
  else if ( n == 17 ) return 'I';
  else if ( n == 18 ) return 'J';
  else if ( n == 19 ) return 'K';
  else if ( n == 20 ) return 'L';
  else if ( n == 21 ) return 'M';
  else if ( n == 22 ) return 'N';
  else if ( n == 23 ) return 'P';
  else if ( n == 24 ) return 'Q';
  else if ( n == 25 ) return 'R';
  else if ( n == 26 ) return 'S';
  else if ( n == 27 ) return 'T';
  else if ( n == 28 ) return 'U';
  else if ( n == 29 ) return 'V';
  else if ( n == 30 ) return 'W';
  else if ( n == 31 ) return 'X';
  else if ( n == 32 ) return 'Y';
  else if ( n == 33 ) return 'Z';
  else return ' ';
  return ' ';
}

/**
 * El algoritmo para obtener el par de caracteres diferenciadores de homonimias
 * es implementado de la siguiente manera:
 *
 *	- Cada uno de los caracteres que conforman el nombre completo, o razon
 *	  social son representados por sus valores numericos, segun el Anexo I.
 *	- Todos estos valores numericos deberan sumarse, de la siguiente manera:
 *		o Por cada caracter se obtendran dos sumandos. El primero
 *		  sumando se forma sumando el ultimo digito del caracter previo
 *		  multiplicado por 10 mas el digito que representa la decena del
 *		  actual caracter, todo esto multiplicado por este ultimo
 *		  numero. El segundo sumando se obtiene multiplicando el valor
 *		  correspondiente al caracter actual por el ultimo digito de
 *		  este mismo numero. Es importante mencionar que sera necesario
 *		  conservar el ultimo digito del caracter actual, porque se
 *		  requerira para la formacion del primer sumando.
 */
unsigned int
decena(const unsigned int valor)
{
  if	  ( valor >= 30 ) return 3;
  else if ( valor >= 20 ) return 2;
  else if ( valor >= 10 ) return 1;
  else			  return 0;
  return 0;
}

char*
homonimia(char* clave, const char* nombre, const char* apellidos, const int debug)
{
  size_t namelen = strlen(nombre);
  size_t lastlen = 0;
  char* copy = NULL;
  unsigned int valor = 0;
  unsigned int digito_anterior = 0;
  int suma = 0;
  div_t result;

  if ( apellidos == NULL ) {
    copy = (char* )calloc(namelen + 1, sizeof(char));
#if _MSC_VER
    strncpy_s(copy, namelen+1, nombre, namelen);
#else
    strncpy(copy, nombre, namelen);
#endif
  } else {
    lastlen = strlen(apellidos);
    copy = (char* )calloc(namelen + lastlen + 2, sizeof(char));
#if _MSC_VER
    strncpy_s(copy, lastlen+1, apellidos, lastlen);
    strncat_s(copy, lastlen+1, " ", 1);
    strncat_s(copy, lastlen+1, nombre, namelen);
#else
    strncpy(copy, apellidos, lastlen);
    strncat(copy, " ", 1); /* Add an space */
    strncat(copy, nombre, namelen); /* The order is important */
#endif
  }

  /* Now convert all the characters to the numeric values defined in the
     Anexo 1 */
  while ( *copy ) {
    if ( isalnum(*copy) || *copy == '&' || isspace(*copy) ) {
      /* Primer suma */
      valor = anexo1(*copy);
      if ( debug ) {
	printf("Caracter [%c] = %2d\t", *copy, valor);
	printf("(%2d * %d = %3d) + ", (digito_anterior * 10 + decena(valor) ), decena(valor), (digito_anterior * 10 + decena(valor) ) * decena(valor));
      }
      suma += (digito_anterior * 10 + decena(valor) ) * decena(valor);

      /* Segunda suma */
      digito_anterior = valor % 10;
      if ( debug ) printf("(%2d * %d = %3d) : ", valor, digito_anterior, valor * digito_anterior);
      suma += valor * digito_anterior;
      if ( debug ) printf(" %4d\t(suma acumulada)\n", suma);
    }
    copy++;
  }
  if ( apellidos != NULL ) copy -= namelen + lastlen + 1;
  else			  copy -= namelen;
  free(copy);

  /* Now, obtain the two characters as for the homonimia */
  suma = suma % 1000; /* Toma unicamente las tres ultimas cifras */
  result = div(suma, 34);

#if _MSC_VER
  _snprintf_s(clave, 3, 3, "%c%c", anexo2(result.quot), anexo2(result.rem));
#else
  snprintf(clave, 3, "%c%c", anexo2(result.quot), anexo2(result.rem));
#endif
  if ( debug ) printf("\tCoeficiente [%d], residuo [%d]\thomonimia [%s]\n", result.quot, result.rem, clave);
  return clave;
}
