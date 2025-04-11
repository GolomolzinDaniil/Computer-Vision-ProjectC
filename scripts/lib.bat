cd ..
@REM Создание объектных файлов
gcc -c functions/Median_filter.c functions/Gauss_filter.c functions/Edges_detect.c functions/Gradation_grey.c functions/helper_func.c functions/Inversion.c functions/Turner_alpha.c functions/Turner_left.c functions/Turner_up.c functions/ReSize.c
@REM Создание статичной библиотеки
ar rcs mylib.a Median_filter.o Gauss_filter.o Edges_detect.o Gradation_grey.o helper_func.o Inversion.o Turner_alpha.o Turner_left.o Turner_up.o ReSize.o
@REM Удаление объектных файлов
rm *.o
@REM Компиляция main.c файла вместе с библиотекой mylib.a
gcc main.c mylib.a -o test
cd scripts