#ifndef __QUEUETIMERTASKFORCLASSES_H_INCLUDED
#define __QUEUETIMERTASKFORCLASSES_H_INCLUDED



using VoidFunc_ptr = void(*)(void);			// ��������� ��������� �� ������� ��� ����������, ������ �� ������������: void AnyFunc(void)  

const int		MAXTIMERSCOUNT	= 16;		// ������������ ����� ������������������ �������� (��� ��������� �������)
const int 		MAX_QUEUE		= 30;		// ������������ ����� ������� �����

class TaskElement;							// ��������������� ���������� ������


//=========================== ������

class TimerListForClasses
{
	private:
		void			Init();
		bool			_active;
		int				_error;

	public:
		TimerListForClasses();

		
	// ��� �������:
		// ���������� ������� ��� ���������� �� ��������� ������� (��� ���������� � ��������� �����):
		int		AddMilSec(VoidFunc_ptr AFunc, long timeMS);					// ������ �� ������� AFunc, ������� ���� ������� ����� timeMS �����������
		int		AddSeconds(VoidFunc_ptr AFunc, int timeSec);					// �� ��, ������ �������� �������� � ��������. 
		int		AddMinutes(VoidFunc_ptr AFunc, int timeMin);					// �� ��, ������ �������� �������� � �������. 
		// ���������� ������� ��� ��������� ����� � ��������� ����� � ������������:
		int		AddMilSec(VoidFunc_ptr AFunc, int priority, long timeMS);		// ������ �� ������� AFunc, ������� ���� ��������� � ������� ����� timeMS �����������
		int		AddSeconds(VoidFunc_ptr AFunc, int priority, int timeSec);		// �� ��, ������ �������� �������� � ��������. 
		int		AddMinutes(VoidFunc_ptr AFunc, int priroity, int timeMin);		// �� ��, ������ �������� �������� � �������. 

	
	// ��� ������� ������ TaskElement:
		// ���������� ������� ��� ���������� �� ��������� ������� (��� ���������� � ��������� �����):
		int		AddMilSec(TaskElement &AElem, long timeMS);						// ������ �� ��������� TaskElement, ����� Step() �������� ����� ������� ����� timeMS �����������
		int		AddSeconds(TaskElement &AElem, int timeSec);					// �� ��, ������ �������� �������� � ��������. 
		int		AddMinutes(TaskElement &AElem, int timeMin);					// �� ��, ������ �������� �������� � �������. 
		// ���������� ������� ��� ��������� ����� � ��������� ����� � ������������:
		int		AddMilSec(TaskElement &AElem, int priority, long timeMS);		// ������ �� ��������� TaskElement, ����� Step() �������� ����� ��������� � ������� ����� timeMS �����������
		int		AddSeconds(TaskElement &AElem, int priority, int timeSec);		// �� ��, ������ �������� �������� � ��������. 
		int		AddMinutes(TaskElement &AElem, int priroity, int timeMin);		// �� ��, ������ �������� �������� � �������. 


	
																				
																				
		bool	CanAdd();					// ���� ����� �������� ������, ������ true
		bool	IsActive();					// true, ���� ���� ���� ������ �������
		void	Delete(int hnd);			// ���������� � �������� ������ hnd
		void	Delete(TaskElement *AElem);	// ���������� � �������� ������ �� ��������� AElem
		void	DeleteAll();				// ���������� � �������� ��� �������
		
		void	Step(void);					// ���������� ������ 1 �� ( _1MSCONST )
		void	Start();					// ��������� ������� ���� �������� ��������
		void	Stop();						// TimerList ��������, ��� ������� ��������������

		void	TimerStop(int hnd);			// ���������� ��������� ������ �� ��� ������
		bool	TimerActive(int hnd);		// ������� �� ���������� ������
		void	TimerStart(int hnd);		// ��������� ��������� ������ (����� ���������) 
											// ���� �� ������������, � ���������� �������

		void	TimerNewInterval(int hnd, long newinterval);	// ������������� ����� �������� ������������ ��� ������� hnd
                                                                // � ��������� ��� (���� �� ��� ����������)
																
		int		Error();					// ������� ������ �������????
};



extern TimerListForClasses TimerList;				// ��������� ��� ������ � �������� (���������� ����� ����� ���������)






//=========================== ������� ������� �����

struct TaskStructFunc						// ���������� ��������� ��� �������� ������� (��� ��������� �������)
{
	
