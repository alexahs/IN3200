#include "functions.h"

int main(int argc, char* argv[]){
    /*
    This program performs the iso diffusion denoising algorithm, implemented with
    MPI parallelism.
    The filename (must be .jpg) is provided as the first command line argument, followed by the
    output filename. The parameter kappa, commonly set to 0.2, must also be provided,
    along with the number of denoising iterations to perform.
    */

    struct timespec start_time, end_time;
    double time_spent;
    int im_height, im_width, num_comp, num_procs, my_rank, my_m, my_n;
    float kappa = atof(argv[3]);
    int iters = atoi(argv[4]);
    int quality = 75;
    char *input_filename, *output_filename;
    input_filename = argv[1];
    output_filename = argv[2];
    image u, u_bar;
    unsigned char *im_chars;
    int L_m, my_chunk_size;
    int rest;
    int my_bounds[2];


    //initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if(my_rank == 0){
        if(argc != 5){
            printf("Not enough commandline arguments. Exiting..\n");
            exit(1);
        }

        //import jpg file to denoise
        import_JPEG_file(input_filename, &im_chars, &im_height, &im_width, &num_comp);
        printf("Loaded %s, %dx%d\n", input_filename, im_height, im_width );
    }


    //broadcast image dimensions to all processes
    MPI_Bcast(&im_height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&im_width, 1, MPI_INT, 0, MPI_COMM_WORLD);



    //calculate workload of each process
    L_m = im_height / num_procs;
    rest = im_height % num_procs;

    if(rest != 0) L_m++;

    my_bounds[0] = my_rank * L_m;
    my_bounds[1] = (my_rank + 1) * L_m;

    if(my_rank == num_procs - 1) my_bounds[1] = im_height;

    my_m = my_bounds[1] - my_bounds[0];
    my_n = im_width;
    my_chunk_size = my_m*my_n;

    //buffer array containing each process' chunk size
    int *chunk_sizes = malloc(num_procs*sizeof(*chunk_sizes));

    //local image chars
    unsigned char *my_im_chars = malloc(my_chunk_size*sizeof(my_im_chars));


    //send chunk sizes to master process
    MPI_Gather(&my_chunk_size, 1, MPI_INT, &chunk_sizes[my_rank], 1, MPI_INT, 0, MPI_COMM_WORLD);


    //array of each process' start index in the main im_char array
    int *displ = malloc(num_procs*sizeof(*displ));
    for(int proc = 0; proc < num_procs; proc++){
        displ[proc] = proc*chunk_sizes[proc];
    }

    //distribute individual im_chars to each process
    MPI_Scatterv(im_chars, chunk_sizes, displ, MPI_UNSIGNED_CHAR, my_im_chars, my_chunk_size, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);


    //allocate memory for image matrix and convert to floats
    allocate_image(&u, my_m, my_n);
    allocate_image(&u_bar, my_m, my_n);
    convert_jpeg_to_image(my_im_chars, &u);


    //perform image denoising
    MPI_Barrier(MPI_COMM_WORLD);
    if(my_rank == 0) clock_gettime(CLOCK_REALTIME, &start_time);
    iso_diffusion_denoising_parallel(&u, &u_bar, kappa, iters, my_rank, num_procs);
    if(my_rank == 0) clock_gettime(CLOCK_REALTIME, &end_time);

    convert_image_to_jpeg(my_im_chars, &u);

    MPI_Barrier(MPI_COMM_WORLD);

    //gather image chars from each process
    MPI_Gatherv(my_im_chars, my_chunk_size, MPI_UNSIGNED_CHAR, im_chars, chunk_sizes, displ, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);


    //export denoised image to file
    if(my_rank == 0){
        export_JPEG_file(output_filename, im_chars, im_height, im_width, num_comp, quality);
        time_spent = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;
        printf("Denoised image written to %s\n", output_filename );
        printf("Execution time: %f\n", time_spent);
    }


    //deallocate memory
    deallocate_image(&u);
    deallocate_image(&u_bar);
    free(chunk_sizes);
    free(my_im_chars);
    if(my_rank==0) free(im_chars);

    MPI_Finalize();
    return 0;
}
