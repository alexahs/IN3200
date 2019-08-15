#include "functions.h"


void pointer_swap(void** a, void** b){
    void* temp;
    temp = *a;
    *a = *b;
    *b = temp;
}



void allocate_image(image *u, int m, int n){
    u->m = m;
    u->n = n;



    u->image_data = (float**)malloc(m*sizeof(float*));
    for (int i = 0; i < m; i++) {
        u->image_data[i] = (float*)malloc(n * sizeof(float));
    }
}

void deallocate_image(image *u){


    for(int i = 0; i < u->m; i++){
        free(u->image_data[i]);
    }
    free(u->image_data);
}


void convert_jpeg_to_image(const unsigned char *image_chars, image *u){

    for(int i = 0; i < u->m; i++){
        for(int j = 0; j < u->n; j++){
            u->image_data[i][j] = (float)image_chars[i*u->n + j];
        }
    }
}

void convert_image_to_jpeg(unsigned char *image_chars, image *u){

    for(int i = 0; i < u->m; i++){
        for(int j = 0; j < u->n; j++){
            image_chars[i*u->n + j] = u->image_data[i][j];
        }
    }
}

void iso_diffusion_denoising(image *u, image *u_bar, float kappa, int iters){


    //set u_bar = u along the boundaries
    for(int i = 0; i < u->m; i++){
        u_bar->image_data[i][0] = u->image_data[i][0];
        u_bar->image_data[i][u->n-1] = u->image_data[i][u->n-1];
    }

    for(int j = 0; j < u->n; j++){
        u_bar->image_data[0][j] = u->image_data[0][j];
        u_bar->image_data[u->m-1][j] = u_bar->image_data[u->m-1][j];
    }


    //denoising sweep
    for(int iter = 0; iter < iters; iter++){
        for(int i = 1; i < u->m - 1; i++){
            for(int j = 1; j < u->n - 1; j++){
                u_bar->image_data[i][j] = u->image_data[i][j] + kappa
                                        *(u->image_data[i+1][j]
                                        + u->image_data[i-1][j]
                                       -4*u->image_data[i][j]
                                        + u->image_data[i][j+1]
                                        + u->image_data[i][j-1]);
            }
        }

        pointer_swap((void*)&u, (void*)&u_bar);

    }//end denoising iterations

    pointer_swap((void*)&u, (void*)&u_bar);

}
