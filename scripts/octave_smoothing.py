import numpy as np

def fractional_octave_smoothing(fft_magnitudes, sample_rate, fft_size, fraction=3):
    """
    Apply fractional octave smoothing to FFT magnitudes.

    Parameters:
    - fft_magnitudes: Array of FFT magnitudes.
    - sample_rate: Sampling rate of the signal.
    - fft_size: Size of the FFT.
    - fraction: Fraction of the octave for smoothing (e.g., 3 for 1/3 octave, 6 for 1/6 octave).

    Returns:
    - smoothed_fft: Array of smoothed FFT magnitudes.
    """
    # Define the reference frequency and calculate center frequencies
    f0 = 1000  # Reference frequency (1 kHz)
    start_freq = 15.625  # Start frequency (20 Hz)
    end_freq = 20480  # End frequency (20.48 kHz)
    
    # Calculate the number of bands needed to cover the frequency range
    n_bands = int(np.ceil(np.log2(end_freq / start_freq) * fraction))
    
    # Generate center frequencies for the fractional octave bands
    center_frequencies = start_freq * 2**(np.arange(n_bands) / fraction)
    
    # Calculate the frequency bin centers for the FFT
    freqs = np.fft.fftfreq(fft_size, 1/sample_rate)[:fft_size//2]
    
    smoothed_fft = np.zeros_like(fft_magnitudes[:fft_size//2])
    
    for fc in center_frequencies:
        fl = fc / 2**(1/(2*fraction))
        fu = fc * 2**(1/(2*fraction))
        
        # Find indices of FFT bins that fall into the current fractional octave band
        indices = np.where((freqs >= fl) & (freqs <= fu))[0]
        
        if len(indices) > 0:
            # Average the magnitudes of the selected FFT bins
            smoothed_fft[indices] = np.mean(fft_magnitudes[indices])
    
    return smoothed_fft

# Example usage:
fft_size = 4096
sample_rate = 44100
fft_data = np.random.rand(fft_size//2)  # Replace with actual FFT magnitudes

# Apply different fractional octave smoothings
smoothed_data_1_3 = fractional_octave_smoothing(fft_data, sample_rate, fft_size, fraction=3)
smoothed_data_1_6 = fractional_octave_smoothing(fft_data, sample_rate, fft_size, fraction=6)
smoothed_data_1_12 = fractional_octave_smoothing(fft_data, sample_rate, fft_size, fraction=12)

# Plotting the results (optional)
# import matplotlib.pyplot as plt

# freqs = np.fft.fftfreq(fft_size, 1/sample_rate)[:fft_size//2]
# plt.plot(freqs, fft_data, label='Original FFT')
# plt.plot(freqs, smoothed_data_1_3, label='1/3 Octave Smoothed FFT')
# plt.plot(freqs, smoothed_data_1_6, label='1/6 Octave Smoothed FFT')
# plt.plot(freqs, smoothed_data_1_12, label='1/12 Octave Smoothed FFT')
# plt.xscale('log')
# plt.xlabel('Frequency (Hz)')
# plt.ylabel('Magnitude')
# plt.legend()
# plt.grid(True)
# plt.show()

np.set_printoptions(suppress=True)

fraction = 3
fcentre  = 10**3 * (2 ** (np.arange(-18,13)/fraction))
fd = 2**(1/(fraction*2))
fupper = fcentre * fd
flower = fcentre / fd
print(fcentre)
print(flower)
print(fupper)