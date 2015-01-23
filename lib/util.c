/*
 * Copyright (c) 2012-2015, L3a,
 *                          Enrique Gamez Flores <egamez@edisson.com.mx>
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
#ifndef INERE_UTIL_H_
#include "inere/util.h"
#endif

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <iconv.h>

#include "inere/config.h"

unsigned int
ustr_len(const unsigned char* s)
{
  int j = 0;
  if ( s == NULL ) return 0;

  while ( *s ) {
    if ( (*s & 0xc0) != 0x80 ) j++;
    s++;
  }
  return j++;
}

char*
to_upper_case_and_convert(const unsigned char* buffer)
{
  iconv_t desc = NULL;
  char *inbuf;
  char *outbuf;
  char *in = NULL;
  size_t res = 0;
  size_t inbytes = 0;
  size_t outbytes = 0;
  char* out = NULL;
  int j = 0;
  size_t len = 0;

  /* Allocate the descriptor for the character conversion */
  desc = iconv_open("UTF-8//TRANSLIT", INERE_LOCAL_CHARSET);

  /* Check for errors */

  inbytes = strlen((char *)buffer);
  outbytes = 2*inbytes;
  inbuf = (char *)buffer;
  in = (char *)calloc(outbytes, sizeof(char));
  outbuf = in;
  res = iconv(desc, &inbuf, &inbytes, &outbuf, &outbytes);

  /* Close the conversion descriptor */
  iconv_close(desc);

  /* Allocate the memory for the converted string */
  outbytes = strlen(in);
  len = ustr_len((unsigned char *)in);
  out = (char*)calloc(len + 1, sizeof(char));

  /* Start the character conversion */
  while ( *in ) {

    /* The octect indicator for the UTF-8 encoding */
    if ( *in == 0xc3 ) {
      in++;
      if      ( *in == 0xa1 || *in == 0xa0 || *in == 0x81 || *in == 0x80 ) out[j] = 0x41; /* A */
      else if ( *in == 0xa9 || *in == 0xa8 || *in == 0x89 || *in == 0x88 ) out[j] = 0x45; /* E */
      else if ( *in == 0xad || *in == 0xac || *in == 0x8d || *in == 0x8c ) out[j] = 0x49; /* I */
      else if ( *in == 0xb3 || *in == 0xb2 || *in == 0x93 || *in == 0x92 ) out[j] = 0x4f; /* O */
      else if ( *in == 0xba || *in == 0xb9 || *in == 0x9a || *in == 0x99 ) out[j] = 0x55; /* U */
      else if ( *in == 0xb1 || *in == 0x91 ) out[j] = 0x5e; /* ñ or Ñ to ^*/

    } else { /* An ASCII character */
      out[j] = toupper(*in);

    }

    in++;
    j++;
  }
  in -= outbytes;
  free(in);

  return out;
}

unsigned char*
recover_translations(const char* in, unsigned char* out)
{
  int j = 0;
  iconv_t desc;
  char *inbuf;
  char *outbuf;
  char source[] = "Ñ";
  char *enhe = NULL;
  size_t res = 0;
  size_t inbytes = 0;
  size_t outbytes = 0;

  /* Allocate the descriptor for the character conversion */
  desc = iconv_open(INERE_LOCAL_CHARSET, "UTF-8");

  /* Check for errors */

  inbytes = 2; /* The ñ character is represented by two bytes in UTF-8 */
  outbytes = 2*inbytes;
  inbuf = source;
  enhe = (char *)calloc(outbytes, sizeof(char));
  outbuf = enhe;
  res = iconv(desc, &inbuf, &inbytes, &outbuf, &outbytes);
  outbytes = strlen(enhe);

  /* Close the conversion descriptor */
  iconv_close(desc);

  while ( *in ) {
    if ( *in == '^' ) {
      /* Convert the UTF-8 Ñ to the a Ñ in the local charset */
      while ( *enhe ) {
	out[j++] = *enhe++;
      }
    } else {
      out[j++] = *in;
    }
    in++;
  }

  /* Give the result */
  return out;
}
