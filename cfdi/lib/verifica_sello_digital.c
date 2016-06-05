/*
 * Copyright (c) 2013-2015 L3a,
 * 			   Enrique Gamez Flores <egamez@edisson.com.mx>
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
#ifndef INERE_CFDI_VERIFICA_SELLO_DIGITAL_H_
#include "inere/cfdi/verifica_sello_digital.h"
#endif
#ifndef INERE_CFDI_GENERA_CADENA_ORIGINAL_H_
#include "inere/cfdi/genera_cadena_original.h"
#endif
#ifndef BASE64_H
#include "inere/cfdi/base64.h"
#endif

#include <string.h>
#include <unistd.h>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>

/* Verifica sello digital */
xmlChar *convert_to_pem(const xmlChar *c, xmlChar *res, int verbose);
EVP_PKEY *extract_pkey(const xmlChar* certificado, BIO *bio_err, int verbose);
int valida_sello(const xmlChar *cadena, const xmlChar *sello, const int sello_len, EVP_PKEY* pkey, const char *md_algo, BIO *bio_err, int verbose);


xmlChar *
convert_to_pem(const xmlChar *c, xmlChar *res, int verbose)
{
  const xmlChar *begin = (const xmlChar *)"-----BEGIN CERTIFICATE-----\n";
  const xmlChar *end = (const xmlChar *)"\n-----END CERTIFICATE-----\n";
  const size_t len = xmlStrlen(c) + xmlStrlen(begin) + xmlStrlen(end) + 1 + 40;
  int i = 0;
  int j = 0;

  res = (xmlChar *)calloc(len, sizeof(xmlChar));
  memset(res, 0, len);

  bcopy(begin, res, xmlStrlen(begin));
  res += xmlStrlen(begin);
  j += xmlStrlen(begin);

  while ( *c ) {
    *res++ = *c++;
    j++;
    if ( !(++i % 64) ) {
      *res++ = '\n';
      j++;
    }
  }
  res -= j;

  xmlStrncat(res, end, 27);

  if ( verbose ) {
    printf("convert_to_pem: Certificado de sello digital, contenido en el comprabante, en formato PEM:\n%s", res);
  }
  return res;
}

/**
 * Extrae la llave pública (public key) del certificado de sello digital
 * que el ampara el comprobante a verificar.
 *
 * La función regresa un puntero de una estructura EVP_PKEY. La memoría
 * para alojar dicha estructura deberá ser liberada por el usuario.
 */
EVP_PKEY *
extract_pkey(const xmlChar* certificado, BIO *bio_err, int verbose)
{
  int result = 0;

  xmlChar *pem_cert = NULL;

  BIO *in = NULL;
  X509 *cert = NULL;
  EVP_PKEY* pkey = NULL;

  /* Now, compose the certificate to a usable PEM format */
  pem_cert = convert_to_pem(certificado, pem_cert, verbose);
  if ( pem_cert == NULL ) {
    BIO_printf(bio_err, "extract_pkey: No fue posible codificar el certificado de sello digital, incluido en el comprobrante, al format PEM.\n");
    return NULL;
  }

  /* Extract the public key from the certificate */
  in = BIO_new(BIO_s_mem());
  if ( in == NULL ) {
    BIO_printf(bio_err, "extract_pkey: No fue posible crear un nuevo BIO.\n");
    ERR_print_errors(bio_err);
    free(pem_cert);
    return NULL;
  }

  result = BIO_puts(in, (char *)pem_cert);
  if ( result == 0 ) {
    BIO_printf(bio_err, "extract_pkey: No fue posible escribir el certificado de sello digital al BIO.\n");
    ERR_print_errors(bio_err);
    BIO_free(in);
    free(pem_cert);
    return NULL;
  }

  cert = PEM_read_bio_X509(in, NULL, 0, NULL);
  if ( cert == NULL ) {
    BIO_printf(bio_err, "extract_pkey: No fue posible analizar el certificado de sello digital.\n");
    ERR_print_errors(bio_err);
    BIO_free(in);
    free(pem_cert);
    return NULL;
  }

  pkey = X509_get_pubkey(cert);
  if ( pkey == NULL ) {
    BIO_printf(bio_err, "extract_pkey: Unable to load the public key from the certificate.\n");
    ERR_print_errors(bio_err);
  }

  if ( verbose ) {
    printf("extract_pkey: Llave publica contenida en el certificado de sello digital en este comprobante fiscal:\n");
    PEM_write_PUBKEY(stdout, pkey);
  }

  X509_free(cert);
  BIO_free(in);
  free(pem_cert);

  return pkey;
}

