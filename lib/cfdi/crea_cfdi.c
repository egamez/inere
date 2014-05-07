/*
 * Copyright (c) 2014, L3a,
 *                     Enrique Gamez Flores <egamez@edisson.com.mx>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef INERE_CREA_CFDI_H
#include "inere/cfdi/crea_cfdi.h"
#endif
#ifndef EDISSON_CERTIFICADO_H_INCLUDED
#include "inere/cfdi/certificado.h"
#endif
#ifndef EDISSON_GENERA_CADENA_ORIGINAL_H
#include "inere/cfdi/genera_cadena_original.h"
#endif
#ifndef EDISSON_SELLO_H_INCLUDED
#include "inere/cfdi/sello.h"
#endif

#include <time.h>
#include <stdio.h>

/* algunas constantes */
static const xmlChar *version = (const xmlChar *)"3.2";
static const xmlChar *cfdi_url = (const xmlChar *)"http://www.sat.gob.mx/cfd/3";
static const xmlChar *xsi_url = (const xmlChar *)"http://www.w3.org/2001/XMLSchema-instance";
static const char *archivo_certificado = "archivo.cer";
static const char *keycert = "archivo.key";
static const char *archivo_emisor = "emisor.xml";
static const char *cadena_original_stylesheet = "cadenaoriginal_3_2.xslt";


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
xmlChar *obten_fecha_ISO8601_alloc(const int verbose);

/**
 * 2014-05-05T21:35:56
 */
xmlChar *
obten_fecha_ISO8601_alloc(const int verbose)
{
  time_t t;
  struct tm tiempo;
  size_t res;
  const size_t len = 20;
  xmlChar *fecha = (xmlChar *)calloc(len, sizeof(xmlChar));

  t = time(&t);
  localtime_r(&t, &tiempo);
  res = strftime((char *)fecha, len, "%Y-%m-%dT%H:%M:%S", &tiempo);
  if ( res == 0 ) {
    /* No se pudo realizar la conversion */
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No pudo generarse la fecha segun el formato requerido por el Comprobante.\n", __FILE__, __LINE__);
    }
    free(fecha);
    return NULL;
  }

  return fecha;
}

/**
 *
 * Si importe es NUL entonces esta función se encargará de calcular el
 * correspondiente importe
 */
cfdi_items_list_t *
append_concepto(cfdi_items_list_t *head,
		const xmlChar *cantidad,
		const xmlChar *unidad,
		const xmlChar *noIdentificacion,
		const xmlChar *descripcion,
		const xmlChar *valorUnitario,
		const xmlChar *importe,
		const int verbose)
{
  cfdi_items_list_t *current = head;
  cfdi_items_list_t *tmp = NULL;

  tmp = (cfdi_items_list_t *)malloc(sizeof(cfdi_items_list_t));
  if ( tmp == NULL ) {
    /* Un error ocurrio */
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible crear una nueva entrada en la lista de conceptos\n", __FILE__, __LINE__);
    }
    return NULL;
  }

  /* Crea el nodo Concepto */
  tmp->xml_node = xmlNewNode(NULL, (const xmlChar *)"Concepto");
  if ( tmp->xml_node == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible crear el nodo Concepto.\n", __FILE__, __LINE__);
    }
    free(tmp);
    return NULL;
  }

  xmlNewProp(tmp->xml_node, (const xmlChar *)"cantidad", cantidad);
  xmlNewProp(tmp->xml_node, (const xmlChar *)"unidad", unidad);
  if ( noIdentificacion != NULL ) {
    xmlNewProp(tmp->xml_node, (const xmlChar *)"noIdentificacion", noIdentificacion );
  }
  xmlNewProp(tmp->xml_node, (const xmlChar *)"descripcion", descripcion);
  xmlNewProp(tmp->xml_node, (const xmlChar *)"valorUnitario", valorUnitario);
  if ( importe != NULL ) {
    xmlNewProp(tmp->xml_node, (const xmlChar *)"importe", importe);

  } else {
    /* Calcula el importe correspondiente */
  }

  if ( head == NULL ) {
    /* Este es el primer componente de la lista */
    head = tmp;
    head->next = NULL;

  } else {
    /* Situate al final de la lista para agregar esta entrada */
    while ( current->next != NULL ) {
      current = current->next;
    }

    /* Agrega esta entrada */
    tmp->next = NULL;
    current->next = tmp;
  }

  return head;
}

/**
 *	tipo		Traslado, Retencion
 *	impuesto	IVA o IEPS
 */
