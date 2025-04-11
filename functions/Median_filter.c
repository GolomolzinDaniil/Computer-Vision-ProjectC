#include "header.h"


/**
 * @brief Применение медианного фильтра для устранения шума "Соль и Перец".
 * 
 * Функция заменяет каждый пиксель изображения на медианное значение среди окружающих пикселей 
 * в окне размером `area x area`, эффективно устраняя импульсный шум, особенно "Соль и Перец".
 * 
 * @param input_file Путь к входному изображению.
 * @param area Размер окна фильтра (нечётное число, например 3, 5, 7 и т.д.).
 * @param output_file Путь для сохранения изображения после обработки.
 * 
 * @return int Код возврата: 0 — успешно, 666 — ошибка чтения изображения.
 * 
 * @note Обработка применяется ко всем каналам изображения отдельно. Размер окна `area` должен быть нечётным.
 *       Для краёв изображения используется отражение индексов (рефлексия), что предотвращает искажение границ.
 * 
 * @warning При слишком больших значениях `area` фильтрация может быть ресурсоёмкой.
 *          Убедитесь, что `area` — положительное нечётное число.
 */

 
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
    // проверка на открытие изображения
    if (image == NULL) {
        printf("============= ERROR READ PICTURE =============");
        return 666;
    }
    // массив для измененной картинки
    uc* new_image = (uc*)calloc(width * height * channels, sizeof(uc));
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
