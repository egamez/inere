/*
 * Copyright (c) 2012, Enrique Gamez Flores <egamez@edisson.com.mx>,
 *                     Lae
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
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifndef INERE_UTIL_INCLUDED_H
#include "inere/util.h"
#endif

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
to_upper_case_and_convert(const unsigned char* in)
{
  char* out = NULL;
  int j = 0;
  size_t len = 0;

  /* Allocate the memory for the converted string */
  len = ustr_len(in);
  out = (char*)calloc(len + 1, sizeof(char));
  memset(out, 0, len + 1);

  /* Start the convertion */
  /* Start the convertion */
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
    /* For the ISO-8859-* encoding system */
    } else if ( *in == 0xc0 || *in == 0xc1 || *in == 0xe0 || *in == 0xe1 ) {
      out[j] = 0x41; /* A */

    } else if ( *in == 0xc8 || *in == 0xc9 || *in == 0xe8 || *in == 0xe9 ) {
      out[j] = 0x45; /* E */

    } else if ( *in == 0xcc || *in == 0xcd || *in == 0xec || *in == 0xed ) {
      out[j] = 0x49; /* I */

    } else if ( *in == 0xd2 || *in == 0xd3 || *in == 0xf2 || *in == 0xf3 ) {
      out[j] = 0x4f; /* O */

    } else if ( *in == 0xd9 || *in == 0xda || *in == 0xf9 || *in == 0xfa ) {
      out[j] = 0x55; /* U */

    } else if ( *in == 0xd1 || *in == 0xc1 ) {
      out[j] = 0x41; /* enhe */

    } else {
      out[j] = toupper(*in);
    }
    in++;
    j++;
  }
  return out;
}

unsigned char*
recover_translations(const char* in, unsigned char* out)
{
  int j = 0;
  while ( *in ) {
    if ( *in == '^' ) {
      out[j++] = 0xc3;
      out[j++] = 0x91;
    } else {
      out[j++] = *in;
    }
    in++;
  }
  return out;
}
