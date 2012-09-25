/**
 * obten_digito.c
 *
 * Copyright (c) 2012, Enrique Gamez Flores <egamez@edisson.com.mx>, and
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
#include <string.h>
#include <stdio.h>

#include <inere/verificador.h>

int
main(int argc, char* argv[])
{
  char result = 0;
  size_t len;

  if ( argc < 2 ) {
    fprintf(stderr, "Uso:\n\n\t%s <clave-de-el-RFC>\n",  argv[0]);
    return 1;
  }

  result = verifica_rfc(argv[1], 0);
  if ( result == 0 ) {
    printf("Digito de verificacion (\"%c\") de la clave de el RFC correcta.\n", result);
  } else {
    len = strlen(argv[1]);
    printf("Clave de el RFC incorrecta, el digito verificador para la clave suministrada es \"%c\", mientras que el digito suministrado es \"%c\".\n", result, argv[1][len-1]);
  }

  return 0;
}
