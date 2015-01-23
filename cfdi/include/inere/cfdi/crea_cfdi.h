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

#ifdef __cplusplus
extern "C" {
#endif

#include <libxml/tree.h>

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
