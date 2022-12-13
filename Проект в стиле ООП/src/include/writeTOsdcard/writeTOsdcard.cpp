#include "writeTOsdcard.h"
#include "floatTOchar.h"





printTOfile* printTOfile::start_ptr = nullptr;
printTOfile* printTOfile::end_ptr = nullptr;

printTOfile::printTOfile()
{

	_print = true;


	for(int i = 0; i < _len_str-1; i++) _param[i] = '0';
	_param[_len_str-1] = '\0';

	_header = _param;
	// определение указателей на экземпляр класса

	if (nullptr == start_ptr)
	{
		pre_ptr = this;
		next_ptr = this;
		start_ptr = this;
		end_ptr = this;

	}
	else
	{
	    end_ptr->next_ptr = this;
		pre_ptr = end_ptr;
		next_ptr = start_ptr;
		end_ptr = this;
		start_ptr->pre_ptr = this;
	}

}


bool	printTOfile::Set_header(const char *header)
{
	_header = header;

	return true;

}

const char*	printTOfile::Print_header()
{
	return _header;

}

bool	printTOfile::Set_param(float param)
{

    floatTOcharExp((double) param, _param, _len_str);

    return true;
}

bool	printTOfile::Set_param(double param)
{

    floatTOcharExp(param, _param, _len_str);

    return true;
}

bool	printTOfile::Set_param(long param)
{

    longTOstr(param, _len_str, _param, _len_str);

    return true;
}

bool	printTOfile::Set_param(int param)
{

    longTOstr((long)param, _len_str, _param, _len_str);

    return true;
}

char*	printTOfile::Print_param()
{
    return _param;

}



printTOfile*	printTOfile::Return_next()
{
	return next_ptr;
	
}

printTOfile*	printTOfile::Return_pre()
{
	
	return pre_ptr;
}

printTOfile*	printTOfile::Return_start()
{
	return start_ptr;
	
}

printTOfile*	printTOfile::Return_end()
{
	return end_ptr;
	
}


























	pinMode(SD_CHIP_SELECT_PIN, OUTPUT);
	
	if (!sd.begin(SD_CHIP_SELECT_PIN, SPI_HALF_SPEED)) {			  // проверка на ошибки SD-card


		int i_sd = 0;
		while (!sd.begin(SD_CHIP_SELECT_PIN, SPI_HALF_SPEED)){
			i_sd++;
			delay(100);

			if (i_sd > 5) {  //при превышении числа ошибок
			
				// индикация ошибок
				error_SD();
				return;
			}
		}
	}

	// получение имени текущего дневного файла
	for (int i_file = 0; i_file < file_open_error; i_file++){

		if ((tm.Hour != last_time_hour_3) && (tm.Hour % SD_HOUR = 0)){
			
			last_time_hour_3 = tm.Hour;
			nn1++;
			
		}
		
		YY_1 = String(tmYearToCalendar(tm.Year) - 2000);
		MM_1   = String(tm.Month);
		if (tm.Month<10) {MM_1 = String(0) + MM_1;}
		DD_1   = String(tm.Day);
		if (tm.Day<10) {DD_1 = String(0) + DD_1;}
		NN_1 = String(nn1);
		if (nn1<10) {NN_1 = String(0) + NN_1;}
		TXT_1  = String(".txt");
		name_file = String(YY_1 +  MM_1 + DD_1 + NN_1  + TXT_1);

		unsigned int length_name_file;
		
		length_name_file = name_file.length();
		
		length_name_file = length_name_file + 1;
		
		char name_file_char[length_name_file];

		name_file.toCharArray(name_file_char, length_name_file);
		

		if (myFile.open(name_file_char, O_RDWR | O_CREAT)) {
			
			i_file = file_open_error;
			
			
		}
		
		else {			//если файл с текущим именем не открылся
			
			if (i_file == (file_open_error - 1)){	//если файл с текущим именем не открылся 5 раз
				nn1++;
				i_file =0;	
			}

			if (nn1 > 20){
				
				nn1 = 0;
				// индикация ошибок
				error_SD();
				return;			
			}
			
			delay(10);
			
		}

	}

	


	
	myFile.seekSet(myFile.fileSize());           //установка позиции курсора в конец файла


	uint32_t min_size_ = 10;						//минимальный размер файла в байтах

	if (myFile.fileSize() <= min_size_){		   //запись в файл заголовков столбцов

	
		char charBufVar_1[] = "Час;Мин;Сек;Шаг_регулирования;Temp_C1;Temp_C2;Temp_C3;Temp_C4;Влажность;Temp;Влажность1;Temp1;CO2;Ошибки 1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;День цикла";	// ; x 28
		int size_charBufVar_1 = sizeof(charBufVar_1);

		const char *index_1 = (const char*)charBufVar_1;    //+  
		
		myFile.write(13);
		myFile.write(10);
		myFile.write(index_1, size_charBufVar_1);
		myFile.write(13);
		myFile.write(10);

		
		if (!myFile.timestamp(T_CREATE, tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second)) {  //установка времени создания файла
			error_SD();
			return;
		}
	
	}


	String Write_data;			//буфер для записываемых данных
	


	
	Write_data = String(tm.Hour) + ";"
				+ String(tm.Minute) + ";"
				+ String(tm.Second) + ";"
				+ String(step_regulate_Temp) + ";"	
			+ String(tempC1) + ";"
			 + String(tempC2) + ";"
			 + String(tempC3) + ";"
			 + String(tempC4) + ";"
			 + String(humidity_SHT1x) + ";" 
			+ String(temp_SHT1x) + ";"
			 + String(/*humidity_DHT21_1*/level_Temp_dynamic) + ";"
			 + String(/*temp_c_DHT21_1*/level_Temp * 100L) + ";"
			 + String(ppm) + ";";



	//текущие ошибки
	for (int i = 1; i < ARRAY_LENGH_N; i++) {
		Write_data = Write_data + String(error_array_N[i])+ ";";
	}



	Write_data = Write_data + String(day_cycle);
	

	
	unsigned int length_Write_data;
	
	length_Write_data = Write_data.length();
	
	length_Write_data = length_Write_data + 1;
	
	char charBufVar[length_Write_data];

	Write_data.toCharArray(charBufVar, length_Write_data);
	
	
	//вывод буфера в файл
	const char *index_2 = (const char*)charBufVar;    //+  

	if (myFile.write(index_2, length_Write_data) != length_Write_data) {
		error_SD();
		return;
	}
	
	myFile.write(13);
	myFile.write(10);



	
	  // установка времени изменения файла
	if (!myFile.timestamp(T_WRITE, tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second)) {
		error_SD();
		return;
	}	
	
  
	if (!myFile.close()) {
		error_SD();
		return;
	}


	// при успешном выполнении операции записи
	reboot_SDcard = 0;
	if (error_array_N[error_SDcard] == 1){
		error_array_N[error_SDcard] = 0;
		error();
	}

