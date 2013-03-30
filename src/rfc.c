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
#include <stdlib.h>
#include <stdio.h>

#include <getopt.h>

#ifndef INERE_PERSONAFISICA_INCLUDED_H
#include "inere/personafisica.h"
#endif
#ifndef INERE_HOMONIMIA_INCLUDED_H
#include "inere/homonimia.h"
#endif
#ifndef INERE_VERIFICADOR_INCLUDED_H
#include "inere/verificador.h"
#endif
#ifndef INERE_UTIL_INCLUDED_H
#include "inere/util.h"
#endif

void
usage()
{
  printf(" -c/--homonimia\t\tObten la clave diferenciadora de homonimias. Se\n");
  printf("\t\t\trequerira tanto el nombre como los apellidos.\n");
  printf(" -g/--digito=RFC\tObten el digito verificador para este RFC abreviado. \n");
  printf(" -r/--rfc\t\tObten la clave del Registro Federal de Contribuyentes.\n");
  printf(" -n/--nombre=NOMBRE\tFija el nombre o razon social que se utilizara\n");
  printf("\t\t\tpara el calculo del RFC.\n");
  printf(" -p/--paterno=APELLIDO\tFija el apellido paterno (o el apellido unico en caso de\n");
  printf("\t\t\tque el contribuyente tenga un unico apellido) en caso de\n");
  printf("\t\t\tpersona fisica que se utilizaran para el calculo del RFC\n");
  printf(" -t/--materno=APELLIDO\tFija el apellido materno, del contribuyente, necesario\n");
  printf("\t\t\tpara obtener el RFC.\n");
  printf(" -d/--dia=DIA\t\tFija el dia de nacimiento del contribuyente.\n");
  printf(" -m/--mes=MES\t\tFija el numero de mes de nacimiento.\n");
  printf(" -a/--ano=AÑO\t\tFija el año de nacimiento.\n");
  printf(" -b/--verifica=RFC\tVerifica que la clave del RFC suministrada\n");
  printf("\t\t\tcoincida con el digito tambien suministrado.\n");
  printf(" -v/--verbose\t\tSet the verbose mode.\n");
  printf(" -h/--help\t\tImprime este mensaje.\n");
  printf("\n\nBugs to: Enrique Gamez <egamez@edisson.com.mx>\n");
}

