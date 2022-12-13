#include "SensorStruct.h"


SensorStr* SensorStr::start_ptr = nullptr;
SensorStr* SensorStr::end_ptr = nullptr;


SensorStr::SensorStr()
{
	_valueCur	= -1.0E+38;
	
	_error		= 0;
	

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



float	SensorStr::ReturnVal()
{
	return _valueCur;
}


int		SensorStr::ReturnErr()
{
	return _error;
}

void		SensorStr::Read()
{
	
}


