import librosa
import librosa.display
import matplotlib.pyplot as plt
import numpy as np

tone_path = 'scripts/test_data/512kMeasSweep_0_to_20000_44k_PCM16_L.wav'
y, sr = librosa.load(tone_path)

fig, ax = plt.subplots()
# librosa.display.waveshow(y, sr=sr, ax=ax[0])

# ax[0].set(title='Wave')
# ax[0].label_outer()

fft_length = 1024
hop_length = 256

fft = np.fft.fft(y)
# ax.xaxis.set_major_formatter(librosa.display.LogHzFormatter())
ax.plot(np.abs(fft[:len(fft)//2]))

# stft = librosa.stft(y, n_fft=fft_length, hop_length=hop_length)
# D = librosa.amplitude_to_db(np.abs(stft), ref=np.max)
# librosa.display.specshow(D, y_axis='log', sr=sr,
#                          hop_length=hop_length, x_axis='time', ax=ax[1])

# ax[1].set(title='Power spectrogram')
# ax[1].label_outer()


plt.show(block=True)
