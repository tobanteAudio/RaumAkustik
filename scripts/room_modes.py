import numpy as np


def hz_to_note(frequency):
    # Reference frequency for A4
    A4_frequency = 440.0
    # Reference position for A4 in the note list
    A4_position = 9
    # List of note names
    note_names = ["C", "C#", "D", "D#", "E",
                  "F", "F#", "G", "G#", "A", "A#", "B"]

    # Calculate the number of semitones between the given frequency and A4
    semitones_from_A4 = 12 * np.log2(frequency / A4_frequency)
    # Round to the nearest semitone
    semitone_offset = round(semitones_from_A4)

    # Calculate the octave
    octave = 4 + (A4_position + semitone_offset) // 12
    # Calculate the note position
    note_position = (A4_position + semitone_offset) % 12

    # Get the note name
    note_name = note_names[note_position]

    return f"{note_name}{octave}"


def room_mode(L, W, H, m, n, p):
    c = 343
    return c*0.5*np.sqrt((m/L)**2 + (n/W)**2 + (p/H)**2)


def room_mode_kind(m, n, p):
    non_zero = (m != 0) + (n != 0) + (p != 0)
    if non_zero == 1:
        return "axial"
    if non_zero == 2:
        return "tangential"
    return "oblique"


def frequency_spacing_index(modes):
    psi = 0.0
    num = len(modes)
    f0 = modes[0]['frequency']
    fn = modes[-1]['frequency']
    delta_hat = (fn-f0)/(num-1)
    for n in range(1, num):
        prev = modes[n-1]['frequency']
        mode = modes[n]
        freq = mode['frequency']
        delta = freq-prev
        psi += (delta/delta_hat)**2
    return psi / (num-1)


if __name__ == "__main__":
    scale = 3.0

    # Tobi
    L = 6.0 * scale
    W = 3.65 * scale
    H = 3.12 * scale

    # Optimal ratio A
    L = 6 * scale
    W = 4.96 * scale
    H = 4.14 * scale

    # Optimal ratio B
    L = 7 * scale
    W = 5.19 * scale
    H = 3.70 * scale

    # Worst ratio (Cube)
    # W = L
    # H = L

    A = L*W
    V = A*H
    S = 2*(L*W+L*H+W*H)

    modes = []
    max_order = 2
    for m in range(max_order+1):
        for n in range(max_order+1):
            for p in range(max_order+1):
                if m+n+p > 0:
                    modes.append({
                        "m": m,
                        "n": n,
                        "p": p,
                        "frequency": room_mode(L, W, H, m, n, p)
                    })

    modes = sorted(modes, key=lambda x: x['frequency'])[:25]

    print(f"{L=:.3f}m {W=:.3f}m {H=:.3f}m")
    print(f"{A=:.2f}m^2 {S=:.2f}m^2 {V=:.2f}m^3")
    print(f"w/h={W/H:.2f} l/h={L/H:.2f} l/w={L/W:.2f}")
    print(f"FSI({len(modes)}): {frequency_spacing_index(modes):.2f}")
    print("")

    for mode in modes:
        m = mode['m']
        n = mode['n']
        p = mode['p']
        freq = mode['frequency']
        note = hz_to_note(freq)
        kind = room_mode_kind(m, n, p)
        print(f"[{m},{n},{p}] = {freq:.2f}Hz ({note}) {kind}")
