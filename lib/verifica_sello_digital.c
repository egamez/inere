/*
 * Copyright (c) 2013,2014 L3a,
 * 			   Enrique Gamez Flores <egamez@edisson.com.mx>,
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
#include "inere/verifica_sello_digital.h"

#include <string.h>

#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>

xmlChar*
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
 * Funcion para suprimir los mensajes de error generados por peticiones
 * de compilacion de instrucciones para version 2.0 de xsl
 */
void
local_error_function(void *ctx, const char* mess, ...)
{
   char *errMsg;
   va_list args;
   va_start(args, mess);
   vasprintf(&errMsg, mess, args);
   va_end(args);
   if ( !strncmp("compilation error", errMsg, 17) &&
	!strncmp("xsl:version: only 1.0 features are supported", errMsg, 44) ) {
     fprintf(stderr, "%s", errMsg);
   }
   free(errMsg);
}

/**
 * Extrae la cadena original del comprobante fiscal
 *
 * La funcion regresa:
 *
 *	0	En caso de generar la cadena original exitosamente,
 *
 * y en caso de error:
 *
 *	1	Cuando la stylsheet, proporcionada para generar la cadena
 *		original no pudo ser compilada.
 *	2	Cuando las transformaciones, definidas en la stylesheet
 *		indicada no pudieron aplicarse al CFDi.
 *	3	No fue posible escribir la cadena original a un buffer
 *
 */
int
cadena_original(const xmlChar *stylesheet, xmlDocPtr doc, xmlChar** cadena, int verbose)
{
  xsltStylesheetPtr style = NULL;
  xmlDocPtr result = NULL;
  int cadena_len = 0;
  int out = 0;

  xmlSubstituteEntitiesDefault(1);
  xmlLoadExtDtdDefaultValue = 1;

  xsltSetGenericErrorFunc(stderr, local_error_function);

  style = xsltParseStylesheetFile(stylesheet);
  if ( style == NULL ) {
    /*fprintf(stderr, "cadena_original: Stylesheet (%s) no analizada.\n", stylesheet);*/
    xsltCleanupGlobals();
    return 1;
  }

  result = xsltApplyStylesheet(style, doc, NULL);
  if ( result == NULL ) {
    /*fprintf(stderr, "cadena_original: Transformaciones de stylesheet (%s) no aplicadas.\n", stylesheet);*/
    xsltFreeStylesheet(style);
    xsltCleanupGlobals();
    return 2;
  }

  out = xsltSaveResultToString(cadena, &cadena_len, result, style);
  if ( out == -1 ) {
    /*fprintf(stderr, "cadena_original: Error al salvar la cadena original en el buffer.\n");*/
    return 3;
  }

  xsltFreeStylesheet(style);
  xmlFreeDoc(result);

  if ( verbose ) {
    printf("cadena_original: Cadena original de la información del comprobante:\n%s\n", *cadena);
  }

  xsltCleanupGlobals();
}

/**
 * Extrae la llave pública (public key) del certificado de sello digital
 * que el ampara el comprobante a verificar.
 *
 * La función regresa un puntero de una estructura EVP_PKEY. La memoría
 * para alojar dicha estructura deberá ser liberada por el usuario.
 */
EVP_PKEY*
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
 * Decodifica el sello digital. El sello digital se encuentra codificado
 * en base 64, dentro del comprobante fiscal digital.
 *
 * La memoría utilizada para alojar el sello digital, deberá ser liberada por
 * el usuario.
 */
