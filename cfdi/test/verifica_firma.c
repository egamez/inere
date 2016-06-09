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
  int i = 0;

  if ( argc < 3 ) {
    fprintf(stderr, "Uso: %s STYLESHEET CFDI [ CFDIs ]\n", argv[0]);
    return 1;
  }

  for (i = 2; i < argc; i++) {
    res = verifica_sello_digital(argv[i], argv[1], verbose);

    if ( res ) {
      /* Un error ocurrio */
      if ( argc == 3 ) {
	fprintf(stderr, "El sello digital del CFDI no coincide.\n");
      } else {
	fprintf(stderr, "%s: El sello digital del CFDI no coincide.\n",argv[i]);
      }

    } else {
      if ( argc == 3 ) {
	printf("Ok.\n");
      } else {
	printf("%s: Ok.\n", argv[i]);
      }
    }
  }

  return 0;
}
