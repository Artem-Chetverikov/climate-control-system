
#include "QueueTimerTaskForClasses.h"
#include "Transceiver_function.h"


TaskElement			FuncElementStatic;
Queue_List			QueueList;
TimerListForClasses	TimerList;


bool InRange(int value, int min, int max)               // ���������� true, ���� value ����� � ��������� �� min �� max (������������)
{
	return (value >= min) && (value <= max);
}





//=========================== ������


TimerListForClasses::TimerListForClasses() : 		// ����������� ������
	_active {false},
	_error {0}
{
	FuncElementStatic._static = true;		// ��� ������ �� ������������ ������ ������ TaskElement

}

int TimerListForClasses::AddMilSec(VoidFunc_ptr AFunc, long timeMS)
{
	for (int i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (FuncElementStatic._array_timers_func[i].CallingFunc == nullptr)			// ����������� �� ������ ��������
		{                                         					// ���� ����� ������ �����, ��������� ������
			INTERRUPTS_OFF;
			
			FuncElementStatic._array_timers_func[i].CallingFunc		= AFunc;
			FuncElementStatic._array_timers_func[i].PriorityFunc	= 0;
			FuncElementStatic._array_timers_func[i].InitCounter		= timeMS;
			FuncElementStatic._array_timers_func[i].WorkingCounter	= timeMS;
			FuncElementStatic._array_timers_func[i].Active			= true;
			
			if (false == _active)  Start();
			
			INTERRUPTS_ON;
			
			return i;
		}
	}
	return -1;                                   // ���� ��� - ������ ��� ������ (-1) 
}

int TimerListForClasses::AddSeconds(VoidFunc_ptr AFunc, int timeSec)
{
	return AddMilSec(AFunc, 1000L*timeSec);          // ���������� ��� ������
}

int TimerListForClasses::AddMinutes(VoidFunc_ptr AFunc, int timeMin)
{
	return AddMilSec(AFunc, timeMin*60L*1000L);     // ���������� ��� �����
}

int TimerListForClasses::AddMilSec(VoidFunc_ptr AFunc, int priority, long timeMS)
{
	for (int i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (FuncElementStatic._array_timers_func[i].CallingFunc == nullptr)			// ����������� �� ������ ��������
		{                                         								// ���� ����� ������ �����, ��������� ������
			INTERRUPTS_OFF;
			
			FuncElementStatic._array_timers_func[i].CallingFunc		= AFunc;
			FuncElementStatic._array_timers_func[i].PriorityFunc	= priority;
			FuncElementStatic._array_timers_func[i].InitCounter		= timeMS;
			FuncElementStatic._array_timers_func[i].WorkingCounter	= timeMS;
			FuncElementStatic._array_timers_func[i].Active			= true;
			
			if (false == _active)  Start();
			
			INTERRUPTS_ON;
			
			return i;
		}
	}
	return -1;                                   // ���� ��� - ������ ��� ������ (-1)
}

int TimerListForClasses::AddSeconds(VoidFunc_ptr AFunc, int priority, int timeSec)
{
	return AddMilSec(AFunc, priority, 1000L*timeSec);          // ���������� ��� ������
}

int TimerListForClasses::AddMinutes(VoidFunc_ptr AFunc, int priority, int timeMin)
{
	return AddMilSec(AFunc, priority, timeMin*60L*1000L);     // ���������� ��� �����
}


int TimerListForClasses::AddMilSec(TaskElement &AElem, long timeMS)
{

	INTERRUPTS_OFF;
	
	AElem._active			= true;
	AElem._priority			= 0;
	AElem._initCounter		= timeMS;
	AElem._workingCounter	= timeMS;

	if (false == _active)  Start();
	
	INTERRUPTS_ON;
		
	
	
	return -1; 
}

int TimerListForClasses::AddSeconds(TaskElement &AElem, int timeSec)
{
	return AddMilSec(AElem, 1000L*timeSec);          // ���������� ��� ������
}

