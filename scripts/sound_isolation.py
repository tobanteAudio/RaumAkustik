import numpy as np


def flexural_rigidity(E, t):
    # E = Modulus of Elasticity (Pa)
    # t = thickness (m)
    return (E * t**3) / 10.5


def characteristic_frequency(E, D, t, a, b, n_x, n_y):
    B_prime = flexural_rigidity(E, t)
    m_prime = D*t  # Surface mass in kg/m^2

    term1 = (n_x / a)**2
    term2 = (n_y / b)**2
    return (np.pi / 2) * np.sqrt(B_prime / m_prime * (term1 + term2))

def grenz_frequenz(E,D,t):
    B_prime = flexural_rigidity(E, t)
    M = D*t  # Surface mass in kg/m^2
    return (343**2)/(2*np.pi)*np.sqrt(M/B_prime)

def main():
    # Plasterboard/Drywall
    t = 0.0125  # m
    E = 2400 * 1e6  # Pa
    D = 720  # kg/m^3

    # Example values
    a = 2.5  # Side length in meters
    b = 1.25  # Side length in meters
    n_x = 1  # Mode number
    n_y = 1  # Mode number

    # Calculate the frequency
    f0 = characteristic_frequency(E, D, t, a, b, n_x, n_y)
    fc = grenz_frequenz(E,D,t)
    print(f"{f0=:.3f}Hz")
    print(f"{fc=:.3f}Hz")


main()
