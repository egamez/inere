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