int TimerListForClasses::AddMinutes(TaskElement &AElem, int timeMin)
{
	return AddMilSec(AElem, timeMin*60L*1000L);     // ���������� ��� �����
}

int TimerListForClasses::AddMilSec(TaskElement &AElem, int priority, long timeMS)
{
	INTERRUPTS_OFF;
	
	AElem._active			= true;
	AElem._priority			= priority;
	AElem._initCounter		= timeMS;
	AElem._workingCounter	= timeMS;

	if (false == _active)  Start();
	
	INTERRUPTS_ON;
		
	
	
	return 1; 
}

int TimerListForClasses::AddSeconds(TaskElement &AElem, int priority, int timeSec)
{
	return AddMilSec(AElem, priority, 1000L*timeSec);          // ���������� ��� ������
}

int TimerListForClasses::AddMinutes(TaskElement &AElem, int priority, int timeMin)
{
	return AddMilSec(AElem, priority, timeMin*60L*1000L);     // ���������� ��� �����
}



bool TimerListForClasses::CanAdd()
{
	for (int i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (FuncElementStatic._array_timers_func[i].CallingFunc != nullptr) continue;  // ���� � ������ ���� ������ �����, ���� ����� �������� ������
		return true;                                 // ������ true
	}
	return false;                                    // ���� ��� - �� false
}

bool TimerListForClasses::IsActive()
{
	return _active;                                   // ���� ���� ���� ������ �������, ������ true, ���� ��� ����������� - false
}

void TimerListForClasses::Delete(int hnd)					// ������� ������ � ������� hnd.     
{
	if (!InRange(hnd, 0, MAXTIMERSCOUNT - 1))return;
	
	INTERRUPTS_OFF;
	
	FuncElementStatic._array_timers_func[hnd].CallingFunc		= nullptr;
	FuncElementStatic._array_timers_func[hnd].PriorityFunc		= -1;		
	FuncElementStatic._array_timers_func[hnd].InitCounter		= 0;
	FuncElementStatic._array_timers_func[hnd].WorkingCounter	= 0;
	FuncElementStatic._array_timers_func[hnd].Active			= false;

	INTERRUPTS_ON;
	
}

void TimerListForClasses::DeleteAll()
{
	_active = false;									// ���������� ��� �������
	
	
	INTERRUPTS_OFF;
	
	for (int i = 0; i < MAXTIMERSCOUNT; i++)
	{
		FuncElementStatic._array_timers_func[i].CallingFunc		= nullptr;
		FuncElementStatic._array_timers_func[i].PriorityFunc	= -1;		
		FuncElementStatic._array_timers_func[i].InitCounter		= 0;
		FuncElementStatic._array_timers_func[i].WorkingCounter	= 0;
		FuncElementStatic._array_timers_func[i].Active			= false;
	}
	
	
	
	TaskElement			*pointer = FuncElementStatic.start_ptr;
	
	
	if (nullptr != pointer)
	{
		do							// ���������� �� ������� ���� 1 ���
		{
			pointer->_active			= false;
			pointer->_priority			= -1;
			pointer->_initCounter		= 0;
			pointer->_workingCounter	= 0;
						
			pointer = pointer->next_ptr;
			
		} while (pointer != FuncElementStatic.start_ptr);

	}
	
	
	INTERRUPTS_ON;
	
}

/// ��� ������� ���������� ��� ���������� ������������ ������� ������ ������������
/// �������� ���� ���������� ���������� ������������

