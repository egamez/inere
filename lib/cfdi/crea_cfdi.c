#ifndef INERE_CREA_CFDI_H
#include "inere/cfdi/crea_cfdi.h"
#endif
#ifndef EDISSON_CERTIFICADO_H_INCLUDED
#include "inere/cfdi/certificado.h"
#endif


#include <stdio.h>

/* algunas constantes */
static const xmlChar *version = (const xmlChar *)"3.2";
static const xmlChar *cfdi_url = (const xmlChar *)"http://www.sat.gob.mx/cfd/3";
static const xmlChar *xsi_url = (const xmlChar *)"http://www.w3.org/2001/XMLSchema-instance";
static const char *archivo_certificado = "/home/egamez/.CFDi/.SAT/csd/gafa7701266v2_1202072108s.cer";
static const char *archivo_emisor = "/home/egamez/git/repos/inere/test/emisor.xml";


/* Forward declaration */
xmlNodePtr crea_elemento_Emisor(const char *source,
				const xmlNsPtr namespace,
				const int verbose);
xmlNodePtr crea_elemento_Receptor(const xmlChar *rfc,
				  const xmlChar *nombre,
				  const xmlChar *calle,
				  const xmlChar *noExterior,
				  const xmlChar *noInterior,
				  const xmlChar *colonia,
				  const xmlChar *localidad,
				  const xmlChar *referencia,
				  const xmlChar *municipio,
				  const xmlChar *estado,
				  const xmlChar *pais,
				  const xmlChar *codigoPostal,
				  const xmlNsPtr namespace,
				  const int verbose);
xmlNodePtr crea_elemento_Comprobante(const xmlChar *serie,
				     const xmlChar *folio,
				     const xmlChar *formaDePago,
				     const xmlChar *condicionesDePago,
				     const xmlChar *motivoDescuento,
				     const xmlChar *TipoCambio,
				     const xmlChar *Moneda,
				     const xmlChar *tipoDeComprobante,
				     const xmlChar *metodoDePago,
				     const xmlChar *LugarExpedicion,
				     const xmlChar *NumCtaPago,
				     const xmlNsPtr namespace,
				     const int verbose);


xmlNodePtr
crea_elemento_Emisor(const char *source,
		     const xmlNsPtr namespace,
		     const int verbose)
{
  xmlDocPtr fuente = NULL;
  xmlNodePtr old = NULL;
  xmlNodePtr emisor = NULL;

  /* Lee el archivo que contiene la informacion del emisor */
  fuente = xmlReadFile(source, "UTF-8", XML_PARSE_NOBLANKS|XML_PARSE_COMPACT);
  if ( fuente == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible leer el archivo que contiene los datos del emisor (%s).\n", __FILE__, __LINE__, source);
    }
    return NULL;
  }

  /* Lee el nodo raiz, que debe corresponder al emisor */
  old = xmlDocGetRootElement(fuente);
  if ( old == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible leer el elemento que contiene la información del emisor.\n", __FILE__, __LINE__);
    }
    xmlFreeDoc(fuente);
    return NULL;
  }

  /* Ahora, copia el nodo */
  emisor = xmlCopyNode(old, 1);

  /* Libera la memoria consumida */
  xmlFreeDoc(fuente);

  /* Set the namespace */
  xmlSetNs(emisor, namespace);

  /* Ahora agrega el namespace para cada uno de los nodos hijo */
  old = emisor->children;
  while ( old != NULL ) {
    xmlSetNs(old, namespace);
    old = old->next;
  }

  return emisor;
}

