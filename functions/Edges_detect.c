#include "header.h"


/**
 * @brief Обнаружение границ изображения с использованием оператора Собеля или Превита.
 * 
 * Выполняет детектирование границ на изображении, применяя фильтр Собеля (если num == 1)
 * или Превита (если num == 2). Изображение сначала преобразуется в градации серого, 
 * после чего применяется свёртка для выделения границ.
 * 
 * @param input_file Путь к входному изображению (формат поддерживаемый stbi, например JPG/PNG).
 * @param num Выбор оператора: 1 — Собель, 2 — Превит.
 * @param output_file Путь для сохранения выходного изображения с выделенными границами.
 * 
 * @return int Код возврата: 0 — успешно, 1 — неверный параметр num, 666 — ошибка загрузки изображения.
 * 
 * @note Преобразование изображения в оттенки серого осуществляется усреднением всех каналов.
 *       Пороговое значение для определения границ жёстко задан (90).
 * 
 * @warning Убедитесь, что путь к изображению указан корректно, и файл существует.
 */


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
    // проверка на правильность чтения файла
    if (image == NULL) {
        printf("============= ERROR READ PICTURE =============\n");
        return 666;
    }
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
