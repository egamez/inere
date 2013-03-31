Plugin para [jQuery][1] para verificar y obtener la clave de el [R.F.C][2]
====

Este plugin fue creado para facilitar la captura de la clave del [R.F.C][2], es decir, ayudar a visualizar posibles errores en su captura.

El plugin opera principalmente en dos escenarios. Completa la clave del [R.F.C][2] al momento mismo de la captura (únicamente para el caso de la clave para una persona fisica), o verifica que no halla posibles errores al termino de la captura.

Complementa la clave.
---------------------

En este modo, el plugin se encargara de completar la clave del [R.F.C][2] calculando tanto la clave diferenciadora de homonimia, como el digito verificador que se obtendría.

La manera de completar la clave es a través de la recopilación previa del nombre, junto con los apellidos, del contribuyente y la introducción de la clave hasta la parte que corresponde a la fecha de nacimiento de la persona. Una vez compilada está información, el plugin completara toda la clave.

El otro modo del plugin, es la de hacer unas cuantas pruebas con la clave del [R.F.C][2], capturada completamente, y utizando el nombre (previamente capturado) de la persona. Las pruebas a realizar son:

* Verifica que la clave diferenciadora de homonimia, que se obtiene directamente del nombre, coincida con la capturada (sólo claves para personas fisicas, por el momento),
* Verifica que el digito verificador, que acompaña a cada clave, coincida con el que se calcula por el plugin (clave de personas fisicas y personas morales),
* Verifica que la clave capturada coincida exactamente con la que el plugin calcula (sólo claves para personas fisicas),
* Verifica que la clave capturada tenga la forma canónica de los carácteres permitidos para estás clave (claves para personas fisicas y morales.)

Los procedimientos para la obtención de los parametros para la verificación de las claves son los descritos en el [Instructivo para formación del Registro Federal de Contribuyentes][3].

Ejemplos.
--------

Para el modo de completar (veáse el archivo [selfcomplete.html][5])

```javascript
	$( "#rfc" ).rfc_tester({
		nombre: $( "#nombre" ),
		primerapellido: $( "#paterno" ),
		segundoapellido: $( "#materno" ),
		selfcomplete: true,
		success: function( clave ) {
			$( this ).val( clave );
		},
		error: function( message, sugerencia ) {
			alert( message );
			console.log( "Sugerencia: " + sugerencia );
		}
	});
```

Para el modo de verificación (veáse el archivo [verifica.html][6])

```javascript
                var container = $( "<span/>" ).attr( "class", "rfc-message" );

                $( "#rfc" ).rfc_tester({
                        nombre: $( "#nombre" ),
                        primerapellido: $( "#paterno" ),
                        segundoapellido: $( "#materno" ),
                        success: function( clave ) {
                                container.text( "Ok" ).attr( "class", "rfc-message" );
                        },
                        error: function( message, sugerencia ) {
                                container.text( "Error" ).attr( "class", "rfc-message" );
                                alert( message );
                                console.log( "Sugerencia: " + sugerencia );
                        },
                        change: function( clave, homonimia, verificador ) {
                                container.text( "" ).attr( "class", "rfc-message" );
                        }
                }).after( container );
```

Para el cálculo de la clave del [R.F.C][2] (veáse el archivo [rfc.html][7])

```javascript
                $( "form#rfc-form" ).submit( function() {
                        var clave = $( "#rfc" ).rfc_tester( "clave",
                                                        $( "#nombre" ).val().toUpperCase(),
                                                        $( "#paterno" ).val().toUpperCase(),
                                                        $( "#materno" ).val().toUpperCase(),
                                                        $( "#dia" ).val(),
                                                        $( "#mes" ).val(),
                                                        $( "#year" ).val() );
                        $( "#rfc" ).val( clave );
                        return false;
                });
```


Autores
-------
Lae,
Enrique Gámez <egamezf@gmail.com>

Licencia
--------
Licensed under the [GPL][4] license

[1]: http://jquery.com/
[2]: http://www.sat.gob.mx/sitio_internet/21_12672.html
[3]: http://www.sisi.org.mx/jspsi/documentos/2006/seguimiento/06101/0610100107106_065.doc
[4]: http://www.gnu.org/licenses/gpl-3.0.html
[5]: https://github.com/egamez/inere/blob/master/js/selfcomplete.html
[6]: https://github.com/egamez/inere/blob/master/js/verifica.html
[7]: https://github.com/egamez/inere/blob/master/js/rfc.html
