/*
 * Copyright (c) 2014-2015, L3a,
 *                     	    Enrique Gamez Flores <egamez@edisson.com.mx>
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
 * El algoritmo básico consiste en representar el numero en familias, y una
 * vez que se halla hecho esta representación se podrá crear el numeral.
 */
#ifndef INERE_NUMERAL_ALLOC_H_
#include "inere/numeral_alloc.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int agrega_las_unidades(const unsigned int numero, char *buffer);
int agrega_las_decenas(const unsigned int numero, char *buffer);
int agrega_las_centenas(const unsigned int numero, char *buffer);
int agrega_diez_o_veinte(const unsigned int numero, char *buffer);
int agrega_el_cien(char *buffer);
int agrega_el_mil(char *buffer);
int agrega_periodo(const unsigned int numero, char *buffer);
char *copia_y_normaliza_alloc(const char *numero, const int verbose);
int numeral_de_clase(const char numero[], char *buffer, const size_t len, const int verbose);

/**
 *
 */
int
agrega_las_unidades(const unsigned int numero, char *buffer)
{
  int len = 0;
  if ( numero == 0 ) {
    len = 4;
    strncat(buffer, "cero", len);
  } else if ( numero == 1 ) {
    len = 2;
    strncat(buffer, "un", len);
  } else if ( numero == 2 ) {
    len = 3;
    strncat(buffer, "dos", len);
  } else if ( numero == 3 ) {
    len = 4;
    strncat(buffer, "tres", len);
  } else if ( numero == 4 ) {
    len = 6;
    strncat(buffer, "cuatro", len);
  } else if ( numero == 5 ) {
    len = 5;
    strncat(buffer, "cinco", len);
  } else if ( numero == 6 ) {
    len = 4;
    strncat(buffer, "seis", len);
  } else if ( numero == 7 ) {
    len = 5;
    strncat(buffer, "siete", len);
  } else if ( numero == 8 ) {
    len = 4;
    strncat(buffer, "ocho", len);
  } else if ( numero == 9 ) {
    len = 5;
    strncat(buffer, "nueve", len);
  }
  return len;
}

/**
 *
 */
int
agrega_las_decenas(const unsigned int numero, char *buffer)
{
  int len = 0;
  if ( numero == 3 ) {
    len = 7;
    strncat(buffer, "treinta", len);
  } else if ( numero == 4 ) {
    len = 8;
    strncat(buffer, "cuarenta", len);
  } else if ( numero == 5 ) {
    len = 9;
    strncat(buffer, "cincuenta", len);
  } else if ( numero == 6 ) {
    len = 7;
    strncat(buffer, "sesenta", len);
  } else if ( numero == 7 ) {
    len = 7;
    strncat(buffer, "setenta", len);
  } else if ( numero == 8 ) {
    len = 7;
    strncat(buffer, "ochenta", len);
  } else if ( numero == 9 ) {
    len = 7;
    strncat(buffer, "noventa", len);
  }
  return len;
}


/**
 *
 */
int
agrega_las_centenas(const unsigned int numero, char *buffer)
{
  int len = 0;
  if      ( numero == 1 ) {
    len = 6;
    strncat(buffer, "ciento", 6);
  } else if ( numero == 2 ) {
    len = 10;
    strncat(buffer, "doscientos", len);
  } else if ( numero == 3 ) {
    len = 11;
    strncat(buffer, "trescientos", len);
  } else if ( numero == 4 ) {
    len = 13;
    strncat(buffer, "cuatrocientos", len);
  } else if ( numero == 5 ) {
    len = 10;
    strncat(buffer, "quinientos", len);
  } else if ( numero == 6 ) {
    len = 11;
    strncat(buffer, "seiscientos", len);
  } else if ( numero == 7 ) {
    len = 11;
    strncat(buffer, "setecientos", len);
  } else if ( numero == 8 ) {
    len = 11;
    strncat(buffer, "ochocientos", len);
  } else if ( numero == 9 ) {
    len = 11;
    strncat(buffer, "novecientos", len);
  }
  return len;
}


/**
 *
 */
