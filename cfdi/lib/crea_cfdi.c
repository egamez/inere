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
#ifndef INERE_CFDI_CREA_CFDI_H_
#include "inere/cfdi/crea_cfdi.h"
#endif
#ifndef INERE_CFDI_LEE_CERTIFICADO_H_
#include "inere/cfdi/lee_certificado.h"
#endif
#ifndef INERE_CFDI_GENERA_CADENA_ORIGINAL_H_
#include "inere/cfdi/genera_cadena_original.h"
#endif
#ifndef INERE_CFDI_SELLO_H_
#include "inere/cfdi/sello.h"
#endif

#include <time.h>
#include <stdio.h>
#include <monetary.h>
#include <string.h>

/* algunas constantes */
static const xmlChar *version = (const xmlChar *)"3.2";
static const xmlChar *cfdi_url = (const xmlChar *)"http://www.sat.gob.mx/cfd/3";
static const xmlChar *xsi_url = (const xmlChar *)"http://www.w3.org/2001/XMLSchema-instance";
static const xmlChar *cfdi_schemaLocation = (const xmlChar *)"http://www.sat.gob.mx/cfd/3 http://www.sat.gob.mx/sitio_internet/cfd/3/cfdv32.xsd";

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
				     const char *archivo_certificado,
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
 * Esta funcion creara la estructura basica Comprobante_t
 *
 */
Comprobante_t *
crea_comprobante(unsigned char *version)
{
  Comprobante_t *cfdi = NULL;

  /* Verifica que la version dada corresponda a la versión soportada */
  if ( strcmp((char *)version, "3.2") == 0 ) {
    cfdi = (Comprobante_t *)malloc(sizeof(Comprobante_t));
    if ( cfdi == NULL ) {
      fprintf(stderr, "%s:%d Error al momento de reservar memoria para el comprobante\n", __FILE__, __LINE__);
    } else {

      cfdi->version = version;

      /* Ahora inicializa todas los demas componentes */
      cfdi->serie                = NULL;
      cfdi->folio                = NULL;
      cfdi->fecha                = NULL;
      cfdi->sello                = NULL;
      cfdi->formaDePago          = NULL;
      cfdi->noCertificado        = NULL;
      cfdi->certificado          = NULL;
      cfdi->condicionesDePago    = NULL;
      cfdi->subTotal             = NULL;
      cfdi->descuento            = NULL;
      cfdi->motivoDescuento      = NULL;
      cfdi->TipoCambio           = NULL;
      cfdi->Moneda               = NULL;
      cfdi->total                = NULL;
      cfdi->tipoDeComprobante    = NULL;
      cfdi->metodoDePago         = NULL;
      cfdi->LugarExpedicion      = NULL;
      cfdi->NumCtaPago           = NULL;
      cfdi->FolioFiscalOrig      = NULL;
      cfdi->SerieFolioFiscalOrig = NULL;
      cfdi->FechaFolioFiscalOrig = NULL;
      cfdi->MontoFolioFiscalOrig = NULL;
      cfdi->Emisor               = NULL;
      cfdi->Receptor             = NULL;
      cfdi->Conceptos            = NULL;
      cfdi->Impuestos            = NULL;
      cfdi->Complemento          = NULL;
      cfdi->Addenda              = NULL;

    }

  } else {
    fprintf(stderr, "%s:%d Error. Versión \"%s\" de CFDI no soportada.\n", __FILE__, __LINE__, version);
  }

  return cfdi;
}

/**
 *
 */
int
asigna_serie(Comprobante_t *cfdi, unsigned char *serie)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( serie == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else if ( strlen((char *)serie) > 25 ) {
    ret = 3;
    fprintf(stderr, "%s:%d Error. Se ha excedido la longitud máxima para asignar la serie del comprobante.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->serie = serie;
  }
  return ret;
}


/**
 *
 */
int
asigna_folio(Comprobante_t *cfdi, unsigned char *folio)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( folio == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else if ( strlen((char *)folio) > 20 ) {
    ret = 3;
    fprintf(stderr, "%s:%d Error. Se ha excedido la longitud máxima para asignar el folio del comprobante.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->folio = folio;
  }
  return ret;
}


/**
 * If 'fecha' is NUL, the program will call 
 */
int
asigna_fecha(Comprobante_t *cfdi, unsigned char *fecha)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( strlen((char *)fecha) > 19 ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Se ha excedido la longitud máxima para asignar la fecha del comprobante.\n", __FILE__, __LINE__);
  } else if ( fecha == NULL ) {
    /* Asigna una fecha por default */
    cfdi->fecha = obten_fecha_ISO8601_alloc(0);
  } else {
    /* Copia el puntero al argumento */
    cfdi->fecha = fecha;
  }
  return ret;
}


/**
 *
 */
int
asigna_sello(Comprobante_t *cfdi, unsigned char *sello)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( sello == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->sello = sello;
  }
  return ret;
}


/**
 *
 */
int
asigna_formaDePago(Comprobante_t *cfdi, unsigned char *formaDePago)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( formaDePago == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->formaDePago = formaDePago;
  }
  return ret;
}


/**
 *
 */
