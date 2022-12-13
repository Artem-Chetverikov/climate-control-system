#ifndef __WRITETOSDCARD_H_INCLUDED
#define __WRITETOSDCARD_H_INCLUDED




class printTOfile
{
private:


protected:

	bool				_print;				// true, если выводится в файл

	const char* 		_header;			// указатель на заголовок

	static const int	_len_str = 15;		// длина строки значения параметра после конвертации

	char 				_param[_len_str];	// строка значения параметра после конвертации


	printTOfile			*next_ptr;			// указатель на следующий экземпляр класса
	printTOfile			*pre_ptr;			// указатель на предыдущий экземпляр класса
	static printTOfile	*start_ptr;			// указатель на первый созданный экземпляр класса
	static printTOfile	*end_ptr;			// указатель на последний созданный экземпляр класса

	
public:

	printTOfile();
	
	bool 			Set_header(const char *header);		// запись строки заголовка выводимой информации (true, если записан)

	const char*		Print_header();						// вывод строки заголовка в файл через указатель

	bool			Set_param(float param);				// запись строки значения параметра (true, если записан)

	bool			Set_param(double param);			// запись строки значения параметра (true, если записан)

	bool			Set_param(int param);				// запись строки значения параметра (true, если записан)
	
    bool			Set_param(long param);				// запись строки значения параметра (true, если записан)

	char*			Print_param();						// вывод строки значения параметра в файл через указатель
	
	
	printTOfile*	Return_next();						// указатель на следующий экземпляр класса
	printTOfile*	Return_pre();						// указатель на предыдущий экземпляр класса
	static printTOfile*	Return_start();					// указатель на первый созданный экземпляр класса
    static printTOfile*	Return_end();					// указатель на последний созданный экземпляр класса

};


















// Объявляем переменные для работы с SD-card 
//(подключена к pin miso - 50, mosi - 51, sck - 52, CS - 53):

SdFat sd;	// класс библиотеки SdFat.h

FatFile myFile;											// класс библиотеки SdFat.h

#define SD_CHIP_SELECT_PIN 53							// CS-выход SD-card
String name_file;										// имя создаваемого файла
String YY_1;											// часть имени создаваемого файла
String MM_1;											// часть имени создаваемого файла
String DD_1;											// часть имени создаваемого файла
String TXT_1;											// часть имени создаваемого файла
String NN_1;
int nn1 = 0;											// счетчик файлов одного дня
int file_open_error = 5;								// количество попыток открытия файла со счетчиком nn1
int last_SD_hour;										// для разбиения файлов через отведенное время
#define SD_HOUR 2										// новый файл каждые 2 часа
int last_time_hour_3;									// последний час создания файла



#endif
