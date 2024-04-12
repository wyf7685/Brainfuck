#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "src/instream.h"
#include "src/interpreter.h"
#include "src/loader.h"
#include <algorithm>
#include <map>
#include <memory>
#include <sstream>

namespace brainfuck {

static PyObject *parse_file(PyObject *self, PyObject *args) {
  char *filename;
  Py_ssize_t length;
  if (!PyArg_ParseTuple(args, "s#", &filename, &length)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return nullptr;
  }

  try {
    std::string code = bf::loader::clean_code(bf::loader::parse_file(filename));
    return PyUnicode_FromString(code.c_str());
  } catch (bf::Exception &e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return nullptr;
  }
}

static PyObject *parse_string(PyObject *self, PyObject *args) {
  char *str;
  Py_ssize_t length;
  if (!PyArg_ParseTuple(args, "s#", &str, &length)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return nullptr;
  }

  try {
    std::string code = bf::loader::clean_code(bf::loader::parse_string(str));
    return PyUnicode_FromString(code.c_str());
  } catch (bf::Exception &e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return nullptr;
  }
}

static PyObject *clean_code(PyObject *self, PyObject *args) {
  char *code;
  Py_ssize_t length;
  if (!PyArg_ParseTuple(args, "s#", &code, &length)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return nullptr;
  }

  std::string cleaned = bf::loader::clean_code(code);
  return PyUnicode_FromString(cleaned.c_str());
}

namespace instream {

enum class StreamType {
  Stdin = 0,
  File,
  String,
};

typedef struct {
  PyObject_HEAD;
  StreamType mode;
  std::shared_ptr<bf::InStream> instream;
} InStream;

static PyObject *InStream_new(PyTypeObject *type, PyObject *args,
                              PyObject *kwds) {
  InStream *self = (InStream *)type->tp_alloc(type, 0);
  if (self != nullptr) {
    self->instream = std::make_shared<bf::InStream>();
    self->mode = StreamType::Stdin;
  }
  return (PyObject *)self;
}

static void InStream_dealloc(InStream *self) {
  Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *set_stdin(InStream *self, PyObject *args) {
  self->instream->setup_stdin();
  self->mode = StreamType::Stdin;
  Py_RETURN_NONE;
}

static PyObject *set_file(InStream *self, PyObject *args) {
  char *filename;
  Py_ssize_t length;
  if (!PyArg_ParseTuple(args, "s#", &filename, &length)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return nullptr;
  }

  self->instream->setup_file(filename);
  self->mode = StreamType::File;
  Py_RETURN_NONE;
}

static PyObject *set_string(InStream *self, PyObject *args) {
  char *str;
  Py_ssize_t length;
  if (!PyArg_ParseTuple(args, "s#", &str, &length)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return nullptr;
  }

  self->instream->setup_string(str);
  self->mode = StreamType::String;
  Py_RETURN_NONE;
}

static PyObject *repr(InStream *self) {
  std::stringstream ss;
  ss << "<InStream mode=";
  switch (self->mode) {
  case StreamType::Stdin:
    ss << "stdin";
    break;
  case StreamType::File:
    ss << "file";
    break;
  case StreamType::String:
    ss << "string";
    break;
  }
  ss << ">";
  return PyUnicode_FromString(ss.str().c_str());
}

static PyMemberDef members[] = {
    {"mode", Py_T_INT, offsetof(InStream, mode), 0}, {nullptr} /* Sentinel */
};

static PyMethodDef methods[] = {
    {"set_stdin", (PyCFunction)set_stdin, METH_VARARGS},
    {"set_file", (PyCFunction)set_file, METH_VARARGS},
    {"set_string", (PyCFunction)set_string, METH_VARARGS},
    {nullptr} /* Sentinel */
};

static PyTypeObject InStreamType = {
    .ob_base = {{{1}, (0)}, (0)},
    .tp_name = "brainfuck.InStream",
    .tp_basicsize = sizeof(InStream),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor)InStream_dealloc,
    .tp_repr = (reprfunc)repr,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = methods,
    .tp_members = members,
    .tp_new = InStream_new,
};

} // namespace instream

namespace interpreter {

typedef struct {
  PyObject_HEAD;
  instream::InStream *inStreamObj;
  std::unique_ptr<bf::Interpreter> interpreter;
} Interpreter;

static PyObject *Interpreter_new(PyTypeObject *type, PyObject *args,
                                 PyObject *kwds) {
  PyObject *inStream;
  PyObject *instream_type = (PyObject *)(&instream::InStreamType);

  if (!PyArg_ParseTuple(args, "O", &inStream) ||
      !PyObject_IsInstance(inStream, instream_type)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return nullptr;
  }

  Interpreter *self = (Interpreter *)type->tp_alloc(type, 0);
  if (self != nullptr) {
    Py_INCREF(inStream);
    self->inStreamObj = (instream::InStream *)inStream;
    self->interpreter =
        std::make_unique<bf::Interpreter>(self->inStreamObj->instream);
  }
  return (PyObject *)self;
}

static void Interpreter_dealloc(Interpreter *self) {
  Py_DECREF(self->inStreamObj);
  Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *execute(Interpreter *self, PyObject *args) {
  char *code;
  Py_ssize_t length;
  if (!PyArg_ParseTuple(args, "s#", &code, &length)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return nullptr;
  }

  try {
    self->interpreter->execute(code);
  } catch (bf::Exception &e) {
    PyErr_SetString(PyExc_RuntimeError, e.what());
    return nullptr;
  }

  Py_RETURN_NONE;
}

static PyObject *dump_memory(Interpreter *self, PyObject *args) {
  auto &memory = self->interpreter->memory;

  auto it = std::find_if(memory.rbegin(), memory.rend(),
                         [](const bf::MemoryValue &v) { return !!v; });
  size_t size = memory.rend() - it;

  PyObject *list = PyList_New(size);
  if (!list)
    return nullptr;

  for (; it != memory.rend(); it++) {
    PyObject *py_int = PyLong_FromUnsignedLong(static_cast<unsigned long>(*it));
    if (!py_int) {
      Py_DECREF(list);
      return nullptr;
    }
    PyList_SetItem(list, --size, py_int);
  }

  return list;
}

static PyObject *set_instream(Interpreter *self, PyObject *args) {
  PyObject *inStream;
  PyObject *instream_type = (PyObject *)(&instream::InStreamType);

  if (!PyArg_ParseTuple(args, "O", &inStream) ||
      !PyObject_IsInstance(inStream, instream_type)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return nullptr;
  }

  Py_DECREF(self->inStreamObj);
  Py_INCREF(inStream);
  self->inStreamObj = (instream::InStream *)inStream;
  self->interpreter->inStream = self->inStreamObj->instream;
  Py_RETURN_NONE;
}

static PyObject *set_memory(Interpreter *self, PyObject *args) {
  int v1, v2;
  if (!PyArg_ParseTuple(args, "ii", &v1, &v2)) {
    PyErr_SetString(PyExc_TypeError, "Argument error");
    return nullptr;
  }
  auto &memory = self->interpreter->memory;
  size_t pos = static_cast<size_t>(v1);
  bf::MemoryValue value = static_cast<bf::MemoryValue>(v2);

  while (pos >= memory.size())
    memory.emplace_back(0);
  memory[pos] = value;
  Py_RETURN_NONE;
}

static PyObject *repr(Interpreter *self) {
  std::stringstream ss;
  ss << "<Interpreter stream="
     << PyUnicode_AsUTF8(instream::repr(self->inStreamObj)) << ">";
  return PyUnicode_FromString(ss.str().c_str());
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"

static PyMemberDef members[] = {
    {"stream", Py_T_OBJECT_EX, offsetof(Interpreter, inStreamObj), 0},
    {nullptr} /* Sentinel */
};

#pragma clang diagnostic pop

static PyMethodDef methods[] = {
    {"execute", (PyCFunction)execute, METH_VARARGS},
    {"dump_memory", (PyCFunction)dump_memory, METH_VARARGS},
    {"set_instream", (PyCFunction)set_instream, METH_VARARGS},
    {"set_memory", (PyCFunction)set_memory, METH_VARARGS},
    {nullptr} /* Sentinel */
};

static PyTypeObject InterpreterType = {
    .ob_base = {{{1}, (0)}, (0)},
    .tp_name = "brainfuck.Interpreter",
    .tp_basicsize = sizeof(Interpreter),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor)Interpreter_dealloc,
    .tp_repr = (reprfunc)repr,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_methods = methods,
    .tp_members = members,
    .tp_new = Interpreter_new,
};

} // namespace interpreter

static PyMethodDef ModuleMethods[] = {
    {"parse_file", parse_file, METH_VARARGS},
    {"parse_string", parse_string, METH_VARARGS},
    {"clean_code", clean_code, METH_VARARGS},
    {nullptr, nullptr, 0, nullptr},
};

static struct PyModuleDef Module = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "brainfuck",
    .m_doc = "Brainfuck interpreter by wyf7685",
    .m_methods = ModuleMethods,
};

static std::map<std::string, PyTypeObject *> Types = {
    {"InStream", &instream::InStreamType},
    {"Interpreter", &interpreter::InterpreterType},
};

} // namespace brainfuck

PyMODINIT_FUNC PyInit_brainfuck(void) {
  for (auto &t : brainfuck::Types) {
    if (PyType_Ready(t.second) < 0)
      return nullptr;
  }

  PyObject *module = PyModule_Create(&brainfuck::Module);
  if (!module)
    return nullptr;

  for (auto &t : brainfuck::Types) {
    Py_INCREF(t.second);
    PyModule_AddObject(module, t.first.c_str(), (PyObject *)t.second);
  }

  return module;
}
