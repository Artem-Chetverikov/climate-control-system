
// ���������� ���������� ������� ����������. ��� ������������ ����� ������������ ���������� ������
// ����������� ������� ������ SMotor_Timer::Step() �������� � ������� ��� �������: ������ ���� � ����������� ��������.
// �������� (�� ��������� ���������� Step/Dir (��� 1), CW/CCW (��� 2), step4-en2) ������ � ������ ����������.
// ���� ����������� ������ ��������� �������� � ��������� ������� � ����������
// ���� ������� �� ������ ������ �� � ��������, ������ Timer1 �����������; ��� ������������� ����� ������������




#ifndef __STEPMOTORTIMER_H_INCLUDED
#define __STEPMOTORTIMER_H_INCLUDED

#include <include/ErrorAdministrator/ErrorAdministrator.h>

#define SPEED_SM_UP		1
#define SPEED_SM_EVENLY	2
#define SPEED_SM_DOWN	3




class SMotor_Timer : public ErrorAdministrator
{
	private:

		static int				_count_obj;					// ������� �������� ������ SMotor_Timer

		int 					_stepsPerRevolution;		// ���������� ���������� ������� �������� ��������� (�� ��������� = 200)
		int 					_divider;					// ������������� �������� �������� ��������� (�� ��������� = 1)
		float					_reducer;					// ������������ ����� ��������� (�� ��������� = 1.0)
		float					_gisteresis;				// ���������� ��������� ��-�� ���������� �������, ��(�� ��������� = 0.0)


		float					_speedSM;					// �������� �������� StepMotor �������� ��/��� (�� ��������� = _speedSM_MIN)
		float					_last_speedSM;				// �������� �������� StepMotor ���������� ��/���
		const float				_speedSM_MAX = 50.0;		// �������� �������� ������������ StepMotor ��/��� (�� ��������� = 50.0)
		const float				_speedSM_MIN = 1.5;			// �������� �������� ����������� StepMotor ��/��� (�� ��������� = 1.0)

		float					_accelSM_Up;				// ��������� �������, ��/��� ���(�� ��������� = _accelSM_Up_MIN)
		const float				_accelSM_Up_MAX = 100.0;	// ��������� ������� ������������, ��/��� ��� (�� ��������� = 100.0)
		const float				_accelSM_Up_MIN = 1.0;		// ��������� ������� �����������, ��/��� ��� (�� ��������� = 1.0)

		float					_accelSM_Down;				// ��������� ����������, ��/��� ���(�� ��������� = _accelSM_Down_MIN)
		const float				_accelSM_Down_MAX = 100.0;	// ��������� ����������	������������, ��/��� ��� (�� ��������� = 100.0)
		const float				_accelSM_Down_MIN = 1.0;	// ��������� ���������� �����������, ��/��� ��� (�� ��������� = 1.0)
		
		volatile long 			_steps_left;				// ������� ���������� ����� �� ��������� ��� �������� ���������� ��������
		long 					_abs_steps;					// �������� ���������� �����
		int						_last_curStep;				// ���������� ������� ������� � ���� ������
		volatile long			_step_delay;				// ������� ������������ ����, � ���
		long					_step_delay_default;		// ������������ ���� ��� �������� ��������, ���
		int						_mode_speed;				// �����: ������� - 1/���������� - 2/���������� - 3
		float					_this_angle;				// �������� ���� � �������� �� 0� �� 360� ��������� ��������� ���� ���������
		
		int						_direction;					// ������� ����������� �������� StepMotor 1 ��� -1
		int						_last_direction;			// ����������� ��� ���������� ����������� 1 ��� -1
		int						_pre_direction;				// ��������� ��� ���������� StepMotor 1 ��� -1
		

		bool					_active;					// ������� ���������� �������� �������(���� true - ��������� ������ �������)
		bool					_inMotion;					// ������� �������� ���� �������� �������(���� true - ������ ��������� � ��������)
		bool					_timer_active;				// ������� ���������� �������(���� true - ������ �������)		
		bool					_shaft;						// ������� ������ ��������� ���� (���� true - ������� ��������)
		bool					_turn_no_stop;				// ������� ������ �������� ��� ��������� (���� true - ��������� �� ��������� �������)

		
	

		float					_delta_accel_up;			// ������������ ��� ���������� ��������� �������
		float					_delta_accel_down;			// ������������ ��� ���������� ��������� ����������
		volatile int			_step_accel_up_count;		// ������� �������� ���� �������
		volatile int			_step_accel_down_count;		// ������� �������� ���� ����������
		int						_step_accel_up_end;			// ����� ������� �������		
		int						_step_accel_down_end;		// ����� ������� ����������
		volatile long			_timer_count;				// ������� ���������� �� �������
		long					_period_timer;				// ������ ����� ������� �� 1 ��� �� 4000 ��� (�� ��������� = 1000 ���)