int
agrega_diez_o_veinte(const unsigned int numero, char *buffer)
{
  int len = 0;
  if ( numero == 10 ) {
    len = 4;
    strncat(buffer, "diez", len);
  } else if ( numero == 11 ) {
    len = 4;
    strncat(buffer, "once", len);
  } else if ( numero == 12 ) {
    len = 4;
    strncat(buffer, "doce", len);
  } else if ( numero == 13 ) {
    len = 5;
    strncat(buffer, "trece", len);
  } else if ( numero == 14 ) {
    len = 7;
    strncat(buffer, "catorce", len);
  } else if ( numero == 15 ) {
    len = 6;
    strncat(buffer, "quince", len);
  } else if ( numero == 16 ) {
    len = 9;
    strncat(buffer, "dieciseis", len);
  } else if ( numero == 17 ) {
    len = 10;
    strncat(buffer, "diecisiete", len);
  } else if ( numero == 18 ) {
    len = 9;
    strncat(buffer, "dieciocho", len);
  } else if ( numero == 19 ) {
    len = 10;
    strncat(buffer, "diecinueve", len);
  } else if ( numero == 20 ) {
    len = 6;
    strncat(buffer, "veinte", len);
  } else if ( numero == 21 ) {
    len = 8;
    strncat(buffer, "veintiun", len);
  } else if ( numero == 22 ) {
    len = 9;
    strncat(buffer, "veintidos", len);
  } else if ( numero == 23 ) {
    len = 10;
    strncat(buffer, "veintitres", len);
  } else if ( numero == 24 ) {
    len = 12;
    strncat(buffer, "veinticuatro", len);
  } else if ( numero == 25 ) {
    len = 11;
    strncat(buffer, "veinticinco", len);
  } else if ( numero == 26 ) {
    len = 10;
    strncat(buffer, "veintiseis", len);
  } else if ( numero == 27 ) {
    len = 11;
    strncat(buffer, "veintisiete", len);
  } else if ( numero == 28 ) {
    len = 10;
    strncat(buffer, "veintiocho", len);
  } else if ( numero == 29 ) {
    len = 11;
    strncat(buffer, "veintinueve", len);
  }
  return len;
}


/**
 *
 */
int
agrega_el_cien(char *buffer)
{
  int len = 4;
  strncat(buffer, "cien", len);
  return len;
}

/**
 *
 */
int
agrega_el_mil(char *buffer)
{
  int len = 3;
  strncat(buffer, "mil", len);
  return len;
}

/**
 *
 */
int
agrega_periodo(const unsigned int numero, char *buffer)
{
  int len = 0;
  if ( numero == 1 ) {
    len = 6;
    strncat(buffer, "millon", len);
  } else if ( numero == 2 ) {
    len = 6;
    strncat(buffer, "billon", len);
  } else if ( numero == 3 ) {
    len = 7;
    strncat(buffer, "trillon", len);
  } else if ( numero == 4 ) {
    len = 10;
    strncat(buffer, "cuatrillon", len);
  } else if ( numero == 5 ) {
    len = 10;
    strncat(buffer, "quintillon", len);
  } else if ( numero == 6 ) {
    len = 9;
    strncat(buffer, "sextillon", len);
  } else if ( numero == 7 ) {
    len = 9;
    strncat(buffer, "septillon", len);
  } else if ( numero == 8 ) {
    len = 8;
    strncat(buffer, "octillon", len);
  } else if ( numero == 9 ) {
    len = 8;
    strncat(buffer, "nonillon", len);
  } else if ( numero == 10 ) {
    len = 8;
    strncat(buffer, "decillon", len);
  } else if ( numero == 11 ) {
    len = 10;
    strncat(buffer, "undecillon", len);
  } else if ( numero == 12 ) {
    len = 11;
    strncat(buffer, "duodecillon", len);
  }

  return len;
}

/**
 *
 */
