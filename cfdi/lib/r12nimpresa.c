/*
 * Copyright (c) 2015, L3a,
 *		       Enrique Gamez Flores <egamez@edisson.com.mx>
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
#ifndef INERE_INVOICE_R12NIMPRESA_H_
#include "inere/cfdi/r12nimpresa.h"
#endif

#include "inere/cfdi/config.h"

#ifndef INERE_CFDI_CREA_CFDI_QRCODE_ALLOC_H_
#include "inere/cfdi/crea_cfdi_qrcode_alloc.h"
#endif
#ifndef INERE_CFDI_LEE_CFDI_H_
#include <inere/cfdi/lee_cfdi.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <locale.h>
#include <monetary.h>
#include <time.h>

#include <hpdf.h>


jmp_buf env;
const unsigned int font_size_label = 8;
const unsigned int font_size       = 8;

const char *info_author          = "EDISSON Equipo Electrico";
const char *info_creator         =  "EDISSON - TPV - Version: 0.4";

const char *folio_fiscal_label   = "Folio Fiscal:";
const char *no_serie_csd_label   = "No. de serie del CSD:";
const char *fecha_emision_label  = "Lugar, fecha y hora de emisión:";
const char *serie_label          = "Serie:";
const char *folio_label          = "Folio:";

const char *cantidad_label       = "Cantidad";
const char *clave_label          = "Clave";
const char *unidad_label         = "Unidad";
const char *descripcion_label    = "Descripción";
const char *valor_unitario_label = "Valor unitario";
const char *importe_label        = "Importe";


/* Forward declarations */
void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no,void *user_data);
HPDF_STATUS fija_opciones_del_documento(HPDF_Doc pdf, Comprobante_t* cfdi);
HPDF_Point imprime_conceptos(HPDF_Page page, const HPDF_REAL margin,
			     HPDF_Point *point, HPDF_Font font,
			      HPDF_Font font_bold, Comprobante_t* cfdi,
			      const int verbose);
HPDF_UINT write_in_a_box(HPDF_Page page, HPDF_REAL x, HPDF_REAL y,
			 HPDF_REAL width, char *data);



/**
 * Error function handler.
 */
void
error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
  printf("function call: %s\n", (char *)user_data);
  fprintf(stderr, "ERROR: error_no=0x%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
  longjmp(env, 1);
}


/**
 * Funcion auxiliar para fijar algunas opciones del documento
 */
