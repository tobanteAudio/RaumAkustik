import numpy as np
import matplotlib.pyplot as plt

# Constants
rho = 2500  # mass density in kg/m^3
h = 150.0/1000.0  # thickness in meters
theta = 0  # angle of incidence in degrees
Z0 = 408  # characteristic impedance of air in kg/m^2/s

# Convert theta to radians
theta_rad = np.radians(theta)

# Frequency range
frequencies = np.linspace(20, 20000, 500)  # from 20 Hz to 20 kHz

# Angular frequency
omega = 2 * np.pi * frequencies

# Compute R
R = 10 * np.log10(1 + (omega**2 * (rho * h * np.cos(theta_rad) / (2 * Z0))**2))

# Plotting
plt.figure(figsize=(10, 6))
plt.semilogx(frequencies, R, label='Sound Reduction Index')
plt.xlabel('Frequency (Hz)')
plt.ylabel('R (dB)')
plt.title('Sound Reduction Index vs Frequency')
plt.grid(True)
plt.legend()
plt.show()
