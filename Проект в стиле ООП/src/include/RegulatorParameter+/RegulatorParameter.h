#ifndef __PARAMETERCLIMAT_H_INCLUDED
#define __PARAMETERCLIMAT_H_INCLUDED

#include "SensorStruct.h"
#include "NoiseFilter.h"
#include "ProcessRegulator.h"


using	Func_1int_ptr = void(*)(int);			// псевдоним указателя на функцию 



class RegulatorParameter				// класс управления параметром
{
	private:
	
		float					_paramSet;				// заданное значение параметра
		float					_paramDelta;			// диапазон отклонения от заданного значения параметра (±)
		float					_paramCur;				// текущее значение параметра
		static const int		_length = 2;			// длина массива предыдущих значений
		float					_history_param[_length];	// массив предыдущих значений параметра
		int						_count_pos;				// текущая позиция для записи в массив значений
		float					_paramFilt;				// фильтрованное значение параметра
		bool					_first_get_val;			// флаг первого запроса значения с датчика
		bool					_filt_default;			// флаг применения МЕДИАННОГО фильтра по умолчанию
		int						_percent;				// величина регулировки в % (±100%))
		SensorStr				*_sensor;				// указатель на функцию чтения ТЕКУЩЕГО значения параметра из датчика, возвращающую float
		NoiseFilter				*_filter;				// ВНЕШНИЙ ФИЛЬТР помех (с параметром: текущее значение), возвращающую float
		Func_1int_ptr			_control_func;			// указатель на функцию, которую нужно вызвать для УПРАВЛЕНИЯ физическими органами управления
														// (с параметром: управляющее воздействие ±100%), ничего не возвращающую
		ProcessRegulator		*_regulator;			// указатель на функцию, которую нужно вызвать для ВЫЧИСЛЕНИЯ управляющего воздействия
														// (с параметрами: текущее значение, заданное значение), возвращающую управляющее воздействие ±100%

		RegulatorParameter			*pre_ptr;			// указатель на предыдущий экземпляр класса
		RegulatorParameter			*next_ptr;			// указатель на следующий экземпляр класса
		static RegulatorParameter	*end_ptr;			// указатель на последний созданный экземпляр класса
		
		
	public:
	// конструктор класса, задающий функцию, передающую ТЕКУЩЕЕ значение параметра,   функцию РЕГУЛИРУЮЩУЮ    и    функцию УПРАВЛЯЮЩИЮ
		RegulatorParameter(SensorStr *Func_read, ProcessRegulator *Func_reg, Func_1int_ptr Func_contr);				
		
		
		// задать целевое значение параметра, допуск (равномерный вверх и вниз. Например (10,2), то будет param = 8...12 )
		void	Set_Param(float, float);
		
		// задать целевое значение параметра, допуск по умолчанию ±2,5%
		void	Set_Param(float);

		// задать функцию, передающую текущее значение параметра,   функцию регулирующую    и    функцию управляющию 		
		void	Set_Func(SensorStr*, ProcessRegulator*, Func_1int_ptr);
	
		void	SetFilt(NoiseFilter*);					// установить внешний фильтр
		
		void	DelFilt();								// удалить внешний фильтр помех
		
		void	Filt_off();								// Отключить все фильтры помех
		
		void	Filt_on();								// Включить все фильтры помех
	
		void	Step();									// исполнение очередного шага регулирования параметра


};


#endif