#include "MHZ19_Simpl.h"
#include <cstdint>				// ���������� ������������� ���� ������ (���� uint32_t � ��.)



MHZ19_Simpl::MHZ19_Simpl(HardwareSerial *port)		// ����������� ������
{		
				
		_serial		= port;
		
		for (int i =0; i < 9; i++) response[i] = 0;
		

		
		_read_error 	= 0;
		
}


void MHZ19_Simpl::Read()		// ��������� ������ � ������� DHT21
{
	
	// ����� ������� ��2 MHZ19
	_serial->.write(cmd, 9);							//�������� ������� � ������ ��2
	
	
	for (int i = 0; i < 9 ; i++){		//��������� ������� ������
		response[i] = 0;
	}
	

	_time_out_MHZ19 = millis();

	int i1 = 0;
	while (int i1 < 9) {
		
		if(_serial->.available()){
			response[i1] = _serial->.read();
			i1++;
		}
		
		if (millis() < _time_out_MHZ19) {					// ��� ������������ millis()
			_time_out_MHZ19 = 0;
		}

		
		if((millis() - _time_out_MHZ19) > 100){
			
				_read_error++;
				return;
		}
		
	}
	
		
	
	//�������� ����������� ����� ������ ������� ��2:
	uint8_t crc = 0;
	for (int i = 1; i < 8; i++) {
		crc += response[i];
	}
	crc = 255 - crc;
	crc++;

	//���������� ������ ��2 �� ������ �������:
	if ( !(response[0] == 0xFF && response[1] == 0x86 && response[8] == crc) ) {

		_read_error++;

	}
	else {
		unsigned int responseHigh = (unsigned int) response[2];
		unsigned int responseLow = (unsigned int) response[3];
		_valueCur = (float)((256 * responseHigh) + responseLow);
		_read_error = 0;
		
		return;
	}

		
		
}











