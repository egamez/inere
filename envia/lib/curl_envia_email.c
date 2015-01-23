/*
 * Copyright (c) 2014-2015, L3a,
 *                          Enrique Gámez Flores <egamez@edisson.com.mx>
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
#include "inere/envia/config.h"
#ifndef INERE_ENVIA_CURL_ENVIA_EMAIL_H_
#include "inere/envia/curl_envia_email.h"
#endif
#ifndef BASE64_H
#include "inere/envia/base64.h"
#endif
#ifndef INERE_ENVIA_NON_ASCII_H_
#include "inere/envia/non_ascii.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <time.h>
#include <locale.h>

#include <curl/curl.h>

#ifdef ENVIA_UTILIZA_UUID
# include <uuid.h>
#else
# include "inere/envia/genera_cadena.h"
#endif

typedef struct {
  char *data;
  size_t len;
  size_t pos;
} imf_mensaje_t;

/* Forward declarations */
static size_t payload_mensaje(void *ptr, size_t size, size_t nmemb,void *userp);
int tiene_extension(const char *filename, const char *extension);
char *obten_fecha_RFC2822(char *fecha, const size_t len);
char *obten_identificador(const unsigned int modo);
int agrega_encabezado(char *mensaje, const char *display_name_from,
		      const char *from, const char *asunto,
		      const char *keywords, const char *organization,
		      const char *importance, const char *sensitivity,
		      const char *boundary, const int verbose);
int agrega_destinatarios(char *mensaje, const char *display_name_to,
			 const char *to, const char *cc, const int verbose);
int agrega_destinatarios_mult(char *mensaje, name_addr_t *to, name_addr_t *cc,
			      const int verbose);
int agrega_cuerpo(char *mensaje, const char *body_filename, const char *cfdi,
		  const char *pdf, const char *boundary, const int verbose);
int agrega_archivo(const char *source, const char *boundary, char *output,
		const int verbose);
int curl_envia_email_enverdad(const char *username, const char *password,
			const char *mailserver, imf_mensaje_t mensaje,
			struct curl_slist *recipients, const int insecure,
			const int verbose);


/**
 * Función para agegar a la linked list 'head' alguna otra name_addr
 * a la cual también enviar el mensaje.
 */
name_addr_t *
name_addr_list_append(name_addr_t *head, const char *nombre, const char *buzon)
{
  char buffer[80];
  name_addr_t *current =  head;
  name_addr_t *tmp = NULL;

  tmp = (name_addr_t *)malloc(sizeof(name_addr_t));
  if ( tmp == NULL ) {
    /* Ocurrio un error, no va a ser posible agregar los datos */
    return head;
  }
  tmp->display_name = strdup(nombre);
  memset(buffer, 0, 80);
  snprintf(buffer, 80, "<%s>", buzon);
  tmp->angle_addr = strdup(buffer);

  if ( head == NULL ) {
    /* Create the first element in the list */
    head = tmp;
    head->next = NULL;

  } else {
    /* Reach the end of the list */
    while ( current->next != NULL ) {
      current = current->next;
    }

    /* Append at the end of the list */
    tmp->next = NULL;
    current->next = tmp;
  }
  return  head;
}

/**
 * Función para actualizar el nombre que identifica al buzón de correo
 * electrónico. Esta función es particularmente util en un ambiente
 * en el que el buzon y el nombre a desplegar son suministrador por
 * separado, por ejemplo en el programa: src/envia.c
 *
 */
name_addr_t *
name_addr_list_update_name(name_addr_t *head, const char *mailbox, const char *display)
{
  char buffer[80];
  name_addr_t *current =  head;

  /* Build up the angle address mail box */
  memset(buffer, 0, 80);
  snprintf(buffer, 80, "<%s>", mailbox);

  /* Make a loop through all the entries in the list till you find the
   * one with the mailbox passed */
  while ( current != NULL ) {
    if ( strcmp(buffer, current->angle_addr) == 0 ) {
      /* This is the entry that we are looking for, update the display
       * name */
      if ( current->display_name != NULL ) {
	free(current->display_name);
	current->display_name = strdup(display);
      }
      break;
    }
    current = current->next;
  }
  return current;
}

/**
 * Limpia la linked list de los buzones a los cuales se envia el mensaje
 */
void
name_addr_list_clear(name_addr_t *head)
{
  name_addr_t *node = head;
  name_addr_t *tmp = NULL;

  while ( node != NULL ) {
    tmp = node;
    free(tmp->display_name);
    free(tmp->angle_addr);
    node = node->next;
    free(tmp);
  }
  head = NULL;
}


/**
 * Función utilizada por libcURL para leer el cuerpo del mensaje.
 */
static size_t
payload_mensaje(void *ptr, size_t size, size_t nmemb, void *stream)
{
  imf_mensaje_t *mensaje = (imf_mensaje_t *)stream;
  size_t len = mensaje->len - mensaje->pos;

  if ( len > size * nmemb ) len = size * nmemb;

  memcpy(ptr, mensaje->data + mensaje->pos, len);
  mensaje->pos += len;

  return len;
}

