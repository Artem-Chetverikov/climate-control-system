
// Библиотека управления шаговым двигателем. Для переключения шагов используется внутренний таймер
// Управляющая функция класса SMotor_Timer::Step() передает в драйвер два сигнала: сигнал шага и направление вращения.
// Драйвера (на алгоритмы управления Step/Dir (тип 1), CW/CCW (тип 2), step4-en2) входят в состав библиотеки.
// Есть возможность задать требуемую скорость и ускорение разгона и торможения
// Если приводы на данный момент не в движении, таймер Timer1 выключается; при необходимости вновь активируется




#ifndef __STEPMOTORTIMER_H_INCLUDED
#define __STEPMOTORTIMER_H_INCLUDED

#include <include/ErrorAdministrator/ErrorAdministrator.h>

#define SPEED_SM_UP		1
#define SPEED_SM_EVENLY	2
#define SPEED_SM_DOWN	3




class SMotor_Timer : public ErrorAdministrator
{
	private:

		static int				_count_obj;					// счетчик объектов класса SMotor_Timer

		int 					_stepsPerRevolution;		// количество физических позиций шагового двигателя (по умолчанию = 200)
		int 					_divider;					// передделитель драйвера шагового двигателя (по умолчанию = 1)
		float					_reducer;					// передаточное число редуктора (по умолчанию = 1.0)
		float					_gisteresis;				// гистерезис редуктора из-за внутренних зазоров, об(по умолчанию = 0.0)


		float					_speedSM;					// скорость вращения StepMotor ЗАДАННАЯ об/мин (по умолчанию = _speedSM_MIN)
		float					_last_speedSM;				// скорость вращения StepMotor ПРЕДЫДУЩАЯ об/мин
		const float				_speedSM_MAX = 50.0;		// скорость вращения максимальная StepMotor об/мин (по умолчанию = 50.0)
		const float				_speedSM_MIN = 1.5;			// скорость вращения минимальная StepMotor об/мин (по умолчанию = 1.0)

		float					_accelSM_Up;				// ускорение разгона, об/мин сек(по умолчанию = _accelSM_Up_MIN)
		const float				_accelSM_Up_MAX = 100.0;	// ускорение разгона максимальное, об/мин сек (по умолчанию = 100.0)
		const float				_accelSM_Up_MIN = 1.0;		// ускорение разгона минимальное, об/мин сек (по умолчанию = 1.0)

		float					_accelSM_Down;				// ускорение торможения, об/мин сек(по умолчанию = _accelSM_Down_MIN)
		const float				_accelSM_Down_MAX = 100.0;	// ускорение торможения	максимальное, об/мин сек (по умолчанию = 100.0)
		const float				_accelSM_Down_MIN = 1.0;	// ускорение торможения минимальное, об/мин сек (по умолчанию = 1.0)
		
		volatile long 			_steps_left;				// текущее количество шагов до окончания при заданном количестве оборотов
		long 					_abs_steps;					// заданное количество шагов
		int						_last_curStep;				// предыдущий уровень сигнала в шаге мотора
		volatile long			_step_delay;				// текущая длительность шага, в мкс
		long					_step_delay_default;		// длительность шага при заданной скорости, мкс
		int						_mode_speed;				// режим: разгона - 1/торможения - 2/равномерно - 3
		float					_this_angle;				// заданный угол в пределах от 0° до 360° установки выходного вала редуктора
		
		int						_direction;					// текущее направление вращения StepMotor 1 или -1
		int						_last_direction;			// направление при предыдущем перемещении 1 или -1
		int						_pre_direction;				// множитель для калибровки StepMotor 1 или -1
		

		bool					_active;					// признак активности текущего привода(если true - разрешена работа привода)
		bool					_inMotion;					// признак рабочего хода текущего привода(если true - привод находится в движении)
		bool					_timer_active;				// признак активности таймера(если true - таймер включен)		
		bool					_shaft;						// признак режима удержания вала (если true - обмотки включены)
		bool					_turn_no_stop;				// признак режима вращения без остановки (если true - вращается до следующей команды)

		
	

		float					_delta_accel_up;			// длительность для реализации ускорения разгона
		float					_delta_accel_down;			// длительность для реализации ускорения торможения
		volatile int			_step_accel_up_count;		// счетчик текущего шага разгона
		volatile int			_step_accel_down_count;		// счетчик текущего шага торможения
		int						_step_accel_up_end;			// конец отрезка разгона		
		int						_step_accel_down_end;		// конец отрезка торможения
		volatile long			_timer_count;				// счетчик прерываний от таймера
		long					_period_timer;				// период счета таймера от 1 мкс до 4000 мкс (по умолчанию = 1000 мкс)



