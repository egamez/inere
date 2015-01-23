/*
 * Copyright (c) 2014-2015, L3a,
 *                          Enrique Gamez Flores <egamez@edisson.com.mx>
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
#ifndef INERE_CFDI_LEE_CFDI_H_
#include "inere/cfdi/lee_cfdi.h"
#endif

#include <string.h>
#include <unistd.h>

#include <libxml/tree.h>

/* Forward declarations */
int lee_conceptos(const xmlNodePtr node, Comprobante_t *cfdi, const int verbose);
int lee_impuestos(const xmlNodePtr node, Comprobante_t *cfdi, const int verbose);
int lee_complementos(const xmlNodePtr node, Comprobante_t *cfdi, const int verbose);
int lee_timbre_fiscal(const xmlNodePtr node, Comprobante_t *cfdi, const int verbose);
int lee_datos_receptor(const xmlNodePtr node, Comprobante_t *cfdi, const int verbose);
int lee_datos_emisor(const xmlNodePtr node, Comprobante_t *cfdi, const int verbose);


Comprobante_t *
lee_cfdi(const char *filename, const int verbose)
{
  xmlDocPtr doc = NULL;
  xmlNodePtr node = NULL;
  xmlNodePtr cur = NULL;
  Comprobante_t *cfdi = NULL;

  /* Hay dos posibilidades:
   *
   *	- Una es que hallas pasado el nombre del archivo que contiene el
   *	  el CFDi, la otra es
   *	- Que hallas pasado el CFDi mismo en un string
   */
  if ( access(filename, R_OK) == 0 ) {
    /* El argumento pasado corresponde a la ruta del archivo que contiene
     * el CFDi */
    /* Ahora lee el archivo */
    doc = xmlParseFile(filename);

  } else {
    /* El string dado contiene el CFDi mismo */
    doc = xmlReadMemory(filename, strlen(filename), "http://www.sat.gob.mx/cfd/3", "UTF-8", XML_PARSE_RECOVER);
 
  }
  if ( doc == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible leer el archivo o documento: %s\n", __FILE__, __LINE__, filename);
    }
    return NULL;
  }

  /* Ahora convierte el tree en el la estructura comprobante con la
   * finalidad de eliminar la lectura de todos los nodos */
  node = xmlDocGetRootElement(doc);
  if ( node == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible extrear el elemento raiz del archivo %s\n", __FILE__, __LINE__, filename);
    }
    xmlFreeDoc(doc);
    return NULL;
  }

  cfdi = (Comprobante_t *)malloc(sizeof(Comprobante_t));

  /* Lee primero los atributos requeridos */
  cfdi->version           = xmlGetProp(node, (const xmlChar *)"version");
  cfdi->fecha             = xmlGetProp(node, (const xmlChar *)"fecha");
  cfdi->sello             = xmlGetProp(node, (const xmlChar *)"sello");
  cfdi->formaDePago       = xmlGetProp(node, (const xmlChar *)"formaDePago");
  cfdi->noCertificado     = xmlGetProp(node, (const xmlChar *)"noCertificado");
  cfdi->certificado       = xmlGetProp(node, (const xmlChar *)"certificado");
  cfdi->subTotal          = xmlGetProp(node, (const xmlChar *)"subTotal");
  cfdi->total             = xmlGetProp(node, (const xmlChar *)"total");
  cfdi->tipoDeComprobante = xmlGetProp(node, (const xmlChar *)"tipoDeComprobante");
  cfdi->metodoDePago      = xmlGetProp(node, (const xmlChar *)"metodoDePago");
  cfdi->LugarExpedicion   = xmlGetProp(node, (const xmlChar *)"LugarExpedicion");

  /* Ahora algunos atributos opcionales */
  cfdi->serie             = xmlGetProp(node, (const xmlChar *)"serie");
  cfdi->folio             = xmlGetProp(node, (const xmlChar *)"folio");
  cfdi->condicionesDePago = xmlGetProp(node, (const xmlChar *)"condicionesDePago");
  cfdi->descuento         = xmlGetProp(node, (const xmlChar *)"descuento");
  cfdi->motivoDescuento   = xmlGetProp(node, (const xmlChar *)"motivoDescuento");
  cfdi->TipoCambio        = xmlGetProp(node, (const xmlChar *)"TipoCambio");
  cfdi->Moneda            = xmlGetProp(node, (const xmlChar *)"Moneda");
  cfdi->NumCtaPago        = xmlGetProp(node, (const xmlChar *)"NumCtaPago");

  /* Ahora lee cada uno de los nodos hijo del comprobante */
  cur = node->children;
  while ( cur != NULL ) {

    if ( cur->type == XML_ELEMENT_NODE ) {

      if ( verbose ) {
	printf("%s:%d Info. Leyendo nodo \"%s\".\n", __FILE__, __LINE__, cur->name);
      }

      if ( xmlStrEqual(cur->name, (const xmlChar *)"Emisor") ) {
	/* Lee los datos del emisor */
	lee_datos_emisor(cur, cfdi, verbose);
      }

      if ( xmlStrEqual(cur->name, (const xmlChar *)"Receptor") ) {
	/* Lee los datos correspondientes al Receptor */
	lee_datos_receptor(cur, cfdi, verbose);
      }

      if ( xmlStrEqual(cur->name, (const xmlChar *)"Conceptos") ) {
	/* Lee ahora todo el nodo correspondiente a los conceptos */
	lee_conceptos(cur, cfdi, verbose);
      }

      if ( xmlStrEqual(cur->name, (const xmlChar *)"Impuestos") ) {
	/* Ahora es el turno de los impuestos */
	lee_impuestos(cur, cfdi, verbose);
      }

      if ( xmlStrEqual(cur->name, (const xmlChar *)"Complemento") ) {
	/* Ahora lee los datos correspondientes los complementos del CFDi */
	lee_complementos(cur, cfdi, verbose);
      }

      /* Este es el sitio para leer la addenda */

    }
    cur = cur->next;
  }

  xmlFreeDoc(doc);

  return cfdi;
}

