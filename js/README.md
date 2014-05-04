Función, para [jQuery][1], para verificar y obtener la clave del [R.F.C][2]
====

Esta función fue creado para facilitar la (posible) detección de errores en la captura de la clave del [R.F.C][2], es decir, ayudar a visualizar posibles errores al momento de la captura de ésta clave, utilizando los metóodos que dan consistencia a dicha clave.

Esta función fue especialmente diseñada para ser utilizada en conjunto con el plugin [autocomplete][3] de [jquery-ui][4], aunque puede también utilizarse en conjunto con algunos otros plugins.

La idea de aplicación de esta función es en el escenario de la captura de la clave del [R.F.C][2] en conjunto con el nombre del contribuyente, en una forma dentro de una página web.


Metodología.
------------

El proceso para la generación de la clave del [R.F.C.][2] se encuentra definido en las [Reglas de la Conformación de la Clave de R.F.C.][5], siendo el [SAT][6] la única autoridad para asignar dicha clave.

La clave del [R.F.C][2] consta básicamente de tres partes: información inherente a cada contribuyente (nombre, fecha), de una clave diferenciadora de homonimos (dos caracteres) y de un caracter de verificación.

Las dos últimas partes de la clave son generadas tomando en cuenta unicamente el nombre completo del contribuyente (para la clave diferenciadora de homonimia), y la consistencia de la clave (a traves del caracter de verificación.) Es de esta manera, generando las dos últimas partes de la clave que la función pretende ser útil, es decir, si al momento de la captura de los datos del contribuyente (nombre y clave) alguno de los datos fueron ingresados de manera erronéa (error de de dedo al momento de la captura), está función generará una clave diferente a la que es proporcionada, ayundado con esto a la posible corrección de errores en la introducción de estos datos.

Uso.
----

El uso principal de esta función es generar la clave del [R.F.C][2] sólo después de que la información inherente halla sido capturada y se esté introduciendo la clave del [R.F.C][2], ya que sólo después de haberse introducido los digitos correspondientes a la fecha, en la clave del [R.F.C][2] la función mostrará su versión de dicha clave.

Por ejemplo, imaginemos que el nombre correcto del contribuyente es "Gámez Flores, Enrique", y que su fecha de nacimiento es 20 de Julio de !974, la clave del [R.F.C][2] es, para este caso: GAFE740720362; pero si en lugar de haber introducido "Gamez" se escribio "Games", la función generará la siguiente clave: GAFE740720P51, con lo que al comparar la clave generada con la suministrada se observara que ambas difieren indicando con esto una posible falla al momento de la captura, falla que en este caso se debe a la incorrecta captura de la palabra "Gamez"

Advertencia.
------------

Las claves generadas por esta función son solamente de caracter informativo y no tienen ninguna válidez oficial, es responsabilidad de cada usuario el uso de está información (ver [Licencia][4].)

Ejemplos.
---------

Para autocompletar la clave del [R.F.C][2] de una persona física (veáse el archivo [fisica.html][8]):

```javascript
	$( "#clave" ).autocomplete({
		minLength: 10,
		source: function( request, response ) {
			var input = $( "#clave" );
			var day = input.clave_rfc( "extract2", "day", input.val() ),
			    month = input.clave_rfc( "extract2", "month", input.val() ),
			    year = input.clave_rfc( "extract2", "year", input.val() );
			var result = [
				input.clave_rfc( "fisica_clave",
						 $( "#nombre" ).val(),
						 $( "#paterno" ).val(),
						 $( "#materno" ).val(),
						 day,
						 month,
						 year ) ];
			response ( result );
		}
	});
```

Para autocompletar la clave del [R.F.C][2] de una persona moral (veáse el archivo [moral.html][9]):

```javascript
	$( "#clave" ).autocomplete({
		minLength: 9,
		source: function( request, response ) {
			var input = $( "#clave" );
			var day = input.clave_rfc( "extract2", "day", input.val() ),
			    month = input.clave_rfc( "extract2", "month", input.val() ),
			    year = input.clave_rfc( "extract2", "year", input.val() );
			response( input.clave_rfc( "moral_clave",
						   $( "#denominacion" ).val(),
						   $( "#tipo" ).val(),
						   day,
						   month,
						   year ) );
		}
	});
```

Para el cálculo de la clave del [R.F.C][2] (veáse el archivo [rfc.html][10])

```javascript
	$( "#genera-rfc" ).click( function( e ) {
		e.preventDefault();
		var clave = $( "#clave" ).clave_rfc( "fisica_clave",
						     $( "#nombre" ).val(),
						     $( "#paterno" ).val(),
						     $( "#materno" ).val(),
						     $( "#day" ).val(),
						     $( "#month" ).val(),
						     $( "#year" ).val() );
		$( "#clave" ).val( clave );
	});
```


Autores
-------
L3a,
Enrique Gámez <egamezf@gmail.com>

Licencia
--------
Licensed under the [GPL][7] license

[1]: http://jquery.com/
[2]: http://www.sat.gob.mx/sitio_internet/21_12672.html
[3]: https://jqueryui.com/autocomplete/
[4]: http://jqueryui.com/
[5]: http://www.sisi.org.mx/jspsi/documentos/2006/seguimiento/06101/0610100107106_065.doc
[6]: http://www.sat.gob.mx/
[7]: http://www.gnu.org/licenses/gpl-3.0.html
[8]: https://github.com/egamez/inere/blob/master/js/fisica.html
[9]: https://github.com/egamez/inere/blob/master/js/moral.html
[10]: https://github.com/egamez/inere/blob/master/js/rfc.html
