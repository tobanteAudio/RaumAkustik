import numpy as np


def main():
    c = 343  # speed of sound m/s (20degC)
    fmax = 20_000  # Hz
    PPW = 6  # points per wavelength at fmax
    duration = 0.25  # seconds
    refl_coeff = 0.9  # reflection coefficient

    Bx, By = 10.0, 4.0  # box dims (with lower corner at origin)
    x_in, y_in = Bx*0.5, By*0.5  # source input position
    R_dome = By*0.5  # heigh of dome (to be centered on roof of box)

    draw = True
    add_dome = False
    apply_rigid = True
    apply_loss = True

    if apply_loss:
        assert apply_rigid

    if add_dome:
        Lx = Bx
        Ly = By+R_dome
    else:
        Lx = Bx
        Ly = By

    # calculate grid spacing, time step, sample rate
    dx = c/fmax/PPW  # grid spacing
    dt = np.sqrt(0.5)*dx/c
    SR = 1/dt

    Nx = np.ceil(Lx/dx)+2  # number of points in x-dir
    Ny = np.ceil(Ly/dx)+2  # number of points in y-dir
    Nt = np.ceil(duration/dt)  # number of time-steps to compute

    print(f'SR = {SR:.3f} Hz')
    print(f'Δx = {dx:.5f} m / {dx*1000:.2f} mm')
    print(f'Nx = {int(Nx)} Ny = {int(Ny)} Nt = {int(Nt)}')


main()
