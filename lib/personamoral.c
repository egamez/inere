/*
 * Copyright (c) 2013,2015, L3a,
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
#ifndef INERE_PERSONAMORAL_H_
#include "inere/personamoral.h"
#endif
#ifndef INERE_NUMERAL_ALLOC_H_
#include "inere/numeral_alloc.h"
#endif
#ifndef INERE_HOMONIMIA_H_
#include "inere/homonimia.h"
#endif
#ifndef INERE_VERIFICADOR_H_
#include "inere/verificador.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/* Forward declarations */
void split(const char *name, char ***words, size_t *len, const int verbose);
char *moral_regla1(char* result, char *palabras[], const int verbose);
char *moral_regla2(const char* year, const char* month, const char* day, char* result);
char *moral_regla3(char *palabra, const int verbose);
char *moral_regla6(char* result, char *palabras[], const int verbose);
char *moral_regla7(char* result, const char *palabra, const int verbose);
char *moral_regla8(char* result, const char *palabra, const int verbose);
int moral_regla10(char *numero, char *result[], const int verbose);
char *moral_regla12(char *palabra, const int verbose);
char *remove_punctuation(char* word, int verbose);


/**
 * Anexo VI.
 * Lista de caracteres especiales y valores que tomarán sólo si están en forma
 * individual dentro del texto del nombre o denominación social.
 *
 *	PERSONAS MORALES			PERSONAS FÍSICAS
 *	CARÁCTER	VALOR			CARÁCTER	VALOR
 *	@		ARROBA			´		APOSTROFE
 *	´		APOSTROFE		.		PUNTO
 *	%		PORCIENTO
 *	#		NUMERO
 *	!		ADMIRACION
 *	.		PUNTO
 *	$		PESOS
 *	"		COMILLAS
 *	-		GUION
 *	/		DIAGONAL
 *	+		SUMA
 *	(		ABRE PARENTESIS
 *	)		CIERRA PARENTESIS
 *
 */

/**
 * Regla 1a.
 *
 * Para la formación de la clave se tomarán como elemento de orden, las tres
 * primeras letras de las tres primeras palabras de la denominación o razón
 * social tal y como esté manifestando en la copia del acta constitutiva o en
 * caso en la copia del documento que motivó su origen, ejemplo:   		
 *
 *	Sonora Industrial Azucarera, S. de R. L.
 *				Primera Palabra: Sonora		S 
 *				Segunda palabra: Industrial	I
 *				Tercera palabra: Azucarera	A
 *	   Resultado de la expresión alfabética:		SIA
 *
 */
char *
moral_regla1(char* result, char *palabras[], const int verbose)
{
  memset(result, 0, 4);
  if ( verbose )
    printf("Aplicando regla 1:\n\t%-20s %c\n\t%-20s %c\n\t%-20s %c\n", palabras[0],
							toupper(*palabras[0]),
							palabras[1],
							toupper(*palabras[1]),
							palabras[2],
							toupper(*palabras[2]));
  snprintf(result, 4, "%c%c%c", toupper(*palabras[0]), toupper(*palabras[1]), toupper(*palabras[2]));
  return result;
}

/**
 * Regla 2a.
 * A continuación se anotará la fecha de constitución o creación, en el
 * siguiente orden:
 *
 *	Año: Se tomarán las dos últimas cifras, escribiéndolas con números
 *	     arábigos.
 *	Mes: Se tomará el mes de constitución o creación, en su número de
 *	     orden, en un año calendario escribiéndolo con números arábigos. 
 *	Día: Se escribirá en números arábigos.
 *
 * Ejemplo:
 *
 *		Año:	1982				82
 *		Mes:	Noviembre			11 
 *		Día:	29				29
 *
 * Dando como resultado la expresión numérica: 821129
 * Y completando con la expresión alfabético numérica tendremos:
 *
 *	Sonora Industrial Azucarera, S. de. R.L.	SIA-821129
 *	Herrajes, Cortinas y Maquinaria, S.A.		HCM-841122
 *	Artículos de piel y Baúles, S. de R. L.		APB-791215
 *
 * Cuando en el mes o día, de la fecha de constitución, aparezca solamente un
 * guarismo se le antepondrá un cero, ejemplos:
 *
 *		Año:	1983				83
 *		Mes:	Marzo				03
 *		Día:	5				05
 *
 * Como resultado tendremos la expresión numérica:	830305
 *
 * Y contemplando la clave alfabético numérica tendremos:
 *
 *	Tecnología y Equipo contra Incendios, S.A.	TEC-830305
 *	Internacional Turística Flacón, S.A.		ITF-850128
 *	Artículos de Caza y Pesca, S. de R. L.		ACP-860215
 *
 */
