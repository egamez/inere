#include "inere/envia/non_ascii.h"
#include "inere/base64.h"

#include <stdlib.h>
#include <string.h>

/**
 * Esta función se encargará de aplicar el mecanismo de para convertir
 * cadenas de caracteres en localizaciones diferentes a ascii...
 * RFC2047
 */
unsigned int
non_ascii_alloc(const char *source, char **encoded)
{
  const unsigned int len1 = strlen(source);
  unsigned int len2 = mbstowcs(NULL, source, 0);
  unsigned int len3 = 0;
  char *tmp = NULL;

  /* El caso más simple y común es cuando 'source' no contiene caracteres
   * diferentes a los ASCII
   */
  if ( len1 == len2 ) {
    /* Este es el caso en donde no hay caracteres a convertir, todos son ASCII*/
    return 0;
  }

  /* Ahora, para hacer las cosas más simples, solo convierte 'source'
   * en una cadena BASE64, y regresalo como una 'encoded-word'
   * junto con los datos de la codificacion deberemos agregar
   * '=?utf-8?B?' y al final '?='
   */
  len2 = BASE64_LENGTH(len1) + 10 + 2 + 1;
  *encoded = (char *)calloc(len2, sizeof(char));
  len3 = base64_encode_alloc(source, len1, &tmp);
  strncat(*encoded, "=?utf-8?B?", 10);
  strncat(*encoded, tmp, len3);
  strncat(*encoded, "?=", 2);
  /*snprintf(*encoded, len2, "=?utf-8?B?%s?=", tmp);*/
  free(tmp);

  len2 = strlen(*encoded);
  return len2;
}
