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
  asigna_LugarExpedicion(factura, (unsigned char *)"Mi casa");
  agrega_metodoDePago(factura, (unsigned char *)"efectivo");

  /* Agrega los datos del emisor */
  agrega_Emisor(factura, (unsigned char *)"GAFA7701266V2", (unsigned char *)"Alma Delia Gámez Flores");
  agrega_Emisor_DomicilioFiscal(factura,
				(unsigned char *)"8 Poniente",
				(unsigned char *)"716",
				(unsigned char *)"A",
				(unsigned char *)"Centro",
				(unsigned char *)"H. Puebla de Zaragoza",
				(unsigned char *)"Puebla",
				(unsigned char *)"Puebla",
				(unsigned char *)"México",
				(unsigned char *)"Entre 7 Norte y 9 Norte",
				(unsigned char *)"72000");

  agrega_Emisor_ExpedidoEn(factura,
				(unsigned char *)"8 Poniente",
				(unsigned char *)"716",
				(unsigned char *)"A",
				(unsigned char *)"Centro",
				(unsigned char *)"H. Puebla de Zaragoza",
				(unsigned char *)"Puebla",
				(unsigned char *)"Puebla",
				(unsigned char *)"México",
				(unsigned char *)"Entre 7 Norte y 9 Norte",
				(unsigned char *)"72000");

  agrega_Emisor_RegimenFiscal(factura, (unsigned char *)"Regimen de las personas fisicas con actividades profesionales");
  agrega_Emisor_RegimenFiscal(factura, (unsigned char *)"Regimen de las personas fisicas con actividades empresariales");
 
  /* Ahora los datos del receptor */
  agrega_Receptor(factura, (unsigned char *)"GAFA7701266V2", (unsigned char *)"Alma Delia Gámez Flores");
  agrega_Receptor_Domicilio(factura,
				(unsigned char *)"8 Poniente",
				(unsigned char *)"716",
				(unsigned char *)"A",
				(unsigned char *)"Centro",
				(unsigned char *)"H. Puebla de Zaragoza",
				(unsigned char *)"Puebla",
				(unsigned char *)"Puebla",
				(unsigned char *)"México",
				(unsigned char *)"Entre 7 Norte y 9 Norte",
				(unsigned char *)"72000");

  agrega_Concepto(factura, cantidad, noIdentificacion, unidad, descripcion, valorUnitario, importe);
  agrega_Concepto(factura, (unsigned char *)"11", noIdentificacion, unidad, descripcion, valorUnitario, importe);
  agrega_Concepto(factura, (unsigned char *)"10", noIdentificacion, unidad, descripcion, valorUnitario, importe);

  agrega_Impuesto_Retencion(factura, (unsigned char *)"IVA", (unsigned char *)"16.00");
  agrega_Impuesto_Retencion(factura, (unsigned char *)"ISR", (unsigned char *)"160.00");

  agrega_Impuesto_Traslado(factura, (unsigned char *)"IVA", (unsigned char *)"16.00", (unsigned char *)"1016.00");
  agrega_Impuesto_Traslado(factura, (unsigned char *)"IEPS", (unsigned char *)"33.00", (unsigned char *)"10160.00");


  data = genera_comprobante_alloc(factura);

  printf("Comprobante:\n\n%s\n", data);
  xmlFree(data);

  termina_comprobante(factura);

  return 0;
}