char *
moral_regla2(const char* year, const char* month, const char* day, char* result)
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
 * Cuando la letra inicial de cualquiera de las tres primeras palabras de la
 * denominación o razón social sea compuesta, únicamente se anotará la inicial
 * de esta. En la Ch la C y en la Ll la L, ejemplos:
 *
 *	Champion Mexicana de Bujías, S.A.		CMB-830702
 *	Casa Chávez de maquinaria, S. de R. L.		CCM-800620
 *	Artículos de Piel y Chamarras, S. de R.L.	APC-810202
 *	Llantas, Cámaras y Refacciones, S. de R.L.	LCR-851015
 *	Candados, Llaves y Cerraduras, S.A.		CLC-830820
 *	Luis Molina Llorantes y Cía., S. de R.L.	LML-860911
 *
 */
char *
moral_regla3(char *palabra, const int verbose)
{
  const size_t len = strlen(palabra);
  char* copy = NULL;
  if ( len == 1 ) return palabra;
  if (	(*palabra == 'C' && *(palabra+1) == 'H') ||
	(*palabra == 'C' && *(palabra+1) == 'h') ||
	(*palabra == 'c' && *(palabra+1) == 'h') ||
	(*palabra == 'c' && *(palabra+1) == 'H') ||
	(*palabra == 'L' && *(palabra+1) == 'L') ||
	(*palabra == 'L' && *(palabra+1) == 'l') ||
	(*palabra == 'l' && *(palabra+1) == 'L') ||
	(*palabra == 'l' && *(palabra+1) == 'l') ) {
    if ( verbose ) printf("moral_regla3: Aplicando...");
    copy = strndup(palabra, len);
    memset(palabra, 0, len + 1);
    *palabra++ = *copy++;
    copy++;
    while ( *copy ) *palabra++ = *copy++;
    *palabra = 0;
    palabra -= (len-1);
    copy -= len;
    free(copy);
    if ( verbose ) printf(" Ok!\n");
  }

  return palabra;
}

/**
 * Regla 4a.
 * En los casos en que la denominación o razón social esté compuesta sólo de
 * iniciales, para estos efectos se consideran palabras, por tanto deben
 * tomarse en cuenta como tales, ejemplos:
 *
 *	F.A.Z., S.A.					FAZ-870420
 *	U.S. Ruber Mexicana, S.A.			USR-860201
 *	H. Prieto y Martínez, S. de R.L.		HPM-841221
 *
 */

/**
 * Regla 5a.
 * Las abreviaturas: "S. en N.C.", "S. en C.", "S. de R.L.", "S. en C. por A.",
 * "S.A.", "S.A. de C.V.", "S.N.C.", "S.C.", "A.C.", "A. en P.", "S.C.L.",
 * "S.C.S.", que expresan tipo de sociedad en las denominaciones o razones
 * sociales, no se tomarán en consideración para efectos de conformación de las
 * claves, ejemplos:
 *
 *	Guantes Industriales Guadalupe, S. en C.	GIG-841215
 *	Construcciones Metálicas Mexicanas, S.A.	CMM-830120
 *	Fundición de Precisión Eutectic, S. de R.L.	FPE-861125 
 *
 */

/**
 * Regla 6a.
 * Si la denominación o razón social se comprende de dos elementos, para
 * efectos de la conformación de la clave, se tomará la letra inicial de la
 * primera palabra y las dos primeras letras de la segunda, ejemplos:
 *
 *	Fonograbaciones Cinelandia, S. de R.L.		FCI-841019
 *	Aceros Ecatepec, S.A. 				AEC-890130
 *	Distribuidora Ges, S.A.				DGE-850628
 *
 */
char *
moral_regla6(char* result, char *palabras[], const int verbose)
{
  memset(result, 0, 4);
  if ( strlen(palabras[1]) > 1 ) {
    if ( verbose )
      printf("Aplicando regla 6:\n\t%-20s %c\n\t%-20s %c%c\n", palabras[0],
							toupper(*palabras[0]),
							palabras[1],
							toupper(*palabras[1]),
							toupper(*(palabras[1]+1)));
    snprintf(result, 4, "%c%c%c", toupper(*palabras[0]), toupper(*palabras[1]), toupper(*(palabras[1]+1)));

  } else {
    /* In this case you must have to complement the three code letters
     * adding an X at the end.
     */
    if ( verbose )
      printf("Aplicando regla 6, y regla 8 (agregando una X al final para completar la clave):\n\t%-20s %c\n\t%-20s %c\n", palabras[0], toupper(*palabras[0]), palabras[1], toupper(*palabras[1]));

    snprintf(result, 4, "%c%cX", toupper(*palabras[0]), toupper(*palabras[1]));

  }

  return result;
}