/**
 * Función auxiliar para determinar la extensión en el nombre de un archivo
 * Argumentos:
 *		file		Nombre del archivo
 *		extension	La extensión a verificar
 *
 * Valor devuelto:
 *		0		Si el nombre del archivo no tiene la extensión
 *				requerida
 *		1		Si el nombre del archivo si tiene la extensión
 *
 * http://stackoverflow.com/questions/5309471/getting-file-extension-in-c
 */
int
tiene_extension(const char *filename, const char *extension)
{
  int res = 0;
  const char *dot = NULL;
  if ( filename == NULL ) {
    return 0;
  }
  dot = strrchr(filename, '.');
  if ( !dot || dot == filename ) {
    /* No es posible realizar el test. */
    return 0;
  } else {
    /* Haz el test para la extension */
    if ( strcmp(extension, dot + 1) == 0 ) {
      /*printf("\tEsta es la extension buscada.\n");*/
      res = 1;
    } else {
      /*printf("\tNo tiene la extension.\n");*/
      res = 0;
    }
  }
  return res;
}


/**
 * Funcion para agregar a un archivo la lectura de otro archivo en formato
 * base64
 *
 * Sintaxis:
 *	source		El nombre del archivo a codificar y agregar
 *	boundary	El delimitador usado para las secciones en el mensaje
 *	output		Pointer to the file where all the message its put
 *	type		Tipo de archivo a agregar
 *				1	Archivo adjunto tipo application/xml
 *				2	Archivo adjunto tipo application/pdf
 *	verbose		Flag para imprimir mensajes
 *
 * La función regresa:
 *
 *      0       En caso de exito
 *      1       En caso de que el archivo que se va adjuntar no se halla
 *                podido abrir.
 *      2       En caso de que el archivo a adjuntar, despues de abierto
 *                no se halla podido leer.
 */
int
agrega_archivo(const char *source, const char *boundary, char *output,
		const int verbose)
{
  int len = 0;
  int read = 0;
  char *buffer = NULL;
  char *encoded = NULL;
  int i = 0;
  int j = 0;
  char *basec = NULL;
  char *bname = NULL;
  char *tmp = NULL; /* For non_ascii encoding convertions */
  FILE *file = NULL;
  int tipo = 0; /* Tipo		0	No defininido
		 *		1	xml
		 *		2	pdf
		 *		-1	txt
		 *		-2	html
		 */

  /* Abre el achivo a cargar */
  file = fopen(source, "rb");
  if ( file == NULL ) {
    if ( verbose ) fprintf(stderr, "No fue posible abrir el archivo %s.\n", source);
    return 1;
  }

  len = fseek(file, 0, SEEK_END);
  if ( len ) {
    /* Error */
    if ( verbose ) fprintf(stderr, "No fue posible poner obtener el final del archivo %s.\n", source);
    fclose(file);
    return 2;
  }

  /* Elimina la posible ruta local del archivo adjunto */
  basec = strdup(source);
  bname = basename(basec);

  /* Escribe el header de este attachment */
  strncat(output, "--", 2);
#ifdef ENVIA_UTILIZA_UUID
  strncat(output, boundary, 38);
#else
  strncat(output, boundary, 30);
#endif
  strncat(output, "\r\n", 2);

  /* Archivo ajunto
   * Determina el tipo de archivo tan solo por su extension */

  if ( tiene_extension(bname, "xml") || tiene_extension(bname, "XML") ) {
    /* CFDi: extension 'xml' */
    strncat(output, "Content-Type: application/xml;\r\n", 32);
    tipo = 1;

  } else if ( tiene_extension(bname, "pdf") || tiene_extension(bname, "PDF") ) {
    /* La representación impresa del CFDi: extension 'pdf' */
    strncat(output, "Content-Type: application/pdf;\r\n", 32);
    tipo = 2;

  } else if ( tiene_extension(bname, "txt") || tiene_extension(bname, "TXT") ) {
    /* Un archivo de texto comun: extension 'txt' */
    strncat(output, "Content-Type: text/plain; charset=utf-8\r\n", 41);
    tipo = -1;

  } else if ( tiene_extension(bname, "html") || tiene_extension(bname, "htm") ) {
    /* Un archivo de con markup HTML: extension 'html' */
    strncat(output, "Content-Type: text/html; charset=utf-8\r\n", 40);
    tipo = -2;

  } else {
    /* Un archivo fuera de lo comun. Agregalo como un archivo adjunto */
    strncat(output, "Content-Type: application/octet-stream;\r\n", 41);
    tipo = 0;

  }

  if ( tipo >= 0 ) {
    /* Este es un archivo adjunto */
    strncat(output, "\tname=\"", 7);
    len = non_ascii_alloc(bname, &tmp);
    if ( len != 0 ) {
      /* Esto significa que el nombre del archivo contiene caracteres no ascii*/
      strncat(output, tmp, len);
    } else {
      /* Caracteres ascii, puedes pasarlos tal y como estan */
      strncat(output, bname, strlen(bname));
      len = 0;
    }
    strncat(output, "\"\r\n", 3);

    strncat(output, "Content-Disposition: attachment;\r\n", 34);
    strncat(output, "\tfilename=\"", 11);
    if ( len != 0 ) {
      /* Esto significa que el nombre del archivo contiene caracteres no ascii*/
      strncat(output, tmp, len);
    } else {
      /* Caracteres ascii, puedes pasarlos tal y como estan */
      strncat(output, bname, strlen(bname));
    }
    strncat(output, "\"\r\n", 3);
  }

  strncat(output, "Content-Transfer-Encoding: base64\r\n", 35);
  strncat(output, "\r\n", 2);

  if ( tmp != NULL ) {
    free(tmp);
    tmp = NULL;
  }
  free(basec);

  /* Ahora codifica el contenido del archivo */
  len = ftell(file);
  rewind(file);
  buffer = (char *)calloc(len + 1, sizeof(char));
  read = fread(buffer, sizeof(char), len, file);
  fclose(file);
  /* Ahora, codifica el contenido en 'buffer' */
  base64_encode_alloc(buffer, len, &encoded);
  free(buffer);
  /* Ahora escribe el archivo codificado */
  len = strlen(encoded);
  j = strlen(output);
  output += j;
  while ( *encoded ) {
    *output++ = *encoded++;
    j++;
    i++;
    if ( i == 76 ) {
      *output++ = '\r';
      *output++ = '\n';
      j += 2;
      i = 0;
    }
  }
  encoded -= len;
  free(encoded);
  output -= j;

  /* Ahora agrega el separador */
  strncat(output, "\r\n\r\n", 4);

  return 0;
}

