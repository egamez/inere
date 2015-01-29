#ifndef INERE_CFDI_CREA_CFDI_QRCODE_CFDI_H_
#define INERE_CFDI_CREA_CFDI_QRCODE_CFDI_H_

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

#endif /* INERE_CFDI_CREA_CFDI_QRCODE_CFDI_H_ */