/**
 * Regla 7a.
 * Si la denominación o razón social se compone de un solo elemento, para
 * efectos de conformación de la clave, se tomarán las tres primeras letras
 * consecutivas del mismo, ejemplo: 
 *
 *	Arsuyama, S.A.					ARS-821129
 *	Calidra, S.A.					CAL-850920
 *	Electrólisis, S.A.				ELE-840821
 *
 */
char *
moral_regla7(char* result, const char *palabra, const int verbose)
{
  const size_t len = strlen(palabra);

  memset(result, 0, 4);
  if ( len >= 3 ) {
    if ( verbose )
      printf("Aplicando regla 7:\n\t%-20s %c%c%c\n", palabra,
						toupper(*palabra),
						toupper(*(palabra+1)),
						toupper(*(palabra+2)));
    snprintf(result, 4, "%c%c%c", toupper(*palabra), toupper(*(palabra + 1)), toupper(*(palabra+2)));

  } else if ( len == 2 ) {
    /* In this case you must have to add one X at the end of the result */
    if ( verbose )
      printf("Aplicando regla 7, y regla 8 (agregando una X al final de la clave):\n\t%-20s %c%c\n",
		palabra,
		toupper(*palabra),
		toupper(*(palabra+1)));

    snprintf(result, 4, "%c%cX", toupper(*palabra), toupper(*(palabra + 1)));

  } else {
    /* In this case you must have to add two Xs at the end of the code */
    if ( verbose )
      printf("Aplicando regla 7, y regla 8 (agregando dos letras X al final de la clave):\n\t%-20s %c\n",
		palabra,
		toupper(*palabra));
    snprintf(result, 4, "%cXX", toupper(*palabra));

  }

  return result;
}

/**
 * Regla 8a.
 * Cuando la denominación o razón social se componga de un solo elemento y sus
 * letras no completen las tres requeridas, para efectos de conformación de la
 * clave, se tomaran las empleadas por el contribuyente y las restantes se
 * suplirán con una “X”, ejemplos: 
 *
 *	Al, S.A.					ALX-830101
 *	Z, S.A.						ZXX-860110
 *
 */
char *
moral_regla8(char* result, const char *palabra, const int verbose)
{
  const size_t len = strlen(palabra);

  memset(result, 0, 4);
  if ( len == 1 ) {
    snprintf(result, 4, "%cXX", toupper(*palabra));
    if ( verbose )
      printf("Aplicando regla 8:\n\t%-20s %cXX\n", palabra, toupper(*palabra));
  } else {
    snprintf(result, 4, "%c%cX", toupper(*palabra), toupper(*(palabra+1)));
    if ( verbose )
      printf("Aplicando regla 8:\n\t%-20s %c%cX\n", palabra,
						    toupper(*palabra),
						    toupper(*(palabra+1)));
  }
  return result;
}

/**
 * Regla 9a.
 * Cuando en la denominación o razón social figuren artículos, preposiciones
 * y conjunciones o contracciones no se tomaran como elementos de integración
 * de la clave, ejemplos:
 *
 *	El abastecedor Ferretero, S.A.			AFE-840510
 *	Cigarros la Tabacalera Mexicana, S.A. de C.V.	CTM-860901
 *	Los Viajes Internacionales de Marco Polo, S.A.	VIM-824225
 *	Artículos y Accesorios para Automóviles, S.A.	AAA-800521
 *	Productos de la Industria del Papel, S.A.	PIP-811231
 *
 */

/**
 * Regla 10a.
 * Cuando la denominación o razón social contenga en algún o en sus tres
 * primeros elementos números arábigos, o números romanos, para efectos de
 * conformación de la clave éstos se tomaran como escritos con letra y seguirán
 * las reglas ya establecidas, ejemplos:
 *
 *	El 12, S.A.					DOC-801029
 *		(DOCE)
 *	El 2 de Enero, S de R.L.			DEN-840101
 *		(DOS)
 *	El 505, S.A.					QCI-851215
 *		(QUINIENTOS CINCO)
 *	Editorial Siglo XXI, S.A.			ESV-831114
 *		(VEINTIUNO)
 *
 */
