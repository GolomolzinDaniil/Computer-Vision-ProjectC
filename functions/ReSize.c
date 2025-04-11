#include "header.h"


/**
 * @brief Масштабирование изображения с коэффициентом увеличения или уменьшения.
 * 
 * Функция изменяет размер изображения согласно переданному коэффициенту `resize`.
 * При уменьшении изображение масштабируется без появления черных краев. При увеличении —
 * оригинальное разрешение сохраняется, и изображение масштабируется внутрь.
 * 
 * @param input_file Путь к входному изображению.
 * @param resize Коэффициент масштабирования (resize < 1 — уменьшение, resize > 1 — увеличение).
 * @param output_file Путь для сохранения изображения после масштабирования.
 * 
 * @return int Код возврата: 0 — успешно, 666 — ошибка загрузки изображения.
 * 
 * @note Для получения значения пикселя вызывается вспомогательная функция `GetNewCoor()`,
 *       предполагается, что она корректно интерполирует значения при изменении размера.
 * 
 * @warning При увеличении изображения может наблюдаться размытость из-за интерполяции.
 *          Убедитесь, что `resize` — положительное число.
 */


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
        return 666;
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