/**
 *
 */
void
termina_cfdi(Comprobante_t *cfdi)
{
  Concepto_list_t *prods = NULL;
  Concepto_list_t *prods_tmp = NULL;
  InformacionAduanera_list_t *info_a = NULL;
  InformacionAduanera_list_t *info_a_tmp = NULL;

  /* Los datos del timbre */
  if ( cfdi->Complemento->TimbreFiscalDigital != NULL ) {
    xmlFree(cfdi->Complemento->TimbreFiscalDigital->selloSAT);
    xmlFree(cfdi->Complemento->TimbreFiscalDigital->selloCFD);
    xmlFree(cfdi->Complemento->TimbreFiscalDigital->FechaTimbrado);
    xmlFree(cfdi->Complemento->TimbreFiscalDigital->noCertificadoSAT);
    xmlFree(cfdi->Complemento->TimbreFiscalDigital->UUID);
    xmlFree(cfdi->Complemento->TimbreFiscalDigital->version);
    free(cfdi->Complemento->TimbreFiscalDigital);
    cfdi->Complemento->TimbreFiscalDigital = NULL;
  }

  /* El complemento */
  if ( cfdi->Complemento != NULL ) {
    free(cfdi->Complemento);
    cfdi->Complemento = NULL;
  }

  /* Los impuestos */
  if ( cfdi->Impuestos != NULL ) {
    if ( cfdi->Impuestos->Traslados != NULL ) {
      Traslado_list_t *node = cfdi->Impuestos->Traslados;
      Traslado_list_t *tmp = NULL;
      while ( node != NULL ) {
	tmp = node;
	xmlFree(tmp->impuesto);
	xmlFree(tmp->importe);
	xmlFree(tmp->tasa);
	node = node->next;
	free(tmp);
      }
      cfdi->Impuestos->Traslados = NULL;
    }

    if ( cfdi->Impuestos->Retenciones != NULL ) {
      Retencion_list_t *node = cfdi->Impuestos->Retenciones;
      Retencion_list_t *tmp = NULL;
      while ( node != NULL ) {
	tmp = node;
	xmlFree(tmp->impuesto);
	xmlFree(tmp->importe);
	node = node->next;
	free(tmp);
      }
      cfdi->Impuestos->Retenciones = NULL;
    }

    free(cfdi->Impuestos);
    cfdi->Impuestos = NULL;
  }

  /* Ahora los conceptos */
  prods = cfdi->Conceptos;
  while ( prods != NULL ) {
    prods_tmp = prods;
    if ( prods_tmp->Parte != NULL ) {
      printf("Se requiere codigo para limpiar Concepto->Parte");
    }
    if ( prods_tmp->CuentaPredial != NULL ) {
      xmlFree(prods_tmp->CuentaPredial->numero);
      free(prods_tmp->CuentaPredial);
      prods_tmp->CuentaPredial = NULL;
    }
    if ( prods_tmp->InformacionAduanera != NULL ) {
      info_a = prods_tmp->InformacionAduanera;
      while ( info_a != NULL ) {
	info_a_tmp = info_a;
	xmlFree(info_a_tmp->aduana);
	xmlFree(info_a_tmp->fecha);
	xmlFree(info_a_tmp->numero);
	info_a = info_a->next;
	free(info_a_tmp);
      }
      prods_tmp->InformacionAduanera = NULL;
    }
    xmlFree(prods_tmp->importe);
    xmlFree(prods_tmp->valorUnitario);
    xmlFree(prods_tmp->descripcion);
    xmlFree(prods_tmp->unidad);
    if ( prods_tmp->noIdentificacion != NULL ) {
      xmlFree(prods_tmp->noIdentificacion);
    }
    xmlFree(prods_tmp->cantidad);
    prods = prods->next;
    free(prods_tmp);
    cfdi->Conceptos = NULL;
  }

  /* Ahora el receptor */
  if ( cfdi->Receptor->Domicilio != NULL ) {
    if ( cfdi->Receptor->Domicilio->codigoPostal != NULL ) {
      xmlFree(cfdi->Receptor->Domicilio->codigoPostal);
    }
    if ( cfdi->Receptor->Domicilio->referencia != NULL ) {
      xmlFree(cfdi->Receptor->Domicilio->referencia);
    }
    xmlFree(cfdi->Receptor->Domicilio->pais);
    if ( cfdi->Receptor->Domicilio->estado != NULL ) {
      xmlFree(cfdi->Receptor->Domicilio->estado);
    }
    if ( cfdi->Receptor->Domicilio->municipio != NULL ) {
      xmlFree(cfdi->Receptor->Domicilio->municipio);
    }
    if ( cfdi->Receptor->Domicilio->localidad != NULL ) {
      xmlFree(cfdi->Receptor->Domicilio->localidad);
    }
    if ( cfdi->Receptor->Domicilio->colonia != NULL ) {
      xmlFree(cfdi->Receptor->Domicilio->colonia);
    }
    if ( cfdi->Receptor->Domicilio->noInterior != NULL ) {
      xmlFree(cfdi->Receptor->Domicilio->noInterior);
    }
    if ( cfdi->Receptor->Domicilio->noExterior != NULL ) {
      xmlFree(cfdi->Receptor->Domicilio->noExterior);
    }
    if ( cfdi->Receptor->Domicilio->calle != NULL ) {
      xmlFree(cfdi->Receptor->Domicilio->calle);
    }
    free(cfdi->Receptor->Domicilio);
    cfdi->Receptor->Domicilio = NULL;
  }
  if ( cfdi->Receptor->nombre != NULL ) {
    xmlFree(cfdi->Receptor->nombre);
  }
  xmlFree(cfdi->Receptor->rfc);
  free(cfdi->Receptor);
  cfdi->Receptor = NULL;

  /* Ahora el emisor */
  if ( cfdi->Emisor->DomicilioFiscal != NULL ) {
    if ( cfdi->Emisor->DomicilioFiscal->codigoPostal != NULL ) {
      xmlFree(cfdi->Emisor->DomicilioFiscal->codigoPostal);
    }
    if ( cfdi->Emisor->DomicilioFiscal->referencia != NULL ) {
      xmlFree(cfdi->Emisor->DomicilioFiscal->referencia);
    }
    xmlFree(cfdi->Emisor->DomicilioFiscal->pais);
    if ( cfdi->Emisor->DomicilioFiscal->estado != NULL ) {
      xmlFree(cfdi->Emisor->DomicilioFiscal->estado);
    }
    if ( cfdi->Emisor->DomicilioFiscal->municipio != NULL ) {
      xmlFree(cfdi->Emisor->DomicilioFiscal->municipio);
    }
    if ( cfdi->Emisor->DomicilioFiscal->localidad != NULL ) {
      xmlFree(cfdi->Emisor->DomicilioFiscal->localidad);
    }
    if ( cfdi->Emisor->DomicilioFiscal->colonia != NULL ) {
      xmlFree(cfdi->Emisor->DomicilioFiscal->colonia);
    }
    if ( cfdi->Emisor->DomicilioFiscal->noInterior != NULL ) {
      xmlFree(cfdi->Emisor->DomicilioFiscal->noInterior);
    }
    if ( cfdi->Emisor->DomicilioFiscal->noExterior != NULL ) {
      xmlFree(cfdi->Emisor->DomicilioFiscal->noExterior);
    }
    if ( cfdi->Emisor->DomicilioFiscal->calle != NULL ) {
      xmlFree(cfdi->Emisor->DomicilioFiscal->calle);
    }
    free(cfdi->Emisor->DomicilioFiscal);
    cfdi->Emisor->DomicilioFiscal = NULL;
  }

  /* ExpedidoEn */
  if ( cfdi->Emisor->ExpedidoEn != NULL ) {
    if ( cfdi->Emisor->ExpedidoEn->codigoPostal != NULL ) {
      xmlFree(cfdi->Emisor->ExpedidoEn->codigoPostal);
    }
    if ( cfdi->Emisor->ExpedidoEn->referencia != NULL ) {
      xmlFree(cfdi->Emisor->ExpedidoEn->referencia);
    }
    xmlFree(cfdi->Emisor->ExpedidoEn->pais);
    if ( cfdi->Emisor->ExpedidoEn->estado != NULL ) {
      xmlFree(cfdi->Emisor->ExpedidoEn->estado);
    }
    if ( cfdi->Emisor->ExpedidoEn->municipio != NULL ) {
      xmlFree(cfdi->Emisor->ExpedidoEn->municipio);
    }
    if ( cfdi->Emisor->ExpedidoEn->localidad != NULL ) {
      xmlFree(cfdi->Emisor->ExpedidoEn->localidad);
    }
    if ( cfdi->Emisor->ExpedidoEn->colonia != NULL ) {
      xmlFree(cfdi->Emisor->ExpedidoEn->colonia);
    }
    if ( cfdi->Emisor->ExpedidoEn->noInterior != NULL ) {
      xmlFree(cfdi->Emisor->ExpedidoEn->noInterior);
    }
    if ( cfdi->Emisor->ExpedidoEn->noExterior != NULL ) {
      xmlFree(cfdi->Emisor->ExpedidoEn->noExterior);
    }
    if ( cfdi->Emisor->ExpedidoEn->calle != NULL ) {
      xmlFree(cfdi->Emisor->ExpedidoEn->calle);
    }
    free(cfdi->Emisor->ExpedidoEn);
    cfdi->Emisor->ExpedidoEn = NULL;
  }
  if ( cfdi->Emisor->nombre != NULL ) {
    xmlFree(cfdi->Emisor->nombre);
  }
  xmlFree(cfdi->Emisor->rfc);
  xmlFree(cfdi->Emisor->RegimenFiscal);
  free(cfdi->Emisor);
  cfdi->Emisor = NULL;

  /* Ahora limpia todos los atributos del Comprobante */
  if ( cfdi->NumCtaPago != NULL ) {
    xmlFree(cfdi->NumCtaPago);
  }
  if ( cfdi->Moneda != NULL ) {
    xmlFree(cfdi->Moneda);
  }
  if ( cfdi->TipoCambio != NULL ) {
    xmlFree(cfdi->TipoCambio);
  }
  if ( cfdi->motivoDescuento != NULL ) {
    xmlFree(cfdi->motivoDescuento);
  }
  if ( cfdi->descuento != NULL ) {
    xmlFree(cfdi->descuento);
  }
  if ( cfdi->condicionesDePago != NULL ) {
    xmlFree(cfdi->condicionesDePago);
  }
  if ( cfdi->folio != NULL ) {
    xmlFree(cfdi->folio);
  }
  if ( cfdi->serie != NULL ) {
    xmlFree(cfdi->serie);
  }
  xmlFree(cfdi->LugarExpedicion);
  xmlFree(cfdi->metodoDePago);
  xmlFree(cfdi->tipoDeComprobante);
  xmlFree(cfdi->total);
  xmlFree(cfdi->subTotal);
  xmlFree(cfdi->certificado);
  xmlFree(cfdi->noCertificado);
  xmlFree(cfdi->formaDePago);
  xmlFree(cfdi->sello);
  xmlFree(cfdi->fecha);
  xmlFree(cfdi->version);

  /* Y finalmente */
  free(cfdi);
  cfdi = NULL;
}

