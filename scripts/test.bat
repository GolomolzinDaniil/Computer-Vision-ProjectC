cd ..
        @REM Градация серого
test images/b2.jpg -grgrey tests/1_Gradation_grey.jpg
test images/b5.jpg -grgrey tests/2_Gradation_Grey.jpg
test images/p3.jpg -grgrey tests/3_Gradation_Grey.jpg
        @REM Инверсия цветов
test images/b1.jpg -invers tests/4_Inversion.jpg
test images/p1.jpg -invers tests/5_Inversion.jpg
test images/bb.jpg -invers tests/6_Inversion.jpg
        @REM Поворот относительно Ox
test images/p3.jpg -leftturn tests/7_Turner_left.jpg
test images/p4.jpg -leftturn tests/8_Turner_left.jpg
test images/p5.jpg -leftturn tests/9_Turner_left.jpg
        @REM Поворот относительно Oy
test images/p1.jpg -upturn tests/10_Turner_up.jpg
test images/p3.jpg -upturn tests/11_Turner_up.jpg
test images/p5.jpg -upturn tests/12_Turner_up.jpg
        @REM Медианный фильтр
test images/m0.jpg -median 3 tests/13_Median_filter_3.jpg
test images/m1.jpg -median 3 tests/14_Median_filter_3.jpg
test images/m2.jpg -median 5 tests/15_Median_filter_5.jpg
        @REM Гауссов фильтр
test images/g0.jpg -gauss 3 tests/16_Gauss_filter_3.jpg
test images/g1.jpg -gauss 5 tests/17_Gauss_filter_5.jpg
test images/g2.jpg -gauss 3 tests/18_Gauss_filter_3.jpg
        @REM Определение границ
test images/b1.jpg -edges 2 tests/19_Edges_detect_2.jpg
test images/b2.jpg -edges 1 tests/20_Edges_detect_1.jpg
test images/b3.jpg -edges 1 tests/21_Edges_detect_1.jpg
        @REM Поворот изображения
test images/b5.jpg -alphaturn 45 tests/22_Turner_alpha_45.jpg
test images/p3.jpg -alphaturn 74 tests/23_Turner_alpha_74.jpg
test images/p5.jpg -alphaturn 120 tests/24_Turner_alpha_120.jpg
    @REM Изменения масштаба изображения
test images/b2.jpg -resize 2.0 tests/25_ReSize_2.jpg
test images/p3.jpg -resize 5.0 tests/26_ReSize_5.jpg
test images/p4.jpg -resize 0.3 tests/27_ReSize_03.jpg
cd scripts