#ifndef __DSSENSOR_SIMPL_H_INCLUDED
#define __DSSENSOR_SIMPL_H_INCLUDED

#include "HardwareSerial.h"
#include <cstdint>				// подключаем фиксированные типы данных (типа uint32_t и др.)





class MHZ19_Simpl : public SensorStr				// класс датчика MHZ19
{
	private:
	
		float				_CO2;				// текущий уровень СО2	
		
		uint8_t cmd[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};	// команда запроса уровня СО2
		uint8_t response[9];													// массив для ответа датчика СО2
	
		
		HardwareSerial	   	*_serial;			// указатель на используемый интерфейс Serial
		
		long				_time_out_MHZ19;	// тайм-аут опроса датчика MHZ19
		int 				_read_error;		// счетчик ошибок чтения с датчика MHZ19	
		
	public:	
	
		MHZ19_Simpl(HardwareSerial);	// конструктор класса
		
		void	Read();					// запросить данные с датчика	


		
};	
	
	
#endif