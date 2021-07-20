#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <math.h>
#include "seamcarving.h"


void create_img(rgb_img **im, size_t height, size_t width){
    *im = (rgb_img *)malloc(sizeof(rgb_img));
    (*im)->height = height;
    (*im)->width = width;
    (*im)->raster = (uint8_t *)malloc(3 * height * width);
}


int read_2bytes(FILE *fp){
    uint8_t bytes[2];
    fread(bytes, sizeof(uint8_t), 1, fp);
    fread(bytes+1, sizeof(uint8_t), 1, fp);
    return (  ((int)bytes[0]) << 8)  + (int)bytes[1];
}

void write_2bytes(FILE *fp, int num){
    uint8_t bytes[2];
    bytes[0] = (uint8_t)((num & 0XFFFF) >> 8);
    bytes[1] = (uint8_t)(num & 0XFF);
    fwrite(bytes, 1, 1, fp);
    fwrite(bytes+1, 1, 1, fp);
}

void read_in_img(rgb_img **im, char *filename){
    FILE *fp = fopen(filename, "rb");
    size_t height = read_2bytes(fp);
    size_t width = read_2bytes(fp);
    create_img(im, height, width);
    fread((*im)->raster, 1, 3*width*height, fp);
    fclose(fp);
}

void write_img(rgb_img *im, char *filename){
    FILE *fp = fopen(filename, "wb");
    write_2bytes(fp, (int)im->height);
    write_2bytes(fp, (int)im->width);
    fwrite(im->raster, 1, im->height * im->width * 3, fp);
    fclose(fp);
}

uint8_t get_pixel(rgb_img *im, int y, int x, int col){
    return im->raster[3 * (y*(im->width) + x) + col];
}

void set_pixel(rgb_img *im, int y, int x, int r, int g, int b){
    im->raster[3 * (y*(im->width) + x) + 0] = r;
    im->raster[3 * (y*(im->width) + x) + 1] = g;
    im->raster[3 * (y*(im->width) + x) + 2] = b;
}

void destroy_image(rgb_img *im)
{
    free(im->raster);
    free(im);
}


void print_grad(rgb_img *grad){
    int height = (int)grad->height;
    int width = (int)grad->width;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            printf("%d\t", get_pixel(grad, i, j, 0));
        }
        printf("\n");
    }
}

double minArray(const double *arr, int len){
    double min = arr[0];
    for(int i = 0; i<len; i++){
//        printf("array: %f\n", arr[i]);
        if(arr[i] < min){
            min = arr[i];
        }
    }
    return min;
}

int findMinIndex(const double *arr, int len){
    int i;
    int index = -1;
    double min = INFINITY;
    for(i = 0; i<len; i++){
        if(arr[i] < min){
            min = arr[i];
            index = i;
        }
    }
    return index;
}

uint8_t calc_pixel(struct rgb_img *im, int y, int x){
    // should be double, not double
    double rx, gx, bx, ry, gy, by;
    if (x == 0){
        rx = (double)(im->raster[3 * (y*(im->width) + (x + 1)) + 0] -
                      (double)im->raster[3 * (y*(im->width) + (im->width - 1)) + 0]);
        gx = (double)(im->raster[3 * (y*(im->width) + (x + 1)) + 1] -
                      (double)im->raster[3 * (y*(im->width) + (im->width - 1)) + 1]);
        bx = (double)(im->raster[3 * (y*(im->width) + (x + 1)) + 2] -
                      (double)im->raster[3 * (y*(im->width) + (im->width - 1)) + 2]);
    }
    else if (x == im->width - 1){
        rx = (double)(im->raster[3 * (y*(im->width) + 0) + 0] -
                      (double)im->raster[3 * (y*(im->width) + (x - 1)) + 0]);
        gx = (double)(im->raster[3 * (y*(im->width) + 0) + 1] -
                      (double)im->raster[3 * (y*(im->width) + (x - 1)) + 1]);
        bx = (double)(im->raster[3 * (y*(im->width) + 0) + 2] -
                      (double)im->raster[3 * (y*(im->width) + (x - 1)) + 2]);
    }
    else{
        rx = (double)(im->raster[3 * (y*(im->width) + (x + 1)) + 0] -
                      (double)im->raster[3 * (y*(im->width) + (x - 1)) + 0]);
        gx = (double)(im->raster[3 * (y*(im->width) + (x + 1)) + 1] -
                      (double)im->raster[3 * (y*(im->width) + (x - 1)) + 1]);
        bx = (double)(im->raster[3 * (y*(im->width) + (x + 1)) + 2] -
                      (double)im->raster[3 * (y*(im->width) + (x - 1)) + 2]);
    }

    if (y == 0){
        ry = (double)(im->raster[3 * ((y + 1)*(im->width) + x) + 0] -
                      (double)im->raster[3 * ((im->height - 1)*(im->width) + x) + 0]);
        gy = (double)(im->raster[3 * ((y + 1)*(im->width) + x) + 1] -
                      (double)im->raster[3 * ((im->height - 1)*(im->width) + x) + 1]);
        by = (double)(im->raster[3 * ((y + 1)*(im->width) + x) + 2] -
                      (double)im->raster[3 * ((im->height - 1)*(im->width) + x) + 2]);
    }
    else if (y == im->height - 1){
        ry = (double)(im->raster[3 * (0*(im->width) + x) + 0] -
                      (double)im->raster[3 * ((y - 1)*(im->width) + x) + 0]);
        gy = (double)(im->raster[3 * (0*(im->width) + x) + 1] -
                      (double)im->raster[3 * ((y - 1)*(im->width) + x) + 1]);
        by = (double)(im->raster[3 * (0*(im->width) + x) + 2] -
                      (double)im->raster[3 * ((y - 1)*(im->width) + x) + 2]);
    }
    else{
        ry = (double)(im->raster[3 * ((y + 1)*(im->width) + x) + 0] -
                      (double)im->raster[3 * ((y - 1)*(im->width) + x) + 0]);
        gy = (double)(im->raster[3 * ((y + 1)*(im->width) + x) + 1] -
                      (double)im->raster[3 * ((y - 1)*(im->width) + x)  + 1]);
        by = (double)(im->raster[3 * ((y + 1)*(im->width) + x) + 2] -
                      (double)im->raster[3 * ((y - 1)*(im->width) + x) + 2]);
    }

    double deltaX = pow(rx, 2.0) + pow(gx, 2.0) + pow(bx, 2.0);
    double deltaY = pow(ry, 2.0) + pow(gy, 2.0) + pow(by, 2.0);
    double energy = sqrt(deltaX + deltaY);
    return (uint8_t)(energy / 10.0);
}

