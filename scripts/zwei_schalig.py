import numpy as np
import matplotlib.pyplot as plt


def calculate_resonant_frequency(m1_prime, m2_prime, d):
    m_double_prime = (m1_prime * m2_prime) / (m1_prime + m2_prime)

    fR_1 = 190 * np.sqrt(0.11 / (d * m_double_prime))
    fR_2 = 65 / np.sqrt(d * m_double_prime)

    return fR_1, fR_2


def calculate_resonant_frequency_full_connection(m1_prime, m2_prime, s_double_prime):
    m_double_prime = (m1_prime * m2_prime) / (m1_prime + m2_prime)
    fR = 190 * np.sqrt(s_double_prime / m_double_prime)
    return fR


def main():
    # Example usage:
    m1_prime = 600*0.012*1 +720*0.012*2 # Example value for m1_prime in kg·m^-2
    m2_prime = 720*0.012*2  # Example value for m2_prime in kg·m^-2
    d = 0.20       # Example value for d in meters

    fR_1, fR_2 = calculate_resonant_frequency(m1_prime, m2_prime, d)
    print(f"fR (approximation 1): {fR_1} Hz")
    print(f"fR (approximation 2): {fR_2} Hz")

    s_double_prime = 0.01  # Example value for s_double_prime in MN·m^-3
    fR = calculate_resonant_frequency_full_connection(
        m1_prime, m2_prime, s_double_prime)
    print(f"fR: {fR} Hz")

    f = np.linspace(20.0, 20000.0, 256)  # Hz
    c = 343  # m/s
    rho = 1.293  # kg/m^3

    # OSB
    m1 = 600*0.012*1  # kg/m^2
    B1_prime = 2100  # Nm
    R1 = 20*np.log10(np.pi*f*m1/(rho*c))-3  # dB
    fc1 = c**2/2*np.pi*np.sqrt(m1/B1_prime)  # Hz
    eta = 0.1
    RX1 = 20*np.log10(np.pi*f*m1/(rho*c)) + 10 * np.log10((2 * eta/np.pi) )*(f / fc1)

    # Plasterboard
    m2 = 720*0.0125*4  # kg/m^2
    B2_prime = 3600  # Nm
    R2 = 20*np.log10(np.pi*f*m2/(rho*c))-3  # dB
    fc2 = c**2/2*np.pi*np.sqrt(m2/B2_prime)  # Hz
    RX2 = 20*np.log10(np.pi*f*m2/(rho*c)) + 10 * np.log10((2 * eta/np.pi) )*(f / fc2)


    plt.figure()
    plt.plot(f, R1, label=f"OSB {m1} kg/m^2 fc={fc1:.2f}Hz")
    plt.plot(f, RX1, label=f"OSB {m1} kg/m^2 fc={fc1:.2f}Hz")
    plt.plot(f, R2, label=f"Plasterboard {m2} kg/m^2 fc={fc2:.2f}Hz")
    plt.plot(f, RX2, label=f"Plasterboard {m2} kg/m^2 fc={fc2:.2f}Hz")
    plt.xlabel('Frequency [Hz]')
    plt.ylabel('SPL [dB]')
    plt.xscale('log')
    # plt.xlim([20, 20000])
    # plt.ylim([50, 100])
    plt.grid(which="both")
    plt.legend()
    plt.show()


main()
