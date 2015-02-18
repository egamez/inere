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

#ifndef INERE_CANTIDADCL_ALLOC_H_
#include "inere/cantidadcl_alloc.h"
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

const char *info_author          = "INERE - Soporte para CFDI";
const char *info_creator         = PACKAGE_STRING;

const char *folio_fiscal_label   = "Folio Fiscal:";
const char *no_serie_csd_label   = "No. de Serie del CSD:";
const char *fecha_emision_label  = "Lugar, Fecha y hora de emisión:";
const char *serie_label          = "Serie:";
const char *folio_label          = "Folio:";
const char *regimen_label	 = "Régimen Fiscal";

const char *cantidad_label       = "Cantidad";
const char *clave_label          = "Clave";
const char *unidad_label         = "Unidad";
const char *descripcion_label    = "Descripción";
const char *valor_unitario_label = "Precio unitario";
const char *importe_label        = "Importe";

const char *motivo_desc_label	 = "Motivo del Descuento:";
const char *moneda_label	 = "Moneda:";
const char *tipo_cambio_label	 = "Tipo de cambio:";
const char *forma_pago_label	 = "Forma de Pago:";
const char *metodo_pago_label	 = "Método de Pago:";
const char *num_cta_pago_label	 = "Número de cuenta de Pago:";
const char *cond_pago_label	 = "Condiciones de Pago:";

const HPDF_REAL line_width	 = 10;

const unsigned int decimales_importe      = 2;
const unsigned int digitos_importe        = 7; /* 9,999,999 max */
const char *format_importe		  = "%13n";
const char *importe_signature		  = "$8,888,888.88";
const unsigned int decimales_precio       = 4;
const unsigned int digitos_precio         = 7; /* 9 999 999 max */
const char *format_precio		  = "%!14.4n";
const char *precio_signature		  = "8,888,888.8888";

const unsigned int decimales_cantidad     = 0;
const unsigned int digitos_cantidad       = 7; /* 9999999 max */
const unsigned int noIdentificacion_width = 20;

/* Forward declarations */
void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no,void *user_data);
HPDF_STATUS fija_opciones_del_documento(HPDF_Doc pdf, Comprobante_t* cfdi);
HPDF_Point imprime_conceptos(HPDF_Page page, const HPDF_REAL margin,
			     HPDF_Point *point, HPDF_Font font,
			     HPDF_Font font_bold, Comprobante_t* cfdi,
			     const unsigned int font_size,
			     const unsigned int font_label_size,
			     const int verbose);
HPDF_UINT write_in_a_box(HPDF_Page page, HPDF_REAL x, HPDF_REAL y,
			 HPDF_REAL width, const char *data);
char *cadena_original_de_certificacion_alloc(const TimbreFiscalDigital_t *timbre);


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
 * Función auxiliar para crear la cadena original del complemento de
 * certificacion digital del SAT
 *
 *	||version|UUID|Fecha de timbrado|selloCFD|noCertificadoSAT||
 *
 */
char *
cadena_original_de_certificacion_alloc(const TimbreFiscalDigital_t *timbre)
{
  char *buffer = NULL;
  const size_t len = 2 + strlen((char *)timbre->version) + 1 +
			 strlen((char *)timbre->UUID) + 1 +
			 strlen((char *)timbre->FechaTimbrado) + 1 +
			 strlen((char *)timbre->selloCFD) + 1 +
			 strlen((char *)timbre->noCertificadoSAT) + 1 + 2 + 1;

  buffer = (char *)malloc(len);
  if ( buffer != NULL ) {
    memset(buffer, 0, len);
    snprintf(buffer, len, "||%s|%s|%s|%s|%s||",
		timbre->version,
		timbre->UUID,
		timbre->FechaTimbrado,
		timbre->selloCFD,
		timbre->noCertificadoSAT);
  }

  return buffer;
}


/**
 * Funcion auxiliar para fijar algunas opciones del documento
 * OK
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
  snprintf(buffer, 49, "CFDi - Representacion Impresa para %s",
							cfdi->Receptor->rfc);
  res = HPDF_SetInfoAttr(pdf, HPDF_INFO_TITLE, buffer);

  /* El asunto del PDF */
  memset(buffer, 0, 256);
  if ( cfdi->Complemento != NULL ) {
    if ( cfdi->Complemento->TimbreFiscalDigital != NULL ) {
      snprintf(buffer, 256, "Compra - Folio Fiscal: %s",
				cfdi->Complemento->TimbreFiscalDigital->UUID);
    } else {
      snprintf(buffer, 256, "Compra");
    }
  } else {
    snprintf(buffer, 256, "Compra");
  }
  res = HPDF_SetInfoAttr(pdf, HPDF_INFO_SUBJECT, buffer);

  /* Algunas palabras claves */
  memset(buffer, 0, 256);
  if ( cfdi->Complemento != NULL ) {
    if ( cfdi->Complemento->TimbreFiscalDigital != NULL ) {
      snprintf(buffer, 256, "%s, %s, %s, %s, CFDi",
				PACKAGE_NAME,
				cfdi->Emisor->rfc,
				cfdi->Receptor->rfc,
				cfdi->Complemento->TimbreFiscalDigital->UUID);
    } else {
      snprintf(buffer, 256, "%s, %s, %s, CFDi",
				PACKAGE_NAME,
				cfdi->Emisor->rfc,
				cfdi->Receptor->rfc);
    }
  } else {
    snprintf(buffer, 256, "%s, %s, %s, CFDi",
				PACKAGE_NAME,
				cfdi->Emisor->rfc,
				cfdi->Receptor->rfc);
  }
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
  creation_date.year    = moddate->tm_year;
  creation_date.month   = moddate->tm_mon + 1;
  creation_date.day     = moddate->tm_mday;
  creation_date.hour    = moddate->tm_hour;
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
 * OK
 */
