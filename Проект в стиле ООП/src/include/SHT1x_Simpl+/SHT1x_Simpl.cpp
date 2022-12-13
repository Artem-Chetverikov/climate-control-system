#include "SHT1x_Simpl.h"
#include "Transceiver_function.h"		// подключаем пользовательские передаточные функции 
#include <cstdint>				// подключаем фиксированные типы данных (типа uint32_t и др.)



#ifndef LSBFIRST
#define LSBFIRST 0
#endif

#ifndef MSBFIRST
#define MSBFIRST 1
#endif





///////// класс для опроса датчика и хранения значения температуры ////////////////////////////////////////

SHT1x_Simpl_Temp::SHT1x_Simpl_Temp(int pinD, int pinC)		// конструктор класса
{		

	_pinData	= pinD;
	_pinClock	= pinC;
	_hum		= -1.0E+38;





	// Определяем переменные для работы с датчиком SHT1x:
/*
	int readStatus_SHT1x  = 0b00000111;	  //команда на чтение статусного регистра
	int writeStatus_SHT1x = 0b00000110;	  //команда на запись статусного регистра
	int softReset_SHT1x	  = 0b00011110;	  //команда на перезагрузку датчика SHT1x (установка флагов регистра по умолчанию)
	int ask_SHT1x;

	sendCommandSHT(softReset_SHT1x);

	delay (100);

	sendCommandSHT(readStatus_SHT1x);

	ask_SHT1x = getData16SHT(_pinData, _pinClock);
	skipCrcSHT(_pinData, _pinClock);

*/







		
}

// функции датчика SHT1x
void SHT1x_Simpl_Temp::Read()
{
	
	
	
	
	sendCommandSHT(gTempCmd);		// отправка команды на конвертацию температуры
	


	// ожидаем не менее 2 мс
	delay (2);
	int N_delay = 0;
	while (HIGH == PIN_READ_DIGITAL(_pinData)) //ждем, пока датчик ответит
	{			
		delay (2);
		N_delay++;
		if (N_delay > 10) 
		{
			_error++;
			return;
		}
	}	
	
	

	int _val;



	_val = getData16SHT();
	skipCrcSHT();

	// Convert raw value to degrees Celsius
	_valueCur = ((float)_val * D2) + D1;
	
	
		
	if (_valueCur <= -60.0 || _valueCur >= 150.0){			//если возвращаемая температура меньше 60°С и больше 150°С
		error_array_N[error_SHT1x] = 1;
		error();
	}

	

	sendCommandSHT(gHumidCmd);		// отправка команды на конвертацию влажности




	// ожидаем не менее 2 мс
	delay (2);
	N_delay = 0;
	while (HIGH == PIN_READ_DIGITAL(_pinData)) //ждем, пока датчик ответит
	{			
		delay (2);
		N_delay++;
		if (N_delay > 10) 
		{
			_error++;
			return;
		}
	}
	
	
	
	_val = 0;					 // Raw humidity value returned from sensor
	float _linearHumidity;		 // Humidity with linear correction applied

	_val = getData16SHT();
	skipCrcSHT();

	// Apply linear conversion to raw value
	_linearHumidity = C1 + C2 * (float)_val + C3 * (float)_val * (float)_val;

	// Correct humidity value for current temperature
	_hum = (_valueCur - 25.0 ) * (T1 + T2 * (float)_val) + _linearHumidity;

	if (_hum <= 1.0L || _hum >= 99.9){			//если возвращаемая влажность меньше 1% и больше 99%
		error_array_N[error_SHT1x] = 1;
		error();
	}	
	
	
	if (error_array_N[error_SHT1x] == 1){
		error_array_N[error_SHT1x] = 0;
		error();
	}
	

}


