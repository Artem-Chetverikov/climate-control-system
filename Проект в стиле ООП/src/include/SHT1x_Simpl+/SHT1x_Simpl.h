#ifndef __SHT1x_SIMPL_H_INCLUDED
#define __SHT1x_SIMPL_H_INCLUDED


#include <cstdint>				// подключаем фиксированные типы данных (типа uint32_t и др.)
	
	
#include "SensorStruct.h"		// подключаем библиотеку для обобщения всех датчиков


//////////класс для опроса датчика и хранения значения температуры ////////////////////////////////////////////////////////////////////////
class SHT1x_Simpl_Hum;	// предварительное объявление класса

class SHT1x_Simpl_Temp  : public SensorStr				// класс датчика SHT1x (родительский класс SensorStr)
{
	private:
	
		float				_hum;				// текущая влажность
		
		int					_pinData;			// пин, к которому подключен датчик (шина данных)
		int					_pinClock;			// пин, к которому подключен датчик (синхоимпульс)
		
		const uint8_t gTempCmd  = 0b00000011;	// команд на конвертацию температуры
		const uint8_t gHumidCmd = 0b00000101;	// команд на конвертацию влажности
		
		// Conversion coefficients from SHT15 datasheet
		const float D1 = -40.0;	 // for 14 Bit @ 5V
		const float D2 =   0.01; // for 14 Bit DEGC
			// Conversion coefficients from SHT15 datasheet
		const float C1 = -4;				// for 12 Bit
		const float C2 =  0.0405;			// for 12 Bit
		const float C3 = -0.0000028;		// for 12 Bit
		const float T1 =  0.01;				// for 12 Bit
		const float T2 =  0.00008;			// for 12 Bit

		
		void sendCommandSHT(uint8_t);
		void skipCrcSHT();
		int getData16SHT();
		uint8_t shiftIn(uint8_t);
		void shiftOut(uint8_t, uint8_t);



		
	public:	
	
		SHT1x_Simpl_Temp(int pinD, int pinC);		// конструктор класса
		
		void	Read();				// запросить данные с датчика	
		
		friend SHT1x_Simpl_Hum;		// класс для хранения значения влажности - дружественный

};


//////////класс для хранения значения влажности	////////////////////////////////////////////////////////////////////////	
	
class SHT1x_Simpl_Hum : public SensorStr				// класс датчика SHT1x (родительский класс SensorStr)
{
	private:
	
		SHT1x_Simpl_Temp	*_sens;				// указатель на экземпляр датчика SHT1x
		
	public:	
	
		SHT1x_Simpl_Hum(SHT1x_Simpl_Temp &_sens);		// конструктор класса
		
		void	Read();				// запросить данные с датчика	

		
};



	




#endif