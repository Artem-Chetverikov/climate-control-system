DSsensor_Simpl

���������� �������������� � �������� ����������� DS18b20


������:
#include "DSsensor_Simpl.h"


OneWire oneWire_5(5);		// ��������� OneWire �� ���� 5



//���������� ��������� ������� (�����,,,,,,,, ��������� OneWire, �������� ����������( 9/10/11/12)):

DSsensor_Simpl 	 Temp_1(0x28, 0xFF, 0xD3, 0x59, 0x58, 0x16, 0x04, 0x6A, oneWire_5, 10);

Temp_1.Read();	//����� �������


t = Temp_1.ReturnVal();				
			
			

e = Temp_1.ReturnErr();		//����� ���������� ������ ��� ������


------------------------------------------------------------------




�������������� 26.05.2020 - 