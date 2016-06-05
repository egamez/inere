/**
 * Programa para verificar la firma del CFDI
 */

#ifndef INERE_CFDI_VERIFICA_SELLO_DIGITAL_H_
#include "inere/cfdi/verifica_sello_digital.h"
#endif

#include <stdio.h>

int
main(int argc, char* argv[])
{
  int res = 0;
  int verbose = 0;

  if ( argc < 3 ) {
    fprintf(stderr, "Uso: %s STYLESHEET CFDIn", argv[0]);
    return 1;
  }

  res = verifica_sello_digital(argv[2], argv[1], verbose);

  if ( res ) {
    /* Un error ocurrio */
    fprintf(stderr, "El sello digital del CFDI no coincide.\n");
  } else {
    printf("Ok.\n");
  }

  return 0;
}
