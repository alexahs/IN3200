#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef struct {
    float** image_data;
    int m;
    int n;
} image;


void pointer_swap(void** a, void** b);
void allocate_image(image *u, int m, int n);
void deallocate_image(image *u);
void convert_jpeg_to_image(const unsigned char *image_chars, image *u);
void convert_image_to_jpeg(unsigned char *image_chars, image *u);
void iso_diffusion_denoising(image *u, image *u_bar, float kappa, int iters);

void import_JPEG_file (const char* filename, unsigned char** image_chars,
    int* image_height, int* image_width,
    int* num_components);

void export_JPEG_file (const char* filename, const unsigned char* image_chars,
    int image_height, int image_width,
    int num_components, int quality);


#endif
