#include "functions/header.h"

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
            if (strcmp(op, "-grgrey"            ) == 0) {
                return Gradation_grey (input_file, output_file);
            }
            // инверсия цветов
            else if (strcmp(op, "-invers"       ) == 0) {
                return Inversion (input_file, output_file);
            }
            // поворот относительно Оу
            else if (strcmp(op, "-leftturn"     ) == 0) {
                return Turner_left (input_file, output_file);
            }
            // поворот относительно Ох
            else if (strcmp(op, "-upturn"       ) == 0) {
                return Turner_up (input_file, output_file);
            }
        }
         // обработка функций с аргументом
        else if (argc == 5) {
             // дополнительное значение
            if (strcmp(op, "-resize"            ) == 0) {
                // для фильтра -resize используем atof для дробного числа
                double resize_val = atof(argv[3]);  // преобразуем строку в double
                // выходной файл
                char* output_file = argv[4];
                return ReSize(input_file, resize_val, output_file);
            } else {
                // для всех других фильтров используем atoi для целого числа
                int val = atoi(argv[3]);
                // выходной файл
                char* output_file = argv[4];
                // Медианный фильтр
                if (strcmp(op, "-median"        ) == 0) {
                    return Median_filter(input_file, val, output_file);
                }
                // Гауссов фильтр
                else if (strcmp(op, "-gauss"    ) == 0) {
                    return Gauss_filter(input_file, val, output_file);
                }
                // определение границ
                else if (strcmp(op, "-edges"    ) == 0) {
                    return Edges_detect(input_file, val, output_file);
                }
                // поворот на угол alpha
                else if (strcmp(op, "-alphaturn") == 0) {
                    return Turner_alpha(input_file, val, output_file);
                }        
            }
        }
        // иной случай
        else {
            fprintf(stderr, "============= UNKNOWN FILTER TYPE =============\n");
            return 1;
        }
    }
}
