# Koliba

This is a Python port of the [koliba library](https://github.com/Pantarheon/libkoliba).
Or will be when it is completed. :)

You need to have it installed on your system. The easiest way is to download it
by git from the above-mentioned link.

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