/**
 * Verifica que el sello digital, contenido en el comprobante, corresponda
 * a la información del comprobante mismo.
 *
 * La función regresa:
 *
 *	1	En caso de que la verificación halla sido existosa, y el
 *		sello digital contenido en el comprobante, coincida con la
 *		la información que ampara dicho comprobante.
 *	0	En caso de error.
 */
int
valida_sello(const xmlChar *cadena, const xmlChar *sello, const int sello_len, EVP_PKEY* pkey, const char *md_algo, BIO *bio_err, int verbose)
{
  int result = 1;

  EVP_MD_CTX ctx;
  const EVP_MD *md;

  OpenSSL_add_all_digests();
  md = EVP_get_digestbyname(md_algo);
  EVP_MD_CTX_init(&ctx);
  EVP_DigestInit_ex(&ctx, md, NULL);

  result = EVP_VerifyInit(&ctx, md);
  if ( result < 0 ) {
    BIO_printf(bio_err, "valida_sello: EVP_VerifyInit error. Value: %d.\n", result);
    ERR_print_errors(bio_err);
  }
  result = EVP_VerifyUpdate(&ctx, cadena, xmlStrlen(cadena));
  if ( result < 1 ) {
    BIO_printf(bio_err, "valida_sello: EVP_VerifyUpdate error.\n");
    ERR_print_errors(bio_err);
  }

  result = EVP_VerifyFinal(&ctx, (const unsigned char*)sello, sello_len, pkey);
  if ( result == 0 ) {
    BIO_printf(bio_err, "valida_sello: El sello digital en el comprobante NO coincide con la información del comprobante. Por lo tanto el comprabante NO es válido según este test.\n");
    if ( verbose ) ERR_print_errors(bio_err);

  } else if ( result == -1 ) {
    BIO_printf(bio_err, "valida_sello: A ocurrido un error en la verificación del sello del comprobante digital.\n");
    ERR_print_errors(bio_err);

  } else {
    if ( verbose ) {
      printf("valida_sello: El sello digital, contenido en este comprobante, coincide con la información del comprobante mismo (según este test.)\n");
    }
  }
  EVP_MD_CTX_cleanup(&ctx);

  return result;
}

/**
 * Verifica el sello digital contenido en un CFDi
 *
 * El proceso de verificación del sello digital, contenido en un CFDi,
 * consistira en:
 *
 *	- Recuperar el certificado de llave publica (contenido en el CFDi)
 *	- Recuperar el sello digital (también contenido en el CFDi)
 *	- Generar la cadena original a partir del propio CFDi
 *	- Verifica que la firma.
 *
 * La función regresa:
 *
 *	0	En caso de que el sello corresponde fielmente con el CFDi
 *	1	En caso de que el sello no coincida con la información del CFDi
 *	2	Si no fue posible leer el CFDi
 *	3	Si el CFDi no contiene información
 *	4	Si el CFDi no contiene la versión
 *	5	Si el CFDi no tiene el certificado (de llave pública)
 *	6	Si el CFDi no contiene el sello digital
 *	7	Si el certificado (de llave pública) contenido en el CFDi no
 *		puede ser utilizable para la verificación
 *	8	No fue posible decodificar (base64) el sello para realizar la
 *		verificación
 *	9	No fue posible obtener la cadena original correspondiente al
 *		CFDi
 *
 */
