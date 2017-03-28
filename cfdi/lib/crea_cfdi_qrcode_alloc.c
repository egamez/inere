/*
 * Copyright (c) 2014-2015, L3a,
 *                          Enrique Gamez Flores <egamez@edisson.com.mx>
 * All rights reserved.
 */
/**
 * qrencode - QR Code encoder
 *
 * QR Code encoding tool
 * Copyright (C) 2006, 2007, 2008, 2009 Kentaro Fukuchi <fukuchi@megaui.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*
 * http://stackoverflow.com/questions/1821806/how-to-encode-png-to-buffer-using-libpng
 */
#ifndef INERE_CFDI_CREA_CFDI_QRCODE_ALLOC_H_
#include "inere/cfdi/crea_cfdi_qrcode_alloc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>

void local_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length);
int writePNG(QRcode *qrcode, cfdi_qrcode_t *code);
int termina_cfdi_qrcode(cfdi_qrcode_t *code);

void
local_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  /* with libpng15 next line causes pointer deference error; use libpng12 */
  cfdi_qrcode_t* p = (cfdi_qrcode_t*)png_get_io_ptr(png_ptr); /* was png_ptr->io_ptr */
  size_t nsize = p->size + length;

  /* allocate or grow buffer */
  if(p->buffer) {
    p->buffer = realloc(p->buffer, nsize);
  } else {
    p->buffer = malloc(nsize);
  }

  if( !p->buffer ) {
    png_error(png_ptr, "Write Error");
  }

  /* copy new bytes to end of buffer */
  memcpy(p->buffer + p->size, data, length);
  p->size += length;
}

/**
 *
 */
int
writePNG(QRcode *qrcode, cfdi_qrcode_t *qrcodigo)
{
  png_structp png_ptr;
  png_infop info_ptr;
  unsigned char *row, *p, *q;
  int x, y, xx, yy, bit;
  int realwidth;
  int margin = 4; /* The width margin */
  int size = 2; /* The size of dot (pixel) 3 is the default, let's make it 2 */

  realwidth = (qrcode->width + margin * 2) * size;
  row = (unsigned char *)malloc((realwidth + 7) / 8);
  if (row == NULL) {
    fprintf(stderr, "Failed to allocate memory.\n");
    exit(EXIT_FAILURE);
  }

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fprintf(stderr, "Failed to initialize PNG writer.\n");
    exit(EXIT_FAILURE);
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    fprintf(stderr, "Failed to initialize PNG write.\n");
    exit(EXIT_FAILURE);
  }

  /* if my_png_flush() is not needed, change the arg to NULL */
  /*png_set_write_fn(png_ptr, &qrcodigo, local_png_write_data, local_png_flush);*/
  png_set_write_fn(png_ptr, qrcodigo, local_png_write_data, NULL);

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fprintf(stderr, "Failed to write PNG image.\n");
    exit(EXIT_FAILURE);
  }

  png_set_IHDR(png_ptr, info_ptr,
		realwidth, realwidth,
		1,
		PNG_COLOR_TYPE_GRAY,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_ptr, info_ptr);

  /* top margin */
  memset(row, 0xff, (realwidth + 7) / 8);
  for (y=0; y<margin * size; y++) png_write_row(png_ptr, row);

  /* data */
  p = qrcode->data;
  for (y=0; y<qrcode->width; y++) {
    bit = 7;
    memset(row, 0xff, (realwidth + 7) / 8);
    q = row;
    q += margin * size / 8;
    bit = 7 - (margin * size % 8);
    for (x=0; x<qrcode->width; x++) {
      for (xx=0; xx<size; xx++) {
	*q ^= (*p & 1) << bit;
	bit--;
	if (bit < 0) {
	  q++;
	  bit = 7;
	}
      }
      p++;
    }
    for (yy=0; yy<size; yy++) png_write_row(png_ptr, row);
  }

  /* bottom margin */
  memset(row, 0xff, (realwidth + 7) / 8);
  for (y=0; y<margin * size; y++) png_write_row(png_ptr, row);

  /*png_write_end(png_ptr, info_ptr);*/

  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  png_destroy_write_struct(&png_ptr, &info_ptr);

  free(row);

  return 0;
}

/**
 *
 */
int
termina_cfdi_qrcode(cfdi_qrcode_t *code)
{
  free(code->buffer);
  free(code);
  return 0;
}

/**
 * Función auxiliar para generar el QR code asociado a un CFDi
 *
 * La estructura de la información contenida en el codigo de barras es:
 *
 *	Prefijo		Datos					     Caracteres
 *	re	RFC del Emisor, a 12/13 posiciones, precidor por
 *		el texto "?re="						16/17
 *	rr	RFC del Receptor, a 12/13 posiciones, precedido por
 *		por el texto "&re="					16/17
 *	tt	Total del comprobante a 17 posiciones (10 para los
 *		enteros, 1 para carácter ".", 6 para los decimales),
 *		precedido por el texto "&tt="				21
 *	id	UUID del comprobante, precedido por el texto "&id="	40
 *					Total de caracteres		95
 *
 * Ejemplo:
 *
 *	?re=XAXX010101000&rr=XAXX010101000&tt=1234567890.123456&id=ad662d33-6934-459c-a128-BDf0393f0f44
 *
 */
cfdi_qrcode_t *
crea_cfdi_qrcode_alloc(const char *rfc_emisor, const char *rfc_receptor,
		       const char *total, const char *uuid)
{
  const int version = 6; /* QR code version 6 */
  QRcode *qrcode = NULL;
  char data[96]; /* The data to be encoded needs to be composed in 95 chars */
  double amount = 0;
  static cfdi_qrcode_t *qrcodigo = NULL; /* avoid clobbering by setjmp. */

  /* Convert the total to the corresponding format */
  amount = strtod(total, (char **)0);

  memset(data, 0, 96);
  snprintf(data, 96, "?re=%s&rr=%s&tt=%017.6f&id=%s", rfc_emisor, rfc_receptor, amount, uuid);

  qrcode = QRcode_encodeString8bit(data, version, QR_ECLEVEL_Q);
  if ( qrcode == NULL ) {
    perror("Failed to encode input data.");
    return qrcodigo;
  }

  /* initialise - put this before png_write_png() call */
  qrcodigo = (cfdi_qrcode_t *)malloc(sizeof(cfdi_qrcode_t));
  qrcodigo->buffer = NULL;
  qrcodigo->size = 0;

  writePNG(qrcode, qrcodigo);
  QRcode_free(qrcode);

  return qrcodigo;
}
