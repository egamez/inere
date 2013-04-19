/*
 * Copyright (c) 2013,  Lae
 *                      Enrique Gámez Flores <egamez@edisson.com.mx>,
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
#include "inere/personamoral.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
  char **palabras = NULL;
  size_t len = 0;
  size_t i = 0;

  if ( argc < 2 ) {
    fprintf(stderr, "Sintax:\n\n\t%s <nombre-de-la-persona-moral>\n\n", argv[0]);
    return 1;
  }

  split(argv[1], &palabras, &len, 1);
  for (i = 0; i < len; i++ ) {
    printf("[%s]\n", palabras[i]);
  }

  for (i = 0; i < len; i++) {
    free(palabras[i]);
  }
  free(palabras);

  return 0;
}