	bool				Active;				// ������� ���������� ������� ������
	int					PriorityFunc;		// ��������� ���������� ������� ��� ������� �����
	long				InitCounter;		// �������� ���������� �����������
	long				WorkingCounter;		// ������� �������. ����� ������ ����� ���� ���������� �������� �� InitCounter
											// � ������ ������������ ����������� �� 1. ��� ����������� 0 ���������� ������� 
											// Step(), � ����� ������������ �������� �� InitCounter. ���� ���������� �������
											
											
	VoidFunc_ptr		CallingFunc;		// �������, ������� ����� ������� ��� ������������	
};

struct Queue_StructFunc						// ���������� ��������� ��� �������� ������� ��������� ������
	{
		VoidFunc_ptr	CallingFunc;		// ��������� �� �������, ������� ����� �������
		int				priorityFunc;		// ��������� ������

	};


class TaskElement							// ������������ ����� ��� �������� ������ ������ (��� ������� ������)
{
protected:
	
	bool				_active;			// ������� ���������� ������� ������
	int					_priority;			// ��������� ���������� ������� ��� ������� ����� (0 - ����� ������� ���������)
	long				_initCounter;		// �������� ���������� �����������
	long				_workingCounter;	// ������� �������. ����� ������ ����� ���� ���������� �������� �� InitCounter
											// � ������ ������������ ����������� �� 1. ��� ����������� 0 ���������� ������� 
											// Step(), � ����� ������������ �������� �� InitCounter. ���� ���������� �������
											
	bool				_static;			// ������� ��������� � ����������� ����� ������ TaskElement
	
	TaskElement			*next_ptr;			// ��������� �� ��������� ��������� ������
	TaskElement			*pre_ptr;			// ��������� �� ���������� ��������� ������
	static TaskElement	*start_ptr;			// ��������� �� ������ ��������� ��������� ������
	static TaskElement	*end_ptr;			// ��������� �� ��������� ��������� ��������� ������
	

	
public:
	TaskElement();
	
	friend TimerListForClasses;				// ����� ������� - ������������� (��������� ����������)
	
	
	virtual void		Step();				// �������, ������� ����� ������� ��� ������������ (������������ � �������� ������)
	
		// ������� ������� � ����������� �������(�������� � ������ TaskElement)
	static bool				_initialisation;					// ������� ��������� ������������� ������ TaskElement
	
	static TaskStructFunc	_array_timers_func[MAXTIMERSCOUNT];	// ������ ��� �������� �������� (��� ��������� �������)
	
	static Queue_StructFunc	_queue_func[MAX_QUEUE];				// ���������� ������� �����(��� ��������� �������)

	void					OrganisateQueueFunc();				// ������������� ���������� ������� ����� �� ����������� (��� ��������� �������)

};



extern TaskElement		FuncElementStatic;			// ��� ������ �� ������������ ������ ������ TaskElement(���������� ����� ����� ��������)





//=========================== ������� �����

struct Queue_Struct							// ���������� ��������� ��� �������� ������� ��������� ������
	{
		TaskElement		*callingTask;		// ��������� �� ��������� TaskElement, ����� Step() �������� ����� �������
		int				priorityTask;		// ��������� ������ (0 - ����� ������� ���������)

	};
	
	
	
class Queue_List							// ����� ������� �����
{
	private:
		
		Queue_Struct		Items[MAX_QUEUE];			// ������ ������� �����
		bool				active;						// ������� ���������� ������� ����� (false - ������� �� ��������������)
		
		void	OrganisateQueue();						// ������������� ������� ����� �� ����������� (������ ���������� ������ � ����� ����� INTERRUPTS_OFF � INTERRUPTS_ON)
		int		error;									// ������� ������ ������� �����

	public:	
		Queue_List();									// ����������� ������

		bool	Add(int priority, VoidFunc_ptr AFunc);	// ��������� ������� AFunc � �������, ������ �� ���������
		void	Delete(VoidFunc_ptr AFunc);				// ������� ������� �� �������
		
		bool	Add(int priority, TaskElement &AElem);	// ��������� ����� Step() ���������� ������ TaskElement � �������, ������ ��� ���������
		void	Delete(TaskElement &AElem);				// ������� ����� Step() ���������� ������ TaskElement �� �������
		
		bool	IsActive();								// true, ���� ��������� ����� �������
		void	Step();									// ���������� ��������� ������� �� �������
		void	Start();								// ��������� ��������� �����
		void	Stop();									// ��������� ����� ��������		
		void	Clean();								// ������ ������� �������		
		int		Error();								// ������� ������ ������� �����

};

extern Queue_List QueueList;




#endif		//__QUEUETIMERTASKFORCLASSES_H_INCLUDED

