import math

# Given values
L = 5  # Length of the beam in meters
b = 0.12  # Width of the beam in meters
h = 0.12  # Height of the beam in meters
E = 11 * 10**9  # Young's modulus for pine wood in Pascals
rho = 500  # Density of pine wood in kg/m^3

# Calculations
I = (b * h**3) / 12  # Moment of inertia
A = b * h  # Cross-sectional area

term_inside_sqrt = (E * I) / (rho * A)
sqrt_value = math.sqrt(term_inside_sqrt)
frequency = (1.875**2) / (2 * math.pi * L**2) * sqrt_value

print(frequency)