/**
 * Funcion auxiliar para generar el formato de fecha compatible con
 * el RFC5322, con una extension de agregar el nombre de la zona horaria
 * como un comentario en el mensaje.
 */
char *
obten_fecha_RFC2822(char *fecha, const size_t len)
{
  time_t t;
  struct tm tiempo;
  size_t res;
  char *locale = NULL;

  /* Salva la localizacion previa */
  locale = setlocale(LC_ALL, NULL);

  /* Regresa al locale "C" */
  setlocale(LC_ALL, "C");

  memset(fecha, 0, len);
  t = time(&t);
  localtime_r(&t, &tiempo);
  res = strftime(fecha, len, "%a, %d %b %Y %H:%M:%S %z (%Z)", &tiempo);
  if ( res == 0 ) {
    /* No se pudo realizar la conversion */
    memset(fecha, 0, len);
  }
  /* Restaura la localizacion de inicio */
  setlocale(LC_ALL, locale);

  return fecha;
}

/**
 * Funcion para obtenr una secuencia de caracteres que serviran para
 * ser utilizados como identificador del mensaje 'Message-ID' (modo 1)
 * y para generar el delimitador de las diferentes secciones en el cuerpo
 * del mensaje 'boundary' (modo 2)
 *
 * Hay dos opciones para esto, utilizar UUIDs o generar una cadena de
 * caracteres aleatorios. La opción por default es generar cadenas de
 * caracteres aleatorios, con lo cual no se requerira de dependencia externa
 * alguna. Estas opciones seran seleccionadas al momento de compilar
 * el programa.
 *
 * El usuario será responsable de liberar (con free())la memoria generada por el
 * llamado de esta funcion.
 *
 *
 * En caso de haber elegido utilizar UUIDs...
 * Los modos basicos son
 *
 *	Version 1 (time and node based)
 *		UUID_MAKE_V1
 *
 *	Version 4 (random data based)
 *		UUID_MAKE_V4
 *
 */
char *
obten_identificador(const unsigned int modo)
{
  char *identificador = NULL;

#ifdef ENVIA_UTILIZA_UUID
  uuid_t *uuid_mess = NULL;

  uuid_create(&uuid_mess);
  uuid_make(uuid_mess, modo);
  uuid_export(uuid_mess, UUID_FMT_STR, (void **)&identificador, NULL);
  uuid_destroy(uuid_mess);

#else
  int size = 0;
  if ( modo == 1 ) size = 51; /* Identificador, tipo GMail */
  else		   size = 29; /* Delimitador, tipo GMail */

  identificador = (char *)calloc(size, sizeof(char));
  identificador = genera_cadena(identificador, size, modo);

#endif

  return identificador;
}

/**
 * Esta funcion creara un archivo que contendra el mensaje a enviar.
 * Este mensaje creara los campos de encabezado (header fields):
 *
 *	Date		(obligatorio)
 *	From		(obligatorio)
 *	To
 *	Message-ID
 *	Subject
 *
 *	Content-Type
 *	MIME-Version	(default: 1.0)
 *	Importance
 *	Sensitivity
 *	Organization
 *	User-Agent	PACKAGE_NAME - Custom libcURL Mail Program PACKAGE_VERSION
 *	Keywords
 *
 */
