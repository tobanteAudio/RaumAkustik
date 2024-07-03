import numpy as np
import matplotlib.pyplot as plt

frequencies_third_octave = [
    12.5, 16, 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400,
    500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000,
    10000, 12500, 16000
]

spl_0_phon_third_octave = [
    75.3, 69.7, 64.1, 58.5, 53.3, 48.4, 43.4, 38.5, 33.8, 29.3, 25.0, 20.9, 17.2,
    13.8, 10.8, 8.1, 5.9, 4.1, 2.6, 1.4, 0.5, -0.2, -0.7, -1.0, -1.2, -1.2,
    -1.1, -0.9, -0.6, -0.2, 0.3, 0.9
]

spl_20_phon_third_octave = [
    95.3, 89.7, 84.1, 78.5, 73.3, 68.4, 63.4, 58.5, 53.8, 49.3, 45.0, 40.9, 37.2,
    33.8, 30.8, 28.1, 25.9, 24.1, 22.6, 21.4, 20.5, 19.8, 19.3, 19.0, 18.8, 18.8,
    18.9, 19.1, 19.4, 19.8, 20.3, 20.9
]

spl_40_phon_third_octave = [
    115.3, 109.7, 104.1, 98.5, 93.3, 88.4, 83.4, 78.5, 73.8, 69.3, 65.0, 60.9, 57.2,
    53.8, 50.8, 48.1, 45.9, 44.1, 42.6, 41.4, 40.5, 39.8, 39.3, 39.0, 38.8, 38.8,
    38.9, 39.1, 39.4, 39.8, 40.3, 40.9
]

spl_0_phon = [74, 64, 56, 47, 39, 31, 23, 17, 15, 17, 22]
spl_20_phon = [94, 82, 72, 61, 52, 44, 40, 37, 35, 37, 42]
spl_40_phon = [114, 100, 89, 77, 66, 57, 50, 47, 45, 47, 50]


def mass_law(f, m):
    return 20*np.log10(f*m)-47.0


def main():
    mpa = 470*0.12 + 720*0.0125*4  # Wood stud + Drywall
    mpa = 470*0.16 + 600*0.0125*2  # Wood stud + osb
    mpa = 1700*0.10 + 600*0.0125*2  # sand + osb
    mpa = 2400*0.15  # concrete
    mpa = 7850*0.0015*2  # steel door
    mpa = 1700*0.01  # sand
    mpa = 720*0.0125*4 + 600*0.025*2  # drywall

    mpa2 = 720*0.0125*3   # drywall
    mpa2 = 720*0.0125*4 + 600*0.012*1  # drywall + osb
    mpa1 = 7850*0.001*3 + 1700*0.03  # sand + steel

    print(f"MPA: {mpa1:.2f} kg/m^2")

    door_width = 1.0
    door_height = 2.0
    door_area = door_width*door_height
    print(f"Door: {door_width}m x {door_height}m = {door_area}m^2; {mpa1*door_area}kg")
    print("")

    db1 = []
    db2 = []
    for freq in frequencies_third_octave:
        db1.append(mass_law(freq, mpa1))
        db2.append(mass_law(freq, mpa2))
        print(f"{freq} Hz: {mass_law(freq, mpa1):.2f} dB")

    noise_level = 100
    noise = np.ones_like(frequencies_third_octave)*noise_level
    transmission1 = noise-db1
    transmission2 = noise-db2
    transmission12 = noise-db1-db2

    # plt.figure()
    # # plt.plot(frequencies_third_octave, noise, label=f"Noise")
    # plt.plot(frequencies_third_octave, spl_0_phon_third_octave, label=f"0 Phon")
    # plt.plot(frequencies_third_octave, spl_20_phon_third_octave, label=f"20 Phon")
    # # plt.plot(frequencies_third_octave, spl_40_phon, label=f"40 Phon")
    # plt.plot(frequencies_third_octave, transmission1, label=f"Wall {mpa1} kg/m^2")
    # plt.plot(frequencies_third_octave, transmission2, label=f"Wall {mpa2} kg/m^2")
    # # plt.plot(frequencies_third_octave, transmission12, label=f"Wall {mpa1} + {mpa2} kg/m^2")
    # plt.xlabel('Frequency [Hz]')
    # plt.ylabel('SPL [dB]')
    # plt.xscale('log')
    # plt.xlim([16, 20000])
    # plt.ylim([0, 110])
    # plt.grid(which="both")
    # plt.legend()
    # plt.show()


main()