xmlChar*
decode_seal(const xmlChar* sello, int *len, BIO *bio_err, int verbose)
{
  const int length = xmlStrlen(sello);

  xmlChar *seal = NULL;

  BIO *b64 = NULL;
  BIO *in = NULL;

  b64 = BIO_new(BIO_f_base64());
  BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

  in = BIO_new_mem_buf((char *)sello, length);
  if ( in == NULL ) {
    BIO_printf(bio_err, "decode_seal: No fue posible crear memoría BIO para alojar el sello digital del comprobante.\n");
    ERR_print_errors(bio_err);
    BIO_free(b64);
    return NULL;
  }

  in = BIO_push(b64, in);
  seal = xmlStrndup(sello, length);
  memset(seal, 0, length);

  *len = BIO_read(in, seal, length);
  if ( *len == 0 ) {
    BIO_printf(bio_err, "decode_seal: Data wasn't succesfully read into a BIO buffer.\n");
    ERR_print_errors(bio_err);
    BIO_free_all(in);
    return NULL;
  }

  if ( verbose ) {
    int i = 0;
    printf("decode_seal: %d leídos.\n", *len);
    printf("decode_seal: Sello digital:\n");
    for (i = 0; i < *len; i++) {
      printf("\\x%0x", *seal++);
    }
    seal -= i;
    printf("\n");
  }

  BIO_free_all(in);

  return seal;
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
  int result = 0;

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

int
verifica_sello_digital(const char *filename, const xmlChar *stylesheet, const int verbose)
{
  int result = 0;
  xmlNodePtr node = NULL;
  xmlChar *certificado = NULL;
  xmlChar *sello = NULL;
  xmlChar *cadena[2048];
  xmlNodePtr cfd = NULL;
  xmlDocPtr doc = NULL;
  xmlChar *seal = NULL;
  xmlChar *version = NULL;
  int seal_len = 0;
  const char *md_algo;

  BIO *bio_err = NULL;
  EVP_PKEY *pkey = NULL;

  doc = xmlReadFile(filename, "UTF-8", XML_PARSE_NOENT);
  if ( doc == NULL ) {
    fprintf(stderr, "verifica_sello_digital: El comprobante no pudo ser analizado. No es posible realizar el test.\n");
    return 6;
  }

  cfd = xmlDocGetRootElement(doc);
  if ( cfd == NULL ) {
    fprintf(stderr, "verifica_sello_digital: Archivo (%s) sin contenido. No es posible realizar el test.\n", filename);
    xmlFreeDoc(doc);
    return 7;
  }

  /* Extract the "certificado" */
  for ( node = cfd; node; node = node->next ) {

    if ( node->type == XML_ELEMENT_NODE ) {

      if ( xmlStrcmp(node->name, (const xmlChar *)"Comprobante") == 0 ) {

	version = xmlGetProp(node, (const xmlChar *)"version");
	if ( version == NULL ) {
	  fprintf(stderr, "verifica_sello_digital: No se encotró la versión del comprobante digital. Imposible realizar la verificación.\n");
	  return 8;
	}

	certificado = xmlGetProp(node, (const xmlChar *)"certificado");
	if ( certificado == NULL ) {
	  fprintf(stderr, "verifica_sello_digital: No se encontró el certificado de sello digital que ampara el CFD, o posiblemente este no sea un CFD. Imposible realizar la verificación.\n");
	  return 1;
	}
	sello = xmlGetProp(node, (const xmlChar *)"sello");
	if ( sello == NULL ) {
	  fprintf(stderr, "verifica_sello_digital: No se encontró el sello digital del comprobante fiscal. Imposible realizar la verificación.\n");
	  return 2;
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

  /* Extract from the certificate its public key */
  pkey = extract_pkey(certificado, bio_err, verbose);
  if ( pkey == NULL ) {
    fprintf(stderr, "No se pudo obtener la clave de llave publica, del certificado que ampara el comprobrante fiscal. No es posible realizar la verificación.\n");
    BIO_free_all(bio_err);
    xmlFreeDoc(doc);
    return 3;
  }

  /* Now decode the sello digital */
  seal = decode_seal(sello, &seal_len, bio_err, verbose);
  if ( seal == NULL ) {
    fprintf(stderr, "No fue posible decodificar el sello digital del comprobante fiscal. No es posible realizar la verificación.\n");
    EVP_PKEY_free(pkey);
    BIO_free_all(bio_err);
    xmlFreeDoc(doc);
    return 4;
  }

  /* Build the cadena oridigal */
  cadena_original(stylesheet, doc, cadena, verbose);
  if ( *cadena == NULL ) {
    fprintf(stderr, "No fue posible formar la cadena original del comprobante fiscal. No es posible realizar la verificación.\n");
    EVP_PKEY_free(pkey);
    BIO_free_all(bio_err);
    xmlFreeDoc(doc);
    return 5;
  }

  /* Now verify the signature. */
  result = valida_sello(*cadena, seal, seal_len, pkey, md_algo, bio_err, verbose);

  EVP_PKEY_free(pkey);

  BIO_free_all(bio_err);
  xmlFree(seal);
  xmlFree(version);
  xmlFree(sello);
  xmlFree(certificado);

  xmlFreeDoc(doc);

  return result;
}
