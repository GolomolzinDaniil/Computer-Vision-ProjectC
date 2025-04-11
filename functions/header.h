#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../libs/stb_image.h"
#include "../libs/stb_image_write.h"
#define uc unsigned char
#define lld long long double
// функции принимающие аргумент
int Median_filter(char* input_file, int area, char* output_file);
int Gauss_filter(char* input_file, int area, char* output_file);
int Edges_detect(char* input_file, int num, char* output_file);
int Turner_alpha(char* input_file, int num, char* output_file);
int ReSize(char* input_file, double resize, char* output_file);
// без аргументные функции
int Turner_left(char* input_file, char* output_file);
int Turner_up (char* input_file, char* output_file);
int Inversion (char* input_file, char* output_file);
int Gradation_grey(char* input_file, char* output_file);
// дополнительные функции (используемые в функциях выше)
int compare(const void* a, const void* b);
int reflect(int coor, int max_size);
double g(double x, double y, double d);
double gx(double x, double d);
double gy(double y, double d);
double newCoorX (double x, double y, double center_x, double center_y, double dgr);
double newCoorY (double x, double y, double center_x, double center_y, double dgr);
double BilinalInterpol (double x, double y, int width, int height, int channels, int ch, uc* image);
double GetNewCoor (int x, int y, double resize, int width, int height, int channels, int ch, uc* image);
#endif