char *
copia_y_normaliza_alloc(const char *numero, const int verbose)
{
  int i = 0;
  const size_t len = strlen(numero);
  char *copy = NULL;

  copy = (char *)calloc(len + 1, sizeof(char));
  if ( copy == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No hubo suficiente memoria para crear la copia local del número.\n", __FILE__, __LINE__);
    }
    return copy;
  }

  while ( *numero != '\0' ) {

    /* Ahora, cabe la posibilidad de que este numero sea cero, y se
     * encuentre al inicio del numero */
    if ( *numero == '0' && i == 0 ) {
      /* Este es el caso que debemos omitir */
      if ( verbose ) {
	printf("%s:%d Info. Se encontró el caracter '%c' al comienzo de la cantidad.\n", __FILE__, __LINE__, *numero);
      }
      numero++;
      continue;
    }

    if ( *numero == '.' ) {
      /* No es necesario seguir copiando este caracter */
      if ( verbose ) {
	printf("%s:%d Info. Se encontró el caracter '%c', esto separa el numeral de sus posibles fracciones.\n", __FILE__, __LINE__, *numero);
      }
      break;

    } else if ( !isdigit(*numero) ) {
      /* No tomes en cuenta este caracter */
      if ( verbose ) {
	printf("%s:%d Info. Suprimiendo el caracter '%c'.\n", __FILE__, __LINE__, *numero);
      }

      numero++;
      continue;
    }

    *copy++ = *numero++;
    i++;
  }

  copy -= i;

  return copy;
}


/**
 *
 */
int
numeral_de_clase(const char numero[], char *buffer, const size_t longitud, const int verbose)
{
  int unidad  = -1;
  int decena  = -1;
  int centena = -1;
  int len = 0;
  const size_t digitos = strlen(numero);

  if ( digitos == 0 ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El número para el cual debería de obtenerse el numeral no fue suministrado.\n", __FILE__, __LINE__);
    }
    return -1;
  } else if ( digitos > 3 ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El número para el cual debería de obtenerse el numeral excede por %d digitos al máximo permitido para una clase.\n", __FILE__, __LINE__, digitos - 3);
    }
    return -2;
  }

  /* Deberá al menos haber un digito */
  unidad = numero[digitos - 1] - 48;
  if ( digitos == 3 ) {
    centena = numero[0] - 48;
    decena  = numero[1] - 48;
  } else if ( digitos == 2 ) {
    decena = numero[0] - 48;
  }

  /* Ahora arma este numeral */
  memset(buffer, 0, longitud);
  if ( centena > 0 ) {
    /* Existe un caso especial, el caso para el numeral de 100 */
    if ( centena == 1 && decena == 0 && unidad == 0 ) {
      len = agrega_el_cien(buffer);
    }  else {
      len = agrega_las_centenas(centena, buffer);
    }
  }

  /* Agrega ahora las decenas */
  if ( decena > 0 ) {
    /* Agrega un espacio al numero solo en caso de que halla habido centenas */
    if ( centena > 0 ) {
      strncat(buffer, " ", 1);
      len += 1;
    }
    if ( decena == 1 || decena == 2 ) {
      len += agrega_diez_o_veinte(decena * 10 + unidad, buffer);
      unidad = -1;
    } else {
      len += agrega_las_decenas(decena, buffer);
    }

  }

  /* Y finalmente las unidades */
  if ( unidad > 0 ) {

    /* Para el caso en el que ya se hallan agregado algunos otros
     * numerales a 'buffer' entonces sera necesario agregar un espacio
     * antes de escribir el numeral correspondiente a esta unidad */
    if ( len > 0 ) {
      strncat(buffer, " ", 1);
      len += 1;
    }

    /* Ahora, en caso de que halla decenas que agregar, decenas mayores
     * a 3 sera necesario agregar la conjuncion 'y' */
    if ( decena >= 3 ) {
      strncat(buffer, "y ", 2);
      len += 2;
    }
    /* Y ahora solo agrega este numeral correspondiente a la unidad */
    len += agrega_las_unidades(unidad, buffer);
  }

  if ( verbose ) {
    printf("%s:%d Info. Número [%s] = [%s], strlen = %d, our len = %d\n", __FILE__, __LINE__, numero, buffer, strlen(buffer), len);
  }

  return len;
}

/**
 *
 */