int
moral_regla10(char *numero, char *result[], const int verbose)
{
  int n = 0;
  char *buffer = NULL;
  char *token = NULL;

  /* Check if this 'numero' is really a numnber. Check the firs char only */
  if ( isdigit(*numero) ) {

/*
    buffer = (char *)calloc(MAXNUMERAL, sizeof(char));
    memset(buffer, 0, MAXNUMERAL);
    numeral(buffer, numero, verbose);
*/
    buffer = numeral_alloc(numero, verbose);

    /* Now, copy at most three words onto 'result' */
    for ( (token = strsep(&buffer, " "));
	  token != NULL && n < 3;
	  token = strsep(&buffer, " ")) {
      result[n] = token;
      n++;
    }
    free(buffer);
  }
  return n;
}

/**
 * Regla 11a.
 * Cuando aparezcan formando parte de la denominación o razón social, la
 * palabra Compañía o su abreviatura Cía., así como la palabra Sociedad o su
 * abreviatura Soc., no se incluirán en la conformación de la clave, ejemplos:
 *
 *	Compañía Periodística Nacional, S.A.		PNA-861121
 *	Cía. De Artículos Eléctricos, S. de R.L.	AEL-850110 
 *	Cía. Nal. De Subsistencias Mexicanas, S.A.	NSM-841011
 *	Pimienta Hnos. y Cía., S.A.			PHN-830228
 *	Sociedad Cooperativa de Producción Agrícola de Michoacán      CPA-861016
 *	Sociedad de Consumo Agrícola del Sur, S.C.L.	CAS-821110
 *	Sociedad de Producción Rural de Sonora		PRS-800101
 *
 */

/**
 * Regla 12a.
 * Cuando aparezcan formando parte de la denominación o razón social los
 * caracteres especiales, éstos deben de excluirse para el cálculo del homónimo
 * y del dígito verificador. Los caracteres se interpretarán, sí y sólo si,
 * están en forma individual dentro del texto de la denominación o razón
 * social. (Anexo VI) Ejemplos:
 *
 *	DENOMINACION O RAZON SOCIAL			RFC GENERADO
 *	LA S@NDIA S.A DE C.V.				SND-861121
 *	LA @ S.A. DE C.V.				ARR-860120
 *	LA @ DEL % SA DE CV				APO-830120
 *	@ COMER.COM					ACO-800210
 *	LAS ( BLANCAS )					APB-700202
 *	EL # DEL TEJADO					NET-010202
 *	LA / DEL SUR					DSU-010102
 *	EL C@FE.NET					CFE-030210
 *
 */
char *
moral_regla12(char *palabra, const int verbose)
{
  const size_t len = strlen(palabra);

  if ( len == 1 ) {
    if ( isalnum(*palabra) || strcmp(palabra, "&") == 0 ) {
      /*Este es un caractecter que no necesita ser interpretado por esta regla
       * ya que se trata de un número arábigo o el carácter ampersand
       */
      return palabra;
    } else {
      char *nombre = NULL;
      if ( verbose ) printf("moral_regla12: Aplicando regla 12. Interpretando caracteres individuales...");
      if      ( *palabra == '@'  ) nombre = "ARROBA";
      else if ( *palabra == '\'' ) nombre = "APOSTROFE";
      else if ( *palabra == '%'  ) nombre = "PORCIENTO";
      else if ( *palabra == '#'  ) nombre = "NUMERO";
      else if ( *palabra == '!'  ) nombre = "ADMIRACION";
      else if ( *palabra == '.'  ) nombre = "PUNTO";
      else if ( *palabra == '$'  ) nombre = "PESOS";
      else if ( *palabra == '"'  ) nombre = "COMILLAS";
      else if ( *palabra == '-'  ) nombre = "GUION";
      else if ( *palabra == '/'  ) nombre = "DIAGONAL";
      else if ( *palabra == '+'  ) nombre = "SUMA";
      else if ( *palabra == '('  ) nombre = "ABRE PARENTESIS";
      else if ( *palabra == ')'  ) nombre = "CIERRA PARENTESIS";

      if ( nombre ) {
	if ( verbose ) printf(" caracter %c interpretado como: %s\n", *palabra, nombre);
	palabra = nombre;
      }
    }
  } else {
    /* Elimina de 'p' los caracteres que no sean letras */
    int i = 0;
    char *copy = strndup(palabra, len);
    memset(palabra, 0, len + 1);
    while ( *copy ) {
      /*if ( !isalpha(*copy) ) {*/
      if ( !isalnum(*copy) ) {
	if ( verbose ) printf("moral_regla12: Eliminando de la palabra el caracter '%c'.\n", *copy);
	copy++;
	continue;
      }
      *palabra++ = *copy++;
      i++;
    }
    *palabra = 0;
    palabra -= i;
    copy -= len;
    free(copy);
  }
  return palabra;
}