void TimerListForClasses::Step(void)
{
	if (false == _active) return;                         // ���� ��� ������� �����������, �� � ������ ���
	
	TaskElement			*pointer = FuncElementStatic.start_ptr;
	
	
	if (nullptr != pointer)
	{
		do							// ���������� �� ������� ���� 1 ���
		{
			if (false == pointer->_active) continue;					// ������ �� �������, ������ �� ��������� ���� 
			if (--pointer->_workingCounter > 0) continue;				// ��������� �� 1 ������� �������
			pointer->_workingCounter = pointer->_initCounter;			// ���� ������ 0 ���������� � ������� ������� ��������� �������� ��� ����� �������
			
			
			if (0 < pointer->_priority)						// ������ � ������� ����� � ���� �� ��������� ����...
			{
				QueueList.Add(pointer->_priority, *pointer);
				continue;
			}		
			
			if (0 == pointer->_priority)
			{
				pointer->Step();							// ...���� ��������  �����-����������
			}
					
			pointer = pointer->next_ptr;
			
		} while (pointer != FuncElementStatic.end_ptr);

	}
	
	
	
	
	
	
	INTERRUPTS_OFF;

	for (int i = 0; i < MAXTIMERSCOUNT; i++)										// ��������� �� ����� ������ ��������
	{
		if (nullptr == FuncElementStatic._array_timers_func[i].CallingFunc) continue;					// ���� �������-���������� �� ���������, ������ �� ��������� ���� 
		if (false == FuncElementStatic._array_timers_func[i].Active) continue;						// ���� ������ ����������, ������ �� ��������� ����
		if (--FuncElementStatic._array_timers_func[i].WorkingCounter > 0) continue;					// ��������� �� 1 ������� �������
		
		FuncElementStatic._array_timers_func[i].WorkingCounter = FuncElementStatic._array_timers_func[i].InitCounter;	// ���� ������ 0 ���������� � ������� ������� ��������� �������� ��� ����� �������
		
		
		if (0 < FuncElementStatic._array_timers_func[i].PriorityFunc)										// ������ � ������� ����� � ���� �� ��������� ����...
		{
			QueueList.Add(FuncElementStatic._array_timers_func[i].PriorityFunc, FuncElementStatic);
			continue;
		}		
		
		if (0 == FuncElementStatic._array_timers_func[i].PriorityFunc)
		{
			FuncElementStatic._array_timers_func[i].CallingFunc();	// ...���� �������� �������-����������
		}
			
		
	}
	
	INTERRUPTS_ON;
}

void TimerListForClasses::Start()                              
{
	if (false == _active) Init();					// ��� ���������� ������� �������, �������������� ���������� ������
}

void TimerListForClasses::Stop()
{
	_active = false;									// ���������� ���������� ������
}

void TimerListForClasses::TimerStop(int hnd)
{
	if (false == InRange(hnd, 0, MAXTIMERSCOUNT - 1)) return;

	FuncElementStatic._array_timers_func[hnd].Active	= false;
}

bool TimerListForClasses::TimerActive(int hnd)
{
	if (false == InRange(hnd, 0, MAXTIMERSCOUNT - 1)) return false;
	else return FuncElementStatic._array_timers_func[hnd].Active;
}

void TimerListForClasses::TimerStart(int hnd)              // ��������� ������������� ������ ����� hnd
{
	if (false == InRange(hnd,0,MAXTIMERSCOUNT-1)) return;
	if (FuncElementStatic._array_timers_func[hnd].CallingFunc == nullptr) return;
	if (FuncElementStatic._array_timers_func[hnd].Active) return;
	
	INTERRUPTS_OFF;
	
	FuncElementStatic._array_timers_func[hnd].Active = true;
	FuncElementStatic._array_timers_func[hnd].WorkingCounter = FuncElementStatic._array_timers_func[hnd].InitCounter;
		
	INTERRUPTS_ON;
	
	if (false == _active) Init();					// �������������� ���������� ������
	
	
	
}

