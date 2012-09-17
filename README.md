inere
=====

Es una libreria con algunas funciones utiles para cumplir con algunos requisitos del Servicio de Administración Tributaria ([SAT][1]).


Funciones
---------

Estas son algunas de las funciones en la libreria:

* fisica_clave -- Función para obtener la clave (básica) del Registro Federal de Contribuyentes.
* homonimia -- Función para obtener la clave diferenciadora de homonimias de la clave (básica) del RFC.
* digito_verificador -- Función para obtener el dígito verificador de la clave del RFC.
* numeral -- Función que convierte el número en su numeral.
* cantidad -- Función para obtener la representacion con letra de alguna cantidad en pesos mexicanos.


Uso
---

Ejemplos de usos de las funciones.

###

La función para obtener la clave del RFC en su forma básica es

	char*
	fisica_clave(char* clave, const char* nombre,
		const char* apellido_paterno,
		const char* apellido_materno,
		const char* año_de_nacimiento,
		const char* número_del_mes_de_nacimiento,
		const char* día de nacimiento,
		const int debug_flag);

La función regresa la clave del RFC en su forma más básica.


Instalación
-----------

	./configure
	make
	make install


Uso
___

Los programas deberan...


Fuentes de la información
_________________________


Se tomaron como fuentes de información, para la elaboración de las rutinas que calcula la clave del RFC los siguientes documentos:

* Solicitud de la norma, regla o instrucción que establece la forma de construcción del Registro Federal de Constribuyentes para personas físicas y morales, incluye dígito verificador y homoclave. Solicitud número 0610100107106 hecha al [IFAI][2]
* Solicitudes 0610100162005 y 0610100107106 hechas en el sistema [INFOMEX][3]
* Documento[4]



[1]: http://www.sat.gob.mx
[2]: http://www.sisi.org.mx/jspsi/documentos/2006/seguimiento/06101/0610100107106_065.doc
[3]: https://www.infomex.org.mx/gobiernofederal/moduloPublico/moduloPublico.action
[4]: http://asuncionez.iespana.es/documentos/RFC.pdf
