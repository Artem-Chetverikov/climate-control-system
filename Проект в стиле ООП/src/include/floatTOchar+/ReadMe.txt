floatTOchar

Библиотека конвертации чисел из типа float в строку типа char[]


Пример:


#include "floatTOchar.h"

    const int yy = 15;
    char rr[yy];

    double c = 98765.123456789;

    floatTOcharExp(c, 5, rr, yy);


    floatTOchar(c, 6, 3, rr, yy);

------------------------------------------------------------------




Протестировано 18.09.2020 - успешно