/**
 * OBSERVACIÓN:
 * En los casos en que se presenten cambios de régimen de capital, seguirá
 * considerándose la misma Clave de R.F.C. asignada inicialmente, ejemplos:
 *
 *		DATOS ACTUALES				CAMBIOS
 *
 *	Instrumentos Bristol, S.A. de C.V.	Instrumentos Bristol, S.A.
 *		IBR-700104				IBR-700104
 *
 *	Lubricantes y Filtros, S.A.	     Lubricantes y Filtros, S.A. de C.V.
 *		LFI-571130				LFI-571130
 *
 */

/**
 * Para el calculo de la clave del R.F.C. de una persona moral se encuentra
 * que hay dos modos utilizados para generar la clave....
 */
char*
clave_rfc_persona_moral(char* clave, const char *denominacion_social, const char *tipo_de_sociedad, const char *year, const char* month, const char *day, const int verbose)
{
  char **palabras = NULL;
  char fecha[7];
  char result[4];
  char clave_diferenciadora[3];
  char tmp_clave[12];
  char digito = 0;
  size_t i = 0;
  size_t len;
  char *nombre = NULL;
  size_t tamano = 0;

  /* Split the name
   * Make an array with all the words in 'denominacion_social'
   * The function split will also apply the rules:
   *
   *	- Regla 3
   *	- Regla 4 -- TODO
   *	- Regla 9
   *	- Regla 10
   *	- Regla 11
   *	- Regla 12
   */
  split(denominacion_social, &palabras, &len, verbose);
  if ( verbose ) {
    printf("%d palabras forman la denominación social:\n", len);
    for (i = 0; i < len; i++) {
      printf("\t%s\n", palabras[i]);
    }
  }

  /* Now, acording to all thw words in the array 'palabras'
   * choose the proper rule to build up the R.F.C. code
   */
  memset(result, 0, 4);
  if ( len >= 3 ) {
    /* Este es el caso estandar, al menos tres palabras para formar
     * la clave
     *
     *	Regla 1
     */
    moral_regla1(result, palabras, verbose);

  } else if ( len == 2 ) {
    /* Para este caso aplica la Regla 6 */
    moral_regla6(result, palabras, verbose);

  } else if ( len == 1 ) {
    /* Aplica la regla 7 */
    moral_regla7(result, palabras[0], verbose);

  }

  /* Free space. */
  for (i = 0; i <= len; i++) {
    free(palabras[i]);
  }
  free(palabras);

  /* Ahora obten la clave diferenciadora de homonimias
   * En algunas ocasiones, para determinar esta clave para una persona
   * moral, se incluye el tipo de sociedad y en otras ocasiones no.
   * Entonces...
   */
  memset(clave_diferenciadora, 0, 3);
  if ( tipo_de_sociedad != NULL ) {
    tamano = strlen(denominacion_social) + strlen(tipo_de_sociedad) + 1;
    nombre = (char *)calloc(tamano, sizeof(char));
    snprintf(nombre, tamano, "%s %s", denominacion_social, tipo_de_sociedad);
    homonimia(clave_diferenciadora, nombre, NULL, verbose);
    free(nombre);

  } else {
    homonimia(clave_diferenciadora, denominacion_social, NULL, verbose);
  }

  /* Ahora aplica la regla 2, para completar la clave */
  memset(fecha, 0, 7);
  moral_regla2(year, month, day, fecha);

  /* Ahora obten el digito verificador */
  memset(tmp_clave, 0, 12);
  snprintf(tmp_clave, 12, "%s%s%s", result, fecha, clave_diferenciadora);
  digito = digito_verificador(tmp_clave, verbose);

  /* Arma la clave */
  memset(clave, 0, 13);
  snprintf(clave, 13, "%s%c", tmp_clave, digito);

  return clave;
}

/**
 * Auxiliar function to remove any '.' or ',' character.
 *
 */
