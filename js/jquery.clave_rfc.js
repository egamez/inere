/*
 * Copyright (c) 2014, L3a,
 *		       Enrique Gámez Flores <egamez@edisson.com.mx>
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

(function($) {

/**
 * @namespace Encapsula todas las funciones para la verificación de la clave del R.F.C.
 */
var inere = {

	/**
	 * Calcula la clave del R.F.C. para una persona fisica.
	 * @param {string} nombre Nombre de pila del contribuyente.
	 * @param {string} primerapellido Primer apellido del contribuyente.
	 * @param {string} segundoapellido Segundo apellido del contribuyente (puede ser null en caso de que el contribuyente no tenga un segundo apellido.
	 * @param {string} dia Día de nacimiento del contribuyente.
	 * @param {string} mes Mes de nacimiento del contribuyente.
	 * @param {string} ano Año de nacimiento del contribuyente.
	 * @returns {string} La clave del R.F.C. para una persona fisica.
	 *
	 * @example
	 * var clave = $( "#input" ).clave_rfc( "fisica_clave", "Enrique", "Gámez", "Flores", "20", "7", "1974");
	 */
	fisica_clave: function( nombre, primerapellido, segundoapellido, dia, mes, ano ) {
		var clave = undefined;
		var name = [];
		var lastname1 = [];
		var lastname2 = [];
		var diferenciadora = undefined;
		var digito = undefined;

		// Regla 8. Elimina del nombre articulos, preposiciones, conjunciones, etc.
		name = inere.fisica_regla8( inere.normalize( nombre.toUpperCase() ) );

		// Regla 6. Elimina del nombre las palabras MARÍA o JOSÉ.
		name = inere.fisica_regla6( name );

		// Regla 8. Elimina del primer (segundo) apellido articulos, preposiciones, conjunciones, etc.
		lastname1 = inere.fisica_regla8( inere.normalize( primerapellido.toUpperCase() ) );
		if ( segundoapellido ) lastname2 = inere.fisica_regla8( inere.normalize( segundoapellido.toUpperCase() ) );

		// Regla 4.
		if ( lastname1[0].length <= 2 ) {
			if ( lastname2.length ) {
				clave = inere.fisica_regla4( name[0], lastname1[0], lastname2[0] );
			} else {
				clave = inere.fisica_regla7( name[0], lastname1[0] );
			}
		}

		// Regla 7.
		if ( lastname2.length === 0 ) {
			clave = inere.fisica_regla7( name[0], lastname1[0] );
		}

		// Regla comun.
		if ( clave === undefined ) {
			clave = inere.fisica_regla1( name[0], lastname1[0], lastname2[0] );
		}

		// Regla 9.
		clave = inere.fisica_regla9( clave );

		// Aplica la regla 2.
		clave = clave + inere.regla2( dia, mes, ano );

		// Obten la clave direfenciadora de homonimias
		diferenciadora = inere.homonimia( (primerapellido + ' ' + segundoapellido + ' ' + nombre).toUpperCase() );

		// Obten el digito verificador.
		digito = inere.digitoVerificador( clave + diferenciadora );

		// Construye la clave
		clave = clave + diferenciadora + digito;

		return clave;
	},


	/**
	 * Calcula un par de claves del R.F.C. para una persona moral.
	 * El motivo de calcular un par de claves es debido a que en algunas
	 * ocasiones...
	 * @param {string} razonsocial Razón social o denominación social.
	 * @param {string} tiposociedad Tipo de sociedad.
	 * @param {string} dia Día de la fecha de constitución de la empresa.
	 * @param {string} mes Mes de la fecha de constitución de la empresa.
	 * @param {string} ano Año de la fecha de constitución de la empresa.
	 * @returns {array} La clave del R.F.C. para una persona fisica.
	 *
	 * @example
	 * var clave = $( "#input" ).clave_rfc( "moral_clave", "Inere sistemas", "S.A. de C.V.", "20", "7", "1974");
	 */
	moral_clave: function( razonsocial, tiposociedad, dia, mes, ano ) {
		var clave = [];
		var diferenciadora = [];
		var digito = [];
		var palabras = [];
		var tmp = razonsocial.toUpperCase();

		// Elimina letras compuestas y acentos
		tmp = inere.normalize( tmp ); // Regla 3

		// Crea un arreglo con todas las palabras que forman la razón o denominación social
		palabras = tmp.split(" "); // TODO. more broad definition of spaces.

		// Elimina las palabras Compañía o su abreviatura Cía., etc. Regla 11
		palabras = inere.moral_regla11( palabras );

		// Elimina preposiciones. Regla 9
		palabras = inere.moral_regla9( palabras );

		// Convierte numeros en numerales. Regla 10
		palabras = inere.moral_regla10( palabras );

		// Convierte caracteres especiales.
		palabras = inere.moral_regla12( palabras );

		// De acuerdo con la cantidad de palabras, que conforman la
		// razón o denominación social, aplica las reglas 1, 6, 7 u 8.
		if ( palabras.length > 2 ) {
			tmp = inere.moral_regla1( palabras[0], palabras[1], palabras[2] );
		} else if ( palabras.length == 2 ) {
			tmp = inere.moral_regla6( palabras[0], palabras[1] );
		} else if ( palabras.length == 1 ) {
			tmp = inere.moral_regla7( palabras[0] );
		}

		// Aplica la regla 2.
		tmp = tmp + inere.regla2( dia, mes, ano );

		// Obten la clave diferenciadora de homonimias para los dos casos:
		//	- para el caso en donde se incluya el tipo de sociedad de la empresa, y
		//	- para el caso en que no se utilize el tipo de sociedad.
                if ( tiposociedad.length ) {
			diferenciadora.push( inere.homonimia( (razonsocial + ' ' + tiposociedad).toUpperCase() ) );
		} else {
			diferenciadora.push( inere.homonimia( razonsocial.toUpperCase() ) );
		}
		diferenciadora.push( inere.homonimia( razonsocial.toUpperCase() ) );

		// Obten el digito verificador para ambas claves.
		digito.push( inere.digitoVerificador( tmp + diferenciadora[0] ) );
		digito.push( inere.digitoVerificador( tmp + diferenciadora[1] ) );

		// Construye las claves.
		clave.push( tmp + diferenciadora[0] + digito[0] );
		clave.push( tmp + diferenciadora[1] + digito[1] );

		return clave;
	},


	/**
	 * Extrae de la clave del R.F.C., utilizando una expresión regular,
	 * la parte de que corresponde a la información requerida en 'selection'
	 * @param {string} selection Nombre clave de la información a extraer.
	 * @param {string} input Clave del R.F.C. de donde se extraera la información solicitado mediante el nombre clave.
	 * @returns {string} la parte de la clave del R.F.C. que corresponde al nombre clave.
	 * @example
	 * $( "#rfc" ).clave_rfc( "extract", "digito_verificador", "GAFA7701266V2" );
	 */
	extract: function( selection, input ) {
		var regex = undefined;
		if 	( selection === "year"			) regex = /\d{2}(?=(\d{4}))/;
		else if ( selection === "month"			) regex = /\d{2}(?=(\w{5}$))/;
		else if ( selection === "day"			) regex = /\d{2}(?=(\w{3}$))/;
		else if ( selection === "homonimia"		) regex = /\w{2}(?=(\w(?=$)))/;
		else if ( selection === "digito_verificador"	) regex = /\w(?=$)/;

		if ( regex ) return regex.exec( input )[0];
		return undefined;
	},


	/**
	 * Extrae de la clave del R.F.C. por completar, utilizando una expresión regular,
	 * la parte de que corresponde a la información requerida en 'selection'
	 * @param {string} selection Nombre clave de la información a extraer.
	 * @param {string} input Clave del R.F.C. de donde se extraera la información solicitado mediante el nombre clave.
	 * @returns {string} la parte de la clave del R.F.C. que corresponde al nombre clave.
	 * @example
	 * $( "#rfc" ).clave_rfc( "extract2", "month", "GAFA770126" );
	 */
	extract2: function( selection, input ) {
		var result = input.match( /\d{2}/g );
		if 	( selection === "year"	) return result[0];
		else if ( selection === "month"	) return result[1];
		else if ( selection === "day"	) return result[2];
		return undefined;
	},


	/**
	 * Obtiene la clave diferenciadora de homoninia.
	 * @param {string} nombre Nombre del contribuyente, segun el formato
	 *			  Primer-apellido Segundo-apellido Nombre
	 * @param {boolean} debug
	 * @returns {string} La clave diferenciadora de homonimia correspondiente al nombre dado.
	 * @example
	 * $( "#rfc" ).clave_rfc( "homonimia", "Gámez Flores Enrique", false );
	 * "36"
	 */
	homonimia: function( nombre ) {

		var anexo1 = {' ':0 , '0':0 , '1':1 , '2':2 , '3':3 , '4':4 , '5':5 , '6':6 , '7':7 , '8':8 , '9':9 ,
			      '&':10, 'A':11, 'B':12, 'C':13, 'D':14, 'E':15, 'F':16, 'G':17, 'H':18, 'I':19,
			      'J':21, 'K':22, 'L':23, 'M':24, 'N':25, 'O':26, 'P':27, 'Q':28, 'R':29,
			      'S':32, 'T':33, 'U':34, 'V':35, 'W':36, 'X':37, 'Y':38, 'Z':39, \u00D1:40,
			      \u00C1:11, \u00C9:15, \u00CD:19, \u00D3:26, \u00DA:34};

		var anexo2 = { 0:'1',  1:'2',  2:'3',  3:'4',  4:'5',  5:'6',  6:'7',  7:'8',  8:'9',  9:'A',
			      10:'B', 11:'C', 12:'D', 13:'E', 14:'F', 15:'G', 16:'H', 17:'I', 18:'J', 19:'K',
			      20:'L', 21:'M', 22:'N', 23:'P', 24:'Q', 25:'R', 26:'S', 27:'T', 28:'U', 29:'V',
			      30:'W', 31:'X', 32:'Y', 33:'Z'};

		var digito_anterior = 0;
		var suma = 0;
		var current = null;
		var valor = 0;
		var residuo = 0;
		var coeficiente = 0;
		var clave = null;
		var accepted = /\w|\s|&|\u00D1|\u00C1|\u00C9|\u00CD|\u00D3|\u00DA/;

		for (var i = 0; i < nombre.length; i++) {
			current = nombre.charAt(i);
			if ( accepted.test( current ) ) {
				$.each( anexo1, function( character, value ) {
					valor = value;
					return (current != character);
				});
				// Primera suma.
				suma += (digito_anterior * 10 + inere.decena( valor ) ) * inere.decena( valor );

				// Save the previous digit
				digito_anterior = valor % 10;
				// Segunda suma
				suma += valor * digito_anterior;

			}
		}
		// Now compute the code
		suma = suma % 1000; // Take only the last 3 digits.
		residuo = suma % 34;
		coeficiente = (suma - residuo) / 34;

		$.each(anexo2, function( valor, caracter ) {
			current = caracter;
			return (coeficiente != valor);
		});
		clave = current;
		$.each(anexo2, function( valor, caracter ) {
			current = caracter;
			return (residuo != valor);
		});
		clave = clave + current;

		return clave;
	},


	/**
	 * @private Private function
	 */
	decena: function( value ) {
		if	( value >= 30 ) return 3;
		else if ( value >= 20 ) return 2;
		else if ( value >= 10 ) return 1;
		else			return 0;
	},


	/**
	 * Cálcula el digito verificador para la clave del R.F.C. (incompleta) dada.
	 * @param {string} clave Clave del R.F.C. incompleta, es decir, sin que la clave contenga el digito verificador.
	 * @param {boolean} debug
	 * @returns {string} El digito verificador para la clave (incompleta) especificada.
	 * @example
	 * $( "#rfc" ).clave_rfc( "digitoVerificador", "GAFE74072036", false );
	 * "2"
	 */
	digitoVerificador: function( clave ) {
		var anexo3 =    {'0': 0, '1': 1, '2': 2, '3': 3,  '4': 4, '5': 5, '6': 6, '7': 7, '8': 8, '9': 9,
				 'A':10, 'B':11, 'C':12, 'D':13,  'E':14, 'F':15, 'G':16, 'H':17, 'I':18, 'J':19,
				 'K':20, 'L':21, 'M':22, 'N':23,  '&':24, 'O':25, 'P':26, 'Q':27, 'R':28, 'S':29,
				 'T':30, 'U':31, 'V':32, 'W':33,  'X':34, 'Y':35, 'Z':36, ' ':37, \u00D1:38};

		var current = '';
		var suma = 0;
		var acumulado = 0;
		var remainder = 0;
		var index = 13;
		var digit = '';
		var rfc_small = null;

		if ( clave.length === 11 ) {
			rfc_small = ' ' + clave;
		} else {
			rfc_small =  clave;
		}

		for (var i = 0; i < rfc_small.length; i++ ) {
			current = rfc_small.charAt(i);

			$.each(anexo3, function(key, val) {
				suma = val;

				return (current != key);
			});
			acumulado += suma * index;
			index -= 1;
		}

		remainder = acumulado % 11;
		// Now assign the 'digito verificador'
		if	( remainder == 0 ) digit = '0';
		else if ( remainder == 1 ) digit = 'A';
		else			   digit = 11-remainder;

		return digit;
	},


	/**
	 * Función auxiliara para eliminar, de la clave del R.F.C., espacios y guiones que se utilizan para facilitar la legibilidad de la clave.
	 * @param {string} clave
	 * @returns {string} Regresa la clave del R.F.C. sin ningun guion o espacio.
	 * @example
	 * $( "#rfc" ).clave_rfc( "normalizeClave", "GAFE 740720 362");
	 * "GAFE740720362"
	 */
	normalizeClave: function( clave ) {
		// Remove any space or dash character.
		return clave.replace(/[ -]/g, '');
	},


	// Persona fisica - Regla 1.
	// Se integra la clave con los siguientes datos:
	//
	// 1. La primera letra del apellido paterno y la siguiente primera vocal del mismo.
	// 2. La primera letra del apellido maternos.
	// 3. La primera letra del nombre.
	//
	// Ejemplo:
	//
	//	Juan Barrios Fernandez
	//		Apellido paterno:	Barrios		BA
	//		Apellido materno:	Fernandez	F
	//		Nombre:			Juan		J
	//		Resultado de la expresion alfabetica:	BAFJ
	//	Eva Iriarte Mendez
	//		Apellido paterno:	Iriarte		II
	//		Apellido materno:	Mendez		M
	//		Nombre:			Eva		E
	//		Resultado de la expresion alfabetica:	IIME
	fisica_regla1: function( nombre, primerapellido, segundoapellido ) {
		var segunda_vocal = primerapellido.substring(1).match(/A|E|I|O|U|\u00C1|\u00C9|\u00CD|\u00D3|\u00DA/i)[0].charAt(0);
		return primerapellido.charAt(0) + segunda_vocal + segundoapellido.charAt(0) + nombre.charAt(0);
	},


	// Persona fisica y Persona moral - Regla 2.
	// A continuacion se anotara la fecha de nacimiento del contribuyente, en el
 	// siguiente orden:
	//
	//	Ano: Se tomaran las dos ultimas cifras, escribiendolas con numeros
	//	     arabigos.
	//	Mes: Se tomara el mes de nacimiento en su numero de orden, en un ano
	//	     calendario, escribiendolo con numeros arabigos.
	//	Dia: Se escribira con numeros arabigos.
	//
	// Ejemlos:
	//
	//	Ano	1970						70
	//	Mes	Diciembre					12
	//	Dia	13						13
	//		Dando como resultado la expresion numerica:	701213
	//
	// Y complementando con la expresion alfabetica numerica tendremos:
	//
	//	Juan Barrios Fernandez					BAFJ 701213
	//	Eva Iriarte Mendez					IIME 691117
	//
	// Cuando en el ano, mes, o dia, de la fecha de nacimiento, aparezcan solamente
	// un guarismo se le antepondra un cero.
	// Ejemplos:
	//
	//	Ano	1907						07
	//	Mes	Abril						04
	//	Dia	1o						01
	//		Como resultado tendresmo la expresion numerica:	070401
	//
	// Y completando la clave alfabetico numerica tendremos:
	//
	//	Juan Barrios Fernandez					BAFJ-070401
	//	Francisco Ortiz Perez					OIPF-290205
	//	Manuel Martinez Hernandez				MAHM-570102
	//	Gabriel Courturier Moreno				COMG-600703
	//
	regla2: function( dia, mes, ano ) {
		if ( dia.length === 1 ) dia = '0' + dia;
		if ( mes.length === 1 ) mes = '0' + mes;
		if ( ano.length === 1 ) ano = '0' + ano;
		else if ( ano.length === 4 ) ano = ano.substr(2);
		return ano + mes + dia;
	},


	// Persona fisica - Regla 3.
	// Cuando la letra inicial de cualquiera de los apellidos o nombre sea compuesta
	// unicamente se anotara la inicial de esta. En la Ch la C y en la Ll la L.
	//
	// Ejemplos:
	// 
	//	Manuel Chavez Gonzalez					CAGM-240618
	//	Felipe Camargo Llamas					CALF-450228
	//	Charles Kennedy Truman					KETC-511012
	//
	regla3: function( nombre ) {
		var clave = undefined;
		if ( nombre.match( /^CH|^LL/i ) ) {
			// Elimina el caracter compuesto.
			clave =  nombre.charAt(0) + nombre.substring(2);
		} else {
			clave = nombre;
		}
		return clave;
	},

	// Persona fisica - Regla 4.
	// En los casos en que el apellido paterno de la persona fisica se componga de
	// una o de dos letras, la clave se formara de la siguiente manera:
	//
 	//	1. La primera letra del apellido paterno.
	//	2. La primera letra del apellido materno.
	//	3. La primera y segunda letra del nombre.
	// Ejemplo:
	//
 	//	Alvaro de la O Lozano					OLAL-401201
	//	Ernesto Ek Rivera					ERER-071120
	//
	fisica_regla4: function( nombre, primerapellido, segundoapellido ) {
		return primerapellido.charAt(0) + segundoapellido.charAt(0) + nombre.charAt(0) + nombre.charAt(1);
	},


	// Persona fisica - Regla 5.
	// Cuando el apellido paterno o materno sean compuestos, se tomara para la
 	// clasificacion de la primera palabra que corresponda a cualquiera de ellos.
	//
	// Ejemplos:
	//
	// Dolores San Martin Davalos					SADD-180812
	// Mario Sanchez de la Barquera Gomez				SAGM-190224
	// Antonio Jimenez Ponce de Leon				JIPA-170808
	//
	fisica_regla5: function( nombre, primerapellido, segundoapellido ) {
	},

	// Persona fisica - Regla 6.
	//  * Cuando el nombre es compuesto, o sea, que esta formado por dos o mas palabras
	// se tomara, para la conformacion la letra inicial de la primera, siempre que
	// no sea MARIA o JOSE dado su frecuente uso, en cuyo caso se tomara la primera
	// letra de la segunda palabra.
	//
	// Ejemplos:
	//
	//	Luz Maria Fernandez Juarez				FEJL-200205
	//	Jose Antonio Camargo Hernandez				CAHA-211218
	//	Maria Luisa Ramirez Sanchez				RASL-25112
	//
	fisica_regla6: function( nombre ) {
		var result = [];
		for ( var i = 0; i < nombre.length; i++ ) {
			if ( nombre[i] === 'MARIA' || nombre[i] === 'MARÍA' ||
			     nombre[i] === 'JOSE'  || nombre[i] === 'JOSÉ' ) {
				continue;
			} else {
				result.push(nombre[i]);
			}
		}
		return result;
	},


	// Persona fisica - Regla 7.
	// En los casos en que la persona fisica tenga un solo apellido, se conformara
	// con la primera y segunda letras del apellido paterno o materno, segun figure
	// en el acta de nacimiento, mas la primera y segunda letras del nombre.
	//
 	// Ejemplos:
	//
	//	Juan Martinez						MAJU-420116
	//	Gerardo Zafra						ZAGE-251125
	//
	fisica_regla7: function( nombre, primerapellido ) {
		//return primerapellido.charAt(0) + primerapellido.charAt(1) + nombre.charAt(0) + nombre.charAt(1);
		var segunda_vocal = primerapellido.substring(1).match(/A|E|I|O|U|\u00C1|\u00C9|\u00CD|\u00D3|\u00DA/i)[0].charAt(0);
		return primerapellido.charAt(0) + segunda_vocal + nombre.charAt(0) + nombre.charAt(1);
	},

	// Persona fisica - Regla 8.
	// Cuando en el nombre de las personas fisicas figuren articulos, preposiciones
	// conjunciones, o contracciones no se tomaran como elementos de integracion de
	// la clave, ejemplos:
	//
	//	Carmen de la Pena Ramirez				PERC-631201
	//	Mario Sanchez de los Cobos				SACM-700110
	//	Roberto Gonzalez y Duran				GODR-600101
	//	Juan del Valle Martinez					VAMJ-691001
	//
	// Regla 3.
	// La regla 3 se aplicara desde aquí, eliminando de las palabras las letras
	// compuestas.
	//
	fisica_regla8: function( nombre ) {
		var result = [];
		nombre = nombre.split( ' ' ); // Convert the string onto an array.
		for ( var i = 0; i < nombre.length; i++ ) {
			if ( nombre[i].length ) {
				if ( nombre[i] === "DE" ||
					nombre[i] === "DEL" ||
					nombre[i] === "LA" ||
					nombre[i] === "LAS" ||
					nombre[i] === "LOS" ||
					nombre[i] === "MC" ||
					nombre[i] === "MAC" ||
					nombre[i] === "MI" ||
					nombre[i] === "VAN" ||
					nombre[i] === "VON" ||
					nombre[i] === "Y" ) {
					continue;
				} else {
					result.push( nombre[i] );
				}
			}
		}
		return result;
	},


	// Persona fisica - Regla 9.
	// Cuando de las cuatro letras que formen la expresion alfabetica, resulte una
	// palabra inconveniente, la ultima letra sera sustituida por una "X".
	//
	//	BUEI	BUEX				KOGE	KOGX
	//	BUEY	BUEX				KOJO	KOJX
	//	CACA	CACX				KAKA	KAKX
	//	CACO	CACX				KULO	KULX
	//	CAGA	CAGX				MAME	MAMX
	//	CAGO	CAGX				MAMO	MAMX
	//	CAKA	CAKX				MEAR	MEAX
	//	COGE	COGX				MEON	MEOX
	//	COJA	COJX				MION	MIOX
	//	COJE	COJX				MOCO	MOCX
	//	COJI	COJX				MULA	MULX
	//	COJO	COJX				PEDA	PEDX
	//	CULO	CULX				PEDO	PEDX
	//	FETO	FETX				PENE	PENX
	//	GUEY	GUEX				PUTA	PUTX
	//	JOTO	JOTX				PUTO	PUTX
	//	KACA	KACX				QULO	QULX
	//	KACO	KACX				RATA	RATX
	//	KAGA	KAGX				RUIN	RUIX
	//	KAGO	KAGX
	//
	fisica_regla9: function( code ) {
		var clave = undefined;
		if (	code === "BUEI" ||
			code === "BUEY" ||
			code === "CACA" ||
			code === "CACO" ||
			code === "CAGA" ||
			code === "CAGO" ||
			code === "CAKA" ||
			code === "COGE" ||
			code === "COJA" ||
			code === "COJE" ||
			code === "COJI" ||
			code === "COJO" ||
			code === "CULO" ||
			code === "FETO" ||
			code === "GUEY" ||
			code === "JOTO" ||
			code === "KACA" ||
			code === "KACO" ||
			code === "KAGA" ||
			code === "KAGO" ||
			code === "KOGE" ||
			code === "KOJO" ||
			code === "KAKA" ||
			code === "KULO" ||
			code === "MAME" ||
			code === "MAMO" ||
			code === "MEAR" ||
			code === "MEON" ||
			code === "MION" ||
			code === "MOCO" ||
			code === "MULA" ||
			code === "PEDA" ||
			code === "PEDO" ||
			code === "PENE" ||
			code === "PUTA" ||
			code === "PUTO" ||
			code === "QULO" ||
			code === "RATA" ||
			code === "RUIN" ) {
			// Replace the last character with an X.
			clave = code.charAt(0) + code.charAt(1) + code.charAt(2) + 'X';
		} else {
			clave = code;
		}
		return clave;
	},

	// Persona fisica - Regla 10.
	// Cuando aparezcan formando parte del nombre, apellido paterno y materno los
	// caracteres especiales, estos deben de excluirse para el calculo del homonimo
	// y del digito verificador. Los caracteres se interpretaran, si y solo si,
	// estan en forma individual dentro del nombre, apellido paterno y apellido
	// materno. Ejemplos:
	//
	//	Roberto O'farril Carballo				OACR-661121
	//	Ruben D'angelo Fargo					DAFR-710108
	//	Luz Ma. Fernandez Juarez				FEJL-830120
	//


	// Persona moral - Regla 1
	//
	// Para la formación de la clave se tomarán como elemento de orden, las
	// tres primeras letras de las tres primeras palabras de la denominación
	// o razón social tal y como esté manifestando en la copia del acta
	// constitutiva o en caso en la copia del documento que motivó su
	// origen, ejemplo:   
	//
	//	Sonora Industrial Azucarera, S. de R. L.
	//		Primera Palabra: Sonora			S 
	//		Segunda palabra: Industrial		I
	//		Tercera palabra: Azucarera		A
	//		Resultado de la expresión alfabética:	SIA
	moral_regla1: function( p1, p2, p3 ) {
		return (p1.charAt(0) + p2.charAt(0) + p3.charAt(0));
	},


	// Persona moral - Regla 3
	// Cuando la letra inicial de cualquiera de las tres primeras palabras
	// de la denominación o razón social sea compuesta, únicamente se
	// anotará la inicial de esta. En la Ch la C y en la Ll la L, ejemplos:
	//
	//	Champion Mexicana de Bujías, S.A.		CMB-830702
	//	Casa Chávez de maquinaria, S. de R. L.		CCM-800620
	//	Artículos de Piel y Chamarras, S. de R.L.	APC-810202
	//	Llantas, Cámaras y Refacciones, S. de R.L.	LCR-851015
	//	Candados, Llaves y Cerraduras, S.A.		CLC-830820
	//	Luis Molina Llorantes y Cía., S. de R.L.	LML-860911
	//


	// Persona moral - Regla 4
	// En los casos en que la denominación o razón social esté compuesta
	// sólo de iniciales, para estos efectos se consideran palabras, por
	// tanto deben tomarse en cuenta como tales, ejemplos:
	//
	//	F.A.Z., S.A.					GIG-841215
	//	U.S. Ruber Mexicana, S.A.			USR-860201
	//	H. Prieto y Martínez, S. de R.L.		HPM-841221


	// Persona moral - Regla 5
	// Las abreviaturas: "S. en N.C.", "S. en C.", "S. de R.L.",
	// "S. en C. por A.", "S.A.", "S.A. de C.V.", "S.N.C.", "S.C.", "A.C.",
	// "A. en P.", "S.C.L.", "S.C.S.", que expresan tipo de sociedad en las
	// denominaciones o razones sociales, no se tomarán en consideración
	// para efectos de conformación de las claves, ejemplos:
	//
	//	Guantes Industriales Guadalupe, S. en C.	GIG-841215
	//	Construcciones Metálicas Mexicanas, S.A.	CMM-830120
	//	Fundición de Precisión Eutectic, S. de R.L.	FPE-861125


	// Persona moral - Regla 6
	// Si la denominación o razón social se comprende de dos elementos,
	// para efectos de la conformación de la clave, se tomará la letra
	// inicial de la primera palabra y las dos primeras letras de la
	// segunda, ejemplos:
	//
	//	Fonograbaciones Cinelandia, S. de R.L.		FCI-841019	
	//	Aceros Ecatepec, S.A.				AEC-890130
	//	Distribuidora Ges, S.A.				DGE-850628
	moral_regla6: function( p1, p2 ) {
		var clave = p1.charAt(0);
		if ( p2.length > 1 ) clave = clave + p2.charAt(0) + p2.charAt(1);
		else		clave = clave + p2.charAt(0) + 'X';
		return clave;
	},


	// Persona moral - Regla 7
	// Si la denominación o razón social se compone de un solo elemento,
	// para efectos de conformación de la clave, se tomarán las tres
	// primeras letras consecutivas del mismo, ejemplo:
	//
	//	Arsuyama, S.A.					ARS-821129
	//	Calidra, S.A.					CAL-850920
	//	Electrólisis, S.A.				ELE-840821
	moral_regla7: function( p ) {
		var clave = undefined;
		if ( p.length > 2 )
			clave = p.charAt(0) + p.charAt(1) + p.charAt(2);
		else if ( p.length == 2 ) 
			clave = p.charAt(0) + p.charAt(1) + "X";
		else if ( p.length == 1 )
			clave = p.charAt(0) + "XX";
		return clave;
	},


	// Persona moral - Regla 8
	// Cuando la denominación o razón social se componga de un solo
	// elemento y sus letras no completen las tres requeridas, para efectos
	// de conformación de la clave, se tomaran las empleadas por el
	// contribuyente y las restantes se suplirán con una “X”, ejemplos:
	//
	//	Al, S.A.					ALX-830101
	//	Z, S.A.						ZXX-860110


	// Persona moral - Regla 9
	// Cuando en la denominación o razón social figuren artículos,
	// preposiciones y conjunciones o contracciones no se tomaran como
	// elementos de integración de la clave, ejemplos:
	//
	//	El abastecedor Ferretero, S.A.			AFE-840510
	//	Cigarros la Tabacalera Mexicana, S.A. de C.V.	CTM-860901
	//	Los Viajes Internacionales de Marco Polo, S.A.	VIM-824225
	//	Artículos y Accesorios para Automóviles, S.A.	AAA-800521
	//	Productos de la Industria del Papel, S.A.	PIP-811231
	moral_regla9: function( palabras ) {
		var tmp = palabras;
		palabras = [];
		for (var i in tmp) {
			if ( tmp[i] == "EL" ||
			     tmp[i] == "LA" ||
			     tmp[i] == "DE" ||
			     tmp[i] == "LOS" ||
			     tmp[i] == "LAS" ||
			     tmp[i] == "PARA" ||
			     tmp[i] == "CON" ||
			     tmp[i] == "EN" ||
			     tmp[i] == "Y" ||
			     tmp[i] == "E" ||
			     tmp[i] == "DEL" ) continue;
			palabras.push( tmp[i] );
		}
		return palabras;
	},


	// Persona moral - Regla 10
	// Cuando la denominación o razón social contenga en algún o en sus
	// tres primeros elementos números arábigos, o números romanos, para
	// efectos de conformación de la clave éstos se tomaran como escritos
	// con letra y seguirán las reglas ya establecidas, ejemplos:
	//
	//	El 12, S.A.
	//		(DOCE)					DOC-801029
	//	El 2 de Enero, S de R.L.
	//		(DOS)					DEN-840101
	//	El 505, S.A.
	//		(QUINIENTOS CINCO)			QCI-851215
	//	Editorial Siglo XXI, S.A.
	//		(VEINTIUNO)				ESV-831114
	moral_regla10: function( palabras ) {
		return palabras;
	},


	// Persona moral - Regla 11
	// Cuando aparezcan formando parte de la denominación o razón social,
	// la palabra Compañía o su abreviatura Cía., así como la palabra
	// Sociedad o su abreviatura Soc., no se incluirán en la conformación
	// de la clave, ejemplos:
	//
	//	Compañía Periodística Nacional, S.A.		PNA-861121
	//	Cía. De Artículos Eléctricos, S. de R.L.	AEL-850110
	//	Cía. Nal. De Subsistencias Mexicanas, S.A.	NSM-841011
	//	Pimienta Hnos. y Cía., S.A.			PHN-830228
	//	Sociedad Cooperativa de Producción Agrícola de Michoacán CPA-861016
	//	Sociedad de Consumo Agrícola del Sur, S.C.L.	CAS-821110
	//	Sociedad de Producción Rural de Sonora		PRS-800101
	moral_regla11: function( palabras ) {
		var tmp = palabras;
		palabras = [];
		for (var i in tmp) {
			if ( tmp[i] == "CIA" || tmp[i] == "COMPANIA" ) continue;
			palabras.push( tmp[i] );
		}
		return palabras;
	},


	// Persona moral - Regla 12
	// Cuando aparezcan formando parte de la denominación o razón social
	// los caracteres especiales, éstos deben de excluirse para el cálculo
	// del homónimo y del dígito verificador. Los caracteres se
	// interpretarán, sí y sólo si, están en forma individual dentro del
	// texto de la denominación o razón social. (Anexo VI) Ejemplos:
	//
	//	DENOMINACION O RAZON SOCIAL			RFC GENERADO
	//	LA S@NDIA S.A DE C.V.				SND-861121
	//	LA @ S.A. DE C.V.				ARR-860120
	//	LA @ DEL % SA DE CV				APO-830120
	//	@ COMER.COM					ACO-800210
	//	LAS ( BLANCAS )					APB-700202
	//	EL # DEL TEJADO					NET-010202
	//	LA / DEL SUR					DSU-010102
	//	EL C@FE.NET					CFE-030210
	moral_regla12: function( palabras ) {
		return palabras;
	},


	// Normalize.
	// Convierte todos las vocales acentudas a caracteres ASCII
	// Elimina apostrofos y signos de puntuación.
	normalize: function( palabra ) {
		var result = '';
		var vocal = null;
		for (var i = 0; i < palabra.length; i++) {
			if	( palabra.charAt(i) === '\u00C1' ) vocal = 'A';
			else if ( palabra.charAt(i) === '\u00C9' ) vocal = 'E';
			else if ( palabra.charAt(i) === '\u00CD' ) vocal = 'I';
			else if ( palabra.charAt(i) === '\u00D3' ) vocal = 'O';
			else if ( palabra.charAt(i) === '\u00DA' ) vocal = 'U';
			else if ( palabra.charAt(i) === '.'	 ) continue;
			else if ( palabra.charAt(i) === '\''	 ) continue;
			else if ( palabra.charAt(i) === ','      ) continue;
			else					   vocal = palabra.charAt(i);

			result = result + vocal;
		}
		return inere.regla3( result );
	}

};

$.fn.clave_rfc = function( method ) {

	if ( inere[method] ) {
		return inere[method].apply( this, Array.prototype.slice.call( arguments, 1) );
	} else if ( typeof method === 'object' || ! method ) {
		return inere.init.apply( this, arguments );
	} else {
		$.error( 'El método ' + method + ' no existe en la presente implementación.' );
	}
};

})(jQuery);
