#include "header.h"


/**
 * @brief Применение гауссова фильтра для размытия изображения.
 * 
 * Функция выполняет двупроходную фильтрацию изображения с помощью раздельных 
 * одномерных гауссовых свёрток (сначала по горизонтали, затем по вертикали). 
 * Это позволяет эффективно достичь размытия, снижая шумы и сглаживая границы.
 * 
 * @param input_file Путь к входному изображению.
 * @param area Размер свёртки (должен быть нечётным числом, например 3, 5, 7 и т.д.).
 * @param output_file Путь для сохранения результирующего изображения после размытия.
 * 
 * @return int Код возврата: 0 — успешно, 666 — ошибка чтения изображения.
 * 
 * @note Степень размытия регулируется параметром `area`: чем больше значение, 
 *       тем сильнее размытие. Значение `delta` рассчитывается как `area / 2`.
 *       Функция использует отражённые индексы для обработки краёв изображения.
 * 
 * @warning Перед использованием убедитесь, что изображение существует и доступно для чтения.
 *          Значение `area` должно быть положительным нечётным числом.
 */


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
