 /*

	Python extension of koliba library.

	kolibamodule.c

	Copyright 2021-2022 G. Adam Stanislav
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

/* Use a static version of libkoliba under Windows. */
#ifdef _WIN32
#define NOKLIBLIB
#endif
#include "koliba.h"

#define DoubleConst(name,val)	PyDict_SetItemString(d, (const char *)name, o=PyFloat_FromDouble((double)val)); \
	Py_DECREF(o);

#define	KLBO	static PyObject *
#define klbo(n,o)	koliba##n##Object *o
#define klbnew(n)	static PyObject * koliba##n##New(PyTypeObject *type, PyObject *args, PyObject *kwds)
#define klbinit(n)	static int koliba##n##Init(koliba##n##Object *self, PyObject *args, PyObject *kwds)
#define	klbdealloc(n)	static void koliba##n##Dealloc(koliba##n##Object *self)
#define isklbtype(n,o)	PyObject_TypeCheck(o,&koliba##n##Type)

#define	klbgetset(n)	static PyGetSetDef koliba##n##GetSet[]


typedef struct {
	PyObject_HEAD
	KOLIBA_ANGLE a;
} kolibaAngleObject;

typedef struct {
	kolibaAngleObject;
	double radius;
} kolibaArcObject;

typedef struct {
	kolibaArcObject;
	double midpoint;
	double exponent;	// This is "private"
	double t;
	int    frames;
	int    frame;
	bool   monocycle;
} kolibaFrangleObject;

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

static PyTypeObject kolibaAngleType;
static PyTypeObject kolibaArcType;
static PyTypeObject kolibaFrangleType;


static int koliba_frangle_noset(char *property) {
	PyErr_Format(PyExc_ValueError, "Frangle.%s is a read-only property (try setting the frame or frames instead))", property);
	return -1;
}


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
	if (isklbtype(Frangle,self)) return koliba_frangle_noset("degrees");
	if (PyFloat_Check(value)) self->a.angle = PyFloat_AsDouble(value);
	else if (PyLong_Check(value)) self->a.angle = (double)PyLong_AsDouble(value);
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
	if (isklbtype(Frangle,self)) return koliba_frangle_noset("radians");
	if (PyFloat_Check(value)) self->a.angle = PyFloat_AsDouble(value);
	else if (PyLong_Check(value)) self->a.angle = (double)PyLong_AsDouble(value);
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
	if (isklbtype(Frangle,self)) return koliba_frangle_noset("turns");
	if (PyFloat_Check(value)) self->a.angle = PyFloat_AsDouble(value);
	else if (PyLong_Check(value)) self->a.angle = (double)PyLong_AsDouble(value);
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
	if (isklbtype(Frangle,self)) return koliba_frangle_noset("pis");
	if (PyFloat_Check(value)) self->a.angle = PyFloat_AsDouble(value);
	else if (PyLong_Check(value)) self->a.angle = (double)PyLong_AsDouble(value);
	else {
		PyErr_SetString(PyExc_TypeError, "The angle must be a number in pis");
		return -1;
	}
	self->a.units = KAU_pis;
	return 0;
}

KLBO kolibaAngleSine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AngleSine(&self->a));
}

KLBO kolibaAngleCosine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AngleCosine(&self->a));
}

KLBO kolibaAngleFactorSine(klbo(Angle,self), PyObject *args) {
	double factor;

	if (!PyArg_ParseTuple(args, "d", &factor)) return NULL;

	return PyFloat_FromDouble(KOLIBA_AngleFactorSine(&self->a, factor));
}

KLBO kolibaAngleFactorCosine(klbo(Angle,self), PyObject *args) {
	double factor;

	if (!PyArg_ParseTuple(args, "d", &factor)) return NULL;

	return PyFloat_FromDouble(KOLIBA_AngleFactorCosine(&self->a, factor));
}

KLBO kolibaAngleMonocycleSine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AngleMonocycleSine(&self->a));
}

KLBO kolibaAngleMonocycleCosine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AngleMonocycleCosine(&self->a));
}

KLBO kolibaAngleMonocyclical(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_MonocyclicalAngle(&self->a));
}

KLBO kolibaAngleVersine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AngleVersine(&self->a));
}

KLBO kolibaAngleHaversine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AngleHaversine(&self->a));
}

KLBO kolibaAnglePolsine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AnglePolsine(&self->a));
}

KLBO kolibaAngleVercosine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AngleVercosine(&self->a));
}

