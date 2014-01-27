/*
 * Copyright (c) 2012-2014, L3a,
 *			    Enrique Gámez Flores <egamez@edisson.com.mx>
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
#ifndef INERE_PERSONAFISICA_INCLUDED_H
#include "inere/personafisica.h"
#endif
#ifndef INERE_PERSONAMORAL_INCLUDED_H
#include "inere/personamoral.h"
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <getopt.h>

void
usage()
{
  printf("Programa para obtener, o verificar algunos parametros asociados con la clave\n");
  printf("del R.F.C.\n\nSintáxis:\n\n");
  printf(" -c/--homonimia\t\tObten la clave diferenciadora de homonimias. Se\n");
  printf("\t\t\trequerira tanto el nombre como los apellidos.\n");
  printf(" -g/--digito=RFC\tObten el digito verificador para este RFC abreviado. \n");
  printf(" -r/--rfc\t\tObten la clave del Registro Federal de Contribuyentes.\n");
  printf(" -s/--razon-social\tFija la razón social que se utilizara para el calculo de\n");
  printf("\t\t\tla clave del R.F.C.\n");
  printf(" -n/--nombre=NOMBRE\tFija el nombre que se utilizara para el calculo de la\n");
  printf("\t\t\tclave del R.F.C.\n");
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
main(int argc, char *argv[])
{
  int ch;
  int want_homonimia = 0;
  int want_verificador = 0;
  int want_rfc = 0;
  int want_verbose = 0;
  int want_verify = 0;
  char *razonsocial = NULL;
  char *nombre = NULL;
  char *paterno = NULL;
  char *materno = NULL;
  char *dia = NULL;
  char *mes = NULL;
  char *ano = NULL;
  char clave_diferenciadora[3];
  char *rfc_corto = NULL;
  char digito = 0;
  char rfc[18];

  /* options descriptor */
  static struct option longopts[] = {
      {"homonimia",	no_argument,		NULL,	'c'},
      {"digito",	required_argument,	NULL,	'g'},
      {"rfc",		no_argument,		NULL,	'r'},
      {"razon-social",	required_argument,	NULL,	's'},
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

  while ((ch=getopt_long(argc,argv,"cg:rs:n:p:t:d:m:a:b:vh",longopts,NULL)) != -1 ) {
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

      case 's':
	/* La razón social fue fijada */
	razonsocial = optarg;
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
	return 0;

      default:
	usage();
	return -1;

    }

  }
  argc -= optind;
  argv += optind;

  /* Verifica que alguno de los tres modos de funcionamiento hallan
   * sido seleccionados
   */
  if ( (want_rfc == want_homonimia) && (want_rfc == want_verificador) &&
       (want_rfc == want_verify) ) {
    usage();
    return 1;
  }


  if ( want_rfc ) {

    /* Verifica que los parametros requeridos para calcular la clave
     * del R.F.C. hallan sido proporcionados, ya que de no ser asi no
     * sera posible obtener la clave.
     */

    if ( dia == NULL ) {
      printf("Es necesario proporcionar el dia de nacimiento o constitución de la empresa, a travez de la opción -d/--dia\n");
    }
    if ( mes == NULL ) {
      printf("Es necesario proporcionar el mes de nacimiento o constitución de la empresa, a travez de la opción -m/--mes\n");
    }
    if ( ano == NULL ) {
      printf("Es necesario proporcionar el año de nacimiento o constitución de la empresa, a travez de la opción -a/--ano\n");
    }

    if ( nombre == razonsocial ) {
      /* No fue proporcionada la base para el calculo de la clave */
      printf("Es necesario proporcionar al nombre de la persona fisica (a travez de la opción -n/--nombre) o razón social (a travez de la opción -s/--razon-social)\n");
      return 2;
    }

    if ( razonsocial == NULL ) {
      /* Esto significa que el parametro --nombre fue proporcionado
       * Verifica que al menos un apellido halla sido proporcionado
       */
      if ( paterno == NULL ) {
	printf("Es necesario proporcionar al menos un apellido a travez de la opción -p/--paterno\n");
	return 3;
      }

    }

    if ( want_verbose ) {
      printf("Clave de el R.F.C. para:\n");
      if ( razonsocial == NULL ) {
	printf("          Nombre: %s\n Primer apellido: %s\n", nombre, paterno);
	if ( materno != NULL ) printf("Segundo apellido: %s\n", materno);
      } else {
	printf("Razón social: %s\n", razonsocial);
      }

      printf("Fecha:\n\tDia: %s\n\tMes: %s\n\tAño: %s\n", dia, mes, ano);
    }

    memset(rfc, 0, 18);
    if ( razonsocial == NULL )
      clave_rfc_persona_fisica(rfc, nombre, paterno, materno, ano, mes, dia, want_verbose);
    else
      clave_rfc_persona_moral(rfc, razonsocial, ano, mes, dia, want_verbose);

    if ( want_verbose ) printf("\nClave del R.F.C.: ");
    printf("%s\n", rfc);


  } else if ( want_homonimia ) {

    char *unombre = NULL;
    char *upaterno = NULL;
    char *umaterno = NULL;
    char *uapellidos = NULL;

    /* Haz un diagnostico de todos los parametros que se requieren
     * para obtener la clave difernciadora de homonimias.
     *
     * Se requieren:
     *		- Nombre/Denominación social
     *		- Apellido paterno (en caso de ser persona moral)
     *		- Apellido materno (en caso de existir y de ser persona moral)
     */

    if ( razonsocial == nombre ) {
      /* Ningun nombre fue especificado, no es posible calcular algo */
      printf("Es necesario especificar el nombre (a travez de la opción -n/--nombre) o la razón social del contribuyente (opción -s/--razon-social)\n");
      return 4;
    }

    if ( razonsocial == NULL ) {
      /* Esto significa que el parametro --nombre fue proporcionado
       * Verifica que al menos un apellido halla sido proporcionado
       */
      if ( paterno == NULL ) {
	printf("Es necesario proporcionar al menos un apellido a travez de la opción -p/--paterno\n");
	return 5;
      }

    }


    if ( razonsocial == NULL ) {

      /* Obten la clave diferenciadora de homonimias para una persona fisica */
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

    } else {
      /* Persona moral */
      unombre = to_upper_case_and_convert((unsigned char*)razonsocial);
    }

    if ( want_verbose ) {
      if ( razonsocial == NULL ) {
	printf("Nombre completo del contribuyente, para el cual se calculara la clave diferenciadora de homonimias: \"%s %s", nombre, paterno);
	if ( materno != NULL ) printf(" %s\".\n", materno);
	else		       printf("\".\n");

      } else {
	printf("Nombre del contribuyente, para el cual se calculara la clave diferenciadora de homonimias: \"%s\".\n", nombre);
      }

    }
    memset(clave_diferenciadora, 0, 3);
    homonimia(clave_diferenciadora, unombre, uapellidos, want_verbose);

    if ( want_verbose ) printf("Clave diferenciadora de homonimias:");
    printf("%s\n", clave_diferenciadora);
    if ( uapellidos != NULL ) free(uapellidos);
    if ( umaterno   != NULL ) free(umaterno);
    if ( upaterno   != NULL ) free(upaterno);
    free(unombre);


  } else if ( want_verificador ) {

    char *clave_corta = 0;

    clave_corta = to_upper_case_and_convert((unsigned char*)rfc_corto);
    digito = digito_verificador(clave_corta, want_verbose);
    if ( want_verbose ) printf("Digito verificador para la version incompleta del RFC '%s': ", rfc_corto);
    printf("%c\n", digito);
    free(clave_corta);

  } else if ( want_verify ) {

    char result = 0;
    size_t len = 0;
    char *tmp_clave = NULL;

    tmp_clave = to_upper_case_and_convert((unsigned char*)rfc);
    result = verifica_rfc(tmp_clave, want_verbose);

    len = strlen(tmp_clave);
    if ( result == 0 ) {
      printf("Digito de verificacion (\"%c\") coincide con el de la clave del R.F.C. propocionada.\n", tmp_clave[len-1]);

    } else if ( result == 100 ) {
      printf("No fue posible verificar la clave del R.F.C. proporcionada.\n");

    } else {
      printf("La clave de el R.F.C. proporcionada no tiene el mismo dígito verificador al calculado por este programa: \"%c\", mientras que el dígito de la clave proporcionada es \"%c\".\n", result, tmp_clave[len-1]);

    }

    free(tmp_clave);

  } else {
    usage();
    return -1;
  }

  return 0;
}
