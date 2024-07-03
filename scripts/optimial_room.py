import sympy as sp
from sympy.abc import x, y, z


def surface_area_rectangular_cube(length, width, height):
    return 2 * (length * width + width * height + height * length)


result = sp.solve([x-7.0, x/z-1.89, x/y-1.35], [x, y, z], dict=True)
L = result[0][x]
W = result[0][y]
H = result[0][z]
A = L*W
V = A*H
S = surface_area_rectangular_cube(L, W, H)

print(f"{L=:.2f}m {W=:.2f}m {H=:.2f}m")
print(f"{A=:.2f}m^2 {S=:.2f}m^2 {V=:.2f}m^3")
