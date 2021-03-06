/*
 * Copyright (c) 2012-2015, L3a,
 *			    Enrique Gámez Flores <egamez@edisson.com.mx>
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
 * ANEXO V (personas fisicas)
 * Lista de palabras que no se utilizan en la formacion de la clave del
 * registro federal de contribuyentes.
 *
 *	DE
 *	LA
 *	LAS
 *	MC
 *	VON
 *	DEL
 *	LOS
 *	Y
 *	MAC
 *	VAN
 *	MI
 *
 *
 * ANEXO VI (personas fisicas)
 * Lista de caracteres especiales y valores que tomaran solo si estan en forma
 * individual dentro del texto del nombre o denominacion social.
 *
 *	'	APOSTROFE
 *	.	PUNTO
 *
 *
 */
#ifndef INERE_PERSONAFISICA_H_
#include "inere/personafisica.h"
#endif
#ifndef INERE_HOMONIMIA_H_
#include "inere/homonimia.h"
#endif
#ifndef INERE_VERIFICADOR_H_
#include "inere/verificador.h"
#endif
#ifndef INERE_UTIL_H_
#include "inere/util.h"
#endif

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

/* Forward declaration */
void fisica_regla1(const char *paterno, const char *materno, const char *nombre, char *result);
char *regla2(const char* year, const char* month, const char* day, char* result);
void fisica_regla3(const char *paterno, const char *materno, const char* nombre, char *result);
void fisica_regla4(const char *paterno, const char *materno, const char *nombre, char *result);
void fisica_regla5(const char *paterno, const char *materno, const char *nombre, char *result);
int fisica_regla6(char* nombre, const int debug);
void fisica_regla7(const char *apellido, const char *nombre, char *result);
int fisica_regla8(char* palabra, int debug);
int fisica_regla9(char *clave, int debug);


/**
 * Regla 1a.
 * Se integra la clave con los siguientes datos:
 *
 *	1. La primera letra del apellido paterno y la siguiente primera vocal
 *	   del mismo.
 *	2. La primera letra del apellido maternos.
 *	3. La primera letra del nombre.
 *
 * Ejemplo:
 *
 *	Juan Barrios Fernandez
 *			Apellido paterno:	Barrios		BA
 *			Apellido materno:	Fernandez	F
 *			Nombre:			Juan		J
 *			Resultado de la expresion alfabetica:	BAFJ
 *	Eva Iriarte Mendez
 *			Apellido paterno:	Iriarte		II
 *			Apellido materno:	Mendez		M
 *			Nombre:			Eva		E
 *			Resultado de la expresion alfabetica:	IIME
 *
 */
void
fisica_regla1(const char *paterno, const char *materno, const char *nombre, char *result)
{
  result[0] = *paterno++;
  while ( *paterno ) {
    if ( *paterno == 'A' || *paterno == 'E' ||
	 *paterno == 'I' || *paterno == 'O' ||
	 *paterno == 'U' ) {
      result[1] = *paterno;
      break;
    }
    paterno++;
  }
  result[2] = *materno;
  result[3] = *nombre;
  result[4] = 0;
}

/**
 * Regla 2a.
 * A continuacion se anotara la fecha de nacimiento del contribuyente, en el
 * siguiente orden:
 *
 *	Ano: Se tomaran las dos ultimas cifras, escribiendolas con numeros
 *	     arabigos.
 *	Mes: Se tomara el mes de nacimiento en su numero de orden, en un ano
 *	     calendario, escribiendolo con numeros arabigos.
 *	Dia: Se escribira con numeros arabigos.
 *
 * Ejemplos:
 *
 *	Ano	1970						70
 *	Mes	Diciembre					12
 *	Dia	13						13
 *		Dando como resultado la expresion numerica:	701213
 *
 * Y complementando con la expresion alfabetica numerica tendremos:
 *
 *	Juan Barrios Fernandez					BAFJ 701213
 *	Eva Iriarte Mendez					IIME 691117
 *
 * Cuando en el ano, mes, o dia, de la fecha de nacimiento, aparezcan solamente
 * un guarismo se le antepondra un cero.
 * Ejemplos:
 *
 *	Ano	1907						07
 *	Mes	Abril						04
 *	Dia	1o						01
 *		Como resultado tendresmo la expresion numerica: 070401
 *
 * Y completando la clave alfabetico numerica tendremos:
 *
 *	Juan Barrios Fernandez					BAFJ-070401
 *	Francisco Ortiz Perez					OIPF-290205
 *	Manuel Martinez Hernandez				MAHM-570102
 *	Gabriel Courturier Moreno				COMG-600703
 * OK!
 */