xmlNodePtr
crea_elemento_Receptor(const xmlChar *rfc,
			const xmlChar *nombre,
			const xmlChar *calle,
			const xmlChar *noExterior,
			const xmlChar *noInterior,
			const xmlChar *colonia,
			const xmlChar *localidad,
			const xmlChar *referencia,
			const xmlChar *municipio,
			const xmlChar *estado,
			const xmlChar *pais,
			const xmlChar *codigoPostal,
			const xmlNsPtr namespace,
			const int verbose)
{
  xmlNodePtr receptor = NULL;
  xmlNodePtr domicilio = NULL;

  /* Verifica que al menos los parametros requeridos hallan sido suministrados*/
  if ( rfc == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. Es necesario suministrar al menos la clave del R.F.C. del receptor del CFDi.\n", __FILE__, __LINE__);
    }
    return NULL;
  }

  receptor = xmlNewNode(namespace, (const xmlChar *)"Receptor");
  xmlNewProp(receptor, (const xmlChar *)"rfc", rfc);

  if ( nombre != NULL ) {
    xmlNewProp(receptor, (const xmlChar *)"nombre", nombre);
  }

  /* Ahora el nodo hijo con el domicilio. El unico atributo requerido para
   * el domicilio es el pais, de modo que verifica si este valor fue
   * proporcionado para agregar el nodo hijo */
  if ( pais != NULL ) {

    domicilio = xmlNewChild(receptor, namespace, (const xmlChar *)"Domicilio", NULL);

    if ( calle != NULL ) {
      xmlNewProp(domicilio, (const xmlChar *)"calle", calle);
    }

    if ( noExterior != NULL ) {
      xmlNewProp(domicilio, (const xmlChar *)"noExterior", noExterior);
    }

    if ( noInterior != NULL ) {
      xmlNewProp(domicilio, (const xmlChar *)"noInterior", noInterior);
    }

    if ( colonia != NULL ) {
      xmlNewProp(domicilio, (const xmlChar *)"colonia", colonia);
    }

    if ( localidad != NULL ) {
      xmlNewProp(domicilio, (const xmlChar *)"localidad", localidad);
    }

    if ( referencia != NULL ) {
      xmlNewProp(domicilio, (const xmlChar *)"referencia", referencia);
    }

    if ( municipio != NULL ) {
      xmlNewProp(domicilio, (const xmlChar *)"municipio", municipio);
    }

    if ( estado != NULL ) {
      xmlNewProp(domicilio, (const xmlChar *)"estado", estado);
    }

    xmlNewProp(domicilio, (const xmlChar *)"pais", pais);

    if ( codigoPostal != NULL ) {
      xmlNewProp(domicilio, (const xmlChar *)"codigoPostal", codigoPostal);
    }

  }

  return receptor;
}


xmlNodePtr
crea_elemento_Comprobante(const xmlChar *serie,
			  const xmlChar *folio,
			  const xmlChar *formaDePago,
			  const xmlChar *condicionesDePago,
			  const xmlChar *motivoDescuento,
			  const xmlChar *TipoCambio,
			  const xmlChar *Moneda,
			  const xmlChar *tipoDeComprobante,
			  const xmlChar *metodoDePago,
			  const xmlChar *LugarExpedicion,
			  const xmlChar *NumCtaPago,
			  const xmlNsPtr namespace,
			  const int verbose)
{
  xmlNodePtr comprobante = NULL;
  char *certificado = NULL;
  xmlChar *noCertificado = NULL;

  /* Agrega cada una de los atributotos del elemento Comprobante.
   * Toma en cuenta los elementos obligatorios */
  if ( formaDePago == NULL ) {
    /* La forma de pago */
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El atributo \"formaDePago\", del Comprobante es de cáracter obligatorio y no fue correctamente suministrado\n", __FILE__, __LINE__);
    }
    return NULL;
  }

  if ( tipoDeComprobante == NULL ) {
    /* El tipo de comprobante, egreso o ingreso */
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El atributo \"tipoDeComprobante\" del Comprobante es de cáracter obligatorio y no fue correctamente suministrado\n", __FILE__, __LINE__);
    }
    return NULL;

  } else if ( xmlStrcasecmp(tipoDeComprobante, (const xmlChar *)"ingreso") &&
	      xmlStrcasecmp(tipoDeComprobante, (const xmlChar *)"egreso") ) {
    /* Solo hay dos opciones para este atributo y ninguna de ellas fue
     * suministrada */
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El valor para el atributo \"tipoDeComprobante\" del Comprobante no fue ni de tipo \"egreso\" o \"ingreso\", el valor proporcionado fue: \"%s\".\n", __FILE__, __LINE__, tipoDeComprobante);
    }
    return NULL;
  }

  if ( metodoDePago == NULL ) {
    /* El metodo de pago */
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El atributo \"metodoDePago\", del Comprobante es de cáracter obligatorio y no fue correctamente suministrado\n", __FILE__, __LINE__);
    }
    return NULL;
  }

  if ( LugarExpedicion == NULL ) {
    /* El luga de expedicion */
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El atributo \"LugarExpedicion\", del Comprobante es de cáracter obligatorio y no fue correctamente suministrado\n", __FILE__, __LINE__);
    }
    return NULL;
  }

  /* Crea ahora el nodo */
  comprobante = xmlNewNode(namespace, (const xmlChar *)"Comprobante");

  /* Agrega los atributos */
  xmlNewProp(comprobante, (const xmlChar *)"version", version);
  xmlNewProp(comprobante, (const xmlChar *)"formaDePago", formaDePago);
  xmlNewProp(comprobante, (const xmlChar *)"tipoDeComprobante", tipoDeComprobante);
  xmlNewProp(comprobante, (const xmlChar *)"metodoDePago", metodoDePago);
  xmlNewProp(comprobante, (const xmlChar *)"LugarExpedicion", LugarExpedicion);

  /* Ahora agrega algunos atributos opcionales */
  if ( serie != NULL ) {
    xmlNewProp(comprobante, (const xmlChar *)"serie", serie);
  }

  if ( folio != NULL ) {
    xmlNewProp(comprobante, (const xmlChar *)"folio", folio);
  }

  if ( condicionesDePago != NULL ) {
    xmlNewProp(comprobante, (const xmlChar *)"condicionesDePago", condicionesDePago);
  }

  if ( motivoDescuento != NULL ) {
    xmlNewProp(comprobante, (const xmlChar *)"motivoDescuento", motivoDescuento);
  }

  if ( TipoCambio != NULL ) {
    xmlNewProp(comprobante, (const xmlChar *)"TipoCambio", TipoCambio);
  }

  if ( Moneda != NULL ) {
    xmlNewProp(comprobante, (const xmlChar *)"Moneda", Moneda);
  }

  if ( NumCtaPago != NULL ) {
    xmlNewProp(comprobante, (const xmlChar *)"NumCtaPago", NumCtaPago);
  }

  /* Ahora deberemos de agregar los parametros como: certificado,
   * noCertificado, subTotal, descuento y total, los cuales deben de ser
   * inferidos */
  certificado = load_certificate_alloc(archivo_certificado, &noCertificado, verbose);
  xmlNewProp(comprobante, (const xmlChar *)"certificado", (const xmlChar *)certificado);
  xmlNewProp(comprobante, (const xmlChar *)"noCertificado", noCertificado);
  free(noCertificado);
  free(certificado);

  return comprobante;
}