KLBO kolibaAngleHavercosine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AngleHavercosine(&self->a));
}

KLBO kolibaAnglePolcosine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AnglePolcosine(&self->a));
}

KLBO kolibaAngleFactorVersine(klbo(Angle,self), PyObject *args) {
	double factor;

	if (!PyArg_ParseTuple(args, "d", &factor)) return NULL;
	return PyFloat_FromDouble(KOLIBA_AngleFactorVersine((&self->a), factor));
}

KLBO kolibaAngleFactorHaversine(klbo(Angle,self), PyObject *args) {
	double factor;

	if (!PyArg_ParseTuple(args, "d", &factor)) return NULL;
	return PyFloat_FromDouble(KOLIBA_AngleFactorHaversine((&self->a), factor));
}

KLBO kolibaAngleFactorVercosine(klbo(Angle,self), PyObject *args) {
	double factor;

	if (!PyArg_ParseTuple(args, "d", &factor)) return NULL;
	return PyFloat_FromDouble(KOLIBA_AngleFactorVercosine((&self->a), factor));
}

KLBO kolibaAngleFactorHavercosine(klbo(Angle,self), PyObject *args) {
	double factor;

	if (!PyArg_ParseTuple(args, "d", &factor)) return NULL;
	return PyFloat_FromDouble(KOLIBA_AngleFactorHavercosine((&self->a), factor));
}

KLBO kolibaAngleMonocycleVersine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AngleMonocycleVersine(&self->a));
}

KLBO kolibaAngleMonocycleHaversine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AngleMonocycleHaversine(&self->a));
}

KLBO kolibaAngleMonocyclePolsine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AngleMonocyclePolsine(&self->a));
}

KLBO kolibaAngleMonocycleVercosine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AngleMonocycleVercosine(&self->a));
}

KLBO kolibaAngleMonocycleHavercosine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AngleMonocycleHavercosine(&self->a));
}

KLBO kolibaAngleMonocyclePolcosine(klbo(Angle,self)) {
	return PyFloat_FromDouble(KOLIBA_AngleMonocyclePolcosine(&self->a));
}

KLBO kolibaAngleFactorMonocycleVersine(klbo(Angle,self), PyObject *args) {
	double factor;

	if (!PyArg_ParseTuple(args, "d", &factor)) return NULL;
	return PyFloat_FromDouble(KOLIBA_AngleFactorMonocycleVersine((&self->a), factor));
}

KLBO kolibaAngleFactorMonocycleHaversine(klbo(Angle,self), PyObject *args) {
	double factor;

	if (!PyArg_ParseTuple(args, "d", &factor)) return NULL;
	return PyFloat_FromDouble(KOLIBA_AngleFactorHaversine((&self->a), factor));
}

KLBO kolibaAngleFactorMonocycleVercosine(klbo(Angle,self), PyObject *args) {
	double factor;

	if (!PyArg_ParseTuple(args, "d", &factor)) return NULL;
	return PyFloat_FromDouble(KOLIBA_AngleFactorMonocycleVercosine((&self->a), factor));
}

KLBO kolibaAngleFactorMonocycleHavercosine(klbo(Angle,self), PyObject *args) {
	double factor;

	if (!PyArg_ParseTuple(args, "d", &factor)) return NULL;
	return PyFloat_FromDouble(KOLIBA_AngleFactorMonocycleHavercosine((&self->a), factor));
}

KLBO kolibaAngleNormalize(klbo(Angle,self), PyObject *args) {
	if (!isklbtype(Frangle, self)) {
		int mono = 0;

		if (!PyArg_ParseTuple(args, "|p", &mono)) return NULL;
		if (mono) {
			if (KOLIBA_AngleMonocycle(&self->a) == NULL) return NULL;
		}
		else {
			if (KOLIBA_AngleNormalize(&self->a) == NULL) return NULL;
		}
	}
	Py_RETURN_NONE;
}

