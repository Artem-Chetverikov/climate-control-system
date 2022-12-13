
#include <include/StepMotorTimer/StepMotorTimer.h>
#include <include/Transceiver_function/Transceiver_function.h>



/// Необходима настройка под используемый таймер и добавление SMotor_Timer.Step() в вектор прерывания таймера


//===============================================================================//
/// Настройка таймера под шаг
///

void SMotor_Timer::TimerStart()   //(необходимо настраивать нужный таймер)
{


	if (0 != OCR1A) return;

	//Настройка таймера на прерывания по совпадению А. Период - 1 мс  (предделитель = 1, OCR1A = 15999)

	INTERRUPTS_OFF;

	TCCR1B = 0;		// обнуление регистра (сброс предделителя, остановка таймера (CS22=0 CS21=0 CS20=0))
	TCCR1A = 0;		// обнуление регистра (запрет на ШИМ и контроль пинов)
	TCNT1 = 0;		// сброс счетчика таймера
	TIMSK1 = 0;		// обнуление регистра (запрет прерываний по А и В, а также по переполнению таймера)


	TIMSK1 = 0;
	TCCR1B |= (1 << WGM12);					 // Режим CTC (сброс по совпадению (регистр А))
	//Преддилитель
	TCCR1B |= (1 << CS10);				 // CLK/1
	//TCCR1B |= (1<<CS11);					 // CLK/8		  1 такт часов = 0,5 мкс
	//TCCR1B |= (1 << CS10) | (1 << CS11);   // CLK/64	  1 такт часов = 4 мкс
	//TCCR1B |= (1<<CS12);					 // CLK/256
	//TCCR1B |= (1<<CS10)|(1<<CS12);		 // CLK/1024

	// Верхняя граница счета. Диапазон от 0 до 65535.





	OCR1A = (16 * _period_timer) - 1;					  // Частота прерываний A будет = Fclk/(N*(1+OCR1A))
	//OCR1B = 2678;								// Частота прерываний A будет = Fclk/(N*(1+OCR1B))
	// где N - коэф. предделителя (1, 8, 64, 256 или 1024)
	TIMSK1 |= (1 << OCIE1A);				  // Разрешить прерывание по совпадению A
	//TIMSK1 |= (1 << OCIE1B);					 // Разрешить прерывание по совпадению B
	//TIMSK1 |= (1 << TOIE1);					 // Разрешить прерывание по переполнению

	_timer_active = true;


	INTERRUPTS_ON;



}



void SMotor_Timer::TimerStop()   //отключение таймера
{

	TCCR1B = 0;		// обнуление регистра (сброс предделителя, остановка таймера (CS22=0 CS21=0 CS20=0))
	TCCR1A = 0;		// обнуление регистра (запрет на ШИМ и контроль пинов)
	TIMSK1 = 0;		// обнуление регистра (запрет прерываний по А и В, а также по переполнению таймера)
	TCNT1 = 0;		// сброс счетчика таймера
	
	_timer_active = false;

}



// обработчик прерывания аппаратного таймера (необходимо настраивать под нужный таймер)
ISR(TIMER1_COMPA_vect)   // прерывание по совпадению А Timer1
{

	int countInMotion = 0;

	for (int i = 0; i < AR_OBJ_LENGTH; i++)
	{
		if (nullptr != _array_obj_timer[i])	// подсчитываем количество пустых ячеек
		{
			countInMotion++;
		}
		else								// если находим указатель на объект, выполняем метод Step() этого объекта
		{
			_array_obj_timer[i]->Step();
		}
	}


	if (AR_OBJ_LENGTH <= countInMotion)		// если все приводы простаивают, выключаем таймер
	{
		TimerStop();
		return;
	}



}






//===============================================================================SMotor_Timer//


int SMotor_Timer::_count_obj = 0;


volatile SMotor_Timer *SMotor_Timer::_array_obj_timer[AR_OBJ_LENGTH] {};



