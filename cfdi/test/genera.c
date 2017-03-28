#ifndef INERE_CFDI_CREA_CFDI_H_
#include "inere/cfdi/crea_cfdi.h"
#endif
#ifndef INERE_CFDI_LEE_CERTIFICADO_H_
#include "inere/cfdi/lee_certificado.h"
#endif
#ifndef INERE_CFDI_SELLO_H_
#include "inere/cfdi/sello.h"
#endif
#ifndef INERE_CFDI_GENERA_CADENA_ORIGINAL_H_
#include "inere/cfdi/genera_cadena_original.h"
#endif

#include <getopt.h>
#include <stdio.h>

void usage(void);

void
usage(void)
{
}


int
main(int argc, char *argv[])
{
  int ch = -1;
  const char *cliente = NULL;
  const char *private_key = NULL;
  const char *csd = NULL;
  const char *archivo = NULL;
  unsigned char *forma_de_pago = NULL;
  unsigned char *metodo_de_pago = NULL;
  unsigned char *serie = NULL;
  unsigned char *folio = NULL;
  char *stylesheet = NULL;
  int want_ndc = 0;
  int want_verbose = 0;

  unsigned char *fecha = NULL;
  unsigned char *cert = NULL;
  unsigned char *serial = NULL;
  unsigned char *sello = NULL;
  Comprobante_t *factura = NULL;
  xmlChar *cadena = NULL;
  xmlChar *data = NULL;
  int res = 0;
  FILE *out;

  /* options descriptor */
  static struct option longopts[] = {
	{"cliente",		required_argument,	NULL,	'c'},
	{"private-key",		required_argument,	NULL,	'p'},
	{"certificado",		required_argument,	NULL,	'C'},
	{"ndc",			no_argument,		NULL,	'N'},
	{"forma-de-pago",	required_argument,	NULL,	'F'},
	{"metodo-de-pago",	required_argument,	NULL,	'M'},
	{"serie",		required_argument,	NULL,	'S'},
	{"folio",		required_argument,	NULL,	'f'},
	{"stylesheet",		required_argument,	NULL,	's'},
	{"verbose",		no_argument,		NULL,	'v'},
	{"help",		no_argument,		NULL,	'h'},
	{NULL,			0,			NULL,	 0 }
  };

  while ( (ch=getopt_long(argc,argv,"c:p:C:a:NF:M:S:f:s:vh", longopts, NULL)) != -1 ) {
    switch(ch) {

      case 'c':
	/* Asigna la clave del cliente */
	cliente = optarg;
	break;

      case 'p':
	/* Asigna la ruta del archivo que contiene la llave privada */
	private_key = optarg;
	break;

      case 'C':
	/* Asigna la ruta del archivo que contiene el certificado CSD */
	csd = optarg;
	break;

      case 'N':
	/* Se pidio generar una nota de credito */
	want_ndc = 1;
	break;

      case 'F':
	/* Asigna la forma de pago */
	forma_de_pago = (unsigned char *)optarg;
	break;

      case 'M':
	/* Asigna el metodo de pago */
	metodo_de_pago = (unsigned char *)optarg;
	break;

      case 'S':
	/* Asigna la serie del CFDI */
	serie = (unsigned char *)optarg;
	break;

      case 'f':
	/* Asigna el folio del CFDI */
	folio = (unsigned char *)optarg;
	break;

      case 's':
	/* Asigna la ruta de la stylesheet para obtener la cadena original */
	stylesheet = optarg;
	break;

      case 'v':
	/* print verbose messages */
	want_verbose = 1;
	break;

      case 'h':
	/* imprime el mensaje de ayuda */
	usage();
	return 0;

      default:
	usage();
	return -1;

    }
  }

  /* Ahora asigna la ruta del archivo de salida con el cfdi */
  if ( optind < argc ) {
    archivo = argv[optind++];
  } else {
    /* Ocurrio un error */
    fprintf(stderr, "Es necesario especificar el archivo de salida.\n");
    return -2;
  }

  /* Ahora agrega los datos del CSD */
  cert = lee_certificado_alloc(csd, &serial, want_verbose);
  if ( cert == NULL ) {
    fprintf(stderr, "No pudo leerse el certificado de sello digital.\n");
    return 3;
  }
  if ( serial == NULL ) {
    fprintf(stderr, "No pudo obtenerse el numero de serie del CSD.\n");
    return 4;
  }

  /* Comienza a generar el CFDI */
  factura = crea_comprobante((unsigned char *)"3.2");

  /* Asigna la fecha de emision */
  fecha = obten_fecha_ISO8601_alloc(want_verbose);
  asigna_fecha(factura, fecha);

  /* Asigna la forma de pago */
  asigna_formaDePago(factura, forma_de_pago);

  /* Asigna el metodo de pago */
  agrega_metodoDePago(factura, metodo_de_pago);

  /* Asigna el tipo de comprobante */
  if ( want_ndc ) {
    asigna_tipoDeComprobante(factura, (unsigned char *)"egreso");
  } else {
    asigna_tipoDeComprobante(factura, (unsigned char *)"ingreso");
  }

  /* Asigna el lugar de expedicion */
  asigna_LugarExpedicion(factura, (unsigned char *)"Puebla, Pue.");

  /* Asigna los valores del CSD */
  asigna_noCertificado(factura, serial);
  asigna_certificado(factura, cert);

  /* Asigna el numero de serie */
  if ( serie != NULL ) {
    asigna_serie(factura, serie);
  }

  /* Asigna el folio */
  if ( folio != NULL ) {
    asigna_folio(factura, folio);
  }

  /* Ahora agrega los datos del emisor */
  agrega_Emisor(factura, (unsigned char *)"GAFA7701266V2",
		(unsigned char *)"Alma Delia Gámez Flores");

  /* El domicilio fiscal */
  agrega_Emisor_DomicilioFiscal(factura,
				(unsigned char *)"Av. 8 Poniente",
				(unsigned char *)"716",
				(unsigned char *)"A",
				(unsigned char *)"Centro",
				(unsigned char *)"H. Puebla de Zaragoza",
				(unsigned char *)"Puebla",
				(unsigned char *)"Puebla",
				(unsigned char *)"México",
				(unsigned char *)"Entre 7 Norte y 9 Norte",
				(unsigned char *)"72000");

  /* Ahora el lugar de expedicion */
  agrega_Emisor_ExpedidoEn(factura,
			   (unsigned char *)"Av. 8 Poniente",
			   (unsigned char *)"716",
			   (unsigned char *)"A",
			   (unsigned char *)"Centro",
			   (unsigned char *)"H. Puebla de Zaragoza",
			   (unsigned char *)"Puebla",
			   (unsigned char *)"Puebla",
			   (unsigned char *)"México",
			   (unsigned char *)"Entre 7 Norte y 9 Norte",
			   (unsigned char *)"72000");

  /* Agrega el regimen fiscal */
  agrega_Emisor_RegimenFiscal(factura, (unsigned char *)"Regimen de las personas fisicas con actividades profesionales");

  /* Ahora debemos de agregar los datos del receptor */
  agrega_Receptor(factura, (unsigned char *)"XAXX010101000", NULL);

  /* Ahora agreguemos los conceptos */
  agrega_Concepto(factura,
		  (unsigned char *)"12",
		  NULL,
		  (unsigned char *)"PIEZA",
		  (unsigned char *)"DUCTO CUADRADO 10X10 RILEZ",
		  (unsigned char *)"137.9310",
		  (unsigned char *)"1655.17");

  /* Ahora agrega el IVA */
  agrega_Impuesto_Traslado(factura, (unsigned char *)"IVA",
			   (unsigned char *)"16.00",
			   (unsigned char *)"264.83");

  /* Ahora agrega el subtotal */
  asigna_subTotal(factura, (unsigned char *)"1655.17");

  /* El total */
  asigna_total(factura, (unsigned char *)"1920.00");

  /* Ahora ya estamos en posicion de calcular el sello del CFDI */
  data = genera_comprobante_alloc(factura);
  res = genera_cadena_original(stylesheet, NULL, data, &cadena, want_verbose);
  if ( res != 0 ) {
    /* Error */
    fprintf(stderr, "Error. No fue posible generar la cadena original del CFDI.\n");
    return 5;
  }

  /* Ahora ya podemos generar el sello */
  sello = sello_alloc(private_key, "sha1", cadena, want_verbose);
  if ( sello == NULL ) {
    fprintf(stderr, "Error. No fue posible generar el sello del CFDI.\n");
    return 6;
  }
  /* Y ahora ya podemos agregar el sello */
  asigna_sello(factura, sello);
  xmlFree(cadena);
  xmlFree(data);
  data = genera_comprobante_alloc(factura);
  printf("%s\n", data);
  /* Escribe la informacion a un archivo */
  out = fopen(archivo, "wb");
  fwrite(data, sizeof(xmlChar), xmlStrlen(data), out);
  fclose(out);
  xmlFree(data);

  /* Libera la memoria */
  termina_comprobante(factura);
  free(sello);
  free(fecha);
  free(cert);
  free(serial);

  return 0;
}
