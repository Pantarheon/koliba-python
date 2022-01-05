# Koliba

This is a Python port of the [koliba library](https://github.com/Pantarheon/libkoliba).
Or will be when it is completed. :)

You need to have it installed on your system. The easiest way is to download it
by git from the above-mentioned link. On Linux and such (including in the Windows
Sublayer for Linux), use:

```
git clone https://github.com/Pantarheon/libkoliba.git
cd libkoliba
cd src
sudo make install clean
```

Or you could download the full `koliba` package,

```
git clone --recurse-submodules https://github.com/Pantarheon/koliba.git
cd koliba
cd libkoliba
cd src
sudo make install clean
cd ../../src       # to get to koliba/src
sudo make install-all clean
cd ../freliba/src  # this is optional for frei0r users
sudo make install clean
```

On `MS Windows` without the Windows Sublayer for Windows, the `koliba` library
is linked statically with the `koliba.pyd` file.

## Globals

The `koliba` module currently contains several global functions, which may or
may not remain once the module is released.

## The Angle class

At first glance this class might be a general math/trigonometry class. It is not.
Everything in `koliba` exists to facilitate color grading and effects. The `Angle`
class is no exception. Its main purpose is to allow gradual change of parameters
by producing values between 0 and 1. Trigonometric functions are very useful
in this endeavor.

We can create an object of the `Angle` class, like this,

```
a = Angle(angle, units)
```

In here `a` is a new object of the `Angle` class, `angle` is, well, the
angle in `units` units, which can be `KAU_degrees`, `KAU_radians`,
`KAU_pis` and `KAU_turns`. Examples,

```
a = Angle(90)
a = Angle(90, KAU_degrees)
a = Angle(pi/2, KAU_radians)
a = Angle(0.5, KAU_pis)
a = Angle(0.25, KAU_turns)
```

Any of these creates object `a` containing the angle of `90°` which we can
easily verify by typing,

```
>>> a.degrees
90.0
```

We can also check the other units,

```
>>> a.radians
1.5707963267948966
>>> a.pis
0.5
>>> a.turns
0.25
```

By the way, `KAU` stands for `Koliba Angle Units`. Also, `pi` is defined
by importing `koliba`,

```
>>> from koliba import *
>>> pi
3.141592653589793
```

> Please note, that while the Python shell displays `pi` as a decimal
> number, it is declared in the `koliba` library as the bit-by-bit
> binary value of a 64-bit floating point number, spat out by the `fldpi`
> op-code of the Intel microprocessor inside my PC.
>
> In other words, it is presumably as precise as the `IEEE 754` double-precision
> 64-bit floating-point binary can handle.

Additionally, a number of colors are defined as their angle on the color
wheel, so we can use them in creating an object of the `Angle` class
(or assign them to an existing object),

```
>>> a = Angle(red)
>>> a.degrees
0.0
>>> a.degrees = blue
>>> a.degrees
240.0
>>> a.degrees = cerise
>>> a.turns
0.875
>>> a.degrees
315.0
```

The colors are, `red`, `scarlet`, `vermilion`, `persimmon`, `orange`,
`orangepeel`, `amber`, `goldenyellow`, `yellow`, `lemon`, `lime`,
`springbud`, `chartreuse`, `brightgreen`, `harlequin`, `neongreen`,
`green`, `jade`, `erin`, `emerald`, `springgreen`, `mint`, `aquamarine`,
`turquoise`, `cyan`, `skyblue`, `capri`, `cornflower`, `azure`, `cobalt`,
`cerulean`, `sapphire`, `blue`, `iris`, `indigo`, `veronica`, `violet`,
`amethyst`, `purple`, `phlox`, `magenta`, `fuchsia`, `cerise`,
`deeppink`, `rose`, `raspberry`, `crimson`, and `amaranth`. They are
spaced out equally along the color wheel, 7.5° apart from their neighbors
on either side (including amaranth and red, since it is a circle).

The `Angle` class provides a number of methods,

```
>>> a = Angle(skyblue)
>>> a.sin()
-0.13052619222005132
>>> a.cos()
-0.9914448613738105
>>> a.versin()
1.9914448613738105
>>> a.haversin()
0.9957224306869052
>>> a.vercos()
0.008555138626189507
>>> a.havercos()
0.004277569313094753
>>> a.polsin()
0.5327015646150717
>>> a.polcos()
0.4672984353849283
```

### Normalization

Given the purpose of the `koliba` library is to help with color effects,
it is generally useful to normalize the angle. In trigonometry
it is common to refer to the angles as being in the `-π < angle <= π` range
of radians.