int
agrega_encabezado(char *mensaje, const char *display_name_from,
		  const char *from, const char *asunto, const char *keywords,
		  const char *organization, const char *importance,
		  const char *sensitivity, const char *boundary,
		  const int verbose)
{
  char buffer[81];
  char fecha[50];
  char *uuid = NULL;
  char *tmp = NULL; /* Para conversiones de caracteres que no sean ascii */
  unsigned int len = 0; /* Tambien para las conversiones */

  /* Comienza haciendo un chequeo de que las partes basicas para crear
   * y enviar el mensaje fueron suministradas
   */
  if ( from == NULL ) {
    /* Error 1. El campo de encabezado From es obligatorio */
    return 1;
  }

  /* La fecha
   * Campo de encabezado:
   *	Date
   */
  obten_fecha_RFC2822(fecha, 50);
  strncat(mensaje, "Date: ", 6);
  strncat(mensaje, fecha, strlen(fecha));
  strncat(mensaje, "\r\n", 2);

  /* El ID del mensaje. Utiliza un UUID version 1
   * Campo de encabezado:
   *	Message-ID
   */
  strncat(mensaje, "Message-ID: <", 13);
#ifdef ENVIA_UTILIZA_UUID
  uuid = obten_identificador(UUID_MAKE_V1);
  strncat(mensaje, uuid, 36);
#else
  uuid = obten_identificador(1);
  strncat(mensaje, uuid, 50);
#endif
  free(uuid);
  strncat(mensaje, "@", 1);
  strncat(mensaje, ENVIA_HOSTNAME, strlen(ENVIA_HOSTNAME));
  strncat(mensaje, ">\r\n", 3);

  /* Ahora los campos de encabezado complementarios*/

  /* Content-Type
   * Necesitamos utilizar como delimitador de secciones, el suministrado
   * como argumento de esta funcion.
   * Utiliza comillas (") para definir el delimitador.
   */
  strncat(mensaje, "Content-Type: multipart/mixed;\r\n", 32);
  strncat(mensaje, "\tboundary=\"", 11);
#ifdef ENVIA_UTILIZA_UUID
  strncat(mensaje, boundary, 38);
#else
  strncat(mensaje, boundary, 30);
#endif
  strncat(mensaje, "\"\r\n", 3);

  /* MIME-Version */
  strncat(mensaje, "MIME-Version: 1.0\r\n", 19);

  /* Importance */
  if ( importance != NULL ) {
    memset(buffer, 0, 81);
    snprintf(buffer, 81, "Importance: %s\r\n", importance);
    strncat(mensaje, buffer, strlen(buffer));
  }

  /* Sensitivity */
  if ( sensitivity != NULL ) {
    memset(buffer, 0, 81);
    snprintf(buffer, 81, "Sensitivity: %s\r\n", sensitivity);
    strncat(mensaje, buffer, strlen(buffer));
  }

  /* Organization */
  if ( organization != NULL ) {
    memset(buffer, 0, 81);
    /* Verifica que todos los caracteres en organization sean ascii */
    len = non_ascii_alloc(organization, &tmp);
    if ( tmp != 0 ) {
      /* Algunos caracteres en organization no son ascii, codificalos */
      snprintf(buffer, 81, "Organization: %s\r\n", tmp);
      free(tmp);
      tmp = NULL;

    } else {
      snprintf(buffer, 81, "Organization: %s\r\n", organization);
    }
    strncat(mensaje, buffer, strlen(buffer));
  }

  /* User-Agent */
  memset(buffer, 0, 81);
  snprintf(buffer, 81, "User-Agent: %s - Custom libcURL Mail Program %s\r\n", PACKAGE_NAME, PACKAGE_VERSION);
  strncat(mensaje, buffer, strlen(buffer));

  /* Keywords */
  if ( keywords != NULL ) {
    memset(buffer, 0, 81);
    /* Verifica que todos los caracteres en organization sean ascii */
    len = non_ascii_alloc(keywords, &tmp);
    if ( tmp != 0 ) {
      /* Algunos caracteres en organization no son ascii, codificalos */
      snprintf(buffer, 81, "Keywords: %s\r\n", tmp);
      free(tmp);
      tmp = NULL;

    } else {
      /* Todos los caracteres son ascii */
      snprintf(buffer, 81, "Keywords: %s\r\n", keywords);
    }
    strncat(mensaje, buffer, strlen(buffer));
  }

  /* El asunto
   * Campo de encabezado:
   *	Subject
   */
  strncat(mensaje, "Subject: ", 9);
  if ( asunto != NULL ) {
    /* Verifica tambien si alguno de los caracteres en el asunto no son ascii */
    len = non_ascii_alloc(asunto, &tmp);
    if ( len != 0 ) {
      /* Hay caracteres que no son todos ascii, codificalos */
      strncat(mensaje, tmp, len);
      free(tmp);
      tmp = NULL;

    } else {
      /* Todos los caracteres en asunto son ascii, es seguro pasarlos asi */
      strncat(mensaje, asunto, strlen(asunto));
    }
  } else {
    /* Agrega un subject default */
    if ( verbose ) fprintf(stderr, "Mensaje sin Subject, agregando uno por defecto\n");
    strncat(mensaje, "Envio de un CFDi", 16);
  }
  strncat(mensaje, "\r\n", 2);

  /* El remitente
   * Campo de encabezado:
   *	From
   * Utiliza la especificacion name-addr del RFC5322
   *		[display-name] angle-addr
   */
  memset(buffer, 0, 81);
  if ( display_name_from != NULL ) {
    /* Verifica que si hay caracteres no ascii */
    len = non_ascii_alloc(display_name_from, &tmp);
    if ( len != 0 ) {
      /* Algunos caracteres del display name no son ascii, codificalos */
      snprintf(buffer, 81, "%s <%s>\r\n", tmp, from);
      free(tmp);
      tmp = NULL;

    } else {
      /* Todos los caracteres son ascii, pasalos como tal */
      snprintf(buffer, 81, "%s <%s>\r\n", display_name_from, from);
    }
  } else {
    snprintf(buffer, 81, "<%s>\r\n", from);
  }
  strncat(mensaje, "From: ", 6);
  strncat(mensaje, buffer, strlen(buffer));

  /* Reply-To */
  strncat(mensaje, "Reply-To: ", 10);
  strncat(mensaje, buffer, strlen(buffer));

  return 0;
}