int
asigna_noCertificado(Comprobante_t *cfdi, unsigned char *noCertificado)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( noCertificado == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else if ( strlen((char *)noCertificado) > 20 ) {
    ret = 3;
    fprintf(stderr, "%s:%d Error. Se ha excedido la longitud máxima para expresar el número de serie del certificado de sello digital que ampara al comprobante.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->noCertificado = noCertificado;
  }
  return ret;
}


/**
 *
 */
int
asigna_certificado(Comprobante_t *cfdi, unsigned char *certificado)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( certificado == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->certificado = certificado;
  }
  return ret;
}


/**
 *
 */
int
asigna_condicionesDePago(Comprobante_t *cfdi, unsigned char *condicionesDePago)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( condicionesDePago == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->condicionesDePago = condicionesDePago;
  }
  return ret;
}


/**
 *
 */
int
asigna_subTotal(Comprobante_t *cfdi, unsigned char *subTotal)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( subTotal == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->subTotal = subTotal;
  }
  return ret;
}


/**
 *
 */
int
asigna_descuento(Comprobante_t *cfdi, unsigned char *descuento)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( descuento == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->descuento = descuento;
  }
  return ret;
}


/**
 *
 */
int
asigna_motivoDescuento(Comprobante_t *cfdi, unsigned char *motivoDescuento)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( motivoDescuento == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->motivoDescuento = motivoDescuento;
  }
  return ret;
}


/**
 *
 */
int
asigna_TipoCambio(Comprobante_t *cfdi, unsigned char *TipoCambio)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( TipoCambio == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->TipoCambio = TipoCambio;
  }
  return ret;
}


/**
 *
 */
int
asigna_Moneda(Comprobante_t *cfdi, unsigned char *Moneda)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( Moneda == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->Moneda = Moneda;
  }
  return ret;
}


/**
 *
 */
int
asigna_total(Comprobante_t *cfdi, unsigned char *total)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( total == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->total = total;
  }
  return ret;
}


/**
 *
 */
int
asigna_tipoDeComprobante(Comprobante_t *cfdi, unsigned char *tipoDeComprobante)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( tipoDeComprobante == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->tipoDeComprobante = tipoDeComprobante;
  }
  return ret;
}


/**
 *
 */
int
asigna_metodoDePago(Comprobante_t *cfdi, unsigned char *metodoDePago)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( metodoDePago == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->metodoDePago = metodoDePago;
  }
  return ret;
}


/**
 *
 */
int
asigna_LugarExpedicion(Comprobante_t *cfdi, unsigned char *LugarExpedicion)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( LugarExpedicion == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->LugarExpedicion = LugarExpedicion;
  }
  return ret;
}


/**
 *
 */
int
asigna_NumCtaPago(Comprobante_t *cfdi, unsigned char *NumCtaPago)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( NumCtaPago == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->NumCtaPago = NumCtaPago;
  }
  return ret;
}


/**
 *
 */
int
asigna_FolioFiscalOrig(Comprobante_t *cfdi, unsigned char *FolioFiscalOrig)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( FolioFiscalOrig == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->FolioFiscalOrig = FolioFiscalOrig;
  }
  return ret;
}


/**
 *
 */
int
asigna_SerieFolioFiscalOrig(Comprobante_t *cfdi, unsigned char *SerieFolioFiscalOrig)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( SerieFolioFiscalOrig == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->SerieFolioFiscalOrig = SerieFolioFiscalOrig;
  }
  return ret;
}


/**
 *
 */
int
asigna_FechaFolioFiscalOrig(Comprobante_t *cfdi, unsigned char *FechaFolioFiscalOrig)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( FechaFolioFiscalOrig == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->FechaFolioFiscalOrig = FechaFolioFiscalOrig;
  }
  return ret;
}


/**
 *
 */