HPDF_STATUS
fija_opciones_del_documento(HPDF_Doc pdf, Comprobante_t* cfdi)
{
  struct tm* moddate = 0;
  time_t now;
  char buffer[256];

  HPDF_Date creation_date;
  HPDF_STATUS res = HPDF_OK;

  /* Habilita el uso de la codificación UTF-8 */
  res = HPDF_UseUTFEncodings(pdf);

  /* Define algunos atributos informativos */
  res = HPDF_SetInfoAttr(pdf, HPDF_INFO_AUTHOR, info_author);
  res = HPDF_SetInfoAttr(pdf, HPDF_INFO_CREATOR, info_creator);

  /* El atributo Titulo del PDF */
  memset(buffer, 0, 256);
  snprintf(buffer, 49, "CFDi - Representacion Impresa para %s", cfdi->Receptor->rfc);
  res = HPDF_SetInfoAttr(pdf, HPDF_INFO_TITLE, buffer);

  /* El asunto del PDF */
  memset(buffer, 0, 256);
  snprintf(buffer, 60, "Compra - Folio Fiscal: %s",
				cfdi->Complemento->TimbreFiscalDigital->UUID);
  res = HPDF_SetInfoAttr(pdf, HPDF_INFO_SUBJECT, buffer);

  /* Algunas palabras claves */
  memset(buffer, 0, 256);
  snprintf(buffer, 56, "EDISSON, %s, %s, %s, CFDi",
				  cfdi->Emisor->rfc,
				  cfdi->Receptor->rfc,
				  cfdi->Complemento->TimbreFiscalDigital->UUID);
  res = HPDF_SetInfoAttr(pdf, HPDF_INFO_KEYWORDS, buffer);

  /* Fija opciones para que esta representacion impresa no pueda
   * ser modificada, el password del documento sera la clave del R.F.C.
   * del receptor
   */
  res = HPDF_SetPassword(pdf, (const char *)cfdi->Receptor->rfc, NULL);
  res = HPDF_SetEncryptionMode(pdf, HPDF_ENCRYPT_R3, 16);
  res = HPDF_SetPermission(pdf, HPDF_ENABLE_READ | HPDF_ENABLE_PRINT | HPDF_ENABLE_COPY );

  /* Define la fecha de creación de esta representación impresa
   * para este atributo utilizaremos la fecha de emisión del CFDi
   */
  sscanf((const char *)cfdi->fecha, "%d-%d-%dT%d:%d:%d",&creation_date.year,
							&creation_date.month,
							&creation_date.day,
							&creation_date.hour,
							&creation_date.minutes,
							&creation_date.seconds);
  creation_date.ind = '+';
  creation_date.off_hour = 6; /* CDT */
  creation_date.off_minutes = 0;
  res = HPDF_SetInfoDateAttr(pdf, HPDF_INFO_CREATION_DATE, creation_date);

  /* Ahora define la fecha de modificación, de ésta representación
   * impresa. Para esta fecha se tomará la fecha actual
   */
  now = time(&now);
  moddate = (struct tm *)calloc(1, sizeof(struct tm));
  localtime_r(&now, moddate);
  creation_date.year = moddate->tm_year;
  creation_date.month = moddate->tm_mon + 1;
  creation_date.day = moddate->tm_mday;
  creation_date.hour = moddate->tm_hour;
  creation_date.minutes = moddate->tm_min;
  creation_date.seconds = moddate->tm_sec;
  res = HPDF_SetInfoDateAttr(pdf, HPDF_INFO_MOD_DATE, creation_date);
  free(moddate);

  /* El modo de compresión */
  res = HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);

  return res;
}

/**
 * Esta funcion es para escribir dentro de una caja una cadena de varios
 * caracteres. Esta funcion es util para escribir la sequencias que
 * corresponden a la representacion del sello digital y algunas otras
 * cosas sin formato intrinseco.
 *
 * Write onto the given window at left = x, top = y and width 'data'
 * and return the number of lines used.
 */
HPDF_UINT
write_in_a_box(HPDF_Page page, HPDF_REAL x, HPDF_REAL y, HPDF_REAL width,
		char *data)
{
  HPDF_UINT text_len = 0;
  HPDF_REAL real_width = 0;
  HPDF_UINT lines = 0;
  char *buffer = 0;

  if ( data == NULL ) {
    printf("write_in_a_box: Nothing to write.\n");
    return 0;
  }

  /* Now we need to make a substrings to write down the 'data' */
  while ( *data ) {

    /* Calculate the length */
    text_len = HPDF_Page_MeasureText(page, data, width, HPDF_FALSE,&real_width);

    buffer = (char *)calloc(text_len + 1, sizeof(char));
    memset(buffer, 0, text_len + 1);
    buffer = strncpy(buffer, data, text_len);

    /* Write the text */
    HPDF_Page_TextOut(page, x, y, buffer);

    /* Move one line down */
    y -= 10;
    /* Acknowledge the increment of a line */
    lines++;

    /* And now, reset the variables */
    data += text_len;
    free(buffer);

  }

  return lines;

}


/**
 * Esta función es para imprimir los conceptos del CFDi.
 * La función regresara la posicion en la que se dibujo la ultima linea
 * que corresponde a la tabla de conceptos.

 */
