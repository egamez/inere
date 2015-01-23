/**
 * obten_digito.c
 *
 * Copyright (c) 2012-2015, L3a,
 *                          Enrique Gamez Flores <egamez@edisson.com.mx>
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
 *
 * gcc -I<path>/include -o obten_digito obten_digito.c -L<path>/lib -linere
 *
 * ./obten_digito CAHA211218UL
 *
 * El resultado debe ser el digito "1"
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <inere/verificador.h>
#include <inere/util.h>

int
main(int argc, char* argv[])
{
  char digito = 0;
  char* clave = 0;

  if ( argc < 2 ) {
    fprintf(stderr, "Uso:\n\t%s <RFC-sin-digito-verificador>\n", argv[0]);
    return 1;
  }

  clave = to_upper_case_and_convert((unsigned char*)argv[1]);
  digito = digito_verificador(clave, 0);
  free(clave);

  printf("Digito verificador: %c\n", digito);

  return 0;
}