cfdi_items_list_t *
append_impuesto(cfdi_items_list_t *head,
		const xmlChar *tipo,
		const xmlChar *impuesto,
		const xmlChar *tasa,
		const xmlChar *importe,
		const int verbose)
{
  cfdi_items_list_t *current = head;
  cfdi_items_list_t *tmp = NULL;

  if ( tipo == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No es posible crear el elemento de impuesto sin antes especificar el tipo de impuesto (Retencion o Traslado)\n", __FILE__, __LINE__);
    }
    return NULL;
  }

  if ( impuesto == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El atributo \"impuesto\" es requerido para la elobaración del comprobante.\n", __FILE__, __LINE__);
    }
    return NULL;
  }

  if ( importe == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El atributo \"importe\" es requerido para la elobaración del comprobante.\n", __FILE__, __LINE__);
    }
    return NULL;
  }

  /* Verifica el nombre del impuesto, solo hay dos valores permitidos
   * IVA e IEPS */
  if ( xmlStrcasecmp(impuesto, (const xmlChar *)"IVA") &&
       xmlStrcasecmp(impuesto, (const xmlChar *)"IEPS") ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. Solo son permitidos dos valores para el atributo \"impuesto\": IVA o IEPS, pero el valor suministrado para este atributo fue: \"%s\"\n", __FILE__, __LINE__, impuesto);
    }
    return NULL;
  }

  /* Verifica que el tipo de impuesto sea el adecuado */
  if ( xmlStrcasecmp(tipo, (const xmlChar *)"Traslado") &&
       xmlStrcasecmp(tipo, (const xmlChar *)"Retencion") ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El tipo de impuesto ... %s\n", __FILE__, __LINE__, tipo);
    }
    return NULL;
  }

  tmp = (cfdi_items_list_t *)malloc(sizeof(cfdi_items_list_t));
  if ( tmp == NULL ) {
    /* Un error ocurrio */
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible crear una nueva entrada en la lista de Impuesto trasladado\n", __FILE__, __LINE__);
    }
    return NULL;
  }

  /* Crea el nodo correspondiente segun el tipo */
  if ( xmlStrcasecmp(tipo, (const xmlChar *)"Traslado") == 0 ) {
    tmp->xml_node = xmlNewNode(NULL, (const xmlChar *)"Traslado");
  } else if ( xmlStrcasecmp(tipo, (const xmlChar *)"Retencion") == 0 ) {
    tmp->xml_node = xmlNewNode(NULL, (const xmlChar *)"Retencion");
  }

  if ( tmp->xml_node == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible crear el nodo del tipo de impuesto %s.\n", __FILE__, __LINE__, tipo);
    }
    free(tmp);
    return NULL;
  }

  xmlNewProp(tmp->xml_node, (const xmlChar *)"impuesto", impuesto);
  xmlNewProp(tmp->xml_node, (const xmlChar *)"importe", importe);
  if ( xmlStrcasecmp(tipo, (const xmlChar *)"Traslado") == 0 ) {
    xmlNewProp(tmp->xml_node, (const xmlChar *)"tasa", tasa);
  }

  if ( head == NULL ) {
    /* Este es el primer componente de la lista */
    head = tmp;
    head->next = NULL;

  } else {
    /* Situate al final de la lista para agregar esta entrada */
    while ( current->next != NULL ) {
      current = current->next;
    }

    /* Agrega esta entrada */
    tmp->next = NULL;
    current->next = tmp;
  }

  return head;
}

/**
 *
 */
void
free_cfdi_items_list(cfdi_items_list_t* head)
{
  cfdi_items_list_t *current = head;
  cfdi_items_list_t *tmp = NULL;

  while ( current != NULL ) {
    tmp = current;
/*
    if ( current->xml_node != NULL ) {
      xmlFreeNode(current->xml_node);
    }
*/
    current = current->next;
    free(tmp);
  }
  head = NULL;
}

/**
 *
 */
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

/**
 *
 */
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

