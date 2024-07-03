import numpy as np

def calculate_Lf(LN, f, Tf, Tr, alpha_f, alpha_r, LU, p0=20e-6):
    """
    Calculate the sound pressure level Lf in dB of a pure tone of frequency f.

    Parameters:
    LN (float): Loudness level in phon
    f (float): Frequency in Hz
    Tf (float): Threshold of hearing in dB at frequency f
    Tr (float): Threshold of hearing at 1000 Hz in dB
    alpha_f (float): Exponent for loudness perception at frequency f
    alpha_r (float): Exponent for loudness perception at 1000 Hz
    LU (float): Magnitude of the linear transfer function normalized at 1000 Hz in dB
    p0 (float): Reference sound pressure level, default is 20 µPa

    Returns:
    float: Sound pressure level Lf in dB
    """

    # Constants
    p0 = 20e-6

    # Calculate Lf using the given formula
    # term1 = (p0 / p0) ** (alpha_r - alpha_f)
    term2 = 10 ** ((alpha_r * LN / 10) - 10 ** (alpha_r * Tr / 10) + 10 ** ((alpha_f * (Tf + LU) / 10)))
    # term3 = 10 ** ((0.03 * LN / phon) - 10 ** 0.072)
    # term4 = 10 ** ((alpha_f * (Tf + LU) / 10))

    Lf = (10 / alpha_f) * np.log10((4 * 10 ** -10) ** (0.3 - alpha_f) * term2 - LU)

    return Lf

# Example usage:
LN = 40  # Loudness level in phon
f = 1000  # Frequency in Hz
Tf = 2.4  # Threshold of hearing in dB at frequency f
Tr = 10  # Threshold of hearing at 1000 Hz in dB
alpha_f = 0.5  # Exponent for loudness perception at frequency f
alpha_r = 0.3  # Exponent for loudness perception at 1000 Hz
LU = 0  # Magnitude of the linear transfer function normalized at 1000 Hz in dB

Lf = calculate_Lf(LN, f, Tf, Tr, alpha_f, alpha_r, LU)
print("Sound pressure level Lf in dB:", Lf)