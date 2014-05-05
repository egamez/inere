/*
 * Copyright (c) 2014, L3a,
 *                     Enrique Gámez Flores <egamez@edisson.com.mx>
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
