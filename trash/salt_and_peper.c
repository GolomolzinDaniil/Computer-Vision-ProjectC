#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_NO_SIMD  // Отключает использование SSE2 (ускорителя)
#include "stb_image.h"
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {

    srand(time(NULL)); // Инициализация генератора
    int width, height, channels;
    unsigned char *image = stbi_load("p5.jpg", &width, &height, &channels, 3);

    // проверка на правильность чтения файла
    if (image == NULL) {
        printf("============= ERROR READ PICTURE =============\n");
        return 666;
    }

    // Увеличиваем вероятность добавления шума
    float noise_probability = 0.07f; // шанс для каждого пикселя
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            float random_value = (float)rand() / RAND_MAX; // Случайное число от 0.0 до 1.0

            if (random_value < noise_probability) {
                int idx = (i * width + j) * 3;
                if (rand() % 2 == 0) { // Случайный выбор: белый или черный
                    image[idx] = 255;     // R
                    image[idx + 1] = 255; // G
                    image[idx + 2] = 255; // B
                } else {
                    image[idx] = 0;     // R
                    image[idx + 1] = 0; // G
                    image[idx + 2] = 0; // B
                }
            }
        }
    }

    stbi_write_jpg("output.jpg", width, height, 3, image, 90);

    stbi_image_free(image);

    return 0;
}