HPDF_UINT
write_in_a_box(HPDF_Page page, HPDF_REAL x, HPDF_REAL y, HPDF_REAL width,
		const char *data)
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
		  const unsigned int font_size,
		  const unsigned int font_label_size, const int verbose)
{
  int tiene_noIdent = 0;
  char buffer[15];

  HPDF_REAL top_y = point->y;
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

  Concepto_list_t *conceptos = NULL;

  if ( verbose ) {
    printf("%s:%d Dibujando la linea superior de los conceptos.\n", __FILE__, __LINE__); 
  }

  /* Ahora, necesitamos de dibujar una linea que sirva de encabezado
   * para los conceptos que el CFDi ampara, debemos cambiar a Path painting
   * mode
   */
  HPDF_Page_SetLineWidth(page, 1);
  HPDF_Page_MoveTo(page, margin, top_y);

  /* Dibuja una linea */
  HPDF_Page_LineTo(page, page_width - margin, top_y);

  if ( verbose ) {
    printf("%s:%d Info. Ahora se escribira la informacion de los conceptos 2.\n", __FILE__, __LINE__);
  }

  *point = HPDF_Page_GetCurrentPos(page);

  /* Print the current path */
  HPDF_Page_Stroke(page);

  HPDF_Page_SetFontAndSize(page, font_bold, font_label_size);
  HPDF_Page_BeginText(page);

  /*
   * Imprime los encabezados
   */

  /* Cantidad */
  rect_cant.top    = point->y;
  rect_cant.bottom = point->y - line_width;
  rect_cant.left   = margin;
  /* Para determinar al ancho del campo 'Cantidad' seleccionaremos el
   * mayor entre la etiqueta 'Cantidad' y la cantidad de digitos
   */
  HPDF_Page_SetFontAndSize(page, font, font_size);
  width_cant = HPDF_Page_TextWidth(page, "8") * digitos_cantidad;
  HPDF_Page_SetFontAndSize(page, font_bold, font_label_size);
  width_cant = (width_cant >
		 HPDF_Page_TextWidth(page, "Cantidad") ?
			width_cant :
			HPDF_Page_TextWidth(page, "Cantidad"));
  rect_cant.right  = margin + width_cant + 2; /* 2 = espacio entre campos */
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
    rect_ident.bottom = point->y - line_width;
    rect_ident.left   = rect_cant.right + 2;
    HPDF_Page_SetFontAndSize(page, font, font_size);
    width_ident       = HPDF_Page_TextWidth(page, "A") * noIdentificacion_width;
    HPDF_Page_SetFontAndSize(page, font_bold, font_label_size);
    rect_ident.right  = rect_ident.left + width_ident;
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
    rect_ident.bottom = point->y - line_width;
    rect_ident.left   = rect_cant.right;
    rect_ident.right  = rect_cant.right;
  }

  /* La unidad de venta */
  rect_unidad.top    = point->y;
  rect_unidad.bottom = point->y - line_width;
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
   *
   * Para calcular el ancho del campo que corresponde al precio unitario y al
   * importe deberemos de...
   */
  HPDF_Page_SetFontAndSize(page, font, font_size);
  width_precio = HPDF_Page_TextWidth(page, precio_signature);
  width_importe = HPDF_Page_TextWidth(page, importe_signature);
  rect_desc.top    = point->y;
  rect_desc.bottom = point->y - line_width;
  rect_desc.left   = rect_unidad.right;
  width_desc       = page_width - margin - width_importe - width_precio - rect_unidad.right - 4; /* 4 espacio acumulado entre descripcion + precio */
  rect_desc.right  = rect_desc.left + width_desc + 2;
  HPDF_Page_SetFontAndSize(page, font_bold, font_label_size);
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
  rect_precio.bottom = point->y - line_width;
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
  rect_importe.bottom = point->y - line_width;
  rect_importe.left   = rect_precio.right;
  rect_importe.right  = rect_importe.left + width_importe;
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
  HPDF_Page_SetLineWidth(page, 1);
  HPDF_Page_MoveTo(page, margin, top_y - line_width);
  HPDF_Page_LineTo(page, page_width - margin, top_y - line_width);

  *point = HPDF_Page_GetCurrentPos(page);

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
    bottom = point->y - line_width;
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
    memset(buffer, 0, 15);
    strfmon(buffer, 15, format_precio,
		atof((const char *)conceptos->valorUnitario));
    HPDF_Page_TextRect(page, rect_precio.left, top, rect_precio.right, bottom,
		       buffer, HPDF_TALIGN_RIGHT, NULL);

    /**
     * El importe
     */
    memset(buffer, 0, 15);
    strfmon(buffer, 15, format_importe, atof((const char *)conceptos->importe));
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
 * Comienzo de página 'page_height - 20'
 *
 * La pagina la dividiremos en 6 secciones,
 *
 *	- una sección para la informacion de identificación del comprobante
 *	  (folio fiscal, fecha, etc.) secc1
 *	- una sección para los datos del emisor y receptor del CFDI, secc2
 *	- una sección para los conceptos que ampara el CFDI, secc3
 *	- una sección para los totales, secc4,
 *	- una sección para la cantidad con letra, forma de pago, etc, secc5, y
 *	- una sección para el sello y el qrcode, secc6
 *
 * El módelo a utilizar para escribir la información será de arriba hacía
 * abajo, comenzando primero por la seccion de identificación, y de ahí a la
 * sección del emisor y receptor.
 *
 */
