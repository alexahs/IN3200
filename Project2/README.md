# Isotropic Diffusion Algorithm

This program runs the Iso Diffusion Denoising algorithm with a JPEG image as input. It is implemented both in a serialized way, and with MPI paralellism. The program outputs a denoised image. The parallelized version can be run with any number of processes. 


The compiling is done by running the Makefiles, which also compiles and links to the library folder 'simple-jpeg', and creates the executables which must be run with the following command line arguments(both versions):
-input filename
-output filename
-the parameter kappa (usually set to 0.2)
-number of denoising iterations



Example serialised: 
~~~
./serial_main.exe mona_lisa_noisy.jpg mona_lisa_denoised.jpg 0.2 100
~~~

Example parallelized:
~~~
mpirun -np 4./parallel_main.exe mona_lisa_noisy.jpg mona_lisa_denoised.jpg 0.2 100
~~~