char *
regla2(const char* year, const char* month, const char* day, char* result)
{
  int ano = 0;
  int mes = 0;
  int dia = 0;

  ano = atoi(year) % 100;
  mes = atoi(month);
  dia = atoi(day);

#if _MSC_VER
  _snprintf_s(result, 7, 7, "%02d%02d%02d", ano, mes, dia);
#else
  snprintf(result, 7, "%02d%02d%02d", ano, mes, dia);
#endif
  return result;
}

/**
 * Regla 3a.
 * Cuando la letra inicial de cualquiera de los apellidos o nombre sea compuesta
 * unicamente se anotara la inicial de esta. En la Ch la C y en la Ll la L.
 *
 * Ejemplos:
 *
 *	Manuel Chavez Gonzalez					CAGM-240618
 *	Felipe Camargo Llamas					CALF-450228
 *	Charles Kennedy Truman					KETC-511012
 * OK!
 */
void
fisica_regla3(const char *paterno, const char *materno, const char* nombre, char *result)
{
  /* The strings passed must no contain any 'letra compuesta' */
  fisica_regla1(paterno, materno, nombre, result);
}


/**
 * Regla 4a.
 * En los casos en que el apellido paterno de la persona fisica se componga de
 * una o de dos letras, la clave se formara de la siguiente manera:
 *
 *	1. La primera letra del apellido paterno.
 *	2. La primera letra del apellido materno.
 *	3. La primera y segunda letra del nombre.
 *
 * Ejemplo:
 *
 *	Alvaro de la O Lozano					OLAL-401201
 *	Ernesto Ek Rivera					ERER-071120
 * OK!
 */
void
fisica_regla4(const char *paterno, const char *materno, const char *nombre, char *result)
{
  result[0] = *paterno;
  result[1] = *materno;
  result[2] = *nombre++;
  result[3] = *nombre;
  result[4] = 0;
}


/**
 * Regla 5a.
 * Cuando el apellido paterno o materno sean compuestos, se tomara para la
 * clasificacion de la primera palabra que corresponda a cualquiera de ellos.
 *
 * Ejemplos:
 *
 *	Dolores San Martin Davalos				SADD-180812
 *	Mario Sanchez de la Barquera Gomez			SAGM-190224
 *	Antonio Jimenez Ponce de Leon				JIPA-170808
 * OK!
 */
void
fisica_regla5(const char *paterno, const char *materno, const char *nombre, char *result)
{
  /* All the 'apellidos' must not contain not usbale words */
  fisica_regla1(paterno, materno, nombre, result);
}

/**
 * Regla 6a.
 * Cuando el nombre es compuesto, o sea, que esta formado por dos o mas palabras
 * se tomara, para la conformacion la letra inicial de la primera, siempre que
 * no sea MARIA o JOSE dado su frecuente uso, en cuyo caso se tomara la primera
 * letra de la segunda palabra.
 *
 * Ejemplos:
 *
 *	Luz Maria Fernandez Juarez				FEJL-200205
 *	Jose Antonio Camargo Hernandez				CAHA-211218
 *	Maria Luisa Ramirez Sanchez				RASL-25112
 */
