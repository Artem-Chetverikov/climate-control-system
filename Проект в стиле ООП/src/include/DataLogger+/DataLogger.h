#ifndef __DATALOGGER_H_INCLUDED
#define __DATALOGGER_H_INCLUDED




class dataStruct
{
private:


protected:

	bool				_isPrint;			// true, если данные выводятся в печать

	const char* 		_header;			// указатель на заголовок данных (строка типа:  "Параметр №1")
	
	static const int	_len_str = 15;		// длина строки значения параметра после конвертации

	char 				_param_str[_len_str];	// строка значения параметра после конвертации


	dataStruct			*next_ptr;			// указатель на следующий экземпляр класса
	dataStruct			*pre_ptr;			// указатель на предыдущий экземпляр класса
	static dataStruct	*start_ptr;			// указатель на первый созданный экземпляр класса
	static dataStruct	*end_ptr;			// указатель на последний созданный экземпляр класса

	
public:

	dataStruct();
	
	bool 			Set_header(const char *header);		// запись строки заголовка выводимой информации (true, если записан)

    bool			Set_param(char param);				// запись строки значения параметра (true, если записан)

	bool			Set_param(int param);				// запись строки значения параметра (true, если записан)

    bool			Set_param(long param);				// запись строки значения параметра (true, если записан)

	bool			Set_param(float param);				// запись строки значения параметра (true, если записан)

	bool			Set_param(double param);			// запись строки значения параметра (true, если записан)

	bool			Set_param(char* param);				// запись строки значения параметра (true, если записан)
	
	bool			Set_param(const char* param);		// запись строки значения параметра (true, если записан)
	
	bool			IsPrint();							// true, данные выводятся в печать

	char*			Print_header();						// вывод строки заголовка через указатель

	char*			Print_param();						// вывод строки значения параметра через указатель
	
	
	dataStruct*				Return_next();				// вернуть указатель на следующий экземпляр класса
	dataStruct*				Return_pre();				// вернуть указатель на предыдущий экземпляр класса
	static dataStruct*		Return_start();				// вернуть указатель на первый созданный экземпляр класса
    static dataStruct*		Return_end();				// вернуть указатель на последний созданный экземпляр класса
	
};






#endif
