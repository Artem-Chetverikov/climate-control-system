#include "DSsensor_Simpl.h"
#include "Transceiver_function.h"		// подключаем пользовательские передаточные функции 
#include <cstdint>				// подключаем фиксированные типы данных (типа uint32_t и др.)


DSsensor_Simpl::DSsensor_Simpl(uint8_t adr_0, uint8_t adr_1, uint8_t adr_2, uint8_t adr_3,
								uint8_t adr_4, uint8_t adr_5, uint8_t adr_6, uint8_t adr_7, OneWire *OWire, int res)		// конструктор класса
{		
	
	_adres[0]	= adr_0;
	_adres[1]	= adr_1;
	_adres[2]	= adr_2;
	_adres[3]	= adr_3;
	_adres[4]	= adr_4;
	_adres[5]	= adr_5;
	_adres[6]	= adr_6;
	_adres[7]	= adr_7;
	
	_one_wire	= OWire;
	_resolution	= res;
	
	sensors_temp.setOneWire(_one_wire);
	sensors_temp.begin();
	sensors_temp.setResolution(_adres, _resolution);
	

		
}


void DSsensor_Simpl::Read()		// получение ответа с датчика DHT21
{
	
		sensors_temp.setOneWire(_one_wire);
		sensors_temp.begin();
	
		sensors_temp.requestTemperatures();
		_valueCur = sensors_temp.getTempC(_adres);
		
		
}