int
asigna_MontoFolioFiscalOrig(Comprobante_t *cfdi, unsigned char *MontoFolioFiscalOrig)
{
  int ret = 0;
  if ( cfdi == NULL ) {
    ret = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else if ( MontoFolioFiscalOrig == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Argumento nulo.\n", __FILE__, __LINE__);
  } else {
    /* Copia el puntero al argumento */
    cfdi->MontoFolioFiscalOrig = MontoFolioFiscalOrig;
  }
  return ret;
}

/**
 *
 */
int
agrega_Emisor(Comprobante_t *cfdi, unsigned char *rfc, unsigned char *RegimenFiscal, unsigned char *nombre)
{
  int res = 0;

  if ( cfdi == NULL ) {
    res = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else {

    if ( rfc == NULL ) {
      res = 2;
      fprintf(stderr, "%s:%d Error. Es necesario indicar la clave del R.F.C. del emisor.\n", __FILE__, __LINE__);

    } else {

      cfdi->Emisor = (Emisor_t *)malloc(sizeof(Emisor_t));
      cfdi->Emisor->rfc             = rfc;
      cfdi->Emisor->RegimenFiscal   = RegimenFiscal;
      cfdi->Emisor->nombre          = nombre;
      cfdi->Emisor->DomicilioFiscal = NULL;
      cfdi->Emisor->ExpedidoEn      = NULL;

    }

  }

  return res;
}

/**
 *
 */
int
agrega_Emisor_DomicilioFiscal(Comprobante_t *cfdi,
			      unsigned char *calle,
			      unsigned char *noExterior,
			      unsigned char *noInterior,
			      unsigned char *colonia,
			      unsigned char *localidad,
			      unsigned char *municipio,
			      unsigned char *estado,
			      unsigned char *pais,
			      unsigned char *referencia,
			      unsigned char *codigoPostal)
{
  int res = 0;

  if ( cfdi == NULL ) {
    res = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else {

    if ( cfdi->Emisor == NULL ) {
      fprintf(stderr, "%s:%d Error. Es necesario primero definir al emisor.\n", __FILE__, __LINE__);
      return 10;
    }

    cfdi->Emisor->DomicilioFiscal = (Domicilio_t *)malloc(sizeof(Domicilio_t));
    cfdi->Emisor->DomicilioFiscal->calle        = NULL;
    cfdi->Emisor->DomicilioFiscal->noExterior   = NULL;
    cfdi->Emisor->DomicilioFiscal->noInterior   = NULL;
    cfdi->Emisor->DomicilioFiscal->colonia      = NULL;
    cfdi->Emisor->DomicilioFiscal->localidad    = NULL;
    cfdi->Emisor->DomicilioFiscal->referencia   = NULL;
    cfdi->Emisor->DomicilioFiscal->municipio    = NULL;
    cfdi->Emisor->DomicilioFiscal->estado       = NULL;
    cfdi->Emisor->DomicilioFiscal->pais         = NULL;
    cfdi->Emisor->DomicilioFiscal->codigoPostal = NULL;


    /* Primero los parametros obligatorios */
    if ( calle == NULL ) {
      res = 2;
      fprintf(stderr, "%s:%d Error. El atributo 'calle' es de caracter obligatorio para el domicilio fiscal del emisor.\n", __FILE__, __LINE__);
    } else {
      cfdi->Emisor->DomicilioFiscal->calle = calle;
    }

    if ( municipio == NULL ) {
      res = 3;
      fprintf(stderr, "%s:%d Error. El atributo 'municipio' es de caracter obligatorio para el domicilio fiscal del emisor.\n", __FILE__, __LINE__);
    } else {
      cfdi->Emisor->DomicilioFiscal->municipio = municipio;
    }

    if ( estado == NULL ) {
      res = 4;
      fprintf(stderr, "%s:%d Error. El atributo 'estado' es de caracter obligatorio para el domicilio fiscal del emisor.\n", __FILE__, __LINE__);
    } else {
      cfdi->Emisor->DomicilioFiscal->estado = estado;
    }

    if ( pais == NULL ) {
      res = 5;
      fprintf(stderr, "%s:%d Error. El atributo 'pais' es de caracter obligatorio para el domicilio fiscal del emisor.\n", __FILE__, __LINE__);
    } else {
      cfdi->Emisor->DomicilioFiscal->pais = pais;
    }

    if ( codigoPostal == NULL ) {
      res = 6;
      fprintf(stderr, "%s:%d Error. El atributo 'codigoPostal' es de caracter obligatorio para el domicilio fiscal del emisor.\n", __FILE__, __LINE__);
    } else {
      cfdi->Emisor->DomicilioFiscal->codigoPostal = codigoPostal;
    }

    /* Y ahora los parametros opcionales */
    cfdi->Emisor->DomicilioFiscal->noExterior = noExterior;
    cfdi->Emisor->DomicilioFiscal->noInterior = noInterior;
    cfdi->Emisor->DomicilioFiscal->colonia    = colonia;
    cfdi->Emisor->DomicilioFiscal->localidad  = localidad;
    cfdi->Emisor->DomicilioFiscal->referencia = referencia;

  }
  return res;
}

/**
 *
 */
int
agrega_Emisor_ExpedidoEn(Comprobante_t *cfdi,
			 unsigned char *calle,
			 unsigned char *noExterior,
			 unsigned char *noInterior,
			 unsigned char *colonia,
			 unsigned char *localidad,
			 unsigned char *municipio,
			 unsigned char *estado,
			 unsigned char *pais,
			 unsigned char *referencia,
			 unsigned char *codigoPostal)
{
  int res = 0;

  if ( cfdi == NULL ) {
    res = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else {

    if ( cfdi->Emisor == NULL ) {
      fprintf(stderr, "%s:%d Error. Es necesario definir primero al emisor.\n", __FILE__, __LINE__);
      return 10;
    }

    cfdi->Emisor->ExpedidoEn = (Domicilio_t *)malloc(sizeof(Domicilio_t));
    cfdi->Emisor->ExpedidoEn->calle        = NULL;
    cfdi->Emisor->ExpedidoEn->noExterior   = NULL;
    cfdi->Emisor->ExpedidoEn->noInterior   = NULL;
    cfdi->Emisor->ExpedidoEn->colonia      = NULL;
    cfdi->Emisor->ExpedidoEn->localidad    = NULL;
    cfdi->Emisor->ExpedidoEn->referencia   = NULL;
    cfdi->Emisor->ExpedidoEn->municipio    = NULL;
    cfdi->Emisor->ExpedidoEn->estado       = NULL;
    cfdi->Emisor->ExpedidoEn->pais         = NULL;
    cfdi->Emisor->ExpedidoEn->codigoPostal = NULL;


    /* Los parametros obligatorios */
    if ( pais == NULL ) {
      res = 2;
      fprintf(stderr, "%s:%d Error. El atributo 'pais' es de caracter obligatorio para expresar en donde fue expedido el CFDI.\n", __FILE__, __LINE__);
    } else {
      cfdi->Emisor->ExpedidoEn->pais = pais;
    }

    /* Ahora los parametros opcionales */
    cfdi->Emisor->ExpedidoEn->calle        = calle;
    cfdi->Emisor->ExpedidoEn->noExterior   = noExterior;
    cfdi->Emisor->ExpedidoEn->noInterior   = noInterior;
    cfdi->Emisor->ExpedidoEn->colonia      = colonia;
    cfdi->Emisor->ExpedidoEn->localidad    = localidad;
    cfdi->Emisor->ExpedidoEn->referencia   = referencia;
    cfdi->Emisor->ExpedidoEn->municipio    = municipio;
    cfdi->Emisor->ExpedidoEn->estado       = estado;
    cfdi->Emisor->ExpedidoEn->codigoPostal = codigoPostal;

  }
  return res;
}

/**
 *
 */
int
agrega_Receptor(Comprobante_t *cfdi,
		unsigned char *rfc,
		unsigned char *nombre)
{
  int res = 0;

  if ( cfdi == NULL ) {
    res = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else {

    if ( rfc == NULL ) {
      res = 2;
      fprintf(stderr, "%s:%d Error. Es necesario indicar la clave del R.F.C. del receptor del cfdi.\n", __FILE__, __LINE__);

    } else {

      cfdi->Receptor = (Receptor_t *)malloc(sizeof(Receptor_t));
      cfdi->Receptor->rfc       = rfc;
      cfdi->Receptor->nombre    = nombre;
      cfdi->Receptor->Domicilio = NULL;
    }

  }
  return res;
}

/**
 *
 */
int
agrega_Receptor_Domicilio(Comprobante_t *cfdi,
			  unsigned char *calle,
			  unsigned char *noExterior,
			  unsigned char *noInterior,
			  unsigned char *colonia,
			  unsigned char *localidad,
			  unsigned char *municipio,
			  unsigned char *estado,
			  unsigned char *pais,
			  unsigned char *referencia,
			  unsigned char *codigoPostal)
{
  int res = 0;

  if ( cfdi == NULL ) {
    res = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else {

    if ( cfdi->Receptor == NULL ) {
      fprintf(stderr, "%s:%d Error. Es necesario definir primero al receptor.\n", __FILE__, __LINE__);
      return 10;
    }

    cfdi->Receptor->Domicilio = (Domicilio_t *)malloc(sizeof(Domicilio_t));
    cfdi->Receptor->Domicilio->calle        = NULL;
    cfdi->Receptor->Domicilio->noExterior   = NULL;
    cfdi->Receptor->Domicilio->noInterior   = NULL;
    cfdi->Receptor->Domicilio->colonia      = NULL;
    cfdi->Receptor->Domicilio->localidad    = NULL;
    cfdi->Receptor->Domicilio->referencia   = NULL;
    cfdi->Receptor->Domicilio->municipio    = NULL;
    cfdi->Receptor->Domicilio->estado       = NULL;
    cfdi->Receptor->Domicilio->pais         = NULL;
    cfdi->Receptor->Domicilio->codigoPostal = NULL;


    if ( pais == NULL ) {
      res = 2;
      fprintf(stderr, "%s:%d Error. Es necesario expresar el atributo 'pais' para el domicilio del emisor.\n", __FILE__, __LINE__);
    } else {

      cfdi->Receptor->Domicilio->calle        = calle;
      cfdi->Receptor->Domicilio->noExterior   = noExterior;
      cfdi->Receptor->Domicilio->noInterior   = noInterior;
      cfdi->Receptor->Domicilio->colonia      = colonia;
      cfdi->Receptor->Domicilio->localidad    = localidad;
      cfdi->Receptor->Domicilio->referencia   = referencia;
      cfdi->Receptor->Domicilio->municipio    = municipio;
      cfdi->Receptor->Domicilio->estado       = estado;
      cfdi->Receptor->Domicilio->codigoPostal = codigoPostal;
    }

  }
  return res;
}

/**
 *
 */
int
agrega_Concepto(Comprobante_t *cfdi,
		unsigned char *cantidad,
		unsigned char *noIdentificacion,
		unsigned char *unidad,
		unsigned char *descripcion,
		unsigned char *valorUnitario,
		unsigned char *importe)
{
  int res = 0;
  Concepto_list_t *concepto = NULL;
  Concepto_list_t *current = NULL;
  Concepto_list_t *tmp = NULL;
  CuentaPredial_t *predial = NULL;
  InformacionAduanera_list_t *info_a = NULL;
  InformacionAduanera_list_t *info_a_tmp = NULL;
  InformacionAduanera_list_t *info_a_current = NULL;
  Parte_list_t *parte = NULL;

  if ( cfdi == NULL ) {
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
    return 1;
  }

  tmp = (Concepto_list_t *)malloc(sizeof(Concepto_list_t));
  if ( tmp == NULL ) {
      fprintf(stderr, "%s:%d Error al momento de reservar memoria para el concepto\n", __FILE__, __LINE__);
    return 2;
  }

  if ( cantidad == NULL ) {
    free(tmp);
    fprintf(stderr, "%s:%d Error. Es requisito indispensable expresar la cantidad para este concepto.\n", __FILE__, __LINE__);
    return 3;
  } else {
    tmp->cantidad = cantidad;
  }

  /* Este campo es opcional */
  tmp->noIdentificacion = noIdentificacion;

  if ( unidad == NULL ) {
    free(tmp);
    fprintf(stderr, "%s:%d Error. Es requisito indispensable expresar la unidad referente a este concepto.\n", __FILE__, __LINE__);
    return 4;
  } else {
    tmp->unidad = unidad;
  }

  if ( descripcion == NULL ) {
    free(tmp);
    fprintf(stderr, "%s:%d Error. Es requisito indispensable expresar la descripcion de este concepto.\n", __FILE__, __LINE__);
    return 5;
  } else {
    tmp->descripcion = descripcion;
  }

  if ( valorUnitario == NULL ) {
    free(tmp);
    fprintf(stderr, "%s:%d Error. Es requisito indispensable expresar el valor unitario de este concepto.\n", __FILE__, __LINE__);
    return 6;
  } else {
    tmp->valorUnitario = valorUnitario;
  }

  if ( importe == NULL ) {
    free(tmp);
    fprintf(stderr, "%s:%d Error. Es requisito indispensable expresar el importe para este concepto.\n", __FILE__, __LINE__);
    return 3;
  } else {
    tmp->importe = importe;
  }

  /* El sentinel */
  tmp->next = NULL;

  /* Ahora debemos de salvar este concepto a la lista de conceptos del CFDI */
  if ( cfdi->Conceptos == NULL ) {
    /* Esta es la primera entrada */
    cfdi->Conceptos = tmp;
    cfdi->Conceptos->size = 1;

  } else {
    /* Agrega una entrada */
    current = cfdi->Conceptos;
    while ( current->next != NULL ) {
      current = current->next;
    }

    current->next = tmp;
    current->size++;
  }

  return res;
}


/**
 * Genera comprobante y regresa el cfdi
 */
xmlChar *
genera_comprobante_alloc(Comprobante_t *cfdi)
{
  xmlDocPtr doc = NULL;
  xmlNodePtr Comprobante = NULL;
  xmlNodePtr Emisor = NULL;
  xmlNodePtr DomicilioFiscal = NULL;
  xmlNodePtr ExpedidoEn = NULL;
  xmlNodePtr Receptor = NULL;
  xmlNodePtr Domicilio = NULL;
  xmlNodePtr Conceptos = NULL;
  xmlNodePtr Concepto = NULL;
  xmlNsPtr cfdi_ns = NULL;
  Concepto_list_t *conceptos = NULL;
  xmlChar *comprobante = NULL;
  int len = 0;

  doc = xmlNewDoc((const xmlChar *)"1.0");
  if ( doc == NULL ) {
    fprintf(stderr, "%s:%d Error. No fue posible crear la estructura del documento.\n", __FILE__, __LINE__);
    return NULL;
  }

  /* Crea el namespace raiz */
  cfdi_ns = xmlNewNs(Comprobante, cfdi_url, (const xmlChar *)"cfdi");
  if ( cfdi_ns == NULL ) {
    fprintf(stderr, "%s:%d Error. No fue posible crear el namespace primario.\
n", __FILE__, __LINE__);
    xmlFreeDoc(doc);
    return NULL;
  }

  /* Crea el elemento raiz */
  Comprobante = xmlNewNode(cfdi_ns, (const xmlChar *)"Comprobante");
  if ( Comprobante == NULL ) {
    fprintf(stderr, "%s:%d Error. No fue posible crear el nodo raiz, correspondiente al comprobante.\
n", __FILE__, __LINE__);
    xmlFreeDoc(doc);
    return NULL;
  }

  /* Crea el namespace xsi. pero para que el CFDi se vea bonito
   * no declares este namespace de la manera usual (via xmlNewNs)
   * mejor solo agregalo como un atributo cualquiera */
  xmlNewProp(Comprobante, (const xmlChar *)"xmlns:xsi", xsi_url);
  xmlNewProp(Comprobante, (const xmlChar *)"xsi:schemaLocation", cfdi_schemaLocation);

  /* Ahora comienza a escribir la información de nuestra estructura,
   * comienza por los atributos obligatorios
   */
  xmlNewProp(Comprobante, (const xmlChar *)"version", cfdi->version);
  xmlNewProp(Comprobante, (const xmlChar *)"fecha", cfdi->fecha);
  xmlNewProp(Comprobante, (const xmlChar *)"sello", cfdi->sello);
  xmlNewProp(Comprobante, (const xmlChar *)"formaDePago", cfdi->formaDePago);
  xmlNewProp(Comprobante, (const xmlChar *)"noCertificado", cfdi->noCertificado);
  xmlNewProp(Comprobante, (const xmlChar *)"certificado", cfdi->certificado);
  xmlNewProp(Comprobante, (const xmlChar *)"subTotal", cfdi->subTotal);
  xmlNewProp(Comprobante, (const xmlChar *)"total", cfdi->total);
  xmlNewProp(Comprobante, (const xmlChar *)"tipoDeComprobante", cfdi->tipoDeComprobante);
  xmlNewProp(Comprobante, (const xmlChar *)"metodoDePago", cfdi->metodoDePago);
  xmlNewProp(Comprobante, (const xmlChar *)"LugarExpedicion", cfdi->LugarExpedicion);

  /* Ahora los atributos opcionales */
  if ( cfdi->serie != NULL ) {
    xmlNewProp(Comprobante, (const xmlChar *)"serie", cfdi->serie);
  }

  if ( cfdi->folio != NULL ) {
    xmlNewProp(Comprobante, (const xmlChar *)"folio", cfdi->folio);
  }

  if ( cfdi->condicionesDePago != NULL ) {
    xmlNewProp(Comprobante, (const xmlChar *)"condicionesDePago", cfdi->condicionesDePago);
  }

  if ( cfdi->descuento != NULL ) {
    xmlNewProp(Comprobante, (const xmlChar *)"descuento", cfdi->descuento);
  }

  if ( cfdi->motivoDescuento != NULL ) {
    xmlNewProp(Comprobante, (const xmlChar *)"motivoDescuento", cfdi->motivoDescuento);
  }

  if ( cfdi->TipoCambio != NULL ) {
    xmlNewProp(Comprobante, (const xmlChar *)"TipoCambio", cfdi->TipoCambio);
  }

  if ( cfdi->Moneda != NULL ) {
    xmlNewProp(Comprobante, (const xmlChar *)"Moneda", cfdi->Moneda);
  }

  if ( cfdi->NumCtaPago != NULL ) {
    xmlNewProp(Comprobante, (const xmlChar *)"NumCtaPago", cfdi->NumCtaPago);
  }

  if ( cfdi->FolioFiscalOrig != NULL ) {
    xmlNewProp(Comprobante, (const xmlChar *)"FolioFiscalOrig", cfdi->FolioFiscalOrig);
  }

  if ( cfdi->SerieFolioFiscalOrig != NULL ) {
    xmlNewProp(Comprobante, (const xmlChar *)"SerieFolioFiscalOrig", cfdi->SerieFolioFiscalOrig);
  }

  if ( cfdi->FechaFolioFiscalOrig != NULL ) {
    xmlNewProp(Comprobante, (const xmlChar *)"FechaFolioFiscalOrig", cfdi->FechaFolioFiscalOrig);
  }

  if ( cfdi->MontoFolioFiscalOrig != NULL ) {
    xmlNewProp(Comprobante, (const xmlChar *)"MontoFolioFiscalOrig", cfdi->MontoFolioFiscalOrig);
  }

  /* Ahora los datos del emisor */
  Emisor = xmlNewChild(Comprobante, cfdi_ns, (const xmlChar *)"Emisor", NULL);
  xmlNewProp(Emisor, (const xmlChar *)"rfc", cfdi->Emisor->rfc);
  if ( cfdi->Emisor->nombre != NULL ) {
    xmlNewProp(Emisor, (const xmlChar *)"nombre", cfdi->Emisor->nombre);
  }

  /* Ahora el domicilio fiscal del emisor, en caso de que exista */
  if ( cfdi->Emisor->DomicilioFiscal != NULL ) {
    DomicilioFiscal = xmlNewChild(Emisor, cfdi_ns, (const xmlChar *)"DomicilioFiscal", NULL);
    /* Primero los parametros obligatorio */
    xmlNewProp(DomicilioFiscal, (const xmlChar *)"calle", cfdi->Emisor->DomicilioFiscal->calle);
    xmlNewProp(DomicilioFiscal, (const xmlChar *)"municipio", cfdi->Emisor->DomicilioFiscal->municipio);
    xmlNewProp(DomicilioFiscal, (const xmlChar *)"estado", cfdi->Emisor->DomicilioFiscal->estado);
    xmlNewProp(DomicilioFiscal, (const xmlChar *)"pais", cfdi->Emisor->DomicilioFiscal->pais);
    xmlNewProp(DomicilioFiscal, (const xmlChar *)"codigoPostal", cfdi->Emisor->DomicilioFiscal->codigoPostal);

    /* Y ahora algunos parametros opcionales */
    if ( cfdi->Emisor->DomicilioFiscal->noExterior != NULL ) {
      xmlNewProp(DomicilioFiscal, (const xmlChar *)"noExterior", cfdi->Emisor->DomicilioFiscal->noExterior);
    }

    if ( cfdi->Emisor->DomicilioFiscal->noInterior != NULL ) {
      xmlNewProp(DomicilioFiscal, (const xmlChar *)"noInterior", cfdi->Emisor->DomicilioFiscal->noInterior);
    }

    if ( cfdi->Emisor->DomicilioFiscal->colonia != NULL ) {
      xmlNewProp(DomicilioFiscal, (const xmlChar *)"colonia", cfdi->Emisor->DomicilioFiscal->colonia);
    }

    if ( cfdi->Emisor->DomicilioFiscal->localidad != NULL ) {
      xmlNewProp(DomicilioFiscal, (const xmlChar *)"localidad", cfdi->Emisor->DomicilioFiscal->localidad);
    }

    if ( cfdi->Emisor->DomicilioFiscal->referencia != NULL ) {
      xmlNewProp(DomicilioFiscal, (const xmlChar *)"referencia", cfdi->Emisor->DomicilioFiscal->referencia);
    }

  }

  /* Ahora los datos del sitio en que fue expedido el CFDI */
  if ( cfdi->Emisor->ExpedidoEn != NULL ) {
    ExpedidoEn = xmlNewChild(Emisor, cfdi_ns, (const xmlChar *)"ExpedidoEn", NULL);

    /* Primero los parametros obligatorios */
    xmlNewProp(ExpedidoEn, (const xmlChar *)"pais", cfdi->Emisor->ExpedidoEn->pais);

    /* Y ahora los parametros opcionales */
    if ( cfdi->Emisor->ExpedidoEn->calle != NULL ) {
      xmlNewProp(ExpedidoEn, (const xmlChar *)"calle", cfdi->Emisor->ExpedidoEn->calle);
    }

    if ( cfdi->Emisor->ExpedidoEn->municipio != NULL ) {
      xmlNewProp(ExpedidoEn, (const xmlChar *)"municipio", cfdi->Emisor->ExpedidoEn->municipio);
    }

    if ( cfdi->Emisor->ExpedidoEn->estado != NULL ) {
      xmlNewProp(ExpedidoEn, (const xmlChar *)"estado", cfdi->Emisor->ExpedidoEn->estado);
    }

    if ( cfdi->Emisor->ExpedidoEn->codigoPostal != NULL ) {
      xmlNewProp(ExpedidoEn, (const xmlChar *)"codigoPostal", cfdi->Emisor->ExpedidoEn->codigoPostal);
    }

    if ( cfdi->Emisor->ExpedidoEn->noExterior != NULL ) {
      xmlNewProp(ExpedidoEn, (const xmlChar *)"noExterior", cfdi->Emisor->ExpedidoEn->noExterior);
    }

    if ( cfdi->Emisor->ExpedidoEn->noInterior != NULL ) {
      xmlNewProp(ExpedidoEn, (const xmlChar *)"noInterior", cfdi->Emisor->ExpedidoEn->noInterior);
    }

    if ( cfdi->Emisor->ExpedidoEn->colonia != NULL ) {
      xmlNewProp(ExpedidoEn, (const xmlChar *)"colonia", cfdi->Emisor->ExpedidoEn->colonia);
    }

    if ( cfdi->Emisor->ExpedidoEn->localidad != NULL ) {
      xmlNewProp(ExpedidoEn, (const xmlChar *)"localidad", cfdi->Emisor->ExpedidoEn->localidad);
    }

    if ( cfdi->Emisor->ExpedidoEn->referencia != NULL ) {
      xmlNewProp(ExpedidoEn, (const xmlChar *)"referencia", cfdi->Emisor->ExpedidoEn->referencia);
    }

  }


  /* Ahora los datos del receptor */
  Receptor = xmlNewChild(Comprobante, cfdi_ns, (const xmlChar *)"Receptor", NULL);
  xmlNewProp(Receptor, (const xmlChar *)"rfc", cfdi->Receptor->rfc);
  if ( cfdi->Receptor->nombre != NULL ) {
    xmlNewProp(Receptor, (const xmlChar *)"nombre", cfdi->Receptor->nombre);
  }

  /* Ahora el domicilio del receptor */
  if ( cfdi->Receptor->Domicilio != NULL ) {

    Domicilio = xmlNewChild(Receptor, cfdi_ns, (const xmlChar *)"Domicilio", NULL);

    /* Primero los parametros obligatorios */
    xmlNewProp(Domicilio, (const xmlChar *)"pais", cfdi->Receptor->Domicilio->pais);

    /* Y ahora los parametros opcionales */
    if ( cfdi->Receptor->Domicilio->calle != NULL ) {
      xmlNewProp(Domicilio, (const xmlChar *)"calle", cfdi->Receptor->Domicilio->calle);
    }

    if ( cfdi->Receptor->Domicilio->noExterior != NULL ) {
      xmlNewProp(Domicilio, (const xmlChar *)"noExterior", cfdi->Receptor->Domicilio->noExterior);
    }

    if ( cfdi->Receptor->Domicilio->noInterior != NULL ) {
      xmlNewProp(Domicilio, (const xmlChar *)"noInterior", cfdi->Receptor->Domicilio->noInterior);
    }

    if ( cfdi->Receptor->Domicilio->colonia != NULL ) {
      xmlNewProp(Domicilio, (const xmlChar *)"colonia", cfdi->Receptor->Domicilio->colonia);
    }

    if ( cfdi->Receptor->Domicilio->localidad != NULL ) {
      xmlNewProp(Domicilio, (const xmlChar *)"localidad", cfdi->Receptor->Domicilio->localidad);
    }

    if ( cfdi->Receptor->Domicilio->referencia != NULL ) {
      xmlNewProp(Domicilio, (const xmlChar *)"referencia", cfdi->Receptor->Domicilio->referencia);
    }

    if ( cfdi->Receptor->Domicilio->municipio != NULL ) {
      xmlNewProp(Domicilio, (const xmlChar *)"municipio", cfdi->Receptor->Domicilio->municipio);
    }

    if ( cfdi->Receptor->Domicilio->estado != NULL ) {
      xmlNewProp(Domicilio, (const xmlChar *)"estado", cfdi->Receptor->Domicilio->estado);
    }

    if ( cfdi->Receptor->Domicilio->codigoPostal != NULL ) {
      xmlNewProp(Domicilio, (const xmlChar *)"codigoPostal", cfdi->Receptor->Domicilio->codigoPostal);
    }

  }


  /* Ahora los conceptos */
  Conceptos = xmlNewChild(Comprobante, cfdi_ns, (const xmlChar *)"Conceptos", NULL);
  conceptos = cfdi->Conceptos;
  while ( conceptos != NULL ) {
    Concepto = xmlNewChild(Conceptos, cfdi_ns, (const xmlChar *)"Concepto", NULL);
    xmlNewProp(Concepto, (const xmlChar *)"cantidad",      conceptos->cantidad);
    xmlNewProp(Concepto, (const xmlChar *)"unidad",        conceptos->unidad);
    xmlNewProp(Concepto, (const xmlChar *)"descripcion",   conceptos->descripcion);
    xmlNewProp(Concepto, (const xmlChar *)"valorUnitario", conceptos->valorUnitario);
    xmlNewProp(Concepto, (const xmlChar *)"importe",       conceptos->importe);
    if ( conceptos->noIdentificacion != NULL ) {
      xmlNewProp(Concepto, (const xmlChar *)"noIdentificacion", conceptos->noIdentificacion);
    }
    conceptos = conceptos->next;
  }

  /* Agrega los impuestos */

  /* Agrega el complemento */

  /* Agrega la addenda */

  /* Define el nodo comprobante como el nodo raiz */
  xmlDocSetRootElement(doc, Comprobante);

  /* Reajusta el namespace */
  xmlReconciliateNs(doc, Comprobante);

  /* Escribe el CFDI a un buffer */
  xmlDocDumpMemoryEnc(doc, &comprobante, &len, "UTF-8");
  if ( len == 0 ) {
    /* An error did ocurr */
  }

  xmlFreeDoc(doc);

  return comprobante;
}

/**
 *
 * Agrega a la lista otro concepto mas.
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
  xmlNewProp(tmp->xml_node, (const xmlChar *)"importe", importe);

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
			  const char *archivo_certificado,
			  const int verbose)
{
  xmlNodePtr comprobante = NULL;
  unsigned char *certificado = NULL;
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
  xmlNewProp(comprobante, (const xmlChar *)"xsi:schemaLocation", cfdi_schemaLocation);

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
  certificado = lee_certificado_alloc(archivo_certificado, &noCertificado, verbose);
  xmlNewProp(comprobante, (const xmlChar *)"certificado", certificado);
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
	  const char *archivo_del_emisor,
	  const char *archivo_certificado,
	  const char *archivo_llave_privada,
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
  unsigned char *sello = NULL;
  int res = 1;
  double retenidos = 0;   /* Total de impuestos retenidos */
  double trasladados = 0; /* Total de impuestos trasladados */
  char cantidad_impuestos[15];

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
					  archivo_certificado,
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
  emisor = crea_elemento_Emisor(archivo_del_emisor, cfdi_ns, verbose);
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
      retenidos += atof((char *)xmlGetProp(imps->xml_node, (const xmlChar *)"importe"));
      xmlSetNs(imps->xml_node, cfdi_ns);
      xmlAddChild(retenciones, imps->xml_node);
      imps = imps->next;
    }
  }
  if ( retenidos > 0 ) {
    memset(cantidad_impuestos, 0, 15);
    strfmon(cantidad_impuestos, 15, "%!.2n", retenidos);
    xmlNewProp(impuestos, (const xmlChar *)"totalImpuestosRetenidos", (const xmlChar *)cantidad_impuestos);
  }

  if ( traslado != NULL ) {
    xmlNodePtr traslados = xmlNewChild(impuestos, cfdi_ns, (const xmlChar *)"Traslados", NULL);
    imps = traslado;
    while ( imps != NULL ) {
      trasladados += atof((char *)xmlGetProp(imps->xml_node, (const xmlChar *)"importe"));
      xmlSetNs(imps->xml_node, cfdi_ns);
      xmlAddChild(traslados, imps->xml_node);
      imps = imps->next;
    }
  }
  if ( trasladados > 0 ) {
    memset(cantidad_impuestos, 0, 15);
    strfmon(cantidad_impuestos, 15, "%!.2n", trasladados);
    xmlNewProp(impuestos, (const xmlChar *)"totalImpuestosTrasladados", (const xmlChar *)cantidad_impuestos);
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
  res = genera_cadena_original(cadena_original_stylesheet,
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
  sello = sello_alloc(archivo_llave_privada, "sha1", cadena, verbose);
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
  xmlNewProp(comprobante, (const xmlChar *)"sello", sello);
  free(sello);

  return doc;
}
