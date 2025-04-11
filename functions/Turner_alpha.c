#include "header.h"


/**
 * @brief Поворот изображения на заданный угол вокруг центра.
 * 
 * Функция выполняет поворот изображения на угол `alpha` (в градусах) 
 * относительно центра изображения с использованием билинейной интерполяции.
 * Координаты, вышедшие за пределы изображения, закрашиваются в чёрный цвет.
 * 
 * @param input_file Путь к исходному изображению.
 * @param alpha Угол поворота в градусах (по часовой стрелке).
 * @param output_file Путь к сохранённому изображению после поворота.
 * 
 * @return int Код возврата: 0 — успешно, 666 — ошибка загрузки изображения.
 * 
 * @note Поворот может обрезать часть изображения, если пиксели оказываются вне исходных границ.
 *       Функция использует вспомогательные методы `newCoorX`, `newCoorY` и `BilinalInterpol`.
 * 
 * @warning Убедитесь, что угол `alpha` корректно интерпретируется в функциях координатного преобразования.
 */


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
        return 666;
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