void TimerListForClasses::TimerNewInterval(int hnd, long newinterval)
{
	if (false == InRange(hnd, 0, MAXTIMERSCOUNT - 1)) return;
	INTERRUPTS_OFF;
	
	FuncElementStatic._array_timers_func[hnd].InitCounter = newinterval;
	FuncElementStatic._array_timers_func[hnd].WorkingCounter = newinterval;
	if (false == FuncElementStatic._array_timers_func[hnd].Active) TimerStart(hnd);
	
	INTERRUPTS_ON;
}

int	TimerListForClasses::Error()
{
	return _error;
}










/// ���������� ��������� ��� ������������ ������ � ���������� TimerList.Step() � ������ ���������� �������

/// ��������� �������� ��� ������� �������������
/// �� ������������ ������ 1 ������������. 

void TimerListForClasses::Init()   //(���������� ����������� ������ ������)
{
	_active = true;
	
	int _1MSCONST = 249;	// ��������� �� ���������� � �������� 1 ��		 
	
INTERRUPTS_OFF;

	TCCR2A = 0;
	TCCR2B = 0;
	TCCR2A |= (1 << WGM21);									 // ����� CTC (����� �� ����������)
	//������������
	//TCCR2B |= (1 << CS20);						  // CLK/1
	//TCCR2B = (1<<CS21);							  // CLK/8
	//TCCR2B = (1<<CS20)|(1<<CS21);					  // CLK/32
	TCCR2B = (1<<CS22);							  // CLK/64
	//TCCR2B = (1<<CS20)|(1<<CS22);					// CLK/128
	//TCCR2B = (1<<CS21)|(1<<CS22);					  // CLK/256
	//TCCR2B = (1<<CS20)|(1<<CS21)|(1<<CS22);		  // CLK/1024

	// ������� ������� �����. �������� �� 0 �� 255.
	OCR2A = _1MSCONST;										// ������� ���������� A ����� = Fclk/(N*(1+OCR2A))
	//OCR2B = 249;													// ������� ���������� B ����� = Fclk/(N*(1+OCR2B))
	// ��� N - ����. ������������ (1, 8, 32, 64, 128, 256 ��� 1024)
	TIMSK2 |= (1 << OCIE2A);								  // ��������� ���������� �� ���������� A
	//TIMSK2 |= (1 << OCIE2B);									 // ��������� ���������� �� ���������� B
	//TIMSK2 |= (1 << TOIE2);									 // ��������� ���������� �� ������������

INTERRUPTS_ON;
	
	
	
}

// ���������� ���������� ����������� ������� (TIMER5_COMPA_vect   ���������� ����������� ��� ������ ������)
ISR(TIMER2_COMPA_vect)
{
	if (false == active) 
	{
	// ��������� ������� 2 
	TCCR2B = 0;
	TCCR2A = 0;
	TCNT2 = 0;	
	}
		
	if (true == active) TimerList.Step();	
}		
		




		
		

//=========================== ������� ������� �����


bool 				TaskElement::_initialisation	= true;
TaskElement			*TaskElement::start_ptr			= nullptr;
TaskElement			*TaskElement::end_ptr			= nullptr;
TaskStructFunc		TaskElement::_array_timers_func[MAXTIMERSCOUNT]{};
Queue_StructFunc	TaskElement::_queue_func[MAX_QUEUE]{};






TaskElement::TaskElement() :
	_active {false},
	_priority {0},
	_initCounter {0},
	_workingCounter {0},
	_static {false}
{
	
	
	if (true == _initialisation)
	{
		for (int i = 0; i < MAXTIMERSCOUNT; i++)
		{
			_array_timers_func[i].Active = false; 
			_array_timers_func[i].PriorityFunc = -1; 
			_array_timers_func[i].InitCounter = 0; 
			_array_timers_func[i].WorkingCounter = 0; 
			_array_timers_func[i].CallingFunc = nullptr;		
		}
		
		for (int i = 0; i < MAX_QUEUE; i++)
		{
			_queue_func[i].CallingFunc    = nullptr;
			_queue_func[i].priorityFunc  = -1;
					
		}		

		_initialisation = false;
	}
	
	
	
	// ����������� ���������� �� ��������� ������:
	if (nullptr == start_ptr)
	{
		pre_ptr = this;
		next_ptr = this;
		start_ptr = this;
		end_ptr = this;
	}
	else
	{
	    end_ptr->next_ptr = this;
		pre_ptr = end_ptr;
		next_ptr = start_ptr;
		end_ptr = this;
		start_ptr->pre_ptr = this;
	}

	
}

