inere
=====

Es una libreria con algunas funciones utiles para el calculo, y verificación de la clave del R.F.C proporcionada a todos los contribuyentes en México, por el Servicio de Administración Tributaria ([SAT][1]).


Funciones
---------

Estas son algunas de las funciones en la libreria:

* fisica_clave_abreviada -- Función para obtener la clave (básica) del Registro Federal de Contribuyentes.
* clave_rfc_persona_fisica -- Función para obtener la clave de el Registro Federal de Contribuyentes para personas físicas.
* homonimia -- Función para obtener la clave diferenciadora de homonimias de la clave (básica) del RFC.
* digito_verificador -- Función para obtener el dígito verificador de la clave del RFC.
* numeral -- Función que convierte el número en su numeral.
* cantidad -- Función para obtener la representacion con letra de alguna cantidad en pesos mexicanos.


Uso
---

Ejemplos de usos de las funciones.

### Funciones

La función para obtener la clave del RFC es

	char*
	clave_rfc_persona_fisica(char* clave,
				 const char *nombre,
				 const char *paterno,
				 const char *materno,
				 const char *year,
				 const char* month,
				 const char *day,
				 const int debug);

La cual regresa un string de 14 elementos (incluyendo el caracter NUL) con la clave del RFC, en la variable 'clave'.

#### Atención.
Todas las funciones asumen la codificación UTF-8 para los nombres y los apellidos. De ahí que la función 'clave_rfc_persona_fisica' asuma que la longitud de la cadena 'clave' sea 18!, que sería la longitud máxima posible para la clave del R.F.C. en la codificación UTF-8 (contemplando que el único carácter no ASCII que puede aparecer en la clave del R.F.C. es el carácter 'Ñ').



La siguiente función regresa, en la variable clave, la clave diferenciadora de homonimias para la clave de el RFC, para el nombre de la persona

	char* homonimia(char* clave,
			const char* nombre,
			const char* apellidos,
			int const debug);

cabe notar que el tercer argumento de la función requiere todos los apellidos de la persona, todos alojados en la misma variable.

La siguiente función regresa el díigito verificador para la clave de el RFC.

	char digito_verificador(const char* rfc-corto,
				const int debug);

Cabe mencionar que el dígito verificador corresponde al último caracter de la clave de el RFC, de modo que este dígito no está presente en la variable 'rfc-corto', es decir, el rfc-corto constará únicamente de al menos 12 elementos (no nulos) para el caso de una persona fisica.

La función 'cantidad' regresa, en la variable 'nombre' la representación con letra de el número dado en la variable 'numero'.

	char* cantidad(char* nombre,
		       const char* numero,
		       const int debug);


La función 'numeral' se encarga de regresar el numeral correspondiente al número dado

	char*
	numeral(char* nombre,
	        const char* numero,
	        const int debug);


### Programas

La libreria cuenta con dos programas, uno para efectos del calculo de la clave de el RFC (por el momento únicamente para personas fisicas) y otro para la representación con letra de cantidades en pesos mexicanos.

#### rfc

El programa 'rfc' tiene las siguientes opciones

	-c/--homonimia         Obten la clave diferenciadora de homonimias. Se
                               requerira tanto el nombre como los apellidos.
	-g/--digito=RFC-corto  Obten el digito verificador para este RFC abreviado. 
	-r/--rfc               Obten la clave del Registro Federal de Contribuyentes.
	-n/--nombre=NOMBRE     Fija el nombre o razon social que se utilizara
                               para el calculo del RFC.
	-p/--paterno=APELLIDO  Fija el apellido paterno (o el apellido unico en caso de
                               que el contribuyente tenga un unico apellido) en caso de
                               persona fisica que se utilizaran para el calculo del RFC
	-t/--materno=APELLIDO  Fija el apellido materno, del contribuyente, necesario
                               para obtener el RFC.
	-d/--dia=DIA           Fija el dia de nacimiento del contribuyente.
	-m/--mes=MES           Fija el numero de mes de nacimiento.
	-a/--ano=AÑO           Fija el año de nacimiento.
	-b/--verifica=RFC      Verifica que la clave del RFC suministrada
                               coincida con el digito tambien suministrado.
	-k/--debug             Set the debug mode.
	-v/--verbose           Set the verbose mode.
	-h/--help              Imprime este mensaje.



