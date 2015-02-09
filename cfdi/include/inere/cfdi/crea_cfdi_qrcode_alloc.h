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
#ifndef INERE_CFDI_CREA_CFDI_QRCODE_ALLOC_H_
#define INERE_CFDI_CREA_CFDI_QRCODE_ALLOC_H_

#include <qrencode.h>

#ifdef __cplusplus
extern "C" {
#endif

/* structure to store PNG image bytes */
typedef struct cfdi_qrcode {
  char *buffer;
  int size;
} cfdi_qrcode_t;


extern int termina_qrcode(cfdi_qrcode_t *code);
extern cfdi_qrcode_t *crea_cfdi_qrcode_alloc(const char *rfc_emisor,
					     const char *rfc_receptor,
					     const char *total,
					     const char *uuid);

#ifdef __cplusplus
}
#endif

#endif /* INERE_CFDI_CREA_CFDI_QRCODE_ALLOC_H_ */
