ErrorAdministrator

���������� ����� � ����������� ������


������:
#include "ErrorAdministrator.h"

ErrorAdministrator	AAA1;

AAA1.AddErr();


AAA1.NullErr();





ErrorAdministrator *ptr_EA = ErrorAdministrator::Return_start();
	
while (1)
{
	if(true == ptr_EA->IsAdmin())
	{
		

		
			
	}
				
	ptr_EA = ptr_EA->Return_next();
	if( ptr_EA == ErrorAdministrator::Return_start()) break;
}



------------------------------------------------------------------




�������������� 27.10.2020 - �������