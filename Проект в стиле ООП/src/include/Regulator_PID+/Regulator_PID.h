#ifndef __REGULATOR_PID_H_INCLUDED
#define __REGULATOR_PID_H_INCLUDED


#include "ProcessRegulator.h"




class Regulator_PID : public ProcessRegulator
{
private:

	float			SetPoint;			// заданное значение
	float			CurPoint;			// текущее значение
	long			error_0;			// предыдущее значение ошибки регулирования
	int				Kp;					// коэффициент пропорциональный
	int				Ki;					// коэффициент интегральный
	int				Kd;					// коэффициент дифференциальный
	long			Sum_Int;			// суммарная интегральная погрешность
	
	int				_type_reg;			// выбор типа регулирования (по умолчанию - PID)
										//	PID	- 1
										//	P	- 2
										//	PI	- 3
										//	PD	- 4

public:

	Regulator_PID(int K1, int K2, int K3);					// конструктор класса (коэффициенты  х1000)
	
	
	void	Set_type(int type);								// выбор типа регулирования (по умолчанию - PID)
			
	int		_PID(float valCur, float valSet);				// ПИД-регулирование
															// получить значения контролируемого параметра и выдать величину воздействия (±100%)
											
	int		_P(float valCur, float valSet);					// П-регулирование
															// получить значения контролируемого параметра и выдать величину воздействия (±100%)

	int		_PI(float valCur, float valSet);				// ПИ-регулирование
															// получить значения контролируемого параметра и выдать величину воздействия (±100%)

	int		_PD(float valCur, float valSet);				// ПД-регулирование
															// получить значения контролируемого параметра и выдать величину воздействия (±100%)	
	virtual int	Compute(float, float);						// функция расчета управляющего воздействия																
	
};




#endif