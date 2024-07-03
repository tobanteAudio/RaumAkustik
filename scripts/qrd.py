# Quadratic Residue Diffusers
import numpy as np


def is_prime(n):
    """Check if a number is a prime number."""
    if n <= 1:
        return False
    if n <= 3:
        return True
    if n % 2 == 0 or n % 3 == 0:
        return False
    i = 5
    while i * i <= n:
        if n % i == 0 or n % (i + 2) == 0:
            return False
        i += 6
    return True


def next_prime(n):
    """Find the next prime number greater or equal to the given number."""
    if is_prime(n):
        return n
    if n <= 1:
        return 2
    prime = n
    found = False
    while not found:
        prime += 1
        if is_prime(prime):
            found = True
    return prime


def wavelength(frequency, s):
    return s/frequency


def qrd_depth(p, fmin, c):
    w = wavelength(fmin, c)
    print(f"{p=} w={w*100:.2f}cm\n")

    max_depth = 0.0
    for i in range(int(p)):
        R = i**2 % 7
        depth = (R*w) / (2*p)
        max_depth = max(depth, max_depth)
        print(f"{i}: {depth*100.0:.2f}cm")

    print(f"\nmax depth: {max_depth*100:.2f}cm")


def main():
    c = 343
    target = 11
    p = next_prime(target)
    fmin = 300.0
    radius = wavelength(fmin, c) * 3

    print(f"{radius=:.2f} m")
    qrd_depth(p, fmin, c)


main()