		driver_SM			_driver;						// указатель на внешний драйвер принимающий два параметра типа int: шаг и направление
		static const int	    AR_OBJ_LENGTH = 6;			// длина массива указателей на функции обработки шага (максимальное количество одновременно
															// работающих приводов; по умолчанию = 6)

		static volatile 		SMotor_Timer *_array_obj_timer[AR_OBJ_LENGTH];	// общий для объектов класса SMotor_Timer массив указателей на приводы,
																				// находящихся в состоянии рабочего хода
																				// при остановке привода, его указатель исключается из данного массива

		void					TimerStart();				// функция инициализации и запуска аппаратного таймера
		void					TimerStop();				// функция отключения аппаратного таймера
		long					compute_delay();			// вычисляет следующее значение длительности шага
		void					_accel_update();			// обновление массивов ускорений разгона и торможения


		
		
		
	public:

		SMotor_Timer(driver_SM& drv);				// конструктор объекта класса
		~SMotor_Timer();							// деструктор объекта класса

		void	SetDrv(driver_SM& drv);				// установить ссылку на программный драйвер шагового двигателя

		void	Set_StepsPR(int step);				// установить количество шагов за оборот шагового двигателя
		void	Set_Accel(float up, float down);	// установить ускорение разгона и торможения
		void	Set_Speed(float sp);				// установить скорость вращения шагового двигателя
		void	Set_Divider(int div);				// установить предделитель драйвера шагового двигателя
		void	Set_Reducer(float reduc);			// установить передаточное число редуктора
		void	Set_Gisteresis(float gist);			// установить гистерезис вала редуктора, об		
		void	Set_TimerPeriod(int period);		// установить период счета аппаратного таймера от 1 мкс до 4000 мкс (по умолчанию = 1000 мкс)
		void	Change_Dir();						// изменить _pre_direction на обратное вращение
		void	Set_ShaftFree();					// режим свободного вала
		void	Set_ShaftHold();					// режим удерживания вала

		void	GoStep(long step);					// повернуть на количество шагов (±)
		void	GoTurn(float turn);					// повернуть шаговый двигатель на количество оборотов (±)
		void	GoTurn();							// вращение без остановки в текущем направлении
		void	GoTurnReducer(float turn);			// повернуть выходной вал редуктора на количество оборотов (±)
		void	GoReducer360(float angle);			// повернуть выходной вал редуктора на заданный угол в пределах от 0° до 360°
		void	SetZero360();						// установить 0° в текущем положении выходного вала редуктора
		void	Stop();								// остановка текущего движения шагового двигателя с заданным ускорением
		void	StopNow();							// экстренная остановка текущего движения шагового двигателя
		
		bool	IsActiveDrv();						// true, если разрешено работать с приводом
		void	StartDrv();							// разрешить работу привода
		void	StopDrv();							// запретить работу привода

		void	Step();								// очередной шаг мотора
		
};




class driver_SM : public ErrorAdministrator 			// родительский класс для драйверов шагового двигателя
{
	protected:
	
		bool	_active;			// признак активности драйвера
		
		
	public:
		driver_SM();
		
		virtual void command(int st, int dir, int en);
		
		
		
};





class driver_SM_StepDir : public driver_SM 				// класс для драйвера Step/Dir
{
	protected:
	
		int		_pinStep;			// номера выходов, к которым подключены обмотки шагового двигателя
		int		_pinDir;			//

		
	public:
		driver_SM_StepDir(int pinst, int pindir);
		
};

class driver_SM_CwCcw : public driver_SM 				// класс для драйвера CW/CCW (тип 2)
{
	protected:

		int		_pinCW;				// номера выходов, к которым подключены обмотки шагового двигателя
		int		_pinCCW;			//


	public:
		driver_SM_CwCcw(int pincw, int pinccw);
		

};

class driver_SM_Step4En2 : public driver_SM		 		// класс для драйвера step4-en2
{
	protected:

		int		_pinA;				// номера выходов, к которым подключены обмотки шагового двигателя
		int		_pinB;				//
		int		_pinC;				//
		int		_pinD;				//

		int		_pinE1;				// номера выходов, к которым подключены разрешающие контакты драйвера
		int		_pinE2;				//
		
		int		_thisStep;			// шаг переключения обмоток
		
		int		_lastSignal;		// последний поступивший сигнал шага

	
	public:
		driver_SM_Step4En2(int pina, int pinb, int pinc, int pind, int pine1, int pine2);
		
		virtual void command(int st, int dir, int en);
		
};


















#endif

