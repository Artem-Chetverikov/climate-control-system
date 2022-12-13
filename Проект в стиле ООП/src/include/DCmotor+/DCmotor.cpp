#include <include/DCmotor/DCmotor.h>
#include <include/Transceiver_function/Transceiver_function.h>





DCmotor* DCmotor::start_ptr = nullptr;
DCmotor* DCmotor::end_ptr = nullptr;


DCmotor::DCmotor(int pinA, int pinB)
{
	_pin_A	= pinA;
	_pin_B	= pinB;
	_pin_En	= -1;

	_preDir	= 1;
	_isActiv = true;

	_isPWM = false;
	


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


DCmotor::DCmotor(int pinA, int pinB, int pinEn)
{
	_pin_A	= pinA;
	_pin_B	= pinB;
	_pin_En	= pinEn;

	_preDir	= 1;
	_isActiv = true;

	_isPWM = false;
	


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




void	DCmotor::Init()
{
	PIN_MODE_OUT(_pin_A);
	PIN_MODE_OUT(_pin_B);

	
	PIN_SET_LOW(_pin_A);
	PIN_SET_LOW(_pin_B);
	
	
	if ((_pin_En >= 0) && (false == _isPWM))
	{
		PIN_MODE_OUT(_pin_En);
		PIN_SET_LOW(_pin_En);
	}
	
	if ((_pin_En >= 0) && (true == _isPWM))
	{
		PIN_MODE_OUT(_pin_En);
		PIN_SET_ANALOG(_pin_En, 0);
	}
	

}


void	DCmotor::Start(int speed)
{
	if (false == _isActiv)	return;
	
	if (0 == speed)
	{
		Stop();
		return;
	}
	
	if (speed > 100)	speed = 100;
	if (speed < -100)	speed = -100;	
	
	speed = _preDir * speed;
	
	if ((_pin_En < 0) && (false == _isPWM))
	{
		if (speed > 0)
		{
			PIN_SET_HIGH(_pin_A);
			PIN_SET_LOW(_pin_B);
		}
		
		if (speed < 0)
		{
			PIN_SET_LOW(_pin_A);
			PIN_SET_HIGH(_pin_B);
		}		
		
	}
	
	
	if ((_pin_En < 0) && (true == _isPWM))
	{
		if (speed > 0)
		{
			PIN_SET_ANALOG(_pin_A, abs(speed));
			PIN_SET_LOW(_pin_B);
		}
		
		if (speed < 0)
		{
			PIN_SET_LOW(_pin_A);
			PIN_SET_ANALOG(_pin_B, abs(speed));
		}		
		
	}
	
	
	if ((_pin_En >= 0) && (false == _isPWM))
	{
		if (speed > 0)
		{
			PIN_SET_HIGH(_pin_A);
			PIN_SET_LOW(_pin_B);
			PIN_SET_HIGH(_pin_En);
		}
		
		if (speed < 0)
		{
			PIN_SET_LOW(_pin_A);
			PIN_SET_HIGH(_pin_B);
			PIN_SET_HIGH(_pin_En);
		}		
		
	}
	
	
	if ((_pin_En >= 0) && (true == _isPWM))
	{
		if (speed > 0)
		{
			PIN_SET_HIGH(_pin_A);
			PIN_SET_LOW(_pin_B);
			PIN_SET_ANALOG(_pin_En, abs(speed));	
		}
		
		if (speed < 0)
		{
			PIN_SET_LOW(_pin_A);
			PIN_SET_HIGH(_pin_B);
			PIN_SET_ANALOG(_pin_En, abs(speed));	
		}		
	}	
	
	
	
	
	
	
}
	

void	DCmotor::Stop()
{
	if ((false == _isPWM) && (_pin_En >= 0))
	{
		PIN_SET_LOW(_pin_En);		
	}
	
	if ((true == _isPWM) && (_pin_En >= 0))
	{
		PIN_SET_ANALOG(_pin_En, 0);		
	}
	
	
	PIN_SET_LOW(_pin_A);
	PIN_SET_LOW(_pin_B);
	
}
	
	
void	DCmotor::PWM_ON()
{
	_isPWM = true;
}


void	DCmotor::PWM_OFF()
{
	_isPWM = false;
}


void	DCmotor::ChangePreDir()
{
	_preDir = -_preDir;
}


bool	DCmotor::IsActiv()
{
	return _isActiv;
}


void	DCmotor::Activ()
{
	_isActiv = true;
}


void	DCmotor::Deactiv()
{
	_isActiv = false;
}

	
	
	
	

DCmotor*	DCmotor::Return_next()
{
	return next_ptr;
	
}


DCmotor*	DCmotor::Return_pre()
{
	
	return pre_ptr;
}


DCmotor*	DCmotor::Return_start()
{
	return start_ptr;
	
}


DCmotor*	DCmotor::Return_end()
{
	return end_ptr;
	
}









