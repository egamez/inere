/*
 * Copyright (c) 2015, L3a,
 *                     Enrique Gamez Flores <egamez@edisson.com.mx>
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
#ifndef INERE_CFDI_R12NIMPRESA_H_
#include "inere/cfdi/r12nimpresa.h"
#endif

/* Para cargar las opciones, por default, al momento de compilar */
#include "inere/cfdi/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>

/**
 * http://stackoverflow.com/questions/5309471/getting-file-extension-in-c
 */

/* Forward declaration */
void usage(void);
char *substitute_filename_alloc(const char *filename);


/**
 *
 */
char *
substitute_filename_alloc(const char *filename)
{
  char *buffer = NULL;
  const size_t len = strlen(filename);
  char *extension = strrchr(filename, '.');
  const size_t len_extension = strlen(extension);

  buffer = (char *)calloc(len + len_extension, sizeof(char));
  memcpy(buffer, filename, len - len_extension);
  strncat(buffer, ".pdf", 4);

  return buffer;
}


void
usage(void)
{
  printf("Uso: ri [OPCIONES] CFDI [OUTPUT]\n\n");
  printf("\
Programa para generar la representación impresa (archivo PDF) a partir de un\n\
CFDI.\n\n");

  printf("\
Principales opciones:\n\
\n\
  -b BANNER, --banner=BANNER	Leyenda a aparecer como encabezado de la\n\
				representación impresa.\n\
  -i INFO,   --info=INFO	Mensaje a aparecer como información, por\n\
				ejemplo: Teléfono, email, etc.\n\
  -f PATH,   --font=PATH	Ruta en donde se encuentran las TTF fonts\n\
				con las que se generará la representación\n\
				impresa, diferente a las TTF enunciadas al\n\
				momento de compilación.\n\
				(default: %s)\n", INERE_TTF_FONT_PATH);
  printf("\
  -F PATH, --font-bold=PATH	Ruta en donde se encuentran las TTF fonts\n\
				en negritas con las que se generará la\n\
				representación impresa, diferente a las TTF\n\
				enunciadas al momento de compilación\n\
				(default: %s)\n\
  -s SIZE, --font-size=SIZE	Tamaño de la font, en dpi, con la que se\n\
				generará la representación impresa\n\
				(default: %d)\n", INERE_TTF_FONT_BOLD_PATH, INERE_FONT_SIZE);
  printf("\
  -l SIZE, --font-label-size=SIZE Tamaño de la font, en dpi, con la que se\n\
				generarán las etiqueta de la información de\n\
				la representación impresa (default: %d)\n\
  -S,  --sucursal		Imprime los datos acerca del domicilio de\n\
				expedición, en caso de que estos se encuentren\n\
				en el CFDI.\n\
  -v,  --verbose		Imprime mensajes extra acerca de la ejecución\n\
				en la generación.\n\
  -h,  --help			Imprime este mensaje.\n", INERE_FONT_LABEL_SIZE);
  printf("\
\n\n\
Bugs a <egamezf@gmail.com>\n");

}

/**
 *
 */