char *
remove_punctuation(char* word, int verbose)
{
  int i = 0;
  const size_t len = strlen(word);
  char *copy = strndup(word, len);

  /* Does this word has only one character */
  if ( len < 2 ) {
    return word;
  }

  /* Reset the word */
  memset(word, 0, len + 1);

  while ( *copy ) {
    if ( *copy == '.' ) {
      /* Replace the '.' by a space, i.e. Regla 3 */
      if ( verbose ) printf("Regla 4. Cambiando un '.' por un ' '\n"); 
      *word++ = ' ';
      copy++;
      continue;

    } else if ( *copy == ',' ) {
      copy++;
      continue;

    }
    *word++ = *copy++;
    i++;
  }
  *word = 0;
  word -= i;
  copy -= len;
  free(copy);

  return word;
}


/**
 * Function to convert a single string onto an its words as an array
 * of strings.
 */
void
split(const char *name, char ***words, size_t *len, const int verbose)
{
  char *p = NULL;
  char *copy = NULL;
  char **list = NULL;
  char **tmp = NULL;
  char *last = NULL;
  char *numeral[4]; /* 3 numerales, maximo */
  int n = 0; /* cantidad de numerales */
  int i = 0;

  /* Initialize the results */
  *words = NULL;
  *len = 0;

  copy = strndup(name, strlen(name));
  if ( copy == NULL ) {
    fprintf(stderr, "split: Not enough memory to copy name");
    return;
  }

  for ( (p = strtok_r(copy, " \t\n", &last));
	p;
	(p = strtok_r(NULL, " \t\n", &last)) ) {

    /* Remove any punctuation sign */
    /* This call may also be used to apply Rule 4 */
    p = remove_punctuation(p, verbose);

    /* Apply rule 9 */
    if ( !strcasecmp(p, "EL") ||
	 !strcasecmp(p, "LA") ||
	 !strcasecmp(p, "DE") ||
	 !strcasecmp(p, "LOS") ||
	 !strcasecmp(p, "LAS") ||
	 !strcasecmp(p, "DEL") ||
	 !strcasecmp(p, "EN" ) ||
	 !strcasecmp(p, "CON" ) ||
	 !strcasecmp(p, "POR" ) ||
	 !strcasecmp(p, "PARA" ) ||
	 !strcasecmp(p, "Y") ) {
      if ( verbose ) {
	printf("split: Aplicando Regla 9. Eliminando la palabra '%s'.\n", p);
      }
      continue;
    }

    /* Apply rule 11 */
    if ( !strcasecmp(p, "Compania") ||
	 !strcasecmp(p, "Cia") ||
	 !strcasecmp(p, "Sociedad") ||
	 !strcasecmp(p, "Soc") ) {
      if ( verbose ) {
	printf("split: Aplicando Regla 11. Eliminando la palabra '%s'.\n", p);
      }
      continue;
    }

    /* Apply rule 3
     * Do not take into account composite characters
     */
    p = moral_regla3(p, verbose);

    /* Apply rule 12
     */
    p = moral_regla12(p, verbose);

    /* Apply rule 10
     * Sustituye numeros arabigos. Para este caso, tanto 'p' como el
     * resultado de aplicar la regla puede resultar en una o mas palabras
     * y cada una de ellas de logitud mayor a la de 'p'
     */
    n = moral_regla10(p, numeral, verbose);
    if ( n > 0 ) {
      /* Agrega el numeral a la lista de palabras para formar la clave */
      for (i = 0; i < n; i++ ) {
        tmp = realloc(list, (sizeof * list) * (*len + 1));
        if ( tmp == NULL ) {
          fprintf(stderr, "Failed to allocate memory for more words.");
          *words = list;
          return;
        }

        list = tmp;

        list[*len] = strndup(numeral[i], strlen(numeral[i]) + 1);
        if ( list[*len] == NULL ) {
          fprintf(stderr, "No fue posible duplicar la cadena.");
          *words = list;
          return;
        }
        (*len)++;
      }

    } else {
 
      tmp = realloc(list, (sizeof * list) * (*len + 1));
      if ( tmp == NULL ) {
        fprintf(stderr, "Failed to allocate memory for more words.");
        *words = list;
        return;
      }

      list = tmp;

      list[*len] = strndup(p, strlen(p) + 1);
      if ( list[*len] == NULL ) {
        fprintf(stderr, "No fue posible duplicar la cadena.");
        *words = list;
        return;
      }
      (*len)++;

    }

  }

  /* Add the sentinel */
  tmp = realloc(list, (sizeof * list) * (*len + 1));
  list = tmp;
  list[*len] = NULL;

  free(copy);
  free(p);

  *words = list;
}
