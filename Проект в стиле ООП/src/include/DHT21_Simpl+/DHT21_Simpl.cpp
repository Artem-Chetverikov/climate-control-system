#include "DHT21_Simpl.h"
#include "Transceiver_function.h"		// ���������� ���������������� ������������ ������� 
#include <cstdint>						// ���������� ������������� ���� ������ (���� uint32_t � ��.)



///////// ����� ��� ������ ������� � �������� �������� ����������� ////////////////////////////////////////
DHT21_Simpl_Temp::DHT21_Simpl_Temp(int pin)		// ����������� ������
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
		_lastreadtime = 0 ;		// ���  = millis();
		_error = 0;
		
}


void DHT21_Simpl_Temp::Read()		// ��������� ������ � ������� DHT21
{

	int Temp = 0;
	unsigned long currenttime = millis();
	
	if (currenttime < _lastreadtime)	_lastreadtime = 0;
	
	if (_firstreading && ((currenttime - _lastreadtime) < 4000)) return;		//�������������� ����� �� �����, ��� ����� 4 ���. ����� ��������� 

	if ((currenttime - _lastreadtime) < 2000) return; 				//����� �� ���� 1 ���� � 2 ���.
	

	_firstreading = false;
	_lastreadtime = millis();


	data[0] = data[1] = data[2] = data[3] = data[4] = 0;		// ����� ���������� ��������
	
	for (int i = 0; i < 83; i++)								// ����� ���������� ��������
	{
		cycles[i]	= 0;
	}
	
	
	PIN_MODE_IN(_pinSensor);			// ����� 
	PIN_SET_HIGH(_pinSensor);			// ���
	delay(5);								// �����

	
	PIN_MODE_OUT(_pinSensor);
	PIN_SET_LOW(_pinSensor);	// ������ �� �������� ������ �� 0,8 �� 20 ��

	
	delay(5);
	
	PIN_MODE_IN(_pinSensor);
	PIN_SET_HIGH(_pinSensor);

	
cli();		//��������� ���������� �� ����� ��������� ������ � �������

	
	_maxcycles = (16000000/1000/1000)*200;				// ����������� ������ ���������������� �� 200 ���.	
	
	
	cycles[0] = ExpectPulse(HIGH);	//�� 20 �� 200 ���
	
	if (0 == cycles[0])	 {
		
		_error++;
		return;
	}

	
	cycles[1] = ExpectPulse(LOW);	//�� 75 �� 85 ���
	
	if (0 == cycles[1])	 {
		
		_error++;
		return;
	}	
	
	cycles[2] = ExpectPulse(HIGH);	//�� 75 �� 85 ���
	
	if (0 == cycles[2])	 {
		
		_error++;
		return;
	}	
	


	// "0" low	time �� 48 �� 55 ���,
	
	// "0" high	time �� 22 �� 30 ���,
	// "1" high	time �� 68 �� 75 ���
	
	
	_maxcycles = (16000000/1000/1000)*75;				// ����������� ������ ���������������� �� 75 ���.
	
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

sei();			//��������� ����������

	for (int i = 0; i < 40; ++i)  {
		
		uint32_t lowCycles	= cycles[2 * i + 3];		// �������� � cycles[3]
		uint32_t highCycles = cycles[2 * i + 1 + 3];	// � cycles[4]
		
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


	
		//	���������� ��������� � �������	DHT21 �10:
		Hum = data[0];
		Hum *= 256;
		Hum += data[1];
		
		//	���������� ����������� � �������  DHT21 �10:
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

unsigned long DHT21_Simpl_Temp::ExpectPulse(bool level) 					 //������� ������ ��������/������� ������ � ������ �������  DHT21
{
	uint32_t count1 = 0;

	while (PIN_READ_DIGITAL(_pinSensor) == level) {
		if (count1++ >= _maxcycles) {
			return 0;
		}

	}
	return count1;
} //expectPulse(bool level)




///////// ����� ��� �������� �������� ��������� ////////////////////////////////////////
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