int
main(int argc, char *argv[])
{
  int res = 0;
  int ch = 0;
  const char *banner = NULL;
  const char *info = NULL;
  const char *font_path = NULL;
  const char *font_bold_path = NULL;
  const char *cfdi = NULL;
  char *cfdi_pdf = NULL;
  int font_size = 0;
  int font_label_size = 0;
  int want_sucursal = 0;
  int want_verbose = 0;
  int want_help = 0;

  /* options descriptor */
  static struct option longopts[] = {
	{"banner",		required_argument,	NULL,	'b'},
	{"info",		required_argument,	NULL,	'i'},
	{"font",		required_argument,	NULL,	'f'},
	{"font-bold",		required_argument,	NULL,	'F'},
	{"font-size",		required_argument,	NULL,	's'},
	{"font-label-size",	required_argument,	NULL,	'l'},
	{"sucursal",		required_argument,	NULL,	'S'},
	{"verbose",		no_argument,		NULL,	'v'},
	{"help",		no_argument,		NULL,	'h'},
	{NULL,			0,			NULL,	 0 }
  };

  while ((ch=getopt_long(argc,argv,"b:i:f:F:s:l:Svh", longopts, NULL)) != -1 ) {

    switch(ch) {

      case 'b':
	/* La leyenda de banner del comprobante */
	banner = optarg;
	break;

      case 'i':
	/* Info a mostrar en la factura, telefonos, email, etc */
	info = optarg;
	break;

      case 'f':
	/* El path de la font */
	font_path = optarg;
	break;

      case 'F':
	/* El path de la font bold */
	font_bold_path = optarg;
	break;

      case 's':
	/* El tamaño de la font en dpi, default 8 dpi */
	font_size = atoi(optarg);
	break;

      case 'l':
	/* El tamaño de la font en dpi, para las etiquetas de la informacion */
	font_label_size = atoi(optarg);
	break;

      case 'S':
	/* Imprime la informacion acerca del domicilio de expedicion */
	want_sucursal = 1;
	break;

      case 'v':
	/* Activa el modo verbose */
	want_verbose = 1;
	break;

      case 'h':
	/* help */
	want_help = 1;
	break;

      case '?':
	/* getopt_long already printed an error message. */
	break;

      default:
	break;

    }
  }

  /* Check if help was selected */
  if ( want_help ) {
    usage();
    return 0;
  }

  /* Ahora, preparate para leer algunos argumento posicionales
   * los argumentos seran:
   *
   *		- el CFDI
   *		- y, opcionalmente el nombre del archivo de salida de
   *		  la representacion impresa
   *
   * En caso de que el nombre del archivo, en el que se escribira la
   * la representacion impresa, no sea suministrado, el nombre se tomará
   * del nombre del CFDI, pero ahora con la extensión pdf
   */
  if ( optind >= argc ) {
    /* Esta es una situacion inusual, no fue proporcionado el CFDI, de
     * modo que no es posible general la representacion impresa
     */
    fprintf(stderr, "Error. No fue propocionado la ruta del CFDI, de modo que no hay representación impresa a generar.\n");
    return 1;
  }

  /* Lee al argumento */
  if ( argv[optind] != NULL ) {
    cfdi = argv[optind++];
  }

  /* Verifica que se hallan definido las fonts */
  if ( font_path == NULL ) {
    if ( strcmp(INERE_TTF_FONT_PATH, "Sin definir") == 0 ) {
      /* No se ha definido la ruta de las fonts. Error */
      fprintf(stderr, "No se ha definido la ruta para las fonts, de modo que no es posible generar la representación.\nUtilice -f,--font para proporcionar la ruta del archivo que contiene las fonts.\n");
      return 2;
    }
  }

  /* Verifica que se hallan definido las fonts */
  if ( font_bold_path == NULL ) {
    if ( strcmp(INERE_TTF_FONT_BOLD_PATH, "Sin definir") == 0 ) {
      /* No se ha definido la ruta de las fonts. Error */
      fprintf(stderr, "No se ha definido la ruta para las fonts, en negritas, de modo que no es posible generar la representación.\nUtilice -F,--font-bold para proporcionar la ruta del archivo que contiene las fonts en negritas.\n");
      return 3;
    }
  }

  /* Verifica si fue proporcionado el archivo de salida */
  if ( argv[optind] != NULL ) {
    cfdi_pdf = argv[optind];

  } else {
    /* Genera el nombre del archivo de salida */
    cfdi_pdf = substitute_filename_alloc(cfdi);
  }

  res = r12nimpresa(cfdi, cfdi_pdf, banner, info, font_path, font_bold_path,
		    font_size, font_label_size, want_sucursal, want_verbose);

  if ( argv[optind] == NULL ) {
    free(cfdi_pdf);
  }

  return res;
}