void TaskElement::Step()
{
	
	
}

void TaskElement::OrganisateQueueFunc()
{
////////������� ������ ����� �� �������:

	int j = 0;								// ������ �������� ������� �����
	
	for (int i = 0; i < MAX_QUEUE; i++)		// ���� ������ ������ �����
	{		
		if (nullptr == _queue_func[i].CallingFunc)
		{
			j = i;
			break;
		}
		
	}

	
	for (int i = (j+1); i < MAX_QUEUE; i++)		// ���������� ��� �������� �������� � ������ �������
	{
		if (nullptr != _queue_func[i].CallingFunc)
		{
			_queue_func[j].CallingFunc	= _queue_func[i].CallingFunc;
			_queue_func[j].priorityFunc	= _queue_func[i].priorityFunc;
			
			_queue_func[i].CallingFunc	= nullptr;
			_queue_func[i].priorityFunc	= -1;
						
			j++;
		}
	
	}		

	
	
///////��������� ������ �� ���������� (���������� ������ ����������):	

	for (int i = 0; i < (j-1); i++)
	{
		if (_queue_func[i].priorityFunc > _queue_func[i+1].priorityFunc)
		{
			
			VoidFunc_ptr afunc = _queue_func[i].CallingFunc;
			int priority = _queue_func[i].priorityFunc;
			
			_queue_func[i].CallingFunc	= _queue_func[i+1].CallingFunc;
			_queue_func[i].priorityFunc	= _queue_func[i+1].priorityFunc;
			
			_queue_func[i+1].CallingFunc	= afunc;
			_queue_func[i+1].priorityFunc	= priority;
			
			if (0 != i ) i = i - 2;
		}
	
	}

	
}








//=========================== ������� �����

Queue_List::Queue_List() :		// ����������� ������
	active	{true},
	error	{0}
{
	for (int i = 0; i < MAX_QUEUE; i++)	// �������������� �������������, ���������� ������ �������� ����������
	{
		Items[i].callingTask	= nullptr;
		Items[i].priorityTask	= -1;

	}
}