void SHT1x_Simpl_Temp::sendCommandSHT(uint8_t _command)
{
  int ack;

  // Transmission Start
  PIN_MODE_OUT(_pinData);
  PIN_MODE_OUT(_pinClock);
  PIN_SET_HIGH(_pinData);
  PIN_SET_HIGH(_pinClock);
  PIN_SET_LOW(_pinData);
  PIN_SET_LOW(_pinClock);
  PIN_SET_HIGH(_pinClock);
  PIN_SET_HIGH(_pinData);
  PIN_SET_LOW(_pinClock);

  // The command (3 msb are address and must be 000, and last 5 bits are command)
  shiftOut(MSBFIRST, _command);

  // Verify we get the correct ack
  PIN_SET_HIGH(_pinClock);
  PIN_MODE_IN(_pinData);
  ack = PIN_READ_DIGITAL(_pinData);
  if (ack != LOW) {
	//Serial.println("Ack Error 0");
  }
  PIN_SET_LOW(_pinClock);
  ack = PIN_READ_DIGITAL(_pinData);
  if (ack != HIGH) {
	//Serial.println("Ack Error 1");
  }
}


void SHT1x_Simpl_Temp::skipCrcSHT()
{
  // Skip acknowledge to end trans (no CRC)
  PIN_MODE_OUT(_pinData);
  PIN_MODE_OUT(_pinClock);

  PIN_SET_HIGH(_pinData);
  PIN_SET_HIGH(_pinClock);
  PIN_SET_LOW(_pinClock);
}


int SHT1x_Simpl_Temp::getData16SHT()
{
  int val;

  // Get the most significant bits
  PIN_MODE_IN(_pinData);
  PIN_MODE_OUT(_pinClock);
  val = shiftIn(8);
  val *= 256;

  // Send the required ack
  PIN_MODE_OUT(_pinData);
  PIN_SET_HIGH(_pinData);
  PIN_SET_LOW(_pinData);
  PIN_SET_HIGH(_pinClock);
  PIN_SET_LOW(_pinClock);

  // Get the least significant bits
  PIN_MODE_IN(_pinData);
  val |= shiftIn(8);

  return val;
}


uint8_t SHT1x_Simpl_Temp::shiftIn(uint8_t _numBits)
{
  int ret = 0;
  int i;

  for (i=0; i<_numBits; ++i)
  {
	 PIN_SET_HIGH(_pinClock);
	 delay(2);								  // I don't know why I need this, but without it I don't get my 8 lsb of temp
	 ret = ret*2 + PIN_READ_DIGITAL(_pinData);
	 PIN_SET_LOW(_pinClock);
  }

  return(ret);
}


/*uint8_t SHT1x_Simpl_Temp::shiftIn(uint8_t bitOrder) 
{
	uint8_t value = 0;
	uint8_t i;

	for (i = 0; i < 8; ++i) {
		PIN_SET_HIGH(_pinClock);
		if (bitOrder == LSBFIRST)
			value |= PIN_READ_DIGITAL(_pinData) << i;
		else
			value |= PIN_READ_DIGITAL(_pinData) << (7 - i);
		PIN_SET_LOW(_pinClock);
	}
	return value;
}*/


void SHT1x_Simpl_Temp::shiftOut(uint8_t bitOrder, uint8_t val)
{
	uint8_t i;

	for (i = 0; i < 8; i++)  {
		if (bitOrder == LSBFIRST)
			(!!(val & (1 << i)))?PIN_SET_HIGH(_pinData):PIN_SET_LOW(_pinData);
		else	
			(!!(val & (1 << (7 - i))))?PIN_SET_HIGH(_pinData):PIN_SET_LOW(_pinData);
		
		PIN_SET_HIGH(_pinClock);
		delay(2);
		PIN_SET_LOW(_pinClock);		
	}
}














///////// класс для хранения значения влажности ////////////////////////////////////////
SHT1x_Simpl_Hum::SHT1x_Simpl_Hum(SHT1x_Simpl_Temp &sens)
{
	_sens = &sens;
}


void SHT1x_Simpl_Hum::Read()
{
	
	if (nullptr == _sens) return;
	
	_valueCur =  _sens->_hum;
	

	_error = 	_sens->_error;
}


