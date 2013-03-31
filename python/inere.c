#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "inere/personafisica.h"
#include "inere/verificador.h"
#include "inere/util.h"
#include "inere/cantidades.h"
#include "inere/verifica_sello_digital.h"

static PyObject *InereError;

static PyObject *
inere_rfc(PyObject *self, PyObject *args, PyObject *keywds)
{
  const char *nombre;
  const char *paterno;
  const char *materno;
  int day;
  int month;
  int year;
  char rfc[18];
  char dia[3];
  char mes[3];
  char anio[5];
  static char *kwlist[] = {"nombre",
			   "paterno",
			   "materno",
			   "día",
			   "mes",
			   "año",
			   NULL};

  if ( !PyArg_ParseTupleAndKeywords(args, keywds, "sssiii", kwlist, &nombre,
				    &paterno, &materno, &day, &month, &year) )
    return NULL;

  memset(rfc, 0, 18);
  sprintf(dia, "%d", day);
  sprintf(mes, "%d", month);
  sprintf(anio, "%d", year);
  clave_rfc_persona_fisica(rfc, nombre, paterno, materno, anio, mes, dia, 0);

  if ( !strlen(rfc) ) {
    /* An error has ocurred */
    PyErr_SetString(InereError, "Clave del R.F.C. nula.");
    return NULL;
  }

  return Py_BuildValue("s", rfc);
}

static PyObject *
inere_rfcinfo(PyObject *self, PyObject *args, PyObject *keywds)
{
  const char *nombre;
  const char *paterno;
  const char *materno;
  int day;
  int month;
  int year;
  char rfc[18];
  char homonimias[3];
  char digito[2];
  size_t len = 0;
  char dia[3];
  char mes[3];
  char anio[5];
  static char *kwlist[] = {"nombre",
			   "paterno",
			   "materno",
			   "día",
			   "mes",
			   "año",
			   NULL};

  if ( !PyArg_ParseTupleAndKeywords(args, keywds, "sssiii", kwlist, &nombre,
				    &paterno, &materno, &day, &month, &year) )
    return NULL;

  sprintf(dia, "%d", day);
  sprintf(mes, "%d", month);
  sprintf(anio, "%d", year);

  memset(rfc, 0, 18);
  clave_rfc_persona_fisica(rfc, nombre, paterno, materno, anio, mes, dia, 0);

  len = strlen(rfc);
  if ( len == 0 ) {
    /* An error has ocurred */
    PyErr_SetString(InereError, "Clave del R.F.C. nula.");
    return NULL;
  }

  /* Extrae la clave diferenciadora de homonimias */
  snprintf(homonimias, 3, "%c%c", rfc[len-3], rfc[len-2]);

  /* Extrae el digito verificado */
  snprintf(digito, 2, "%c", rfc[len-1]);

  return Py_BuildValue("{s:s,s:s,s:s}", "rfc", rfc,
					"homonimia", homonimias,
					"digito", digito);
}

static PyObject *
inere_verificarfc(PyObject *self, PyObject *args, PyObject *keywds)
{
  const char *rfc;
  char result = 0;
  char *clave = 0;
  Py_ssize_t size;
  static char *kwdlist[] = {"rfc", NULL};

  if ( !PyArg_ParseTupleAndKeywords(args, keywds, "s#", kwdlist, &rfc, &size) )
    return NULL;

  if ( size < 13 ) {
    /* For 'personas fisicas' the RFC must have at least 13 characters */ 
    PyErr_SetString(InereError, "La clave del R.F.C. debe ser de al menos 13 carácteres (para personas físicas). Es imposible verificar la clave.");
    return NULL;
  }

  /* Capitalize */
  clave = to_upper_case_and_convert((unsigned char*)rfc);

  result = verifica_rfc(clave, 0);
  free(clave);

  if ( result == 0 ) return Py_BuildValue("O", Py_True);
  else		     return Py_BuildValue("O", Py_False);

  return NULL;
}

static PyObject *
inere_cantidadconletras(PyObject *self, PyObject *args, PyObject *keywds)
{
  double number;
  int rounded = 0; /*By default do not round the number to the nearest integer*/
  char q[16];
  char nombre[MAXNUMERAL];
  static char *kwdlist[] = {"cantidad", "round", NULL};

  if ( !PyArg_ParseTupleAndKeywords(args, keywds, "d|p", kwdlist,
					&number, &rounded) )
    return NULL;

  if ( rounded ) {
    number = round(number);
  }

  snprintf(q, 16, "%.2f", number);
  memset(nombre, 0, MAXNUMERAL);
  cantidad(nombre, q, 0);

  return Py_BuildValue("s", nombre);

}

static PyObject *
inere_verificasello(PyObject *self, PyObject *args, PyObject *keywds)
{
  const char *cfd;
  const char *stylesheet;
  int debug;
  int result = 0;
  static char *kwdlist[] = {"cfd",
			   "stylesheet",
			   "debug",
			   NULL};

  if ( !PyArg_ParseTupleAndKeywords(args, keywds, "ss|i", kwdlist,
					&cfd, &stylesheet, &debug) )
    return NULL;

  /* Verifica el sello */
  result = verifica_sello_digital(cfd, (const unsigned char *)stylesheet, debug);

  if ( result == 1 ) return Py_BuildValue("O", Py_True);
  else		     return Py_BuildValue("O", Py_False);

  return NULL;
}


static PyMethodDef InereMethods[] = {
	{"rfc", (PyCFunction)inere_rfc,
	 METH_VARARGS | METH_KEYWORDS,
	 "Función para obtener la clave del R.F.C. Mexicano."},
	{"rfcinfo", (PyCFunction)inere_rfcinfo,
	 METH_VARARGS | METH_KEYWORDS,
	 "Función para obtener la clave del R.F.C. Mexicano, la clave diferenciadora de homonimias y el dígito verificador de la clave."},
	{"verificarfc", (PyCFunction)inere_verificarfc,
	 METH_VARARGS | METH_KEYWORDS,
	 "Función para verificar la clave del R.F.C. Mexicano."},
	{"cantidadconletras", (PyCFunction)inere_cantidadconletras,
	 METH_VARARGS | METH_KEYWORDS,
	 "Función para expresar la cantidad dada a su representación con letras, en pesos mexicanos."},
	{"verificasello", (PyCFunction)inere_verificasello,
	 METH_VARARGS | METH_KEYWORDS,
	 "Función para verificar la correctez del sello digital que ampara la ntegridad del CFD"},
	{NULL, NULL, 0, NULL}
};

#ifdef PYTHON_ABI_VERSION
static struct PyModuleDef ineremodule = {
	PyModuleDef_HEAD_INIT,
	"inere",
	NULL,
	-1,
	InereMethods
};

PyMODINIT_FUNC
PyInit_inere(void)
{
  PyObject *m;

  m = PyModule_Create(&ineremodule);
  if ( m == NULL ) return NULL;

  InereError = PyErr_NewException("inere.error", NULL, NULL);
  Py_INCREF(InereError);
  PyModule_AddObject(m, "error", InereError);

  return m;
}
#else
PyMODINIT_FUNC
initinere(void)
{
  PyObject *m;

  m = Py_InitModule("inere", InereMethods);
  if ( m == NULL ) return;

  InereError = PyErr_NewException("inere.error", NULL, NULL);
  if ( InereError == NULL ) return;
  Py_INCREF(InereError);
  PyModule_AddObject(m, "error", InereError);

}
#endif
