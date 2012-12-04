[jQuery][1] plugin para verificar la clave de el [R.F.C][2]
====

Este plugin fue creado para facilitar la captura de la clave del [R.F.C][2], es decir, ayudar a visualizar posibles errores en su captura. El método para lograrlo es a través de la comparación de el dígito verificador suministrado por la captura y uno que se calcula dentro de la función.

Este plugin está basados en el plugin [password_strength][3] de Sagie Maoz.

El procedimiento para verificar la clave se basa en la comparación de el dígito varificador (último carácter de la clave del [R.F.C][2]). Para esto, se raliza el calculo de el dígito verificador para después compararlo con el suministrado.

El procedimiento para el calculo de este dígito está descrito en el [Instructivo para formación del Registro Federal de Contribuyentes][4].

Uso
---

Para su uso únicamente es necesario llamar a la función para un campo de tipo texto:

	$( '#rfc' ).rfc_digito_verificador();

La función elimina cualquier espacio o guion antes de realizar el calculo de el dígito, de modo que la introducción, del [R.F.C][2] en el campo, puede incluir estos carácteres para mejorar su lectura.

=== Persona física / Persona moral

La función opera con ambos tipos de claves sin ningun problema (hasta el momento.)

Resultados
----------

La función, por defecto, colorea en verde el campo cuando el dígito verificador suminstrado coincide con el calculado, en caso contrario el campo el coloreado con un fundo guinda.


Opciones
--------

Ejemplos
--------

Veáse el archivo rfc.html

Autores
-------
Lae,
Enrique Gámez <egamezf@gmail.com>

Licencia
--------
Licensed under the [GPL][5] license

[1]: http://jquery.com/
[2]: http://www.sat.gob.mx/sitio_internet/21_12672.html
[3]: http://archive.plugins.jquery.com/project/password_strength
[4]: http://www.sisi.org.mx/jspsi/documentos/2006/seguimiento/06101/0610100107106_065.doc
[5]: http://www.gnu.org/licenses/gpl-3.0.html
