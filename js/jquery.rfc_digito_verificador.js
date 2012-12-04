/*
 * Copyright (c) 2012	Lae,
 *			Enrique Gámez <egamez@edisson.com.mx>
 *
 * The present plugin it was based on the work of
 * Sagie Maoz <n0nick@php.net>
 * http://n0nick.net/jquery/password-strength/
 *
 * Copyright (c) 2010 Sagie Maoz <n0nick@php.net>
 * Licensed under the GPL license, see http://www.gnu.org/licenses/gpl-3.0.html 
 *
 * And some ideas on the work of Pawel Decowski,
 * specially the function 'normalize'.
 *
 * http://paweldecowski.github.com/jQuery-CreditCardValidator/
 *
 */
(function($) {

var inere = {

	init: function(options) {
		var settings = $.extend({
			container: null
		}, options);

		return this.each(function() {

			var container = null;
			if ( settings.container ) {
				container = $( settings.container );
			} else {
				container = $( "<span/>" ).attr( "class" , "rfc-verificador" );
				$( this ).after( container );
			}

			$(this).bind( "keyup.rfc_verificador", function() {

				var val = inere.rfcNormalize($(this).val().toUpperCase());
				var bienformado = inere.rfcWellFormed(val);

				if ( bienformado ) {

					if ( inere.rfcValidate(val) ) {
						console.log("Valid");
						container.text( "Valido" ).attr( "class", "rfc-verificador" );
					} else {
						container.text( "No valido" ).attr( "class", "rfc-verificador" );
					}

				} else {
					container.text( "" ).attr( "class", "rfc_verificador" );
				}
			});

			if ($(this).val() != '') { // thanks Jason Judge
				$(this).trigger('keyup.rfc_verificador');
			}
		});

	},

	rfcGetVerificatorDigit: function( rfc ) {
		var anexo3 =	{'0': 0, '1': 1, '2': 2, '3': 3,  '4': 4, '5': 5, '6': 6, '7': 7, '8': 8, '9': 9,
				 'A':10, 'B':11, 'C':12, 'D':13,  'E':14, 'F':15, 'G':16, 'H':17, 'I':18, 'J':19,
				 'K':20, 'L':21, 'M':22, 'N':23,  '&':24, 'O':25, 'P':26, 'Q':27, 'R':28, 'S':29,
				 'T':30, 'U':31, 'V':32, 'W':33,  'X':34, 'Y':35, 'Z':36, ' ':37, \u00D1:38};
		var current;
		var suma = 0;
		var acumulado = 0;
		var remainder = 0;
		var index = 13;
		var digit = '';
		var pattern = /\w+(?!$)/; // Pattern to remove the last character.
		var rfc_small = null;

		if ( rfc.length == 12 ) {
			rfc_small = ' ' + pattern.exec(rfc);
		} else {
			rfc_small = '' + pattern.exec(rfc);
		}

		jQuery.each(rfc_small, function() {
			current = this;
			jQuery.each(anexo3, function(key, val) {
				suma = val;
				return (current != key);
			});
			acumulado += suma * index;
			index -= 1;
		});

		remainder = acumulado % 11;
		// Now assign the 'digito verificador'
		if	( remainder == 0 )	digit = '0';
		else if ( remainder == 1 )	digit = 'A';
		else				digit = 11-remainder;

		return digit;
	},

	rfcWellFormed: function( rfc ) {
		// The following regexp seems to capture the full 'Clave del RFC'
		// but is used a more relaxed one to capture possible mistakes
		// on the 'digito verificado de la clave del RFC', mistakes
		// like using some other letter instead of the unique valid ('A')
		//var regexp = /^([A-Z&\u00D1]{3,4})([0-9]{2})((0[1-9])|(1[012]))((0[1-9])|([12][0-9])|(3[01]))([0-9A-Z]{2})[0-9A]$/;
		var regexp = /^([A-Z&\u00D1]{3,4})([0-9]{2})((0[1-9])|(1[012]))((0[1-9])|([12][0-9])|(3[01]))[0-9A-Z]{3}$/;
		return regexp.test( rfc);
	},

	rfcValidate: function( rfc ) {
		var digit = inere.rfcGetVerificatorDigit(rfc);
		var pattern = /\w(?=$)/;
		var supplied = pattern.exec(rfc);
		return (digit == supplied);
	},

	rfcNormalize: function( rfc ) {
		// Remove any space or dash character.
		return rfc.replace(/[ -]/g, '');
	},

};

$.fn.rfc_verificador = function( method ) {

	if ( inere[method] ) {
		return inere[method].apply( this, Array.prototype.slice.call( arguments, 1 ));
	} else if ( typeof method === 'object' || ! method ) {
		return inere.init.apply( this, arguments );
	} else {
		$.error( 'Method ' +  method + ' does not exist on jQuery.rfc_verificador.' );
	}

};

})(jQuery);
