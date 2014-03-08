#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "inere/personafisica.h"
#include "inere/personamoral.h"
#include "inere/verificador.h"
#include "inere/util.h"
#include "inere/cantidades.h"
#include "inere/verifica_sello_digital.h"
#include "inere/extrae_fecha.h"

static PyObject *InereError;
static PyObject *InereClaveIncompleta;

static PyObject *
inere_rfc(PyObject *self, PyObject *args, PyObject *keywds)
{
  const char *nombre = NULL;
  const char *paterno = NULL;
  const char *materno = NULL;
  const char *razonsocial = NULL;
  const char *tiposociedad = NULL;
  int day;
  int month;
  int year;
  char rfc[18];
  char dia[3];
  char mes[3];
  char anio[5];
  static char *kwlist[] = {"dia",
			   "mes",
			   "anio",
			   "nombre",
			   "paterno",
			   "materno",
			   "razonsocial",
			   "tiposociedad",
			   NULL};

  if ( !PyArg_ParseTupleAndKeywords(args, keywds, "iii|sssss", kwlist, &day,
				    &month, &year, &nombre, &paterno, &materno,
				    &razonsocial, &tiposociedad) )
    return NULL;

  /* Now, make some checks on the options passed */
  if ( nombre == razonsocial ) {
    /* There's no way to perform any calculation at all */
    PyErr_SetString(InereError, "Es necesario especificar el nombre de la persona física, o la razón social de la persona moral.");
    return NULL;

  } else if ( razonsocial == NULL ) {
    /* This is the case for a persona fisica, check that you have at least
     * one apellido, passed as apellido paterno
     */
    if ( paterno == NULL ) {
      /* There's no way to perform any calculation at all */
      PyErr_SetString(InereError, "Es necesario especificar al menos una apellido, para la persona fisica. El apellido deberá ser dado como si se tratase de un apellido paterno (aunque no lo sea.)");
      return NULL;

    }

  } else {
    /* Este es el posiblemente el caso de una persona moral, verifica que el
     * nombre no halla sido pasado como argumento, tambien.
     */
    if ( nombre != NULL ) {
      /* No es posible realizar el calculo de la clave */
      PyErr_SetString(InereError, "Ambos, razón social y nombre fueron especificados, no se realizara el calculo de la clave del R.F.C");
      return NULL;

    }

  }

  memset(rfc, 0, 18);
  sprintf(dia, "%d", day);
  sprintf(mes, "%d", month);
  sprintf(anio, "%d", year);
  if ( razonsocial == NULL ) {
    /* Clave del R.F.C. para una persona fisica */
    clave_rfc_persona_fisica(rfc, nombre, paterno, materno, anio, mes, dia, 0);
  } else {
    /* Clave del R.F.C. para una persona moral */
    clave_rfc_persona_moral(rfc, razonsocial, tiposociedad, anio, mes, dia, 0);
  }

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
  const char *nombre = NULL;
  const char *paterno = NULL;
  const char *materno = NULL;
  const char *razonsocial = NULL;
  const char *tiposociedad = NULL;
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
  static char *kwlist[] = {"dia",
			   "mes",
			   "anio",
			   "nombre",
			   "paterno",
			   "materno",
			   "razonsocial",
			   "tiposociedad",
			   NULL};

  if ( !PyArg_ParseTupleAndKeywords(args, keywds, "iii|sssss", kwlist, &day,
				    &month, &year, &nombre, &paterno, &materno,
				    &razonsocial, &tiposociedad) )
    return NULL;

  /* Now, make some checks on the options passed */
  if ( nombre == razonsocial ) {
    /* There's no way to perform any calculation at all */
    PyErr_SetString(InereError, "Es necesario especificar el nombre de la persona física, o la razón social de la persona moral.");
    return NULL;

  } else if ( razonsocial == NULL ) {
    /* This is the case for a persona fisica, check that you have at least
     * one apellido, passed as apellido paterno
     */
    if ( paterno == NULL ) {
      /* There's no way to perform any calculation at all */
      PyErr_SetString(InereError, "Es necesario especificar al menos una apellido, para la persona fisica. El apellido deberá ser dado como si se tratase de un apellido paterno (aunque no lo sea.)");
      return NULL;

    }

  } else {
    /* Este es el posiblemente el caso de una persona moral, verifica que el
     * nombre no halla sido pasado como argumento, tambien.
     */
    if ( nombre != NULL ) {
      /* No es posible realizar el calculo de la clave */
      PyErr_SetString(InereError, "Ambos, razón social y nombre fueron especificados, no se realizara el calculo de la clave del R.F.C");
      return NULL;

    }

  }

  sprintf(dia, "%d", day);
  sprintf(mes, "%d", month);
  sprintf(anio, "%d", year);

  memset(rfc, 0, 18);
  if ( razonsocial == NULL ) {
    /* Clave del R.F.C. para una persona fisica */
    clave_rfc_persona_fisica(rfc, nombre, paterno, materno, anio, mes, dia, 0);
  } else {
    /* Clave del R.F.C. para una persona moral */
    clave_rfc_persona_moral(rfc, razonsocial, tiposociedad, anio, mes, dia, 0);
  }

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

static PyObject *
inere_extraefecha(PyObject *self, PyObject *args, PyObject *keywds)
{
  int result = 0;
  const char *semiclave = NULL;
  int verbose = 0;
  char day[3];
  char month[3];
  char year[3];
  static char *kwdlist[] = {"clave", "verbose", NULL};

  if ( !PyArg_ParseTupleAndKeywords(args, keywds, "s|i", kwdlist,
					&semiclave, &verbose) )
    return NULL;

  /* Extrae los datos correspondientes a la fecha de la clave del R.F.C. */
  result = extrae_fecha(semiclave, day, month, year, verbose);

  /* Ahora, nosotros deberiamos de definir algunas exceptions para los errores*/
  if ( result == -1 ) {
    PyErr_SetString(InereClaveIncompleta, "La clave del R.F.C. suministrada no contiene los datos de la fecha a extraer.");
    return NULL;

  } else if ( result == -2 ) {
    PyErr_SetString(InereClaveIncompleta, "La clave del R.F.C. suministrada no contiene los datos de la fecha a extraer.");
    return NULL;

  } else if ( result == -3 ) {
    PyErr_SetString(InereClaveIncompleta, "La clave del R.F.C. suministrada posiblemente contenga los datos de la fecha a extraer pero no contiene la cantidad de letras con las que la clave inicia, de modo que no es posible deternimar los datos de la fecha a extraer.");
    return NULL;

  } else if ( result == -4 ) {
    PyErr_SetString(InereError, "Error improbable. No es posible emitir algun diagnostico.");
    return NULL;

  } else if ( result == -5 ) {
    PyErr_SetString(InereError, "No fue posible extraer los datos correspondientes a la fecha.");
    return NULL;

  }

  return Py_BuildValue("{s:s,s:s,s:s}", "day", day,
					"month", month,
					"year", year);
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
	{"extraefecha", (PyCFunction)inere_extraefecha,
	 METH_VARARGS | METH_KEYWORDS,
	"Función para extraer los datos correspondientes a la fecha del contribuyente, que se encuentran en la clave del R.F.C."},
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

  /* A general exception */
  InereError = PyErr_NewException("inere.Error", NULL, NULL);
  Py_INCREF(InereError);
  PyModule_AddObject(m, "Error", InereError);

  /* An exception when the given R.F.C. code is not enough complete */
  InereClaveIncompleta = PyErr_NewException("inere.ClaveIncompleta", NULL, NULL);
  Py_INCREF(InereClaveIncompleta);
  PyModule_AddObject(m, "Error", InereClaveIncompleta);

  return m;
}
#else
PyMODINIT_FUNC
initinere(void)
{
  PyObject *m;

  m = Py_InitModule("inere", InereMethods);
  if ( m == NULL ) return;

  InereError = PyErr_NewException("inere.Error", NULL, NULL);
  if ( InereError == NULL ) return;
  Py_INCREF(InereError);
  PyModule_AddObject(m, "Error", InereError);

  /* An exception when the given R.F.C. code is not enough complete */
  InereClaveIncompleta = PyErr_NewException("inere.ClaveIncompleta", NULL, NULL);
  if ( InereClaveIncompleta == NULL ) return;
  Py_INCREF(InereClaveIncompleta);
  PyModule_AddObject(m, "Error", InereClaveIncompleta);

}
#endif