/**
 *
 */
int
lee_complementos(const xmlNodePtr node, Comprobante_t *cfdi, const int verbose)
{
  int res = 0;
  Complemento_t *complemento = NULL;
  xmlNodePtr cur = NULL;

  if ( xmlStrEqual(node->name, (const xmlChar *)"Complemento") == 0 ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El nodo suministrado no es el correcto para leer los datos del Complemento del CFDi, el nodo proporcionado es \"%s\".\n", __FILE__, __LINE__, cur->name);
    }
    cfdi->Complemento = NULL;
    return 1;
  }

  if ( verbose ) {
    fprintf(stderr, "%s:%d Info. Leyendo el nodo \"%s\".\n", __FILE__, __LINE__, node->name);
  }

  complemento = (Complemento_t *)malloc(sizeof(Complemento_t));
  cfdi->Complemento = complemento;

  /* Ahora has un loop para identificar el complemento correspondiente
   * al Timbre Fiscal Digital */
  cur = node->children;
  while ( cur != NULL ) {

    if ( cur->type == XML_ELEMENT_NODE ) {

      if ( xmlStrEqual(cur->name, (const xmlChar *)"TimbreFiscalDigital") ) {
	/* Lee los datos del timbre fiscal */
	if ( verbose ) {
	  fprintf(stderr, "%s:%d Info. El nodo \"%s\" se encontro dentro del nodo \"Complemento\", se procedara a leerlo.\n", __FILE__, __LINE__, cur->name);
	}
	res = lee_timbre_fiscal(cur, cfdi, verbose);
	if ( res != 0 ) {
	    fprintf(stderr, "%s:%d Error. El nodo \"%s\" no pudo leerse adecuadamente.\n", __FILE__, __LINE__, cur->name);
	  if ( cfdi->Complemento->TimbreFiscalDigital != NULL ) {
	    free(cfdi->Complemento->TimbreFiscalDigital);
	    cfdi->Complemento->TimbreFiscalDigital = NULL;
	  }
	  res = 10 + res;
	}

      } else {
	printf("No hay implementacion para leer el nodo \"%s\".\n", cur->name);
      }

    }

    cur = cur->next;
  }
  return res;

}