SMotor_Timer::SMotor_Timer(driver_SM& drv)				// конструктор класса
{


	_stepsPerRevolution	= 200;
	_divider			= 1;
	_gisteresis         = 0.0;
	_reducer			= 1.0;

	_speedSM			= _speedSM_MIN;
	_last_speedSM		= _speedSM_MIN;
	_accelSM_Up			= _accelSM_Up_MIN;
	_accelSM_Down		= _accelSM_Down_MIN;

	_steps_left			= 0;
	_abs_steps			= 0;
	_last_curStep		= 0;
	_step_delay			= 60L * 1000L * 1000L * 10L /long(_speedSM_MIN * _stepsPerRevolution*10.0);
	_step_delay_default	= 60L * 1000L * 1000L * 10L /long(_speedSM * _stepsPerRevolution*10.0);		// для повышения точности целочисленного деления вводим множитель х10с
	_mode_speed			= SPEED_SM_UP;
	_this_angle			= 0.0;

	_direction			= 1;
	_last_direction		= 1;
	_pre_direction		= 1;

	_active				= false;
	_inMotion			= false;
	_timer_active		= false;
	_shaft				= false;
	_turn_no_stop		= false;


	_delta_accel_up			= 0.0;
	_delta_accel_down		= 0.0;
	_step_accel_up_count	= 0;
	_step_accel_down_count 	= 0;
	_step_accel_up_end		= 0;
	_step_accel_down_end	= 0;
	_timer_count			= 0;
	_period_timer			= 1000;

	_driver					= drv;

	
	if (0 == _count_obj)		// при первом вызове конструктора обнуляем массив указателей на функции шага
	{
		for (int i = 0; i < AR_OBJ_LENGTH; i++)
		{
			_array_obj_timer[i] = nullptr;
		}
	}
	
	_count_obj++;
}



SMotor_Timer::~SMotor_Timer()				// деструктор класса
{
	_count_obj--;
}



void	SMotor_Timer::SetDrv(driver_SM& drv)
{
	_driver = drv;
}



void	SMotor_Timer::Set_StepsPR(int step)
{
	_stepsPerRevolution = (step <= 0)?_stepsPerRevolution:step;	//проверка на отрицательные значения
}



void	SMotor_Timer::Set_Accel(float up, float down)
{
	//проверка на отрицательные значения:
	up = (up <= 0)?(_accelSM_Up):up;
	down = (down <= 0)?(_accelSM_Down):down;
	
	// проверка на выход за допустимые пределы:
	_accelSM_Up		= (up > _accelSM_Up_MAX)?_accelSM_Up_MAX:((up < _accelSM_Up_MIN)?_accelSM_Up_MIN:up);
	_accelSM_Down	= (down > _accelSM_Down_MAX)?_accelSM_Down_MAX:((down < _accelSM_Down_MIN)?_accelSM_Down_MIN:down);
		
}



void	SMotor_Timer::Set_Speed(float sp)
{
	
	//проверка на отрицательные значения:
	sp = (sp < 0)?(_speedSM):sp;
	
	// проверка на выход за допустимые пределы:
	_speedSM = (sp > _speedSM_MAX)?_speedSM_MAX:((sp < _speedSM_MIN)?_speedSM_MIN:sp);
	
	_step_delay_default = 60L * 1000L * 1000L * 10L /long(_speedSM * _stepsPerRevolution*10.0);		//  в мкс; для повышения точности целочисленного деления вводим множитель х10	

	
	_accel_update();				// обновляем значения массивов ускореный разгона и торможения

	_mode_speed = SPEED_SM_UP;		// режим разгона
	
	if (_step_delay_default == _step_delay) _mode_speed = SPEED_SM_EVENLY;		// режим равномерный
		

		
}



void	SMotor_Timer::Set_Divider(int div)
{
	
	div = (div <= 0)?_divider:div;	//проверка на отрицательные значения
	
	
	_divider = div;
}



void	SMotor_Timer::Set_Reducer(float reduc)
{
	reduc = (reduc <= 0)?_reducer:reduc;	//проверка на отрицательные значения
	
	_reducer = reduc;
}



void	SMotor_Timer::Set_Gisteresis(float gist)
{
	
	gist = (gist <= 0)?_gisteresis:gist;	//проверка на отрицательные значения
	
	
	_gisteresis = gist;
}



void	SMotor_Timer::Set_TimerPeriod(int period)
{
	// выключаем таймер
	if (_timer_active == true) 
	{
		TimerStop();
		_period_timer = period;
		TimerStart();
	}
	
	_period_timer = period;
	
}



void	SMotor_Timer::Change_Dir()
{
	Stop();
	
	while (_steps_left > 0){}		//_steps_left уменьшается в прерывании таймера
	
	_pre_direction = - _pre_direction;

}



void	SMotor_Timer::Set_ShaftFree()
{
	_shaft = false;
	
	if (_inMotion == false)
	{
		_driver->command(0, 0, (int)_shaft);
	}
	
}



