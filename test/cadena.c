/**
 * Ejemplo para mostrar la generación de la cadena original de un CFDi
 */
#ifndef EDISSON_GENERA_CADENA_ORIGINAL_H
#include "inere/cfdi/genera_cadena_original.h"
#endif
#include <stdio.h>

int
main(int argc, char *argv[])
{
  int i = 0;
  int res = 0;
  int verbose = 0;
  xmlDocPtr cfdi = NULL;
  xmlChar *cadena = NULL;

  if ( argc < 3 ) {
    fprintf(stderr, "Uso:\n\n%s [STYLESHEET] [CFDis]\n", argv[0]);
    return 1;
  }

  /* Comienza el loop sobre todos los CFDis suministrados */
  for (i = 2; i < argc; i++) {
    if ( argc > 3 ) {
      printf("%s: ", argv[i]);
    }
    cfdi = xmlReadFile(argv[i], "UTF-8", XML_PARSE_NOENT);
    if ( cfdi == NULL ) {
      printf("El CFDi (%s) no pudo ser leido.\n", argv[i]);
      continue;
    }

    res = cadena_original(argv[1], cfdi, &cadena, verbose);
    if ( res == 0 ) {
      /* Si fue posible extraer la cadena, imprime el resultado */
      printf("%s\n", cadena);
      xmlFree(cadena);

    } else if ( res == 1 ) {
      printf("La stylesheet (%s) no pudo ser compilada.\n", argv[1]);
    } else if ( res == 2 ) {
      printf("Las transformaciones definidas en la stylesheet (%s) no pudieron aplicarse al CFDi (%s)\n", argv[1], argv[i]);
    } else if ( res == 3 ) {
      printf("No fue posible escribir la cadena original al buffer temporal.\n");
    }
    xmlFree(cfdi);
  }

  return 0;
}