/**
 *
 */
int
lee_timbre_fiscal(const xmlNodePtr node, Comprobante_t *cfdi, const int verbose)
{
  xmlNodePtr cur = NULL;
  TimbreFiscalDigital_t *Timbre = NULL;

  if ( verbose ) {
    fprintf(stderr, "%s:%d Info. Leyendo el nodo \"%s\".\n", __FILE__, __LINE__, node->name);
  }

  cur = node;
  if ( cur == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El presente nodo no contiene información del Timbre Fiscal Digital.\n", __FILE__, __LINE__);
      cfdi->Complemento->TimbreFiscalDigital = NULL;
      return 1;
    }
  }

  if ( xmlStrEqual(cur->name, (const xmlChar *)"TimbreFiscalDigital") == 0 ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No sera posible leer la información del Timbre Fiscal Digital ya que el nodo suministrado es: \"%s\".\n", __FILE__, __LINE__, cur->name);
    }
    cfdi->Complemento->TimbreFiscalDigital = NULL;
    return 2;
  }

  Timbre = (TimbreFiscalDigital_t *)malloc(sizeof(TimbreFiscalDigital_t));
  Timbre->version          = xmlGetProp(cur, (const xmlChar *)"version");
  Timbre->UUID             = xmlGetProp(cur, (const xmlChar *)"UUID");
  Timbre->noCertificadoSAT = xmlGetProp(cur, (const xmlChar *)"noCertificadoSAT");
  Timbre->FechaTimbrado    = xmlGetProp(cur, (const xmlChar *)"FechaTimbrado");
  Timbre->selloCFD         = xmlGetProp(cur, (const xmlChar *)"selloCFD");
  Timbre->selloSAT         = xmlGetProp(cur, (const xmlChar *)"selloSAT");

  /* Ahora asigna el timbre al CFDi */
  cfdi->Complemento->TimbreFiscalDigital = Timbre;

  return 0;
}