KLBO kolibaAngleAdd(PyObject *augmend, PyObject *addend) {
	if ((isklbtype(Angle, augmend)) && (!isklbtype(Frangle, augmend)) && (isklbtype(Angle, addend))) {
		PyObject *sum = kolibaAngleNew((PyTypeObject*)PyObject_Type(augmend), NULL, NULL);
		KOLIBA_AngleAdd(&((kolibaAngleObject *)sum)->a, &((kolibaAngleObject *)augmend)->a, &((kolibaAngleObject *)addend)->a);
		if (isklbtype(Arc, augmend)) ((kolibaArcObject *)sum)->radius = ((kolibaArcObject *)augmend)->radius;
		return sum;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

KLBO kolibaAngleInPlaceAdd(PyObject *augmend, PyObject *addend) {
	if ((isklbtype(Angle, augmend)) && (!isklbtype(Frangle, augmend)) && (isklbtype(Angle, addend))) {
		Py_INCREF(augmend);
		KOLIBA_AngleAdd(&((kolibaAngleObject *)augmend)->a, &((kolibaAngleObject *)augmend)->a, &((kolibaAngleObject *)addend)->a);
		return augmend;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

KLBO kolibaAngleSubtract(PyObject *minuend, PyObject *subtrahend) {
	if ((isklbtype(Angle, minuend)) && (!isklbtype(Frangle, minuend)) && (isklbtype(Angle, subtrahend))) {
		PyObject *difference = kolibaAngleNew((PyTypeObject*)PyObject_Type(minuend), NULL, NULL);
		KOLIBA_AngleSubtract(&((kolibaAngleObject *)difference)->a, &((kolibaAngleObject *)minuend)->a, &((kolibaAngleObject *)subtrahend)->a);
		if (isklbtype(Arc, minuend)) ((kolibaArcObject *)difference)->radius = ((kolibaArcObject *)minuend)->radius;
		return difference;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

KLBO kolibaAngleInPlaceSubtract(PyObject *minuend, PyObject *subtrahend) {
	if ((isklbtype(Angle, minuend)) && (!isklbtype(Frangle, minuend)) && (isklbtype(Angle, subtrahend))) {
		Py_INCREF(minuend);
		KOLIBA_AngleSubtract(&((kolibaAngleObject *)minuend)->a, &((kolibaAngleObject *)minuend)->a, &((kolibaAngleObject *)subtrahend)->a);
		return minuend;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

KLBO koliba_angle_multiply(kolibaAngleObject *multiplicand, PyObject *multiplier) {
	double factor;
	PyObject *result;
	if (isklbtype(Frangle, multiplicand)) {
		Py_RETURN_NOTIMPLEMENTED;
	}
	if (PyFloat_Check(multiplier)) factor = PyFloat_AsDouble(multiplier);
	else if (PyLong_Check(multiplier)) factor = PyLong_AsDouble(multiplier);
	else {
		Py_RETURN_NOTIMPLEMENTED;
	}
	result = kolibaAngleNew((PyTypeObject*)PyObject_Type((PyObject*)multiplicand), NULL, NULL);
	KOLIBA_AngleMultiply(&((kolibaAngleObject *)result)->a, &multiplicand->a, factor);
	if (isklbtype(Arc, result)) ((kolibaArcObject *)result)->radius = ((kolibaArcObject *)multiplicand)->radius;
	return result;
}

KLBO kolibaAngleMultiply(PyObject *multiplicand, PyObject *multiplier) {
		if (isklbtype(Angle, multiplicand)) 
			return koliba_angle_multiply((kolibaAngleObject *)multiplicand, multiplier);
		else if (isklbtype(Angle, multiplier))
			return koliba_angle_multiply((kolibaAngleObject *)multiplier, multiplicand);
	Py_RETURN_NOTIMPLEMENTED;
}

KLBO kolibaAngleInPlaceMultiply(PyObject *multiplicand, PyObject *multiplier) {
	double factor;

	if ((!isklbtype(Frangle, multiplicand)) && isklbtype(Angle, multiplicand)) {
		if (PyFloat_Check(multiplier)) factor = PyFloat_AsDouble(multiplier);
		else if (PyLong_Check(multiplier)) factor = PyLong_AsDouble(multiplier);
		else {
			Py_RETURN_NOTIMPLEMENTED;
		}
		Py_INCREF(multiplicand);
		KOLIBA_AngleMultiply(&((kolibaAngleObject *)multiplicand)->a, &((kolibaAngleObject *)multiplicand)->a, factor);
		return multiplicand;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

KLBO koliba_angle_power(PyObject *base, double exponent, PyObject *normalize, bool inplace) {
	PyObject *power;
	bool normal;

	if ((!isklbtype(Frangle, base)) && isklbtype(Angle, base)) {
		if (normalize == Py_None) normal = true;
		else if (PyBool_Check(normalize)) normal = (normalize == Py_True);
		else if (PyLong_Check(normalize)) normal = (PyLong_AsLong(normalize) != 0);
		else if (PyFloat_Check(normalize)) normal = (PyFloat_AsDouble(normalize) != 0.0);
		else {Py_RETURN_NOTIMPLEMENTED;}
		power = (inplace) ? base : kolibaAngleNew((PyTypeObject*)PyObject_Type((PyObject*)base), NULL, NULL);
		KOLIBA_AnglePower(&((kolibaAngleObject *)power)->a, &((kolibaAngleObject *)base)->a, exponent, normal);
		if (inplace) {
			Py_INCREF(power);
		}
		else {
			if (isklbtype(Arc, power)) ((kolibaArcObject *)power)->radius = ((kolibaArcObject *)base)->radius;
		}
		return power;
	}
	Py_RETURN_NOTIMPLEMENTED;
}

KLBO kolibaAnglePower(PyObject *base, PyObject *exponent, PyObject *normalize) {
	if (PyFloat_Check(exponent))
		return koliba_angle_power(base, PyFloat_AsDouble(exponent), normalize, false);
	else if (PyLong_Check(exponent))
		return koliba_angle_power(base, PyLong_AsDouble(exponent), normalize, false);
	else {
		Py_RETURN_NOTIMPLEMENTED;
	}
}

KLBO kolibaInPlaceAnglePower(PyObject *base, PyObject *exponent, PyObject *normalize) {
	if (PyFloat_Check(exponent))
		return koliba_angle_power(base, PyFloat_AsDouble(exponent), normalize, true);
	else if (PyLong_Check(exponent))
		return koliba_angle_power(base, PyLong_AsDouble(exponent), normalize, true);
	else {
		Py_RETURN_NOTIMPLEMENTED;
	}
}

KLBO koliba_angle_divide(PyObject *dividend, double divisor, bool floored, bool inplace) {
	PyObject *result;
	if (!isklbtype(Frangle, dividend)) {
		if (divisor == 0.0) {
			PyErr_Format(PyExc_ValueError, "The divisor must not be zero");
			return NULL;
		}
		if (isklbtype(Angle, dividend)) {
			result = (inplace) ? dividend : kolibaAngleNew((PyTypeObject*)PyObject_Type((PyObject*)dividend), NULL, NULL);
			KOLIBA_AngleDivide(&((kolibaAngleObject *)result)->a, &((kolibaAngleObject *)dividend)->a, divisor, floored);
			if (inplace) {
				Py_INCREF(dividend);
			}
			else {
				if (isklbtype(Arc, result)) ((kolibaArcObject *)result)->radius = ((kolibaArcObject *)dividend)->radius;
			}
			return result;
		}
	}
	Py_RETURN_NOTIMPLEMENTED;
}

KLBO kolibaAngleTrueDivide(PyObject *dividend, PyObject *divisor) {
	if (PyFloat_Check(divisor))
		return koliba_angle_divide(dividend, PyFloat_AsDouble(divisor), false, false);
	else if (PyLong_Check(divisor))
		return koliba_angle_divide(dividend, PyLong_AsDouble(divisor), false, false);
	else {
		Py_RETURN_NOTIMPLEMENTED;
	}
}

KLBO kolibaAngleInPlaceTrueDivide(PyObject *dividend, PyObject *divisor) {
	if (PyFloat_Check(divisor))
		return koliba_angle_divide(dividend, PyFloat_AsDouble(divisor), false, true);
	else if (PyLong_Check(divisor))
		return koliba_angle_divide(dividend, PyLong_AsDouble(divisor), false, true);
	else {
		Py_RETURN_NOTIMPLEMENTED;
	}
}

KLBO kolibaAngleFloorDivide(PyObject *dividend, PyObject *divisor) {
	if (PyFloat_Check(divisor))
		return koliba_angle_divide(dividend, PyFloat_AsDouble(divisor), true, false);
	else if (PyLong_Check(divisor))
		return koliba_angle_divide(dividend, PyLong_AsDouble(divisor), true, false);
	else {
		Py_RETURN_NOTIMPLEMENTED;
	}
}

KLBO kolibaAngleInPlaceFloorDivide(PyObject *dividend, PyObject *divisor) {
	if (PyFloat_Check(divisor))
		return koliba_angle_divide(dividend, PyFloat_AsDouble(divisor), true, true);
	else if (PyLong_Check(divisor))
		return koliba_angle_divide(dividend, PyLong_AsDouble(divisor), true, true);
	else {
		Py_RETURN_NOTIMPLEMENTED;
	}
}

KLBO kolibaAngleRichCompare(PyObject *self, PyObject *cosi, int op) {
	if (isklbtype(Angle, cosi)) {
		Py_RETURN_RICHCOMPARE(KOLIBA_AngleDegrees(&((kolibaAngleObject *)self)->a), KOLIBA_AngleDegrees(&((kolibaAngleObject *)cosi)->a), op);
	}
	Py_RETURN_NOTIMPLEMENTED;
}

static PyNumberMethods kolibaAngleAsNumber = {
	.nb_add = kolibaAngleAdd,
	.nb_inplace_add = kolibaAngleInPlaceAdd,
	.nb_subtract = kolibaAngleSubtract,
	.nb_inplace_subtract = kolibaAngleInPlaceSubtract,
	.nb_multiply = kolibaAngleMultiply,
	.nb_inplace_multiply = kolibaAngleInPlaceMultiply,
	.nb_true_divide = kolibaAngleTrueDivide,
	.nb_inplace_true_divide = kolibaAngleInPlaceTrueDivide,
	.nb_floor_divide = kolibaAngleFloorDivide,
	.nb_inplace_floor_divide = kolibaAngleInPlaceFloorDivide,
	.nb_power = kolibaAnglePower,
};

static PyMethodDef kolibaAngleMethods[] = {
	{"sin", (PyCFunction)kolibaAngleSine, METH_NOARGS, "Return the sine of the angle"},
	{"cos", (PyCFunction)kolibaAngleCosine, METH_NOARGS, "Return the cosine of the angle"},
	{"fsin", (PyCFunction)kolibaAngleFactorSine, METH_VARARGS, "Return the sine of the factor times the angle"},
	{"fcos", (PyCFunction)kolibaAngleFactorCosine, METH_VARARGS, "Return the cosine of the factor times the angle"},
	{"monosin", (PyCFunction)kolibaAngleMonocycleSine, METH_NOARGS, "Return the monocyclic sine of the angle"},
	{"monocos", (PyCFunction)kolibaAngleMonocycleCosine, METH_NOARGS, "Return the monocyclic cosine of the angle"},
	{"monocycle", (PyCFunction)kolibaAngleMonocyclical, METH_NOARGS, "Return the monocycle of the angle"},
	{"versin", (PyCFunction)kolibaAngleVersine, METH_NOARGS, "Return the versine of the angle"},
	{"haversin", (PyCFunction)kolibaAngleHaversine, METH_NOARGS, "Return the haversine of the angle"},
	{"polsin", (PyCFunction)kolibaAnglePolsine, METH_NOARGS, "Return the polsine of the angle"},
	{"vercos", (PyCFunction)kolibaAngleVercosine, METH_NOARGS, "Return the vercosine of the angle"},
	{"havercos", (PyCFunction)kolibaAngleHavercosine, METH_NOARGS, "Return the havercosine of the angle"},
	{"polcos", (PyCFunction)kolibaAnglePolcosine, METH_NOARGS, "Return the polcosine of the angle"},
	{"fversin", (PyCFunction)kolibaAngleFactorVersine, METH_VARARGS, "Return the versine of a factor times the angle"},
	{"fhaversin", (PyCFunction)kolibaAngleFactorHaversine, METH_VARARGS, "Return the haversine of a factor times the angle"},
	{"fvercos", (PyCFunction)kolibaAngleFactorVercosine, METH_VARARGS, "Return the vercosine of a factor times the angle"},
	{"fhavercos", (PyCFunction)kolibaAngleFactorHavercosine, METH_VARARGS, "Return the havercosine of a factor times the angle"},
	{"monoversin", (PyCFunction)kolibaAngleMonocycleVersine, METH_NOARGS, "Return the monocyclic versine of the angle"},
	{"monohaversin", (PyCFunction)kolibaAngleMonocycleHaversine, METH_NOARGS, "Return the monocyclic haversine of the angle"},
	{"monopolsin", (PyCFunction)kolibaAngleMonocyclePolsine, METH_NOARGS, "Return the monocyclic polsine of the angle"},
	{"monovercos", (PyCFunction)kolibaAngleMonocycleVercosine, METH_NOARGS, "Return the monocyclic vercosine of the angle"},
	{"monohavercos", (PyCFunction)kolibaAngleMonocycleHavercosine, METH_NOARGS, "Return the monocyclic havercosine of the angle"},
	{"monopolcos", (PyCFunction)kolibaAngleMonocyclePolcosine, METH_NOARGS, "Return the monocyclic polcosine of the angle"},
	{"fmonoversin", (PyCFunction)kolibaAngleFactorMonocycleVersine, METH_VARARGS, "Return the monocyclic versine of a factor times the angle"},
	{"fmonohaversin", (PyCFunction)kolibaAngleFactorMonocycleHaversine, METH_VARARGS, "Return the monocyclic haversine of a factor times the angle"},
	{"fmonovercos", (PyCFunction)kolibaAngleFactorMonocycleVercosine, METH_VARARGS, "Return the monocyclic vercosine of a factor times the angle"},
	{"fmonohavercos", (PyCFunction)kolibaAngleFactorMonocycleHavercosine, METH_VARARGS, "Return the monocyclic havercosine of a factor times the angle"},
	{"normalize", (PyCFunction)kolibaAngleNormalize, METH_VARARGS, "Normalize the angle to the first turn"},
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
	.tp_as_number = &kolibaAngleAsNumber,
	.tp_richcompare = kolibaAngleRichCompare,
};

klbinit(Arc) {
	static char *kwlist[] = {"angle", "units", "radius", NULL};
	double angle = self->a.angle;
	double radius = 1.0;
	unsigned int units = (unsigned int)self->a.units;
	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|did", kwlist, &angle, &units, &radius))
		return -1;
	else if (KOLIBA_AngleSet(&self->a, angle, units) == NULL) {
		PyErr_Format(PyExc_ValueError, "Units must be %s, %s, %s, or %s", kau[0], kau[1], kau[2], kau[3]);
		return -1;
	}
	else if (radius <= 0.0) {
		PyErr_Format(PyExc_ValueError, "The radius must be a positive number");
		return -1;
		
	}
	self->radius = radius;
	return 0;
}

KLBO kolibaArcGetRadius(klbo(Arc,self), void *closure) {
	return PyFloat_FromDouble(self->radius);
}

static int kolibaArcSetRadius(klbo(Arc,self), PyObject *value, void *closure) {
	bool err = false;
	double radius;
	if (PyFloat_Check(value)) radius = PyFloat_AsDouble(value);
	else if (PyLong_Check(value)) radius = (double)PyLong_AsDouble(value);
	else err = true;
	if ((err) || (radius <= 0.0)){
		PyErr_SetString(PyExc_TypeError, "The radius must be a positive number");
		return -1;
	}
	self->radius = radius;
	return 0;
}

KLBO kolibaArcSectorArea(klbo(Arc,self)) {
	return PyFloat_FromDouble(KOLIBA_CircularArcSectorArea(&self->a, self->radius));
}

KLBO kolibaArcLength(klbo(Arc,self)) {
	return PyFloat_FromDouble(KOLIBA_CircularArcLength(&self->a, self->radius));
}

KLBO kolibaChordLength(klbo(Arc,self)) {
	return PyFloat_FromDouble(KOLIBA_CircularChordLength(&self->a, self->radius));
}

KLBO kolibaArcSagitta(klbo(Arc,self)) {
	return PyFloat_FromDouble(self->radius*KOLIBA_AngleVersine(&self->a));
}

static PyMethodDef kolibaArcMethods[] = {
	{"area", (PyCFunction)kolibaArcSectorArea, METH_NOARGS, "Return the circular arc sector area of the angle and radius"},
	{"length", (PyCFunction)kolibaArcLength, METH_NOARGS, "Return the circular arc length"},
	{"chord", (PyCFunction)kolibaChordLength, METH_NOARGS, "Return the circular chord length"},
	{"sagitta", (PyCFunction)kolibaArcSagitta, METH_NOARGS, "Return the sagitta of the circular arc"},
	{NULL}
};

klbgetset(Arc) = {
	{"radius", (getter)kolibaArcGetRadius, (setter)kolibaArcSetRadius, "radius of the arc", NULL},
	{NULL}
};

static PyTypeObject kolibaArcType = {
	PyVarObject_HEAD_INIT(NULL,0)
	.tp_name = "koliba.Arc",
	.tp_doc  = "Arc objects",
	.tp_basicsize = sizeof(kolibaArcObject),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_init = (initproc)kolibaArcInit,
	.tp_methods = kolibaArcMethods,
	.tp_getset = kolibaArcGetSet,
};

bool koliba_frangle_exponent(double midpoint, double *exponent) {
	if ((midpoint <= 0.0) || (midpoint >= 1.0)) {
		PyErr_Format(PyExc_ValueError, "The midpoint must be greater than zero and lesser than one");
		return false;
	}
	*exponent = KOLIBA_MidpointShift(midpoint);
	return true;
}

KLBO koliba_frangle_recalculate(klbo(Frangle,self), bool t) {
	if (t) self->t = (double)self->frame / (double)self->frames;
	if (!self->monocycle) {
		KOLIBA_AngleFromT(&self->a, self->t, self->exponent);
	}
	else KOLIBA_AngleMonocycleFromT(&self->a, self->t, self->exponent);
	return (PyObject *)self;
}

klbinit(Frangle) {
	static char *kwlist[] = {"frames", "frame", "midpoint", "mono", "radius", NULL};
	double midpoint = 0.5;
	double exponent;
	double radius   = KOLIBA_1DivSqrtPi;
	int    frames   = 120;
	int    frame    = 0;
	int    mono     = 1;	// C parses bool values into full-size int.

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|iidpd", kwlist, &frames, &frame, &midpoint, &mono, &radius))
		return -1;
	else if (frames <= 0) {
		PyErr_Format(PyExc_ValueError, "The frames must be a positive integer");
		return -1;
	}
	else if (radius <= 0.0) {
		PyErr_Format(PyExc_ValueError, "The radius must be a positive number");
		return -1;
		
	}
	else if (!koliba_frangle_exponent(midpoint, &exponent)) return -1;
	self->frames    = frames;
	self->frame     = frame;
	self->midpoint  = midpoint;
	self->exponent  = exponent;
	self->monocycle = (bool)mono;
	self->radius    = radius;
	koliba_frangle_recalculate(self, true);
	return 0;
}

KLBO kolibaFrangleGetFrames(klbo(Frangle,self), void *closure) {
	return PyLong_FromLong(self->frames);
}

static int kolibaFrangleSetFrames(klbo(Frangle,self), PyObject *value, void *closure) {
	int frames = 0;
	if (PyLong_Check(value)) frames = PyLong_AsLong(value);
	if (frames <= 0) {
		PyErr_Format(PyExc_ValueError, "The frames must be a positive integer");
		return -1;
	}
	self->frames = frames;
	koliba_frangle_recalculate(self, true);
	return 0;
}

KLBO kolibaFrangleGetFrame(klbo(Frangle,self), void *closure) {
	return PyLong_FromLong(self->frame);
}

static int kolibaFrangleSetFrame(klbo(Frangle,self), PyObject *value, void *closure) {
	if (PyLong_Check(value)) self->frame = PyLong_AsLong(value);
	else {
		PyErr_Format(PyExc_ValueError, "The frame must be an integer");
		return -1;
	}
	koliba_frangle_recalculate(self, true);
	return 0;
}

KLBO kolibaFrangleGetMidpoint(klbo(Frangle,self), void *closure) {
	return PyFloat_FromDouble(self->midpoint);
}

static int kolibaFrangleSetMidpoint(klbo(Frangle,self), PyObject *value, void *closure) {
	double midpoint = 0.0;
	double exponent;
	if (PyFloat_Check(value)) midpoint = PyFloat_AsDouble(value);
	else if (PyLong_Check(value)) midpoint = (double)PyLong_AsDouble(value);
	if (!koliba_frangle_exponent(midpoint, &exponent)) return -1;
	self->midpoint = midpoint;
	self->exponent = exponent;
	koliba_frangle_recalculate(self, false);
	return 0;
}

KLBO kolibaFrangleGetMonocycle(klbo(Frangle,self), void *closure) {
	if (self->monocycle) {
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

static int kolibaFrangleSetMonocycle(klbo(Frangle,self), PyObject *value, void *closure) {
	if (PyBool_Check(value)) self->monocycle = (value == Py_True);
	else {
		PyErr_Format(PyExc_ValueError, "The monocycle property must be True or False");
		return -1;
	}
	koliba_frangle_recalculate(self, false);
	return 0;
}

KLBO kolibaFrangleGetT(klbo(Frangle,self), void *closure) {
	return PyFloat_FromDouble(self->t);
}

static int kolibaFrangleSetT(klbo(Frangle,self), PyObject *value, void *closure) {
	double t;
	if (PyFloat_Check(value)) t = PyFloat_AsDouble(value);
	else if (PyLong_Check(value)) t = (double)PyLong_AsDouble(value);
	else return -1;
	self->t = t;
	koliba_frangle_recalculate(self, false);
	return 0;
}

KLBO kolibaFranglePolsine(klbo(Frangle,self)) {
	return PyFloat_FromDouble((self->monocycle) ? ((1.0-KOLIBA_AngleFactorCosine(&self->a,0.5))/2.0) : KOLIBA_AnglePolsine(&self->a));
}

KLBO kolibaFranglePolcosine(klbo(Frangle,self)) {
	return PyFloat_FromDouble((self->monocycle) ? ((1.0+KOLIBA_AngleFactorCosine(&self->a,0.5))/2.0) : KOLIBA_AnglePolcosine(&self->a));
}

static PyMethodDef kolibaFrangleMethods[] = {
	{"polsin", (PyCFunction)kolibaFranglePolsine, METH_NOARGS, "Return the polsine of the frangle"},
	{"polcos", (PyCFunction)kolibaFranglePolcosine, METH_NOARGS, "Return the polcosine of the frangle"},
	{NULL}
};

klbgetset(Frangle) = {
	{"frames", (getter)kolibaFrangleGetFrames, (setter)kolibaFrangleSetFrames, "total number of frames of the frangle", NULL},
	{"frame", (getter)kolibaFrangleGetFrame, (setter)kolibaFrangleSetFrame, "current frame of the frangle", NULL},
	{"midpoint", (getter)kolibaFrangleGetMidpoint, (setter)kolibaFrangleSetMidpoint, "midpoint of the frangle", NULL},
	{"mono", (getter)kolibaFrangleGetMonocycle, (setter)kolibaFrangleSetMonocycle, "the monocycle state of the frangle", NULL},
	{"monocycle", (getter)kolibaFrangleGetMonocycle, (setter)kolibaFrangleSetMonocycle, "the monocycle state of the frangle", NULL},
	{"t", (getter)kolibaFrangleGetT, (setter)kolibaFrangleSetT, "the 't' of the frangle", NULL},
	{NULL}
};

static PyTypeObject kolibaFrangleType = {
	PyVarObject_HEAD_INIT(NULL,0)
	.tp_name = "koliba.Frangle",
	.tp_doc  = "Frangle (Frame Angle) objects",
	.tp_basicsize = sizeof(kolibaFrangleObject),
	.tp_itemsize = 0,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	.tp_init = (initproc)kolibaFrangleInit,
	.tp_methods = kolibaFrangleMethods,
	.tp_getset = kolibaFrangleGetSet,
};

KLBO koliba_MidpointShift(PyObject *self, PyObject *args) {
	double midpoint;

	if (!PyArg_ParseTuple(args, "d", &midpoint)) return NULL;
	else if ((midpoint <= 0.0) || (midpoint >= 1.0)) {
		PyErr_SetString(PyExc_TypeError, "The midpoint must be greater than 0 and lesser than 1");
		return NULL;
	}
	return PyFloat_FromDouble(KOLIBA_MidpointShift(midpoint));	
}

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
	{"MidpointToShift", koliba_MidpointShift, METH_VARARGS, "Converts a midpoint into a shift."},
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

	kolibaArcType.tp_base = &kolibaAngleType;
	kolibaFrangleType.tp_base = &kolibaArcType;
	if ((PyType_Ready(&kolibaAngleType) < 0) ||
		(PyType_Ready(&kolibaArcType) < 0) ||
		(PyType_Ready(&kolibaFrangleType) < 0)) return NULL;
	if ((m = PyModule_Create(&kolibamodule)) == NULL) return NULL;
	Py_INCREF(&kolibaAngleType);
	if (PyModule_AddObject(m, "Angle", (PyObject *)&kolibaAngleType) < 0) {
		Py_DECREF(&kolibaAngleType);
		Py_DECREF(m);
		return NULL;
	}
	Py_INCREF(&kolibaArcType);
	if (PyModule_AddObject(m, "Arc", (PyObject *)&kolibaArcType) < 0) {
		Py_DECREF(&kolibaArcType);
		Py_DECREF(&kolibaAngleType);
		Py_DECREF(m);
		return NULL;
	}
	Py_INCREF(&kolibaFrangleType);
	if (PyModule_AddObject(m, "Frangle", (PyObject *)&kolibaFrangleType) < 0) {
		Py_DECREF(&kolibaFrangleType);
		Py_DECREF(&kolibaArcType);
		Py_DECREF(&kolibaAngleType);
		Py_DECREF(m);
		return NULL;
	}
	if ((d = PyModule_GetDict(m))) {
		Py_INCREF(d);
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
		DoubleConst("ickappa", KOLIBA_1Div1MinKappa);
		Py_DECREF(d);
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