/**
 *
 */
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

  /* Crea el namespace xsi. pero para que el CFDi se vea bonito
   * no declares este namespace de la manera usual (via xmlNewNs)
   * mejor solo agregalo como un atributo cualquiera */
  xmlNewProp(comprobante, (const xmlChar *)"xmlns:xsi", xsi_url);
  xmlNewProp(comprobante, (const xmlChar *)"xsi:schemaLocation", (const xmlChar *)"http://www.sat.gob.mx/cfd/3 http://www.sat.gob.mx/sitio_internet/cfd/3/cfdv32.xsd");

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
crea_cfdi(const xmlChar *subTotal,
	  const xmlChar *descuento,
	  const xmlChar *total,
	  cfdi_items_list_t *productos,
	  cfdi_items_list_t *retencion,
	  cfdi_items_list_t *traslado,
	  const int verbose)
{
  xmlNsPtr cfdi_ns = NULL;
  xmlNsPtr xsi_ns = NULL;
  xmlNodePtr comprobante = NULL;
  xmlNodePtr emisor = NULL;
  xmlNodePtr receptor = NULL;
  xmlNodePtr conceptos = NULL;
  xmlNodePtr impuestos = NULL;
  xmlNodePtr complemento = NULL;
  xmlNodePtr addenda = NULL;
  cfdi_items_list_t *prod = NULL;
  cfdi_items_list_t *imps = NULL;
  xmlDocPtr doc = NULL;
  xmlChar *cadena = NULL;
  xmlChar *fecha = NULL;
  char *sello = NULL;
  int res = 1;

  if ( traslado == NULL && retencion == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. Es necesario declarar que tipo de impuestos amparan el comprobante (mediante la creación de listas.)\n", __FILE__, __LINE__);
    }
    return NULL;
  }

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

  /* Crea el nodo del emisor */
  emisor = crea_elemento_Emisor(archivo_emisor, cfdi_ns, verbose);
  if ( emisor == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible crear el elemento \"Emisor\",\n", __FILE__, __LINE__);
    }
    xmlFreeNs(xsi_ns);
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

  /* Ahora, si el subTotal. descuentos y total son NULL
   * se deberan de inferir estas cantidad de la suma de los conceptos */
  if ( subTotal != NULL ) {
    xmlNewProp(comprobante, (const xmlChar *)"subTotal", subTotal);
  }
  if ( descuento != NULL ) {
    xmlNewProp(comprobante, (const xmlChar *)"descuento", descuento);
  }
  if ( total != NULL ) {
    xmlNewProp(comprobante, (const xmlChar *)"total", total);
  }


  /* Ahora tenemos que agregar el nodo correspondiente a los Conceptos */
  conceptos = xmlNewChild(comprobante, cfdi_ns, (const xmlChar *)"Conceptos", NULL);
  /* Ahora agrega cada uno de los conceptos individuales */
  prod = productos;
  while ( prod != NULL ) {
    xmlSetNs(prod->xml_node, cfdi_ns);
    xmlAddChild(conceptos, prod->xml_node);
    prod = prod->next;
  }

  /* A los Impuestos */
  impuestos = xmlNewChild(comprobante, cfdi_ns, (const xmlChar *)"Impuestos", NULL);
  if ( retencion != NULL ) {
    xmlNodePtr retenciones = xmlNewChild(impuestos, cfdi_ns, (const xmlChar *)"Retenciones", NULL);
    imps = retencion;
    while ( imps != NULL ) {
      xmlSetNs(imps->xml_node, cfdi_ns);
      xmlAddChild(retenciones, imps->xml_node);
      imps = imps->next;
    }
  }

  if ( traslado != NULL ) {
    xmlNodePtr traslados = xmlNewChild(impuestos, cfdi_ns, (const xmlChar *)"Traslados", NULL);
    imps = traslado;
    while ( imps != NULL ) {
      xmlSetNs(imps->xml_node, cfdi_ns);
      xmlAddChild(traslados, imps->xml_node);
      imps = imps->next;
    }
  }

  /* Al Complemento */
  complemento = xmlNewChild(comprobante, cfdi_ns, (const xmlChar *)"Complemento", NULL);

  /* Y posiblemente a la Addenda */
  addenda = xmlNewChild(comprobante, cfdi_ns, (const xmlChar *)"Addenda", NULL);

  /* Define el nodo comprobante como el nodo raiz */
  xmlDocSetRootElement(doc, comprobante);

  /* Y agrega la fecha */
  fecha = obten_fecha_ISO8601_alloc(verbose);
  if ( fecha == NULL ) {
    /* No pudo generarse la fecha */
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No hay fecha a la cual asociar con el comprobante\n", __FILE__, __LINE__);
    }
    xmlFreeDoc(doc);
    return NULL;
  }
  xmlNewProp(comprobante, (const xmlChar *)"fecha", fecha);
  free(fecha);

  /* Reajusta el namespace */
  xmlReconciliateNs(doc, comprobante);

  /* Ahora, con toda la informacion de arriba, deberá ser posible
   * obtener la cadena original y de esta ya se podrá generar el sello */
  res = cadena_original(cadena_original_stylesheet,
			doc,
			&cadena,
			verbose);
  if ( res == 0 ) {
    /* Si fue posible extraer la cadena, imprime el resultado */
    if ( verbose ) {
      printf("Cadena original: %s\n", cadena);
    }

  } else if ( res == 1 ) {
    fprintf(stderr, "%s:%d Error. La stylesheet (%s) no pudo ser compilada.\n", __FILE__, __LINE__, cadena_original_stylesheet);
    xmlFreeDoc(doc);
    return NULL;

  } else if ( res == 2 ) {
    fprintf(stderr, "%s:%d Error. Las transformaciones definidas en la stylesheet (%s) no pudieron apicarse al Comprobante\n", __FILE__, __LINE__, cadena_original_stylesheet);
    xmlFreeDoc(doc);
    return NULL;

  } else if ( res == 3 ) {
    fprintf(stderr, "%s:%d Error. No fue posible escribir la cadena original al buffer temporal.\n", __FILE__, __LINE__);
    xmlFreeDoc(doc);
    return NULL;

  }

  /* Ahora obten el sello digital */
  sello = sello_alloc(keycert, "sha1", cadena, verbose);
  xmlFree(cadena);
  if ( sello == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible generar el sello digital del comprobante.\n", __FILE__, __LINE__);
    }
    xmlFree(doc);
    return NULL;
  }

  if ( verbose ) {
    printf("Sello digital: %s\n", sello);
  }

  /* Ahora agrega el sello al comprobante */
  xmlNewProp(comprobante, (const xmlChar *)"sello", (const xmlChar *)sello);
  free(sello);

  return doc;
}