int
fisica_regla6(char* nombre, const int debug)
{
  /* Break the nombre onto its words */
  char** ap = NULL;
  char* word[10];
  int excluded = 0;
  int counter = 0;
  int i = 0;
  size_t len = 0;
  char *copy = NULL;
  int applied = 0;

  len = strlen(nombre);
  copy = strdupa(nombre);
  memset(nombre, 0, len+1);

  for (ap = word; ap < &word[9] && (*ap = strsep(&copy, " \t\n")) != NULL; ) {
    if ( **ap != '\0' ) ap++;
    counter++;
  }
  *ap = NULL;

  /* Now make a loop for all the words in nombre, and append only those
   * which are not MARIA or JOSE.
   *
   * If there name consist of one single word, do not apply the rule.
   *
   */
  if ( counter > 1 ) {
    for (i = 0; i < counter; i++ ) {
      excluded = 0;
      if ( strcmp(word[i], "MARIA") == 0 || strcmp(word[i], "MA.") == 0 ||
	   strcmp(word[i], "JOSE" ) == 0 || fisica_regla8(word[i], debug) ) {
	excluded = 1;
	applied = 1;
	if ( debug ) printf("Eliminando la palabra \"%s\".\n", word[i]);
      }

      if ( !excluded ) {
	if ( strlen(nombre) ) strncat(nombre, " ", 1);
	strncat(nombre, word[i], strlen(word[i]));
      }

    }
  } else {
    strncat(nombre, word[0], strlen(word[0]));
  }

  /* Check if this was not the case in which the names are those who
   * need to be supressed. If this is the case take the last name.
   */
  if ( !strlen(nombre) ) {
    strncat(nombre, word[counter-1], strlen(word[counter-1]));
  }
  return applied;
}

/**
 * Regla 7a.
 * En los casos en que la persona fisica tenga un solo apellido, se conformara
 * con la primera y segunda letras del apellido paterno o materno, segun figure
 * en el acta de nacimiento, mas la primera y segunda letras del nombre.
 *
 * Ejemplos:
 *
 *	Juan Martinez						MAJU-420116
 *	Gerardo Zafra						ZAGE-251125
 * OK!
 */
void
fisica_regla7(const char *apellido, const char *nombre, char *result)
{
  memset(result, 0, 5);
  result[0] = *apellido++;
  while ( *apellido ) {
    if ( *apellido == 'A' || *apellido == 'E' || *apellido == 'I' ||
	 *apellido == 'O' || *apellido == 'U' ) {
      result[1] = *apellido;
      break;
    }
    apellido++;
  }
  result[2] = *nombre++;
  result[3] = *nombre;
  result[4] = 0;
}


/**
 * Regla 8a.
 * Cuando en el nombre de las personas fisicas figuren articulos, preposiciones
 * conjunciones, o contracciones no se tomaran como elementos de integracion de
 * la clave, ejemplos:
 *
 *	Carmen de la Pena Ramirez				PERC-631201
 *	Mario Sanchez de los Cobos				SACM-700110
 *	Roberto Gonzalez y Duran				GODR-600101
 *	Juan del Valle Martinez					VAMJ-691001
 */
int
fisica_regla8(char* palabra, int debug)
{
  int excluded = 0;
  char **ap = NULL;
  char *word[10];
  int counter = 0;
  int i = 0;
  size_t len = 0;
  char *copy = NULL; /* To free */
  int applied = 0;

  len = strlen(palabra);
  copy = strdupa(palabra);
  memset(palabra, 0, len+1);


  /* Split the palabra onto its words */
  for (ap = word; ap < &word[9] && (*ap = strsep(&copy, " \t\n")) != NULL;) {
   if (**ap != '\0') {
     ap++;
   }
   counter++;
  }
  *ap = NULL;

  for (i = 0; i < counter; i++ ) {

    excluded = 0;

    if      ( strcmp(word[i], "DE")  == 0 ) excluded = 1;
    else if ( strcmp(word[i], "DEL") == 0 ) excluded = 1;
    else if ( strcmp(word[i], "LA")  == 0 ) excluded = 1;
    else if ( strcmp(word[i], "LAS") == 0 ) excluded = 1;
    else if ( strcmp(word[i], "LOS") == 0 ) excluded = 1;
    else if ( strcmp(word[i], "MC")  == 0 ) excluded = 1;
    else if ( strcmp(word[i], "MAC") == 0 ) excluded = 1;
    else if ( strcmp(word[i], "MI")  == 0 ) excluded = 1;
    else if ( strcmp(word[i], "VAN") == 0 ) excluded = 1;
    else if ( strcmp(word[i], "VON") == 0 ) excluded = 1;
    else if ( strcmp(word[i], "Y")   == 0 ) excluded = 1;
    else {
      if ( strlen(palabra) ) strncat(palabra, " ", 1);
      strncat(palabra, word[i], strlen(word[i]));
    }

    if ( excluded ) {
      if ( debug ) printf("Eliminando la palabra \"%s\".\n", word[i]);
      applied = 1;
    }

  }

  return applied;
}

