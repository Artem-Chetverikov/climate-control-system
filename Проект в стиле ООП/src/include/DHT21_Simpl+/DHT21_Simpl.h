#ifndef __DHT21SIMPL_H_INCLUDED
#define __DHT21SIMPL_H_INCLUDED

#include "SensorStruct.h"	


class DHT21_Simpl_Hum; // предварительное объявление



//////////класс для опроса датчика и хранения значения температуры ////////////////////////////////////////////////////////////////////////

class DHT21_Simpl_Temp : public SensorStr				// класс датчика DH21 (родительский класс SensorStr)
{
	private:
	
		int					Hum;				// текущая влажность
		int					_pinSensor;			// пин, к которому подключен датчик
		unsigned long 		cycles[83];			// буфер для хранения приходящих битов от DHT 21 (3 стартовых цикла + 2*40 циклов данных)
		unsigned char 		data[5];			// массив для хранения ответа датчика DHT 21 (4 байта)
		unsigned long		_maxcycles ;		// максимальное число циклов при получении результата
		bool	   			_firstreading;		// флаг первоначального опроса
		unsigned long		_lastreadtime;		// время предыдущего опроса (пауза между опросами не менее 2 сек.)
		
		unsigned long		ExpectPulse(bool level);	//счетчик циклов высокого/низкого уровны в ответе датчика  DHT21
		
	public:	
	
		DHT21_Simpl_Temp(int pin);		// конструктор класса
		
		void	Read();				// запросить данные с датчика	
		
		friend DHT21_Simpl_Hum;		// класс для хранения значения влажности - дружественный

		
};	


	
//////////класс для хранения значения влажности	////////////////////////////////////////////////////////////////////////

class DHT21_Simpl_Hum : public SensorStr				// класс датчика DH21 (родительский класс SensorStr)
{
	private:
	
		DHT21_Simpl_Temp	*_sens;				// указатель на экземпляр датчика DHT21
		
	public:	
	
		DHT21_Simpl_Hum(DHT21_Simpl_Temp &_sens);		// конструктор класса
		
		void	Read();				// запросить данные с датчика	

		
};	
	
#endif