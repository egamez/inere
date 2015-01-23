#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "inere/cfdi/verifica_sello_digital.h"

static PyObject *InereCfdiError;

static PyObject *
inere_verificasello(PyObject *self, PyObject *args, PyObject *keywds)
{
  const char *cfd;
  const char *stylesheet;
  int verbose;
  int result = 0;
  static char *kwdlist[] = {"cfd",
			   "stylesheet",
			   "verbose",
			   NULL};

  if ( !PyArg_ParseTupleAndKeywords(args, keywds, "ss|i", kwdlist,
					&cfd, &stylesheet, &verbose) )
    return NULL;

  /* Verifica el sello */
  result = verifica_sello_digital(cfd, stylesheet, verbose);

  if ( result == 1 ) return Py_BuildValue("O", Py_True);
  else		     return Py_BuildValue("O", Py_False);

  return NULL;
}



static PyMethodDef InereCfdiMethods[] = {
	{"verificasello", (PyCFunction)inere_verificasello,
	 METH_VARARGS | METH_KEYWORDS,
	 "Función para verificar la correctez del sello digital que ampara la ntegridad del CFD"},
	{NULL, NULL, 0, NULL}
};

#ifdef PYTHON_ABI_VERSION
static struct PyModuleDef inerecfdimodule = {
	PyModuleDef_HEAD_INIT,
	"inerecfdi",
	NULL,
	-1,
	InereCfdiMethods
};

PyMODINIT_FUNC
PyInit_inerecfdi(void)
{
  PyObject *m;

  m = PyModule_Create(&inerecfdimodule);
  if ( m == NULL ) return NULL;

  /* A general exception */
  InereCfdiError = PyErr_NewException("inerecfdi.Error", NULL, NULL);
  Py_INCREF(InereCfdiError);
  PyModule_AddObject(m, "Error", InereCfdiError);

  return m;
}
#else
PyMODINIT_FUNC
initinerecfdi(void)
{
  PyObject *m;

  m = Py_InitModule("inerecfdi", InereCfdiMethods);
  if ( m == NULL ) return;

  InereCfdiError = PyErr_NewException("inerecfdi.Error", NULL, NULL);
  if ( InereCfdiError == NULL ) return;
  Py_INCREF(InereCfdiError);
  PyModule_AddObject(m, "Error", InereCfdiError);

}
#endif