char *
numeral_alloc(const char *numero, const int verbose)
{
  div_t division;
  char *copy = NULL;
  size_t len = 0;
  char *result = NULL;
  int i = 0;
  int j = 0;
  int periodo = 0;
  int con_periodo = 0;
  int plural = 0;
  int familia = 0;
  int n = 0;
  char numeral_clase[40];
  char numeral_familia[16];
  int nclases = 0;
  char clase[4];
  size_t len_numeral_clase = 0;
  size_t len_numeral_familia = 0;

  copy = copia_y_normaliza_alloc(numero, verbose);
  if ( copy == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No es posible crear el numeral que representaría el numero dado.\n", __FILE__, __LINE__);
    }
    return NULL;
  }
  len = strlen(copy);

  /* ahora obten el numero de familias que se requieren para crear el numeral */
  division = div(len, 3);
  nclases = division.quot + (division.rem ? 1 : division.rem);

  if ( verbose ) {
    printf("Se requerirá de crear %d familias, y la última familia tendra %d digitos.\n", nclases, division.rem);
  }

  n = nclases;
  familia = n;
  len = 0;

  /* Ahora, para formar el numeral solo requerimos de ir concatenando las
   * los numerales correspondientes a cada una de las clases */
  for (i = 0; i < nclases && *copy != '\0'; i++) {

    len_numeral_clase = 0;
    len_numeral_familia = 0;
    memset(clase, 0, 4);

    if ( division.rem ) {
      /* Este es el caso de una clase incompleta */
      for (j = 0; j < division.rem; j++) {
	clase[j] = *copy++;
      }
      division.rem = 0;

    } else {
      /* Arma una clase con 3 caracteres */
      clase[0] = *copy++;
      clase[1] = *copy++;
      clase[2] = *copy++;
      j += 3;

    }
    len_numeral_clase = numeral_de_clase(clase, numeral_clase, 40, verbose);

    /* Informa que hay numerales en este periodo */
    if ( len_numeral_clase ) {
      con_periodo = 1;
      if ( strcmp(numeral_clase, "un") ) {
	plural = 1;
      }
    }

    /* Ahora debemos generar el sufijo de familia.
     */
    memset(numeral_familia, 0, 16);
    if ( *copy != '\0' ) {
      if ( familia % 2 ) {
	periodo = familia - ((familia+1)/2);
	if ( periodo && con_periodo ) {
	  len_numeral_familia = agrega_periodo(periodo, numeral_familia);
	  con_periodo = 0;
	  /*
	   * Ahora agrega el plural, en caso de que 'len_numeral_clase'
	   * sea mayor a dos, o si ya hay algun ...
	   */
	  if ( plural ) {
	    strncat(numeral_familia, "es", 2);
	    len_numeral_familia += 2;
	    plural = 0;
	  }
	}

      } else {
	if ( len_numeral_clase ) {
	  len_numeral_familia = agrega_el_mil(numeral_familia);
	  plural = 1;
	}
      }
    }
    familia--;

    /* Ahora construye el numeral final */
    if ( len ) {
      /* El la memoría para el resultado ya ha sido reservada,
       * y será necesario agregar un espacio
       */
      if ( len_numeral_clase ) {
	if ( verbose ) {
	  printf("Agregando el numeral: \"%s\"\n", numeral_clase);
	}
	result = (char *)realloc(result, sizeof(char) * (len + len_numeral_clase + 1));
	strncat(result, " ", 1);
	strncat(result, numeral_clase, len_numeral_clase);
	len += len_numeral_clase + 1;
      }

    } else {
      if ( verbose ) {
	printf("Escribiendo el numeral: \"%s\"\n", numeral_clase);
      }
      result = (char *)calloc(len_numeral_clase + 1, sizeof(char));
      strncpy(result, numeral_clase, len_numeral_clase);
      len = len_numeral_clase + 1;
    }

    if ( len_numeral_familia ) {
      if ( verbose ) {
	printf("Agregando el numeral: \"%s\"\n", numeral_familia);
      }
      result = (char *)realloc(result, sizeof(char) * (len + len_numeral_familia + 1));
      strncat(result, " ", 1);
      strncat(result, numeral_familia, len_numeral_familia);
      len += len_numeral_familia + 1;
    }

  }

  copy -= j;

  free(copy);

  return result;
}
