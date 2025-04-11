#include "header.h"


/**
 * @brief Поворот изображения относительно оси Ox (горизонтальное отражение).
 * 
 * Функция выполняет зеркальное отражение изображения относительно горизонтальной оси (оси Ox),
 * т.е. инвертирует координаты по вертикали.
 * 
 * @param input_file Путь к исходному изображению.
 * @param output_file Путь к сохранённому изображению после поворота.
 * 
 * @return int Код возврата: 0 — успешно, 666 — ошибка при чтении файла.
 * 
 * @note Эта функция меняет вертикальное расположение пикселей, оставляя горизонтальное положение неизменным.
 * 
 * @warning При использовании изображения с нечетной высотой может быть небольшая ошибка в выравнивании пикселей.
 */


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
                // индекс оригинальноого изображения
                int fst_ind = (i * width + j) * channels + ch;
                // новый индекс
                int ind1 = ( (height - i) * width + j) * channels + ch;
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