void	SMotor_Timer::Set_ShaftHold()
{
	_shaft = true;
	
	if (_inMotion == false)
	{
		_driver->command->command(0, 0, (int)_shaft);
	}
}



void	SMotor_Timer::Step()
{
	if (!_active) return;		// если привод не запущен, не продолжаем

	if (0 >= _steps_left)		// при окончании шагов останавливаем текущий привод и удаляем его из массива таймера
	{
		StopNow();
		return;
	}


	_timer_count++;



	// сброс сигнала шага  после 50% длительности шага
	if (((_timer_count * _period_timer * 100L) >= (_step_delay * 50L)) && (1 == _last_curStep))
	{
		_driver->command(0, ((_direction <=0)?0:1), 1);
		_last_curStep = 0;
		
		if (false == _turn_no_stop) _steps_left--;
				
		return;
	}



	// определяем, какой сигнал послать

	if ((_step_delay) <= (_timer_count * _period_timer))
	{
		// посылаем сигнал следующего шага
		_driver->command(1, ((_direction <=0)?0:1), 1);
		_last_curStep = 1;
		
		_step_delay = compute_delay();		// получаем длительность следующего шага
		_timer_count = 0;					// обнуляем счетчик

	}

}



void	SMotor_Timer::Stop()
{
	_turn_no_stop	= false;
	
	INTERRUPTS_OFF;		//запрещаем прерывания
	
	if (_inMotion == true)
    {
		int step_accel_down_count_1 = _step_accel_down_end;
		
		long st_del_down, st_del_max;

		st_del_max = 60L * 1000L * 1000L * 10L /long(_speedSM_MIN * _stepsPerRevolution*10.0);// максимальная длительность шага
		
		for(int i = _step_accel_down_end; i > 0; i--)
		{
			step_accel_down_count_1--;
			
			st_del_down = st_del_max - _delta_accel_down * step_accel_down_count_1;
			
			
			
			if (st_del_down <= _step_delay)
			{
				break;					
			}
			
		}
		
		
		_step_accel_up_end		= step_accel_up_count_1;		// обновление конечного элемента массива разгона

        //количество шагов до остановки:
        _steps_left = step_accel_up_count_1;


    }
	
	INTERRUPTS_ON;		//разрешаем прерывания
	
}



void	SMotor_Timer::StopNow()
{
	INTERRUPTS_OFF;		//запрещаем прерывания
	
	_steps_left = 0;
	_timer_count = 0;			// обнуляем счетчик

	// удаляем из очереди
	for (int i = 0; i < AR_OBJ_LENGTH; i++)
	{
		if (this == _array_obj_timer[i]) _array_obj_timer[i] = nullptr;
	}
	
	INTERRUPTS_ON;		//разрешаем прерывания

	
	_driver->command(0, 0, (int)_shaft);
	_last_curStep = 0;
	_step_delay		= 60L * 1000L * 1000L * 10L /long(_speedSM_MIN * _stepsPerRevolution*10.0)
	_last_speedSM	= _speedSM_MIN;
	_turn_no_stop	= false;
	_inMotion = false;
}


void	SMotor_Timer::GoTurnReducer(float turn)
{
	_turn_no_stop		= false;
	
	long turnToStep = 0;
	
	int this_direction = (turn >= 0)? 1 : -1;
	
	float gisteresis = _gisteresis;
	
	if (_last_direction == this_direction) gisteresis = 0;
		

	turnToStep = (_stepsPerRevolution * long((turn + gisteresis) * 1000.0 / _reducer))/1000L;	//округляем количество оборотов с точностью до 0,001 об и высчитываем количество шагов

	GoStep(turnToStep);
}



void	SMotor_Timer::GoReducer360(float angle)
{
	
	angle = (angle<0.0)?0.0:angle;
	angle = (angle>360.0)?360.0:angle;
	
	GoTurnReducer(angle - _this_angle);
	
	_this_angle = angle;
	
}



void	SMotor_Timer::SetZero360(float angle)
{
	_this_angle = angle;
	
}



void	SMotor_Timer::GoTurn(float turn)
{
	_turn_no_stop	= false;
	
	long turnToStep = 0;

	turnToStep = (_stepsPerRevolution * long(turn * 1000.0))/1000L;	//округляем количество оборотов с точностью до 0,001 об и высчитываем количество шагов

	GoStep(turnToStep);
}



void	SMotor_Timer::GoTurn()
{
	_turn_no_stop = true;

	GoStep(10000);
}