int
r12nimpresa(const char *input, const char *output,
	    const char *banner,
	    const unsigned int banner_lines,
	    const char *extra_info,
	    const char *footer,
	    const char *ttf_font_path,
	    const char *ttf_bold_font_path,
	    const unsigned int user_font_size,
	    const unsigned int user_font_label_size,
	    const int sucursal,
	    const int verbose)
{
  const char *font_name;
  const char *font_bold_name;
  char buffer[1024];
  char totales[15];
  char *cantidadcl = NULL;
  char *cadena_certificacion = NULL;
  unsigned int font_size = 0;
  unsigned int font_size_label = 0;

  HPDF_Doc pdf;
  HPDF_Page page;
  HPDF_Font font;
  HPDF_Font font_bold;
  HPDF_Point point;
  HPDF_REAL page_height;
  HPDF_REAL page_width;
  const HPDF_REAL margin = 20; /* width margin */
  HPDF_INT lines = 1;
  HPDF_REAL secc1_width = 0;
  HPDF_REAL secc1_height = 0;
  HPDF_REAL secc5_width = 0;
  HPDF_REAL secc5_height = 0;
  HPDF_Image qrcode_image;
  HPDF_REAL qrcode_image_size = 114 * .75; /* The image already measures 114dpi
					    * which is roughly 4cm, and we
					    * need one of 3cm
					    */

  HPDF_REAL y = 0; /* Para la coordenada y en que se estará escribiendo */
  HPDF_REAL x = 0; /* Para la coordenada x en que se estará escribiendo */

  Comprobante_t *cfdi = NULL;
  RegimenFiscal_list_t *regimen = NULL;
  Retencion_list_t *retencion = NULL;
  Traslado_list_t *traslado = NULL;
  cfdi_qrcode_t *qrcode = NULL;

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

  /* Verifica algunas opciones */
  if ( user_font_size == 0 ) {
    if ( verbose ) {
      printf("%s:%d Info. No se definio el tamaño de la fuente para generar la representación impresa, se utilizará el valor default (%d dpi)\n", __FILE__, __LINE__, INERE_FONT_SIZE);
    }
    font_size = INERE_FONT_SIZE;
  } else {
    if ( verbose ) {
      printf("%s:%d Info. Se ha definido un tamaño para la fuente, para generar la representación impresa (%d dpi)\n", __FILE__, __LINE__, user_font_size);
    }
    font_size = user_font_size;
  }

  if ( user_font_label_size == 0 ) {
    if ( verbose ) {
      printf("%s:%d Info. No se definio el tamaño de la fuente, en negritas, para generar la representación impresa, se utilizará el valor default (%d dpi)\n", __FILE__, __LINE__, INERE_FONT_LABEL_SIZE);
    }
    font_size_label = INERE_FONT_LABEL_SIZE;
  } else {
    if ( verbose ) {
      printf("%s:%d Info. Se ha definido un tamaño para la fuente, en negritas, para generar la representación impresa (%d dpi)\n", __FILE__, __LINE__, user_font_label_size);
    }
    font_size_label = user_font_label_size;
  }


  if ( verbose ) {
    printf("%s:%d Info. Fijando opciones generales del documento.\n", __FILE__, __LINE__);
  }
  /* Fija las opciones generales del documento */
  fija_opciones_del_documento(pdf, cfdi);

  if ( verbose ) {
    printf("%s:%d Info. Las opciones generales del documento han sido fijadas.\n", __FILE__, __LINE__);
  }

  /* Ahora es tiempo de comenzar con los atributos de la pagina */
  page = HPDF_AddPage(pdf);
  HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_LETTER, HPDF_PAGE_PORTRAIT);

  /* Lee el tamaño real de la pagina */
  page_height = HPDF_Page_GetHeight(page);
  page_width = HPDF_Page_GetWidth(page);

  /* Define las fonts a utilizar */
  if ( ttf_font_path != NULL ) {
    font_name = HPDF_LoadTTFontFromFile(pdf, ttf_font_path, HPDF_TRUE);
  } else {
    /* Use the default font path */
    font_name = HPDF_LoadTTFontFromFile(pdf, INERE_TTF_FONT_PATH, HPDF_TRUE);
  }
  if ( ttf_bold_font_path != NULL ) {
    font_bold_name = HPDF_LoadTTFontFromFile(pdf, ttf_bold_font_path,HPDF_TRUE);
  } else {
    /* Use the default font path */
    font_bold_name = HPDF_LoadTTFontFromFile(pdf, INERE_TTF_FONT_BOLD_PATH, HPDF_TRUE);
  }
  font = HPDF_GetFont(pdf, font_name, "UTF-8");
  font_bold = HPDF_GetFont(pdf, font_bold_name, "UTF-8");



  /* Ahora comienza a escribir la informacion */
  if ( verbose ) {
    printf("%s:%d Info. Comenzando a escribir la información basica del comprobante\n", __FILE__, __LINE__);
  }

  /* Vamos a comenzar desde el lado derecho de la pagina */
  HPDF_Page_BeginText(page);


  y = page_height - 20;


  /* El folio fiscal */
  if ( cfdi->Complemento != NULL ) {
    if ( cfdi->Complemento->TimbreFiscalDigital != NULL ) {
      HPDF_Page_SetFontAndSize(page, font_bold, font_size_label);
      x = page_width - margin - HPDF_Page_TextWidth(page, folio_fiscal_label);
      HPDF_Page_TextOut(page, x, y, folio_fiscal_label);

      HPDF_Page_SetFontAndSize(page, font, font_size);
      y -= line_width;
      x = page_width - margin - HPDF_Page_TextWidth(page,
		(const char *)cfdi->Complemento->TimbreFiscalDigital->UUID);
      HPDF_Page_TextOut(page, x, y,
		(const char *)cfdi->Complemento->TimbreFiscalDigital->UUID);
      secc1_width = HPDF_Page_TextWidth(page,
		   (const char *)cfdi->Complemento->TimbreFiscalDigital->UUID);
      y -= line_width;
    }
  }


  /* Ahora la leyenda "No. de serie del CSD" */
  HPDF_Page_SetFontAndSize(page, font_bold, font_size_label);
  x = page_width - margin - HPDF_Page_TextWidth(page, no_serie_csd_label);
  HPDF_Page_TextOut(page, x, y, no_serie_csd_label);

  HPDF_Page_SetFontAndSize(page, font, font_size);
  y -= line_width;
  x = page_width - margin - HPDF_Page_TextWidth(page,
					(const char *)cfdi->noCertificado);
  HPDF_Page_TextOut(page, x, y, (const char *)cfdi->noCertificado);


  /* Ahora la leyenda "Lugar, fecha y hora de emisión" */
  HPDF_Page_SetFontAndSize(page, font_bold, font_size_label);
  y -= line_width;
  x = page_width - margin - HPDF_Page_TextWidth(page,fecha_emision_label);
  HPDF_Page_TextOut(page, x, y, fecha_emision_label);

  memset(buffer, 0, 1024);
  snprintf(buffer, 1024, "%s, %s", cfdi->LugarExpedicion, cfdi->fecha);
  HPDF_Page_SetFontAndSize(page, font, font_size);
  y -= line_width;
  x = page_width - margin - HPDF_Page_TextWidth(page, buffer);
  HPDF_Page_TextOut(page, x, y, buffer);

  if ( HPDF_Page_TextWidth(page, buffer) > secc1_width ) {
    secc1_width = HPDF_Page_TextWidth(page, buffer);
  }

  /* Ahora, imprime la serie (interno) del CFDi. Este es un atributo opcional */
  if ( cfdi->serie != NULL ) {

    HPDF_Page_SetFontAndSize(page, font_bold, font_size_label);
    y -= line_width;
    x = page_width - margin - HPDF_Page_TextWidth(page, serie_label);
    HPDF_Page_TextOut(page, x, y, serie_label);

    HPDF_Page_SetFontAndSize(page, font, font_size);
    y -= line_width;
    x = page_width -margin -HPDF_Page_TextWidth(page,(const char *)cfdi->serie);
    HPDF_Page_TextOut(page, x, y, (const char *)cfdi->serie);

  }

  /* Ahora, el folio (interno), otro atributo opcional */
  if ( cfdi->folio != NULL ) {

    HPDF_Page_SetFontAndSize(page, font_bold, font_size_label);
    y -= line_width;
    x = page_width - margin - HPDF_Page_TextWidth(page, folio_label);
    HPDF_Page_TextOut(page, x, y, folio_label);

    HPDF_Page_SetFontAndSize(page, font, font_size);
    y -= line_width;
    x = page_width -margin -HPDF_Page_TextWidth(page,(const char *)cfdi->folio);
    HPDF_Page_TextOut(page, x, y, (const char *)cfdi->folio);

  }

  /* Fija la coordenada y hasta la que se llegó en esta sección */
  secc1_height = y;

  if ( verbose ) {
    printf("%s:%d Info. Ahora se escribira la informacion del emisor y receptor.\n", __FILE__, __LINE__);
  }

  /* Ahora imprime los datos del emisor
   *
   * Para imprimir todos los datos del emisor y receptor utilizaremos el
   * concepto de 'write_in_a_box', para hacer posible escribir campos
   * con longitud casi ilimitada
   */

  y = page_height - 20;
  x = margin;
  /** Banner
   * Para el banner de texto, si el valor en el argumento de esta funcion
   * es cero, se utilizaran dos lineas.
   *
   * Debería de ser posible implementar una chequeo acercar de cuantos
   * caracteres fueron escritos, y si se requerira de escribir mas caracteres
   * en una caja justo abajo de la inicial.
   *
   */
  if ( banner != NULL ) {
    int banner_height = 0;
    if ( banner_lines < 2 ) {
      banner_height = 2 * line_width;
    } else {
      banner_height = banner_lines * line_width;
    }
    HPDF_Page_SetFontAndSize(page, font_bold, banner_height);
    HPDF_Page_TextRect(page, x, y, page_width - secc1_width, y - banner_height,
		       banner, HPDF_TALIGN_CENTER, NULL);
    y -= banner_height;
  }

  /* Los datos del emisor */
  HPDF_Page_SetFontAndSize(page, font_bold, 8);
  y -= line_width;
  HPDF_Page_TextOut(page, x, y, "Datos del emisor:");

  y -= line_width;
  HPDF_Page_SetFontAndSize(page, font_bold, 10);

  /* Nombre del emisor */
  if ( cfdi->Emisor->nombre != NULL ) {
    lines = write_in_a_box(page, x, y, page_width - x - secc1_width, (char *)cfdi->Emisor->nombre);
    y -= line_width * lines;
  }

  /* La clave del R.F.C. */
  lines = write_in_a_box(page, x, y, page_width - x - secc1_width, (char *)cfdi->Emisor->rfc);

  /*
   * REGIMEN FISCAL
   */
  regimen = cfdi->Emisor->RegimenFiscal;
  while ( regimen != NULL ) {
    memset(buffer, 0, 1024);
    snprintf(buffer, 1024, "%s", regimen->Regimen);
    y -= line_width * lines;
    lines = write_in_a_box(page, x, y, page_width - x - secc1_width, buffer);
    regimen = regimen->next;
  }

  /*
   * DOMICILIO FISCAL del EMISOR
   */
  if ( cfdi->Emisor->DomicilioFiscal != NULL ) {

    /* Para el domicilio fiscal, escribiremos primero:
     *
     *		- La calle, numero exterior, numero interior y colonia
     */
    memset(buffer, 0, 1024);
    if ( cfdi->Emisor->DomicilioFiscal->calle  != NULL ) {
      strcat(buffer, (char *)cfdi->Emisor->DomicilioFiscal->calle);
    }

    if ( cfdi->Emisor->DomicilioFiscal->noExterior != NULL ) {
      strcat(buffer, " ");
      strcat(buffer, (char *)cfdi->Emisor->DomicilioFiscal->noExterior);
    }

    if ( cfdi->Emisor->DomicilioFiscal->noInterior != NULL ) {
      strcat(buffer, " ");
      strcat(buffer, (char *)cfdi->Emisor->DomicilioFiscal->noInterior);
    }

    if ( cfdi->Emisor->DomicilioFiscal->colonia != NULL ) {
      strcat(buffer, ", COL. ");
      strcat(buffer, (char *)cfdi->Emisor->DomicilioFiscal->colonia);
    }

    if ( cfdi->Emisor->DomicilioFiscal->codigoPostal != NULL ) {
      strcat(buffer, ", C.P. ");
      strcat(buffer, (char *)cfdi->Emisor->DomicilioFiscal->codigoPostal);
    }

    y -= line_width * lines;
    lines = write_in_a_box(page, x, y, page_width - x - secc1_width, buffer);

    memset(buffer, 0, 1024);
    if ( cfdi->Emisor->DomicilioFiscal->municipio  != NULL ) {
      strcat(buffer, (char *)cfdi->Emisor->DomicilioFiscal->municipio);
    }

    if ( cfdi->Emisor->DomicilioFiscal->estado != NULL ) {
      strcat(buffer, ", ");
      strcat(buffer, (char *)cfdi->Emisor->DomicilioFiscal->estado);
    }

    y -= line_width * lines;
    lines = write_in_a_box(page, x, y, page_width - x - secc1_width, buffer);

  }

  /*
   * DOMICILIO DE EMISION del CFDI
   */
  if ( cfdi->Emisor->ExpedidoEn != NULL && sucursal ) {

    if ( verbose ) {
      printf("%s:%d Info. Se imprimira la información acerca del domicilio de emisión.\n", __FILE__, __LINE__);
    }

    HPDF_Page_SetFontAndSize(page, font_bold, 8);
    y -= line_width;
    HPDF_Page_TextOut(page, x, y, "Expedido en:");

    y -= line_width;
    HPDF_Page_SetFontAndSize(page, font, 10);

    /* Para el domicilio fiscal, escribiremos primero:
     *
     *		- La calle, numero exterior, numero interior y colonia
     */
    memset(buffer, 0, 1024);
    if ( cfdi->Emisor->ExpedidoEn->calle  != NULL ) {
      strcat(buffer, (char *)cfdi->Emisor->ExpedidoEn->calle);
    }

    if ( cfdi->Emisor->ExpedidoEn->noExterior != NULL ) {
      strcat(buffer, " ");
      strcat(buffer, (char *)cfdi->Emisor->ExpedidoEn->noExterior);
    }

    if ( cfdi->Emisor->ExpedidoEn->noInterior != NULL ) {
      strcat(buffer, " ");
      strcat(buffer, (char *)cfdi->Emisor->ExpedidoEn->noInterior);
    }

    if ( cfdi->Emisor->ExpedidoEn->colonia != NULL ) {
      strcat(buffer, ", COL. ");
      strcat(buffer, (char *)cfdi->Emisor->ExpedidoEn->colonia);
    }

    if ( cfdi->Emisor->ExpedidoEn->codigoPostal != NULL ) {
      strcat(buffer, ", C.P. ");
      strcat(buffer, (char *)cfdi->Emisor->ExpedidoEn->codigoPostal);
    }

    lines = write_in_a_box(page, x, y, page_width - x - secc1_width, buffer);

    memset(buffer, 0, 1024);
    if ( cfdi->Emisor->ExpedidoEn->municipio  != NULL ) {
      strcat(buffer, (char *)cfdi->Emisor->ExpedidoEn->municipio);
    }

    if ( cfdi->Emisor->ExpedidoEn->estado != NULL ) {
      strcat(buffer, ", ");
      strcat(buffer, (char *)cfdi->Emisor->ExpedidoEn->estado);
    }

    y -= line_width * lines;
    lines = write_in_a_box(page, x, y, page_width - x - secc1_width, buffer);

  }

  /* Extra info del emisor */
  if ( extra_info != NULL ) {
    y -= line_width * lines;
    lines = write_in_a_box(page, x, y, page_width - x - secc1_width,extra_info);
  }

  /* Actuliza la coordenada y, de todo lo que se ha escrito */
  y -= line_width * lines;

  /*
   * Los datos del RECEPTOR
   */
  HPDF_Page_SetFontAndSize(page, font_bold, 8);
  HPDF_Page_TextOut(page, x, y, "Datos del receptor:");
  HPDF_Page_SetFontAndSize(page, font, 10);
  if ( cfdi->Receptor->nombre != NULL ) {
    y -= line_width;
    HPDF_Page_TextOut(page, x, y, (const char *)cfdi->Receptor->nombre);
  }

  y -= line_width;
  HPDF_Page_TextOut(page, x, y, (const char *)cfdi->Receptor->rfc);
  y -= line_width;

  if ( cfdi->Receptor->Domicilio != NULL ) {

    memset(buffer, 0, 1024);
    if ( cfdi->Receptor->Domicilio->calle != NULL ) {
      strcat(buffer, (const char *)cfdi->Receptor->Domicilio->calle);
    }

    if ( cfdi->Receptor->Domicilio->noExterior != NULL ) {
      strcat(buffer, " ");
      strcat(buffer, (const char *)cfdi->Receptor->Domicilio->noExterior);
    }

    if ( cfdi->Receptor->Domicilio->noInterior != NULL ) {
      strcat(buffer, " ");
      strcat(buffer, (const char *)cfdi->Receptor->Domicilio->noInterior);
    }

    if ( cfdi->Receptor->Domicilio->colonia != NULL ) {
      strcat(buffer, ", COL. ");
      strcat(buffer, (const char *)cfdi->Receptor->Domicilio->colonia);
    }

    if ( cfdi->Receptor->Domicilio->codigoPostal != NULL ) {
      strcat(buffer, ", C.P. ");
      strcat(buffer, (const char *)cfdi->Receptor->Domicilio->codigoPostal);
    }
    lines = write_in_a_box(page, x, y, page_width - x - secc1_width, buffer);

    memset(buffer, 0, 1024);
    if ( cfdi->Receptor->Domicilio->municipio != NULL ) {
     strcat(buffer, (const char *)cfdi->Receptor->Domicilio->municipio);
    }

    if ( cfdi->Receptor->Domicilio->estado != NULL ) {
     strcat(buffer, ", ");
     strcat(buffer, (const char *)cfdi->Receptor->Domicilio->estado);
    }

    if ( cfdi->Receptor->Domicilio->pais != NULL ) {
     strcat(buffer, ". ");
     strcat(buffer, (const char *)cfdi->Receptor->Domicilio->pais);
    }
    y -= line_width * lines;
    lines = write_in_a_box(page, x, y, page_width - x - secc1_width, buffer);

    /* Actualiza el valor de la coordenada 'y' */
    y -= line_width * lines;
  }


  /* Lee la posicion en la se escribieron los datos de identificacion del
   * receptor y emisor
   */
  point = HPDF_Page_GetCurrentTextPos(page);

  /* Cambia de modo */
  HPDF_Page_EndText(page);

  /* Ahora debemos de seleccionar la posicion en la que comenzaran a escribirse
   * los conceptos. Para esto, deberemos de elegir la sección más grande,
   * si la sección 1, o la seccion 2
   */
  point.y = secc1_height <= y ? secc1_height : y;


  if ( verbose ) {
    printf("%s:%d Info. Ahora se escribira la informacion de los conceptos 1.\n", __FILE__, __LINE__);
  }

  /* 
   * CONCEPTOS
   */
  point = imprime_conceptos(page, margin, &point, font, font_bold, cfdi, font_size, font_size_label, verbose);
  if ( verbose ) {
    printf("%s:%d Info. Conceptos terminados.\n", __FILE__, __LINE__);
  }


  /**
   * SECCION 5
   * Ahora debemos de escribir los totales y el total con letra
   *
   * Procederemos de la misma manera que para las secciones 1 y 2,
   * escribiremos primero la parte de los totales, a la derecha, y
   * regresaremos a la cantidad con letra y demas parametros acerca del
   * la forma de pago, moneda, etc.
   */
  HPDF_Page_BeginText(page);

  if ( verbose ) {
    printf("%s:%d Info. Posicion antes de imprimir los totales, x = %f, y = %f.\n", __FILE__, __LINE__, point.x, point.y);
  }

  /*
   * El formato en el que se escribiran los totales sera:
   *
   *	subtotal
   *	descuento
   *	impuestos
   *	total
   *
   * De todos los mencionados, el 'descuento' es un parametro opcional.
   * y los impuestos son lo unicos variables, habiendo dos grupos:
   *
   *		Retencion
   *				ISR
   *				IVA
   * y
   *		Traslado
   *				IVA
   *				IEPS
   *
   * de modo que imprimiremos la leyenda acerca del tipo de impuesto,
   * Retencion o Traslado entre parentesis, despues el impuesto, y nuevamente
   * entre parentesis la tasa del impuesto, en caso de que exista
   *
   * Debemos de calcular el tamaño de la caja, y la palabra más grande es:
   *
   *		(Retencion) IEPS (%16.00): $9,999,999.99
   *		(Traslado) IEPS (%16.00): $9,999,999.99
   */
  HPDF_Page_SetFontAndSize(page, font_bold, font_size_label);
  secc5_width = HPDF_Page_TextWidth(page, "  (Retencion) IEPS (%16.00): $9,999,999.99");
  y = point.y;
  x = page_width - margin - secc5_width;
  memset(totales, 0, 15);
  strfmon(totales, 15, format_importe, atof((char *)cfdi->subTotal));
  memset(buffer, 0, 1024);
  snprintf(buffer, 1024, "Subtotal: %s", totales);
  HPDF_Page_TextRect(page, page_width - margin -secc5_width,
			   y,
			   page_width - margin,
			   y - line_width,
			   buffer,
			   HPDF_TALIGN_RIGHT,
			   NULL);
  y -= line_width;

  if ( cfdi->descuento != NULL ) {
    memset(totales, 0, 15);
    strfmon(totales, 15, format_importe, atof((char *)cfdi->descuento));
    memset(buffer, 0, 1024);
    snprintf(buffer, 1024, "Descuento: %s", totales);
    HPDF_Page_TextRect(page, page_width - margin -secc5_width,
			     y,
			     page_width - margin,
			     y - line_width,
			     buffer,
			     HPDF_TALIGN_RIGHT,
			     NULL);
    y -= line_width;
  }

  /* Ahora los impuestos */
  if ( cfdi->Impuestos->Retenciones != NULL ) {
    retencion = cfdi->Impuestos->Retenciones;
    while ( retencion != NULL ) {
      memset(totales, 0, 15);
      strfmon(totales, 15, format_importe, atof((char *)retencion->importe));
      memset(buffer, 0, 1024);
      snprintf(buffer, 1024, "(Retencion) %s: %s", retencion->impuesto, totales);
      HPDF_Page_TextRect(page, page_width - margin -secc5_width,
			       y,
			       page_width - margin,
			       y - line_width,
			       buffer,
			       HPDF_TALIGN_RIGHT,
			       NULL);
      y -= line_width;
      retencion = retencion->next;
    }
  }

  if ( cfdi->Impuestos->Traslados != NULL ) {
    traslado = cfdi->Impuestos->Traslados;
    while ( traslado != NULL ) {
      memset(totales, 0, 15);
      strfmon(totales, 15, format_importe, atof((char *)traslado->importe));
      memset(buffer, 0, 1024);
      snprintf(buffer, 1024, "(Traslado) %s (%s%%): %s", traslado->impuesto, traslado->tasa, totales);
      HPDF_Page_TextRect(page, page_width - margin -secc5_width,
			       y,
			       page_width - margin,
			       y - line_width,
			       buffer,
			       HPDF_TALIGN_RIGHT,
			       NULL);
      y -= line_width;
      traslado = traslado->next;
    }
  }

  /* El TOTAL */
  memset(totales, 0, 15);
  strfmon(totales, 15, format_importe, atof((char *)cfdi->total));
  memset(buffer, 0, 1024);
  snprintf(buffer, 1024, "Total: %s", totales);
  HPDF_Page_TextRect(page, page_width - margin -secc5_width,
			   y,
			   page_width - margin,
			   y - line_width,
			   buffer,
			   HPDF_TALIGN_RIGHT,
			   NULL);
  secc5_height = y;


  /* Ahora deberemos de escribir el total con letra y los demas parametros
   * acerca del pago.
   */
  x = margin;
  y = point.y - line_width;


  /**
   * el total con letra
   */
  if ( verbose ) {
    printf("%s:%d Info. Escribiendo el total con letra.\n", __FILE__, __LINE__);
  }
  HPDF_Page_SetFontAndSize(page, font_bold, 8);
  HPDF_Page_TextOut(page, x, y, "Total con letra:");
  y -= line_width;
  cantidadcl = cantidadcl_alloc((char *)cfdi->total, verbose);
  HPDF_Page_SetFontAndSize(page, font, font_size);
  lines = write_in_a_box(page, x, y, page_width - x - secc5_width, cantidadcl);
  free(cantidadcl);

  /* Forma de pago, requerido  */
  if ( verbose ) {
    printf("%s:%d Info. Escribiendo la forma de pago.\n", __FILE__, __LINE__);
  }
  y -= line_width * lines;
  HPDF_Page_SetFontAndSize(page, font_bold, 8);
  HPDF_Page_TextOut(page, x, y, "Forma de pago:");
  y -= line_width;
  HPDF_Page_SetFontAndSize(page, font, font_size);
  lines = write_in_a_box(page, x, y, page_width - x - secc5_width, (char *)cfdi->formaDePago);

  /* Numero de cuenta de Pago, atributo opcional pero util para la represent */
  if ( cfdi->NumCtaPago != NULL ) {
    if ( verbose ) {
      printf("%s:%d Info. Escribiendo el número de cuenta de pago.\n", __FILE__, __LINE__);
    }
    y -= line_width * lines;
    HPDF_Page_SetFontAndSize(page, font_bold, 8);
    HPDF_Page_TextOut(page, x, y, "Número de cuenta de pago:");
    y -= line_width;
    HPDF_Page_SetFontAndSize(page, font, font_size);
    lines = write_in_a_box(page, x, y, page_width - x - secc5_width, (char *)cfdi->NumCtaPago);
  }

  /* Motivo del Descuento */
  if ( cfdi->motivoDescuento != NULL ) {
    if ( verbose ) {
      printf("%s:%d Info. Escribiendo el motivo del descuento.\n", __FILE__, __LINE__);
    }
    y -= line_width * lines;
    HPDF_Page_SetFontAndSize(page, font_bold, 8);
    HPDF_Page_TextOut(page, x, y, "Motivo del Descuento:");
    y -= line_width;
    HPDF_Page_SetFontAndSize(page, font, font_size);
    lines = write_in_a_box(page, x, y, page_width - x - secc5_width, (char *)cfdi->motivoDescuento);
  }

  /* Moneda */
  if ( cfdi->Moneda != NULL ) {
    if ( verbose ) {
      printf("%s:%d Info. Escribiendo el tipo de moneda.\n", __FILE__, __LINE__);
    }
    y -= line_width * lines;
    HPDF_Page_SetFontAndSize(page, font_bold, 8);
    HPDF_Page_TextOut(page, x, y, "Moneda:");
    y -= line_width;
    HPDF_Page_SetFontAndSize(page, font, font_size);
    lines = write_in_a_box(page, x, y, page_width - x - secc5_width, (char *)cfdi->Moneda);
  }

  /* Tipo de cambio */
  if ( cfdi->TipoCambio != NULL ) {
    if ( verbose ) {
      printf("%s:%d Info. Escribiendo el tipo de cambio.\n", __FILE__, __LINE__);
    }
    y -= line_width * lines;
    HPDF_Page_SetFontAndSize(page, font_bold, 8);
    HPDF_Page_TextOut(page, x, y, "Tipo de cambio:");
    y -= line_width;
    HPDF_Page_SetFontAndSize(page, font, font_size);
    lines = write_in_a_box(page, x, y, page_width - x - secc5_width, (char *)cfdi->TipoCambio);
  }

  /* Metodo de Pago */
  if ( verbose ) {
    printf("%s:%d Info. Escribiendo el metodo de pago.\n", __FILE__, __LINE__);
  }
  y -= line_width * lines;
  HPDF_Page_SetFontAndSize(page, font_bold, 8);
  HPDF_Page_TextOut(page, x, y, "Metodo de pago:");
  y -= line_width;
  HPDF_Page_SetFontAndSize(page, font, font_size);
  lines = write_in_a_box(page, x, y, page_width - x - secc5_width, (char *)cfdi->metodoDePago);

  /* Condiciones de Pago */
  if ( cfdi->condicionesDePago != NULL ) {
    if ( verbose ) {
      printf("%s:%d Info. Escribiendo las condiciones de pago.\n", __FILE__, __LINE__);
    }
    y -= line_width * lines;
    HPDF_Page_SetFontAndSize(page, font_bold, 8);
    HPDF_Page_TextOut(page, x, y, "Condiciones de Pago:");
    y -= line_width;
    HPDF_Page_SetFontAndSize(page, font, font_size);
    lines = write_in_a_box(page, x, y, page_width - x - secc5_width, (char *)cfdi->condicionesDePago);
  }
  y -= line_width * lines;

  HPDF_Page_EndText(page);


  /* Ahora debemos de escribir el qr code, y la informacion del timbrado
   * y para esto debemos de calcular la altura a la que comenzaran a escribirse
   * esta informacion
   */
  y = (y < secc5_height ? y : secc5_height);


  if ( cfdi->Complemento != NULL ) {

    if ( cfdi->Complemento->TimbreFiscalDigital != NULL ) {

      if ( verbose ) {
	printf("%s:%d Info. Generando el qrcode para este CFDI.\n", __FILE__, __LINE__);
      }
      qrcode = crea_cfdi_qrcode_alloc((char *)cfdi->Emisor->rfc,
				    (char *)cfdi->Receptor->rfc,
				    (char *)cfdi->total,
				    (char *)cfdi->Complemento->TimbreFiscalDigital->UUID);

      qrcode_image = HPDF_LoadPngImageFromMem(pdf,
					(const HPDF_BYTE *)qrcode->buffer,
					(HPDF_UINT)qrcode->size);

      HPDF_Page_DrawImage(page, qrcode_image, x, y - qrcode_image_size,
			qrcode_image_size, qrcode_image_size);

      if ( qrcode != NULL ) {
	free(qrcode);
      }


      /* Ahora escribe los parametros del timbrado */
      x = qrcode_image_size + margin;
      /*y -= line_width;*/
      HPDF_Page_BeginText(page);

      /* Sello digital del CFDI */
      HPDF_Page_SetFontAndSize(page, font_bold, 8);
      HPDF_Page_TextOut(page, x, y, "Sello digital del CFDI:");
      y -= line_width;
      HPDF_Page_SetFontAndSize(page, font, font_size);
      lines = write_in_a_box(page, x, y, page_width - x - margin, (char *)cfdi->Complemento->TimbreFiscalDigital->selloCFD);

      /* Sello del SAT */
      y -= line_width * lines;
      HPDF_Page_SetFontAndSize(page, font_bold, 8);
      HPDF_Page_TextOut(page, x, y, "Sello del SAT:");
      y -= line_width;
      HPDF_Page_SetFontAndSize(page, font, font_size);
      lines = write_in_a_box(page, x, y, page_width - x - margin, (char *)cfdi->Complemento->TimbreFiscalDigital->selloSAT);

      /* Cadena Original del complemento de certificacion digital del SAT */
      y -= line_width * lines;
      HPDF_Page_SetFontAndSize(page, font_bold, 8);
      HPDF_Page_TextOut(page, x, y, "Cadena Original del complemento de certificación digital del SAT:");
      y -= line_width;
      HPDF_Page_SetFontAndSize(page, font, font_size);
      cadena_certificacion = cadena_original_de_certificacion_alloc(cfdi->Complemento->TimbreFiscalDigital);
      lines = write_in_a_box(page, x, y, page_width - x - margin, cadena_certificacion);
      free(cadena_certificacion);

      /* No. de Serie del Certificado del SAT */
      y -= line_width * lines;
      HPDF_Page_SetFontAndSize(page, font_bold, 8);
      HPDF_Page_TextOut(page, x, y, "No. de Serie del Certificado del SAT:");
      y -= line_width;
      HPDF_Page_SetFontAndSize(page, font, font_size);
      lines = write_in_a_box(page, x, y, page_width - x - margin, (char *)cfdi->Complemento->TimbreFiscalDigital->noCertificadoSAT);

      /* Fecha y hora de certificacion */
      y -= line_width * lines;
      HPDF_Page_SetFontAndSize(page, font_bold, 8);
      HPDF_Page_TextOut(page, x, y, "Fecha y hora de certificación:");
      y -= line_width;
      HPDF_Page_SetFontAndSize(page, font, font_size);
      lines = write_in_a_box(page, x, y, page_width - x - margin, (char *)cfdi->Complemento->TimbreFiscalDigital->FechaTimbrado);

      HPDF_Page_EndText(page);

      y -= line_width * lines;
    }
  }

  /* Ahora imprime la leyenda
   * Este documento es una representación impresa de un CFDI
   */
  if ( cfdi->Complemento != NULL ) {
    if ( cfdi->Complemento->TimbreFiscalDigital != NULL ) {
      HPDF_Page_BeginText(page);
      HPDF_Page_SetFontAndSize(page, font_bold, font_size_label);
      HPDF_Page_TextRect(page, margin, y + line_width, page_width - margin, y,
	"Este documento es una representación impresa de un CFDI",
	HPDF_TALIGN_CENTER, NULL);
      HPDF_Page_EndText(page);
      y -= line_width;
    }
  }

  if ( footer != NULL ) {
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, font, font_size_label);
    HPDF_Page_TextOut(page, margin, y, footer);
    HPDF_Page_EndText(page);
  }

  /* Libera la memoria consumida */
  termina_cfdi(cfdi);

  /* Escribe la informacion */
  HPDF_SaveToFile(pdf, output);

  HPDF_Free(pdf);

  return 0;
}
