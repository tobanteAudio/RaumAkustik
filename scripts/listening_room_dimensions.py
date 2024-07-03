def is_near_integer_ratio(a, b, threshold=0.05):
    x = a/b
    nearest_integer = round(x)
    return abs(x - nearest_integer) < threshold


def is_in_range(v, min_val, max_val):
    return v >= min_val and v <= max_val


def listening_room_dimensions(l, w, h):
    # Minimum floor area
    area = l*w
    if area < 30:
        print(f"Area to small: {area}m2 < 30m2")

    # mode distribution
    if not is_in_range(l/h, 1.1*w/h, 4.5*w/h-4.0):
        print(f"l/h to w/h not good: {l/h:.3f} ", end="")
        print(f"should be between {1.1*w/h:.3f} and {4.5*w/h-4.0:.3f}")

    if l >= 3*h:
        print(f"l is to long compared to h: {l=:.3f}")

    if w >= 3*h:
        print(f"w is to long compared to h: {w=:.3f}")


def main():
    l = 6.0
    w = 4.96#3.65
    h = 4.14#3.12
    listening_room_dimensions(l, w, h)


main()
