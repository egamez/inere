/**
 * Small program to test the certificado.c functions
 */
#ifndef EDISSON_CERTIFICADO_H_INCLUDED
#include "inere/cfdi/certificado.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int
main(int argc, char* argv[])
{
  char *cert = NULL;
  unsigned char *serial = NULL;

  if ( argc < 2 ) {
    fprintf(stderr, "Error. Sintax:\n\t%s CERTIFICADO-DE-LLAVE-PUBLICA\n", argv[0]);
    return 1;
  }

  cert = load_certificate_alloc(argv[1], &serial, 1);

  if ( cert != NULL ) {
    printf("Certificado de llave publica:\n%s\n", cert);
    free(cert);
  } else {
    fprintf(stderr, "No fue posible leer el certificado de llave publica.\n");
  }

  if ( serial != NULL ) {
    printf("Numero de serie del certificado de llave publica: %s.\n", serial);
    free(serial);
  } else {
    fprintf(stderr, "No fue posible leer el numero de serie del certificado de llave publica.\n");
  }

  return 0;
}