bool	Queue_List::Add(int priority, VoidFunc_ptr AFunc)
{
	
	if (priority < 0) return false;
	
	
	
	INTERRUPTS_OFF;		//������ ���������� �� ����� ������ � �������� �����
	
	if(nullptr != FuncElementStatic._queue_func[MAX_QUEUE-1].CallingFunc)		// ���� � ������� ��� ����� - ���������, ���������� false
	{
		INTERRUPTS_ON;		//���������� ����������
		return false;
	}

	if(nullptr == FuncElementStatic._queue_func[0].CallingFunc)	//���� ������ ������� � ������� �� ������, ��������� ������ � �������
	{
		FuncElementStatic._queue_func[0].CallingFunc   = AFunc;
		FuncElementStatic._queue_func[0].priorityFunc  = priority;
		
		int flag_first = 0;
	
	
		for (int i = 0; i < MAX_QUEUE; i++)		// ���� ��������� FuncElementStatic � ������� ����� � �������� ��������� �� ������� �� ������� �������
		{
			if ((&FuncElementStatic == Items[i].callingTask) && (0 != flag_first))	// ������� ��������� �� �������
			{
				Items[i].callingTask	= nullptr;
				Items[i].priorityTask	= -1;
			}

			if ((&FuncElementStatic == Items[i].callingTask) && (0 == flag_first))
			{
				Items[i].priorityTask = FuncElementStatic._queue_func[0].priorityFunc;
				flag_first = 1;
				continue;
				
			}

			
		}
		
		if (0 == flag_first)
		{
			Add(FuncElementStatic._queue_func[0].priorityFunc, FuncElementStatic);		// ������ ��������� �� ������������ ������ � ������� � ����������� 
					
		}
			
		
		OrganisateQueue();
			
		
		INTERRUPTS_ON;		//���������� ����������
		
		return true;
	}

	
	for (int i = 0; i < MAX_QUEUE; i++)	// ���� ����� ��� ��������� ������
	{
		
		if(FuncElementStatic._queue_func[i].priorityFunc >= priority) continue;	// ���� ��������� ������� ����� ������� ��� ������ ��������� - ���� �� ��������� ��������

		
		if(nullptr == FuncElementStatic._queue_func[i].CallingFunc)	//���� ��������� ������� � ������� �� ������, ��������� ������ � ������� �� �����
		{
			FuncElementStatic._queue_func[i].CallingFunc    = AFunc;
			FuncElementStatic._queue_func[i].priorityFunc  = priority;
			break;
		}	
		
		for (int j = i; j < MAX_QUEUE; j++)		// ���� ��������� ������ �������
		{
			if(nullptr == FuncElementStatic._queue_func[j].CallingFunc) 
			{
			
				for (j; j > i; j--)	// ���������� ����� ������� �� ���� �������
				{
					FuncElementStatic._queue_func[j].CallingFunc    = FuncElementStatic._queue_func[j-1].CallingFunc;
					FuncElementStatic._queue_func[j].priorityFunc  = FuncElementStatic._queue_func[j-1].priorityFunc;
				}
				
				break;
			}
		}
		
		FuncElementStatic._queue_func[i].CallingFunc    = AFunc;
		FuncElementStatic._queue_func[i].priorityFunc  = priority;
		break;
		
		

	}	
	
	
	FuncElementStatic.OrganisateQueueFunc();					// ������������� ������� ������� � ����������� ������� FuncElementStatic
		
	
	
	
	
	
	int flag_first = 0;
	
	
	for (int i = 0; i < MAX_QUEUE; i++)		// ���� ��������� FuncElementStatic � ������� ����� � �������� ��������� �� ������� �� ������� �������
	{
		if ((&FuncElementStatic == Items[i].callingTask) && (0 != flag_first))	// ������� ��������� �� �������
		{
			Items[i].callingTask	= nullptr;
			Items[i].priorityTask	= -1;
		}

		if ((&FuncElementStatic == Items[i].callingTask) && (0 == flag_first))
		{
			Items[i].priorityTask = FuncElementStatic._queue_func[0].priorityFunc;
			flag_first = 1;
			continue;
			
		}

		
	}
	
	if (0 == flag_first)
	{
		Add(FuncElementStatic._queue_func[0].priorityFunc, FuncElementStatic);		// ������ ��������� �� ������������ ������ � ������� � ����������� 
				
	}
	
	
	
	OrganisateQueue();

	INTERRUPTS_ON;		//���������� ����������
	
	return true;
	
}

void	Queue_List::Delete(VoidFunc_ptr AFunc)
{
	
	
	INTERRUPTS_OFF;		//������ ���������� �� ����� ������ � �������� �����

	for (int i = 0; i < MAX_QUEUE; i++)	// ���� ��������� ���������
	{
		if (FuncElementStatic._queue_func[i].CallingFunc == AFunc)
		{
			FuncElementStatic._queue_func[i].CallingFunc = nullptr;
			FuncElementStatic._queue_func[i].priorityFunc  = -1;
		}
		
	}
	
	FuncElementStatic.OrganisateQueueFunc();	
	
	
	
	
	int flag_first = 0;
	
	
	for (int i = 0; i < MAX_QUEUE; i++)		// ���� ��������� FuncElementStatic � ������� ����� � �������� ��������� �� ������� �� ������� �������
	{
		if ((&FuncElementStatic == Items[i].callingTask) && (0 == flag_first))
		{
			Items[i].priorityTask = FuncElementStatic._queue_func[0].priorityFunc;
			flag_first = 1;
			continue;
			
		}
		if ((&FuncElementStatic == Items[i].callingTask) && (0 != flag_first))	// ������� ��������� �� �������
		{
			Items[i].callingTask	= nullptr;
			Items[i].priorityTask	= -1;
		}
		
	}
	
		
	INTERRUPTS_ON;		//���������� ����������
}

