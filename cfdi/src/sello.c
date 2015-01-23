/*
 * Copyright (c) 2013-2015, L3a,
 *			    Enrique Gamez Flores <egamez@edisson.com.mx>
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
#ifndef INERE_CFDI_VERIFICA_SELLO_DIGITAL_H_
#include "inere/cfdi/verifica_sello_digital.h"
#endif

#include <stdlib.h>
#include <stdio.h>

/**
 * Programa para verificar el sello digital de(los) comprobante(s)
 * digital(es)
 */
int
main(int argc, char *argv[])
{
  int result = 0;
  int debug = 0;
  int i = 0;

  if ( argc < 3 ) {
    fprintf(stderr, "Uso:\n\n\t%s RUTA-DEL-ARCHIVO-DE-LA-CADENAORIGINAL CFDi(s)\n\n", argv[0]);
    return 1;
  }

  for (i = 2; i < argc; i++) {

    /* Verifica el "sello digital" */
    result = verifica_sello_digital(argv[i], argv[1], debug);

    if ( argc > 3 ) printf("%s: ", argv[i]);

    if ( result == 0 )
      printf("Ok.\n");

    else if ( result == 1 ) 
      printf("Sello digital en el documento no válido.\n");

    else
      printf("No fue posible realizar el test.\n");

  }

  return 0;
}