void calc_energy(struct rgb_img *im, struct rgb_img **grad){
    *grad = (struct rgb_img *)malloc(sizeof(struct rgb_img));
    (*grad)->height = im->height;
    (*grad)->width = im->width;
    (*grad)->raster = (uint8_t *)malloc(3 * (*grad)->height * (*grad)->width);
    for(int i = 0; i < (*grad)->height; i++){
        for(int j = 0; j < (*grad)->width; j++){
            uint8_t energy = calc_pixel(im, i, j);
            (*grad)->raster[3 * (i*(im->width) + j) + 0] = energy;
            (*grad)->raster[3 * (i*(im->width) + j) + 1] = energy;
            (*grad)->raster[3 * (i*(im->width) + j) + 2] = energy;
        }
    }
}

void dynamic_seam(struct rgb_img *grad, double **best_arr){
    int h,w;
    int height = grad->height;
    int width = (int)(grad->width) * 3;
//    printf("%zu\n", grad->width);
    (*best_arr) = malloc(sizeof(double)*height*width);
    for(h = 0; h<width; h = h + 3){
        (*best_arr)[(int)(h/3)] = (double)grad->raster[h];
    }
    for(h = width; h<(width*height); h = h + (width)){
        for(w = 0; w<width; w = w + 3){
            if(w == 0){
                (*best_arr)[(int)(h/3)] = (double)grad->raster[h] + minArray(&(*best_arr)[(int)((h-width)/3)], 2);
            }else if(w == width - 3){
                (*best_arr)[(int)((h+w)/3)] = (double)grad->raster[h+w] + minArray(&(*best_arr)[(int)((h-width+w)/3)-1], 2);
            }
            else{
                (*best_arr)[(int)((h+w)/3)] = (double)grad->raster[h+w] + minArray(&(*best_arr)[(int)((h-width+w)/3)-1], 3);
            }
        }
    }
}

void recover_path(double *best, int height, int width, int **path){
    int h;
    (*path) = malloc(sizeof(int)*height);
    (*path)[height-1] = findMinIndex(&(best[(height-1)*width]), width);
    for(h = height - 2; h>=0; h = h-1){
        int prev = (*path)[h+1];
        if(prev == 0){
            (*path)[h] = prev + findMinIndex(&(best[h*width]), 2);
        }else if(prev == width-1){
            (*path)[h] = prev -1 + findMinIndex(&(best[(h*width) + prev - 1]), 2);
        }else{
            (*path)[h] = prev -1 + findMinIndex(&(best[(h*width) + prev - 1]), 3);
        }
    }
}

void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path){
    *dest = (struct rgb_img *)malloc(sizeof(struct rgb_img));
    (*dest)->height = src->height;
    (*dest)->width = src->width - 1;
    (*dest)->raster = (uint8_t *)malloc(3 * (*dest)->height * (*dest)->width);
    int pathInd = 0;
    int destInd = -1;
    for(int i = 0; i < src->height; i++){
        for(int j = 0; j < src->width; j++){
            if (j != path[pathInd]){
                (*dest)->raster[++destInd] = src->raster[3 * (i*(src->width) + j) + 0];
                (*dest)->raster[++destInd] = src->raster[3 * (i*(src->width) + j) + 1];
                (*dest)->raster[++destInd] = src->raster[3 * (i*(src->width) + j) + 2];
            }
        }
        pathInd++;
    }
}

