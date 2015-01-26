/*
 * Copyright (c) 2014-2015, L3a,
 *		            Enrique Gamez Flores <egamez@edisson.com.mx>
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
#define INERE_CFDI_CREA_CFDI_H_

#ifndef INERE_CFDI_COMPROBANTE_H_
#include "inere/cfdi/comprobante.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <libxml/tree.h>

extern Comprobante_t *crea_comprobante(unsigned char *version);
extern int asigna_serie(Comprobante_t *cfdi, unsigned char *serie);
extern int asigna_folio(Comprobante_t *cfdi, unsigned char *folio);
extern int asigna_fecha(Comprobante_t *cfdi, unsigned char *fecha);
extern int asigna_sello(Comprobante_t *cfdi, unsigned char *sello);
extern int asigna_formaDePago(Comprobante_t *cfdi, unsigned char *formaDePago);
extern int asigna_noCertificado(Comprobante_t *cfdi, unsigned char *noCertificado);
extern int asigna_certificado(Comprobante_t *cfdi, unsigned char *certificado);
extern int asigna_condicionesDePago(Comprobante_t *cfdi, unsigned char *condicionesDePago);
extern int asigna_subTotal(Comprobante_t *cfdi, unsigned char *subTotal);
extern int asigna_descuento(Comprobante_t *cfdi, unsigned char *descuento);
extern int asigna_motivoDescuento(Comprobante_t *cfdi, unsigned char *motivoDescuento);
extern int asigna_TipoCambio(Comprobante_t *cfdi, unsigned char *TipoCambio);
extern int asigna_Moneda(Comprobante_t *cfdi, unsigned char *Moneda);
extern int asigna_total(Comprobante_t *cfdi, unsigned char *total);
extern int asigna_tipoDeComprobante(Comprobante_t *cfdi, unsigned char *tipoDeComprobante);
extern int asigna_metodoDePago(Comprobante_t *cfdi, unsigned char *metodoDePago);
extern int asigna_LugarExpedicion(Comprobante_t *cfdi, unsigned char *LugarExpedicion);
extern int asigna_NumCtaPago(Comprobante_t *cfdi, unsigned char *NumCtaPago);
extern int asigna_FolioFiscalOrig(Comprobante_t *cfdi, unsigned char *FolioFiscalOrig);
extern int asigna_SerieFolioFiscalOrig(Comprobante_t *cfdi, unsigned char *SerieFolioFiscalOrig);
extern int asigna_FechaFolioFiscalOrig(Comprobante_t *cfdi, unsigned char *FechaFolioFiscalOrig);
extern int asigna_MontoFolioFiscalOrig(Comprobante_t *cfdi, unsigned char *MontoFolioFiscalOrig);

extern int agrega_concepto(Comprobante_t *cfdi, unsigned char *cantidad, unsigned char *noIdentificacion, unsigned char *unidad, unsigned char *descripcion, unsigned char *valorUnitario, unsigned char *importe);
extern xmlChar *genera_comprobante_alloc(Comprobante_t *cfdi);

typedef struct cfdi_items_list {
  xmlNodePtr xml_node;
  struct cfdi_items_list *next;
} cfdi_items_list_t;

extern xmlDocPtr crea_cfdi(const xmlChar *subTotal,
			   const xmlChar *descuento,
			   const xmlChar *total,
			   cfdi_items_list_t* productos,
			   cfdi_items_list_t *retencion,
			   cfdi_items_list_t *traslado,
			   const char *archivo_del_emisor,
			   const char *archivo_certificado,
			   const char *archivo_llave_privada,
			   const int verbose);

extern cfdi_items_list_t *append_concepto(cfdi_items_list_t *head,
					  const xmlChar *cantidad,
					  const xmlChar *unidad,
					  const xmlChar *noIdentificacion,
					  const xmlChar *descripcion,
					  const xmlChar *valorUnitario,
					  const xmlChar *importe,
					  const int verbose);

extern cfdi_items_list_t *append_impuesto(cfdi_items_list_t *head,
					  const xmlChar *tipo,
					  const xmlChar *impuesto,
					  const xmlChar *tasa,
					  const xmlChar *importe,
					  const int verbose);

extern void free_cfdi_items_list(cfdi_items_list_t* head);

#ifdef __cplusplus
}
#endif

#endif /* INERE_CFDI_CREA_CFDI_H_ */
