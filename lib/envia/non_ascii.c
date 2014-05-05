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
#include "inere/envia/non_ascii.h"
#include "inere/base64.h"

#include <stdlib.h>
#include <string.h>

/**
 * Esta función se encargará de aplicar el mecanismo de para convertir
 * cadenas de caracteres en localizaciones diferentes a ascii...
 * RFC2047
 */
unsigned int
non_ascii_alloc(const char *source, char **encoded)
{
  const unsigned int len1 = strlen(source);
  unsigned int len2 = mbstowcs(NULL, source, 0);
  unsigned int len3 = 0;
  char *tmp = NULL;

  /* El caso más simple y común es cuando 'source' no contiene caracteres
   * diferentes a los ASCII
   */
  if ( len1 == len2 ) {
    /* Este es el caso en donde no hay caracteres a convertir, todos son ASCII*/
    return 0;
  }

  /* Ahora, para hacer las cosas más simples, solo convierte 'source'
   * en una cadena BASE64, y regresalo como una 'encoded-word'
   * junto con los datos de la codificacion deberemos agregar
   * '=?utf-8?B?' y al final '?='
   */
  len2 = BASE64_LENGTH(len1) + 10 + 2 + 1;
  *encoded = (char *)calloc(len2, sizeof(char));
  len3 = base64_encode_alloc(source, len1, &tmp);
  strncat(*encoded, "=?utf-8?B?", 10);
  strncat(*encoded, tmp, len3);
  strncat(*encoded, "?=", 2);
  /*snprintf(*encoded, len2, "=?utf-8?B?%s?=", tmp);*/
  free(tmp);

  len2 = strlen(*encoded);
  return len2;
}
