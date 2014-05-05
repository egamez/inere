#include "inere/envia/curl_envia_email.h"

#include <stdio.h>
#include <stdlib.h>

#include <locale.h>
#include <getopt.h>

void print_help();

static const unsigned int kmax_mailboxes = 10;

void
print_help()
{
  printf("\nUso: multiple [OPTIONS] [  TO  CFDi [  CFDi-PDF  BODY  ASUNTO  ] ]\n");
  printf("Envia un e-mail para la entrega de un CFDi y su posible representación impresa.\n\n");

  printf("Los parametros obligatorios son:\n\n");
  printf("  -u, --usuario=USERNAME\tDeclara el nombre de usuario que tiene acceso\n\t\t\t\tal servidor de correo electrónico\n");
  printf("  -p, --password=PASSWORD\tDeclara el password (del usuario) para acceder\n\t\t\t\tal servidor de correo electrónico\n");
  printf("  -s, --servidor=URL\t\tDeclara el URL del servidor de correo\n\t\t\t\telectrónico. Por ejemplo:\n\t\t\t\t\tsmtps://smtp.gmail.com/\n\t\t\t\t\tsmtps://smtp.mail.yahoo.com/\n\t\t\t\t\tsmtp://smtp-mail.outlook.com:587/\n");

  printf("\n\nSe han definido el URL de algunos servidores de correo electrónico\nbastente comunes y son los siguientes:\n\n");

  printf("  -g, --gmail\t\t\tUtiliza el servidor SMTP de GMail(R)\n\t\t\t\t\t(smtps://smtp.gmail.com/)\n");
  printf("  -y, --yahoo\t\t\tUtiliza el servidor SMTP de Yahoo(R)\n\t\t\t\t\t(smtps://smtp.mail.yahoo.com/)\n");
  printf("  -l, --live\t\t\tUtiliza el servidor SMTP de Microsoft(R)\n\t\t\t\t(live.com)\n\t\t\t\t\t(smtp://smtp.live.com:587/)\n");
  printf("  -w, --outlook\t\t\tUtiliza el servidor SMTP de Microsoft(R)\n\t\t\t\t(Outlook - HotMail)\n\t\t\t\t\t(smtp://smtp-mail.outlook.com:587/)\n");

  printf("\n\n");
  printf("Algunas parametros opcionales son:\n\n");
  printf("  -r, --remitente=BUZON\t\tFija el buzón de correo electrónico que sera\n\t\t\t\tutilizado para el campo de encabezado 'From'\n");
  printf("  -d, --destinatario=BUZON\tDeclara el buzón de correo electrónico del\n\t\t\t\tdestinatario, campo de encabezado 'To'\n");
  printf("  -c, --copia-fiel=BUZON\tCarbon copy\n");
  printf("  -F, --cfdi=ARCHIVO\t\tLa ruta del CFDi\n");

  printf("  -a, --asunto=ASUNTO\t\tEl asunto del mensaje ('Subject')\n");
  printf("  -b, --body=FILE\t\tLa ruta del archivo que contiene el cuerpo del\n\t\t\t\tmensaje\n");
  printf("  -P, --cfdi-pdf=FILE\t\tLa ruta de la representación impresa del CFDI\n");
  printf("  -R, --nombre-remitente=NOMBRE\tFija el nombre a desplegar como remitente\n");
  printf("  -D, --nombre-destinatario=NOMBRE\n\t\t\t\tFija el nombre a desplegar del destinatario\n");
  printf("  -C, --nombre-copia=NOMBRE\tNombre asociado al buzón Cc:\n");
  printf("  -e, --palabras-clave=PALABRAS\tPalabras clave para agregar en el campo de\n\t\t\t\tencabezado del mensaje. Las palabras clave\n\t\t\t\tdeberan ser separadas por comas.\n");
  printf("  -o, --organizacion=NOMBRE\tDeclara el nombre de la organización\n");
  printf("  -A, --importancia-alta\tDefine el campo de encabezado auxiliar\n\t\t\t\tImportance como High\n");
  printf("  -N, --importancia-media\tDefine el campo de encabezado auxiliar\n\t\t\t\tImportance como Normal\n");
  printf("  -B, --importancia-baja\tDefine el campo de encabezado auxiliar\n\t\t\t\tImportance como Low\n");
  printf("  -X, --caracter-personal\tDefine el campo de encabezado auxiliar\n\t\t\t\tSensitivity como Personal\n");
  printf("  -Y, --caracter-privado\tDefine el campo de encabezado auxiliar\n\t\t\t\tSensitivity como Private\n");
  printf("  -Z, --caracter-confidencial\tDefine el campo de encabezado auxiliar\n\t\t\t\tSensitivity como Company confidential\n");
  printf("  -k, --insecure\t\tNo verifiques el proceso de verificación de\n\t\t\t\tcerticados\n");
  printf("  -v, --verbose\t\t\tEnvia mensajes a stdout acerca de las acciones\n\t\t\t\trealizadas por el programa\n");
  printf("  -h, --help\t\t\tImprime este mensaje\n");

  printf("\nBugs to <egamez@edisson.com.mx>\n");
}

