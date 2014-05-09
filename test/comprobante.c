#ifndef INERE_CFDI_CREA_CFDI_H_
#include "inere/cfdi/crea_cfdi.h"
#endif

int
main(void)
{
  int res = 0;
  int verbose = 1;
  xmlDocPtr cfdi = NULL;
  cfdi_items_list_t *productos = NULL;
  cfdi_items_list_t *traslado = NULL;
  cfdi_items_list_t *retencion = NULL;
  const char *archivo_certificado = "certificado.cer";
  const char *archivo_llave_privada = "key.key";
  const char *archivo_del_emisor = "emisor.xml";

  /* Agrega unos productos */
  productos = append_concepto(productos, (const xmlChar *)"11",
			(const xmlChar *)"pieza",
			(const xmlChar *)"Q23",
			(const xmlChar *)"Contacto 2P+T",
			(const xmlChar *)"32.50",
			(const xmlChar *)"357.50",
			verbose);

  /* Agrega algunos impuestos retenidos */
  retencion = append_impuesto(retencion, (const xmlChar *)"Retencion",
				(const xmlChar *)"IVA",
				NULL,
				(const xmlChar *)"16.16",
				verbose);

  /* Agrega algunos impuestos trasladados */
  traslado = append_impuesto(traslado, (const xmlChar *)"Traslado",
				(const xmlChar *)"IVA",
				(const xmlChar *)"16.00",
				(const xmlChar *)"16.16",
				verbose);

  /* Agrega algunos impuestos retenidos */
  traslado = append_impuesto(traslado, (const xmlChar *)"Traslado",
				(const xmlChar *)"IEPS",
				(const xmlChar *)"3.00",
				(const xmlChar *)"3.16",
				verbose);


  cfdi = crea_cfdi((const xmlChar *)"100.00", NULL, (const xmlChar *)"116.00",
		   productos, retencion, traslado,
			archivo_del_emisor,
			archivo_certificado,
			archivo_llave_privada,
			verbose);
  if ( cfdi == NULL ) {
    fprintf(stderr, "No fue posible crear el documento.\n");
    return 1;
  }

  /* Salva el documento */
  res = xmlSaveFormatFileEnc("moles.xml", cfdi, "UTF-8", 1);
  printf("xmlSaveFormatFileEnc = %d\n", res);

  /* Libera la memoria consumida */
  xmlFreeDoc(cfdi);

  free_cfdi_items_list(productos);
  return 0;
}