/**
 * Regla 9a.
 * Cuando de las cuatro letras que formen la expresion alfabetica, resulte una
 * palabra inconveniente, la ultima letra sera sustituida por una "X".
 *
 *	BUEI	BUEX				KOGE	KOGX
 *	BUEY	BUEX				KOJO	KOJX
 *	CACA	CACX				KAKA	KAKX
 *	CACO	CACX				KULO	KULX
 *	CAGA	CAGX				MAME	MAMX
 *	CAGO	CAGX				MAMO	MAMX
 *	CAKA	CAKX				MEAR	MEAX
 *	COGE	COGX				MEON	MEOX
 *	COJA	COJX				MION	MIOX
 *	COJE	COJX				MOCO	MOCX
 *	COJI	COJX				MULA	MULX
 *	COJO	COJX				PEDA	PEDX
 *	CULO	CULX				PEDO	PEDX
 *	FETO	FETX				PENE	PENX
 *	GUEY	GUEX				PUTA	PUTX
 *	JOTO	JOTX				PUTO	PUTX
 *	KACA	KACX				QULO	QULX
 *	KACO	KACX				RATA	RATX
 *	KAGA	KAGX				RUIN	RUIX
 *	KAGO	KAGX
 */
int
fisica_regla9(char *clave, int debug)
{
  int substituted = 0;
  char* correcta = NULL;

  if      ( strcmp(clave, "BUEI") == 0 ) correcta = "BUEX";
  else if ( strcmp(clave, "BUEI") == 0 ) correcta = "BUEX";
  else if ( strcmp(clave, "BUEY") == 0 ) correcta = "BUEX";
  else if ( strcmp(clave, "CACA") == 0 ) correcta = "CACX";
  else if ( strcmp(clave, "CACO") == 0 ) correcta = "CACX";
  else if ( strcmp(clave, "CAGA") == 0 ) correcta = "CAGX";
  else if ( strcmp(clave, "CAGO") == 0 ) correcta = "CAGX";
  else if ( strcmp(clave, "CAKA") == 0 ) correcta = "CAKX";
  else if ( strcmp(clave, "COGE") == 0 ) correcta = "COGX";
  else if ( strcmp(clave, "COJA") == 0 ) correcta = "COJX";
  else if ( strcmp(clave, "COJE") == 0 ) correcta = "COJX";
  else if ( strcmp(clave, "COJI") == 0 ) correcta = "COJX";
  else if ( strcmp(clave, "COJO") == 0 ) correcta = "COJX";
  else if ( strcmp(clave, "CULO") == 0 ) correcta = "CULX";
  else if ( strcmp(clave, "FETO") == 0 ) correcta = "FETX";
  else if ( strcmp(clave, "GUEY") == 0 ) correcta = "GUEX";
  else if ( strcmp(clave, "JOTO") == 0 ) correcta = "JOTX";
  else if ( strcmp(clave, "KACA") == 0 ) correcta = "KACX";
  else if ( strcmp(clave, "KACO") == 0 ) correcta = "KACX";
  else if ( strcmp(clave, "KAGA") == 0 ) correcta = "KAGX";
  else if ( strcmp(clave, "KAGO") == 0 ) correcta = "KAGX";
  else if ( strcmp(clave, "KOGE") == 0 ) correcta = "KOGX";
  else if ( strcmp(clave, "KOJO") == 0 ) correcta = "KOJX";
  else if ( strcmp(clave, "KAKA") == 0 ) correcta = "KAKX";
  else if ( strcmp(clave, "KULO") == 0 ) correcta = "KULX";
  else if ( strcmp(clave, "MAME") == 0 ) correcta = "MAMX";
  else if ( strcmp(clave, "MAMO") == 0 ) correcta = "MAMX";
  else if ( strcmp(clave, "MEAR") == 0 ) correcta = "MEAX";
  else if ( strcmp(clave, "MEON") == 0 ) correcta = "MEOX";
  else if ( strcmp(clave, "MION") == 0 ) correcta = "MIOX";
  else if ( strcmp(clave, "MOCO") == 0 ) correcta = "MOCX";
  else if ( strcmp(clave, "MULA") == 0 ) correcta = "MULX";
  else if ( strcmp(clave, "PEDA") == 0 ) correcta = "PEDX";
  else if ( strcmp(clave, "PEDO") == 0 ) correcta = "PEDX";
  else if ( strcmp(clave, "PENE") == 0 ) correcta = "PENX";
  else if ( strcmp(clave, "PUTA") == 0 ) correcta = "PUTX";
  else if ( strcmp(clave, "PUTO") == 0 ) correcta = "PUTX";
  else if ( strcmp(clave, "QULO") == 0 ) correcta = "QULX";
  else if ( strcmp(clave, "RATA") == 0 ) correcta = "RATX";
  else if ( strcmp(clave, "RUIN") == 0 ) correcta = "RUIX";

  if ( correcta != NULL ) {
    if ( debug )
      printf("Palabra inconveniente para formar parte de la clave del Registro Federal de Contribuyentes (%s)... sustituida por \"%s\".\n", clave, correcta);

    substituted = 1;

    memset(clave, 0, 5);
#if _MSC_VER
    strncpy_s(clave, 5, correcta, 4);
#else
    strncpy(clave, correcta, 4);
#endif
  }

  return substituted;
}

