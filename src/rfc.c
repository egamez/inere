/*
 * Copyright (c) 2012, Enrique Gamez Flores <egamez@edisson.com.mx>,
 *                     L.A.E.
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
#include <stdlib.h>
#include <stdio.h>

#include <getopt.h>

#include "inere/personafisica.h"
#include "inere/homonimia.h"
#include "inere/verificador.h"

void
usage()
{
  printf(" -c/--homonimia\t\tObten la clave diferenciadora de homonimias. Se\n");
  printf("\t\t\trequerira tanto el nombre como los apellidos.\n");
  printf(" -k/--digito=RFC\tObten el digito verificador para este RFC abreviado. \n");
  printf(" -r/--rfc\t\tObten la clave del Registro Federal de Contribuyentes.\n");
  printf(" -n/--nombre=NOMBRE\tFija el nombre o razon social que se utilizara\n");
  printf("\t\t\tpara el calculo del RFC.\n");
  printf(" -p/--paterno=APELLIDO\tFija el apellido paterno (o el apellido unico en caso de\n");
  printf("\t\t\tque el contribuyente tenga un unico apellido) en caso de\n");
  printf("\t\t\tpersona fisica que se utilizaran para el calculo del RFC\n");
  printf(" -t/--materno=APELLIDO\tFija el apellido materno, del contribuyente, necesario\n");
  printf("\t\t\tpara obtener el RFC.\n");
  printf(" -i/--dia=DIA\t\tFija el dia de nacimiento del contribuyente.\n");
  printf(" -m/--mes=MES\t\tFija el numero de mes de nacimiento.\n");
  printf(" -a/--ano=ANO\t\tFija el ano de nacimiento.\n");
  printf(" -d/--debug\t\tSet the debug mode.\n");
  printf(" -v/--verbose\t\tSet the verbose mode.\n");
  printf(" -h/--help\t\tImprime este mensaje.\n");
  printf("\n\nBugs to: Enrique Gamez <egamez@edisson.com.mx>\n");
}

int
main(int argc, char* argv[])
{
  int ch;
  int want_homonimia = 0;
  int want_debug = 0;
  int want_verificador = 0;
  int want_rfc = 0;
  int want_verbose = 0;
  char* nombre = NULL;
  char* paterno = NULL;
  char* materno = NULL;
  char* dia = NULL;
  char* mes = NULL;
  char* ano = NULL;
  char clave_diferenciadora[3];
  char* rfc_corto = NULL;
  char digito = 0;
  char rfc[14];

  /* options descriptor */
  static struct option longopts[] = {
      {"homonimia",	no_argument,		NULL,	'c'},
      {"digito",	required_argument,	NULL,	'k'},
      {"rfc",		no_argument,		NULL,	'r'},
      {"nombre",	required_argument,	NULL,	'n'},
      {"paterno",	required_argument,	NULL,	'p'},
      {"materno",	required_argument,	NULL,	't'},
      {"dia",		required_argument,	NULL,	'i'},
      {"mes",		required_argument,	NULL,	'm'},
      {"ano",		required_argument,	NULL,	'a'},
      {"debug",		no_argument,		NULL,	'd'},
      {"verbose",	no_argument,		NULL,	'v'},
      {"help",		no_argument,		NULL,	'h'},
      {NULL,		0,			NULL,	0}
    };

  while ((ch=getopt_long(argc,argv,"ck:rn:p:t:i:m:a:dvh",longopts,NULL)) != -1 ) {
    switch(ch) {
      case 'c':
	/* Get the clave diferenciadora de homonimias */
	want_homonimia = 1;
	break;

      case 'k':
	/* Get the digito verificador */
	want_verificador = 1;
	rfc_corto = optarg;
	break;

      case 'r':
	/* Obten la clave del RFC */
	want_rfc = 1;
	break;

      case 'n':
	/* Set the name to obtain the RFC */
	nombre = optarg;
	break;

      case 'p':
	/* Set the apellido paterno, needed to build up the RFC */
	paterno = optarg;
	break;

      case 't':
	/* Set the apellido materno, needed to build up the RFC */
	materno = optarg;
	break;

      case 'i':
	/* Set the day number to obtain the RFC */
	dia = optarg;
	break;

      case 'm':
	/* Set the month number to obtain the RFC */
	mes = optarg;
	break;

      case 'a':
	/* Set the yaer number to obtain the RFC */
	ano = optarg;
	break;

      case 'd':
	/* Debug mode */
	want_debug = 1;
	break;

      case 'v':
	/* Verbosity */
	want_verbose = 1;
	break;

      case 'h':
	usage();
	break;

      default:
	printf("Hola tarola\n");
	usage();
	break;

    }

  }
  argc -= optind;
  argv += optind;

  if ( want_rfc ) {
    /* Check that the other parameters are present */
    if ( nombre == NULL ) {
      fprintf(stderr, "Necesita proporcionar el nombre de la persona, opcion -n/--nombre.\n");
      return 11;
    }
    if ( paterno == NULL ) {
      fprintf(stderr, "Necesita proporcionar al menos el primer apellido de la persona, opcion -p/--paterno.\n");
      return 12;
    }
    if ( dia == NULL ) {
      fprintf(stderr, "Necesita proporcionar el dia del nacimiento de la persona, opcion -i/--dia.\n");
      return 13;
    }
    if ( mes == NULL ) {
      fprintf(stderr, "Necesita proporcionar el numero del mes del nacimiento de la persona, opcion -m/--mes.\n");
      return 14;
    }
    if ( ano == NULL ) {
      fprintf(stderr, "Necesita proporcionar el ano del nacimiento de la persona, opcion -a/--ano.\n");
      return 15;
    }

    /* Now, obtain the RFC corto */
    if ( want_verbose ) {
      printf("Clave de el RFC para:\nNombre: %s\nPrimer apellido: %s\n", nombre, paterno);
      if ( materno != NULL ) printf("Segundo apellido: %s\n", materno);
      printf("Fecha de nacimiento:\n\tDia: %s\n\tMes: %s\n\tAno: %s\n", dia, mes, ano);
    }
    memset(rfc, 0, 14);
    fisica_clave_abreviada(rfc, nombre, paterno, materno, ano, mes, dia, want_debug);

    /* Now you need to obtain the clave diferenciadora de homonimias */
    want_homonimia = 1;

  }

  if ( want_homonimia ) {
    if ( nombre == NULL ) {
      fprintf(stderr, "Para obtener la clave diferenciadora de homonimias se requiere utilizar el nombre, denominacion o razon social del contribuyente.\n");
      return 1;

    } else {
      /* Obtain the homonimia */
      if ( paterno == NULL ) {
        /* This is the case for a persona moral */
        homonimia(clave_diferenciadora, nombre, NULL, want_debug);

      } else {
        char* apellidos = NULL;
        if ( materno != NULL ) {
          apellidos = (char* )calloc(strlen(paterno) + strlen(materno) + 2, sizeof(char));
          memset(apellidos, 0, strlen(paterno) + strlen(materno) + 2);
          strncpy(apellidos, paterno, strlen(paterno));
          strncat(apellidos, " ", 1);
          strncat(apellidos, materno, strlen(materno));

        } else {
          apellidos = (char* )calloc(strlen(paterno) + 1, sizeof(char));
          memset(apellidos, 0, strlen(paterno) + 1);
          strncpy(apellidos, paterno, strlen(paterno));
        }

	if ( (want_verbose && !want_rfc) || want_debug ) {
	  printf("Nombre completo del contribuyente, para el cual se calculara la clave diferenciadora de homonimias: \"%s %s\".\n", nombre, apellidos);
	}
        homonimia(clave_diferenciadora, nombre, apellidos, want_debug);
        free(apellidos);
      }

    }

    /* Print the results only if set the option from an option */
    if ( want_rfc ) {
      strncat(rfc, clave_diferenciadora, 2);
    } else {

      if ( want_verbose ) printf("Clave diferenciadora de homonimias");

      if ( materno == NULL && paterno == NULL ) {
        if ( want_debug ) printf(" para el nombre '%s': ", nombre);
      } else if ( materno == NULL ) {
        if ( want_debug ) printf(" para el nombre '%s, %s': ", paterno, nombre);
      } else {
        if ( want_debug ) printf(" para el nombre '%s %s, %s': ", paterno, materno, nombre);
      }

      if ( want_verbose ) printf(": ");
      printf("%s\n", clave_diferenciadora);
    }

  }

  /* Print the results */
  if ( want_rfc ) {
    char tmp[2];
    digito = digito_verificador(rfc, want_debug);
    snprintf(tmp, 2, "%c", digito);
    strncat(rfc, tmp, 1);
    /* And print the result */
    if ( want_verbose ) printf("RFC: ");
    printf("%s\n", rfc);
  } else {
    if ( want_verificador && rfc_corto != NULL ) {
      digito = digito_verificador(rfc_corto, want_debug);
      if ( want_verbose || want_debug ) printf("Digito verificador para la version incompleta del RFC '%s': ", rfc_corto);
      printf("%c\n", digito);
    }
  }

  return 0;



/*
  char *year = "77";
  char *month = "1";
  char *day = "26";
  char clave[11];

  memset(clave, 0, 11);

  if ( argc < 3 ) return 1;

  if ( argc == 4 ) fisica_clave(clave, argv[1], argv[2], argv[3], year, month, day, 1);
  else fisica_clave(clave, argv[1], argv[2] , NULL, year, month, day, 1);

  printf("Clave [%s].\n", clave);

  return 0;
*/
}