int
main(int argc, char* argv[])
{
  int ch;
  int want_homonimia = 0;
  int want_verificador = 0;
  int want_rfc = 0;
  int want_verbose = 0;
  int want_verify = 0;
  char* nombre = NULL;
  char* paterno = NULL;
  char* materno = NULL;
  char* dia = NULL;
  char* mes = NULL;
  char* ano = NULL;
  char clave_diferenciadora[3];
  char* rfc_corto = NULL;
  char digito = 0;
  char rfc[18];

  /* options descriptor */
  static struct option longopts[] = {
      {"homonimia",	no_argument,		NULL,	'c'},
      {"digito",	required_argument,	NULL,	'g'},
      {"rfc",		no_argument,		NULL,	'r'},
      {"nombre",	required_argument,	NULL,	'n'},
      {"paterno",	required_argument,	NULL,	'p'},
      {"materno",	required_argument,	NULL,	't'},
      {"dia",		required_argument,	NULL,	'd'},
      {"mes",		required_argument,	NULL,	'm'},
      {"ano",		required_argument,	NULL,	'a'},
      {"verifica",	required_argument,	NULL,	'b'},
      {"verbose",	no_argument,		NULL,	'v'},
      {"help",		no_argument,		NULL,	'h'},
      {NULL,		0,			NULL,	0}
    };

  while ((ch=getopt_long(argc,argv,"cg:rn:p:t:d:m:a:b:vh",longopts,NULL)) != -1 ) {
    switch(ch) {
      case 'c':
	/* Get the clave diferenciadora de homonimias */
	want_homonimia = 1;
	want_rfc = 0;
	want_verificador = 0;
	break;

      case 'g':
	/* Get the digito verificador */
	want_verificador = 1;
	want_homonimia = 0;
	want_rfc = 0;
	rfc_corto = optarg;
	break;

      case 'r':
	/* Obten la clave del RFC */
	want_rfc = 1;
	want_homonimia = 0;
	want_verificador = 0;
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

      case 'd':
	/* Set the day number to obtain the RFC */
	want_rfc = 1;
	dia = optarg;
	break;

      case 'm':
	/* Set the month number to obtain the RFC */
	want_rfc = 1;
	mes = optarg;
	break;

      case 'a':
	/* Set the yaer number to obtain the RFC */
	want_rfc = 1;
	ano = optarg;
	break;

      case 'b':
	/* Want to verify the RFC */
	want_verify = 1;
	snprintf(rfc, strlen(optarg)+1, optarg);
	break;

      case 'v':
	/* Verbosity */
	want_verbose = 1;
	break;

      case 'h':
	usage();
	break;

      default:
	usage();
	return -1;

    }

  }
  argc -= optind;
  argv += optind;

  if ( want_rfc ) {

    /* Check the availability of all the needed parameters */
    if ( nombre == NULL ) {
      fprintf(stderr, "Necesita proporcionar el nombre de la persona, opcion -n/--nombre.\n");
      usage();
      return 11;
    }
    if ( paterno == NULL ) {
      fprintf(stderr, "Necesita proporcionar al menos el primer apellido de la persona, opcion -p/--paterno.\n");
      usage();
      return 12;
    }
    if ( dia == NULL ) {
      fprintf(stderr, "Necesita proporcionar el dia del nacimiento de la persona, opcion -d/--dia.\n");
      usage();
      return 13;
    }
    if ( mes == NULL ) {
      fprintf(stderr, "Necesita proporcionar el numero del mes del nacimiento de la persona, opcion -m/--mes.\n");
      usage();
      return 14;
    }
    if ( ano == NULL ) {
      fprintf(stderr, "Necesita proporcionar el ano del nacimiento de la persona, opcion -a/--ano.\n");
      usage();
      return 15;
    }

    if ( want_verbose ) {
      printf("Clave de el RFC para:\nNombre: %s\nPrimer apellido: %s\n", nombre, paterno);
      if ( materno != NULL ) printf("Segundo apellido: %s\n", materno);
      printf("Fecha de nacimiento:\n\tDia: %s\n\tMes: %s\n\tAno: %s\n", dia, mes, ano);
    }
    memset(rfc, 0, 18);
    clave_rfc_persona_fisica(rfc, nombre, paterno, materno, ano, mes, dia, want_verbose);
    if ( want_verbose ) printf("RFC: ");
    printf("%s\n", rfc);


  } else if ( want_homonimia ) {

    char* unombre = 0;
    char* upaterno = 0;
    char* umaterno = 0;
    char* uapellidos = 0;

    /* Check if we have the name and the apellido(s) */
    if ( nombre == NULL ) {
      fprintf(stderr, "Necesita proporcionar el nombre de la persona, opcion -n/--nombre.\n");
      usage();
      return 11;
    }
    if ( paterno == NULL ) {
      fprintf(stderr, "Necesita proporcionar al menos el primer apellido de la persona, opcion -p/--paterno.\n");
      usage();
      return 12;
    }

    unombre = to_upper_case_and_convert((unsigned char*)nombre);
    upaterno = to_upper_case_and_convert((unsigned char*)paterno);
    if ( materno != NULL ) {
      umaterno = to_upper_case_and_convert((unsigned char*)materno);
      uapellidos = (char* )calloc(strlen(upaterno) + strlen(umaterno) + 2, sizeof(char));
      memset(uapellidos, 0, strlen(upaterno) + strlen(umaterno) + 2);
      strncpy(uapellidos, upaterno, strlen(upaterno));
      strncat(uapellidos, " ", 1);
      strncat(uapellidos, umaterno, strlen(umaterno));

    } else {
      uapellidos = (char* )calloc(strlen(upaterno) + 1, sizeof(char));
      memset(uapellidos, 0, strlen(upaterno) + 1);
      strncpy(uapellidos, upaterno, strlen(upaterno));
    }

    if ( want_verbose ) {
      printf("Nombre completo del contribuyente, para el cual se calculara la clave diferenciadora de homonimias: \"%s %s %s\".\n", nombre, paterno, (materno != NULL ? materno : ""));
    }
    memset(clave_diferenciadora, 0, 3);
    homonimia(clave_diferenciadora, unombre, uapellidos, want_verbose);

    if ( want_verbose ) printf("Clave diferenciadora de homonimias:");
    printf("%s\n", clave_diferenciadora);
    free(uapellidos);
    if ( umaterno != NULL ) free(umaterno);
    free(upaterno);
    free(unombre);


  } else if ( want_verificador ) {

    char* clave_corta = 0;

    clave_corta = to_upper_case_and_convert((unsigned char*)rfc_corto);
    digito = digito_verificador(clave_corta, want_verbose);
    if ( want_verbose ) printf("Digito verificador para la version incompleta del RFC '%s': ", rfc_corto);
    printf("%c\n", digito);
    free(clave_corta);

  } else if ( want_verify ) {

    char result = 0;
    size_t len = 0;
    char* tmp_clave = 0;

    tmp_clave = to_upper_case_and_convert((unsigned char*)rfc);
    result = verifica_rfc(tmp_clave, want_verbose);

    len = strlen(tmp_clave);
    if ( result == 0 ) {
      printf("Digito de verificacion (\"%c\") de la clave del RFC correcta.\n", tmp_clave[len-1]);
    } else {
      printf("Clave de el RFC incorrecta, el digito verificador para la clave suministrada es \"%c\", mientras que el digito suministrado es \"%c\".\n", result, tmp_clave[len-1]);
    }

    free(tmp_clave);

  } else {
    usage();
    return -1;
  }

  return 0;
}
