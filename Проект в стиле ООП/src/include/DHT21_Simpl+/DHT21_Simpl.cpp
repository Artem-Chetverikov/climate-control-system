#include "DHT21_Simpl.h"
#include "Transceiver_function.h"		// подключаем пользовательские передаточные функции 
#include <cstdint>						// подключаем фиксированные типы данных (типа uint32_t и др.)



///////// класс для опроса датчика и хранения значения температуры ////////////////////////////////////////
DHT21_Simpl_Temp::DHT21_Simpl_Temp(int pin)		// конструктор класса
{		
		_pinSensor = pin;
		Hum			= -2730;	
		
		for (int i = 0; i < 83; i++)	 
		{
			cycles[i] = 0;
		}
		
		for (int i = 0; i < 4; i++)	 
		{
			data[i] = 0;
		}
		
		_maxcycles = 100 ;
		_firstreading = true;
		_lastreadtime = 0 ;		// или  = millis();
		_error = 0;
		
}


void DHT21_Simpl_Temp::Read()		// получение ответа с датчика DHT21
{

	int Temp = 0;
	unsigned long currenttime = millis();
	
	if (currenttime < _lastreadtime)	_lastreadtime = 0;
	
	if (_firstreading && ((currenttime - _lastreadtime) < 4000)) return;		//первоначальный опрос не ранее, чем через 4 сек. после включения 

	if ((currenttime - _lastreadtime) < 2000) return; 				//опрос не чаще 1 раза в 2 сек.
	

	_firstreading = false;
	_lastreadtime = millis();


	data[0] = data[1] = data[2] = data[3] = data[4] = 0;		// сброс предыдущих значений
	
	for (int i = 0; i < 83; i++)								// сброс предыдущих значений
	{
		cycles[i]	= 0;
	}
	
	
	PIN_MODE_IN(_pinSensor);			// можно 
	PIN_SET_HIGH(_pinSensor);			// без
	delay(5);								// этого

	
	PIN_MODE_OUT(_pinSensor);
	PIN_SET_LOW(_pinSensor);	// запрос на передачу данных от 0,8 до 20 мс

	
	delay(5);
	
	PIN_MODE_IN(_pinSensor);
	PIN_SET_HIGH(_pinSensor);

	
cli();		//запрещаем прерывания на время получения ответа с датчика

	
	_maxcycles = (16000000/1000/1000)*200;				// максимально циклов микроконтроллера за 200 мкс.	
	
	
	cycles[0] = ExpectPulse(HIGH);	//от 20 до 200 мкс
	
	if (0 == cycles[0])	 {
		
		_error++;
		return;
	}

	
	cycles[1] = ExpectPulse(LOW);	//от 75 до 85 мкс
	
	if (0 == cycles[1])	 {
		
		_error++;
		return;
	}	
	
	cycles[2] = ExpectPulse(HIGH);	//от 75 до 85 мкс
	
	if (0 == cycles[2])	 {
		
		_error++;
		return;
	}	
	


	// "0" low	time от 48 до 55 мкс,
	
	// "0" high	time от 22 до 30 мкс,
	// "1" high	time от 68 до 75 мкс
	
	
	_maxcycles = (16000000/1000/1000)*75;				// максимально циклов микроконтроллера за 75 мкс.
	
	for (int i = 3; i < 83; i += 2)	 {

		cycles[i]	= ExpectPulse(LOW);
		if (cycles[i] == 0){
			_error++;			
			return;
		}

		cycles[i + 1] = ExpectPulse(HIGH);
		if (cycles[i + 1] == 0){
			_error++;			
			return;
		}
	}

sei();			//разрешаем прерывания

	for (int i = 0; i < 40; ++i)  {
		
		uint32_t lowCycles	= cycles[2 * i + 3];		// начинаем с cycles[3]
		uint32_t highCycles = cycles[2 * i + 1 + 3];	// и cycles[4]
		
		if ((lowCycles == 0) || (highCycles == 0))	 {
			
			_error++;
			return;
		}
		data[i / 8] <<= 1;

		if (highCycles > lowCycles)	  {
			data[i / 8] |= 1;
		} 
	}


	if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))  {


	
		//	вычисление влажности с датчика	DHT21 х10:
		Hum = data[0];
		Hum *= 256;
		Hum += data[1];
		
		//	вычисление температуры с датчика  DHT21 х10:
		Temp = data[2] & 0x7F;
		Temp *= 256;
		Temp += data[3];
		if (data[2] & 0x80)
		{
			Temp *= -1;
		}
		
		_valueCur = ((float)(Temp))/10.0;

		_error = 0;

		return;
	}
	else {
		_error++;		
		return;
	}
	
	

}

unsigned long DHT21_Simpl_Temp::ExpectPulse(bool level) 					 //счетчик циклов высокого/низкого уровны в ответе датчика  DHT21
{
	uint32_t count1 = 0;

	while (PIN_READ_DIGITAL(_pinSensor) == level) {
		if (count1++ >= _maxcycles) {
			return 0;
		}

	}
	return count1;
} //expectPulse(bool level)




///////// класс для хранения значения влажности ////////////////////////////////////////
DHT21_Simpl_Hum::DHT21_Simpl_Hum(DHT21_Simpl_Temp &sens)
{
	_sens = &sens;
}


void DHT21_Simpl_Hum::Read()
{
	
	if (nullptr == _sens) return;
	
	if(_sens->Hum <= -2730)	_valueCur =  -1.0E+38;
	else	_valueCur =  ((float)(_sens->Hum))/10.0;
	

	_error = 	_sens->_error;
}



