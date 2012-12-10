import numpy
from mayavi.mlab import *

n = 4
t = numpy.linspace(-numpy.pi, numpy.pi, n)
z = numpy.exp(1j*t)
x = z.real.copy()
y = z.imag.copy()
z = numpy.zeros_like(x)

triangles = [(0, i, i+1) for i in range(1, n)]
x1 = numpy.r_[0, x]
y1 = numpy.r_[0, y]
z1 = numpy.r_[1, z]
t1 = numpy.r_[0, t]

triangular_mesh(x1, y1, z1, triangles, scalars=t1)