/**
 *
 */
int
lee_impuestos(const xmlNodePtr node, Comprobante_t *cfdi, const int verbose)
{
  xmlNodePtr cur = NULL;
  xmlNodePtr tmp1 = NULL;
  Impuestos_t *Impuestos = NULL;
  Retencion_list_t *retenciones = NULL;
  Retencion_list_t *ret_tmp = NULL;
  Retencion_list_t *ret_current = NULL;
  Traslado_list_t *traslados = NULL;
  Traslado_list_t *tras_tmp = NULL;
  Traslado_list_t *tras_current = NULL;

  /* Verifica que el nodo corresponda a los impuestos */
  if ( xmlStrEqual(node->name, (const xmlChar *)"Impuestos") == 0 ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El nodo suministrado no corresponde al nodo de los \"Impuestos\"; el nodo dado es: \"%s\".\n", __FILE__, __LINE__, node->name);
    }
    cfdi->Impuestos = NULL;
    return 1;
  }  

  Impuestos = (Impuestos_t *)malloc(sizeof(Impuestos_t));
  /* Ahora carga cada uno de los distintos tipos de impuesto */
  cur = node->children;
  while ( cur != NULL ) {

    if ( cur->type == XML_ELEMENT_NODE ) {
      if ( verbose ) {
	printf("\tImpuestos: subnode name: %s\n", cur->name);
      }

      if ( xmlStrEqual(cur->name, (const xmlChar *)"Traslados") ) {

	/* Tenemos que hacer otro ciclo para capturar todos los
	 * posibles impuestos trasladados */
	tmp1 = cur->children;
	while ( tmp1 != NULL ) {
	  if ( tmp1->type == XML_ELEMENT_NODE ) {
	    if ( verbose ) {
	      printf("\t\tTraslados: node name = %s\n", tmp1->name);
	    }

	    tras_tmp = (Traslado_list_t *)malloc(sizeof(Traslado_list_t));
	    tras_tmp->importe  = xmlGetProp(tmp1, (const xmlChar *)"importe");
	    tras_tmp->impuesto = xmlGetProp(tmp1, (const xmlChar *)"impuesto");
	    tras_tmp->tasa     = xmlGetProp(tmp1, (const xmlChar *)"tasa");

	    if ( traslados == NULL ) {
	      /* Esta es la primera entrada de la lista */
	      traslados = tras_tmp;
	      traslados->next = NULL;
	      tras_current = traslados;
	      traslados->size = 1;

	    } else {
	      /* Agrega otro impuesto trasladado a la lista */
	      while ( tras_current->next != NULL ) {
		tras_current = tras_current->next;
	      }

	      /* Agrega esta entrada al final de la lista */
	      tras_tmp->next = NULL;
	      tras_current->next = tras_tmp;
	      traslados->size++;
	    }

	  }
	  tmp1 = tmp1->next;
	}

      } else if ( xmlStrEqual(cur->name, (const xmlChar *)"Retenciones") ) {

	/* Tenemos que hacer otro ciclo para capturar todos los
	 * posibles impuestos retenidos */
	tmp1 = cur->children;
	while ( tmp1 != NULL ) {
	  if ( tmp1->type == XML_ELEMENT_NODE ) {
	    if ( verbose ) {
	      printf("\t\tRetenciones: node name = %s\n", tmp1->name);
	    }

	    ret_tmp = (Retencion_list_t *)malloc(sizeof(Retencion_list_t));
	    ret_tmp->importe  = xmlGetProp(tmp1, (const xmlChar *)"importe");
	    ret_tmp->impuesto = xmlGetProp(tmp1, (const xmlChar *)"impuesto");

	    if ( retenciones == NULL ) {
	      /* Esta es la primera entrada de la lista */
	      retenciones = ret_tmp;
	      retenciones->next = NULL;
	      ret_current = retenciones;
	      retenciones->size = 1;

	    } else {
	      /* Agrega otro impuesto a la lista */
	      while ( ret_current->next != NULL ) {
		ret_current = ret_current->next;
	      }

	      /* Agrega este impuesto al final de la lista */
	      ret_tmp->next = NULL;
	      ret_current->next = ret_tmp;
	      retenciones->size++;
	    }
	  }
	  tmp1 = tmp1->next;
	}

      } else {
	if ( verbose ) {
	  fprintf(stderr, "%s:%d Error. No es posible leer este tipo de nodo de impunestos \"%s\".\n", __FILE__, __LINE__, cur->name);
	}
      }


    }
    cur = cur->next;
  }

  /* Agrega estos impuestos */
  Impuestos->Traslados = traslados;
  /* Agregas estos impuestos */
  Impuestos->Retenciones = retenciones;

  /* Y finalmente agrega esta lista al CFDi */
  cfdi->Impuestos = Impuestos;

  return 0;
}