/**
 * Regla 10a.
 * Cuando aparezcan formando parte del nombre, apellido paterno y materno los
 * caracteres especiales, estos deben de excluirse para el calculo del homonimo
 * y del digito verificador. Los caracteres se interpretaran, si y solo si,
 * estan en forma individual dentro del nombre, apellido paterno y apellido
 * materno. Ejemplos:
 *
 *	Roberto O'farril Carballo				OACR-661121
 *	Ruben D'angelo Fargo					DAFR-710108
 *	Luz Ma. Fernandez Juarez				FEJL-830120
 *
 */

/**
 * Observacion:
 * En caso de la mujer, se debera usar el nombre de soltera.
 */


/**
 * Esta funcion
 */
char*
fisica_clave_abreviada(char* clave, const char *nombre, const char *paterno, const char *materno, const char *year, const char* month, const char *day, const int debug)
{
  char result[5];
  char fecha[7];
  int bad_word = 0;
  int res = 0;

  char* name = NULL;
  char* lastname1 = NULL;
  char* lastname2 = NULL;

  memset(result, 0, 5);
  memset(fecha, 0, 7);

  /*
   * Nombre
   */
#if _MSC_VER
  name = _strdup(nombre);
#else
  name = strndup(nombre, strlen(nombre));
#endif

  /*
   * Regla 8. Elimina del nombre articulos, preposiciones, conjunciones, etc.
   */
  res = 0;
  res = fisica_regla8(name, debug);
  if ( debug && res ) {
    printf("Regla 8 aplicada a \"%s\". resultado \"%s\".\n", nombre, name);
  }

  /*
   * Regla 6. Elimina del nombre las palabras Maria o Jose
   */
  res = 0;
  res = fisica_regla6(name, debug);
  if ( debug && res ) {
    printf("Regla 6 aplicada al nombre \"%s\", resultado \"%s\".\n", nombre, name);
  }

  /*
   * Apellido paterno
   */
#if _MSC_VER
  lastname1 = _strdup(paterno);
#else
  lastname1 = strndup(paterno, strlen(paterno));
#endif
  res = 0;
  res = fisica_regla8(lastname1, debug);
  if ( debug && res ) {
    printf("Regla 8 aplicada a \"%s\". resultado \"%s\".\n", paterno, lastname1);
  }

  /*
   * Apellido materno
   */
  if ( materno != NULL ) {
#if _MSC_VER
    lastname2 = _strdup(materno);
#else
    lastname2 = strndup(materno, strlen(materno));
#endif
    res = 0;
    res = fisica_regla8(lastname2, debug);
    if ( debug && res ) {
      printf("Regla 8 aplicada a \"%s\". resultado \"%s\".\n", materno, lastname2);
    }
  }

  /*
   * Regla 4
   */
  if ( strlen(lastname1) <= 2 ) {
    if ( lastname2 != NULL ) {
      if ( debug ) {
	printf("Regla 4 aplicada. Apellido paterno (%s) compuesto de una o dos letras unicamente", lastname1);
      }
      fisica_regla4(lastname1, lastname2, name,result);
    } else {
      if ( debug ) printf(". Regla 7 aplicada. No hay segundo apellido");
      fisica_regla7(lastname1, name, result);
    }
    if ( debug ) printf(".\n");
  }

  /*
   * Regla 7
   */
  if ( lastname2 == NULL ) {
    if ( debug ) printf("Regla 7 aplicada. No hay segundo apellido.\n");
    fisica_regla7(lastname1, name, result);
  }

  /*
   * Default rule. Rule 1
   */
  if ( strlen(result) == 0 ) {
    if ( debug ) printf("Regla 1.\n");
    fisica_regla1(lastname1, lastname2, name, result);
  }
  
  if ( lastname2 ) free(lastname2);
  free(lastname1);
  free(name);

  /* Now, verify that the current 'clave' does not match any offensive word */
  bad_word = fisica_regla9(result, debug);
  if ( debug && bad_word ) {
    printf("Regla 9 aplicada. La expresion alfabetica obtenida para formar la clave del RFC es una palabra inconveniente.\n");
  }

  if ( debug ) printf("Clave [%s].\n", result);

  /* Now compose the clave */
  regla2(year, month, day, fecha);
  if ( debug ) printf("Fecha [%s].\n", fecha);

#if _MSC_VER
  _snprintf_s(clave, 11, 11, "%s%s", result, fecha);
#else
  snprintf(clave, 11, "%s%s", result, fecha);
#endif

  return clave;
}


