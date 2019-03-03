import numpy as np
from math import *


def to_rad(angle):
    radian = (float(angle) * pi) / 180
    return radian


def sin_f(angle):
    angle = to_rad(angle)
    taylor_sum = 0
    for i in range(0, 8, 1):
        taylor_sum += (((-1)**i) * angle**(2*i + 1)) / factorial(2*i + 1)

    return taylor_sum


def cos_f(angle):
    angle = to_rad(angle)
    taylor_sum = 0
    for i in range(0, 8, 1):
        taylor_sum += (((-1)**i) * angle**(2*i)) / factorial(2*i)

    return taylor_sum


print(cos_f(60))


phi = 10
eps = 35
theta = 80

x_axis = np.asarray([[1, 0, 0],
                     [0, cos_f(phi), -sin_f(phi)],
                     [0, sin_f(phi), cos_f(phi)]])
y_axis = np.asarray([[cos_f(theta), 0, sin_f(theta)],
                     [0, 1, 0],
                     [-sin_f(theta), 0, cos_f(theta)]])
z_axis = np.asarray([[cos_f(eps), -sin_f(eps), 0],
                     [sin_f(eps), cos_f(eps), 0],
                     [0, 0, 1]])

print((x_axis * y_axis) * z_axis)
