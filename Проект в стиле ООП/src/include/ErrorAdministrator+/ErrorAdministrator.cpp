#include "ErrorAdministrator.h"





ErrorAdministrator* ErrorAdministrator::start_ptr = nullptr;
ErrorAdministrator* ErrorAdministrator::end_ptr = nullptr;
int ErrorAdministrator::_error_EA = 0;


ErrorAdministrator::ErrorAdministrator()
{
	
	//обнуление заголовка:
	for(int i = 0; i < _length; i++) 
	{
		_head_str[i] = '0';
		
		if(i == _length-1)
		{
			_head_str[i] = '\0';
		}		
	}
	
		
	_isAdmin = true;
	_errSum = 0;
	
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


bool	ErrorAdministrator::Set_header(const char *headler)
{
	
	//установка заголовка:
	for(int i = 0; i < _length; i++) 
	{
		if(headler[i] == '\0')
		{
			for(i; i < _length; i++) 
			{
				_head_str[i] = '\0';
			}
			break;
		}
	
		_head_str[i] = headler[i];
		
		if(i == _length-1)
		{
			_head_str[i] = '\0';
		}		
	}

	
	
	
	//проверка на дублирующие заголовки:
	ErrorAdministrator *ptr_EA = start_ptr;
	
	int pre_error = _error_EA;
	
	while (1)
	{
		if(ptr_EA == nullptr) break;
		
		int err = 0;
		
		for (int i = 0; i < _length; i++)
		{
			if(_head_str[i] == ptr_EA->_head_str[i]) err++;
		}
		
	
		if(err == _length)
		{
			_error_EA++;
		}

		ptr_EA = ptr_EA->next_ptr;
		if( ptr_EA == start_ptr) break;
	}
	
	
	if(pre_error != _error_EA) return false;	// если уже существует такой же заголовок
		
	return true;
}


bool	ErrorAdministrator::IsAdmin()
{
	return _isAdmin;
}


void	ErrorAdministrator::AddErr()
{
	_errSum++;
}


void	ErrorAdministrator::NullErr()
{
	_errSum = 0;
}


void	ErrorAdministrator::Step()		//????? не нужно?? 
{


	ErrorAdministrator *ptr_EA = start_ptr;
	
	while (1)
	{
		if(true == ptr_EA->_isAdmin)
		{
		
		
		
		}
				
		ptr_EA = ptr_EA->next_ptr;
		if( ptr_EA == start_ptr) break;
	}

}



ErrorAdministrator*	ErrorAdministrator::Return_next()
{
	return next_ptr;	
}


ErrorAdministrator*	ErrorAdministrator::Return_pre()
{	
	return pre_ptr;
}


ErrorAdministrator*	ErrorAdministrator::Return_start()
{
	return start_ptr;	
}


ErrorAdministrator*	ErrorAdministrator::Return_end()
{
	return end_ptr;	
}









