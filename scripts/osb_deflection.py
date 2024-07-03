# Constants
load_kg = 240
gravity = 9.81  # m/s^2
span_length_m = 0.625  # 25 cm
thickness_m = 0.018  # 18 mm
width_m = 0.67  # Assume width to be 1 meter for calculation
modulus_of_elasticity_pa = 3.5 * 10**9  # 3500 MPa in N/m^2

# Convert load to Newtons
load_n = load_kg * gravity

# Distribute load equally among three spans
load_per_span_n = load_n / 3

# Moment of inertia I for a rectangular section
moment_of_inertia_m4 = (width_m * thickness_m**3) / 12

# Deflection formula: δ = (P * L^3) / (48 * E * I)
deflection_m = (load_per_span_n * span_length_m**3) / (48 * modulus_of_elasticity_pa * moment_of_inertia_m4)

# Convert deflection to millimeters
deflection_mm = deflection_m * 1000

# Output result
print(f"Deflection per span: {deflection_mm:.2f} mm")