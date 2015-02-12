#ifndef INERE_CFDI_R12NIMPRESA_H_
#include "inere/cfdi/r12nimpresa.h"
#endif

#include <stdio.h>

/**
 * http://stackoverflow.com/questions/5309471/getting-file-extension-in-c
 */

int
main(int argc, char *argv[])
{
  int res = 0;
  int verbose = 1;

  if ( argc == 2 ) {
    /* En este caso, solo fue dado el CFDI y el archivo para la
     * representacion impresa sera tomado del nombre del archivo que contiene
     * el CFDI, pero ahora con extension .pdf
     */
  }

  if ( argc < 3 ) {
    fprintf(stderr, "uso: %s CFDI PDF-FILE\n", argv[0]);
    return 1;
  }

  res = r12nimpresa(argv[1], argv[2], NULL, NULL, verbose);

  return res;
}