HPDF_Point
imprime_conceptos(HPDF_Page page, const HPDF_REAL margin, HPDF_Point *point,
		  HPDF_Font font, HPDF_Font font_bold, Comprobante_t* cfdi,
		  const int verbose)
{
  const float ndigits = 10;
  int tiene_noIdent = 0;
  char buffer[14];
  HPDF_REAL top_y = 0;
  HPDF_UINT len = 0;
  HPDF_Rect rect_cant;
  HPDF_Rect rect_ident;
  HPDF_Rect rect_unidad;
  HPDF_Rect rect_desc;
  HPDF_Rect rect_precio;
  HPDF_Rect rect_importe;
  HPDF_REAL width_cant = 0;
  HPDF_REAL width_ident = 0;
  HPDF_REAL width_unidad = 0;
  HPDF_REAL width_desc = 0;
  HPDF_REAL width_precio = 0;
  HPDF_REAL width_importe = 0;
  HPDF_REAL top = 0;
  HPDF_REAL bottom = 0;
  const HPDF_REAL page_width = HPDF_Page_GetWidth(page);
  const HPDF_REAL page_height = HPDF_Page_GetHeight(page);
  Concepto_list_t *conceptos = NULL;

  top_y = 150;

  if ( verbose ) {
    printf("%s:%d Dibujando la linea superior de los conceptos.\n", __FILE__, __LINE__); 
  }

  /* Ahora, necesitamos de dibujar una linea que sirva de encabezado
   * para los conceptos que el CFDi ampara, debemos cambiar a Path painting
   * mode
   */
  printf("0'. sacatelas babuchas margin = %f, page_height = %f, top_y = %f, y = %f\n", margin, page_height, top_y, page_height - top_y);
  HPDF_Page_SetLineWidth(page, 1);
  HPDF_Page_MoveTo(page, margin, page_height - top_y);

  /* Dibuja una linea */
  HPDF_Page_LineTo(page, page_width - margin, page_height - top_y);

  if ( verbose ) {
    printf("%s:%d Info. Ahora se escribira la informacion de los conceptos 2.\n", __FILE__, __LINE__);
  }

  *point = HPDF_Page_GetCurrentPos(page);
  printf("1. Current position: y = %f, x = %f\n", point->y, point->x);

  /* Print the current path */
  HPDF_Page_Stroke(page);

  HPDF_Page_SetFontAndSize(page, font_bold, 8);
  HPDF_Page_BeginText(page);

  /*
   * Imprime los encabezados
   */

  /* Cantidad */
  rect_cant.top    = point->y;
  rect_cant.bottom = point->y - 10;
  rect_cant.left   = margin;
  width_cant       = HPDF_Page_TextWidth(page, "8") * ndigits;
  rect_cant.right  = margin + width_cant + 2;
  HPDF_Page_TextRect(page,
			rect_cant.left,
			rect_cant.top,
			rect_cant.right,
			rect_cant.bottom,
			cantidad_label,
			HPDF_TALIGN_CENTER,
			&len);
  if ( verbose ) {
    printf("%s:%d Info. %d caracteres impresos correspondientes a \"%s\"\n", __FILE__, __LINE__, len, cantidad_label);
  }

  /* Ahora el noIdentificacion. Para este apartado, deberemos verificar
   * si los conceptos que ampara el CFDi lo requieren
   */
  conceptos = cfdi->Conceptos;
  while ( conceptos != NULL ) {
    if ( conceptos->noIdentificacion != NULL ) {
      tiene_noIdent = 1;
    }
    conceptos = conceptos->next;
  }

  if ( tiene_noIdent ) {
    /* Hay numeros de identificacion de al menos uno de los conceptos */
    rect_ident.top    = point->y;
    rect_ident.bottom = point->y - 10;
    rect_ident.left   = rect_cant.right;
    width_ident       = HPDF_Page_TextWidth(page, "AAAAAAAAAAAAAAAAAAAA");
    rect_ident.right  = rect_ident.left + width_ident + 2;
    HPDF_Page_TextRect(page,
			rect_ident.left,
			rect_ident.top,
			rect_ident.right,
			rect_ident.bottom,
			clave_label,
			HPDF_TALIGN_CENTER,
			&len);
    if ( verbose ) {
      printf("%s:%d Info. %d caracteres impresos correspondientes a \"%s\"\n", __FILE__, __LINE__, len, clave_label);
    }

  } else {

    if ( verbose ) {
      printf("%s:%d Info. No hay claves que imprimir.\n", __FILE__, __LINE__);
    }

    rect_ident.top    = point->y;
    rect_ident.bottom = point->y - 10;
    rect_ident.left   = rect_cant.right;
    rect_ident.right  = rect_cant.right;
  }

  /* La unidad de venta */
  rect_unidad.top    = point->y;
  rect_unidad.bottom = point->y - 10;
  rect_unidad.left   = rect_ident.right;
  width_unidad       = HPDF_Page_TextWidth(page, unidad_label);
  rect_unidad.right  = rect_unidad.left + width_unidad + 2;
  HPDF_Page_TextRect(page,
			rect_unidad.left,
			rect_unidad.top,
			rect_unidad.right,
			rect_unidad.bottom,
			unidad_label,
			HPDF_TALIGN_CENTER,
			&len);

  if ( verbose ) {
    printf("%s:%d Info. %d caracteres impresos correspondientes a \"%s\"\n", __FILE__, __LINE__, len, unidad_label);
  }


  /* La descripcion.
   * Para este caso utilizaremos el espacio que quede de descontar 
   * el ancho para el importe y el total para cada concepto
   */
  width_precio = HPDF_Page_TextWidth(page, "888888888888");
  width_importe = HPDF_Page_TextWidth(page, "8888888888");
  rect_desc.top    = point->y;
  rect_desc.bottom = point->y - 10;
  rect_desc.left   = rect_unidad.right;
  width_desc       = page_width - margin - width_importe - width_precio - rect_unidad.right;
  rect_desc.right  = rect_desc.left + width_desc + 2;
  HPDF_Page_TextRect(page,
			rect_desc.left,
			rect_desc.top,
			rect_desc.right,
			rect_desc.bottom,
			descripcion_label,
			HPDF_TALIGN_CENTER,
			&len);

  if ( verbose ) {
    printf("%s:%d Info. %d caracteres impresos correspondientes a \"%s\"\n", __FILE__, __LINE__, len, descripcion_label);
  }

  /* El valor unitario */
  rect_precio.top    = point->y;
  rect_precio.bottom = point->y - 10;
  rect_precio.left   = rect_desc.right;
  rect_precio.right  = rect_precio.left + width_precio + 2;
  HPDF_Page_TextRect(page,
			rect_precio.left,
			rect_precio.top,
			rect_precio.right,
			rect_precio.bottom,
			valor_unitario_label,
			HPDF_TALIGN_CENTER,
			&len);

  if ( verbose ) {
    printf("%s:%d Info. %d caracteres impresos correspondientes a \"%s\"\n", __FILE__, __LINE__, len, valor_unitario_label);
    printf("%s:%d Info. Top = %f, bottom = %f, left = %f, rigth = %f\n", __FILE__, __LINE__, rect_precio.top, rect_precio.bottom, rect_precio.left, rect_precio.right);
  }

  /* El importe */
  rect_importe.top    = point->y;
  rect_importe.bottom = point->y - 10;
  rect_importe.left   = rect_precio.right;
  rect_importe.right  = rect_importe.left + width_importe + 2;
  HPDF_Page_TextRect(page,
			rect_importe.left,
			rect_importe.top,
			rect_importe.right,
			rect_importe.bottom,
			importe_label,
			HPDF_TALIGN_CENTER,
			&len);

  if ( verbose ) {
    printf("%s:%d Info. %d caracteres impresos correspondientes a \"%s\"\n", __FILE__, __LINE__, len, importe_label);
  }

  HPDF_Page_EndText(page);

  /* Draw another line */
  *point = HPDF_Page_GetCurrentPos(page);
  printf("2. Current position: y = %f, x = %f\n", point->y, point->x);
  printf("La linea inferior sera dibujada en: y = %f, x = %f\n", page_height - 150 - 15, page_width - margin);

  HPDF_Page_SetLineWidth(page, 1);
  HPDF_Page_MoveTo(page, margin, page_height - top_y - 15);
  HPDF_Page_LineTo(page, page_width - margin, page_height - top_y - 15);

  *point = HPDF_Page_GetCurrentPos(page);
  printf("3. Current position: y = %f, x = %f\n", point->y, point->x);

  /* Print the current path */
  HPDF_Page_Stroke(page);


  /**
   * Ahora imprime los conceptos los conceptos
   */
  HPDF_Page_BeginText(page);
  HPDF_Page_SetFontAndSize(page, font, font_size);
  conceptos = cfdi->Conceptos;
  while ( conceptos != NULL ) {

    /**
     * La cantidad
     */
    top = point->y;
    bottom = point->y - 10;
    HPDF_Page_TextRect(page, rect_cant.left, top, rect_cant.right, bottom,
		       (const char *)conceptos->cantidad, HPDF_TALIGN_RIGHT,
		       NULL);

    /**
     * El numero de identificacion del concepto, o catalogo
     * en caso de que exista.
     */
    if ( tiene_noIdent ) {
      HPDF_Page_TextRect(page, rect_ident.left, top, rect_ident.right, bottom,
			 (const char *)conceptos->noIdentificacion,
			 HPDF_TALIGN_LEFT, NULL);
    }

    /**
     * La unidad de venta
     */
    HPDF_Page_TextRect(page, rect_unidad.left, top, rect_unidad.right, bottom,
		       (const char *)conceptos->unidad, HPDF_TALIGN_LEFT, NULL);

    /**
     * La descripcion del concepto
     */
    HPDF_Page_TextRect(page, rect_desc.left, top, rect_desc.right, bottom,
		       (const char *)conceptos->descripcion, HPDF_TALIGN_LEFT,
		       NULL);

    /**
     * El precio unitario
     */
    memset(buffer, 0, 14);
    strfmon(buffer, 14, "%!.4n", atof((const char *)conceptos->valorUnitario));
    HPDF_Page_TextRect(page, rect_precio.left, top, rect_precio.right, bottom,
		       buffer, HPDF_TALIGN_RIGHT, NULL);

    /**
     * El importe
     */
    memset(buffer, 0, 14);
    strfmon(buffer, 14, "%n", atof((const char *)conceptos->importe));
    HPDF_Page_TextRect(page, rect_importe.left, top, rect_importe.right, bottom,
		       buffer, HPDF_TALIGN_RIGHT, NULL);

    *point = HPDF_Page_GetCurrentTextPos(page);
    conceptos = conceptos->next;
  }

  HPDF_Page_EndText(page);

  /* Ahora dibuja otra linea para indicar el final de los conceptos */
  HPDF_Page_SetLineWidth(page, 1);
  HPDF_Page_MoveTo(page, margin, point->y - 5);

  /* Dibuja una linea */
  HPDF_Page_LineTo(page, page_width - margin, point->y - 5);

  *point = HPDF_Page_GetCurrentPos(page);

  /* Paint the current path */
  HPDF_Page_Stroke(page);

  return *point;
}