int
verifica_sello_digital(const char *filename, const char *stylesheet, const int verbose)
{
  int result = 0;
  xmlNodePtr node = NULL;
  xmlChar *certificado = NULL;
  xmlChar *sello = NULL;
  xmlChar *cadena[2048];
  xmlNodePtr cfd = NULL;
  xmlDocPtr doc = NULL;
  char *seal = NULL;
  xmlChar *version = NULL;
  size_t seal_len = 0;
  const char *md_algo;

  BIO *bio_err = NULL;
  EVP_PKEY *pkey = NULL;

  /* Hay dos posibilidades:
   *
   *    - Una es que hallas pasado el nombre del archivo que contiene el
   *      el CFDi, la otra es
   *    - Que hallas pasado el CFDi mismo en un string
   */
  if ( access(filename, R_OK) == 0 ) {
    /* El argumento dado corresponde a la ruta del archivo que contiene el
     * CFDI*/
    doc = xmlParseFile(filename);
  } else {
    /* El string dado contiene el CFDi mismo */
    doc = xmlReadMemory(filename, strlen(filename), "http://www.sat.gob.mx/cfd/3", "UTF-8", XML_PARSE_RECOVER);
  }

  if ( doc == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "verifica_sello_digital: No fue posible leer el CFDi (%s). No es posible realizar el test.\n", filename);
    }
    return 2;
  }

  cfd = xmlDocGetRootElement(doc);
  if ( cfd == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "verifica_sello_digital: Archivo (%s) sin contenido. No es posible realizar el test.\n", filename);
    }
    xmlFreeDoc(doc);
    return 3;
  }

  /* Extract the "certificado" */
  for ( node = cfd; node; node = node->next ) {

    if ( node->type == XML_ELEMENT_NODE ) {

      if ( xmlStrcmp(node->name, (const xmlChar *)"Comprobante") == 0 ) {

	version = xmlGetProp(node, (const xmlChar *)"version");
	if ( version == NULL ) {
	  if ( verbose ) {
	    fprintf(stderr, "verifica_sello_digital: No se encotró la versión del comprobante digital. Imposible realizar la verificación.\n");
	  }
	  return 4;
	}

	certificado = xmlGetProp(node, (const xmlChar *)"certificado");
	if ( certificado == NULL ) {
	  if ( verbose ) {
	    fprintf(stderr, "verifica_sello_digital: No se encontró el certificado de sello digital que ampara el CFD, o posiblemente este no sea un CFD. Imposible realizar la verificación.\n");
	  }
	  return 5;
	}
	sello = xmlGetProp(node, (const xmlChar *)"sello");
	if ( sello == NULL ) {
	  if ( verbose ) {
	    fprintf(stderr, "verifica_sello_digital: No se encontró el sello digital del comprobante fiscal. Imposible realizar la verificación.\n");
	  }
	  return 6;
	}

	break;
      }
    }
  }

  bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);

  if ( xmlStrcmp(version, (const xmlChar *)"2.0") ) {
    md_algo = "sha1";
  } else {
    md_algo = "md5";
  }
  if ( verbose ) printf("verifica_sello_digital: Utilizando el algoritmo %s para resumir la cadena original.\n", md_algo);

  if ( verbose ) {
    printf("verifica_sello_digital: Sello digital en el comprobante:\n%s\nCertificado de sello digital en el comprobante:\n%s\n", sello, certificado);
  }

  /* Convierte el certificado de llave pública a un formato usable */
  pkey = extract_pkey(certificado, bio_err, verbose);
  if ( pkey == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "No se pudo obtener la clave de llave publica, del certificado que ampara el comprobrante fiscal. No es posible realizar la verificación.\n");
    }
    BIO_free_all(bio_err);
    xmlFreeDoc(doc);
    return 7;
  }

  /* Now decode the sello digital */
  base64_decode_alloc((const char *)sello, xmlStrlen(sello), &seal, &seal_len);
  if ( seal == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "No fue posible decodificar el sello digital del comprobante fiscal. No es posible realizar la verificación.\n");
    }
    EVP_PKEY_free(pkey);
    BIO_free_all(bio_err);
    xmlFreeDoc(doc);
    return 8;
  }

  /* Build the cadena oridigal */
  genera_cadena_original(stylesheet, doc, cadena, verbose);
  if ( *cadena == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "No fue posible formar la cadena original del comprobante fiscal. No es posible realizar la verificación.\n");
    }
    EVP_PKEY_free(pkey);
    BIO_free_all(bio_err);
    xmlFreeDoc(doc);
    return 9;
  }

  /* Now verify the signature. */
  result = valida_sello(*cadena, (const xmlChar *)seal, seal_len, pkey, md_algo, bio_err, verbose);
  /* Now convert the return result */
  if ( result == 1 ) {
    result = 0;
  } else {
    /* El sello digital contenido en el CFDi no coincide con la informacion */
    result = 1;
  }

  EVP_PKEY_free(pkey);

  BIO_free_all(bio_err);
  xmlFree(seal);
  xmlFree(version);
  xmlFree(sello);
  xmlFree(certificado);

  xmlFreeDoc(doc);

  return result;
}
