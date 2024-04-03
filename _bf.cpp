#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "src/instream.h"
#include "src/interpreter.h"
#include "src/loader.h"
#include <sstream>

namespace internal {
using namespace bf;

// InStream
std::map<uint64_t, InStream> instream_map;
uint64_t instream_id = 0;

static uint64_t create_instream() {
  InStream instream;
  instream_map[instream_id++] = instream;
  return instream_id - 1;
}

static void destroy_instream(uint64_t id) {
  if (instream_map.find(id) != instream_map.end())
    instream_map.erase(id);
}

// Interpreter
std::map<uint64_t, Interpreter> interpreter_map;
uint64_t interpreter_id = 0;

static uint64_t create_interpreter(const InStream &instream) {
  interpreter_map[interpreter_id++] = Interpreter(instream);
  return interpreter_id - 1;
}

static void destroy_interpreter(uint64_t id) {
  if (interpreter_map.find(id) != interpreter_map.end())
    interpreter_map.erase(id);
}

} // namespace internal

namespace pymethod {

#define PYMETHOD(identifier)                                                   \
  static PyObject *identifier(PyObject *self, PyObject *args)
#define CHECK_ID(where, id)                                                    \
  if (internal::where.find(id) == internal::where.end()) {                     \
    std::stringstream ss;                                                      \
    ss << "ID " << id << " does not exist";                                    \
    PyErr_SetString(PyExc_ValueError, ss.str().c_str());                       \
    return NULL;                                                               \
  }
#define CHECK_INSTREAM(id) CHECK_ID(instream_map, id)
#define CHECK_INTERPRETER(id) CHECK_ID(interpreter_map, id)

namespace instream {

PYMETHOD(create) {
  uint64_t id = internal::create_instream();
  PyObject *py_int = PyLong_FromUnsignedLongLong(id);
  if (!py_int) {
    internal::destroy_instream(id);
    return NULL;
  }
  return py_int;
}

PYMETHOD(destroy) {
  int value;
  if (!PyArg_ParseTuple(args, "i", &value)) {
    PyErr_SetString(PyExc_TypeError, "Argument must be an integer");
    return NULL;
  }
  uint64_t id = static_cast<uint64_t>(value);
  CHECK_INSTREAM(id);

  internal::destroy_instream(id);
  Py_RETURN_NONE;
}

PYMETHOD(set_stdin) {
  int value;
  if (!PyArg_ParseTuple(args, "i", &value)) {
    PyErr_SetString(PyExc_TypeError, "Argument must be an integer");
    return NULL;
  }
  uint64_t id = static_cast<uint64_t>(value);
  CHECK_INSTREAM(id);

  internal::instream_map[id].setup_stdin();
  Py_RETURN_NONE;
}

PYMETHOD(set_file) {
  int value;
  char *filename;
  Py_ssize_t length;
  if (!PyArg_ParseTuple(args, "is#", &value, &filename, &length)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return NULL;
  }
  uint64_t id = static_cast<uint64_t>(value);
  CHECK_INSTREAM(id);

  if (internal::instream_map[id].setup_file(filename))
    Py_RETURN_TRUE;
  Py_RETURN_FALSE;
}

PYMETHOD(set_string) {
  int value;
  char *str;
  Py_ssize_t length;
  if (!PyArg_ParseTuple(args, "is#", &value, &str, &length)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return NULL;
  }
  uint64_t id = static_cast<uint64_t>(value);
  CHECK_INSTREAM(id);

  internal::instream_map[id].setup_string(str);
  Py_RETURN_NONE;
}

} // namespace instream

namespace interpreter {

PYMETHOD(create) {
  int value;
  if (!PyArg_ParseTuple(args, "i", &value)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return NULL;
  }
  uint64_t sid = static_cast<uint64_t>(value);
  CHECK_INSTREAM(sid);

  bf::InStream &inStream = internal::instream_map[sid];
  uint64_t id = internal::create_interpreter(inStream);
  PyObject *py_int = PyLong_FromUnsignedLongLong(id);
  if (!py_int) {
    internal::destroy_instream(id);
    return NULL;
  }
  return py_int;
}

PYMETHOD(destroy) {
  int value;
  if (!PyArg_ParseTuple(args, "i", &value)) {
    PyErr_SetString(PyExc_TypeError, "Argument must be an integer");
    return NULL;
  }
  uint64_t id = static_cast<uint64_t>(value);
  CHECK_INTERPRETER(id);

  internal::destroy_interpreter(id);
  Py_RETURN_NONE;
}

PYMETHOD(execute) {
  int value;
  char *code;
  Py_ssize_t length;
  if (!PyArg_ParseTuple(args, "is#", &value, &code, &length)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return NULL;
  }
  uint64_t id = static_cast<uint64_t>(value);
  CHECK_INTERPRETER(id);

  internal::interpreter_map[id].execute(code);
  Py_RETURN_NONE;
}

PYMETHOD(dump_memory) {
  int value;
  if (!PyArg_ParseTuple(args, "i", &value)) {
    PyErr_SetString(PyExc_TypeError, "Argument must be an integer");
    return NULL;
  }
  uint64_t id = static_cast<uint64_t>(value);
  CHECK_INTERPRETER(id);

  bf::Interpreter &interpreter = internal::interpreter_map[id];
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

PYMETHOD(set_instream) {
  int v1, v2;
  if (!PyArg_ParseTuple(args, "ii", &v1, &v2)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return NULL;
  }
  uint64_t sid = static_cast<uint64_t>(v1);
  CHECK_INSTREAM(sid);
  uint64_t id = static_cast<uint64_t>(v2);
  CHECK_INTERPRETER(id);

  internal::interpreter_map[id].inStream = internal::instream_map[sid];
  Py_RETURN_NONE;
}

PYMETHOD(set_memory) {
  int v1, v2, v3;
  if (!PyArg_ParseTuple(args, "iii", &v1, &v2, &v3)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return NULL;
  }
  uint64_t id = static_cast<uint64_t>(v1);
  CHECK_INTERPRETER(id);
  size_t pos = static_cast<size_t>(v2);
  bf::MemoryValue value = static_cast<bf::MemoryValue>(v3);

  auto &memory = internal::interpreter_map[id].memory;

  while (pos >= memory.size())
    memory.emplace_back(0);
  memory[pos] = value;

  Py_RETURN_NONE;
}

} // namespace interpreter

PYMETHOD(parse_file) {
  char *filename;
  Py_ssize_t length;
  if (!PyArg_ParseTuple(args, "s#", &filename, &length)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return NULL;
  }

  std::string code = bf::loader::read_code_file(filename);
  return PyUnicode_FromString(code.c_str());
}

PYMETHOD(clean_code) {
  char *code;
  Py_ssize_t length;
  if (!PyArg_ParseTuple(args, "s#", &code, &length)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return NULL;
  }

  std::string cleaned = bf::loader::clean_code(code);
  return PyUnicode_FromString(cleaned.c_str());
}

static PyMethodDef methods[] = {
    {"stream_create", instream::create, METH_VARARGS},
    {"stream_destroy", instream::destroy, METH_VARARGS},
    {"stream_set_stdin", instream::set_stdin, METH_VARARGS},
    {"stream_set_file", instream::set_file, METH_VARARGS},
    {"stream_set_string", instream::set_string, METH_VARARGS},
    {"interp_create", interpreter::create, METH_VARARGS},
    {"interp_destroy", interpreter::destroy, METH_VARARGS},
    {"interp_execute", interpreter::execute, METH_VARARGS},
    {"interp_dump_memory", interpreter::dump_memory, METH_VARARGS},
    {"interp_set_instream", interpreter::set_instream, METH_VARARGS},
    {"interp_set_memory", interpreter::set_memory, METH_VARARGS},
    {"parse_file", parse_file, METH_VARARGS},
    {"clean_code", clean_code, METH_VARARGS},
    {NULL, NULL, 0, NULL},
};

} // namespace pymethod

static struct PyModuleDef bf_module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "brainfuck",
    .m_doc = "Brainfuck interpreter by wyf7685",
    .m_methods = pymethod::methods,
};

PyMODINIT_FUNC PyInit__brainfuck(void) { return PyModule_Create(&bf_module); }
