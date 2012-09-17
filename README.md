inere
=====

Es una libreria con algunas funciones utiles para cumplir con algunos requisitos del Servicio de Administración Tributaria ([SAT][1]).


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

La función para obtener la clave del RFC en su forma básica es

	char*
	fisica_clave_abreviada( char* clave, const char* nombre,
				const char* apellido_paterno,
				const char* apellido_materno,
				const char* año_de_nacimiento,
				const char* número_del_mes_de_nacimiento,
				const char* día de nacimiento,
				const int debug_flag);

La función regresa la clave del RFC en su forma más básica, es decir, la función regresa únicamente un string de 11 elementos (incluyendo el caracter NUL) en la variable 'clave'.

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
Las dos anteriores funciones requieren un string con longitudes de 11 y 14 elementos, respectivamente, para funcionar adecuadamente. En caso de no pasar strings de estás longitudes el comportamiento de las funciones queda indefinido.

La siguiente función regresa, en la variable clave, la clave diferenciadora de homonimias para la clave de el RFC, para el nombre de la persona

	char* homonimia(char* clave,
			const char* nombre,
			const char* apellidos,
			int const debug);

cabe notar que el tercer argumento de la función requiere todos los apellidos de la persona, todos alojados en la misma variable.

La siguiente función regresa el díigito verificador para la clave de el RFC.

	char digito_verificador(const char* rfc-corto,
				const int debug);

Cabe mencionar que el dígito verificador corresponde al último caracter de la clave de el RFC, de modo que este dígito no está presente en la variable 'rfc-corto', es decir, el rfc-corto constará únicamente de 12 elementos (no nulos) para el caso de una persona fisica.

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

	-c/--homonimia
	-k/--digito=<RFC-corto>
	-r/--rfc
	-n/--nombre=<NOMBRE>
	-p/--paterno=<APELLIDO>
	-t/--materno=<APELLIDO>
	-i/--dia=<DIA>
	-m/--mes=<MES>
	-a/--ano=<AÑO>
	-d/--debug
	-v/--verbose
	-h/--help

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


Uso
---

Los programas deberan...


Fuentes de la información
-------------------------


Se tomaron como fuentes de información, para la elaboración de las rutinas que calcula la clave del RFC los siguientes documentos:

* Solicitud de la norma, regla o instrucción que establece la forma de construcción del Registro Federal de Constribuyentes para personas físicas y morales, incluye dígito verificador y homoclave. Solicitud número 0610100107106 hecha al [IFAI][2]
* Solicitudes 0610100162005 y 0610100107106 hechas en el sistema [INFOMEX][3]
* Documento[4]



[1]: http://www.sat.gob.mx
[2]: http://www.sisi.org.mx/jspsi/documentos/2006/seguimiento/06101/0610100107106_065.doc
[3]: https://www.infomex.org.mx/gobiernofederal/moduloPublico/moduloPublico.action
[4]: http://asuncionez.iespana.es/documentos/RFC.pdf
