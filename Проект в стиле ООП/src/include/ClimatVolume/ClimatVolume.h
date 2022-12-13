#ifndef __CLIMATVOLUME_H_INCLUDED
#define __CLIMATVOLUME_H_INCLUDED

#include "ParameterClimat.h"



using	Func_1int_ptr = void(*)(int);	// ��������� ��������� �� ������� ������������� (� ����������: ��������� ���������� �100%), ������ �� ������������



	
class Volume					// ����� ���������� �������������
{
	private:
	

		const int			length = 4;					// ����� ������� �������������� ����������
		RegulatorParam		*param[length];				// ������ ���������� �� �������������� ��������� ������������ � ������
		
		Volume				*pre_ptr					// ��������� �� ���������� ��������� ���������
		static Volume		*end_ptr					// ��������� �� ��������� ��������� ��������� ��������
		
		bool				active;						// ������� ���������� ���������� ������������� � ������ ������
		int					error;						// ������� ������ ��� ���������� �������������
		
	public:	
	
		Volume(RegulatorParam Param_1, ...);					// ����������� ������
		
		void	Regulate();					// ���������� ���������� ���� ������������� �������
		
		bool AddParam(RegulatorParam param);		// �������� �������������� �������� ������������ � ������ ������
		bool DelParam(RegulatorParam param);		// ������� �������� ������������
		bool IsControl(RegulatorParam param);		// true, ���� ������ �������� �������������� � ������ ������

		void	StartClim();				// ��������� ���������� ������������� � ������ ������
		void	StopClim();					// ���������� ���������� ������������� � ������ ������
		bool	IsActive();					// true, ���� ������������ ���������� ������������� � ������ ������
		int		Error();					// ������� ������

};


#endif