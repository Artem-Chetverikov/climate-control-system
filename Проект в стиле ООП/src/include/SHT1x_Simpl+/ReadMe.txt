SHT1x_Simpl

���������� �������������� � �������� ����������� � ��������� SHT1x


������:
#include "SHT1x_Simpl.h"


SHT1x_Simpl_Temp 	SHT1x_1_temp(10, 12);		//���������� ������ �� data-���� �10  clock-���� �12
SHT1x_Simpl_Hum 	SHT1x_1_hum(SHT1x_1_temp);	//���������� ������ �� data-���� �10  clock-���� �12


SHT1x_1_temp.Read();	//����� �������
SHT1x_1_hum.Read();	//����� �������


t = SHT1x_1_temp.ReturnVal();				
h =SHT1x_1_hum.ReturnVal();				
			

err1 = SHT1x_1_temp.ReturnErr();		//����� ���������� ������ ��� ������

err2 = SHT1x_1_hum.ReturnErr();	



------------------------------------------------------------------




�������������� 02.11.2020 - �������