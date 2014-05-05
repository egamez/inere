#include "inere/envia/genera_cadena.h"

#include <stdlib.h>
#include <time.h>

/**
 * Función para generar una cadena de caracteres aleatoria.
 * Los caracteres aleatorios a generar son los mismos caracteres
 * que se utilizan para codificar en base64
 *
 * Es IMPORTANTE que antes de llamar a esta función se llam
 *
 * Argumentos:
 *
 *	str	La cadena en la cual se depositaran los caracteres a generar
 *	length	La longitud de la cadena incluyendo el caracter NUL
 *	mode	Flag para seleccionar, de un par de charsets, los caracteres
 *		  a seleccionar.
 *		  Util para generar el identificador del mensaje, y
 *		  Para el delimitador de las partes del mensaje.
 *
 * La función regresa el puntero de la cadena generada.
 */
char *
genera_cadena(char *str, unsigned int length, const int mode)
{
  unsigned int i = 0;
  static const char charset1[64] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  static const char charset2[36] =
    "0123456789abcdefghijklmnopqrstuvwxyz";

  /* Seed number for rand() */
  srand((unsigned int) time(NULL));

  for (i = 0; i < length - 1; ++i) {
    if ( mode == 1 ) str[i] = charset1[rand() % 64];
    else	     str[i] = charset2[rand() % 36];
  }

  str[length - 1] = '\0';
  return str;
}
