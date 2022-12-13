SHT1x_Simpl

Библиотека взаимодействия с датчиком температуры и влажности SHT1x


Пример:
#include "SHT1x_Simpl.h"


SHT1x_Simpl_Temp 	SHT1x_1_temp(10, 12);		//определяем датчик на data-пине №10  clock-пине №12
SHT1x_Simpl_Hum 	SHT1x_1_hum(SHT1x_1_temp);	//определяем датчик на data-пине №10  clock-пине №12


SHT1x_1_temp.Read();	//опрос датчика
SHT1x_1_hum.Read();	//опрос датчика


t = SHT1x_1_temp.ReturnVal();				
h =SHT1x_1_hum.ReturnVal();				
			

err1 = SHT1x_1_temp.ReturnErr();		//вывод количества ошибок при опросе

err2 = SHT1x_1_hum.ReturnErr();	



------------------------------------------------------------------




Протестировано 02.11.2020 - успешно