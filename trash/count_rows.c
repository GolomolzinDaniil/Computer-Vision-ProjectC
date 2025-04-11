#include "header.h"
// определение границ с помощью сверток Собеля (1) или Превита (2) 
int Edges_detect (char* input_file, int num, char* output_file) {
     // ширина узображения в пикселях
    int width;
     // высота
    int height;
     // кол-во каналов
    int channels;
    // картинка в массив цветов
    uc* image     = stbi_load(input_file, &width, &height, &channels, 0);
     // итоговый массив
    uc* new_image = (uc*)calloc(width * height * channels, sizeof(uc));
     // так как свертка всегда (3 на 3)
    int border = 1;
    // оператор Собеля
    int Sobel_x[]  = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    int Sobel_y[]  = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    // оператор Превита
    int Previt_x[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
    int Previt_y[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
    // используемая свертка в зависимости от num
     // горизонтальна свертка
    int Gx[9];
     // вертикальная свертка
    int Gy[9];
    // выбор какого оператора для пользования
    for (int i = 0; i < 9; i++) {
        // оператор Собеля
        if (num == 1) {
            Gx[i] = Sobel_x[i];
            Gy[i] = Sobel_y[i];
        }
        // оператор Превита
        else if (num == 2) {
            Gx[i] = Previt_x[i];
            Gy[i] = Previt_y[i];
        }
        // иной случай
        else {
            fprintf(stderr, "======= NOT VALID DATA IN TERMINAL");
            return 1;
        }
    }
    // перевод изображения в градации серого (чтобы граница была не цветная)
     // индексация
    int ind0 = 0;
    // проход по ппикселям изображения
     // по высоте
    for (int i = 0; i < height; i++) {
         // по ширине
        for (int j = 0; j < width; j++) {
            int sum = 0;
             // по каналам (1 проход)
            for (int ch = 0; ch < channels; ch++) {
                int ind1 = (i*width + j) * channels + ch;
                sum += image[ind0];
            }
            // (2 проход) чтоб записать среднее арифметическое
            for (int ch = 0; ch < channels; ch++) { 
                image[ind0++] = sum / channels;
            }
        }
    }
    // проход по пикселям изображения
     // по высоте 
    for (int i = 0; i < height; i++) { 
         // по ширине  
        for (int j = 0; j < width; j++) {  
             // по каналам
            for (int ch = 0; ch < channels; ch++) {
                int ind0 = 0;
                // суммы
                int sum_x = 0;
                int sum_y = 0;
                // индексы
                int ind_x = 0;
                int ind_y = 0;
                for (int around_i = -border; around_i <= border; around_i++) {
                    for (int around_j = -border; around_j <= border; around_j++) {
                        int ind_i = reflect(i + around_i, height);
                        int ind_j = reflect(j + around_j, width);                       
                        int ind1 = (ind_i * width + ind_j) * channels + ch;
                        sum_x += image[ind1] * Gx[ind_x++];
                        sum_y += image[ind1] * Gy[ind_y++];
                    }
                }
                int curr = sqrt(sum_x * sum_x + sum_y * sum_y);
                if (curr > 90) {
                    new_image[(i*width + j) * channels + ch] = 255;
                }
                else {
                    new_image[(i*width + j) * channels + ch] = 0;
                }                 
            }
        }
    }
    stbi_write_jpg(output_file, width, height, channels, new_image, 100);
    // чистка памяти
    stbi_image_free(image);
    free(new_image);
    return 0;
}
#include "header.h"
// фильтр для образования размытия, путем Гауссовых сверток
int Gauss_filter (char* input_file, int area, char* output_file) {
    // отвечает за резкость размытия
    int delta = area / 2; 
    // ширина узображения в пикселях
    int width; 
    // высота
    int height;
     // кол-во каналов 
    int channels;
     // картинка в массив цветов
    uc* image = stbi_load(input_file, &width, &height, &channels, 0);
     // массив для измененной картинки
    uc* new_image = (uc*)calloc(width * height * channels, sizeof(uc));
     // временный массив 
    uc* curr_image= (uc*)calloc(width * height * channels, sizeof(uc)); 
    // проверка на правильность чтения файла
    if (image == NULL) {
        printf("============= ERROR READ PICTURE =============\n");
        return 666;
    }
                            /*СОЗДАНИЕ И ЗАПОЛНЕНИЕ ГОРИЗОНТАЛЬНОЙ И ВЕРТИКАЛЬНОЙ СВЕРТКИ*/
    // свертки
     // горизонтальная свертка
    double Gx[area];
     // вертикальная светрка 
    double Gy[area];
    // для нормирования сверток
    double sum_x = 0;
    double sum_y = 0;
    int border = (area - 1) / 2; // длина края в пикселях от главного пикселя
    // заполнение горизонтальной свертки
    for (int j0 = -border, ind0 = 0; j0 <= border; j0++, ind0++) {
        // элемент свертки (ее значение)
        double val_gx = gx(j0, delta);
        // заполнение свертки значениями
        Gx[ind0] = val_gx;
        // увеличиваем сумму значений ядра
        sum_x += val_gx;
    }
    // заполнение вертикальной свертки
    for (int i0 = -border, ind0 = 0; i0 <= border; i0++, ind0++) {
        // элемент свертки (ее значение)
        double val_gy = gy(i0, delta);
        // заполнение свертки значениями
        Gy[ind0] = val_gy;
        // увеличиваем сумму значений ядра
        sum_y += val_gy;
    }
    // нормирование сверток (чтобы не изменялась яркость изображения)
    for (int i = 0; i < area; i++) {
        // нормируем
        Gx[i] /= sum_x;
        Gy[i] /= sum_y;
    }
    // проход по пикселям изображения горизонтально
     // по высоте
    for (int i = 0; i < height; i++) {
         // по ширине      
        for (int j = 0; j < width; j++) {  
             // по каналам       
            for (int ch = 0; ch < channels; ch++) {
                 // сумма элементов Gx * пиксель 
                double sum0 = 0;                           
                // проход по горизонтали
                for (int around_j = -border, ind1 = 0; around_j <= border; around_j++, ind1++) {
                    // отражение пикселей
                    int ind_j = reflect(j + around_j, width);
                    // увеличивание суммы
                    sum0 += image[(i * width + ind_j) * channels + ch] * Gx[ind1];
                }         
                 // индекс главного пикселя
                int ind = (i * width + j) * channels + ch;
                 // запись во временный массив
                curr_image[ind] = (int)(sum0);
            }
        }
    }
    // проход по пикселям изображения вертикально
     // по высоте 
    for (int i = 0; i < height; i++) {
         // по ширине   
        for (int j = 0; j < width; j++) {     
             // по каналам  
            for (int ch = 0; ch < channels; ch++) {
                 // сумма элементов Gy * пиксель 
                double sum1 = 0;        
                // проход по вертикали
                for (int around_i = -border, ind2 = 0; around_i <= border; around_i++, ind2++) {
                    // отражение пикселей
                    int ind_i = reflect(i + around_i, height);
                    // увеличивание суммы
                    sum1 += curr_image[(ind_i * width + j) * channels + ch] * Gy[ind2];
                }        
                 // индекс главного пикселя
                int ind = (i * width + j) * channels + ch;
                 // запись в итоговый массив пикселей
                new_image[ind] = (int)(sum1);
            }
        }
    }
    // создание изображения после замены битов
    stbi_write_jpg(output_file, width, height, channels, new_image, 100);
    // чистка памяти
    stbi_image_free(image);
    free(new_image);
    free(curr_image);
    return 0;
}
#include "header.h"
// перевод цветного изображения в градацию серого (кол-во каналов неизменно)
int Gradation_grey (char* input_file, char* output_file) {
     // ширина узображения в пикселях
    int width;
     // высота
    int height;
     // кол-во каналов
    int channels;
    // картинка в массив цветов
    uc* image     = stbi_load(input_file, &width, &height, &channels, 0);
    // массив для измененной картинки
    uc* new_image = (uc*)calloc(width * height * channels, sizeof(uc));
    // счетчик индекса
    int ind0 = 0;
    // по высоте
    for (int i = 0; i < height; i++) {
        // по ширине
        for (int j = 0; j < width; j++) {
            // итоговая сумма
            int sum = 0;
            // по каналам
            for (int ch = 0; ch < channels; ch++) {
                // для удобства записи
                int ind1 = (i*width + j) * channels + ch;
                // увеличиваем сумму
                sum += image[ind0];
            }
            // снова проходим по каналам
            for (int ch = 0; ch < channels; ch++) {
                // записываем среднее арифметическое
                new_image[ind0++] = sum / channels;
            }
        }
    }
    stbi_write_jpg(output_file, width, height, channels, new_image, 100);
    // чистка памяти
    stbi_image_free(image);
    free(new_image);
    return 0;
}
#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stb_image.h"
#include "stb_image_write.h"
#define uc unsigned char
#define lld long long double
// функции принимающие аргумент
int Median_filter(char* input_file, int area, char* output_file);
int Gauss_filter(char* input_file, int area, char* output_file);
int Edges_detect(char* input_file, int num, char* output_file);
int Turner_alpha(char* input_file, int num, char* output_file);
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
#endif
#include "header.h"
#define STBI_NO_SIMD  // отключаем так как SSE2 не работает с tcc
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
// отражение окружных пикселей (обработка границ изображения)
int reflect (int coor, int max_size) {
    if (coor < 0) { 
        return -coor;
    }
    else if (coor >= max_size) { 
        return 2 * max_size - 2 - coor;
    }
    else {
        return coor;
    }
}
// cравнение двух элементов
int compare(const void* a, const void* b) {
    return (*(uc*)a - *(uc*)b);
}
// функция для Ядра Гаусса (двумерная)
double g(double x, double y, double d) {
    return ( ( 1 / ( 2 * M_PI * d * d ) ) * exp ( ( x * x + y * y ) / ( -2 * d * d ) ) );
}
// одномерная функция для X для горизонтальной свертки
double gx(double x, double d) {
    return ( (1 / sqrt ( 2 * M_PI * d * d ) ) * exp ( ( x * x ) / ( -2 * d * d ) ) );
}
// одномерная функция для Y для вертикальной свертки
double gy(double y, double d) {
    return ( (1 / sqrt ( 2 * M_PI * d * d ) ) * exp ( ( y * y ) / ( -2 * d * d ) ) );
}
// возвращает новые координаты по X (поворот относительно центра изображения)
double newCoorX (double x, double y, double center_x, double center_y, double dgr) {
    // так как мы поворачиваем относительно центра избражения
    x -= center_x;
    y -= center_y;
    // перевод в радианы
    double new_dgr = dgr * M_PI / 180.0;
    // снова прибавляем, возвращая на предыдущее место
    return (x * cos(new_dgr) - y * sin(new_dgr) + center_x);
}
// возвращает новые координаты по Y (поворот относительно центра изображения)
double newCoorY (double x, double y, double center_x, double center_y, double dgr) {
    // так как мы поворачиваем относительно центра избражения
    x -= center_x;
    y -= center_y;
    // перевод в радианы
    double new_dgr = dgr * M_PI / 180.0;
    // снова прибавляем, возвращая на предыдущее место
    return (x * sin(new_dgr) + y * cos(new_dgr) + center_y);
}
// билинейная интерполяция усредняющая значение пикселя по приращению (расстоянию) и весу пикселя
double BilinalInterpol (double x, double y, int width, int height, int channels, int ch, uc* image) {
    // нижняя граница    // верхняя граница     // приращение дробной и целой
    int x_down = (int)x;
    int x_up = x_down + 1;
    double dx = x - x_down;
    int y_down = (int)y;
    int y_up = y_down + 1;
    double dy = y - y_down;
    // проверки на выход ха границы (если вышло, то берем крайнее значение)
    x_up   = (x_up   < 0) ? 0 : (x_up   >=  width) ? width - 1  : x_up;
    x_down = (x_down < 0) ? 0 : (x_down >=  width) ? width - 1  : x_down;
    y_up   = (y_up   < 0) ? 0 : (y_up   >= height) ? height - 1 : y_up;
    y_down = (y_down < 0) ? 0 : (y_down >= height) ? height - 1 : y_down;
    // взятие весов крайних пикселей
    int p11 = image[(y_down * width + x_down  ) * channels + ch]; // левый нижний
    int p21 = image[(y_down * width + x_up    ) * channels + ch]; // правый нижний
    int p12 = image[(y_up   * width + x_down  ) * channels + ch]; // левый верхний
    int p22 = image[(y_up   * width + x_up    ) * channels + ch]; // правый вехний
    // усредненное значение пикселя
    double curr_res = (1-dx)*(1-dy)*p11 + dx*(1-dy)*p21 + (1-dx)*dy*p12 + dx*dy*p22;
    // return ( (curr_res < 0) ? 0 : (curr_res > 255) ? 255 : curr_res; // вероятно лишнее
    return curr_res;
}
// получение новых координат при масштабировании
double GetNewCoor (int x, int y, double resize, int width, int height, int channels, int ch, uc* image) {
    // дробные новые координаты (относительно центра)
    double half_width = width / 2.0, half_height = height / 2.0;
    double new_x = (x - half_width) / resize + half_width, new_y = (y - half_height) / resize + half_height;
    // так как нам не нужны увеличенные одинаковыми пикселями края
    if (new_x < 0 || new_x >= width || new_y < 0 || new_y >= height) { return 0; }
    // нижняя граница        // верхняя граница     // приращение дробной и целой
    int x_down = (int)new_x;
    int x_up = x_down + 1;
    double dx = new_x - x_down;
    int y_down = (int)new_y;
    int y_up = y_down + 1;
    double dy = new_y - y_down;
    if (x_up < 0 || x_up >= width || x_down < 0 || x_down >= width
        || y_up < 0 || y_up >= height || y_down < 0 || y_down >= height) { return 0; }
    int p11 = image[(y_down * width + x_down) * channels + ch];
    int p21 = image[(y_down * width + x_up  ) * channels + ch];
    int p12 = image[(y_up * width + x_down  ) * channels + ch];
    int p22 = image[(y_up * width + x_up    ) * channels + ch];
    return ( (1-dx)*(1-dy)*p11 + dx*(1-dy)*p21 + (1-dx)*dy*p12 + dx*dy*p22 );
}
#include "header.h"
// инверсия цветов пикселей
int Inversion (char* input_file, char* output_file) {
    int width; // ширина узображения в пикселях
    int height; // высота
    int channels; // кол-во каналов
    uc* image     = stbi_load(input_file, &width, &height, &channels, 0); // картинка в массив цветов
    uc* new_image = (uc*)calloc(width * height * channels, sizeof(uc)); // массив для измененной картинки
    for (int ind = 0; ind < width * height * channels; ind++) {
        // запись обратного пикселя нынешнему
        new_image[ind] = 255 - image[ind];
    }
    stbi_write_jpg(output_file, width, height, channels, new_image, 100);
    // чистка памяти
    stbi_image_free(image);
    free(new_image);
    return 0;
}
#include "header.h"
int main(int argc, char* argv[]) {
    if (argc < 4 && 5 > argc) {
        fprintf(stderr, "============= NOT VALID DATA IN TERMINAL =============\n");
        return 1;
    }
    else {
         // входной файл
        char* input_file  = argv[1];
         // тип фильтра
        char* op          = argv[2];
         // обработка функций без аргумента
        if (argc == 4) {
             // выходной файл
            char* output_file = argv[3];
            // градация серого цвета
            if      (strcmp(op, "-grgrey"   ) == 0) {
                return Gradation_grey (input_file, output_file);
            }
            // инверсия цветов
            else if (strcmp(op, "-invers"   ) == 0) {
                return Inversion (input_file, output_file);
            }
            // поворот относительно Оу
            else if (strcmp(op, "-leftturn" ) == 0) {
                return Turner_left (input_file, output_file);
            }
            // поворот относительно Ох
            else if (strcmp(op, "-upturn"   ) == 0) {
                return Turner_up (input_file, output_file);
            }
        }
         // обработка функций с аргументом
        else if (argc == 5) {
             // дополнительное значение
            int val           = atoi(argv[3]);
             // выходной файл
            char* output_file = argv[4];
            // Медианный фильтр
            if      (strcmp(op, "-median"   ) == 0) {
                return Median_filter (input_file, val, output_file);
            }
            // Гауссов фильтр
            else if (strcmp(op, "-gauss"    ) == 0) {
                return Gauss_filter (input_file, val, output_file);
            }
            // определение границ
            else if (strcmp(op, "-edges"    ) == 0) {
                return Edges_detect (input_file, val, output_file);
            }
            // поворот на угол alpha
            else if (strcmp(op, "-alphaturn") == 0) {
                return Turner_alpha (input_file, val, output_file);
            }           
        }
        // иной случай
        else {
            fprintf(stderr, "============= UNKNOWN FILTER TYPE =============\n");
            return 1;
        }
    }
}
#include "header.h"
// Медианный фильтр, направленный для исправления шума Соль и Перец
int Median_filter (char* input_file, int area, char* output_file) {
     // ширина узображения в пикселях
    int width;
     // высота
    int height;
     // кол-во каналов
    int channels;
    // картинка в массив цветов
    uc* image     = stbi_load(input_file, &width, &height, &channels, 0); 
     // массив для измененной картинки
    uc* new_image = (uc*)calloc(width * height * channels, sizeof(uc));
    // проверка на открытие изображения
    if (image == NULL) {
        printf("============= ERROR READ PICTURE =============");
        return 666;
    }
     // длина края в пикселях от главного пикселя (радиус)
    int border = (area - 1) / 2;
    // проход по пикселям изображения
     // по высоте
    for (int i = 0; i < height; i++) {
         // по ширине
        for (int j = 0; j < width; j++) {
             // по каналам
            for (int ch = 0; ch < channels; ch++) {
                 // индекс результирующего изображения
                int ind0 = 0;
                 // сменяемый массив окружных пикселей
                uc arr_pixel[area*area];
                // обработка окружных пикселей
                 // по высоте диаметра
                for (int around_i = -border; around_i <= border; around_i++) {
                     // по ширине диаметра
                    for (int around_j = -border; around_j <= border; around_j++) {
                         // левый и верхний край
                        int ind_i = reflect(i + around_i, height);
                         // правый и нижний край
                        int ind_j = reflect(j + around_j, width);
                         // индекс итогового пикселя
                        int ind1 = (ind_i * width + ind_j) * channels + ch;
                         // берем пиксель по индексу
                        arr_pixel[ind0++] = image[ind1];
                    }
                }
                 // сортируем по возрастанию
                qsort(arr_pixel, ind0, sizeof(uc), compare);
                 // записываем медианный элемент
                new_image[(i * width + j) * channels + ch] = arr_pixel[(ind0 - 1) / 2];
            }
        }
    }
    // создание изображения после применения фильтра
    stbi_write_jpg(output_file, width, height, channels, new_image, 100);
    // чистка памяти
    stbi_image_free(image);
    free(new_image);
    return 0;
}
#include "header.h"
// изменения масштабирования изображения (<1 - уменьшение, >1 - увеличение)
int ReSize (char* input_file, double resize, char* output_file) {
     // ширина узображения в пикселях
    int width;
     // высота
    int height;
     // кол-во каналов
    int channels;
     // картинка в массив цветов
    uc* image     = stbi_load(input_file, &width, &height, &channels, 0);
     // массив для измененной картинки
    uc* new_image = (uc*)calloc(width * height * channels, sizeof(uc));
    // проверка на открытие изображения
    if (image == NULL) {
        printf("============= ERROR READ PICTURE =============\n");
        return 13;
    }
    // уменьшенная ширина
    int width_less  = (int)(width * resize);
    // уменьшенная высота
    int height_less = (int)(height * resize);
    // результирующий массив пикселей
    uc* result_image = (uc*)calloc(width_less * height_less * channels, sizeof(uc));
    // счетчик
    int count = 0;
    // по высоте
    for (int i = 0; i < height; i++) {
        // по ширине
        for (int j = 0; j < width; j++) {
            // по каналам
            for (int ch = 0; ch < channels; ch++) {
                // для удобства записи
                int ind0 = (i * width + j) * channels + ch;
                // получение значения пикселя
                uc val = (uc)GetNewCoor(j, i, resize, width, height, channels, ch, image);
                // если мы не хотим видеть черные края
                int curr0 = (height - height * resize) / 2;
                int curr1 = (width - width * resize) / 2;
                if (curr0 <= i && i < height - curr0 && curr1 <= j && j < width - curr1) {
                    result_image[count++] = val;
                }
            }
        }
    }
    // создание изображения после применения фильтра
    // если мы не хотим видеть черные края
    if (resize < 1) {
        stbi_write_jpg(output_file, width_less, height_less, channels, result_image, 100);
    }
    else {
        stbi_write_jpg(output_file, width, height, channels, result_image, 100);
    }
    // чистка памяти
    stbi_image_free(image);
    free(new_image);
    return 0;
}
#include "header.h"
// поворот изображение на alpha угол, относительно центра изображения
int Turner_alpha (char* input_file, int alpha, char* output_file) {
    // ширина узображения в пикселях
    int width; 
     // высота
    int height;
     // кол-во каналов
    int channels;
     // картинка в массив цветов
    uc* image     = stbi_load(input_file, &width, &height, &channels, 0);
     // массив для измененной картинки
    uc* new_image = (uc*)calloc(width * height * channels, sizeof(uc));
    // проверка на открытие изображения
    if (image == NULL) {
        printf("============= ERROR READ PICTURE =============\n");
        return 13;
    }
    // проход по пикселям изображения
     // по высоте
    for (int i = 0; i < height; i++) {
         // по ширине
        for (int j = 0; j < width; j++) {
             // по каналам
            for (int ch = 0; ch < channels; ch++) {
                // дробные координаты нового изображения
                double doubleNewX = newCoorX(j, i, width / 2.0, height / 2.0, alpha);
                double doubleNewY = newCoorY(j, i, width / 2.0, height / 2.0, alpha);
                // целая часть дробных
                int ind_x = (int)doubleNewX;
                int ind_y = (int)doubleNewY;
                int ind0 = (i * width + j) * channels + ch;
                // проверка на валидность координат
                if (ind_x >= 0 && ind_x < width && ind_y >= 0 && ind_y < height) {
                    uc val = (uc)BilinalInterpol (doubleNewX, doubleNewY, width, height, channels, ch, image);
                    new_image[ind0] = val;
                }
                 // если получилось так, что координаты вышли за пределы 
                else {              
                    new_image[ind0] = 0;
                }
            }
        }
    }
    // создание изображения после применения фильтра
    stbi_write_jpg(output_file, width, height, channels, new_image, 100);
    // чистка памяти
    stbi_image_free(image);
    free(new_image);
    return 0;
}
#include "header.h"
// поворот изображения относительно координат оси Oy
int Turner_left (char* input_file, char* output_file) {
     // ширина узображения в пикселях
    int width;
     // высота
    int height;
     // кол-во каналов
    int channels;
     // картинка в массив цветов
    uc* image     = stbi_load(input_file, &width, &height, &channels, 0);
     // массив для измененной картинки
    uc* new_image = (uc*)calloc(width * height * channels, sizeof(uc));
    // по высоте
    for (int i = 0; i < height; i++) {
        // по ширине
        for (int j = 0; j < width; j++) {
            // по каналам
            for (int ch = 0; ch < channels; ch++) {
                // индекс оригинальноого изображения
                int fst_ind = (i * width + j) * channels + ch;
                // новый индекс
                int ind1 = ( (i * width ) + 2 * width - j + 2) * channels + ch;
                // запись
                new_image[fst_ind] = image[ind1]; 
            }
        }
    }
    stbi_write_jpg(output_file, width, height, channels, new_image, 100);
    // чистка памяти
    stbi_image_free(image);
    free(new_image);
    return 0;
}
#include "header.h"
// поворот изображения относительно координат оси Ox
int Turner_up (char* input_file, char* output_file) {
     // ширина узображения в пикселях
    int width;
     // высота
    int height;
     // кол-во каналов
    int channels;
     // картинка в массив цветов
    uc* image     = stbi_load(input_file, &width, &height, &channels, 0);
     // массив для измененной картинки
    uc* new_image = (uc*)calloc(width * height * channels, sizeof(uc));
    // по высоте
    for (int i = 0; i < height; i++) {
        // по ширине
        for (int j = 0; j < width; j++) {
            // по каналам
            for (int ch = 0; ch < channels; ch++) {
                // индекс оригинальноого изображения
                int fst_ind = (i * width + j) * channels + ch;
                // новый индекс
                int ind1 = ( (height - i) * width + j) * channels + ch;
                // запись
                new_image[fst_ind] = image[ind1];           
            }
1        }
    }
    stbi_write_jpg(output_file, width, height, channels, new_image, 100);
    // чистка памяти
    stbi_image_free(image);
    free(new_image);
    return 0;
}
gcc -c Median_filter.c Gauss_filter.c Edges_detect.c Gradation_grey.c helper_func.c Inversion.c Turner_alpha.c Turner_left.c Turner_up.c
ar rcs mylib.a Median_filter.o Gauss_filter.o Edges_detect.o Gradation_grey.o helper_func.o Inversion.o Turner_alpha.o Turner_left.o Turner_up.o.
rm *.o
gcc main.c mylib.a -o test.exe
        @REM Градация серого
@REM test b2.jpg -grgrey a1_result.jpg
@REM test b5.jpg -grgrey a2_result.jpg
@REM test p2.jpg -grgrey a3__result.jpg
        @REM Инверсия цветов
@REM test b1.jpg -invers b1_result.jpg
@REM test p1.jpg -invers b2_result.jpg
@REM test p2.jpg -invers b3_result.jpg
        @REM Поворот относительно Ox
@REM test p3.jpg -leftturn c1_result.jpg
@REM test p4.jpg -leftturn c2_result.jpg
@REM test p5.jpg -leftturn c3_result.jpg
        @REM Поворот относительно Oy
@REM test p1.jpg -upturn d1_result.jpg
@REM test p3.jpg -upturn d2_result.jpg
@REM test p5.jpg -upturn d3_result.jpg
        @REM Медианный фильтр
@REM test m0.jpg -median 3 e1_result.jpg
@REM test m1.jpg -median 3 e2_result.jpg
@REM test m2.jpg -median 5 e3_result.jpg
        @REM Гауссов фильтр
@REM test g0.jpg -gauss 3 f1_result.jpg
@REM test g1.jpg -gauss 5 f2_result.jpg
@REM test g2.jpg -gauss 3 f3_result.jpg
        @REM Определение границ
@REM test b1.jpg -edges 2 g1_result.jpg
@REM test b2.jpg -edges 1 g2_result.jpg
@REM test b3.jpg -edges 1 g3_result.jpg
        @REM Поворот изображения
@REM test b5.jpg -alphaturn 45 h1_result.jpg
@REM test p3.jpg -alphaturn 74 h2_result.jpg
@REM test p5.jpg -alphaturn 120 h3_result.jpg
    @REM Измененеия масштаба изображения
test b2.jpg -resize 
test p3.jpg -resize 
test g1.jpg -resize 
    