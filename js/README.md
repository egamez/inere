[jQuery][1] plugin para verificar la clave de el [R.F.C][2]
====

Este plugin fue creado para facilitar la captura de la clave del [R.F.C][2], es decir, ayudar a visualizar posibles errores en su captura. El método para lograrlo es a través de la comparación de el dígito verificador suministrado por la captura y uno que se calcula dentro de la función.

Este plugin está basados en el plugin [password_strength][3] de Sagie Maoz.

El procedimiento para verificar la clave se basa en la comparación de el dígito varificador (último carácter de la clave del [R.F.C][2]). Para esto, se raliza el calculo de el dígito verificador para después compararlo con el suministrado.

El procedimiento para el calculo de este dígito está descrito en el [Instructivo para formación del Registro Federal de Contribuyentes][4].

Uso
---

Para su uso únicamente es necesario llamar a la función para un campo de tipo texto:

	$( '#rfc' ).digito_verificador();

La función elimina cualquier espacio o guion antes de realizar el calculo de el dígito, de modo que la introducción, del [R.F.C][2] en el campo, puede incluir estos carácteres para mejorar su lectura.

Resultados
----------

La función, por defecto, colorea en verde el campo cuando el dígito verificador suminstrado coincide con el calculado, en caso contrario el campo el coloreado con un fundo guinda.


Opciones
--------

Las opciones disponibles por el momento son:
* legendOnFail -- Cadena de carácteres para ser desplegados contiguos al campo de introducción del R.F.C cuando la clave introducida no coincida con el dígito calculado. El valor, por defecto, es una cadena vacia,
* legendOnSuccess -- Cadena de carácteres cuando el dígito calculado coincida con el suministrado. El valor por defecto es una cadena vacia,
* withColors -- Variable de tipo booleana para instruir si el campo de entrada del RFC cambiara de color segun el resultado de la verificación.

Ejemplos
--------

Para desplegar las leyendas 'Captura correcta' y 'Posible error en la captura' sólo sera necesario declararlo en las opciones de la manera siguiente:

	$('#rfc').digito_verificador({
		legendOnSuccess: "Captura correcta.",
		legendOnFail: "Posible error en la captura."
	});

es posible agregar a la cadena de instruccuines de el selector algunos atributos CSS, por ejemplo:

	$('#rfc').digito_verificador({
		legendOnSuccess: "Captura correcta.",
		legendOnFail: "Posible error en la captura."
	}).css('text-transform', 'uppercase');

el ejemplo anterior agregara las leyendas junto al campo de introducción de la clave del [R.F.C][2] y cambiara de color el fondo del campo también, pero si se quiere desabilitar lo opción del cambio de color solo sera necesario agregar, a la opciones de arriba la opción 'withColors' con el valor 'false':

	$('#rfc').digito_verificador({
		legendOnSuccess: "Captura correcta.",
		legendOnFail: "Posible error en la captura.",
		withColors: false
	}).css('text-transform', 'uppercase');

Se provee también un ejemplo de uso para una página web simple (rfc.html).

Autores
-------
LAE
Enrique Gámez <egamezf@gmail.com>

Licencia
--------
Licensed under the [GPL][5] license

[1]: http://jquery.com/
[2]: http://www.sat.gob.mx/sitio_internet/21_12672.html
[3]: http://archive.plugins.jquery.com/project/password_strength
[4]: http://www.sisi.org.mx/jspsi/documentos/2006/seguimiento/06101/0610100107106_065.doc
[5]: http://www.gnu.org/licenses/gpl-3.0.html
