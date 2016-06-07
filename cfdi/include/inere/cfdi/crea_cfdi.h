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
extern int termina_comprobante(Comprobante_t *cfdi);
extern unsigned char *obten_fecha_ISO8601_alloc(const int verbose);
extern int asigna_serie(Comprobante_t *cfdi,
			unsigned char *serie);
extern int asigna_folio(Comprobante_t *cfdi,
			unsigned char *folio);
extern int asigna_fecha(Comprobante_t *cfdi,
			unsigned char *fecha);
extern int asigna_sello(Comprobante_t *cfdi,
			unsigned char *sello);
extern int asigna_formaDePago(Comprobante_t *cfdi,
			      unsigned char *formaDePago);
extern int asigna_noCertificado(Comprobante_t *cfdi,
				unsigned char *noCertificado);
extern int asigna_certificado(Comprobante_t *cfdi,
			      unsigned char *certificado);
extern int asigna_condicionesDePago(Comprobante_t *cfdi,
				    unsigned char *condicionesDePago);
extern int asigna_subTotal(Comprobante_t *cfdi,
			   unsigned char *subTotal);
extern int asigna_descuento(Comprobante_t *cfdi,
			    unsigned char *descuento);
extern int asigna_motivoDescuento(Comprobante_t *cfdi,
				  unsigned char *motivoDescuento);
extern int asigna_TipoCambio(Comprobante_t *cfdi,
			     unsigned char *TipoCambio);
extern int asigna_Moneda(Comprobante_t *cfdi,
			 unsigned char *Moneda);
extern int asigna_total(Comprobante_t *cfdi,
			unsigned char *total);
extern int asigna_tipoDeComprobante(Comprobante_t *cfdi,
				    unsigned char *tipoDeComprobante);
extern int agrega_metodoDePago(Comprobante_t *cfdi,
			       unsigned char *metodoDePago);
extern int asigna_LugarExpedicion(Comprobante_t *cfdi,
				  unsigned char *LugarExpedicion);
extern int asigna_NumCtaPago(Comprobante_t *cfdi,
			     unsigned char *NumCtaPago);
extern int asigna_FolioFiscalOrig(Comprobante_t *cfdi,
				  unsigned char *FolioFiscalOrig);
extern int asigna_SerieFolioFiscalOrig(Comprobante_t *cfdi,
				       unsigned char *SerieFolioFiscalOrig);
extern int asigna_FechaFolioFiscalOrig(Comprobante_t *cfdi,
				       unsigned char *FechaFolioFiscalOrig);
extern int asigna_MontoFolioFiscalOrig(Comprobante_t *cfdi,
				       unsigned char *MontoFolioFiscalOrig);
extern int agrega_Concepto(Comprobante_t *cfdi,
			   unsigned char *cantidad,
			   unsigned char *noIdentificacion,
			   unsigned char *unidad,
			   unsigned char *descripcion,
			   unsigned char *valorUnitario,
			   unsigned char *importe);
extern xmlChar *genera_comprobante_alloc(Comprobante_t *cfdi);
extern int agrega_Emisor_RegimenFiscal(Comprobante_t *cfdi,
				       unsigned char *Regimen);
extern int agrega_Emisor(Comprobante_t *cfdi,
			 unsigned char *rfc,
			 unsigned char *nombre);
extern int agrega_Emisor_DomicilioFiscal(Comprobante_t *cfdi,
					 unsigned char *calle,
					 unsigned char *noExterior,
					 unsigned char *noInterior,
					 unsigned char *colonia,
					 unsigned char *localidad,
					 unsigned char *municipio,
					 unsigned char *estado,
					 unsigned char *pais,
					 unsigned char *referencia,
					 unsigned char *codigoPostal);
extern int agrega_Emisor_ExpedidoEn(Comprobante_t *cfdi,
				    unsigned char *calle,
				    unsigned char *noExterior,
				    unsigned char *noInterior,
				    unsigned char *colonia,
				    unsigned char *localidad,
				    unsigned char *municipio,
				    unsigned char *estado,
				    unsigned char *pais,
				    unsigned char *referencia,
				    unsigned char *codigoPostal);
extern int agrega_Receptor(Comprobante_t *cfdi,
			   unsigned char *rfc,
			   unsigned char *nombre);
extern int agrega_Receptor_Domicilio(Comprobante_t *cfdi,
				     unsigned char *calle,
				     unsigned char *noExterior,
				     unsigned char *noInterior,
				     unsigned char *colonia,
				     unsigned char *localidad,
				     unsigned char *municipio,
				     unsigned char *estado,
				     unsigned char *pais,
				     unsigned char *referencia,
				     unsigned char *codigoPostal);
extern int agrega_Impuesto_Retencion(Comprobante_t *cfdi,
				      unsigned char *impuesto,
				      unsigned char *importe);
extern int agrega_Impuesto_Traslado(Comprobante_t *cfdi,
				    unsigned char *impuesto,
				    unsigned char *tasa,
				    unsigned char *importe);
extern int agrega_Complemento_TimbreFiscalDigital(Comprobante_t *cfdi,
				      unsigned char *version,
				      unsigned char *UUID,
				      unsigned char *noCertificadoSAT,
				      unsigned char *FechaTimbrado,
				      unsigned char *selloCFD,
				      unsigned char *selloSAT);


#ifdef __cplusplus
}
#endif

#endif /* INERE_CFDI_CREA_CFDI_H_ */
