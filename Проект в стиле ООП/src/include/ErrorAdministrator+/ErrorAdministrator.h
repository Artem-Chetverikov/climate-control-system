#ifndef __ERRORADMINISTRATOR_H_INCLUDED
#define __ERRORADMINISTRATOR_H_INCLUDED




class ErrorAdministrator
{
private:


protected:
	
	static const int			_length = 11;		// длина строки идентификатора
	
	char						_head_str[_length];	// уникальный идентификатор текущего вида ошибки (10 символов)
	
	int							_errSum;			// количество возникших ошибок текущего вида
	
	bool						_isAdmin;			// true, если данные по текущей ошибке учитываются
	
	ErrorAdministrator			*next_ptr;			// указатель на следующий экземпляр класса
	ErrorAdministrator			*pre_ptr;			// указатель на предыдущий экземпляр класса
	static ErrorAdministrator	*start_ptr;			// указатель на первый созданный экземпляр класса
	static ErrorAdministrator	*end_ptr;			// указатель на последний созданный экземпляр класса
	
	static int					_error_EA;			// счетчик ошибок по классу ErrorAdministrator
	

	
public:

	
	ErrorAdministrator();			// конструктор с установкой уникального идентификатора текущего вида ошибки

	bool			Set_header(const char *headler);	// установка заголовка
	
	void			AddErr();							// увеличение количества ошибок текущего вида на единицу
	void			NullErr();							// обнуление количества ошибок текущего вида

	bool			IsAdmin();							// true, если данные по ошибке текущего вида администрируются
	
	static void		Step();								// шаг обхода всех экземпляров данного класса на наличие ошибок
	
	
	ErrorAdministrator*				Return_next();		// вернуть указатель на следующий экземпляр класса
	ErrorAdministrator*				Return_pre();		// вернуть указатель на предыдущий экземпляр класса
	static ErrorAdministrator*		Return_start();		// вернуть указатель на первый созданный экземпляр класса
    static ErrorAdministrator*		Return_end();		// вернуть указатель на последний созданный экземпляр класса

	
};






#endif
