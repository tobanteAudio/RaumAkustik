
def main():
    vars_per_point = 2
    bytes_per_var = 8
    bytes_per_point = vars_per_point*bytes_per_var

    ppw = 3.2
    sigma = ppw/2
    c = 343
    fmax = 20_000

    point_density = sigma**3 * (2*fmax/c)**3
    storage_density = bytes_per_point * point_density

    length = 7.0
    width = 5.19
    height = 3.7
    volume = length*width*height

    print(f"ppw:             {ppw}")
    print(f"fmax:            {fmax:.2f} Hz")
    print(f"bytes/point:     {bytes_per_point:.0f}")
    print("")

    print(f"point_density:   {point_density/1_000_000:.1f} M / m^3")
    print(f"storage_density: {storage_density/1_000_000:.2f} MB / m^3")
    print("")

    print(f"volume:  {volume:.2f} m^3")
    print(f"points:  {point_density*volume/1_000_000:.1f} M")
    print(f"storage: {storage_density*volume/1_000_000_000:.2f} GB")


main()