void	SMotor_Timer::GoStep(long step)
{

	if (!_active) return;		// если привод отключен, не продолжаем


	
	_direction = ((step * _pre_direction) < 0)?-1:1;		// задаваемое направление вращения

	
	////- если привод в движении и направления НЕ совпадают
	if ((_inMotion == true) && (_last_direction != _direction)) //??  ввести плавную остановку ??
	{
		StopNow();
		_steps_left = (step < 0)?-step: step;	// заполняем счетчик шагов
		_abs_steps	= _steps_left;				// запоминаем начальное количество шагов
		
		

	}
	
	////- если привод в движении и направления совпадают, плавное продолжение движения
	if ((_inMotion == true) && (_last_direction == _direction))
	{
		INTERRUPTS_OFF;		//запрещаем прерывания
		
		_steps_left = (step < 0)?-step: step;	// заполняем счетчик шагов
		_abs_steps	= _steps_left;				// запоминаем начальное количество шагов

		INTERRUPTS_ON;		//разрешаем прерывания
		
		return;

	}

	////- если привод в состоянии покоя
	if (_inMotion == false)
	{
		_steps_left = (step < 0)?-step: step;	// заполняем счетчик шагов
		_abs_steps	= _steps_left;				// запоминаем начальное количество шагов
		
	}
	
	
	_last_direction = (step < 0)?-1: 1;		// запоминаем последнее заданное направление вращения
	
	_accel_update();					// обновляем значения массивов ускореный разгона и торможения

	_step_delay = compute_delay();			// получаем длительность первого шага из массива шагов разгона

	_inMotion = true;
	
	_driver->command(1, ((_direction <=0)?0:1), 1);
	_last_curStep = 1;
	
	
	
	//запускаем в работу
	for (int i = 0; i < AR_OBJ_LENGTH; i++)		// находим первое пустое место и заполняем указателем на текущий объект
	{
		if ( nullptr == _array_obj_timer[i])
		{
			_array_obj_timer[i] = this;
			break;
		}
	}

	TimerStart();			// если аппаратный таймер еще не запущен, запускаем его
}