int
main(int argc, char *argv[])
{
  int res = 0;
  const char *username = NULL;
  const char *password = NULL;
  const char *mailserver = NULL;
  const char *from = NULL;
  const char *from_displayname = NULL;
  name_addr_t *to = NULL;
  const char *to_mailbox = NULL;
  name_addr_t *cc = NULL;
  const char *cc_mailbox = NULL;
  name_addr_t *current = NULL;
  const char *asunto = NULL;
  const char *body_filename = NULL;
  const char *cfdi_filename = NULL;
  const char *pdf_filename = NULL;
  const char *keywords = NULL;
  const char *organization = NULL;
  const char *importance = NULL;
  const char *sensitivity = NULL;
  int insecure = 1;
  int verbose = 0;
  int help = 0;
  int ch = 0;

  static struct option long_options[] = {
    {"usuario",				required_argument,	NULL,	'u'},
    {"password",			required_argument,	NULL,	'p'},
    {"gmail",				no_argument,		NULL,	'g'},
    {"yahoo",				no_argument,		NULL,	'y'},
    {"live",				no_argument,		NULL,	'l'},
    {"outlook",				no_argument,		NULL,	'w'},
    {"servidor",			required_argument,	NULL,	's'},
    {"remitente",			required_argument,	NULL,	'r'},
    {"nombre-remitente",		required_argument,	NULL,	'R'},
    {"destinatario",			required_argument,	NULL,	'd'},
    {"nombre-destinatario",		required_argument,	NULL,	'D'},
    {"copia-fiel",			required_argument,	NULL,	'c'},
    {"nombre-copia",			required_argument,	NULL,	'C'},
    {"asunto",				required_argument,	NULL,	'a'},
    {"body",				required_argument,	NULL,	'b'},
    {"cfdi",				required_argument,	NULL,	'F'},
    {"cfdi-pdf",			required_argument,	NULL,	'P'},
    {"palabras-clave",			required_argument,	NULL,	'e'},
    {"organizacion",			required_argument,	NULL,	'o'},
    {"importancia-alta",		no_argument,		NULL,	'A'},
    {"importancia-media",		no_argument,		NULL,	'M'},
    {"importancia-baja",		no_argument,		NULL,	'B'},
    {"caracter-personal",		no_argument,		NULL,	'X'},
    {"caracter-privado",		no_argument,		NULL,	'Y'},
    {"caracter-confidencial",		no_argument,		NULL,	'Z'},
    {"insecure",			no_argument,		NULL,	'k'},
    {"help",				no_argument,		NULL,	'h'},
    {"verbose",				no_argument,		NULL,	'v'},
    {NULL,				0,			NULL,	 0 }
  };

  setlocale(LC_ALL, "");

  while ( (ch = getopt_long(argc, argv, "u:p:gylws:r:R:d:D:c:C:a:b:F:P:e:o:AMBXYZkhv", long_options, NULL)) != -1 ) {

    switch(ch) {
      case 'u':
	/* The username */
	username = optarg;
	break;

      case 'p':
	/* The password to access the mailserver */
	password = optarg;
	break;

      case 'g':
	/* The mailserver will the SMTP server of GMail */
	mailserver = "smtps://smtp.gmail.com/";
	break;

      case 'y':
	/* The mailserver will the SMTP server of Yahoo */
	mailserver = "smtps://smtp.mail.yahoo.com/";
	break;

      case 'l':
	/* The mailserver will the SMTP server of Microsoft (live.com) */
	mailserver = "smtp://smtp.live.com:587/";
	break;

      case 'w':
	/* The mailserver will the SMTP server of Microsoft (outlook.com) */
	mailserver = "smtp://smtp-mail.outlook.com:587/";
	break;

      case 's':
	/* The mailserver url */
	mailserver = optarg;
	break;

      case 'r':
	/* The From: mailbox */
	from = optarg;
	break;

      case 'R':
	/* The display name of the sender */
	from_displayname = optarg;
	break;

      case 'd':
	/* A receiver mailbox */
	to = name_addr_list_append(to, "", optarg);
	/* Guarda el valor de to_mailbox para asociarlo a la lista */
	to_mailbox = optarg;
	break;

      case 'D':
	/* The display name of the receiver */
        name_addr_list_update_name(to, to_mailbox, optarg);
	/* Resetea el valor, para uso posterior. */
	to_mailbox = NULL;
	break;

      case 'c':
	/* Carbon copy to */
	cc = name_addr_list_append(cc, "", optarg);
	/* Guarda el valor de to_mailbox para asociarlo a la lista */
	cc_mailbox = optarg;
	break;

      case 'C':
	/* Carbon copy display name */
	name_addr_list_update_name(cc, cc_mailbox, optarg);
	/* Resetea el valor, para uso posterior. */
	cc_mailbox = NULL;
	break;

      case 'a':
	/* The message subject */
	asunto = optarg;
	break;

      case 'b':
	/* The message's bidy file name */
	body_filename = optarg;
	break;

      case 'F':
	/* The CFDi filename */
	cfdi_filename = optarg;
	break;

      case 'P':
	/* The CFDi printed version filename */
	pdf_filename = optarg;
	break;

      case 'e':
	/* Keywords to be added into the header section */
	keywords = optarg;
	break;

      case 'o':
	/* The Organization name */
	organization = optarg;
	break;

      case 'A':
	/* The message importance = High */
	importance = "High";
	break;

      case 'M':
	/* The message importance = Normal */
	importance = "Normal";
	break;

      case 'B':
	/* The message importance = Low */
	importance = "Low";
	break;

      case 'X':
	/* The message sensitivity = Personal */
	sensitivity = "Personal";
	break;

      case 'Y':
	/* The message sensitivity = Private */
	sensitivity = "Private";
	break;

      case 'Z':
	/* The message sensitivity = Company confidential */
	sensitivity = "Company confidential";
	break;

      case 'k':
	/* Insecure connection */
	insecure = 0;
	break;

      case 'h':
	/* The help flag */
	help = 1;
	break;

      case 'v':
	/* The verbose flag */
	verbose = 1;
	break;

      case '?':
	/* getopt_long already printed an error message. */
	break;

      default:
	break;

    }

  }

  /* Check if help was selected */
  if ( help ) {
    print_help();
    return 0;
  }

  /* Ahora, preparate a leer algunos argumentos sin opciones
   * Estos argumentos seran:
   *
   *	Requeridos, si es que hay argumentos extra sin opciones:
   *		To
   *	Y opcionales, si es que hay argumentos extra sin opciones:
   *		CFDi
   *		CFDi - Representación impresa
   *		BODY
   *		Subject
   */
  if ( optind < argc ) {
    /* Si hay argumentos extra sin opcion, verifica cuantos de estos argumentos
     * extra tenemos */
    if ( argv[optind] != NULL ) {
      /* Asigna este primer argumento extra a To */
      to = name_addr_list_append(to, "", argv[optind++]);
    }

    /* Hay algun otro argumento? */
    if ( cfdi_filename == NULL && optind < argc && argv[optind] != NULL ) {
      /* Hay otro argumento, debe ser el nombre del archivo del CFDi */
      cfdi_filename = argv[optind++];
    }

    /* Hay algun otro argumento? */
    if ( pdf_filename == NULL && optind < argc && argv[optind] != NULL ) {
      /* Hay otro argumento, debe ser el nombre del archivo de la representacion
       * impresa del CFDi */
      pdf_filename = argv[optind++];
    }

    /* Hay algun otro argumento? */
    if ( body_filename == NULL && optind < argc && argv[optind] != NULL ) {
      /* Hay otro argumento, debe ser el nombre del archivo que contiene el
       * BODY del mensaje  */
      body_filename = argv[optind++];
    }

    /* Hay algun otro argumento? */
    if ( asunto == NULL && optind < argc && argv[optind] != NULL ) {
      /* Hay otro argumento, debe ser el Subject */
      asunto = argv[optind++];
    }

  }

  /* Check some required arguments */
  if ( username == NULL ) {
    /* Es necesario proveer el nombre de usuario para acceder al servidor
     * de correo electronico */
    fprintf(stderr, "ERROR. Es necesario especificar el nombre de usuario que tiene acceso al servidor de correo electrónico. Opción -u/--username=USERNAME.\n\nUtiliza --help para mayor información.\n");
    print_help();
    return 1;
  }

  if ( password == NULL ) {
    /* Es necesario proveer el paswword del usuario para acceder al servidor
     * de correo electronico */
    fprintf(stderr, "ERROR. Es necesario especificar la clave de acceso del usuario del servidor de correo electrónico. Opción -p/--password=PASSWORD.\n\nUtiliza --help para mayor información.\n");
    return 2;
  }

  if ( mailserver == NULL ) {
    /* Es inutil realizar algo si no hay servidor de salida */
    fprintf(stderr, "ERROR. Es necesario especificar el URL del servidor de correo electrónico. Opción -m/--mailserver=URL.\n\nUtiliza --help para mayor información.\n");
    return 3;
  }

  if ( to == NULL ) {
    /* Es inutil hacer algo si no existe persona a quien enviar */
    fprintf(stderr, "ERROR. Es necesario especificar el buzón de correo electrónico del destinatario. Opción -t/--to=MAILBOX.\n\nUtiliza --help para mayor información.\n");
    return 4;
  }

  if ( body_filename == cfdi_filename ) {
    /* Es decir, no fueron suministrados las rutas de alguno de estos
     * archivos, entonces no hay nada que realizar */
    fprintf(stderr, "ERROR. Es necesario especificar al menos la ruta del archivo que contiene el cuerpo del mensaje, o la ruta del CFDi a enviar. Opción -c/--cfdi=FILE.\n\nUtiliza --help para mayor información.\n");
    return 5;
  }

  /* Ahora, existe la posibilidad de que no se halla incluido explicitamente
   * el buzón de quien aparecera como remitente, en el campo de encabezado
   * From. Si este es el caso, utiliza el username para este valor */
  if ( from == NULL ) {
    /* Es inutil hacer algo si no hay persona quien envie */
    if ( verbose ) {
      fprintf(stderr, "ATENCION. No fue suministrado, explicitamente, el buzón de correo electrónico a utilizarse para el campo de encabezado 'From', de modo que se utilizara como buzón el nombre del usuario que tiene acceso al servidor de correo electrónico indicado.\n");
    }
    from = username;
  }

  /* Muestra algunas los argumentos suministrados */
  if ( verbose ) {
    if ( username != NULL ) {
      printf("    Nombre de usuario del servidor de SMTP: %s\n", username);
    }
/*
    if ( password != NULL ) {
      printf("Password: %s\n", password);
    }
*/

    if ( mailserver != NULL ) {
      printf("                  URL del servidor de SMTP: %s\n", mailserver);
    }

    if ( from != NULL ) {
      printf("                       Buzón del remitente: %s\n", from);
    }

    if ( from_displayname != NULL ) {
      printf("                      Nombre del remitente: %s\n", from_displayname);
    }

    current = to;
    while ( current != NULL ) {
      printf("                    Buzón del destinatario: %s %s\n", current->display_name, current->angle_addr);
      current = current->next;
    }

    current = cc;
    while ( current != NULL ) {
      printf("                          Carbon copy para: %s %s\n", current->display_name, current->angle_addr);
      current = current->next;
    }

    if ( asunto != NULL ) {
      printf("                                    Asunto: %s\n", asunto);
    }

    if ( body_filename != NULL ) {
      printf("           Ruta del archivo para el cuerpo: %s\n", body_filename);
    }

    if ( cfdi_filename != NULL ) {
      printf("                             Ruta del CFDi: %s\n", cfdi_filename);
    }

    if ( pdf_filename != NULL ) {
      printf("Ruta de la representacion impresa del CFDi: %s\n", pdf_filename);
    }

    if ( keywords != NULL ) {
      printf("                                  Keywords: %s\n", keywords);
    }

    if ( organization != NULL ) {
      printf("                 Nombre de la organizacion: %s\n", organization);
    }

    if ( importance != NULL ) {
      printf("                   Importancia del mensaje: %s\n", importance);
    }

    if ( sensitivity != NULL ) {
      printf("    Sensibilidad del contenido del mensaje: %s\n", sensitivity);
    }

    if ( insecure == 0 ) {
      printf("     Realiza la vericación de certificados: No\n");
    } else {
      printf("     Realiza la vericación de certificados: Si\n");
    }

  }

  /* Ahora envia lo que halla que enviar */
  res = curl_envia_email_mult(username, password, mailserver, from_displayname,
			      from, to, cc, asunto, body_filename,
			      cfdi_filename, pdf_filename, keywords,
			      organization, importance, sensitivity, insecure,
			      verbose);

  name_addr_list_clear(cc);
  name_addr_list_clear(to);

  return res;
}
