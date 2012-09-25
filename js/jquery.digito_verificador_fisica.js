/*
 * Copyright (c) 2012	Lae,
 *			Enrique Gámez <egamez@edisson.com.mx>
 * Persona fisica.
 *
 * http://n0nick.net/jquery/password-strength/
 *
 * Password Strength (0.1.2)
 * by Sagie Maoz (n0nick.net)
 * n0nick@php.net
 *
 * This plugin will check the value of a password field and evaluate the
 * strength of the typed password. This is done by checking for
 * the diversity of character types: numbers, lowercase and uppercase
 * letters and special characters.
 *
 * Copyright (c) 2010 Sagie Maoz <n0nick@php.net>
 * Licensed under the GPL license, see http://www.gnu.org/licenses/gpl-3.0.html 
 *
 *
 * NOTE: This script requires jQuery to work.  Download jQuery at www.jquery.com
 *
 */
(function($) {

var digitoVerificador = new function()
{

	this.obtenDigitoVerificador = function(rfc)
	{
		var anexo3 =	{0:0, 1:1, 2:2, 3:3, 4:4, 5:5, 6:6, 7:7, 8:8, 9:9,
				A:10, B:11, C:12, D:13,   E:14, F:15, G:16, H:17, I:18, J:19,
				K:20, L:21, M:22, N:23, '&':24, O:25, P:26, Q:27, R:28, S:29,
				T:30, U:31, V:32, W:33,   X:34, Y:35, Z:36, ' ':37};
		var current;
		var suma = 0;
		var acumulado = 0;
		var remainder = 0;
		var index = 13;
		var digit = '';
		var rfc_small = rfc.substring(0, 12);

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
	};

	this.validaDigitoVerificador = function(rfc)
	{
		var digito = this.obtenDigitoVerificador(rfc);
		return (digito == rfc.charAt(12)) ? 1 : 0;
	};
};

$.fn.digito_verificador = function(options)
{
	var settings = $.extend({
		'minLength' : 6,
		'texts' : {
			0: 'Digito no concuerda',
			1: 'Digito identico'
		},
		'onCheck': null
	}, options);

	return this.each(function()
	{
		var container = null;
		container = $('<span/>').attr('class', 'digito_verificador');
		$(this).after(container);

		$(this).bind('keyup.digito_verificador', function()
		{
			var val = $(this).val();
			if ( val.length > 12 )
			{
				var level = digitoVerificador.validaDigitoVerificador(val);
				var _class = 'digito_verificador_' + level;

				if (!container.hasClass(_class) && level in settings.texts)
				{
					container.text(settings.texts[level]).attr('class', 'digito_verificador ' + _class);
				}
			}
			else
			{
				container.text('').attr('class', 'digito_verificador');
			}
			if ( settings.onCheck ) {
				settings.onCheck.call(this, level);
			}
		});

		if ($(this).val() != '') { // thanks Jason Judge
			$(this).trigger('keyup.digito_verificador');
		}
	});
};

})(jQuery);
