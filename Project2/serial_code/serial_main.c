#include "functions.h"


int main(int argc, char* argv[]){
    /*
    This program performs the iso diffusion denoising algorithm.
    The filename (must be .jpg) is provided as the first command line argument, followed by the
    output filename. The parameter kappa, commonly set to 0.2, must also be provided,
    along with the number of denoising iterations to perform.
    */


    struct timespec start_time, end_time;
    int im_height, im_width, num_comp;
    float kappa = atof(argv[3]);
    int iters = atoi(argv[4]);
    int quality = 75;
    image u, u_bar;
    unsigned char *im_chars;
    char *input_filename, *output_filename;
    input_filename = argv[1];
    output_filename = argv[2];

    if(argc < 5){
        printf("Not enough commandline arguments. Exiting..\n");
        exit(1);
    }


    import_JPEG_file(input_filename, &im_chars, &im_height, &im_width, &num_comp);
    printf("Loaded %s, %dx%d\n", input_filename, im_height, im_width );

    allocate_image(&u, im_height, im_width);
    allocate_image(&u_bar, im_height, im_width);
    convert_jpeg_to_image(im_chars, &u);

    clock_gettime(CLOCK_REALTIME, &start_time);
    iso_diffusion_denoising(&u, &u_bar, kappa, iters);
    clock_gettime(CLOCK_REALTIME, &end_time);

    convert_image_to_jpeg(im_chars, &u);


    export_JPEG_file(output_filename, im_chars,
                           im_height, im_width,
                           num_comp, quality);
    printf("Image written to %s\n", output_filename);



    deallocate_image(&u);
    deallocate_image(&u_bar);
    free(im_chars);


    double time_spent = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;

    printf("Execution time: %f\n", time_spent);

    return 0;
}
