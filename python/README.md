Extensión, para [Python][1], de inere
=====

Las funciones de extensión son:

* rfc -- Obtiene la clave del R.F.C.
* rfcinfo -- Además de calcular la clave del R.F.C. proporciona algunos otros componentes de ésta.
* verificarfc -- Verifica claves del R.F.C.
* cantidadconletras -- Regresa el numeral correspondiente a una cantidad.
* verificasello -- Verifica la integridad de la información mercantil contenida en un CFD/CFDi.

### Funciones

#### rfc

La función 'rfc'regresa, en un 'string' la clave del R.F.C. correspondiente. Por ejemplo:

	rfc(nombre="Enrique", paterno="Gámez", materno="Flores"
	    día=20, mes=7, año=1974)

regresara, como resultado

	GAFE740720326

#### rfcinfo

Esta función tiene una sintaxis idéntica a la de la función 'rfc',

	rfc(nombre="Enrique", paterno="Gámez", materno="Flores"
	    día=20, mes=7, año=1974)

excepto que ahora regresará, como un diccionario, la información calculada,

	{rfc: <la-clave-del-RFC>,
	 homonimia: <la-clave-diferenciadora-de-homonimia>,
	 digito: <el-digito-verificador-de-la-clave-del-RFC> }

que para el ejemplo mencionado, el diccionario será:

	{rfc: "GAFE770720326", homonimia: "32", digito: "6"}

#### verificarfc

Esta función regresara True o False, en caso de que la clave del R.F.C., proporcionada como argumento a esta función, concuerde o no, respectivamente, con el cálculo propio.

	verificarfc("GAFE770720327")

regresará 'False'.

#### cantidadconletras

Convierte el nómero a su numeral. Y en caso de que el número corresponda a una cantidad de moneda (que es el caso por defecto) redondeara la cantidad como si se tratase de pesos mexicanos.

	cantidadconletras(1234.456)

regresara el string,

	un mil doscientos treinta y cuatro pesos 46/100 M.N.

#### verificasello

Esta función regresara True si el sello digital del comprobante digital especificado por la ruta de archivo como primer argumento, corresponda con la información de dicho comprobante. El segundo argumento, para la función es la ruta de archivo de la 'stylesheet' con la que se obtendra la cadena original que corresponde a la información que el comprobante ampara. Un tercer agumento, opcional, es para imprimir (a la stdout) algunos pasos que ejecuta la función.

	verificasello(<ruta-del-CFD>, <ruta-de-la-stylesheet>, <verbisidad>)



Instalación
-----------

	python setup.py build
	python setup.py install

Uso
---

```python
import inere


```
