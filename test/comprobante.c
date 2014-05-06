#ifndef INERE_CREA_CFDI_H
#include "inere/cfdi/crea_cfdi.h"
#endif

int
main(void)
{
  int res = 0;
  int verbose = 1;
  xmlDocPtr cfdi = NULL;

  cfdi = crea_cfdi(verbose);
  if ( cfdi == NULL ) {
    fprintf(stderr, "No fue posible crear el documento.\n");
    return 1;
  }

  /* Salva el documento */
  res = xmlSaveFormatFileEnc("moles.xml", cfdi, "UTF-8", 1);
  printf("xmlSaveFormatFileEnc = %d\n", res);

  /* Libera la memoria consumida */
  xmlFreeDoc(cfdi);

  return 0;
}