/**
 * Función auxiliar para leer el nodo Conceptos
 */
int
lee_conceptos(const xmlNodePtr node, Comprobante_t *cfdi, const int verbose)
{
  xmlNodePtr cur = NULL;
  xmlNodePtr cur1 = NULL;
  Concepto_list_t *conceptos = NULL;
  Concepto_list_t *current = NULL;
  Concepto_list_t *tmp = NULL;
  CuentaPredial_t *predial = NULL;
  InformacionAduanera_list_t *info_a = NULL;
  InformacionAduanera_list_t *info_a_tmp = NULL;
  InformacionAduanera_list_t *info_a_current = NULL;
  Parte_list_t *parte = NULL;

  /* Verifica que estemos leyendo el nodo correspondiente a los Conceptos */
  if ( xmlStrEqual(node->name, (const xmlChar *)"Conceptos") == 0 ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El nodo a leer debería ser el de los \"Conceptos\", sin embargo el nodo dado tiene por nombre \"%s\".\n", __FILE__, __LINE__, cur->name);
    }
    cfdi->Conceptos = NULL;
    return 1;
  }

  /* Ahora comienza el ciclo de lectura de todos los conceptos en el nodo */
  cur = node->children;
  while ( cur != NULL ) {

    if ( cur->type == XML_ELEMENT_NODE ) {

      tmp = (Concepto_list_t *)malloc(sizeof(Concepto_list_t));
      tmp->cantidad         = xmlGetProp(cur, (const xmlChar *)"cantidad");
      tmp->unidad           = xmlGetProp(cur, (const xmlChar *)"unidad");
      tmp->descripcion      = xmlGetProp(cur, (const xmlChar *)"descripcion");
      tmp->valorUnitario    = xmlGetProp(cur, (const xmlChar *)"valorUnitario");
      tmp->importe          = xmlGetProp(cur, (const xmlChar *)"importe");
      tmp->noIdentificacion = xmlGetProp(cur, (const xmlChar *)"noIdentificacion");

      /* Ahora, existe la posibilidad de que este nodo contenga mas
       * nodos hijo con InformacionAduanera, CuentaPredial, Parte */
      cur1 = cur->children;
      while ( cur1 != NULL ) {
	if ( cur1->type == XML_ELEMENT_NODE ) {

	  if ( xmlStrEqual(cur1->name, (const xmlChar *)"InformacionAduanera")){
	    /* Puede haber mas de un elemento con información aduanera para
	     * este concepto, segun el estandar, de modo que lo mejor es
	     * hacer una lista para cada concepto */
	    info_a_tmp = (InformacionAduanera_list_t *)malloc(sizeof(InformacionAduanera_list_t));
	    info_a_tmp->numero = xmlGetProp(cur1, (const xmlChar *)"numero");
	    info_a_tmp->fecha  = xmlGetProp(cur1, (const xmlChar *)"fecha");
	    info_a_tmp->aduana = xmlGetProp(cur1, (const xmlChar *)"aduana");

	    if ( info_a == NULL ) {
	      /* Esta es la primera entrada en la lista */
	      info_a = info_a_tmp;
	      info_a->next = NULL;
	      info_a_current = info_a;
	      info_a->size = 1;

	    } else {
	      /* La lista ya existe, agregale una entrada mas al final */
	      while ( info_a_current->next != NULL ) {
		info_a_current = info_a_current->next;
	      }

	      info_a_tmp->next = NULL;
	      info_a_current->next = info_a_tmp;
	      info_a->size++;
	    }

	  }

	  if ( xmlStrEqual(cur1->name, (const xmlChar *)"CuentaPredial") ) {
	    predial = (CuentaPredial_t *)malloc(sizeof(CuentaPredial_t));
	    predial->numero = xmlGetProp(cur1, (const xmlChar *)"numero");
	  }

	  if ( xmlStrEqual(cur1->name, (const xmlChar *)"Parte") ) {
	    printf("Nodo: \"%s\" por ser implementado.\n", cur->name);
	  }

	}
	cur1 = cur1->next;
      }
      /* Agrega la informacion de alguno de los nodos hijos */
      tmp->CuentaPredial = predial;
      tmp->Parte = parte;
      tmp->InformacionAduanera = info_a;
      /* Reinicia algunas variables */
      info_a = NULL;
      info_a_tmp = NULL;
      info_a_current = NULL;
      predial = NULL;

      if ( conceptos == NULL ) {
	/* Esta es la primera entrada de lista */
	conceptos = tmp;
	conceptos->next = NULL;
	current = conceptos;
	conceptos->size = 1;

      } else {
	/* Agrega otra entrada a la lista */
	while ( current->next != NULL ) {
	  current = current->next;
	}

	/* Agrega esta entrada al final de la lista */
	tmp->next = NULL;
	current->next = tmp;
	conceptos->size++;

      }
    }

    cur = cur->next;
  }

  /* Y finalmete agrega la lista de conceptos al CFDi */
  cfdi->Conceptos = conceptos;

  return 0;
}

