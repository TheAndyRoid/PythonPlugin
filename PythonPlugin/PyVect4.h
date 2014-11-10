/********************************************************************************
Copyright (C) 2014 Andrew Skinner <obs@theandyroid.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
********************************************************************************/

#pragma once


#include <Python.h>
#include "structmember.h"



/*Super basic Vect4 class*/

typedef struct {
	PyObject_HEAD
	PyObject *x;
	PyObject *y;
	PyObject *z;
	PyObject *w;
} PyVect4Object;


static void
PyVect4_dealloc(PyVect4Object* self)
{
	Py_XDECREF(self->y);
	Py_XDECREF(self->x);
	Py_XDECREF(self->z);
	Py_XDECREF(self->w);
	Py_TYPE(self)->tp_free((PyObject*)self);
}


static PyObject *
PyVect4_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	PyVect4Object *self;

	self = (PyVect4Object *)type->tp_alloc(type, 0);
	if (self != NULL) {
		self->x = PyFloat_FromDouble(0.0);
		if (self->x == NULL)
		{
			Py_DECREF(self);
			return NULL;
		}

		self->y = PyFloat_FromDouble(0.0);
		if (self->y == NULL)
		{
			Py_DECREF(self);
			return NULL;
		}
		self->z = PyFloat_FromDouble(0.0);
		if (self->z == NULL)
		{
			Py_DECREF(self);
			return NULL;
		}
		self->w= PyFloat_FromDouble(0.0);
		if (self->w == NULL)
		{
			Py_DECREF(self);
			return NULL;
		}


	}

	return (PyObject *)self;
}


static int
PyVect4_init(PyVect4Object *self, PyObject *args, PyObject *kwds)
{
	PyObject *x = NULL, *y = NULL, *z = NULL, *w = NULL, *tmp;

	static char *kwlist[] = { "x", "y","z","w", NULL };

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OOOO", kwlist, &x, &y, &z, &w))
		return -1;

	if (x) {
		tmp = self->x;
		Py_INCREF(x);
		self->x = x;
		Py_XDECREF(tmp);
	}

	if (y) {
		tmp = self->y;
		Py_INCREF(y);
		self->y = y;
		Py_XDECREF(tmp);
	}
	if (z) {
		tmp = self->z;
		Py_INCREF(z);
		self->z = z;
		Py_XDECREF(tmp);
	}
	if (w) {
		tmp = self->w;
		Py_INCREF(w);
		self->w = w;
		Py_XDECREF(tmp);
	}

	return 0;
}

static PyMemberDef PyVect4_members[] = {
		{ "x", T_OBJECT_EX, offsetof(PyVect4Object, x), 0, "x" },
		{ "y", T_OBJECT_EX, offsetof(PyVect4Object, y), 0, "y" },
		{ "z", T_OBJECT_EX, offsetof(PyVect4Object, z), 0, "z" },
		{ "w", T_OBJECT_EX, offsetof(PyVect4Object, w), 0, "w" },
		{ NULL }  /* Sentinel */
};

static PyMethodDef PyVect4_methods[] = {
		{ NULL }  /* Sentinel */
};

static PyTypeObject PyVect4_Object = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"OBS.Vect4",             /*tp_name*/
	sizeof(PyVect4Object),             /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)PyVect4_dealloc, /*tp_dealloc*/
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
	"Simple Vect4",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	PyVect4_methods,             /* tp_methods */
	PyVect4_members,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)PyVect4_init,      /* tp_init */
	0,                         /* tp_alloc */
	PyVect4_new,                 /* tp_new */
};

