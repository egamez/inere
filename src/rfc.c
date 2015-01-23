/*
 * Copyright (c) 2012-2015, L3a,
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
#ifndef INERE_PERSONAFISICA_H_
#include "inere/personafisica.h"
#endif
#ifndef INERE_PERSONAMORAL_H_
#include "inere/personamoral.h"
#endif
#ifndef INERE_HOMONIMIA_H_
#include "inere/homonimia.h"
#endif
#ifndef INERE_VERIFICADOR_H_
#include "inere/verificador.h"
#endif
#ifndef INERE_UTIL_H_
#include "inere/util.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <getopt.h>

/* Forward declaration */
void usage(void);


/**
 * Función para mostrar las opciones de uso
 */
void
usage(void)
{
  printf("Uso: rfc [MODO] [OPCIONES]\n");
  printf("\
Programa para obtener, o verificar algunos parametros asociados con la clave\n\
del R.F.C.\n\n");

  printf("\
Hay tres modos de operación del programa, y debe de elegirse al menos uno de\n\
ellos:\n\n");

  printf("\
  -R, --rfc          Calcula la clave del R.F.C. (sera necesario proporcionar\n\
                     los parametros para dicho calculo.)\n");
  printf("\
  -H, --homonimia    Obten la clave diferenciadora de homonimos.\n\
  -D, --digito       Calcula el digito verificador de la clave (incompleta)\n\
                     suministrada.\n");

  printf("\nOpciones requeridas para el calculo de la clave del R.F.C.:\n\n");

  printf("\
  -d DIA, --dia=DIA  Define el día de nacimiento de la persona, o día de la\n\
                     fecha de constitución.\n");
  printf("\
  -m MES, --mes=MES  Define el mes de nacimiento de la persona, o mes de la\n\
                     fecha de constitución.\n");
  printf("\
  -a AÑO, --anio=AÑO Define el año de nacimiento de la persona, o año de la\n\
                     fecha de constitución.\n");

  printf("\ny para el caso de la clave del R.F.C. para una persona física:\n\n");

  printf("\
  -N NOMBRE, --nombre=NOMBRE\n\
                     Define el nombre de la persona.\n");
  printf("\
  -P APELLIDO, --paterno=APELLIDO\n\
                     Define el apellido paterno, o primer apellido de la persona\n");
  printf("\
  -M APELLIDO, --materno=APELLIDO\n\
                     Define el apellido materno, o segundo apellido de la\n\
                     persona en caso de que exista.\n");

  printf("\npero para el caso de la clave para una persona moral:\n\n");

  printf("\
  -r NOMBRE, --razon-social=NOMBRE\n\
                     Razón o denominación social.\n");
  printf("\
  -t TIPO, --tipo-de-sociedad=TIPO\n\
                     Siglas que indican el tipo de sociedad (S.A. de C.V., etc.\n\n");


  printf("Opciones requeridas para el calculo de la clave diferenciadora de homonimos:\n\n");

  printf("\
  -N NOMBRE, --nombre=NOMBRE\n\
                     Define el nombre de la persona.\n");
  printf("\
  -P APELLIDO, --paterno=APELLIDO\n\
                     Define el apellido paterno, o primer apellido de la persona\n");
  printf("\
  -M APELLIDO, --materno=APELLIDO\n\
                     Define el apellido materno, o segundo apellido de la\n\
                     persona en caso de que exista.\n");

  printf("\npero para el caso de la clave para una persona moral:\n\n");

  printf("\
  -r NOMBRE, --razon-social=NOMBRE\n\
                     Razón o denominación social.\n");
  printf("\
  -t TIPO, --tipo-de-sociedad=TIPO\n\
                     Siglas que indican el tipo de sociedad (S.A. de C.V., etc.\n\n");

  printf("\
Para el modo de generar el digito de verificación de la clave:\n\n");

  printf("\
  -D RFC, --digito=RFC\n\
                     Obten el digito verificador para este RFC abreviado.\n\n");

  printf("Opciones adicionales:\n\n");
  printf("\
  -V RFC, --verifica=RFC\n\
                     Verifica que la clave del R.F.C. suministrada coincida con\n\
                     el digito tambien suministrado.\n");
  printf("\
  -v, --verbose      Imprime algunos mensajes extra durante la ejecución.\n\
  -h, --help         Imprime este mensaje.\n");

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
  const char *razonsocial = NULL;
  const char *tiposociedad = NULL;
  const char *nombre = NULL;
  const char *paterno = NULL;
  const char *materno = NULL;
  const char *dia = NULL;
  const char *mes = NULL;
  const char *ano = NULL;
  const char *rfc_corto = NULL;
  char clave_diferenciadora[3];
  char digito = 0;
  char rfc[18];

  /* options descriptor */
  static struct option longopts[] = {
      {"homonimia",		no_argument,		NULL,	'H'},
      {"digito",		required_argument,	NULL,	'D'},
      {"rfc",			no_argument,		NULL,	'R'},
      {"razon-social",		required_argument,	NULL,	'r'},
      {"tipo-de-socieda",	required_argument,	NULL,	't'},
      {"nombre",		required_argument,	NULL,	'N'},
      {"paterno",		required_argument,	NULL,	'P'},
      {"materno",		required_argument,	NULL,	'M'},
      {"dia",			required_argument,	NULL,	'd'},
      {"mes",			required_argument,	NULL,	'm'},
      {"anio",			required_argument,	NULL,	'a'},
      {"verifica",		required_argument,	NULL,	'V'},
      {"verbose",		no_argument,		NULL,	'v'},
      {"help",			no_argument,		NULL,	'h'},
      {NULL,			0,			NULL,	 0 }
    };

  while ((ch=getopt_long(argc,argv,"HD:Rr:t:N:P:M:d:m:a:V:vh",longopts,NULL)) != -1 ) {
    switch(ch) {
      case 'H':
	/* Get the clave diferenciadora de homonimias */
	want_homonimia = 1;
	want_rfc = 0;
	want_verificador = 0;
	break;

      case 'D':
	/* Get the digito verificador */
	want_verificador = 1;
	want_homonimia = 0;
	want_rfc = 0;
	rfc_corto = optarg;
	break;

      case 'R':
	/* Obten la clave del RFC */
	want_rfc = 1;
	want_homonimia = 0;
	want_verificador = 0;
	break;

      case 'r':
	/* La razón social fue fijada */
	razonsocial = optarg;
	break;

      case 't':
	/* Tipo de sociedad de la persona moral */
	tiposociedad = optarg;
	break;

      case 'N':
	/* Set the name to obtain the RFC */
	nombre = optarg;
	break;

      case 'P':
	/* Set the apellido paterno, needed to build up the RFC */
	paterno = optarg;
	break;

      case 'M':
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

      case 'V':
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
      printf("Es necesario proporcionar el año de nacimiento o constitución de la empresa, a travez de la opción -a/--anio\n");
    }

    if ( nombre == razonsocial ) {
      /* No fue proporcionada la base para el calculo de la clave */
      printf("Es necesario proporcionar al nombre de la persona fisica (a travez de la opción -N/--nombre) o razón social (a travez de la opción -r/--razon-social)\n");
      return 2;
    }

    if ( razonsocial == NULL ) {
      /* Esto significa que el parametro --nombre fue proporcionado
       * Verifica que al menos un apellido halla sido proporcionado
       */
      if ( paterno == NULL ) {
	printf("Es necesario proporcionar al menos un apellido a travez de la opción -P/--paterno\n");
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
	if ( tiposociedad != NULL ) printf("Tipo de sociedad: %s\n", tiposociedad);
      }

      printf("Fecha:\n\tDia: %s\n\tMes: %s\n\tAño: %s\n", dia, mes, ano);
    }

    memset(rfc, 0, 18);
    if ( razonsocial == NULL )
      clave_rfc_persona_fisica(rfc, nombre, paterno, materno, ano, mes, dia, want_verbose);
    else
      clave_rfc_persona_moral(rfc, razonsocial, tiposociedad, ano, mes, dia, want_verbose);

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
      printf("Es necesario especificar el nombre (a travez de la opción -N/--nombre) o la razón social del contribuyente (opción -r/--razon-social)\n");
      return 4;
    }

    if ( razonsocial == NULL ) {
      /* Esto significa que el parametro --nombre fue proporcionado
       * Verifica que al menos un apellido halla sido proporcionado
       */
      if ( paterno == NULL ) {
	printf("Es necesario proporcionar al menos un apellido a travez de la opción -P/--paterno\n");
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
      if ( tiposociedad != NULL ) upaterno = to_upper_case_and_convert((unsigned char*)tiposociedad);
    }

    if ( want_verbose ) {
      if ( razonsocial == NULL ) {
	printf("Nombre completo del contribuyente, para el cual se calculara la clave diferenciadora de homonimias: \"%s %s", nombre, paterno);
	if ( materno != NULL ) printf(" %s\".\n", materno);
	else		       printf("\".\n");

      } else {
	printf("Nombre del contribuyente, para el cual se calculara la clave diferenciadora de homonimias: \"%s", nombre);
	if ( tiposociedad != NULL ) printf(", %s", tiposociedad);
	printf("\"\n");
      }

    }
    memset(clave_diferenciadora, 0, 3);
    if ( tiposociedad == NULL )
      homonimia(clave_diferenciadora, unombre, uapellidos, want_verbose);
    else
      homonimia(clave_diferenciadora, unombre, upaterno, want_verbose);


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