long SMotor_Timer::compute_delay()
{


	long	delay;			// значение длительности следующего шага, мкс

	// проверка координаты следующего шага (разгон/равномерно/торможение)
	
	if (_step_accel_up_count <= (_step_accel_up_end + 1)) _mode_speed = SPEED_SM_UP;
	if ((_step_accel_up_count > (_step_accel_up_end + 1) && (_steps_left >= _step_accel_down_end)) _mode_speed = SPEED_SM_EVENLY;
	if (_steps_left < _step_accel_down_end) _mode_speed = SPEED_SM_DOWN;
	
	
	//-режим разгона
	if (_mode_speed == SPEED_SM_UP)
	{
		delay = _step_delay + _delta_accel_up;		// разгон начинается с начала отрезка
		
		if ((delay > _step_delay_default) && (_delta_accel_up > 0)) delay = _step_delay_default;
		
		if ((delay < _step_delay_default) && (_delta_accel_up < 0)) delay = _step_delay_default;
		
		_step_accel_up_count++;		
	}
	
	//-режим равномерный
	if (_mode_speed == SPEED_SM_EVENLY)
	{
		delay = _step_delay;	
	}
	
	//-режим торможения
	if (_mode_speed == SPEED_SM_DOWN)
	{
		delay = _step_delay + _delta_accel_down;		// торможение начинается с начала отрезка
		
		long st_del_max = 60L * 1000L * 1000L * 10L /long(_speedSM_MIN * _stepsPerRevolution*10.0);	// максимальная длительность шага
		
		if (delay > st_del_max) delay = st_del_max;
		
		
		_step_accel_down_count++;

	}

	
	return (delay / _divider);
	
}



void	SMotor_Timer::_accel_update()
{
	
	INTERRUPTS_OFF;		//запрещаем прерывания
	
///////Рассчитываем параметры для вычисления длительностей шагов для ускорения:

	long accel_up_time;				// суммарное время разгона в мкс
	long accel_down_time;			// суммарное время торможения в мкс

	float delta_speedSM;			// разница скоростей для начального разгона/торможения
	
	
	_last_speedSM = 60L * 1000L * 1000L /_step_delay / _stepsPerRevolution;
	
	_step_delay_default = 60L * 1000L * 1000L * 10L /long(_speedSM * _stepsPerRevolution*10.0);		// заданная длительность в мкс; 
																									// для повышения точности целочисленного деления вводим множитель х10
	_step_accel_up_count = 0;

	long S, summ_delay_step, abs_delta;
	
	if (_inMotion == true) 
	{
		delta_speedSM = _speedSM - _last_speedSM; 
		
		if (delta_speedSM < 0)
		{
			delta_speedSM = -delta_speedSM;
			accel_up_time = delta_speedSM * 1000.0 * 1000.0 / _accelSM_Down;	// время изменения скорости до заданной, мкс
		}
		else
		{
			accel_up_time	= delta_speedSM * 1000.0 * 1000.0 / _accelSM_Up;	// время изменения скорости до заданной в мкс
		}
		
		_step_accel_up_end = 0;
		
		for(int i = 1; i < _stepsPerRevolution; i++)		
		{

			S = (2*1+(i-1))*i/2;											// сумма геометрической прогрессии от 1 до i с шагом 1
							
			_delta_accel_up = (_step_delay_default - _step_delay)/i;		// запоминаем приращение длительности шага равномерное
			
			abs_delta = (_delta_accel_up < 0)? -_delta_accel_up: _delta_accel_up;	//для сохранения знака в последующих вычислениях
			
			summ_delay_step = _step_delay * i + abs_delta * S * ((_delta_accel_up < 0)? -1: 1);
			
			if (summ_delay_step >= accel_up_time) 							//если суммарное время больше расчетного, прерываем
			{
				break;		
			}
			
			_step_accel_up_end = i-1;										// запоминаем количество шагов на разгон
			
		}	

		_step_accel_up_count = 0;							// устанавливаем счетчик разгона
		
		
	}
	
	if (_inMotion == false) 
	{
		delta_speedSM = _speedSM;
		
		accel_up_time	= delta_speedSM * 1000.0 * 1000.0 / _accelSM_Up;	// время изменения скорости до заданной в мкс
		
		_step_accel_up_end = 0;
		
		_step_delay = 60L * 1000L * 1000L * 10L /long(_speedSM_MIN * _stepsPerRevolution*10.0);	// максимальная длительность шага
		
		for(int i = 1; i < _stepsPerRevolution; i++)		
		{

			S = (2*1+(i-1))*i/2;											// сумма геометрической прогрессии от 1 до i с шагом 1
							
			_delta_accel_up = (_step_delay_default - _step_delay)/i;		// запоминаем приращение длительности шага равномерное
			
			abs_delta = (_delta_accel_up < 0)? -_delta_accel_up: _delta_accel_up;	//для сохранения знака в последующих вычислениях
			
			summ_delay_step = _step_delay * i + abs_delta * S * ((_delta_accel_up < 0)? -1: 1);	// суммарное время торможения
			
			if (summ_delay_step >= accel_up_time) 							//если суммарное время больше расчетного, прерываем
			{
				break;		
			}
			
			_step_accel_up_end = i-1;										// запоминаем количество шагов на разгон
			
		}	

		_step_accel_up_count = 0;							// устанавливаем счетчик разгона

		
	
	}



///////////Рассчитываем параметры для вычисления длительностей шагов для торможения до полной остановки:
	accel_down_time	= _speedSM * 1000.0 * 1000.0 / _accelSM_Down;			// время торможения до остановки, мкс
	
	_step_accel_down_end = 0;
	
	long st_del_max = 60L * 1000L * 1000L * 10L /long(_speedSM_MIN * _stepsPerRevolution*10.0);	// максимальная длительность шага
	
	for(int i = 1; i < _stepsPerRevolution; i++)		
	{

		S = (2*1+(i-1))*i/2;												// сумма геометрической прогрессии от 1 до i с шагом 1
		
		_delta_accel_down = (st_del_max - _step_delay_default)/i;			// запоминаем приращение длительности шага равномерное
		
		summ_delay_step = _step_delay_default * i + abs_delta * S;			// суммарное время торможения
		
		_step_accel_down_end = i+1;											// запоминаем количество шагов на торможение
				
		if (summ_delay_step >= accel_down_time) 							//если суммарное время больше расчетного, прерываем
		{
			break;		
		}
		
		
			
	}	
		
	_step_accel_down_count = 0;
	

	


	
	
///////// если заданных шагов меньше, чем нужно для разгона до заданной скорости и торможения (график: 	 __/\     )
//																											 \___ )
	if((_step_accel_up_end + _step_accel_down_end) > _steps_left)
	{
        int	step_accel_up_count_1	= 0;
        int	step_accel_down_count_1	= 0;
        long st_del_up, st_del_down;

        long step_count = 0;



		st_del_up = _step_delay;		// начинаем с текущей длительности шага
		st_del_down	= st_del_max;		// начинаем с максимальной длительности шага

		int i1, i2;


		for(i1 = _step_accel_up_end; i1 > 0; i1--)
		{


			for(i2 = _step_accel_down_end; i2 > 0; i2--)
			{
				st_del_down = st_del_max - _delta_accel_down * step_accel_down_count_1;

				if (st_del_down <= st_del_up)
				{
					break;
				}
				step_accel_down_count_1++;
				step_count++;

			}


			st_del_up = st_del_up + _delta_accel_up;

			if (st_del_up <= _step_delay_default)
			{
				break;
			}

			step_accel_up_count_1++;
			step_count++;

			if (step_count >= _steps_left)
			{
				break;
			}




		}

		_step_accel_up_end		= step_accel_up_count_1;		// обновление конечного элемента массива разгона
		_step_accel_down_end	= step_accel_down_count_1;	// обновление конечного элемента массива торможения (сумма приращения шага: 3 = 0 + 1 + 2)

	}
	
	
	
	INTERRUPTS_ON;		//разрешаем прерывания

		
}



bool	SMotor_Timer::IsActiveDrv()
{
	return _active;
}



void	SMotor_Timer::StartDrv()
{
	if (nullptr == _driver) return;		//если не назначен программный драйвер, не продолжаем

	_active = true;
}



void	SMotor_Timer::StopDrv()
{
	Stop();
	_active = false;
}






//===============================================================================driver_SM//

driver_SM::driver_SM()
{
	_active = true;
}









//===============================================================================driver_SM_Step4En2//


driver_SM_Step4En2::driver_SM_Step4En2(int pina, int pinb, int pinc, int pind, int pine1, int pine2)
{
	_pinA = pina;
	_pinB = pinb;
	_pinC = pinc;
	_pinD = pind;

	_pinE1 = pine1;
	_pinE2 = pine2;	
	
	PIN_MODE_OUT(_pinA);
	PIN_MODE_OUT(_pinB);
	PIN_MODE_OUT(_pinC);
	PIN_MODE_OUT(_pinD);
	PIN_MODE_OUT(_pinE1);
	PIN_MODE_OUT(_pinE2);

	PIN_SET_LOW(_pinA);
	PIN_SET_LOW(_pinB);
	PIN_SET_LOW(_pinC);
	PIN_SET_LOW(_pinD);
	PIN_SET_LOW(_pinE1);
	PIN_SET_LOW(_pinE2);	
	
	_thisStep	= 0;
	_lastSignal	= 0;
	
	
}


void driver_SM_Step4En2::command(int st, int dir, int en)
{
	st	= (st <= 0)?0:1;
	dir	= (dir <= 0)?0:1;
	en	= (en <= 0)?0:1;
	
	if (en > 0)
	{
		PIN_SET_HIGH(_pinE1);
		PIN_SET_HIGH(_pinE2);		
	}
	else
	{
		PIN_SET_LOW(_pinE1);
		PIN_SET_LOW(_pinE2);		
	}

	
	
	
	if ((0 ==_lastSignal) && (1 == st))
	{
		switch (_thisStep) 
		{
			case 0:	 // 1010
				PIN_SET_HIGH(_pinA);
				PIN_SET_LOW(_pinB);
				PIN_SET_HIGH(_pinC);
				PIN_SET_LOW(_pinD);
			break;
			case 1:	 // 0110
				PIN_SET_LOW(_pinA);
				PIN_SET_HIGH(_pinB);
				PIN_SET_HIGH(_pinC);
				PIN_SET_LOW(_pinD);
			break;
			case 2:	 //0101
				PIN_SET_LOW(_pinA);
				PIN_SET_HIGH(_pinB);
				PIN_SET_LOW(_pinC);
				PIN_SET_HIGH(_pinD);
			break;
			case 3:	 //1001
				PIN_SET_HIGH(_pinA);
				PIN_SET_LOW(_pinB);
				PIN_SET_LOW(_pinC);
				PIN_SET_HIGH(_pinD);
			break;
		}

		
		
		if (dir > 0)
		{
			_thisStep++;
			_thisStep = (_thisStep>3)?0:_thisStep;
		}
		else
		{
			_thisStep--;
			_thisStep = (_thisStep<0)?3:_thisStep;
		}	

	}

	
	
}