/**
 *
 */
int
agrega_destinatarios(char *mensaje, const char *display_name_to, const char *to,
		     const char *cc, const int verbose)
{
  char buffer[81];
  size_t len = 0;
  char *tmp = NULL; /* Para conversiones de caracteres que no sean ascii */

  /* El destinatario
   * Campo de encabezado:
   *	To
   * Utiliza la especificacion name-addr del RFC5322
   */
  memset(buffer, 0, 81);
  if ( display_name_to != NULL ) {
    /* Verifica que si hay caracteres no ascii */
    len = non_ascii_alloc(display_name_to, &tmp);
    if ( len != 0 ) {
      /* Algunos caracteres del display name no son ascii, codificalos */
      if ( verbose ) {
	printf("Algunos de los caracteres en '%s' no son ASCII, se codificaran a UTF-8. El resultado será: %s\n", display_name_to, tmp);
      }
      snprintf(buffer, 81, "%s <%s>\r\n", tmp, to);
      free(tmp);
      tmp = NULL;

    } else {
      /* Todos los caracteres son ascii, pasalos como tal */
      snprintf(buffer, 81, "%s <%s>\r\n", display_name_to, to);
    }
  } else {
    snprintf(buffer, 81, "<%s>\r\n", to);
  }
  strncat(mensaje, "To: ", 4);
  strncat(mensaje, buffer, strlen(buffer));

  /**
   * Carbon copy
   */
  if ( cc != NULL ) {
    memset(buffer, 0, 81);
    snprintf(buffer, 81, "Cc: <%s>\r\n", cc);
    strncat(mensaje, buffer, strlen(buffer));
  }

  return 0;
}

/**
 *
 */
