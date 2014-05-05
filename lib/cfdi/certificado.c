#ifndef EDISSON_CERTIFICADO_H_INCLUDED
#include "inere/cfdi/certificado.h"
#endif
#ifndef BASE64_H
#include "inere/base64.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <openssl/x509.h>

/**
 * The user is responsable to free the result (with free())
 */
char *
load_certificate(const char *filename, unsigned char **serial_number, const int verbose)
{
  int len = 0;
  int read = 0;
  char *orig = NULL;
  char *cert = NULL;
  const char *default_serial_number = "00001000000200258016";
  X509* x509 = NULL;
  FILE *file = NULL;

  file = fopen(filename, "rb");
  if ( file == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d No fue posible abrir el archivo %s.\n", __FILE__, __LINE__, filename);
    }
    return NULL;
  }

  len = fseek(file, 0, SEEK_END);
  len = ftell(file);
  rewind(file);

  /* Load the certificate */
  orig = (char *)calloc(len + 1, sizeof(char));
  read = fread(orig, sizeof(char), len, file);
  if ( read != len ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Ocurrio un error. El número de bytes leídos fue %d, pero debieron de haberse leido %d.\n", __FILE__, __LINE__, read, len);
    }
    free(orig);
    fclose(file);
    return NULL;
  }


  /* Now encode the certificate to base64 */
  base64_encode_alloc(orig, len, &cert);
  if ( cert == NULL ) {
    free(orig);
    fclose(file);
    if ( verbose ) {
      fprintf(stderr, "%s:%d: No fue posible codificar el certificado (base64) contenido en %s.", __FILE__, __LINE__, filename);
    }
    return NULL;
  }
  free(orig);

  /* Allocate the memory for the serial number */
  if ( *serial_number == NULL ) *serial_number = (unsigned char*)malloc(21);
  memset(*serial_number, 0, 21);

  rewind(file);
  x509 = d2i_X509_fp(file, &x509);
  if ( x509 == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d: Se produjo un error al leer el certificado x509 para extraer el número serial del certificado mismo. Se utilizará el número serial (%s)\n", __FILE__, __LINE__, default_serial_number);
    }
    strncpy((char *)*serial_number, default_serial_number, 20);
  }
  fclose(file);

  if ( x509 != NULL ) {
    /* Get the serial number */
    strncpy((char *)*serial_number, (const char *)x509->cert_info->serialNumber->data, 20);
    if ( verbose ) {
      printf("%s:%d: Número de serie extraído del certificado: %s\n", __FILE__, __LINE__, *serial_number);
    }
    if ( strlen((char *)serial_number) == 0 ) {
      if ( verbose ) {
	fprintf(stderr, "%s:%d: No fue posible leer el numero de serie del certificado de llave publica.\n", __FILE__, __LINE__);
      }
    }

    X509_free(x509);
  }

  return cert;
}