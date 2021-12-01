/*

	Python extension of koliba library.

	kolibamodule.c

	Copyright 2021 G. Adam Stanislav
	All rights reserved

	Redistribution and use in source and binary forms,
	with or without modification, are permitted provided
	that the following conditions are met:

	1. Redistributions of source code must retain the
	above copyright notice, this list of conditions
	and the following disclaimer.

	2. Redistributions in binary form must reproduce the
	above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or
	other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the
	names of its contributors may be used to endorse or
	promote products derived from this software without
	specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
	AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
	FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
	SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
	OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
	STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"

#include "koliba.h"

#define DoubleConst(name,val)	PyDict_SetItemString(d, (const char *)name, o=PyFloat_FromDouble((double)val)); \
	Py_DECREF(o);

#define	KLBO	static PyObject *
#define klbo(n,o)	koliba##n##Object *o
#define klbnew(n)	static PyObject * koliba##n##New(PyTypeObject *type, PyObject *args, PyObject *kwds)
#define klbinit(n)	static int koliba##n##Init(koliba##n##Object *self, PyObject *args, PyObject *kwds)
#define	klbdealloc(n)	static void koliba##n##Dealloc(koliba##n##Object *self)

#define	klbgetset(n)	static PyGetSetDef koliba##n##GetSet[]


typedef struct {
	PyObject_HEAD
	KOLIBA_ANGLE a;
} kolibaAngleObject;

static const char * const kau[] = {
	"KAU_degrees",
	"KAU_radians",
	"KAU_turns",
	"KAU_pis"
};

static const char * const kqc[] = {
	"KQC_red",
	"KQC_scarlet",
	"KQC_vermilion",
	"KQC_persimmon",
	"KQC_orange",
	"KQC_orangepeel",
	"KQC_amber",
	"KQC_goldenyellow",
	"KQC_yellow",
	"KQC_lemon",
	"KQC_lime",
	"KQC_springbud",
	"KQC_chartreuse",
	"KQC_brightgreen",
	"KQC_harlequin",
	"KQC_neongreen",
	"KQC_green",
	"KQC_jade",
	"KQC_erin",
	"KQC_emerald",
	"KQC_springgreen",
	"KQC_mint",
	"KQC_aquamarine",
	"KQC_turquoise",
	"KQC_cyan",
	"KQC_skyblue",
	"KQC_capri",
	"KQC_cornflower",
	"KQC_azure",
	"KQC_cobalt",
	"KQC_cerulean",
	"KQC_sapphire",
	"KQC_blue",
	"KQC_iris",
	"KQC_indigo",
	"KQC_veronica",
	"KQC_violet",
	"KQC_amethyst",
	"KQC_purple",
	"KQC_phlox",
	"KQC_magenta",
	"KQC_fuchsia",
	"KQC_cerise",
	"KQC_deeppink",
	"KQC_rose",
	"KQC_raspberry",
	"KQC_crimson",
	"KQC_amaranth"
};

klbdealloc(Angle) {
	Py_TYPE(self)->tp_free((PyObject *)self);
}

klbnew(Angle) {
	kolibaAngleObject *self;
	self = (kolibaAngleObject *) type->tp_alloc(type, 0);
	self->a.angle = 0.0;
	self->a.units = KAU_degrees;
	return (PyObject *)self;
}

klbinit(Angle) {
	static char *kwlist[] = {"angle", "units", NULL};
	double angle = self->a.angle;
	unsigned int units = (unsigned int)self->a.units;
	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|di", kwlist, &angle, &units))
		return -1;
	else if (KOLIBA_AngleSet(&self->a, angle, units) == NULL) {
		PyErr_Format(PyExc_ValueError, "Units must be %s, %s, %s, or %s", kau[0], kau[1], kau[2], kau[3]);
		return -1;
	}
	return 0;
}

KLBO kolibaAngleGetDegrees(klbo(Angle,self), void *closure) {
	return PyFloat_FromDouble(KOLIBA_AngleDegrees(&self->a));
}

static int kolibaAngleSetDegrees(klbo(Angle,self), PyObject *value, void *closure) {
	if (PyFloat_Check(value)) self->a.angle = PyFloat_AsDouble(value);
	else if (PyLong_Check(value)) self->a.angle = (double)PyLong_AsLong(value);
	else {
		PyErr_SetString(PyExc_TypeError, "The angle must be a number in degrees");
		return -1;
	}
	self->a.units = KAU_degrees;
	return 0;
}

KLBO kolibaAngleGetRadians(klbo(Angle,self), void *closure) {
	return PyFloat_FromDouble(KOLIBA_AngleRadians(&self->a));
}

static int kolibaAngleSetRadians(klbo(Angle,self), PyObject *value, void *closure) {
	if (PyFloat_Check(value)) self->a.angle = PyFloat_AsDouble(value);
	else if (PyLong_Check(value)) self->a.angle = (double)PyLong_AsLong(value);
	else {
		PyErr_SetString(PyExc_TypeError, "The angle must be a number in radians");
		return -1;
	}
	self->a.units = KAU_radians;
	return 0;
}

KLBO kolibaAngleGetTurns(klbo(Angle,self), void *closure) {
	return PyFloat_FromDouble(KOLIBA_AngleTurns(&self->a));
}

static int kolibaAngleSetTurns(klbo(Angle,self), PyObject *value, void *closure) {
	if (PyFloat_Check(value)) self->a.angle = PyFloat_AsDouble(value);
	else if (PyLong_Check(value)) self->a.angle = (double)PyLong_AsLong(value);
	else {
		PyErr_SetString(PyExc_TypeError, "The angle must be a number in turns");
		return -1;
	}
	self->a.units = KAU_turns;
	return 0;
}

KLBO kolibaAngleGetPis(klbo(Angle,self), void *closure) {
	return PyFloat_FromDouble(KOLIBA_AnglePis(&self->a));
}

static int kolibaAngleSetPis(klbo(Angle,self), PyObject *value, void *closure) {
	if (PyFloat_Check(value)) self->a.angle = PyFloat_AsDouble(value);
	else if (PyLong_Check(value)) self->a.angle = (double)PyLong_AsLong(value);
	else {
		PyErr_SetString(PyExc_TypeError, "The angle must be a number in pis");
		return -1;
	}
	self->a.units = KAU_pis;
	return 0;
}

KLBO kolibaAngleSine(klbo(Angle,self), void *closure) {
	return PyFloat_FromDouble(KOLIBA_AngleSine(&self->a));
}

KLBO kolibaAngleCosine(klbo(Angle,self), void *closure) {
	return PyFloat_FromDouble(KOLIBA_AngleCosine(&self->a));
}

static PyMethodDef kolibaAngleMethods[] = {
	{"sin", (PyCFunction)kolibaAngleSine, METH_NOARGS, "Return the sine of the angle"},
	{"cos", (PyCFunction)kolibaAngleCosine, METH_NOARGS, "Return the cosine of the angle"},
	{NULL}
};

klbgetset(Angle) = {
	{"degrees", (getter)kolibaAngleGetDegrees, (setter)kolibaAngleSetDegrees, "angle in degrees", NULL},
	{"radians", (getter)kolibaAngleGetRadians, (setter)kolibaAngleSetRadians, "angle in radians", NULL},
	{"turns", (getter)kolibaAngleGetTurns, (setter)kolibaAngleSetTurns, "angle in turns", NULL},
	{"pis", (getter)kolibaAngleGetPis, (setter)kolibaAngleSetPis, "angle in pis", NULL},
	{NULL}
};

static PyTypeObject kolibaAngleType = {
	PyVarObject_HEAD_INIT(NULL,0)
	.tp_name = "koliba.Angle",
	.tp_doc  = "Angle objects",
	.tp_basicsize = sizeof(kolibaAngleObject),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_new = kolibaAngleNew,
	.tp_init = (initproc)kolibaAngleInit,
	.tp_dealloc = (destructor)kolibaAngleDealloc,
	.tp_methods = kolibaAngleMethods,
	.tp_getset = kolibaAngleGetSet,
};

KLBO koliba_Double_const_mul(PyObject *self, PyObject *args, double val) {
	double d = 1.0;

	if (!PyArg_ParseTuple(args, "|d", &d)) return NULL;
	return PyFloat_FromDouble(d*val);
}

KLBO koliba_Pi(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, KOLIBA_Pi);
}

KLBO koliba_invPi(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, KOLIBA_1DivPi);
}

KLBO koliba_Tau(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, KOLIBA_2Pi);
}

KLBO koliba_invTau(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, KOLIBA_1Div2Pi);
}

KLBO koliba_Pi2(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, KOLIBA_PiDiv2);
}

KLBO koliba_invPi2(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, 1.0/KOLIBA_PiDiv2);
}

KLBO koliba_Pi180(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, KOLIBA_PiDiv180);
}

KLBO koliba_invPi180(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, KOLIBA_180DivPi);
}

KLBO koliba_PDeg(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, KOLIBA_180);
}

KLBO koliba_invPDeg(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, KOLIBA_1Div180);
}

KLBO koliba_invTDeg(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, KOLIBA_1Div360);
}

KLBO koliba_TDeg(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, KOLIBA_360);
}

KLBO koliba_Kappa(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, KOLIBA_Kappa);
}

KLBO koliba_invKappa(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, KOLIBA_1DivKappa);
}

KLBO koliba_compKappa(PyObject *self, PyObject *args) {
		return koliba_Double_const_mul(self, args, KOLIBA_1MinKappa);
}

KLBO koliba_absKappa(PyObject *self, PyObject *args, PyObject *kwargs) {
	double start, radius;
	static char *kwlist[] = {"start", "radius", NULL};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "dd", kwlist, &start, &radius)) return NULL;
	return PyFloat_FromDouble(start+radius*KOLIBA_Kappa);
}

static PyMethodDef KolibaMethods[] = {
	{"Pi", koliba_Pi, METH_VARARGS, "Multiplies a value by pi."},
	{"DivPi", koliba_invPi, METH_VARARGS, "Divides a value by pi."},
	{"Tau", koliba_Tau, METH_VARARGS, "Multiplies a value by tau (2pi)."},
	{"DivTau", koliba_invTau, METH_VARARGS, "Divides a value by tau (2pi)."},
	{"HalfPi", koliba_Pi2, METH_VARARGS, "Multiplies a value by pi and divides by 2."},
	{"DivHalfPi", koliba_invPi2, METH_VARARGS, "Divides a value by pi and multiplies by 2."},
	{"DegreesToRadians", koliba_Pi180, METH_VARARGS, "Converts degrees to radians."},
	{"RadiansToDegrees", koliba_invPi180, METH_VARARGS, "Converts radians to degrees."},
	{"PisToDegrees", koliba_PDeg, METH_VARARGS, "Converts pis to degrees."},
	{"DegreesToPis", koliba_invPDeg, METH_VARARGS, "Converts degrees to pis."},
	{"DegreesToTurns", koliba_invTDeg, METH_VARARGS, "Converts turns to degrees."},
	{"TurnsToDegrees", koliba_TDeg, METH_VARARGS, "Converts degrees to turns."},
	{"TangentFromRadius", koliba_Kappa, METH_VARARGS, "Multiplies by 4(sqrt(2)-1)/3."},
	{"RadiusFromTangent", koliba_invKappa, METH_VARARGS, "Multiplies by 3/(4(sqrt(2)-1))."},
	{"TangentToRadius", koliba_compKappa, METH_VARARGS, "Multiplies by (1 - 4(sqrt(2)-1)/3)."},
	{"AbsoluteTangent", (PyCFunction)koliba_absKappa, METH_VARARGS | METH_KEYWORDS, "Returns start + 4 radius (sqrt(2)-1)/3."},
	{NULL, NULL, 0, NULL}
};

static struct PyModuleDef kolibamodule = {
	PyModuleDef_HEAD_INIT,
	"koliba",
	"Python implementation of libkoliba.",
	-1,
	KolibaMethods
};

PyMODINIT_FUNC
PyInit_koliba(void)
{
	PyObject *m, *d, *o;

	if (PyType_Ready(&kolibaAngleType) < 0) return NULL;
	if ((m = PyModule_Create(&kolibamodule)) == NULL) return NULL;
	Py_INCREF(&kolibaAngleType);
	if (PyModule_AddObject(m, "Angle", (PyObject *)&kolibaAngleType) < 0) {
		Py_DECREF(&kolibaAngleType);
		Py_DECREF(m);
		return NULL;
	}
	if ((d = PyModule_GetDict(m))) {
		DoubleConst("pi", KOLIBA_Pi);
		DoubleConst("invpi", KOLIBA_1DivPi);
		DoubleConst("tau", KOLIBA_2Pi);
		DoubleConst("invtau", KOLIBA_1Div2Pi);
		DoubleConst("rad", KOLIBA_PiDiv180);
		DoubleConst("invrad", KOLIBA_180DivPi);

		DoubleConst((char *)&kqc[KQC_red][4], (double)KQC_red*7.5);
		DoubleConst((char *)&kqc[KQC_scarlet][4], (double)KQC_scarlet*7.5);
		DoubleConst((char *)&kqc[KQC_vermilion][4], (double)KQC_vermilion*7.5);
		DoubleConst((char *)&kqc[KQC_persimmon][4], (double)KQC_persimmon*7.5);
		DoubleConst((char *)&kqc[KQC_orange][4], (double)KQC_orange*7.5);
		DoubleConst((char *)&kqc[KQC_orangepeel][4], (double)KQC_orangepeel*7.5);
		DoubleConst((char *)&kqc[KQC_amber][4], (double)KQC_amber*7.5);
		DoubleConst((char *)&kqc[KQC_goldenyellow][4], (double)KQC_goldenyellow*7.5);
		DoubleConst((char *)&kqc[KQC_yellow][4], (double)KQC_yellow*7.5);
		DoubleConst((char *)&kqc[KQC_lemon][4], (double)KQC_lemon*7.5);
		DoubleConst((char *)&kqc[KQC_lime][4], (double)KQC_lime*7.5);
		DoubleConst((char *)&kqc[KQC_springbud][4], (double)KQC_springbud*7.5);
		DoubleConst((char *)&kqc[KQC_chartreuse][4], (double)KQC_chartreuse*7.5);
		DoubleConst((char *)&kqc[KQC_brightgreen][4], (double)KQC_brightgreen*7.5);
		DoubleConst((char *)&kqc[KQC_harlequin][4], (double)KQC_harlequin*7.5);
		DoubleConst((char *)&kqc[KQC_neongreen][4], (double)KQC_neongreen*7.5);
		DoubleConst((char *)&kqc[KQC_green][4], (double)KQC_green*7.5);
		DoubleConst((char *)&kqc[KQC_jade][4], (double)KQC_jade*7.5);
		DoubleConst((char *)&kqc[KQC_erin][4], (double)KQC_erin*7.5);
		DoubleConst((char *)&kqc[KQC_emerald][4], (double)KQC_emerald*7.5);
		DoubleConst((char *)&kqc[KQC_springgreen][4], (double)KQC_springgreen*7.5);
		DoubleConst((char *)&kqc[KQC_mint][4], (double)KQC_mint*7.5);
		DoubleConst((char *)&kqc[KQC_aquamarine][4], (double)KQC_aquamarine*7.5);
		DoubleConst((char *)&kqc[KQC_turquoise][4], (double)KQC_turquoise*7.5);
		DoubleConst((char *)&kqc[KQC_cyan][4], (double)KQC_cyan*7.5);
		DoubleConst((char *)&kqc[KQC_skyblue][4], (double)KQC_skyblue*7.5);
		DoubleConst((char *)&kqc[KQC_capri][4], (double)KQC_capri*7.5);
		DoubleConst((char *)&kqc[KQC_cornflower][4], (double)KQC_cornflower*7.5);
		DoubleConst((char *)&kqc[KQC_azure][4], (double)KQC_azure*7.5);
		DoubleConst((char *)&kqc[KQC_cobalt][4], (double)KQC_cobalt*7.5);
		DoubleConst((char *)&kqc[KQC_cerulean][4], (double)KQC_cerulean*7.5);
		DoubleConst((char *)&kqc[KQC_sapphire][4], (double)KQC_sapphire*7.5);
		DoubleConst((char *)&kqc[KQC_blue][4], (double)KQC_blue*7.5);
		DoubleConst((char *)&kqc[KQC_iris][4], (double)KQC_iris*7.5);
		DoubleConst((char *)&kqc[KQC_indigo][4], (double)KQC_indigo*7.5);
		DoubleConst((char *)&kqc[KQC_veronica][4], (double)KQC_veronica*7.5);
		DoubleConst((char *)&kqc[KQC_violet][4], (double)KQC_violet*7.5);
		DoubleConst((char *)&kqc[KQC_amethyst][4], (double)KQC_amethyst*7.5);
		DoubleConst((char *)&kqc[KQC_purple][4], (double)KQC_purple*7.5);
		DoubleConst((char *)&kqc[KQC_phlox][4], (double)KQC_phlox*7.5);
		DoubleConst((char *)&kqc[KQC_magenta][4], (double)KQC_magenta*7.5);
		DoubleConst((char *)&kqc[KQC_fuchsia][4], (double)KQC_fuchsia*7.5);
		DoubleConst((char *)&kqc[KQC_cerise][4], (double)KQC_cerise*7.5);
		DoubleConst((char *)&kqc[KQC_deeppink][4], (double)KQC_deeppink*7.5);
		DoubleConst((char *)&kqc[KQC_rose][4], (double)KQC_rose*7.5);
		DoubleConst((char *)&kqc[KQC_raspberry][4], (double)KQC_raspberry*7.5);
		DoubleConst((char *)&kqc[KQC_crimson][4], (double)KQC_crimson*7.5);
		DoubleConst((char *)&kqc[KQC_amaranth][4], (double)KQC_amaranth*7.5);

		DoubleConst("kappa", KOLIBA_Kappa);
		DoubleConst("invkappa", KOLIBA_1DivKappa);
		DoubleConst("compkappa", KOLIBA_1MinKappa);
	}

	PyModule_AddIntConstant(m, (char *)kau[KAU_degrees], (long)KAU_degrees);
	PyModule_AddIntConstant(m, (char *)kau[KAU_radians], (long)KAU_radians);
	PyModule_AddIntConstant(m, (char *)kau[KAU_turns], (long)KAU_turns);
	PyModule_AddIntConstant(m, (char *)kau[KAU_pis], (long)KAU_pis);

	PyModule_AddIntConstant(m, (char *)kqc[KQC_red], (long)KQC_red);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_scarlet], (long)KQC_scarlet);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_vermilion], (long)KQC_vermilion);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_persimmon], (long)KQC_persimmon);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_orange], (long)KQC_orange);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_orangepeel], (long)KQC_orangepeel);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_amber], (long)KQC_amber);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_goldenyellow], (long)KQC_goldenyellow);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_yellow], (long)KQC_yellow);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_lemon], (long)KQC_lemon);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_lime], (long)KQC_lime);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_springbud], (long)KQC_springbud);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_chartreuse], (long)KQC_chartreuse);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_brightgreen], (long)KQC_brightgreen);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_harlequin], (long)KQC_harlequin);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_neongreen], (long)KQC_neongreen);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_green], (long)KQC_green);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_jade], (long)KQC_jade);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_erin], (long)KQC_erin);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_emerald], (long)KQC_emerald);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_springgreen], (long)KQC_springgreen);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_mint], (long)KQC_mint);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_aquamarine], (long)KQC_aquamarine);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_turquoise], (long)KQC_turquoise);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_cyan], (long)KQC_cyan);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_skyblue], (long)KQC_skyblue);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_capri], (long)KQC_capri);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_cornflower], (long)KQC_cornflower);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_azure], (long)KQC_azure);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_cobalt], (long)KQC_cobalt);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_cerulean], (long)KQC_cerulean);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_sapphire], (long)KQC_sapphire);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_blue], (long)KQC_blue);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_iris], (long)KQC_iris);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_indigo], (long)KQC_indigo);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_veronica], (long)KQC_veronica);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_violet], (long)KQC_violet);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_amethyst], (long)KQC_amethyst);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_purple], (long)KQC_purple);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_phlox], (long)KQC_phlox);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_magenta], (long)KQC_magenta);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_fuchsia], (long)KQC_fuchsia);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_cerise], (long)KQC_cerise);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_deeppink], (long)KQC_deeppink);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_rose], (long)KQC_rose);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_raspberry], (long)KQC_raspberry);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_crimson], (long)KQC_crimson);
	PyModule_AddIntConstant(m, (char *)kqc[KQC_amaranth], (long)KQC_amaranth);
	return m;
}