bool	Queue_List::Add(int priority, TaskElement &AElem)
{
	if (priority < 0) return false;
	
	INTERRUPTS_OFF;		//������ ���������� �� ����� ������ � �������� �����
	
	if(nullptr != Items[MAX_QUEUE-1].callingTask)		// ���� � ������� ��� ����� - ���������, ���������� false
	{
		INTERRUPTS_ON;		//���������� ����������
		return false;
	}

	if(nullptr == Items[0].callingTask)		//���� ������ ������� � ������� �� ������, ��������� ������ � �������
	{
		Items[0].callingTask	= &AElem;
		Items[0].priorityTask	= priority;
		
		INTERRUPTS_ON;		//���������� ����������
		return true;
	}

	
	for (int i = 0; i < MAX_QUEUE; i++)	// ���� ����� ��� ��������� ������
	{
		
		if((Items[i].priorityTask <= priority) && (Items[i].priorityTask >= 0)) continue;	// ���� ��������� ������� ����� ������� ��� ������ ��������� - ���� �� ��������� ��������

		
		if(nullptr == Items[i].callingTask)				//���� ��������� ������� � ������� �� ������, ��������� ������ � ������� �� �����
		{
			Items[i].callingTask    = &AElem;
			Items[i].priorityTask	= priority;
			break;
		}	
		
		if (Items[i].priorityTask > priority)
		{
			for (int j = i; j < MAX_QUEUE; j++)		// ���� ��������� ������ �������
			{
				if(nullptr == Items[j].callingTask) 
				{
				
					for (j; j > i; j--)				// ���������� ����� ������� �� ���� �������
					{
						Items[j].callingTask    = Items[j-1].callingTask;
						Items[j].priorityTask	= Items[j-1].priorityTask;
					}
					
					break;
				}
			}
			
			Items[i].callingTask    = &AElem;
			Items[i].priorityTask	= priority;
			break;
		}
		

	}	
	
	
	OrganisateQueue();					// ������������� ������� ������� � ����������� ������� FuncElementStatic
		

	INTERRUPTS_ON;		//���������� ����������
	
	return true;
	
}

void	Queue_List::Delete(TaskElement &AElem)
{
	INTERRUPTS_OFF;		//������ ���������� �� ����� ������ � �������� �����
	
	for (int i = 0; i < MAX_QUEUE; i++)	// ���� ��������� ��������� � ������� ��� ��������� �������� �� �������
	{
		if (Items[i].callingTask == &AElem)
		{
			Items[i].callingTask	= nullptr;
			Items[i].priorityTask	= -1;
		}
		
	}
	
	OrganisateQueue();	

	INTERRUPTS_ON;		//���������� ����������	
}

bool	Queue_List::IsActive()
{
	return active;
}