/**
 *
 */
int
lee_datos_emisor(const xmlNodePtr node, Comprobante_t *cfdi, const int verbose)
{
  xmlNodePtr cur = NULL;
  Emisor_t *emisor = NULL;
  Domicilio_t *fiscal = NULL;
  Domicilio_t *expedido = NULL;

  /* Verifica que en realidad este nodo corresponda al emisor */
  if ( xmlStrEqual(node->name, (const xmlChar *)"Emisor") == 0 ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El nodo proporcionado no corresponde al nodo del Emisor, el nodo proporcionado es \"%s\".\n", __FILE__, __LINE__, cur->name);
    }
    return 1;
  }

  /* Extrae los atributos principales del emisor */
  cur = node;
  emisor = (Emisor_t *)malloc(sizeof(Emisor_t));
  emisor->rfc = xmlGetProp(cur, (const xmlChar *)"rfc");

  /* En caso de que no exista el atributo correspondiente al nombre, se
   * asignara NULL */
  emisor->nombre = xmlGetProp(cur, (const xmlChar *)"nombre");

  /* Ahora tenemos que leer a lo mas 3 nodos mas, DomicilioFiscal,
   * ExpedidoEn y RegimenFiscal */
  cur = cur->children;
  while ( cur != NULL ) {
    if ( cur->type == XML_ELEMENT_NODE ) {
      /* Verifica que tipo de info necesitamos guardar */
      if ( xmlStrEqual(cur->name, (const xmlChar *)"DomicilioFiscal") ) {
	if ( verbose ) {
	  printf("%s:%d Info. Nodo actual: %s\n", __FILE__, __LINE__, cur->name);
	}

	fiscal = (Domicilio_t *)malloc(sizeof(Domicilio_t));
	fiscal->calle        = xmlGetProp(cur, (const xmlChar *)"calle");
	fiscal->noExterior   = xmlGetProp(cur, (const xmlChar *)"noExterior");
	fiscal->noInterior   = xmlGetProp(cur, (const xmlChar *)"noInterior");
	fiscal->colonia      = xmlGetProp(cur, (const xmlChar *)"colonia");
	fiscal->localidad    = xmlGetProp(cur, (const xmlChar *)"localidad");
	fiscal->municipio    = xmlGetProp(cur, (const xmlChar *)"municipio");
	fiscal->estado       = xmlGetProp(cur, (const xmlChar *)"estado");
	fiscal->pais         = xmlGetProp(cur, (const xmlChar *)"pais");
	fiscal->referencia   = xmlGetProp(cur, (const xmlChar *)"referencia");
	fiscal->codigoPostal = xmlGetProp(cur, (const xmlChar *)"codigoPostal");

      } else if ( xmlStrEqual(cur->name, (const xmlChar *)"ExpedidoEn") ) {
	if ( verbose ) {
	  printf("%s:%d Info. Nodo actual: %s\n", __FILE__, __LINE__, cur->name);
	}

	expedido = (Domicilio_t *)malloc(sizeof(Domicilio_t));
	expedido->calle        = xmlGetProp(cur, (const xmlChar *)"calle");
	expedido->noExterior   = xmlGetProp(cur, (const xmlChar *)"noExterior");
	expedido->noInterior   = xmlGetProp(cur, (const xmlChar *)"noInterior");
	expedido->colonia      = xmlGetProp(cur, (const xmlChar *)"colonia");
	expedido->localidad    = xmlGetProp(cur, (const xmlChar *)"localidad");
	expedido->municipio    = xmlGetProp(cur, (const xmlChar *)"municipio");
	expedido->estado       = xmlGetProp(cur, (const xmlChar *)"estado");
	expedido->pais         = xmlGetProp(cur, (const xmlChar *)"pais");
	expedido->referencia   = xmlGetProp(cur, (const xmlChar *)"referencia");
	expedido->codigoPostal = xmlGetProp(cur, (const xmlChar *)"codigoPostal");

      } else if ( xmlStrEqual(cur->name, (const xmlChar *)"RegimenFiscal") ) {
	emisor->RegimenFiscal = xmlGetProp(cur, (const xmlChar *)"Regimen");
	if ( verbose ) {
	  printf("%s:%d Info. Nodo actual: %s, valor = %s\n", __FILE__, __LINE__, cur->name, emisor->RegimenFiscal);
	}

      }
    }
    cur = cur->next;
  }

  emisor->ExpedidoEn = expedido;
  emisor->DomicilioFiscal = fiscal;

  cfdi->Emisor = emisor;
  return 0;
}

