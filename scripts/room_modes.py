import numpy as np


def calculate_axial_modes(length, width, height):
    # Speed of sound in air in meters per second
    c = 343

    # Axial modes along the length, width, and height
    modes = {
        'Length (1,0,0)': c / (2 * length),
        'Width (0,1,0)': c / (2 * width),
        'Height (0,0,1)': c / (2 * height)
    }

    return modes


def room_mode(L, W, H, m, n, p):
    c = 343
    return c*0.5*np.sqrt((m/L)**2 + (n/W)**2 + (p/H)**2)


def print_modes(modes):
    print("Axial Room Modes (in Hz):")
    for mode, frequency in modes.items():
        print(f"{mode}: {frequency:.2f} Hz")


if __name__ == "__main__":
    # Example room dimensions in meters
    length = 6.0
    width = 3.65
    height = 3.12

    print(f"L: {length} W: {width}, H: {height}\n")

    print(f"(1,0,0): {room_mode(length, width, height, 1, 0, 0):.2f} Hz")
    print(f"(0,1,0): {room_mode(length, width, height, 0, 1, 0):.2f} Hz")
    print(f"(0,0,1): {room_mode(length, width, height, 0, 0, 1):.2f} Hz")
    print("")

    print(f"(1,1,0): {room_mode(length, width, height, 1, 1, 0):.2f} Hz")
    print(f"(0,1,1): {room_mode(length, width, height, 0, 1, 1):.2f} Hz")
    print(f"(1,0,1): {room_mode(length, width, height, 1, 0, 1):.2f} Hz")
    print("")

    print(f"(1,1,1): {room_mode(length, width, height, 1, 1, 1):.2f} Hz")
    print("")

    print(f"(2,0,0): {room_mode(length, width, height, 2, 0, 0):.2f} Hz")
    print(f"(0,2,0): {room_mode(length, width, height, 0, 2, 0):.2f} Hz")
    print(f"(0,0,2): {room_mode(length, width, height, 0, 0, 2):.2f} Hz")
    print("")
