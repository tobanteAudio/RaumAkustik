import numpy as np


def surface_area(l, w, h):
    return 2 * (l*w + l*h + w*h)


def room_constant(S, alpha):
    return (S*alpha)/(1-alpha)


def critical_distance(R, Q):
    return 0.141 * np.sqrt(R*Q)


def rt60(V, S, A):
    # RT60 = k * V / A
    k = 0.161
    return k * V / (S*A)


def nominal_reverberation_time(V):
    V0 = 100.0
    return np.clip(0.25 * (V/V0)**(1/3), 0.2, 0.4)


def required_absorption(V, target_T60):
    return (0.161*V)/target_T60


def main():
    L = 6.0
    W = 3.65
    H = 3.12

    L = 7.0
    W = 5.19
    H = 3.70

    S = surface_area(L, W, H)
    V = L*W*H
    A = 0.62
    R = room_constant(S, A)
    Q = 1.0

    distance = critical_distance(R, Q)
    t60 = rt60(V, S, A)

    target_t60 = nominal_reverberation_time(V)
    absorption = required_absorption(V, target_t60)

    print(f"{L=} {W=} {H=} {S=:.2f}m2")
    print(f"{V=:.2f}m3 {A=:.2f} {R=:.2f} {Q=:.2f}\n")

    print(f"RT60                = {t60:.2f}s")
    print(f"Critical distance   = {distance:.2f}m\n")

    print(f"Target RT60         = {target_t60:.3f}s")
    print(f"Required absorption = {absorption:.2f} sabins")


main()
