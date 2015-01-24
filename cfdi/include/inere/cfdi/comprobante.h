/*
 * Copyright (c) 2015, L3a,
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
#ifndef INERE_CFDI_COMPROBANTE_H_
#define INERE_CFDI_COMPROBANTE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Domicilio {
  unsigned char *calle;
  unsigned char *noExterior;
  unsigned char *noInterior;
  unsigned char *colonia;
  unsigned char *localidad;
  unsigned char *municipio;
  unsigned char *estado;
  unsigned char *pais;
  unsigned char *referencia;
  unsigned char *codigoPostal;
} Domicilio_t;

typedef struct _Emisor {
  unsigned char *rfc;
  unsigned char *nombre;
  Domicilio_t   *DomicilioFiscal;
  Domicilio_t   *ExpedidoEn;
  unsigned char *RegimenFiscal;
} Emisor_t;

typedef struct _Receptor {
  unsigned char *rfc;
  unsigned char *nombre;
  Domicilio_t   *Domicilio;
} Receptor_t;

typedef struct _InformacionAduanera_list {
  unsigned char                    *numero;
  unsigned char                    *fecha;
  unsigned char                    *aduana;
  unsigned int                      size;
  struct _InformacionAduanera_list *next;
} InformacionAduanera_list_t;

typedef struct _CuentaPredial {
  unsigned char *numero;
} CuentaPredial_t;

typedef struct _Parte_list {
  unsigned char              *cantidad;
  unsigned char              *noIdentificacion;
  unsigned char              *unidad;
  unsigned char              *descripcion;
  unsigned char              *valorUnitario;
  unsigned char              *importe;
  InformacionAduanera_list_t *InformacionAduanera;
  unsigned int                size;
  struct _Parte_list         *next;
} Parte_list_t;

typedef struct _Concepto_list {
  unsigned char              *cantidad;
  unsigned char              *noIdentificacion;
  unsigned char              *unidad;
  unsigned char              *descripcion;
  unsigned char              *valorUnitario;
  unsigned char              *importe;
  InformacionAduanera_list_t *InformacionAduanera;
  CuentaPredial_t            *CuentaPredial;
  Parte_list_t               *Parte;
  unsigned int                size;
  struct _Concepto_list      *next;
} Concepto_list_t;

typedef struct _Traslado_list {
  unsigned char         *impuesto;
  unsigned char         *tasa;
  unsigned char         *importe;
  unsigned int           size;
  struct _Traslado_list *next;
} Traslado_list_t;

typedef struct _Retencion_list {
  unsigned char          *impuesto;
  unsigned char          *importe;
  unsigned int            size;
  struct _Retencion_list *next;
} Retencion_list_t;

typedef struct _Impuestos {
  Retencion_list_t *Retenciones;
  Traslado_list_t  *Traslados;
} Impuestos_t;

typedef struct _TimbreFiscalDigital {
  unsigned char *version;
  unsigned char *UUID;
  unsigned char *noCertificadoSAT;
  unsigned char *FechaTimbrado;
  unsigned char *selloCFD;
  unsigned char *selloSAT;
} TimbreFiscalDigital_t;

typedef struct _Complemento {
  TimbreFiscalDigital_t *TimbreFiscalDigital;
} Complemento_t;

typedef struct _Addenda {
  unsigned char *addenda;
} Addenda_t;

typedef struct _Comprobante {
  unsigned char   *version;
  unsigned char   *serie;
  unsigned char   *folio;
  unsigned char   *fecha;
  unsigned char   *sello;
  unsigned char   *formaDePago;
  unsigned char   *noCertificado;
  unsigned char   *certificado;
  unsigned char   *condicionesDePago;
  unsigned char   *subTotal;
  unsigned char   *descuento;
  unsigned char   *motivoDescuento;
  unsigned char   *TipoCambio;
  unsigned char   *Moneda;
  unsigned char   *total;
  unsigned char   *tipoDeComprobante;
  unsigned char   *metodoDePago;
  unsigned char   *LugarExpedicion;
  unsigned char   *NumCtaPago;
  unsigned char   *FolioFiscalOrig;
  unsigned char   *SerieFolioFiscalOrig;
  unsigned char   *FechaFolioFiscalOrig;
  unsigned char   *MontoFolioFiscalOrig;
  Emisor_t        *Emisor;
  Receptor_t      *Receptor;
  Concepto_list_t *Conceptos;
  Impuestos_t     *Impuestos;
  Complemento_t   *Complemento;
  Addenda_t       *Addenda;
} Comprobante_t;

#ifdef __cplusplus
}
#endif

#endif /* INERE_CFDI_COMPROBANTE_H_ */
