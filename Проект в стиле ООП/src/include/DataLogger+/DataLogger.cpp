#include "DataLogger.h"
#include "floatTOchar.h"





dataStruct* dataStruct::start_ptr = nullptr;
dataStruct* dataStruct::end_ptr = nullptr;

dataStruct::dataStruct()
{

	_isPrint = true;


	for(int i = 0; i < _len_str-1; i++) _param_str[i] = '0';
	_param_str[_len_str-1] = '\0';

	_header = _param_str;
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


bool	dataStruct::Set_header(const char *header)
{
	_header = header;

	return true;

}

bool	dataStruct::Set_param(float param)
{

    floatTOcharExp((double) param, _param_str, _len_str);

    return true;
}

bool	dataStruct::Set_param(double param)
{

    floatTOcharExp(param, _param_str, _len_str);

    return true;
}

bool	dataStruct::Set_param(long param)
{

    longTOstr(param, _len_str, _param_str, _len_str);

    return true;
}

bool	dataStruct::Set_param(int param)
{

    longTOstr((long)param, _len_str, _param_str, _len_str);

    return true;
}

bool	dataStruct::Set_param(char param)
{

    longTOstr((long)param, _len_str, _param_str, _len_str);

    return true;
}

bool	dataStruct::Set_param(char* param)
{
	for(int i = 0; i < _len_str; i++) 
	{
		if(param[i] == '\0')
		{
			for(i; i < _len_str; i++) 
			{
				_param_str[i] = '\0';
			}
			break;
		}
	
		_param_str[i] = param[i];
		
		if(i == _len_str-1)
		{
			_param_str[i] = '\0';
		}		
	}
	
    return true;
}

bool	dataStruct::Set_param(const char* param)
{
	for(int i = 0; i < _len_str; i++) 
	{
		if(param[i] == '\0')
		{
			for(i; i < _len_str; i++) 
			{
				_param_str[i] = '\0';
			}
			break;
		}
	
		_param_str[i] = param[i];
		
		if(i == _len_str-1)
		{
			_param_str[i] = '\0';
		}		
	}
	
    return true;
}

bool	dataStruct::IsPrint()
{
	return _isPrint;
}

char*	dataStruct::Print_header()
{
	return (char*)_header;

}

char*	dataStruct::Print_param()
{
    return _param_str;

}



dataStruct*	dataStruct::Return_next()
{
	return next_ptr;
	
}

dataStruct*	dataStruct::Return_pre()
{
	
	return pre_ptr;
}

dataStruct*	dataStruct::Return_start()
{
	return start_ptr;
	
}

dataStruct*	dataStruct::Return_end()
{
	return end_ptr;
	
}