/**
 *
 */
int
r12nimpresa(const char *input, const char *output, int verbose)
{
  Comprobante_t *cfdi = NULL;

  HPDF_Doc pdf;
  HPDF_Page page;
  HPDF_Font font;
  HPDF_Font font_bold;
  HPDF_Point point;
  HPDF_REAL page_height;
  HPDF_REAL page_width;
  const HPDF_REAL margin = 20; /* width margin */

  const char *font_name;
  const char *font_bold_name;
  char buffer[256];

  setlocale(LC_ALL, "");

  cfdi = lee_cfdi(input, verbose);
  if ( cfdi == NULL ) {
    fprintf(stderr, "%s:%d Error. No pudo completarse la representacion del CFDi.\n", __FILE__, __LINE__);
    return 1;
  }

  pdf = HPDF_New(error_handler, "Sacatelas babuchas");
  if ( !pdf ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. No fue posible crear el objeto para la creación de la representación impresa del CFDi", __FILE__, __LINE__);
    }
    return 1;
  }

  if ( verbose ) {
    printf("PDF new created.\n");
  }

  if ( setjmp(env) ) {
    HPDF_Free(pdf);
    if ( verbose ) {
      fprintf(stderr, "%s:%d Error. Contexto de pila invalidado para tratar con los errores e interrupciones encontradas en una subrutina de bajo nivel.\n", __FILE__, __LINE__);
    }
    return 2;
  }

  if ( verbose ) {
    printf("%s:%d Info. Fijando opciones basicas del documento.\n", __FILE__, __LINE__);
  }

  /* Fija las opciones generales del documento */
  fija_opciones_del_documento(pdf, cfdi);

  if ( verbose ) {
    printf("%s:%d Info. Las opciones basicas del documento han sido fijadas.\n", __FILE__, __LINE__);
  }

  /* Ahora es tiempo de comenzar con los atributos de la pagina */
  page = HPDF_AddPage(pdf);
  HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_LETTER, HPDF_PAGE_PORTRAIT);

  /* Lee el tamaño real de la pagina */
  page_height = HPDF_Page_GetHeight(page);
  page_width = HPDF_Page_GetWidth(page);

  /* Define las fonts a utilizar */
  /*font_name = HPDF_LoadTTFontFromFile(pdf, "/file/path/to/the/font", HPDF_TRUE);*/
  font_name = HPDF_LoadTTFontFromFile(pdf, INERE_TTF_FONT_PATH, HPDF_TRUE);
  /*font_bold_name = HPDF_LoadTTFontFromFile(pdf, "/file/path/to/the/font", HPDF_TRUE);*/
  font_bold_name = HPDF_LoadTTFontFromFile(pdf, INERE_TTF_FONT_BOLD_PATH, HPDF_TRUE);
  font = HPDF_GetFont(pdf, font_name, "UTF-8");
  font_bold = HPDF_GetFont(pdf, font_bold_name, "UTF-8");

  /* Ahora comienza a escribir la informacion */
  if ( verbose ) {
    printf("%s:%d Info. Comenzando a escribir la información basica del comprobante\n", __FILE__, __LINE__);
  }

  /* Vamos a comenzar desde el lado derecho de la pagina */
  HPDF_Page_BeginText(page);

  /* El folio fiscal */
  HPDF_Page_SetFontAndSize(page, font_bold, font_size_label);
  HPDF_Page_TextOut(page,
	    page_width - margin - HPDF_Page_TextWidth(page, folio_fiscal_label),
	    page_height - 20, folio_fiscal_label);

  HPDF_Page_SetFontAndSize(page, font, font_size);
  HPDF_Page_TextOut(page,
            page_width - margin - HPDF_Page_TextWidth(page,
			(const char *)cfdi->Complemento->TimbreFiscalDigital->UUID),
	    page_height - 30, (const char *)cfdi->Complemento->TimbreFiscalDigital->UUID);


  /* Ahora la leyenda "No. de serie del CSD" */
  HPDF_Page_SetFontAndSize(page, font_bold, font_size_label);
  HPDF_Page_TextOut(page,
	    page_width - margin - HPDF_Page_TextWidth(page, no_serie_csd_label),
	    page_height - 40, no_serie_csd_label);

  HPDF_Page_SetFontAndSize(page, font, font_size);
  HPDF_Page_TextOut(page,
            page_width - margin - HPDF_Page_TextWidth(page,(const char *)cfdi->noCertificado),
	    page_height - 50, (const char *)cfdi->noCertificado);


  /* Ahora la leyenda "Lugar, fecha y hora de emisión" */
  HPDF_Page_SetFontAndSize(page, font_bold, font_size_label);
  HPDF_Page_TextOut(page,
	    page_width - margin - HPDF_Page_TextWidth(page,fecha_emision_label),
	    page_height - 60, fecha_emision_label);

  memset(buffer, 0, 256);
  snprintf(buffer, 256, "%s, %s", cfdi->LugarExpedicion, cfdi->fecha);
  HPDF_Page_SetFontAndSize(page, font, font_size);
  HPDF_Page_TextOut(page,
            page_width - margin - HPDF_Page_TextWidth(page, buffer),
	    page_height - 70, buffer);

  /* Ahora, imprime la serie (interno) del CFDi. Este es un atributo opcional */
  if ( cfdi->serie != NULL ) {

    HPDF_Page_SetFontAndSize(page, font_bold, font_size_label);
    HPDF_Page_TextOut(page,
	    page_width - margin - HPDF_Page_TextWidth(page, serie_label),
	    page_height - 80, serie_label);

    HPDF_Page_SetFontAndSize(page, font, font_size);
    HPDF_Page_TextOut(page,
            page_width - margin - HPDF_Page_TextWidth(page,(const char *)cfdi->serie),
	    page_height - 90, (const char *)cfdi->serie);

  }

  /* Ahora, el folio (interno), otro atributo opcional */
  if ( cfdi->folio != NULL ) {

    HPDF_REAL offset = 0;
    if ( cfdi->serie != NULL ) {
      offset = 20;
    }
    HPDF_Page_SetFontAndSize(page, font_bold, font_size_label);
    HPDF_Page_TextOut(page,
	    page_width - margin - HPDF_Page_TextWidth(page, folio_label),
	    page_height - 80 - offset, folio_label);

    HPDF_Page_SetFontAndSize(page, font, font_size);
    HPDF_Page_TextOut(page,
            page_width - margin - HPDF_Page_TextWidth(page,(const char *)cfdi->folio),
	    page_height - 90 - offset, (const char *)cfdi->folio);

  }

  if ( verbose ) {
    printf("%s:%d Info. Ahora se escribira la informacion del amisor y receptor.\n", __FILE__, __LINE__);
  }

  /* Ahora imprime los datos del emisor
   * Para el emisor utilizaremos desde la base de "page_height - 30"
   * hasta "page_height - 90"
   */

  /* Los datos del receptor */
  HPDF_Page_SetFontAndSize(page, font_bold, 8);
  HPDF_Page_TextOut(page, margin, page_height - 100, "Datos del receptor:");
  HPDF_Page_SetFontAndSize(page, font, 10);
  if ( cfdi->Receptor->nombre != NULL ) {
    HPDF_Page_TextOut(page, margin, page_height - 110, (const char *)cfdi->Receptor->nombre);
  }
  HPDF_Page_TextOut(page, margin, page_height - 120, (const char *)cfdi->Receptor->rfc);
  if ( cfdi->Receptor->Domicilio != NULL ) {
  }
  point = HPDF_Page_GetCurrentTextPos(page);
  printf("0. Despues de escribir los datos del cliente, x = %f, y = %f\n", point.x, point.y);
  HPDF_Page_EndText(page);

  if ( verbose ) {
    printf("%s:%d Info. Ahora se escribira la informacion de los conceptos 1.\n", __FILE__, __LINE__);
  }

  /* Ahora procederemos a escribir las conceptos
   * top_x = page_width
   * top_y = page_height - 150
   * imprime_conceptos(page, margin, top_x, top_y);
   */
  /*imprime_conceptos(page, margin, 150, font, font_bold, cfdi, verbose);*/
  point = imprime_conceptos(page, margin, &point, font, font_bold, cfdi, verbose);
  if ( verbose ) {
    printf("%s:%d Info. Conceptos terminados.\n", __FILE__, __LINE__);
  }

  /**
   * Ahora debemos de escribir los totales y el total con letra
   */
  HPDF_Page_BeginText(page);

  if ( verbose ) {
    printf("%s:%d Info. Posicion antes de imprimir los totales, x = %f, y = %f.\n", __FILE__, __LINE__, point.x, point.y);
  }

  /**
   * Comenzaremos primero con el total con letra y la forma de pago
   */
  HPDF_Page_SetFontAndSize(page, font_bold, font_size_label);
  HPDF_Page_TextRect(page, margin, point.y, margin + HPDF_Page_TextWidth(page, "Total con letra:") + 10, point.y - 10, "Total con letra:", HPDF_TALIGN_LEFT, NULL);
  HPDF_Page_SetFontAndSize(page, font, font_size);



  HPDF_Page_EndText(page);


  /* Libera la memoria consumida */
  termina_cfdi(cfdi);

  /* Escribe la informacion */
  HPDF_SaveToFile(pdf, output);

  HPDF_Free(pdf);

  return 0;
}
