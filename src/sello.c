/*
 * Copyright (c) 2013, Enrique Gamez Flores <egamez@edisson.com.mx>,
 *                     Lae
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

#include <stdlib.h>
#include <stdio.h>

#include "inere/verifica_sello_digital.h"

int
main(int argc, char *argv[])
{
  int result = 0;
  int debug = 0;

  if ( argc < 3 ) {
    fprintf(stderr, "Sintax:\n\n\t%s <CFD instance> <cadenaorigina-stylesheet-path> [<debug-flag>]\n\n", argv[0]);
    return 1;
  }

  if ( argc == 4 ) debug = 1;

  /* Verify the "sello digital" */
  result = verifica_sello_digital(argv[1], (const unsigned char *)argv[2], debug);

  if ( result == 1 ) {
    printf("Documento con sello digital válido.\n");
  } else {
    fprintf(stderr, "El sello digital del documento no coincide.\n");
    return 2;
  }


  return 0;
}