#### cantidad

El programa 'cantidad' está diseñado para regresar la representación con letra de alguna cantidad en pesos mexicanos. Por el momento la cantidad más grande que se puede representar es 999,999.99 pesos mexicanos. La sintáxis es la siguiente

	$ cantidad <cantidad con numeros a representar>

por ejemplo:

	$ cantidad 17845.45
	Diecisiete mil ochocientos cuarenta y cinco pesos 45/100 M.N.

cabe mencionar que la siguiente expresión para la cantidad es también válida, y regresa el valor esperado,

	$ cantidad 17,845.45
	Diecisiete mil ochocientos cuarenta y cinco pesos 45/100 M.N.


Instalación
-----------

	./autogen.sh
	./configure
	make


Ejemplos
--------

La manera de obtener la clave de el RFC es

```c
#include <string.h>
#include <stdio.h>

#include <inere/personafisica.h>

int
main(int argc, char* argv[])
{
  char rfc[18];
  char* nombre = NULL;
  char* primer_apellido = NULL;
  char* segundo_apellido = NULL;
  char* year = NULL;
  char* month = NULL;
  char* day = NULL;

  if ( argc < 6 ) {
    fprintf(stderr, "Sintax:\n%s <dia> <mes> <año> <nombre> <primer-apellido> [<segundo-apellido>]\n", argv[0]);
    return 1;
  }
  day = argv[1];
  month = argv[2];
  year = argv[3];
  nombre = argv[4];
  primer_apellido = argv[5];
  if ( argc == 7 ) segundo_apellido = argv[6];

  memset(rfc, 0, 18);

  clave_rfc_persona_fisica(rfc, nombre, primer_apellido, segundo_apellido, year, month, day, 0);

  printf("La clave de el RFC es: %s\n", rfc);
  return 0;
}
```

y compilandolo con

	gcc -I<installation-path>/include -o obten_rfc obten_rfc.c -L<installation-path>/lib -linere

y ejecutandolo con la instrucción

	./obten_rfc 18 12 1921 "Jose Antonio" Camargo Hernandez

se obtiene la clave de el RFC (CAHA211218UL1).

Hay varios ejemplos en el subdirectorio ejemplos.

Notése que la longitud de la cadena 'rfc' tiene una longitud de 18 carácteres! Esto es debido a que las letras que corresponden al nombre del contribuyente pueden incluir el carácter 'Ñ'.

TODO
----

Actualmente las funciones no obtienen la clave diferenciadora de homonimias correcta para apellidos que contengan la letra 'Ñ'. Por ejemplo, para el hipotético caso de un contribuyente de nombre "Pedrito Nuño Cabañas" y con fecha de nacimiento "20 de Julio de 1973", el sitio ([Recaudanet][5]) proporciona la siguiente clave "NUCM730720H87", y esta aplicación da la siguiente clave "NUCP730720IC7"; pero la mayor dificultad es que ambas aplicaciones (la del sitio ([Recaudanet][5]) y ésta misma) no coinciden con claves de sujetos reales, es decir, no coinciden con claves proporcionadas por el ([SAT][1]) para casos reales.

Licencia
--------

Ver COPYING.

Fuentes de la información
-------------------------


Se tomaron como fuentes de información, para la elaboración de las rutinas que calcula la clave del RFC los siguientes documentos:

* Solicitud de la norma, regla o instrucción que establece la forma de construcción del Registro Federal de Constribuyentes para personas físicas y morales, incluye dígito verificador y homoclave. Solicitud número 0610100107106 hecha al [IFAI][2].
* Solicitudes 0610100162005 y 0610100107106 hechas en el sistema [INFOMEX][3].
* Documento[4].
* Aplicación para el calculo del R.F.C. en el sition [Recaudanet][5].


[1]: http://www.sat.gob.mx
[2]: http://www.sisi.org.mx/jspsi/documentos/2006/seguimiento/06101/0610100107106_065.doc
[3]: https://www.infomex.org.mx/gobiernofederal/moduloPublico/moduloPublico.action
[4]: http://asuncionez.iespana.es/documentos/RFC.pdf
[5]: https://www.recaudanet.gob.mx/recaudanet/rfc.jsp
