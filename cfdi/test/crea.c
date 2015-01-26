#ifndef INERE_CFDI_CREA_CFDI_H_
#include "inere/cfdi/crea_cfdi.h"
#endif

int
main(void)
{
  unsigned char *cantidad         = (unsigned char *)"12";
  unsigned char *noIdentificacion = (unsigned char *)"Q23";
  unsigned char *unidad           = (unsigned char *)"PIEZA";
  unsigned char *descripcion      = (unsigned char *)"MOles señor";
  unsigned char *valorUnitario    = (unsigned char *)"13";
  unsigned char *importe          = (unsigned char *)"130";
  Comprobante_t *factura = NULL;
  xmlChar *data = NULL;

  factura = crea_comprobante((unsigned char *)"3.2");

  asigna_fecha(factura, (unsigned char *)"2015-01-25T11:00:00");
  asigna_sello(factura, (unsigned char *)"sello");
  asigna_formaDePago(factura, (unsigned char *)"pago en una solo exhibicion");
  asigna_noCertificado(factura, (unsigned char *)"12345678901234567890");
  asigna_certificado(factura, (unsigned char *)"Este es el certificado");
  asigna_subTotal(factura, (unsigned char *)"100.00");
  asigna_total(factura, (unsigned char *)"116.00");
  asigna_tipoDeComprobante(factura, (unsigned char *)"ingreso");
  asigna_metodoDePago(factura, (unsigned char *)"efectivo");
  asigna_LugarExpedicion(factura, (unsigned char *)"Mi casa");

  agrega_concepto(factura, cantidad, noIdentificacion, unidad, descripcion, valorUnitario, importe);

  data = genera_comprobante_alloc(factura);

  printf("Comprobante:\n\n%s\n", data);
  xmlFree(data);

  return 0;
}
