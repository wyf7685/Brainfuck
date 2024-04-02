#define PY_SSIZE_T_CLEAN

#ifdef _BRAINFUCK_RELEASE
#include <Python.h>
#else
#include "C:\Program Files\Python312\include\Python.h"
#endif

#include "src/interpreter.h"
#include "src/loader.h"

static PyObject *_execute_code(const std::string &code) {
  bf::Interpreter interpreter(bf::Loader::clean_code(code));
  interpreter.execute();
  auto &memory = interpreter.memory;

  auto it = std::find_if(memory.rbegin(), memory.rend(),
                         [](const bf::MemoryValue &v) { return !!v; });
  size_t size = memory.rend() - it;

  PyObject *list = PyList_New(size);
  if (!list)
    return NULL;

  for (; it != memory.rend(); it++) {
    PyObject *py_int = PyLong_FromUnsignedLong(static_cast<unsigned long>(*it));
    if (!py_int) {
      Py_DECREF(list);
      return NULL;
    }
    PyList_SetItem(list, --size, py_int);
  }

  return list;
}

static PyObject *execute(PyObject *self, PyObject *args) {
  char *bf_code;
  Py_ssize_t length;
  if (!PyArg_ParseTuple(args, "s#", &bf_code, &length))
    return NULL;

  return _execute_code(bf_code);
}

static PyObject *run_file(PyObject *self, PyObject *args) {
  char *filename;
  Py_ssize_t length;
  if (!PyArg_ParseTuple(args, "s#", &filename, &length))
    return NULL;

  std::string code = bf::Loader::read_code_file(filename);
  return _execute_code(code);
}

static PyMethodDef methods[] = {
    {"execute", execute, METH_VARARGS, "execute a string of brainfuck script"},
    {"run_file", run_file, METH_VARARGS, "execute a file of brainfuck script"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef bf_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "brainfuck",
    .m_doc = "BrainFuck interpreter by wyf7685",
    .m_methods = methods,
};

PyMODINIT_FUNC PyInit__brainfuck(void) { return PyModule_Create(&bf_module); }
