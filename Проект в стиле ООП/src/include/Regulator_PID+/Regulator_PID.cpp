#include "Regulator_PID.h"



Regulator_PID::Regulator_PID(int K1, int K2, int K3)
{
	SetPoint	= -1.0E+38;
	CurPoint	= -1.0E+38;
	error_0		= 0;
	Kp			= K1;
	Ki			= K2;
	Kd			= K3;
	Sum_Int		= 0;
	_type_reg	= 1;
	
}



int		Regulator_PID::Compute(float valCur, float valSet)
{	

	if (1 == _type_reg) 
	{
		return _PID(valCur, valSet);
	}
	
	if (2 == _type_reg) 
	{
		return _P(valCur, valSet);
	}
	
	if (3 == _type_reg) 
	{
		return _PI(valCur, valSet);
	}
	
	if (4 == _type_reg) 
	{
		return _PD(valCur, valSet);
	}
	
	return 0;
}





void	Regulator_PID::Set_type(int type)
{
	if ((type > 4) || (type < 1 )) 
	{
		return;
	}
	
	_type_reg	= type;
}






int Regulator_PID::_PID(float valCur, float valSet)
{
	long U	= 0;
	long P	= 0;
	long I	= 0;
	long D	= 0;
	long e	= 0;
	
	
	e = long((valSet - valCur)* 100.0);		// вычисляем текущую ошибку (100.0 - множитель для выполнения целочисленных операций без потери необходимой точности)
	
	P = Kp * e / 100L / 1000L;					// пропроциональная составляющая
	// ограничиваем воздействие ±100%:
	if (P > 100) P = 100;
	if (P < -100) P = -100;	
	
	
	I = Ki * e / 100L / 1000L + Sum_Int;			// интегральная составляющая
	// ограничиваем воздействие ±100%:
	if (I > 100) I = 100;
	if (I < -100) I = -100;
	
	
	D = Kd * (e - error_0) / 100L / 1000L;		// дифференциальная составляющая
	// ограничиваем воздействие ±100%:
	if (D > 100) D = 100;
	if (D < -100) D = -100;	
	

	U = P + I + D;
	
	// запоминаем необходимые данные для следующего шага:
	error_0 = e;
	Sum_Int = I;
	
	
	// ограничиваем результирующее воздействие ±100%:
	if (U > 100) U = 100;
	if (U < -100) U = -100;
	
	
	return U;
	
	
}


int Regulator_PID::_P(float valCur, float valSet)
{
	long U	= 0;
	long P	= 0;
	long I	= 0;
	long D	= 0;
	long e	= 0;
	
	
	e = long((valSet - valCur)* 100.0);		// вычисляем текущую ошибку (100.0 - множитель для выполнения целочисленных операций без потери необходимой точности)
	
	P = Kp * e / 100L / 1000L;					// пропроциональная составляющая
	// ограничиваем воздействие ±100%:
	if (P > 100) P = 100;
	if (P < -100) P = -100;	
	
	
/* 	I = Ki * e / 100L / 1000L + Sum_Int;			// интегральная составляющая
	// ограничиваем воздействие ±100%:
	if (I > 100) I = 100;
	if (I < -100) I = -100;
	
	
	D = Kd * (e - error_0) / 100L / 1000L;		// дифференциальная составляющая
	// ограничиваем воздействие ±100%:
	if (D > 100) D = 100;
	if (D < -100) D = -100;	
 */	

	U = P + I + D;
	
	// запоминаем необходимые данные для следующего шага:
	error_0 = e;
	//Sum_Int = I;
	
	
	// ограничиваем результирующее воздействие ±100%:
	if (U > 100) U = 100;
	if (U < -100) U = -100;
	
	
	return U;
	
	
}



int Regulator_PID::_PI(float valCur, float valSet)
{
	long U	= 0;
	long P	= 0;
	long I	= 0;
	long D	= 0;
	long e	= 0;
	
	
	e = long((valSet - valCur)* 100.0);		// вычисляем текущую ошибку (100.0 - множитель для выполнения целочисленных операций без потери необходимой точности)
	
	P = Kp * e / 100L / 1000L;					// пропроциональная составляющая
	// ограничиваем воздействие ±100%:
	if (P > 100) P = 100;
	if (P < -100) P = -100;	
	
	
	I = Ki * e / 100L / 1000L + Sum_Int;			// интегральная составляющая
	// ограничиваем воздействие ±100%:
	if (I > 100) I = 100;
	if (I < -100) I = -100;
	
	
/* 	D = Kd * (e - error_0) / 100L / 1000L;		// дифференциальная составляющая
	// ограничиваем воздействие ±100%:
	if (D > 100) D = 100;
	if (D < -100) D = -100;	
 */	

	U = P + I + D;
	
	// запоминаем необходимые данные для следующего шага:
	error_0 = e;
	Sum_Int = I;
	
	
	// ограничиваем результирующее воздействие ±100%:
	if (U > 100) U = 100;
	if (U < -100) U = -100;
	
	
	return U;
	
	
}


int Regulator_PID::_PD(float valCur, float valSet)
{
	long U	= 0;
	long P	= 0;
	long I	= 0;
	long D	= 0;
	long e	= 0;
	
	
	e = long((valSet - valCur)* 100.0);		// вычисляем текущую ошибку (100.0 - множитель для выполнения целочисленных операций без потери необходимой точности)
	
	P = Kp * e / 100L / 1000L;					// пропроциональная составляющая
	// ограничиваем воздействие ±100%:
	if (P > 100) P = 100;
	if (P < -100) P = -100;	
	
	
/* 	I = Ki * e / 100L / 1000L + Sum_Int;			// интегральная составляющая
	// ограничиваем воздействие ±100%:
	if (I > 100) I = 100;
	if (I < -100) I = -100;
 */	
	
	D = Kd * (e - error_0) / 100L / 1000L;		// дифференциальная составляющая
	// ограничиваем воздействие ±100%:
	if (D > 100) D = 100;
	if (D < -100) D = -100;	
	

	U = P + I + D;
	
	// запоминаем необходимые данные для следующего шага:
	error_0 = e;
	//Sum_Int = I;
	
	
	// ограничиваем результирующее воздействие ±100%:
	if (U > 100) U = 100;
	if (U < -100) U = -100;
	
	
	return U;
	
	
}

















