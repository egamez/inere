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

var methods = {

	init: function(options) {
		var settings = $.extend({
			'legendOnFail': '',
			'legendOnSuccess': '',
			'withColors': true
		}, options);

		return this.each(function() {


			$(this).bind('keyup.digito_verificador', function() {

				var val = methods.normalize($(this).val().toUpperCase());
				var bienformado = methods.rfcValido(val);
				var container = null;

				if ( settings.legendOnFail.length || settings.legendOnSuccess.length ) {
					container = $('<span/>').attr('class', 'digito_verificador');
					$(this).after(container);
				}

				if ( bienformado ) {
					var legend = '';
					var bg_color = '';
					if ( methods.validaDigitoVerificador(val) ) {
						legend = settings.legendOnSuccess;
						bg_color = '#00CC00';
					} else {
						legend = settings.legendOnFail;
						bg_color = '#990000';
					}
					if ( legend.length ) container.text(legend).attr('class', 'digito_verificador');
					if ( settings.withColors ) {
						$(this).css('color', '#fff')
							.css('background', bg_color);
					}

				} else {
					// This block will erase legends when the
					// the RFC clave isn't complete
					if ( settings.legendOnFail.length || settings.legendOnSuccess.length ) container.text('').attr('class', 'digito_verificador');
					$(this).css('color', '');
					$(this).css('background', '');
				}
			});

			if ($(this).val() != '') { // thanks Jason Judge
				$(this).trigger('keyup.digito_verificador');
			}
		});

	},

	obtenDigitoVerificador: function( rfc ) {
		var anexo3 =	{0:0, 1:1, 2:2, 3:3, 4:4, 5:5, 6:6, 7:7, 8:8, 9:9,
				A:10, B:11, C:12, D:13,   E:14, F:15, G:16, H:17, I:18, J:19,
				K:20, L:21, M:22, N:23, '&':24, O:25, P:26, Q:27, R:28, S:29,
				T:30, U:31, V:32, W:33,   X:34, Y:35, Z:36, ' ':37, \u00D1:38};
		var current;
		var suma = 0;
		var acumulado = 0;
		var remainder = 0;
		var index = 13;
		var digit = '';
		var pattern = /\w+(?!$)/; /* Remove the last character */
		var rfc_small = '';

		if ( rfc.length == 12 ) {
			rfc_small = ' ' + pattern.exec(rfc);
		} else {
			rfc_small = '' + pattern.exec(rfc); /* Boggus */
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
		// Ahora asigna el digito verificador.
		if	( remainder == 0 )	digit = '0';
		else if ( remainder == 1 )	digit = 'A';
		else				digit = 11-remainder;

		return digit;
	},

	rfcValido: function( rfc ) {
		var validRFC = true;
		var regexp = /^([A-Z\u00D1]{3,4})([0-9]{2})((0[1-9])|(1[012]))((0[1-9])|([12][0-9])|(3[01]))([0-9A-Z]{2})[0-9Z]$/;
		validRFC = regexp.test( rfc );
		return validRFC;
	},

	validaDigitoVerificador: function( rfc ) {
		var digito = methods.obtenDigitoVerificador(rfc);
		var pattern = /\w(?=$)/;
		var supplied = pattern.exec(rfc);
		return (digito == supplied);
	},

	normalize: function( rfc ) {
		return rfc.replace(/[ -]/g, '');
	},

};

$.fn.digito_verificador = function( method ) {

	if ( methods[method] ) {
		return methods[method].apply( this, Array.prototype.slice.call( arguments, 1 ));
	} else if ( typeof method === 'object' || ! method ) {
		return methods.init.apply( this, arguments );
	} else {
		$.error( 'Method ' +  method + ' does not exist on jQuery.tooltip' );
	}

};

})(jQuery);