int
agrega_destinatarios_mult(char *mensaje, name_addr_t *to, name_addr_t *cc,
			  const int verbose)
{
  char buffer[321];
  char box[81];
  size_t len = 0;
  name_addr_t *current = NULL;
  char *tmp = NULL; /* Para conversiones de caracteres que no sean ascii */

  /* El destinatario
   * Campo de encabezado:
   *	To:
   * Utiliza la especificacion name-addr del RFC5322
   */
  memset(buffer, 0, 321);
  strncat(buffer, "To: ", 4);

  /* Inicia el loop */
  current = to;
  while ( current != NULL ) {

    memset(box, 0, 81);
    /* Verifica que si hay caracteres no ascii */
    len = non_ascii_alloc(current->display_name, &tmp);
    if ( len != 0 ) {
      /* Algunos caracteres del display name no son ascii, codificalos */
      if ( verbose ) {
	printf("Algunos de los caracteres en '%s' no son ASCII, se codificaran a UTF-8. El resultado será: %s\n", current->display_name, tmp);
      }
      snprintf(box, 81, "%s %s", tmp, current->angle_addr);
      free(tmp);
      tmp = NULL;

    } else {
      /* Todos los caracteres son ascii, pasalos como tal.
       * aunque aqui existe la posibilidad de que no halla display_name */
      if ( strlen(current->display_name) ) {
	snprintf(box, 81, "%s %s", current->display_name, current->angle_addr);
      } else {
	snprintf(box, 81, "%s", current->angle_addr);
      }

    }
    strncat(buffer, box, strlen(box));

    current = current->next;
    /* Verifica aqui si hay mas mailboxes para agregar, y en caso de que así
     * sea sera necesario agregar ',' como separador de los buzones */
    if ( current != NULL ) {
      /* Si hay más buzones, agrega el separador */
      strncat(buffer, ",", 1);
    }
    /* Agrega el terminador de la linea */
    strncat(buffer, "\r\n", 2);

  }

  /* Agrega el campo de encabezado To: */
  strncat(mensaje, buffer, strlen(buffer));


  /**
   * Carbon copy
   * Campo de encabezado Cc:
   */
  if ( cc != NULL ) {
    memset(buffer, 0, 321);
    strncat(buffer, "Cc: ", 4);
    /* Inicia el loop */
    current = cc;
    while ( current != NULL ) {

      memset(box, 0, 81);
      /* Verifica que si hay caracteres no ascii */
      len = non_ascii_alloc(current->display_name, &tmp);
      if ( len != 0 ) {
        /* Algunos caracteres del display name no son ascii, codificalos */
        if ( verbose ) {
	  printf("Algunos de los caracteres en '%s' no son ASCII, se codificaran a UTF-8. El resultado será: %s\n", current->display_name, tmp);
	}
        snprintf(box, 81, "%s %s", tmp, current->angle_addr);
        free(tmp);
        tmp = NULL;

      } else {
        /* Todos los caracteres son ascii, pasalos como tal.
         * aunque aqui existe la posibilidad de que no halla display_name */
        if ( strlen(current->display_name) ) {
	  snprintf(box, 81, "%s %s", current->display_name, current->angle_addr);
        } else {
	  snprintf(box, 81, "%s", current->angle_addr);
        }

      }
    strncat(buffer, box, strlen(box));

      current = current->next;
      /* Verifica aqui si hay mas mailboxes para agregar, y en caso de que así
       * sea sera necesario agregar ',' como separador de los buzones */
      if ( current != NULL ) {
	/* Si hay más buzones, agrega el separador */
	strncat(buffer, ",", 1);
      }
      /* Agrega el terminador de la linea */
      strncat(buffer, "\r\n", 2);

    }

    /* Agrega el campo de encabezado Cc: */
    strncat(mensaje, buffer, strlen(buffer));
  }

  return 0;
}

/**
 *
 */
int
agrega_cuerpo(char *mensaje, const char *body_filename, const char *cfdi,
	      const char *pdf, const char *boundary, const int verbose)
{

  /* El separador entre la seccion de encabezados (headers section)
   * y el cuerpo de mensaje (body)
   */
  strncat(mensaje, "\r\n", 2);


  /* Ahora, este es un buen sitio para agregar los archivos adjuntos
   * el CFDi mismo y su representacion impresa
   */

  /* Ahora agrega el cuerpo del mensaje */
  if ( body_filename != NULL ) {
    agrega_archivo(body_filename, boundary, mensaje, verbose);
  }

  /* Agrega el CFDi como un archivo adjunto */
  if ( cfdi != NULL ) {
    agrega_archivo(cfdi, boundary, mensaje, verbose);
  }

  /* Agrega la representacion impresa del CFDi */
  if ( pdf != NULL ) {
    agrega_archivo(pdf, boundary, mensaje, verbose);
  }

  /* Y agrega finalmente el indicador de que no hay mas partes del mensaje */
  strncat(mensaje, "--", 2);
#ifdef ENVIA_UTILIZA_UUID
  strncat(mensaje, boundary, 38);
#else
  strncat(mensaje, boundary, 30);
#endif
  strncat(mensaje, "--\r\n\r\n\r\n", 8);

  return 0;
}

/**
 *
 */
