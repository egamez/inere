/*
 * Copyright (c) 2014, L3a,
 *                     Enrique Gámez Flores <egamez@edisson.com.mx>
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
#ifndef EDISSON_CURL_ENVIA_EMAIL_H
#define EDISSON_CURL_ENVIA_EMAIL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct name_addr {
  char *display_name;
  char *angle_addr;
  struct name_addr *next;
} name_addr_t;

extern int curl_envia_email(const char *username, const char *password,
		const char *mailserver, const char *display_name_from,
		const char *from, const char *display_name_to, const char *to,
		const char *cc, const char *asunto, const char *body_filename,
		const char *cfdi_filename, const char *pdf_filename,
		const char *keywords, const char *organization,
		const char *importance, const char *sensitivity,
		const int insecure, const int verbose);

extern int curl_envia_email_mult(const char *username, const char *password,
				 const char *mailserver,
				 const char *display_name_from,
				 const char *from, name_addr_t *to,
				 name_addr_t *cc, const char *asunto,
				 const char *body_filename,
				 const char *cfdi_filename,
				 const char *pdf_filename,
				 const char *keywords, const char *organization,
				 const char *importance,
				 const char *sensitivity,
				 const int insecure, const int verbose);

extern name_addr_t *name_addr_list_append(name_addr_t *head, const char *nombre, const char *buzon);

extern void name_addr_list_clear(name_addr_t *head);

extern name_addr_t *name_addr_list_update_name(name_addr_t *head, const char *mailbox, const char *display);

#ifdef __cplusplus
}
#endif

#endif /* EDISSON_CURL_ENVIA_EMAIL_H */
