/*
 * Copyright (c) 2014-2017, L3a,
 *                          Enrique Gámez Flores <egamez@edisson.com.mx>
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
 * IMPORTANTE. Esta funcion solo podra firmar el hash la cadena con una
 * llave privada sin password y en formato PEM
 *
 * Instrucciones para remover la contraseña de la llave privada:
 *	openssl pkcs8 -inform DER -in certificado-de-llave-privada.key -passin pass:AQUI-VA-EL-PASSWORD -out certificado-de-llave-privada.key.pem
 *
 */
#ifndef INERE_CFDI_SELLO_H_
#include "inere/cfdi/sello.h"
#endif
#ifndef BASE64_H
#include "inere/cfdi/base64.h"
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

/**
 * Esta función se encargará de producir, lo que para el SAT[1], es un sello
 * digital. Este sello digital consiste en el firmado digital del hash de un
 * string, que para los casos de un CFDi se trataría de la cadena original
 *
 * El usuario es responsable de liberar la memoria del resultado (con free())
 * [1]: http://www.sat.gob.mx
 */
unsigned char *
sello_alloc(const char *keyfile, const char *digest, const unsigned char *cadena, const int verbose)
{
  int read = 0;
  int len = 0;
  unsigned char *buffer = NULL;
  unsigned char signbuffer[1024];
  unsigned int signlen = 0;
  char *data = NULL;
  FILE *file = NULL;
  BIO *key_bio = NULL;
  BIO* err = NULL;
  EVP_MD_CTX mdctx;
  EVP_PKEY *privateKey = NULL;

  /* Set the BIO method for the error messages */
  if ( err == NULL ) {
    if ( (err = BIO_new(BIO_s_file())) ) {
      BIO_set_fp(err, stderr, BIO_NOCLOSE|BIO_FP_TEXT);
    }
  }

  /* Load the file where private key is, to sign the data */
  key_bio = BIO_new(BIO_s_file());
  if ( BIO_read_filename(key_bio, keyfile) <= 0 ) {
    BIO_printf(err, "Error opening %s\n", keyfile);
    ERR_print_errors(err);
    BIO_free(err);
    return NULL;
  }

  privateKey = PEM_read_bio_PrivateKey(key_bio, NULL, NULL, NULL);
  if ( privateKey == NULL ) {
    if ( verbose ) {
      BIO_printf(err, "Error at reading the private key on %s.\n", keyfile);
      ERR_print_errors(err);
    }
    BIO_free(err);
    BIO_free(key_bio);
    return NULL;
  }

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
    BIO_free(key_bio);
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
    BIO_free(key_bio);
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
    BIO_free(key_bio);
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
    BIO_free(key_bio);
    return NULL;
  }

  EVP_MD_CTX_cleanup(&mdctx);
  EVP_PKEY_free(privateKey);
  EVP_cleanup();
  BIO_free(err);
  BIO_free(key_bio);

  /* Now prepare the data to be base64 encoded */
  base64_encode_alloc((const char *)signbuffer, signlen, &data);

  return (unsigned char *)data;
}
