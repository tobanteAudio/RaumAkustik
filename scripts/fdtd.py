import numpy as np

c = 343  # speed of sound m/s (20degC)
fmax = 4000  # Hz
PPW = 6  # points per wavelength at fmax
duration = 3.0  # seconds

Lx, Ly, Lz = 3.65, 6.0, 3.12  # box dims (with lower corner at origin)
x_in, y_in, z_in = Lx*0.5, Ly*0.5, Lz*0.5  # source input position


# calculate grid spacing, time step, sample rate
dx = c/fmax/PPW  # grid spacing
dt = np.sqrt(0.5)*dx/c
SR = 1/dt


Nx = int(np.ceil(Lx/dx)+2)  # number of points in x-dir
Ny = int(np.ceil(Ly/dx)+2)  # number of points in y-dir
Nz = int(np.ceil(Lz/dx)+2)  # number of points in z-dir
Nt = int(np.ceil(duration/dt))  # number of time-steps to compute

print(f'{Nx=}, {Ny=}, {Nz=}, {Nt=}, Op=2^{round(np.log2(Nx*Ny*Nz*Nt))}')
print(f'SR  = {SR:.3f} Hz')
print(f'Δx  = {dx*1000:.2f} mm')
print(f'Mem = {Nx*Ny*Nz*3*8/1024/1024/1024:.2f} GByte')
