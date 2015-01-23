/**
 * obten_rfc.c
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
 * gcc -I<path>/include -o obten_rfc obten_rfc.c -L<path>/lib -linere
 *
 * ./obten_rfc 18 12 1921 "Jose Antonio" Camargo Hernandez
 *
 * El resultado debe ser "CAHA211218UL1"
 *
 */
#include <string.h>
#include <stdio.h>

#include <inere/personafisica.h>

int
main(int argc, char* argv[])
{
  char rfc[18];
  char* nombre = NULL;
  char* primer_apellido = NULL;
  char* segundo_apellido = NULL;
  char* year = NULL;
  char* month = NULL;
  char* day = NULL;

  if ( argc < 6 ) {
    fprintf(stderr, "Sintax:\n%s <dia> <mes> <año> <nombre> <primer-apellido> [<segundo-apellido>]\n", argv[0]);
    return 1;
  }
  day = argv[1];
  month = argv[2];
  year = argv[3];
  nombre = argv[4];
  primer_apellido = argv[5];
  if ( argc == 7 ) segundo_apellido = argv[6];

  memset(rfc, 0, 18);

  clave_rfc_persona_fisica(rfc, nombre, primer_apellido, segundo_apellido, year, month, day, 1);

  printf("La clave del RFC es: %s\n", rfc);
  return 0;
}