int
curl_envia_email_enverdad(const char *username, const char *password,
			  const char *mailserver, imf_mensaje_t mensaje,
			  struct curl_slist *recipients, const int insecure,
			  const int verbose)
{
  CURL *curl = NULL;
  CURLcode res = CURLE_OK;
  char buffer[81];

  curl = curl_easy_init();
  if ( curl ) {
    /* Set username and password */ 
    curl_easy_setopt(curl, CURLOPT_USERNAME, username);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
 
    /* This is the URL for your mailserver. Note the use of smtps:// rather
     * than smtp:// to request a SSL based connection. */ 
    curl_easy_setopt(curl, CURLOPT_URL, mailserver);

    /* In this example, we'll start with a plain text connection, and upgrade
     * to Transport Layer Security (TLS) using the STARTTLS command. Be careful
     * of using CURLUSESSL_TRY here, because if TLS upgrade fails, the transfer
     * will continue anyway - see the security discussion in the libcurl
     * tutorial for more details. */ 
    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, (long)CURL_SSLVERSION_DEFAULT);

    /* The Microsoft(R) servers use an old chiper, make sure of set
     * this old cipher (RC4-MD5) only when using their servers, otherwise use
     * the the default cipher */
    if ( strcmp(mailserver, "smtp://smtp.live.com:587/") == 0 ||
         strcmp(mailserver, "smtp://smtp.live.com:587") == 0 ||
	 strcmp(mailserver, "smtp://smtp-mail.outlook.com:587/") == 0 ||
	 strcmp(mailserver, "smtp://smtp-mail.outlook.com:587") == 0 ) {
      curl_easy_setopt(curl, CURLOPT_SSL_CIPHER_LIST, "RC4-MD5");
    }
 
    /* If you want to connect to a site who isn't using a certificate that is
     * signed by one of the certs in the CA bundle you have, you can skip the
     * verification of the server's certificate. This makes the connection
     * A LOT LESS SECURE.
     *
     * If you have a CA cert for the server stored someplace else than in the
     * default bundle, then the CURLOPT_CAPATH option might come handy for
     * you. */ 
    if ( insecure ) {
      /* Cambia la opcion por default que es verificar la autenticidad del
       * servidor en los certificados */
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    }
 
    /* If the site you're connecting to uses a different host name that what
     * they have mentioned in their server certificate's commonName (or
     * subjectAltName) fields, libcurl will refuse to connect. You can skip
     * this check, but this will make the connection less secure. */ 
    if ( insecure ) {
      /* Cambia la opcion por default que es verificar la autenticidad del
       * servidor en los certificados */
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    }
 
    /* Note that this option isn't strictly required, omitting it will result in
     * libcurl sending the MAIL FROM command with empty sender data. All
     * autoresponses should have an empty reverse-path, and should be directed
     * to the address in the reverse-path which triggered them. Otherwise, they
     * could cause an endless loop. See RFC 5321 Section 4.5.5 for more details.
     */ 
    memset(buffer, 0, 81);
    snprintf(buffer, 81, "<%s>", username);
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, username);
 
    /* Add two recipients, in this particular case they correspond to the
     * To: and Cc: addressees in the header, but they could be any kind of
     * recipient. */ 
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
 
    /* We're using a callback function to specify the payload (the headers and
     * body of the message). You could just use the CURLOPT_READDATA option to
     * specify a FILE pointer to read from. */ 
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_mensaje);
    curl_easy_setopt(curl, CURLOPT_READDATA, &mensaje);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
 
    /* Since the traffic will be encrypted, it is very useful to turn on debug
     * information within libcurl to see what is happening during the
     * transfer */ 
    if ( verbose ) {
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }
 
    /* Send the message */ 
    res = curl_easy_perform(curl);

    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* Free the list of recipients */ 
    /*curl_slist_free_all(recipients);*/ /* Delete it in curl_envia_email... */
 
    /* Always cleanup */ 
    curl_easy_cleanup(curl);
  }
 
  return (int)res;
}

/**
 *
 */
int
curl_envia_email(const char *username, const char *password,
		const char *mailserver, const char *display_name_from,
		const char *from, const char *display_name_to, const char *to,
		const char *cc, const char *asunto, const char *body_filename,
		const char *cfdi_filename, const char *pdf_filename,
		const char *keywords, const char *organization,
		const char *importance, const char *sensitivity,
		const int insecure, const int verbose)
{
  struct curl_slist *recipients = NULL;
  imf_mensaje_t mensaje;
  char buffer[80];
  char *pre_boundary = NULL;
  char boundary[39];
  int status = 0;

  /* Inicializa la variable que lleva el conteo de los characteres leidos */
  mensaje.len = 0;
  mensaje.data = (char *)calloc(200*1024, sizeof(char));

  /* Vamos a necesitar crear desde aqui el string que sirve como delimitador
   * de las diferentes secciones del mensaje */
#ifdef ENVIA_UTILIZA_UUID
  pre_boundary = obten_identificador(UUID_MAKE_V4);
#else
  pre_boundary = obten_identificador(2);
#endif
  memset(boundary, 0, 39);
  snprintf(boundary, 39, "_%s_", pre_boundary);
  free(pre_boundary);

  /* Crea el mensaje */
  /* Agrega el encabezado basico */
  status = agrega_encabezado(mensaje.data, display_name_from, from, asunto,
			     keywords, organization, importance, sensitivity, 
			     boundary, verbose);
  if ( status != 0 ) {
    /* Ha ocurrido un error */
    if ( verbose ) {
      fprintf(stderr, "ERROR. No fue posible crear los campos básicos en la sección del encabezado del mensaje.\n");
    }
    free(mensaje.data);
    return 1;
  }

  /* Agrega el destinatario */
  status = agrega_destinatarios(mensaje.data, display_name_to, to, cc, verbose);
  if ( status != 0 ) {
    /* Ha ocurrido un error */
    if ( verbose ) {
      fprintf(stderr, "ERROR. No fue posible agregar los destinatarios al cuerpo del mensaje\n");
    }
    free(mensaje.data);
    return 2;
  }

  /* Agrega los archivos que corresponderan al body y a los archivos
   * adjuntos */
  status = agrega_cuerpo(mensaje.data, body_filename, cfdi_filename,
			 pdf_filename, boundary, verbose);
  if ( status != 0 ) {
    /* Ha ocurrido un error */
    if ( verbose ) {
      fprintf(stderr, "ERROR. No fue posible agregar los archivos que corresponden a las secciones del mensaje.\n");
    }
    free(mensaje.data);
    return 3;
  }

  mensaje.len = strlen(mensaje.data);
  mensaje.pos = 0;

  /* Arma la lista de todos los recipientes a los que hay que enviar
   * el mensaje */
  /* Add two recipients, in this particular case they correspond to the
   * To: and Cc: addressees in the header, but they could be any kind of
   * recipient. */ 
  /* To: */
  memset(buffer, 0, 80);
  snprintf(buffer, 80, "<%s>", to);
  recipients = curl_slist_append(recipients, buffer);
  /* Cc: */
  if ( cc != NULL ) {
    memset(buffer, 0, 80);
    snprintf(buffer, 80, "<%s>", cc);
    recipients = curl_slist_append(recipients, buffer);
  }

  /* Ahora, pasa todos los parametros necesarios para que cURL envie
   * el mensaje */
  status = curl_envia_email_enverdad(username, password, mailserver, mensaje,
				     recipients, insecure, verbose);

  if ( status != CURLE_OK ) {
    /* Un error ocurrio */
  }
  /* Free the list of recipients */ 
  curl_slist_free_all(recipients);

  /* */
  free(mensaje.data);

  return status;
}

