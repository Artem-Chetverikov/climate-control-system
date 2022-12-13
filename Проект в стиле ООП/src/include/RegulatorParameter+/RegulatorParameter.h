#ifndef __PARAMETERCLIMAT_H_INCLUDED
#define __PARAMETERCLIMAT_H_INCLUDED

#include "SensorStruct.h"
#include "NoiseFilter.h"
#include "ProcessRegulator.h"


using	Func_1int_ptr = void(*)(int);			// ��������� ��������� �� ������� 



class RegulatorParameter				// ����� ���������� ����������
{
	private:
	
		float					_paramSet;				// �������� �������� ���������
		float					_paramDelta;			// �������� ���������� �� ��������� �������� ��������� (�)
		float					_paramCur;				// ������� �������� ���������
		static const int		_length = 2;			// ����� ������� ���������� ��������
		float					_history_param[_length];	// ������ ���������� �������� ���������
		int						_count_pos;				// ������� ������� ��� ������ � ������ ��������
		float					_paramFilt;				// ������������� �������� ���������
		bool					_first_get_val;			// ���� ������� ������� �������� � �������
		bool					_filt_default;			// ���� ���������� ���������� ������� �� ���������
		int						_percent;				// �������� ����������� � % (�100%))
		SensorStr				*_sensor;				// ��������� �� ������� ������ �������� �������� ��������� �� �������, ������������ float
		NoiseFilter				*_filter;				// ������� ������ ����� (� ����������: ������� ��������), ������������ float
		Func_1int_ptr			_control_func;			// ��������� �� �������, ������� ����� ������� ��� ���������� ����������� �������� ����������
														// (� ����������: ����������� ����������� �100%), ������ �� ������������
		ProcessRegulator		*_regulator;			// ��������� �� �������, ������� ����� ������� ��� ���������� ������������ �����������
														// (� �����������: ������� ��������, �������� ��������), ������������ ����������� ����������� �100%

		RegulatorParameter			*pre_ptr;			// ��������� �� ���������� ��������� ������
		RegulatorParameter			*next_ptr;			// ��������� �� ��������� ��������� ������
		static RegulatorParameter	*end_ptr;			// ��������� �� ��������� ��������� ��������� ������
		
		
	public:
	// ����������� ������, �������� �������, ���������� ������� �������� ���������,   ������� ������������    �    ������� �����������
		RegulatorParameter(SensorStr *Func_read, ProcessRegulator *Func_reg, Func_1int_ptr Func_contr);				
		
		
		// ������ ������� �������� ���������, ������ (����������� ����� � ����. �������� (10,2), �� ����� param = 8...12 )
		void	Set_Param(float, float);
		
		// ������ ������� �������� ���������, ������ �� ��������� �2,5%
		void	Set_Param(float);

		// ������ �������, ���������� ������� �������� ���������,   ������� ������������    �    ������� ����������� 		
		void	Set_Func(SensorStr*, ProcessRegulator*, Func_1int_ptr);
	
		void	SetFilt(NoiseFilter*);					// ���������� ������� ������
		
		void	DelFilt();								// ������� ������� ������ �����
		
		void	Filt_off();								// ��������� ��� ������� �����
		
		void	Filt_on();								// �������� ��� ������� �����
	
		void	Step();									// ���������� ���������� ���� ������������� ���������


};


#endif