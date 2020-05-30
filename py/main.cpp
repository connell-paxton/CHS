#define PY_SSIZE_T_CLEAN
#define PYTHON_HTTPD
#include <Python.h>
#include "../server.hpp"
#include "stealfunc.hpp"
#include <unistd.h>


HTTPServer* server;
#define HTTPD_FUNC static PyObject*
#define HTTPD_ARGS (PyObject* self, PyObject* args)

HTTPD_FUNC HTTPDServer_init HTTPD_ARGS;
HTTPD_FUNC HTTPDServer_write HTTPD_ARGS;
HTTPD_FUNC HTTPDServer_serverinit HTTPD_ARGS;

static PyMethodDef SpamMethods[] = {
	{"server", HTTPDServer_init, METH_VARARGS,
	"HTTPServer::HTTPServer"},
	{"write", HTTPDServer_write, METH_VARARGS, "Linux Syscall Write"},
//	{"init", HTTPDServer_listen, METH_VARARGS},
	{"serverinit", HTTPDServer_serverinit, METH_VARARGS, "_"},
	{NULL, NULL, 0, NULL}
};

static PyObject* SpamError;

static PyModuleDef spammodule = {
	PyModuleDef_HEAD_INIT,
	"spam", 	// 		name
	NULL,  		// 		docs
	-1,     	// 		-1 if module keeps stat in global variables, or size of per-interpreter state
	SpamMethods 	// 		methods
};

HTTPD_FUNC HTTPDServer_init(PyObject* self, PyObject* args) {
	PyObject* callback;
	char* port;

	if(!PyArg_ParseTuple(args, "sO", &port, &callback))
		return NULL;

	// check if its callable
	if(!PyCallable_Check(callback)) {
		PyErr_SetString(PyExc_TypeError, "Need a callable object!");
	} else {
		server = new HTTPServer(port);
		server->pycallback = callback;
		server->listen(100);
	}
	Py_INCREF(Py_None);
	return Py_None;
}

HTTPD_FUNC HTTPDServer_write HTTPD_ARGS {
	int fd;
	char* msg;
	int len;
	if(!PyArg_ParseTuple(args, "Is#", &fd, &msg, &len)){
		return NULL;
	}

	write(fd, msg, len);
	return Py_None;
}

HTTPD_FUNC HTTPDServer_serverinit HTTPD_ARGS {
	int fd;
	char* msg;
	int len;
	if(!PyArg_ParseTuple(args, "Is#", &fd, &msg, &len)){
		return NULL;
	}
	server_init(fd, msg);
	Py_INCREF(Py_None);
	return Py_None;
}

PyMODINIT_FUNC PyInit_spam (void){
	PyObject* m;

	m = PyModule_Create(&spammodule);
	if (m == NULL)
		return NULL;

	SpamError = PyErr_NewException("spam.error", NULL, NULL);
	Py_XINCREF(SpamError);
	if (PyModule_AddObject(m, "error", SpamError) < 0) {
		Py_XDECREF(SpamError);
		Py_CLEAR(SpamError);
		Py_DECREF(m);
		return NULL;
	}

	return m;
}

