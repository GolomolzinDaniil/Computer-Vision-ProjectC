#include "header.h"
#define STBI_NO_SIMD  // отключаем так как SSE2 не работает с tcc
#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../libs/stb_image_write.h"
/**
 * @brief Отражение координаты пикселя с учётом границ изображения.
 * 
 * Эта функция реализует отражение координаты пикселя относительно краёв изображения. Если координата 
 * выходит за пределы изображения, она будет "отражена" в пределах допустимого диапазона.
 * 
 * @param coor Координата пикселя.
 * @param max_size Размер изображения (ширина или высота).
 * 
 * @return int Новая координата после отражения.
 */
int reflect(int coor, int max_size) {
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

/**
 * @brief Сравнение двух элементов для использования в функции сортировки.
 * 
 * Эта функция используется для сравнения двух элементов в массиве (например, для использования с `qsort`).
 * 
 * @param a Указатель на первый элемент.
 * @param b Указатель на второй элемент.
 * 
 * @return int Возвращает положительное значение, если первый элемент больше, отрицательное, если меньше, 
 * и 0, если элементы равны.
 */
int compare(const void* a, const void* b) {
    int A = *(uc*)a;
    int B = *(uc*)b;
    return ( (A > B) ? 1 : (A < B) ? -1 : 0);
}

/**
 * @brief Ядро Гаусса для двумерной фильтрации.
 * 
 * Функция вычисляет значение ядра Гаусса для данных координат в двумерном пространстве с учётом заданной 
 * дисперсии (параметр d).
 * 
 * @param x Координата по оси X.
 * @param y Координата по оси Y.
 * @param d Дисперсия Гаусса.
 * 
 * @return double Значение ядра Гаусса для данных координат.
 */
double g(double x, double y, double d) {
    return ( ( 1 / ( 2 * M_PI * d * d ) ) * exp ( ( x * x + y * y ) / ( -2 * d * d ) ) );
}

/**
 * @brief Одномерная Гауссова функция для оси X (горизонтальная свертка).
 * 
 * Функция вычисляет значение одномерной Гауссовой функции для оси X с заданной дисперсией (параметр d).
 * 
 * @param x Координата по оси X.
 * @param d Дисперсия Гаусса.
 * 
 * @return double Значение одномерной Гауссовой функции.
 */
double gx(double x, double d) {
    return ( (1 / sqrt ( 2 * M_PI * d * d ) ) * exp ( ( x * x ) / ( -2 * d * d ) ) );
}

/**
 * @brief Одномерная Гауссова функция для оси Y (вертикальная свертка).
 * 
 * Функция вычисляет значение одномерной Гауссовой функции для оси Y с заданной дисперсией (параметр d).
 * 
 * @param y Координата по оси Y.
 * @param d Дисперсия Гаусса.
 * 
 * @return double Значение одномерной Гауссовой функции.
 */
double gy(double y, double d) {
    return ( (1 / sqrt ( 2 * M_PI * d * d ) ) * exp ( ( y * y ) / ( -2 * d * d ) ) );
}

/**
 * @brief Вычисление новых координат по оси X при повороте изображения.
 * 
 * Эта функция рассчитывает новые координаты по оси X для пикселя изображения после его поворота на 
 * заданный угол относительно центра изображения.
 * 
 * @param x Координата пикселя по оси X.
 * @param y Координата пикселя по оси Y.
 * @param center_x Координата центра изображения по оси X.
 * @param center_y Координата центра изображения по оси Y.
 * @param dgr Угол поворота в градусах.
 * 
 * @return double Новая координата по оси X.
 */
double newCoorX(double x, double y, double center_x, double center_y, double dgr) {
    // так как мы поворачиваем относительно центра изображения
    x -= center_x;
    y -= center_y;
    // перевод в радианы
    double new_dgr = dgr * M_PI / 180.0;
    // снова прибавляем, возвращая на предыдущее место
    return (x * cos(new_dgr) - y * sin(new_dgr) + center_x);
}

/**
 * @brief Вычисление новых координат по оси Y при повороте изображения.
 * 
 * Эта функция рассчитывает новые координаты по оси Y для пикселя изображения после его поворота на 
 * заданный угол относительно центра изображения.
 * 
 * @param x Координата пикселя по оси X.
 * @param y Координата пикселя по оси Y.
 * @param center_x Координата центра изображения по оси X.
 * @param center_y Координата центра изображения по оси Y.
 * @param dgr Угол поворота в градусах.
 * 
 * @return double Новая координата по оси Y.
 */
double newCoorY(double x, double y, double center_x, double center_y, double dgr) {
    // так как мы поворачиваем относительно центра изображения
    x -= center_x;
    y -= center_y;
    // перевод в радианы
    double new_dgr = dgr * M_PI / 180.0;
    // снова прибавляем, возвращая на предыдущее место
    return (x * sin(new_dgr) + y * cos(new_dgr) + center_y);
}

/**
 * @brief Билинейная интерполяция для расчета значения пикселя на новых координатах.
 * 
 * Эта функция выполняет билинейную интерполяцию для вычисления значения пикселя на новых координатах 
 * (с учетом приращений и весов пикселей).
 * 
 * @param x Новая координата по оси X.
 * @param y Новая координата по оси Y.
 * @param width Ширина изображения.
 * @param height Высота изображения.
 * @param channels Количество каналов в изображении.
 * @param ch Канал, для которого выполняется интерполяция.
 * @param image Массив данных изображения.
 * 
 * @return double Значение пикселя, полученное после интерполяции.
 */
double BilinalInterpol(double x, double y, int width, int height, int channels, int ch, uc* image) {
    // нижняя граница    // верхняя граница     // приращение дробной и целой
    int x_down = (int)x;
    int x_up = x_down + 1;
    double dx = x - x_down;
    int y_down = (int)y;
    int y_up = y_down + 1;
    double dy = y - y_down;
    // проверки на выход за границы (если вышло, то берем крайнее значение)
    x_up   = (x_up   < 0) ? 0 : (x_up   >=  width) ? width - 1  : x_up;
    x_down = (x_down < 0) ? 0 : (x_down >=  width) ? width - 1  : x_down;
    y_up   = (y_up   < 0) ? 0 : (y_up   >= height) ? height - 1 : y_up;
    y_down = (y_down < 0) ? 0 : (y_down >= height) ? height - 1 : y_down;
    // взятие весов крайних пикселей
    int p11 = image[(y_down * width + x_down  ) * channels + ch]; // левый нижний
    int p21 = image[(y_down * width + x_up    ) * channels + ch]; // правый нижний
    int p12 = image[(y_up   * width + x_down  ) * channels + ch]; // левый верхний
    int p22 = image[(y_up   * width + x_up    ) * channels + ch]; // правый верхний
    // усредненное значение пикселя
    double curr_res = (1-dx)*(1-dy)*p11 + dx*(1-dy)*p21 + (1-dx)*dy*p12 + dx*dy*p22;
    // return ( (curr_res < 0) ? 0 : (curr_res > 255) ? 255 : curr_res; // вероятно лишнее
    return curr_res;
}

/**
 * @brief Получение новых координат для масштабирования изображения.
 * 
 * Эта функция вычисляет новые координаты для пикселя изображения при его масштабировании, 
 * с учётом параметра масштаба и с сохранением пропорций.
 * 
 * @param x Координата пикселя по оси X.
 * @param y Координата пикселя по оси Y.
 * @param resize Коэффициент масштабирования.
 * @param width Ширина изображения.
 * @param height Высота изображения.
 * @param channels Количество каналов изображения.
 * @param ch Канал, для которого выполняется масштабирование.
 * @param image Массив данных изображения.
 * 
 * @return double Значение пикселя на новых координатах.
 */
// получение новых координат при масштабировании
double GetNewCoor (int x, int y, double resize, int width, int height, int channels, int ch, uc* image) {
    // дробные новые координаты (относительно центра)
    double half_width = width / 2.0;
    double half_height = height / 2.0;
    double new_x = (x - half_width) / resize + half_width;
    double new_y = (y - half_height) / resize + half_height;
    // так как нам не нужны увеличенные одинаковыми пикселями края
    if (new_x < 0 || new_x >= width || new_y < 0 || new_y >= height) {
        return 0;
    }
    // нижняя граница        // верхняя граница     // приращение дробной и целой
    int x_down = (int)new_x;
    int x_up = x_down + 1;
    double dx = new_x - x_down;
    int y_down = (int)new_y;
    int y_up = y_down + 1;
    double dy = new_y - y_down;
    if (x_up < 0 || x_up >= width || x_down < 0 || x_down >= width) {
        if (y_up < 0 || y_up >= height || y_down < 0 || y_down >= height) {
            return 0;
        }
    }
    int p11 = image[(y_down * width + x_down) * channels + ch];
    int p21 = image[(y_down * width + x_up  ) * channels + ch];
    int p12 = image[(y_up * width + x_down  ) * channels + ch];
    int p22 = image[(y_up * width + x_up    ) * channels + ch];
    return ( (1-dx)*(1-dy)*p11 + dx*(1-dy)*p21 + (1-dx)*dy*p12 + dx*dy*p22 );
}
