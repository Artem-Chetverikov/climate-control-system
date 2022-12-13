DataLogger

Библиотека сбора данных и вывода в файл либо в строку


Пример:


#include "DataLogger.h"


class AAA : public dataStruct
{
};



dataStruct	A_1;
AAA 		A_2;



A_1.Set_header("Data_1");
A_2.Set_header("Data_2");


A_1.Set_param(560.0);
A_2.Set_param(898);





dataStruct *ptr_ = dataStruct::Return_start();


while (1)
{
    cout << if(ptr_->IsPtint()){ptr_->Print_header()} << "; ";



    ptr_ = ptr_->Return_next();
    if( ptr_ == dataStruct::Return_start()) break;
}


while (1)
{
    cout << if(ptr_->IsPtint()){ptr_->Print_param()} << "; ";



    ptr_ = ptr_->Return_next();
    if( ptr_ == dataStruct::Return_start()) break;
}





------------------------------------------------------------------




Протестировано 24.10.2020 - успешно