For our purposes it is more useful to normalize them either to the
`0 <= angle < 1` turns, or the `0 <= angle <= 1` turns, range.

> Please note the subtle difference, the first range never reaches `1`,
> while the second does.

The first of these is cyclical, that is, it repeats ever 360° (or every turn).
That is the default of sine, cosine, and other trigonometry functions, so
most of the time we do not need to normalize the angle explicitly. But we can
if we need to (or even want to):

```
>>> from koliba import Angle
>>> a = Angle(500)
>>> a.sin()
0.642787609686539
>>> a.degrees
500.0
>>> a.normalize()
>>> a.sin()
0.6427876096865395
>>> a.degrees
140.0
>>> a.degrees = -987
>>> a.sin()
0.9986295347545739
>>> a.degrees
-987.0
>>> a.normalize()
>>> a.sin()
0.9986295347545738
>>> a.degrees
93.0
```

#### Monocycle

The other normalizing method restrains the angle to just one
cycle, or `monocycle`. That is, if an angle is negative, it is treated
as if it equaled `0`. When an angle is greater than `360°` (or `1` turn, or
`2π` radians...), it is treated as `360°` (or `1` turn, or `2π` radians...).

For example,

```
>>> from koliba import *
>>> a = Angle(60)
>>> a.cos()
0.5000000000000001
>>> a.monocos()
0.5000000000000001
>>> a.degrees = 567.89
>>> a.cos()
-0.8838472857870293
>>> a.monocos()
1.0
>>> a.degrees = -567.89
>>> a.cos()
-0.8838472857870293
>>> a.monocos()
1.0
```

Once again, we can use the same `Angle.normalize()` method, but we need
to pass `True` to it (technically, we should have passed `False` before,
but that is the default, so we did not have to):

```
>>> from koliba import Angle
>>> a = Angle(500)
>>> a.sin()
0.642787609686539
>>> a.degrees
500.0
>>> a.normalize(True)
>>> a.sin()
-2.4492127076447545e-16
>>> a.degrees
360.0
>>> a.degrees = -987
>>> a.sin()
0.9986295347545739
>>> a.degrees
-987.0
>>> a.normalize(True)
>>> a.sin()
0.0
>>> a.degrees
0.0
```

> And yes, sin 360° = 0, not -2.4492127076447545e-16 but Python seems to
> act as if the computers could do double float math exactly (just as
> they do integer math as long as it does not overflow).
>
> They cannot, and if Python did not go overboard with printing so many
> decimal places, it would have shown the -2.4492127076447545e-16 as 0.

We have a separate (sub)class that allows us more control over monocycle
use. It shall be described below as soon as I can get to it.

To see the complete list of the methods supported by the `koliba.Angle` class
use the `dir()` function built into Python:

```
>>> import koliba
>>> dir (koliba.Angle)
['__add__', '__class__', '__delattr__', '__dir__', '__doc__', '__eq__', '__floordiv__', '__format__', '__ge__', '__getattribute__', '__gt__', '__hash__', '__iadd__', '__ifloordiv__', '__imul__', '__init__', '__init_subclass__', '__isub__', '__itruediv__', '__le__', '__lt__', '__mul__', '__ne__', '__new__', '__pow__', '__radd__', '__reduce__', '__reduce_ex__', '__repr__', '__rfloordiv__', '__rmul__', '__rpow__', '__rsub__', '__rtruediv__', '__setattr__', '__sizeof__', '__str__', '__sub__', '__subclasshook__', '__truediv__', 'cos', 'degrees', 'fcos', 'fhavercos', 'fhaversin', 'fmonohavercos', 'fmonohaversin', 'fmonovercos', 'fmonoversin', 'fsin', 'fvercos', 'fversin', 'havercos', 'haversin', 'monocos', 'monocycle', 'monohavercos', 'monohaversin', 'monopolcos', 'monopolsin', 'monosin', 'monovercos', 'monoversin', 'normalize', 'pis', 'polcos', 'polsin', 'radians', 'sin', 'turns', 'vercos', 'versin']
```

Please note that the methods whose names start with an `f` take an argument
of the `factor` to multiply the `angle` with before applying the math (so, if the `angle` is `90°` and the `factor` is `1/2`, `fsin(1/2)` will return `sin 45°`):

```
>>> from koliba import Angle
>>> a = Angle(90)
>>> a.sin()
1.0
>>> a.cos()
6.123031769111886e-17
>>> a.fsin(1/2)
0.7071067811865475
>>> a.fcos(1/2)
0.7071067811865476
>>> a.fsin(2)
1.2246063538223773e-16
>>> a.fcos(2)
-1.0
```
