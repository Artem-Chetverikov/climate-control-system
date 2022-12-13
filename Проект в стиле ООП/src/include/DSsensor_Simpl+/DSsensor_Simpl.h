#ifndef __DSSENSOR_SIMPL_H_INCLUDED
#define __DSSENSOR_SIMPL_H_INCLUDED

#include "DallasTemperature.h"
#include "OneWire.h"
#include <cstdint>				// подключаем фиксированные типы данных (типа uint32_t и др.)
#include "SensorStruct.h"




class DSsensor_Simpl : public SensorStr				// класс датчика DS18b20
{
private:

	uint8_t 			_adres[8];			// адрес датчика
	OneWire	   			*_one_wire;			// указатель на используемый интерфейс OneWire
	int					_resolution;		// точность вычисления температуры

public:	

	DSsensor_Simpl(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, *OneWire, int);		// конструктор класса
	
	void	Read();				// запросить данные с датчика	
		
};	
	
	
extern DallasTemperature	sensors_temp;			// экземпляр класса работы с датчиками (видна глобально)
	
#endif