char*
clave_rfc_persona_fisica(char* clave, const char *nombre, const char *paterno, const char *materno, const char *year, const char* month, const char *day, const int debug)
{
  char clave_diferenciadora[3];
  char verificador[2];
  char digito = 0;
  char* unombre = 0;
  char* upaterno = 0;
  char* umaterno = 0;
  char* apellidos = NULL;
  size_t apellidos_len = 0;
  char tmp_clave[14];

  unombre = to_upper_case_and_convert((unsigned char*)nombre);
  upaterno = to_upper_case_and_convert((unsigned char*)paterno);
  if ( materno != NULL ) {
    /* Existe la posibilidad de que 'materno' este vacio */
    if ( strlen(materno) ) {
      umaterno = to_upper_case_and_convert((unsigned char*)materno);
    }
  }

  memset(tmp_clave, 0, 14);
  fisica_clave_abreviada(tmp_clave, unombre, upaterno, umaterno, year, month, day, debug);

  /* Ahora obten la clave diferenciadora de homonimias */
  apellidos_len = strlen(upaterno);
  if ( umaterno != NULL ) {
     apellidos_len += strlen(umaterno) + 2;
     apellidos = (char*)calloc(apellidos_len, sizeof(char));
     memset(apellidos, 0, apellidos_len);
#if _MSC_VER
     strncpy_s(apellidos, strlen(upaterno)+1, upaterno, strlen(upaterno));
     strncat_s(apellidos, apellidos_len, " ", 1);
     strncat_s(apellidos, apellidos_len, umaterno, strlen(umaterno));
#else
     strncpy(apellidos, upaterno, strlen(upaterno));
     strncat(apellidos, " ", 1);
     strncat(apellidos, umaterno, strlen(umaterno));
#endif

  } else {
    apellidos = (char*)calloc(apellidos_len + 1, sizeof(char));
    memset(apellidos, 0, apellidos_len + 1);
#if _MSC_VER
    strncpy_s(apellidos, apellidos_len+1, upaterno, apellidos_len);
#else
    strncpy(apellidos, upaterno, apellidos_len);
#endif

  }

  /* Ahora obten la clave diferenciadora de homonimias */
  memset(clave_diferenciadora, 0, 3);
  homonimia(clave_diferenciadora, unombre, apellidos, debug);

  if ( umaterno != NULL ) free(umaterno);
  free(upaterno);
  free(unombre);

  /* Agrega la clave */
#if _MSC_VER
  strncat_s(tmp_clave, 14, clave_diferenciadora, 2);
#else
  strncat(tmp_clave, clave_diferenciadora, 2);
#endif

  /* Libera memoria */
  free(apellidos);

  /* Ahora obten el digito verificador */
  digito = digito_verificador(tmp_clave, debug);
#if _MSC_VER
  _snprintf_s(verificador, 2, 2, "%c", digito);
  strncat_s(tmp_clave, 14, verificador, 1);
#else
  snprintf(verificador, 2, "%c", digito);
  strncat(tmp_clave, verificador, 1);
#endif

  /* Now translate any '^' which stands for a 'Ñ' */
  memset(clave, 0, 18);
  clave = (char*)recover_translations(tmp_clave, (unsigned char*)clave);

  return clave;
}
