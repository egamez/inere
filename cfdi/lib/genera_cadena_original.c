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
#ifndef INERE_CFDI_GENERA_CADENA_ORIGINAL_H_
#include "inere/cfdi/genera_cadena_original.h"
#endif

#include <string.h>
#include <stdarg.h>

#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

/* Forward declarations */
void local_error_function(void *ctx, const char* mess, ...);


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
genera_cadena_original(const char *stylesheet, xmlDocPtr doc, xmlChar *cfdi, xmlChar** cadena, int verbose)
{
  xsltStylesheetPtr style = NULL;
  xmlDocPtr result = NULL;
  int cadena_len = 0;
  int out = 0;


  xmlSubstituteEntitiesDefault(1);
  xmlLoadExtDtdDefaultValue = 1;

  xsltSetGenericErrorFunc(stderr, local_error_function);

  style = xsltParseStylesheetFile((const xmlChar *)stylesheet);
  if ( style == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Ocurrio un Error. Stylesheet (%s) no analizada.\n", __FILE__, __LINE__, stylesheet);
    }
    xsltCleanupGlobals();
    return 1;
  }

  /* verifica primero si fue suministrado el cfdi */
  if ( doc != NULL ) {
    result = xsltApplyStylesheet(style, doc, NULL);
  } else if ( cfdi != NULL ) {
    /* Debemos de crear un DocNodePtr */
    result = xsltApplyStylesheet(style, xmlReadMemory((char *)cfdi, strlen((char *)cfdi), "http://www.sat.gob.mx/cfd/3", "UTF-8", XML_PARSE_RECOVER), NULL);

  } else {
    /* Error */
    fprintf(stderr, "%s:%d Ocurrio un Error. Es necesario especificar el nodo o el string.\n", __FILE__, __LINE__);
    xsltFreeStylesheet(style);
    xsltCleanupGlobals();
    return 4;
  }

  if ( result == NULL ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Ocurrio un Error. Transformaciones de stylesheet (%s) no aplicadas.\n", __FILE__, __LINE__, stylesheet);
    }
    xsltFreeStylesheet(style);
    xsltCleanupGlobals();
    return 2;
  }

  out = xsltSaveResultToString(cadena, &cadena_len, result, style);
  if ( out == -1 ) {
    if ( verbose ) {
      fprintf(stderr, "%s:%d Ocurrio un error. Error al salvar la cadena original en el buffer.\n", __FILE__, __LINE__);
    }
    return 3;
  }

  xsltFreeStylesheet(style);
  xmlFreeDoc(result);

  if ( verbose ) {
    printf("%s:%d Cadena original de la información del comprobante:\n%s\n", __FILE__, __LINE__, *cadena);
  }

  xsltCleanupGlobals();

  return 0;
}
