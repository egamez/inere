from distutils.core import setup, Extension

module1 = Extension('inere',
		    define_macros = [('MAJOR_VERSION', '0'),
				     ('MINOR_VERSION', '3')],
		    include_dirs = ['../include'],
		    libraries = ['inere'],
		    library_dirs = ['../lib/.libs'],
		    sources = ['inere.c'])

setup (	name = 'inere',
	version = '1.0',
	description = 'Paquete para el calculo del R.F.C. Mexicano.',
	author = 'Enrique Gamez Flores',
	author_email = 'egamez@edisson.com.mx',
	url = 'http://edisson.com.mx',
	ext_modules = [module1])
