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

/**
 * 2014-05-05T21:35:56
 */
unsigned char *
obten_fecha_ISO8601_alloc(const int verbose)
{
  time_t t;
  struct tm tiempo;
  size_t res;
  const size_t len = 20;
  unsigned char *fecha = (unsigned char *)calloc(len, sizeof(unsigned char));

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
termina_comprobante(Comprobante_t *cfdi)
{
  Concepto_list_t *prods1 = NULL;
  Concepto_list_t *prods2 = NULL;
  Retencion_list_t *ret1 = NULL;
  Retencion_list_t *ret2 = NULL;
  Traslado_list_t *tras1 = NULL;
  Traslado_list_t *tras2 = NULL;
  RegimenFiscal_list_t *reg1 = NULL;
  RegimenFiscal_list_t *reg2 = NULL;
  int res = 0;

  if ( cfdi->Addenda != NULL ) {
    free(cfdi->Addenda);
  }

  if ( cfdi->Complemento->TimbreFiscalDigital != NULL ) {
    free(cfdi->Complemento->TimbreFiscalDigital);
  }

  if ( cfdi->Complemento != NULL ) {
    free(cfdi->Complemento);
  }

  ret1 = cfdi->Impuestos->Retenciones;
  while ( ret1 != NULL ) {
    ret2 = ret1;
    free(ret2);
    ret1 = ret1->next;
  }
  tras1 = cfdi->Impuestos->Traslados;
  while ( tras1 != NULL ) {
    tras2 = tras1;
    free(tras2);
    tras1 = tras1->next;
  }
  free(cfdi->Impuestos);

  prods1 = cfdi->Conceptos;
  while ( prods1 != NULL ) {
    prods2 = prods1;
    free(prods2);
    prods1 = prods1->next;
  }
  free(cfdi->Conceptos);

  if ( cfdi->Receptor->Domicilio != NULL ) {
    free(cfdi->Receptor->Domicilio);
  }
  free(cfdi->Receptor);

  reg1 = cfdi->Emisor->RegimenFiscal;
  while ( reg1 != NULL ) {
    reg2 = reg1;
    free(reg2);
    reg1 = reg1->next;
  }

  if ( cfdi->Emisor->ExpedidoEn != NULL ) {
    free(cfdi->Emisor->ExpedidoEn);
  }
  if ( cfdi->Emisor->DomicilioFiscal != NULL ) {
    free(cfdi->Emisor->DomicilioFiscal);
  }
  free(cfdi->Emisor);

  free(cfdi);
  cfdi = NULL;

  return res;
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
  } else if ( fecha == NULL ) {
    ret = 2;
    fprintf(stderr, "%s:%d Error. Fecha nula.\n", __FILE__, __LINE__);
  } else if ( strlen((char *)fecha) > 19 ) {
    ret = 3;
    fprintf(stderr, "%s:%d Error. Se ha excedido la longitud máxima para asignar la fecha del comprobante.\n", __FILE__, __LINE__);
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
agrega_Complemento_TimbreFiscalDigital(Comprobante_t *cfdi,
			   unsigned char *version,
			   unsigned char *UUID,
			   unsigned char *noCertificadoSAT,
			   unsigned char *FechaTimbrado,
			   unsigned char *selloCFD,
			   unsigned char *selloSAT)
{
  int res = 0;

  if ( cfdi == NULL ) {
    res = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else {

    if ( version          == NULL ||
	 UUID             == NULL ||
	 noCertificadoSAT == NULL ||
	 FechaTimbrado    == NULL ||
	 selloCFD         == NULL ||
	 selloSAT         == NULL ) {

      res = 2;
      fprintf(stderr, "%s:%d Error. Alguno o todos los parametros para expresar el Timbre Fiscal digital son nulos.\n", __FILE__, __LINE__);

    } else {

      if ( cfdi->Complemento == NULL ) {
	cfdi->Complemento = (Complemento_t *)malloc(sizeof(Complemento_t));
      }

      if ( cfdi->Complemento->TimbreFiscalDigital != NULL ) {
	free(cfdi->Complemento->TimbreFiscalDigital);
      }

      cfdi->Complemento->TimbreFiscalDigital = (TimbreFiscalDigital_t *)malloc(sizeof(TimbreFiscalDigital_t));
      cfdi->Complemento->TimbreFiscalDigital->version = version;
      cfdi->Complemento->TimbreFiscalDigital->UUID = UUID;
      cfdi->Complemento->TimbreFiscalDigital->noCertificadoSAT = noCertificadoSAT;
      cfdi->Complemento->TimbreFiscalDigital->FechaTimbrado = FechaTimbrado;
      cfdi->Complemento->TimbreFiscalDigital->selloCFD = selloCFD;
      cfdi->Complemento->TimbreFiscalDigital->selloSAT = selloSAT;

    }

  }

  return res;
}


/**
 *
 */
int
agrega_Emisor(Comprobante_t *cfdi, unsigned char *rfc, unsigned char *nombre)
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
      cfdi->Emisor->nombre          = nombre;
      cfdi->Emisor->RegimenFiscal   = NULL;
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
agrega_Emisor_RegimenFiscal(Comprobante_t *cfdi, unsigned char *Regimen)
{
  RegimenFiscal_list_t *current = NULL;
  RegimenFiscal_list_t *tmp = NULL;
  int res = 0;

  if ( cfdi == NULL ) {
    res = 1;
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
  } else {

    if ( cfdi->Emisor == NULL ) {
      res = 2;
      fprintf(stderr, "%s:%d Error. Es necesario definir primero la identificacion del Emisor.\n", __FILE__, __LINE__);

    } else {

      tmp = (RegimenFiscal_list_t *)malloc(sizeof(RegimenFiscal_list_t));
      tmp->Regimen = Regimen;
      tmp->next = NULL;

      /* Verifica que la lista de los regimenes ya halla sido creado */
      if ( cfdi->Emisor->RegimenFiscal == NULL ) {

	/* Esta es la primera entrada */
	cfdi->Emisor->RegimenFiscal = tmp;
	cfdi->Emisor->RegimenFiscal->size = 1;

      } else {

	/* Agrega una entrada */
	current = cfdi->Emisor->RegimenFiscal;
	while ( current->next != NULL ) {
	  current = current->next;
	}
	current->next = tmp;
	current->size++;

      }

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
      cfdi->Receptor->Domicilio->pais         = pais;
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
  Concepto_list_t *current = NULL;
  Concepto_list_t *tmp = NULL;
/*
  CuentaPredial_t *predial = NULL;
  InformacionAduanera_list_t *info_a = NULL;
  InformacionAduanera_list_t *info_a_tmp = NULL;
  InformacionAduanera_list_t *info_a_current = NULL;
  Parte_list_t *parte = NULL;
*/

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
 *
 */
int
agrega_Impuesto_Retencion(Comprobante_t *cfdi,
			  unsigned char *impuesto,
			  unsigned char *importe)
{
  Retencion_list_t *current = NULL;
  Retencion_list_t *tmp = NULL;
  int res = 0;

  if ( cfdi == NULL ) {
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
    return 1;
  }

  if ( impuesto == NULL ) {
    fprintf(stderr, "%s:%d Error. Impuesto nulo.\n", __FILE__, __LINE__);
    return 2;
  } else if ( strlen((char *)impuesto) == 0 ) {
    fprintf(stderr, "%s:%d Error. No hay descripcion del impuesto.\n", __FILE__, __LINE__);
    return 3;
  } else if ( importe == NULL ) {
    fprintf(stderr, "%s:%d Error. Importe nulo.\n", __FILE__, __LINE__);
    return 4;
  } else if ( strlen((char *)importe) == 0 ) {
    fprintf(stderr, "%s:%d Error. No hay valor para el importe.\n", __FILE__, __LINE__);
    return 5;
  }

  tmp = (Retencion_list_t *)malloc(sizeof(Retencion_list_t));
  if ( tmp == NULL ) {
    fprintf(stderr, "%s:%d Error al momento de reservar memoria para esta retencion de impuestos.\n", __FILE__, __LINE__);
    return 6;
  }

  tmp->impuesto = impuesto;
  tmp->importe  = importe; 
  tmp->next     = NULL;

  if ( cfdi->Impuestos == NULL ) {
    cfdi->Impuestos = (Impuestos_t *)malloc(sizeof(Impuestos_t));
    cfdi->Impuestos->Retenciones = NULL;
    cfdi->Impuestos->Traslados   = NULL;
  }

  if ( cfdi->Impuestos->Retenciones == NULL ) {
    /* Esta es la primera entrada */
    cfdi->Impuestos->Retenciones = tmp;
    cfdi->Impuestos->Retenciones->size = 1;

  } else {
    /* Agrega una entrada */
    current = cfdi->Impuestos->Retenciones;
    while ( current->next != NULL ) {
      current = current->next;
    }

    current->next = tmp;
    current->size++;
  }

  return res;
}


/**
 *
 */
int
agrega_Impuesto_Traslado(Comprobante_t *cfdi,
			 unsigned char *impuesto,
			 unsigned char *tasa,
			 unsigned char *importe)
{
  Traslado_list_t *current = NULL;
  Traslado_list_t *tmp = NULL;
  int res = 0;

  if ( cfdi == NULL ) {
    fprintf(stderr, "%s:%d Error. Comprobante nulo.\n", __FILE__, __LINE__);
    return 1;
  }

  if ( impuesto == NULL ) {
    fprintf(stderr, "%s:%d Error. Impuesto nulo.\n", __FILE__, __LINE__);
    return 2;
  } else if ( strlen((char *)impuesto) == 0 ) {
    fprintf(stderr, "%s:%d Error. No hay descripcion del impuesto.\n", __FILE__, __LINE__);
    return 3;
  } else if ( importe == NULL ) {
    fprintf(stderr, "%s:%d Error. Importe nulo.\n", __FILE__, __LINE__);
    return 4;
  } else if ( strlen((char *)importe) == 0 ) {
    fprintf(stderr, "%s:%d Error. No hay valor para el importe.\n", __FILE__, __LINE__);
    return 5;
  } else if ( tasa == NULL ) {
    fprintf(stderr, "%s:%d Error. Tasa nulo.\n", __FILE__, __LINE__);
    return 6;
  } else if ( strlen((char *)tasa) == 0 ) {
    fprintf(stderr, "%s:%d Error. No hay valor para la tasa del impuesto trasladado.\n", __FILE__, __LINE__);
    return 7;
  } else if ( strcasecmp((char *)impuesto, "IVA") &&
	      strcasecmp((char *)impuesto, "IEPS") ) {
    fprintf(stderr, "%s:%d Error. El tipo de impuesto no es ni IVA ni IEPS, por tanto no será válido el comprobante, el tipo de impuesto a trasladador dado es: '%s'\n", __FILE__, __LINE__, impuesto);
    return 7;
  }

  tmp = (Traslado_list_t *)malloc(sizeof(Traslado_list_t));
  if ( tmp == NULL ) {
    fprintf(stderr, "%s:%d Error al momento de reservar memoria para este traslado de impuestos.\n", __FILE__, __LINE__);
    return 9;
  }

  tmp->impuesto = impuesto;
  tmp->tasa     = tasa;
  tmp->importe  = importe; 
  tmp->next     = NULL;

  if ( cfdi->Impuestos == NULL ) {
    cfdi->Impuestos = (Impuestos_t *)malloc(sizeof(Impuestos_t));
    cfdi->Impuestos->Retenciones = NULL;
    cfdi->Impuestos->Traslados   = NULL;
  }

  if ( cfdi->Impuestos->Traslados == NULL ) {
    /* Esta es la primera entrada */
    cfdi->Impuestos->Traslados = tmp;
    cfdi->Impuestos->Traslados->size = 1;

  } else {
    /* Agrega una entrada */
    current = cfdi->Impuestos->Traslados;
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
  xmlDocPtr doc                 = NULL;
  xmlNsPtr cfdi_ns              = NULL;
  xmlNodePtr Comprobante        = NULL;
  xmlNodePtr Emisor             = NULL;
  xmlNodePtr RegimenFiscal      = NULL;
  xmlNodePtr DomicilioFiscal    = NULL;
  xmlNodePtr ExpedidoEn         = NULL;
  xmlNodePtr Receptor           = NULL;
  xmlNodePtr Domicilio          = NULL;
  xmlNodePtr Conceptos          = NULL;
  xmlNodePtr Concepto           = NULL;
  xmlNodePtr Impuestos          = NULL;
  xmlNodePtr Retenciones        = NULL;
  xmlNodePtr Retencion          = NULL;
  xmlNodePtr Traslados          = NULL;
  xmlNodePtr Traslado           = NULL;
  xmlNodePtr Complemento        = NULL;
  xmlNodePtr TimbreFiscalDigital= NULL;
  Concepto_list_t *conceptos    = NULL;
  Retencion_list_t *retenciones = NULL;
  Traslado_list_t *traslados    = NULL;
  RegimenFiscal_list_t *regimen = NULL;
  xmlChar *comprobante          = NULL;
  int len                       = 0;

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

  /* Y finalmente el regimen fiscal */
  regimen = cfdi->Emisor->RegimenFiscal;
  while ( regimen != NULL ) {
    RegimenFiscal = xmlNewChild(Emisor, cfdi_ns, (const xmlChar *)"RegimenFiscal", NULL);
    xmlNewProp(RegimenFiscal, (const xmlChar *)"Regimen", regimen->Regimen);
    regimen = regimen->next;
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


  /* Agrega los impuestos
   * y obten un total de los impuestos
   */
  if ( cfdi->Impuestos != NULL ) {

    Impuestos = xmlNewChild(Comprobante, cfdi_ns, (const xmlChar *)"Impuestos", NULL);
    if ( cfdi->Impuestos->Retenciones != NULL ) {
      Retenciones = xmlNewChild(Impuestos, cfdi_ns, (const xmlChar *)"Retenciones", NULL);
      retenciones = cfdi->Impuestos->Retenciones;
      while ( retenciones != NULL ) {
	Retencion = xmlNewChild(Retenciones, cfdi_ns, (const xmlChar *)"Retencion", NULL);
	xmlNewProp(Retencion, (const xmlChar *)"impuesto", retenciones->impuesto);
	xmlNewProp(Retencion, (const xmlChar *)"importe",  retenciones->importe);
	retenciones = retenciones->next;
      }
    }

    if ( cfdi->Impuestos->Traslados != NULL ) {
      Traslados = xmlNewChild(Impuestos, cfdi_ns, (const xmlChar *)"Traslados", NULL);
      traslados = cfdi->Impuestos->Traslados;
      while ( traslados != NULL ) {
	Traslado = xmlNewChild(Traslados, cfdi_ns, (const xmlChar *)"Traslado", NULL);
	xmlNewProp(Traslado, (const xmlChar *)"impuesto", traslados->impuesto);
	xmlNewProp(Traslado, (const xmlChar *)"tasa",     traslados->tasa);
	xmlNewProp(Traslado, (const xmlChar *)"importe",  traslados->importe);
	traslados = traslados->next;
      }

    }

  }


  /* Agrega el complemento */
  if ( cfdi->Complemento != NULL ) {
    Complemento = xmlNewChild(Comprobante, cfdi_ns, (const xmlChar *)"Complemento", NULL);

    if ( cfdi->Complemento->TimbreFiscalDigital != NULL ) {
    }

  }

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
