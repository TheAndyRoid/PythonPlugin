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



/*Super basic vect2 class*/

typedef struct {
	PyObject_HEAD
	PyObject *x;
	PyObject *y;
} PyVect2Object;


static void
PyVect2_dealloc(PyVect2Object* self)
{
	Py_XDECREF(self->y);
	Py_XDECREF(self->x);
	Py_TYPE(self)->tp_free((PyObject*)self);
}


static PyObject *
PyVect2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	PyVect2Object *self;

	self = (PyVect2Object *)type->tp_alloc(type, 0);
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

		
	}

	return (PyObject *)self;
}


static int
PyVect2_init(PyVect2Object *self, PyObject *args, PyObject *kwds)
{
	PyObject *x = NULL, *y = NULL, *tmp;

	static char *kwlist[] = { "x", "y", NULL };

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OO", kwlist, &x, &y))
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

	return 0;
}

static PyMemberDef PyVect2_members[] = {
		{ "x", T_OBJECT_EX, offsetof(PyVect2Object, x), 0,"x" },
		{ "y", T_OBJECT_EX, offsetof(PyVect2Object, y), 0,"y" },
		{ NULL }  /* Sentinel */
};

static PyMethodDef PyVect2_methods[] = {
		{ NULL }  /* Sentinel */
};

static PyTypeObject PyVect2_Object = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"OBS.Vect2",             /*tp_name*/
	sizeof(PyVect2Object),             /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)PyVect2_dealloc, /*tp_dealloc*/
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
	"Simple Vect2",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	PyVect2_methods,             /* tp_methods */
	PyVect2_members,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)PyVect2_init,      /* tp_init */
	0,                         /* tp_alloc */
	PyVect2_new,                 /* tp_new */
};

