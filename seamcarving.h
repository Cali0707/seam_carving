#ifndef SEAM_CARVING_LIBRARY_H
#define SEAM_CARVING_LIBRARY_H

#include <stdlib.h>
#include <stdint.h>


typedef struct rgb_img{
    uint8_t *raster;
    size_t height;
    size_t width;
} rgb_img;

void create_img(rgb_img **im, size_t height, size_t width);
void read_in_img(rgb_img **im, char *filename);
void write_img(rgb_img *im, char *filename);
uint8_t get_pixel(rgb_img *im, int y, int x, int col);
void set_pixel(rgb_img *im, int y, int x, int r, int g, int b);
void destroy_image(rgb_img *im);
void calc_energy(rgb_img *im, rgb_img **grad);
void dynamic_seam(rgb_img *grad, double **best_arr);
void recover_path(double *best, int height, int width, int **path);
void remove_seam(rgb_img *src, rgb_img **dest, int *path);

#endif //SEAM_CARVING_LIBRARY_H