/**
 *
 */
int
curl_envia_email_mult(const char *username, const char *password,
		      const char *mailserver, const char *display_name_from,
		      const char *from, name_addr_t *to, name_addr_t *cc,
		      const char *asunto, const char *body_filename,
		      const char *cfdi_filename, const char *pdf_filename,
		      const char *keywords, const char *organization,
		      const char *importance, const char *sensitivity,
		      const int insecure, const int verbose)
{
  struct curl_slist *recipients = NULL;
  imf_mensaje_t mensaje;
  int status = 0;
  char *pre_boundary = NULL;
  char boundary[39];
  name_addr_t *current = NULL;

  /* Inicializa la variable que lleva el conteo de los characteres leidos */
  mensaje.len = 0;
  mensaje.data = (char *)calloc(200*1024, sizeof(char));

  /* Vamos a necesitar crear desde aqui el string que sirve como delimitador
   * de las diferentes secciones del mensaje */
#ifdef ENVIA_UTILIZA_UUID
  pre_boundary = obten_identificador(UUID_MAKE_V4);
#else
  pre_boundary = obten_identificador(2);
#endif
  memset(boundary, 0, 39);
  snprintf(boundary, 39, "_%s_", pre_boundary);
  free(pre_boundary);

  /* Crea el mensaje */
  /* Agrega el encabezado basico */
  status = agrega_encabezado(mensaje.data, display_name_from, from, asunto,
			     keywords, organization, importance, sensitivity, 
			     boundary, verbose);
  if ( status != 0 ) {
    /* Ha ocurrido un error */
    if ( verbose ) {
      fprintf(stderr, "ERROR. No fue posible crear los campos básicos en la sección del encabezado del mensaje.\n");
    }
    free(mensaje.data);
    return 1;
  }

  /* Agrega el destinatario */
  status = agrega_destinatarios_mult(mensaje.data, to, cc, verbose);
  if ( status != 0 ) {
    /* Ha ocurrido un error */
    if ( verbose ) {
      fprintf(stderr, "ERROR. No fue posible agregar los destinatarios al cuerpo del mensaje\n");
    }
    free(mensaje.data);
    return 2;
  }

  /* Agrega los archivos que corresponderan al body y a los archivos
   * adjuntos */
  status = agrega_cuerpo(mensaje.data, body_filename, cfdi_filename,
			 pdf_filename, boundary, verbose);
  if ( status != 0 ) {
    /* Ha ocurrido un error */
    if ( verbose ) {
      fprintf(stderr, "ERROR. No fue posible agregar los archivos que corresponden a las secciones del mensaje.\n");
    }
    free(mensaje.data);
    return 3;
  }

  mensaje.len = strlen(mensaje.data);
  mensaje.pos = 0;

  /* Arma la lista de todos los recipientes a los que hay que enviar
   * el mensaje */
  /* Add two kind of recipients, in this particular case they correspond to the
   * To: and Cc: addressees in the header, but they could be any kind of
   * recipient. */ 
  /* To: */
  current = to;
  while ( current != NULL ) {
    recipients = curl_slist_append(recipients, current->angle_addr);
    current = current->next;
  }

  /* Cc: */
  current = cc;
  while ( current != NULL ) {
    recipients = curl_slist_append(recipients, current->angle_addr);
    current = current->next;
  }

  /* Ahora, pasa todos los parametros necesarios para que cURL envie
   * el mensaje */
  status = curl_envia_email_enverdad(username, password, mailserver, mensaje,
				     recipients, insecure, verbose);

  if ( status != CURLE_OK ) {
    /* Un error ocurrio */
  }

  /* Free the list of recipients */ 
  curl_slist_free_all(recipients);

  /* */
  free(mensaje.data);

  return status;
}