		driver_SM			_driver;						// ��������� �� ������� ������� ����������� ��� ��������� ���� int: ��� � �����������
		static const int	    AR_OBJ_LENGTH = 6;			// ����� ������� ���������� �� ������� ��������� ���� (������������ ���������� ������������
															// ���������� ��������; �� ��������� = 6)

		static volatile 		SMotor_Timer *_array_obj_timer[AR_OBJ_LENGTH];	// ����� ��� �������� ������ SMotor_Timer ������ ���������� �� �������,
																				// ����������� � ��������� �������� ����
																				// ��� ��������� �������, ��� ��������� ����������� �� ������� �������

		void					TimerStart();				// ������� ������������� � ������� ����������� �������
		void					TimerStop();				// ������� ���������� ����������� �������
		long					compute_delay();			// ��������� ��������� �������� ������������ ����
		void					_accel_update();			// ���������� �������� ��������� ������� � ����������


		
		
		
	public:

		SMotor_Timer(driver_SM& drv);				// ����������� ������� ������
		~SMotor_Timer();							// ���������� ������� ������

		void	SetDrv(driver_SM& drv);				// ���������� ������ �� ����������� ������� �������� ���������

		void	Set_StepsPR(int step);				// ���������� ���������� ����� �� ������ �������� ���������
		void	Set_Accel(float up, float down);	// ���������� ��������� ������� � ����������
		void	Set_Speed(float sp);				// ���������� �������� �������� �������� ���������
		void	Set_Divider(int div);				// ���������� ������������ �������� �������� ���������
		void	Set_Reducer(float reduc);			// ���������� ������������ ����� ���������
		void	Set_Gisteresis(float gist);			// ���������� ���������� ���� ���������, ��		
		void	Set_TimerPeriod(int period);		// ���������� ������ ����� ����������� ������� �� 1 ��� �� 4000 ��� (�� ��������� = 1000 ���)
		void	Change_Dir();						// �������� _pre_direction �� �������� ��������
		void	Set_ShaftFree();					// ����� ���������� ����
		void	Set_ShaftHold();					// ����� ����������� ����

		void	GoStep(long step);					// ��������� �� ���������� ����� (�)
		void	GoTurn(float turn);					// ��������� ������� ��������� �� ���������� �������� (�)
		void	GoTurn();							// �������� ��� ��������� � ������� �����������
		void	GoTurnReducer(float turn);			// ��������� �������� ��� ��������� �� ���������� �������� (�)
		void	GoReducer360(float angle);			// ��������� �������� ��� ��������� �� �������� ���� � �������� �� 0� �� 360�
		void	SetZero360();						// ���������� 0� � ������� ��������� ��������� ���� ���������
		void	Stop();								// ��������� �������� �������� �������� ��������� � �������� ����������
		void	StopNow();							// ���������� ��������� �������� �������� �������� ���������
		
		bool	IsActiveDrv();						// true, ���� ��������� �������� � ��������
		void	StartDrv();							// ��������� ������ �������
		void	StopDrv();							// ��������� ������ �������

		void	Step();								// ��������� ��� ������
		
};




class driver_SM : public ErrorAdministrator 			// ������������ ����� ��� ��������� �������� ���������
{
	protected:
	
		bool	_active;			// ������� ���������� ��������
		
		
	public:
		driver_SM();
		
		virtual void command(int st, int dir, int en);
		
		
		
};





class driver_SM_StepDir : public driver_SM 				// ����� ��� �������� Step/Dir
{
	protected:
	
		int		_pinStep;			// ������ �������, � ������� ���������� ������� �������� ���������
		int		_pinDir;			//

		
	public:
		driver_SM_StepDir(int pinst, int pindir);
		
};

class driver_SM_CwCcw : public driver_SM 				// ����� ��� �������� CW/CCW (��� 2)
{
	protected:

		int		_pinCW;				// ������ �������, � ������� ���������� ������� �������� ���������
		int		_pinCCW;			//


	public:
		driver_SM_CwCcw(int pincw, int pinccw);
		

};

class driver_SM_Step4En2 : public driver_SM		 		// ����� ��� �������� step4-en2
{
	protected:

		int		_pinA;				// ������ �������, � ������� ���������� ������� �������� ���������
		int		_pinB;				//
		int		_pinC;				//
		int		_pinD;				//

		int		_pinE1;				// ������ �������, � ������� ���������� ����������� �������� ��������
		int		_pinE2;				//
		
		int		_thisStep;			// ��� ������������ �������
		
		int		_lastSignal;		// ��������� ����������� ������ ����

	
	public:
		driver_SM_Step4En2(int pina, int pinb, int pinc, int pind, int pine1, int pine2);
		
		virtual void command(int st, int dir, int en);
		
};


















#endif