void	Queue_List::Step()
{

	if (false == active) return;


	INTERRUPTS_OFF;		//������ ���������� �� ����� ������ � �������� �����
	
	if (nullptr == Items[0].callingTask)	return;
	if (0 > Items[0].priorityTask)			return;
	
	VoidFunc_ptr AFunc = nullptr;
	TaskElement *AElem = nullptr;
	
	if (&FuncElementStatic == Items[0].callingTask)		// ���� �� ������� - ������� �� ����������� �������
	{
		
		if (nullptr == FuncElementStatic._queue_func[0].CallingFunc)	return;
		if (0 > FuncElementStatic._queue_func[0].priorityFunc)			return;
		
		AFunc = FuncElementStatic._queue_func[0].CallingFunc;
		
		
		Items[0].callingTask	= nullptr;
		Items[0].priorityTask	= -1;	

		FuncElementStatic._queue_func[0].CallingFunc    = nullptr;
		FuncElementStatic._queue_func[0].priorityFunc  = -1;
		FuncElementStatic.OrganisateQueueFunc();					// ������������� ������� ������� � ����������� ������� FuncElementStatic
		
		int count_func = 0;
		
		for (int i = 0; i < MAX_QUEUE; i++)		// ���� � ������� ��� ���� ������� �� ����������� �������
		{
			if (nullptr != FuncElementStatic._queue_func[0].CallingFunc) count_func++;
			
		}
		
		if (count_func > 0) Add(FuncElementStatic._queue_func[0].priorityFunc, FuncElementStatic);		// ������ ��������� �� ������������ ������ � ������� � �����������
		
	}
	else
	{
		AElem = Items[0].callingTask;
		
		Items[0].callingTask	= nullptr;
		Items[0].priorityTask	= -1;
		
	}
	
	
	OrganisateQueue();
	
	INTERRUPTS_ON;		//���������� ����������
	
	
	
	
	if (nullptr != AElem) AElem->Step();		//���������� ��������� ������
	if (nullptr != AFunc) AFunc();				//���������� ��������� ������
	
}

void	Queue_List::Start()
{
	active = true;
}

void	Queue_List::Stop()
{
	active = false;
}

void	Queue_List::Clean()
{
	
	INTERRUPTS_OFF;		//������ ���������� �� ����� ������ � �������� �����

	for (int i = 0; i < MAX_QUEUE; i++)	
	{
		Items[i].callingTask	= nullptr;
		Items[i].priorityTask	= -1;

	}

	INTERRUPTS_ON;		//���������� ����������
}

void	Queue_List::OrganisateQueue()		//������ ���������� ������ � ����� ����� INTERRUPTS_OFF � INTERRUPTS_ON
{
	
////////������� ������������ �������� �� �������:	
	
	for (int i = 0; i < MAX_QUEUE; i++)	
	{		
		if (Items[i].priorityTask < 0)
		{
			Items[i].callingTask = nullptr;
			Items[i].priorityTask = -1;
		}	
		
	}
	
	
////////������� ������ ����� �� �������:

	int j = 0;							// ������ �������� ������� �����
	
	for (int i = 0; i < MAX_QUEUE; i++)	// ���� ������ ������ �����
	{		
		if (nullptr == Items[i].callingTask)
		{
			j = i;
			break;
		}
		
	}

	
	for (int i = (j+1); i < MAX_QUEUE; i++)
	{
		if (nullptr != Items[i].callingTask)
		{
			Items[j].callingTask = Items[i].callingTask;
			Items[j].priorityTask = Items[i].priorityTask;
			
			Items[i].callingTask = nullptr;
			Items[i].priorityTask = -1;
						
			j++;
		}
	
	}		

	
	
///////��������� ������ �� ���������� (���������� ������ ����������):	

	for (int i = 0; i < (j-1); i++)
	{
		if (Items[i].priorityTask > Items[i+1].priorityTask)
		{
			TaskElement *AElem = Items[i].callingTask;
			int prior = Items[i].priorityTask;
			
			Items[i].callingTask = Items[i+1].callingTask;
			Items[i].priorityTask = Items[i+1].priorityTask;
			
			Items[i+1].callingTask = AElem;
			Items[i+1].priorityTask = prior;
			
			if (0 != i ) i = i - 2;
		}
	
	}
	
}

int		Queue_List::Error()
{
	return error;
	
}







