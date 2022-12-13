#include "RegulatorParameter.h"


RegulatorParameter::RegulatorParameter(SensorStr *Func_read, ProcessRegulator *Func_reg, Func_1int_ptr Func_contr)
{
	_paramSet		= -1.0E+38;
	_paramDelta		= -1.0E+38;
	_paramCur		= -1.0E+38;
	for ( int i = 0; i < _length; i++)	_history_param[i] = -1.0E+38;
	_count_pos		= 0;
	_paramFilt		= -1.0E+38;
	_first_get_val	= true;
	_filt_default	= true;
	_percent		= 0;
	
	_filter	= nullptr;

	_sensor			= Func_read;
	_regulator		= Func_reg;
	_control_func	= Func_contr;
	
	
	// определение указателей на экземпляр класса
	next_ptr		= nullptr;
	
	if (nullptr == end_ptr)
	{
		pre_ptr = nullptr;
	}
	else 
	{
		end_ptr->next_ptr = this;
		pre_ptr = end_ptr;
	}

	end_ptr = this;

}

RegulatorParameter	*RegulatorParameter::end_ptr = nullptr;


void	RegulatorParameter::Set_Param(float val, float delta)
{
	_paramSet		= val;
	_paramDelta		= delta;
}

void	RegulatorParameter::Set_Param(float val)
{
	_paramSet		= val;
	_paramDelta		= val*0.025;
}		



void	RegulatorParameter::Set_Func(SensorStr *Func_read, ProcessRegulator *Func_reg, Func_1int_ptr Func_contr)
{
	_sensor			= Func_read;
	_regulator		= Func_reg;
	_control_func	= Func_contr;
}		

		
void	RegulatorParameter::SetFilt(NoiseFilter	*filt)
{
	_filter = filt;			
}		


void	RegulatorParameter::DelFilt()
{
	_filter	= nullptr;
}


void	RegulatorParameter::Filt_off()
{
	_filt_default	= false;
}


void	RegulatorParameter::Filt_on()
{
	_filt_default	= true;
}


void	RegulatorParameter::Step()
{
	
	if ((nullptr == _control_func) || (nullptr == _regulator) || (nullptr == _sensor)) 
	{
		return;
	}


	if (_first_get_val)
	{
		float val = _sensor->ReturnVal();
		
		if (-1.0E+38 == val) return;				
		
		for ( int i = 0; i < _length; i++)	_history_param[i] = val;

		_paramCur = val;
		
		_first_get_val = false;
		
		return;
	}
	
	
	_history_param[_count_pos] = _paramCur;
	
	if (++_count_pos >= _length) _count_pos = 0;

	_paramCur = _sensor->ReturnVal();			
	
	
	

	if (_filt_default)		//если используем фильтр помех
	{

		if (nullptr == _filter)		// если нет внешнего фильтра помех, применяем по умолчанию МЕДИАННЫЙ фильтр 3 порядка 
		{

			int pre_count_pos = (_count_pos > 0) ? (_count_pos - 1) : (_length - 1);
			
			if ((_history_param[pre_count_pos] <= _history_param[_count_pos]) && (_history_param[pre_count_pos] <= _paramCur)) {
				_paramFilt = (_history_param[_count_pos] <= _paramCur) ? _history_param[_count_pos] : _paramCur;
			}
			else {
				if ((_history_param[_count_pos] <= _history_param[pre_count_pos]) && (_history_param[_count_pos] <= _paramCur)) {
					_paramFilt = (_history_param[pre_count_pos] <= _paramCur) ? _history_param[pre_count_pos] : _paramCur;
				}
				else {
					_paramFilt = (_history_param[pre_count_pos] <= _history_param[_count_pos]) ? _history_param[pre_count_pos] : _history_param[_count_pos];
				}
			}
		}
		
		else _paramFilt = _filter->filtered(_paramCur);		// иначе применяем внешний фильтр
	}
	
	
	
	if (!_filt_default)
	{
		_paramFilt = _paramCur;	// если не используем фильтр помех, используем текущее значение без фильтра помех
	}
	
	
	
	if (_paramFilt >= (_paramSet + _paramDelta)) 
	{				
	    

	    percent = _regulator->Compute(_paramFilt, (_paramSet + _paramDelta));
        // ограничиваем результирующее воздействие ±100%:
        if (percent > 100) percent = 100;
        if (percent < -100) percent = -100;


	}
	
	
	
		if (_paramFilt <= (_paramSet - _paramDelta))
	{				
	    

	    percent = _regulator->Compute(_paramFilt, (_paramSet - _paramDelta));
        // ограничиваем результирующее воздействие ±100%:
        if (percent > 100) percent = 100;
        if (percent < -100) percent = -100;


		
	}
	
	_control_func(percent);	// определение регулирующего воздействия и регулирование
	
	
	
}















