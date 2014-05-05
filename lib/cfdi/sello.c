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
/**
 * Instrucciones para remover la contraseña del certificado de llave privada:
 *
 *
 */
#ifndef EDISSON_SELLO_H_INCLUDED
#include "inere/cfdi/sello.h"
#endif
#ifndef BASE64_H
#include "inere/base64.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <openssl/evp.h>
#include <openssl/err.h>

/**
 * Obten el sello digital del comporbante.
 * El usuario es responsable de liberar la memoria del resultado (con free())
 */
char *
sello(const char *keyfile, const char *digest, const unsigned char *cadena, const int verbose)
{
  int read = 0;
  int len = 0;
  unsigned char *buffer = NULL;
  const unsigned char *tmp;
  unsigned char signbuffer[1024];
  unsigned int signlen = 0;
  char *data = NULL;
  FILE *file = NULL;
  BIO* err = NULL;
  EVP_MD_CTX mdctx;
  EVP_PKEY *privateKey = NULL;

  file = fopen(keyfile, "rb");
  if ( file == NULL ) {
    /* An error ocurred */
    if ( verbose ) {
      fprintf(stderr, "No fue posible leer correctamente el archivo %s.\n", keyfile);
    }
    return NULL;
  }
  len = fseek(file, 0, SEEK_END);
  if ( len ) {
    /* An error did occur */
    if ( verbose ) {
      fprintf(stderr, "No fue posible obtener el final del archivo %s.\n", keyfile);
    }
    fclose(file);
    return NULL;
  }
  len = ftell(file);
  rewind(file);


  buffer = (unsigned char *)calloc(len + 1, sizeof(unsigned char));
  read = fread(buffer, sizeof(unsigned char), len, file);
  fclose(file);
  if ( read != len ) {
    if ( verbose ) {
      fprintf(stderr, "An error has ocurred. The number of items read was %d, but it should be %d instead.\n", read, len);
      free(buffer);
    }
    return NULL;
  }

  /* Set the BIO method for the error messages */
  if ( err == NULL ) {
    if ( (err = BIO_new(BIO_s_file())) ) {
      BIO_set_fp(err, stderr, BIO_NOCLOSE|BIO_FP_TEXT);
    }
  }

  /* Now convert the bytes to a EVP_PKEY structure */
  tmp = buffer;
  privateKey = d2i_AutoPrivateKey(NULL, &tmp, len);
  if ( privateKey == NULL ) {
    if ( verbose ) {
      BIO_printf(err, "Error at reading the private key on %s.\n", keyfile);
      ERR_print_errors(err);
    }
    free(buffer);
    return NULL;
  }
  free(buffer);

  /* Add all digest algorithms to the table */
  OpenSSL_add_all_digests();

  /* Initialize the digest context */
  EVP_MD_CTX_init(&mdctx);
  if ( EVP_DigestInit_ex(&mdctx, EVP_get_digestbyname(digest), 0 ) == 0 ) {
    if ( verbose ) {
      BIO_printf(err, "Error at initializing the digest context to use '%s' as digest algorithm.\n", digest);
      ERR_print_errors(err);
    }
    EVP_PKEY_free(privateKey);
    EVP_cleanup();
    BIO_free(err);
    return NULL;
  }

  /* Sign up the data in the current context */
  if ( EVP_SignInit_ex(&mdctx, EVP_get_digestbyname(digest), 0) == 0 ) {
    if ( verbose ) {
      BIO_printf(err, "Error at setting up the signing context to use digest '%s'.\n", digest);
      ERR_print_errors(err);
    }
    EVP_PKEY_free(privateKey);
    EVP_cleanup();
    BIO_free(err);
    return NULL;
  }
  if ( EVP_SignUpdate(&mdctx, cadena, strlen((char *)cadena)) == 0 ) {
    if ( verbose ) {
      BIO_printf(err, "Error hashing the data into the signing context.\n");
      ERR_print_errors(err);
    }
    EVP_PKEY_free(privateKey);
    EVP_cleanup();
    BIO_free(err);
    return NULL;
  }

  signlen = sizeof(signbuffer);
  memset(signbuffer, 0, 1024);
  if ( EVP_SignFinal(&mdctx, signbuffer, (unsigned int* )&signlen, privateKey) == 0 ) {
    if ( verbose ) {
      BIO_printf(err, "Error signing the data in the context with the private key.\n");
      ERR_print_errors(err);
    }
    EVP_PKEY_free(privateKey);
    EVP_cleanup();
    BIO_free(err);
    return NULL;
  }

  EVP_MD_CTX_cleanup(&mdctx);
  EVP_PKEY_free(privateKey);
  EVP_cleanup();
  BIO_free(err);

  /* Now prepare the data to be base64 encoded */
  base64_encode_alloc((const char *)signbuffer, signlen, &data);

  return data;
}
