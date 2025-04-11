#include "header.h"


/**
 * @brief Инвертирование цветов изображения.
 * 
 * Функция выполняет инверсию цветов каждого пикселя изображения по формуле: 
 * `new_value = 255 - original_value`. Применяется ко всем каналам, сохраняя структуру изображения.
 * 
 * @param input_file Путь к исходному изображению.
 * @param output_file Путь для сохранения изображения с инвертированными цветами.
 * 
 * @return int Код возврата: 0 — успешно, 666 — ошибка чтения изображения.
 * 
 * @note Инверсия применяется ко всем каналам изображения, включая альфа-канал, если он есть.
 *       Размер и формат изображения остаются неизменными.
 * 
 * @warning Убедитесь, что путь к изображению указан корректно, и файл доступен для чтения.
 */

 
// инверсия цветов пикселей
int Inversion (char* input_file, char* output_file) {
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
    // массив для измененной картинки
    uc* new_image = (uc*)calloc(width * height * channels, sizeof(uc));
    // по высоте
    for (int i = 0; i < height; i++) {
        // по ширине
        for (int j = 0; j < width; j++) {
            // по каналам
            for (int ch = 0; ch < channels; ch++) {
                // индекс для удобства
                int ind = (i*width + j) * channels + ch;
                // записываем в итоговый массив
                new_image[ind] = 255 - image[ind];
            }
        }
    }
    stbi_write_jpg(output_file, width, height, channels, new_image, 100);
    // чистка памяти
    stbi_image_free(image);
    free(new_image);
    return 0;
}