/**
 *
 */
xmlDocPtr
crea_cfdi(const int verbose)
{
  xmlNsPtr cfdi_ns = NULL;
  xmlNsPtr xsi_ns = NULL;
  xmlNodePtr comprobante = NULL;
  xmlNodePtr emisor = NULL;
  xmlNodePtr receptor = NULL;
  xmlDocPtr doc = NULL;

  /* Crea el documento */
  doc = xmlNewDoc((const xmlChar *)"1.0");
  if ( doc == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible crear la estructura del documento.\n", __FILE__, __LINE__);
    }
    return NULL;
  }

  /* Crea el namespace raiz */
  cfdi_ns = xmlNewNs(comprobante, cfdi_url, (const xmlChar *)"cfdi");
  if ( cfdi_ns == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible crear el namespace primario.\n", __FILE__, __LINE__);
    }
    xmlFreeDoc(doc);
    return NULL;
  }

  /* Crea el elemento raiz */
  comprobante = crea_elemento_Comprobante((const xmlChar *)"C",
					  (const xmlChar *)"00001",
					  (const xmlChar *)"Pago en una sola..",
					  (const xmlChar *)"Quien sabe",
					  NULL,
					  (const xmlChar *)"1.0",
					  (const xmlChar *)"Pesos Mexicanos",
					  (const xmlChar *)"ingreso",
					  (const xmlChar *)"efectivo",
					  (const xmlChar *)"Puebla",
					  (const xmlChar *)"----",
					  cfdi_ns,
					  verbose);

  if ( comprobante == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible crear el elemento raiz\n", __FILE__, __LINE__);
    }
    xmlFreeNs(cfdi_ns);
    xmlFreeDoc(doc);
    return NULL;
  }

  /* Crea el namespace xsi */
  xsi_ns = xmlNewNs(comprobante, xsi_url, (const xmlChar *)"xsi");

  /* Crea el nodo del emisor */
  emisor = crea_elemento_Emisor(archivo_emisor, cfdi_ns, verbose);
  if ( emisor == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible crear el elemento \"Emisor\",\n", __FILE__, __LINE__);
    }
    xmlFreeNode(comprobante);
    xmlFreeNs(cfdi_ns);
    xmlFree(doc);
    return NULL;
  }

  /* Agrega el nodo emisor como child del nodo comprobante */
  xmlAddChild(comprobante, emisor);

  /* Ahora el nodo del receptor */
  receptor = crea_elemento_Receptor((const xmlChar *)"FODI460918IZ3",
			(const xmlChar *)"Flores Domínguez, Irene",
			(const xmlChar *)"4 Sur",
			(const xmlChar *)"306",
			(const xmlChar *)"506",
			(const xmlChar *)"Centro",
			NULL,
			(const xmlChar *)"Entre 3 Oriente y 5 Oriente",
			(const xmlChar *)"Puebla",
			(const xmlChar *)"Puebla",
			(const xmlChar *)"México",
			(const xmlChar *)"72000",
			cfdi_ns,
			verbose);
  /* Agrega este ultimo nodo, como un nodo hijo */
  xmlAddChild(comprobante, receptor);

  /* Ahora tenemos que agregar el nodo correspondiente a los Conceptos */

  /* A los Impuestos */

  /* Al Complemento */

  /* Y posiblemente a la Addenda */

  /* Define el nodo comprobante como el nodo raiz */
  xmlDocSetRootElement(doc, comprobante);

  /* Reajusta el namespace */
  xmlReconciliateNs(doc, comprobante);

  xmlFreeNs(cfdi_ns);

  return doc;
}