/**
 *
 */
int
lee_datos_receptor(const xmlNodePtr node, Comprobante_t *cfdi, const int verbose)
{
  xmlNodePtr cur = NULL;
  Receptor_t *receptor = NULL;
  Domicilio_t *domicilio = NULL;

  /* Verifica que en realidad este nodo corresponda al emisor */
  if ( xmlStrEqual(node->name, (const xmlChar *)"Receptor") == 0 ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. El nodo proporcionado no corresponde al nodo del Receptor, el nodo proporcionado es \"%s\".\n", __FILE__, __LINE__, cur->name);
    }
    return 1;
  }

  /* Extrae los atributos principales del receptor */
  cur = node;
  receptor = (Receptor_t *)malloc(sizeof(Receptor_t));
  receptor->rfc = xmlGetProp(cur, (const xmlChar *)"rfc");

  /* En caso de que no exista el atributo correspondiente al nombre, se
   * asignara NULL */
  receptor->nombre = xmlGetProp(cur, (const xmlChar *)"nombre");

  /* Ahora necesitamos el domicilio de la persona. Para esto necesitamos
   * verificar que el halla información que leer */
  cur = cur->children;

  while ( cur != NULL ) {

    if ( cur->type == XML_ELEMENT_NODE ) {

      if ( xmlStrEqual(cur->name, (const xmlChar *)"Domicilio") ) {

	domicilio = (Domicilio_t *)malloc(sizeof(Domicilio_t));

	domicilio->calle        = xmlGetProp(cur, (const xmlChar *)"calle");
	domicilio->noExterior   = xmlGetProp(cur, (const xmlChar *)"noExterior");
	domicilio->noInterior   = xmlGetProp(cur, (const xmlChar *)"noInterior");
	domicilio->colonia      = xmlGetProp(cur, (const xmlChar *)"colonia");
	domicilio->localidad    = xmlGetProp(cur, (const xmlChar *)"localidad");
	domicilio->municipio    = xmlGetProp(cur, (const xmlChar *)"municipio");
	domicilio->estado       = xmlGetProp(cur, (const xmlChar *)"estado");
	domicilio->pais         = xmlGetProp(cur, (const xmlChar *)"pais");
	domicilio->referencia   = xmlGetProp(cur, (const xmlChar *)"referencia");
	domicilio->codigoPostal = xmlGetProp(cur, (const xmlChar *)"codigoPostal");
      }
    }

    cur = cur->next;
  }

  if ( domicilio == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Atención. No existe el nodo de Domicilio del Receptor.\n", __FILE__, __LINE__);
    }
  }


  receptor->Domicilio = domicilio;

  cfdi->Receptor = receptor;

  return 0;
}
