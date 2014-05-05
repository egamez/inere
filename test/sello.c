/**
 * Simple test program to make a test of sello.c proram
 */
#ifndef EDISSON_SELLO_H_INCLUDED
#include "inere/cfdi/sello.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int
main(int argc, char* argv[])
{
  char* seal = NULL;

  if ( argc < 3 ) {
    fprintf(stderr, "Error. Sintax\n\t%s KEYCERT CADENA\n", argv[0]);
    return 1;
  }

  seal = sello_alloc(argv[1], "sha1", (unsigned char*)argv[2], 1);

  if ( seal == NULL ) {
    fprintf(stderr, "No fue posible crear el sello digital de la cadena %s.\n", argv[2]);
  } else {
    printf("Sello digital de %s:\n\n%s\n", argv[2], seal);
    free(seal);
  }

  return 0;
}
