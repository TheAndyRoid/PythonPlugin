#pragma once


#include <Python.h>
#include "structmember.h"
#include "OBSApi.h"

/*
Contains Configuration Objects and extensions for obs

*/


/*Object data*/
typedef struct {
	PyObject_HEAD
		/* Type-specific fields go here. */
		XElement *node; // This is the node that the python object can manipulate
} pyConfig;


/*Python Methods*/

static void
pyConfig_dealloc(pyConfig* self)
{
	self->ob_type->tp_free((PyObject*)self);
}

/*Parse arguments*/
static PyObject * pyConfig_new(PyTypeObject *type, PyObject *args, PyObject *kwds){
	pyConfig *self;
	self = (pyConfig*)type->tp_alloc(type, 0);
	if (self != NULL) {
		/*Create c++ imagesource*/
	}
	return (PyObject*)self;
}



/*Construct*/
static int
pyConfig_init(pyConfig *self, PyObject *args, PyObject *kwds)
{
	/**/

	return 0;
}


/*Method Table*/
static PyMethodDef pyConfig_methods[] = {
		{ NULL }  /* Sentinel */
};

/*Member table*/
static PyMemberDef pyConfig_members[] = {
		{ NULL }  /* Sentinel */
};


/*Python Type Object */
static PyTypeObject pyConfigType = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"OBS.Config",         /*tp_name*/
	sizeof(pyConfig),     /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)pyConfig_dealloc, /*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,                         /*tp_compare*/
	0,                         /*tp_repr*/
	0,                         /*tp_as_number*/
	0,                         /*tp_as_sequence*/
	0,                         /*tp_as_mapping*/
	0,                         /*tp_hash */
	0,                         /*tp_call*/
	0,                         /*tp_str*/
	0,                         /*tp_getattro*/
	0,                         /*tp_setattro*/
	0,                         /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
	"OBS Config Element",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	pyConfig_methods,             /* tp_methods */
	pyConfig_members,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)pyConfig_init,      /* tp_init */
	0,                         /* tp_alloc */
	pyConfig_new,                 /* tp_new */
};