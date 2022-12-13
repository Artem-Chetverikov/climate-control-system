
#include "QueueTimerTaskForClasses.h"
#include "Transceiver_function.h"


TaskElement			FuncElementStatic;
Queue_List			QueueList;
TimerListForClasses	TimerList;


bool InRange(int value, int min, int max)               // возвращает true, если value лежит в диапазоне от min до max (включительно)
{
	return (value >= min) && (value <= max);
}





//=========================== ТАЙМЕР


TimerListForClasses::TimerListForClasses() : 		// конструктор класса
	_active {false},
	_error {0}
{
	FuncElementStatic._static = true;		// для работы со статическими полями класса TaskElement

}

int TimerListForClasses::AddMilSec(VoidFunc_ptr AFunc, long timeMS)
{
	for (int i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (FuncElementStatic._array_timers_func[i].CallingFunc == nullptr)			// пробегаемся по списку таймеров
		{                                         					// если нашли пустую дырку, добавляем таймер
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
	return -1;                                   // если нет - вернем код ошибки (-1) 
}

int TimerListForClasses::AddSeconds(VoidFunc_ptr AFunc, int timeSec)
{
	return AddMilSec(AFunc, 1000L*timeSec);          // аналогично для секунд
}

int TimerListForClasses::AddMinutes(VoidFunc_ptr AFunc, int timeMin)
{
	return AddMilSec(AFunc, timeMin*60L*1000L);     // аналогично для минут
}

int TimerListForClasses::AddMilSec(VoidFunc_ptr AFunc, int priority, long timeMS)
{
	for (int i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (FuncElementStatic._array_timers_func[i].CallingFunc == nullptr)			// пробегаемся по списку таймеров
		{                                         								// если нашли пустую дырку, добавляем таймер
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
	return -1;                                   // если нет - вернем код ошибки (-1)
}

int TimerListForClasses::AddSeconds(VoidFunc_ptr AFunc, int priority, int timeSec)
{
	return AddMilSec(AFunc, priority, 1000L*timeSec);          // аналогично для секунд
}

int TimerListForClasses::AddMinutes(VoidFunc_ptr AFunc, int priority, int timeMin)
{
	return AddMilSec(AFunc, priority, timeMin*60L*1000L);     // аналогично для минут
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
	return AddMilSec(AElem, 1000L*timeSec);          // аналогично для секунд
}

int TimerListForClasses::AddMinutes(TaskElement &AElem, int timeMin)
{
	return AddMilSec(AElem, timeMin*60L*1000L);     // аналогично для минут
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
	return AddMilSec(AElem, priority, 1000L*timeSec);          // аналогично для секунд
}

int TimerListForClasses::AddMinutes(TaskElement &AElem, int priority, int timeMin)
{
	return AddMilSec(AElem, priority, timeMin*60L*1000L);     // аналогично для минут
}



bool TimerListForClasses::CanAdd()
{
	for (int i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (FuncElementStatic._array_timers_func[i].CallingFunc != nullptr) continue;  // если в списке есть пустые места, куда можно добавить таймер
		return true;                                 // вернем true
	}
	return false;                                    // если нет - то false
}

bool TimerListForClasses::IsActive()
{
	return _active;                                   // если хоть один таймер активен, вернет true, если все остановлены - false
}

void TimerListForClasses::Delete(int hnd)					// удалить таймер с хэндлом hnd.     
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
	_active = false;									// остановить все таймеры
	
	
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
		do							// выполнится по крайней мере 1 раз
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

/// эта функция вызывается при аппаратном срабатывании таймера каждую миллисекунду
/// запуская цикл декремента интервалов срабатывания

void TimerListForClasses::Step(void)
{
	if (false == _active) return;                         // если все таймеры остановлены, то и смысла нет
	
	TaskElement			*pointer = FuncElementStatic.start_ptr;
	
	
	if (nullptr != pointer)
	{
		do							// выполнится по крайней мере 1 раз
		{
			if (false == pointer->_active) continue;					// задача не активна, уходим на следующий цикл 
			if (--pointer->_workingCounter > 0) continue;				// уменьшаем на 1 рабочий счетчик
			pointer->_workingCounter = pointer->_initCounter;			// если достиг 0 записываем в рабочий счетчик начальное значение для счета сначала
			
			
			if (0 < pointer->_priority)						// ставим в очередь задач и идем на следующий цикл...
			{
				QueueList.Add(pointer->_priority, *pointer);
				continue;
			}		
			
			if (0 == pointer->_priority)
			{
				pointer->Step();							// ...либо вызываем  метод-обработчик
			}
					
			pointer = pointer->next_ptr;
			
		} while (pointer != FuncElementStatic.end_ptr);

	}
	
	
	
	
	
	
	INTERRUPTS_OFF;

	for (int i = 0; i < MAXTIMERSCOUNT; i++)										// пробегаем по всему списку таймеров
	{
		if (nullptr == FuncElementStatic._array_timers_func[i].CallingFunc) continue;					// если функция-обрабоччик не назначена, уходим на следующий цикл 
		if (false == FuncElementStatic._array_timers_func[i].Active) continue;						// если таймер остановлен, уходим на следующий цикл
		if (--FuncElementStatic._array_timers_func[i].WorkingCounter > 0) continue;					// уменьшаем на 1 рабочий счетчик
		
		FuncElementStatic._array_timers_func[i].WorkingCounter = FuncElementStatic._array_timers_func[i].InitCounter;	// если достиг 0 записываем в рабочий счетчик начальное значение для счета сначала
		
		
		if (0 < FuncElementStatic._array_timers_func[i].PriorityFunc)										// ставим в очередь задач и идем на следующий цикл...
		{
			QueueList.Add(FuncElementStatic._array_timers_func[i].PriorityFunc, FuncElementStatic);
			continue;
		}		
		
		if (0 == FuncElementStatic._array_timers_func[i].PriorityFunc)
		{
			FuncElementStatic._array_timers_func[i].CallingFunc();	// ...либо вызываем функцию-обработчик
		}
			
		
	}
	
	INTERRUPTS_ON;
}

void TimerListForClasses::Start()                              
{
	if (false == _active) Init();					// при добавлении первого таймера, инициализируем аппаратный таймер
}

void TimerListForClasses::Stop()
{
	_active = false;									// остановить аппаратный таймер
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

void TimerListForClasses::TimerStart(int hnd)              // запустить остановленный таймер номер hnd
{
	if (false == InRange(hnd,0,MAXTIMERSCOUNT-1)) return;
	if (FuncElementStatic._array_timers_func[hnd].CallingFunc == nullptr) return;
	if (FuncElementStatic._array_timers_func[hnd].Active) return;
	
	INTERRUPTS_OFF;
	
	FuncElementStatic._array_timers_func[hnd].Active = true;
	FuncElementStatic._array_timers_func[hnd].WorkingCounter = FuncElementStatic._array_timers_func[hnd].InitCounter;
		
	INTERRUPTS_ON;
	
	if (false == _active) Init();					// инициализируем аппаратный таймер
	
	
	
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










/// Необходима настройка под используемый таймер и добавление TimerList.Step() в вектор прерывания таймера

/// Настройка таймеров для первого использования
/// на срабатывание каждую 1 миллисекунду. 

void TimerListForClasses::Init()   //(необходимо настраивать нужный таймер)
{
	_active = true;
	
	int _1MSCONST = 249;	// настройка на прерывание с периодом 1 мс		 
	
INTERRUPTS_OFF;

	TCCR2A = 0;
	TCCR2B = 0;
	TCCR2A |= (1 << WGM21);									 // Режим CTC (сброс по совпадению)
	//Преддилитель
	//TCCR2B |= (1 << CS20);						  // CLK/1
	//TCCR2B = (1<<CS21);							  // CLK/8
	//TCCR2B = (1<<CS20)|(1<<CS21);					  // CLK/32
	TCCR2B = (1<<CS22);							  // CLK/64
	//TCCR2B = (1<<CS20)|(1<<CS22);					// CLK/128
	//TCCR2B = (1<<CS21)|(1<<CS22);					  // CLK/256
	//TCCR2B = (1<<CS20)|(1<<CS21)|(1<<CS22);		  // CLK/1024

	// Верхняя граница счета. Диапазон от 0 до 255.
	OCR2A = _1MSCONST;										// Частота прерываний A будет = Fclk/(N*(1+OCR2A))
	//OCR2B = 249;													// Частота прерываний B будет = Fclk/(N*(1+OCR2B))
	// где N - коэф. предделителя (1, 8, 32, 64, 128, 256 или 1024)
	TIMSK2 |= (1 << OCIE2A);								  // Разрешить прерывание по совпадению A
	//TIMSK2 |= (1 << OCIE2B);									 // Разрешить прерывание по совпадению B
	//TIMSK2 |= (1 << TOIE2);									 // Разрешить прерывание по переполнению

INTERRUPTS_ON;
	
	
	
}

// обработчик прерывания аппаратного таймера (TIMER5_COMPA_vect   необходимо настраивать под нужный таймер)
ISR(TIMER2_COMPA_vect)
{
	if (false == active) 
	{
	// Обнуление таймера 2 
	TCCR2B = 0;
	TCCR2A = 0;
	TCNT2 = 0;	
	}
		
	if (true == active) TimerList.Step();	
}		
		




		
		

//=========================== ЭЛЕМЕНТ ОЧЕРЕДИ ЗАДАЧ


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
	
	
	
	// определение указателей на экземпляр класса:
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
////////удаляем пустые места из очереди:

	int j = 0;								// индекс текущего пустого места
	
	for (int i = 0; i < MAX_QUEUE; i++)		// ищем первое пустое место
	{		
		if (nullptr == _queue_func[i].CallingFunc)
		{
			j = i;
			break;
		}
		
	}

	
	for (int i = (j+1); i < MAX_QUEUE; i++)		// перемещаем все непустые элементы к началу очереди
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

	
	
///////сортируем задачи по приоритету (используем Гномью сортировку):	

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








//=========================== ОЧЕРЕДЬ ЗАДАЧ

Queue_List::Queue_List() :		// конструктор класса
	active	{true},
	error	{0}
{
	for (int i = 0; i < MAX_QUEUE; i++)	// первоначальная инициализация, заполнение списка нулевыми значениями
	{
		Items[i].callingTask	= nullptr;
		Items[i].priorityTask	= -1;

	}
}

bool	Queue_List::Add(int priority, VoidFunc_ptr AFunc)
{
	
	if (priority < 0) return false;
	
	
	
	INTERRUPTS_OFF;		//запрет прерываний на время работы с очередью задач
	
	if(nullptr != FuncElementStatic._queue_func[MAX_QUEUE-1].CallingFunc)		// если в очереди нет места - прерываем, возвращаем false
	{
		INTERRUPTS_ON;		//разрешение прерываний
		return false;
	}

	if(nullptr == FuncElementStatic._queue_func[0].CallingFunc)	//если первая позиция в очереди не занята, добавляем задачу и выходим
	{
		FuncElementStatic._queue_func[0].CallingFunc   = AFunc;
		FuncElementStatic._queue_func[0].priorityFunc  = priority;
		
		int flag_first = 0;
	
	
		for (int i = 0; i < MAX_QUEUE; i++)		// ищем экземпляр FuncElementStatic в очереди задач и изменяем приоритет на текущий из очереди функций
		{
			if ((&FuncElementStatic == Items[i].callingTask) && (0 != flag_first))	// убираем двойников из очереди
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
			Add(FuncElementStatic._queue_func[0].priorityFunc, FuncElementStatic);		// ставим экземпляр со статическими полями в очередь с приоритетом 
					
		}
			
		
		OrganisateQueue();
			
		
		INTERRUPTS_ON;		//разрешение прерываний
		
		return true;
	}

	
	for (int i = 0; i < MAX_QUEUE; i++)	// ищем место под очередную задачу
	{
		
		if(FuncElementStatic._queue_func[i].priorityFunc >= priority) continue;	// если очередная позиция имеет больший или равный приоритет - идем на следующую итерацию

		
		if(nullptr == FuncElementStatic._queue_func[i].CallingFunc)	//если очередная позиция в очереди не занята, добавляем задачу и выходим из цикла
		{
			FuncElementStatic._queue_func[i].CallingFunc    = AFunc;
			FuncElementStatic._queue_func[i].priorityFunc  = priority;
			break;
		}	
		
		for (int j = i; j < MAX_QUEUE; j++)		// ищем окончание хвоста очереди
		{
			if(nullptr == FuncElementStatic._queue_func[j].CallingFunc) 
			{
			
				for (j; j > i; j--)	// перемещаем хвост очереди на один элемент
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
	
	
	FuncElementStatic.OrganisateQueueFunc();					// упорядочиваем очередь функций в статической области FuncElementStatic
		
	
	
	
	
	
	int flag_first = 0;
	
	
	for (int i = 0; i < MAX_QUEUE; i++)		// ищем экземпляр FuncElementStatic в очереди задач и изменяем приоритет на текущий из очереди функций
	{
		if ((&FuncElementStatic == Items[i].callingTask) && (0 != flag_first))	// убираем двойников из очереди
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
		Add(FuncElementStatic._queue_func[0].priorityFunc, FuncElementStatic);		// ставим экземпляр со статическими полями в очередь с приоритетом 
				
	}
	
	
	
	OrganisateQueue();

	INTERRUPTS_ON;		//разрешение прерываний
	
	return true;
	
}

void	Queue_List::Delete(VoidFunc_ptr AFunc)
{
	
	
	INTERRUPTS_OFF;		//запрет прерываний на время работы с очередью задач

	for (int i = 0; i < MAX_QUEUE; i++)	// ищем требуемый указатель
	{
		if (FuncElementStatic._queue_func[i].CallingFunc == AFunc)
		{
			FuncElementStatic._queue_func[i].CallingFunc = nullptr;
			FuncElementStatic._queue_func[i].priorityFunc  = -1;
		}
		
	}
	
	FuncElementStatic.OrganisateQueueFunc();	
	
	
	
	
	int flag_first = 0;
	
	
	for (int i = 0; i < MAX_QUEUE; i++)		// ищем экземпляр FuncElementStatic в очереди задач и изменяем приоритет на текущий из очереди функций
	{
		if ((&FuncElementStatic == Items[i].callingTask) && (0 == flag_first))
		{
			Items[i].priorityTask = FuncElementStatic._queue_func[0].priorityFunc;
			flag_first = 1;
			continue;
			
		}
		if ((&FuncElementStatic == Items[i].callingTask) && (0 != flag_first))	// убираем двойников из очереди
		{
			Items[i].callingTask	= nullptr;
			Items[i].priorityTask	= -1;
		}
		
	}
	
		
	INTERRUPTS_ON;		//разрешение прерываний
}

bool	Queue_List::Add(int priority, TaskElement &AElem)
{
	if (priority < 0) return false;
	
	INTERRUPTS_OFF;		//запрет прерываний на время работы с очередью задач
	
	if(nullptr != Items[MAX_QUEUE-1].callingTask)		// если в очереди нет места - прерываем, возвращаем false
	{
		INTERRUPTS_ON;		//разрешение прерываний
		return false;
	}

	if(nullptr == Items[0].callingTask)		//если первая позиция в очереди не занята, добавляем задачу и выходим
	{
		Items[0].callingTask	= &AElem;
		Items[0].priorityTask	= priority;
		
		INTERRUPTS_ON;		//разрешение прерываний
		return true;
	}

	
	for (int i = 0; i < MAX_QUEUE; i++)	// ищем место под очередную задачу
	{
		
		if((Items[i].priorityTask <= priority) && (Items[i].priorityTask >= 0)) continue;	// если очередная позиция имеет больший или равный приоритет - идем на следующую итерацию

		
		if(nullptr == Items[i].callingTask)				//если очередная позиция в очереди не занята, добавляем задачу и выходим из цикла
		{
			Items[i].callingTask    = &AElem;
			Items[i].priorityTask	= priority;
			break;
		}	
		
		if (Items[i].priorityTask > priority)
		{
			for (int j = i; j < MAX_QUEUE; j++)		// ищем окончание хвоста очереди
			{
				if(nullptr == Items[j].callingTask) 
				{
				
					for (j; j > i; j--)				// перемещаем хвост очереди на один элемент
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
	
	
	OrganisateQueue();					// упорядочиваем очередь функций в статической области FuncElementStatic
		

	INTERRUPTS_ON;		//разрешение прерываний
	
	return true;
	
}

void	Queue_List::Delete(TaskElement &AElem)
{
	INTERRUPTS_OFF;		//запрет прерываний на время работы с очередью задач
	
	for (int i = 0; i < MAX_QUEUE; i++)	// ищем требуемый указатель и удаляем все указанные элементы из очереди
	{
		if (Items[i].callingTask == &AElem)
		{
			Items[i].callingTask	= nullptr;
			Items[i].priorityTask	= -1;
		}
		
	}
	
	OrganisateQueue();	

	INTERRUPTS_ON;		//разрешение прерываний	
}

bool	Queue_List::IsActive()
{
	return active;
}

void	Queue_List::Step()
{

	if (false == active) return;


	INTERRUPTS_OFF;		//запрет прерываний на время работы с очередью задач
	
	if (nullptr == Items[0].callingTask)	return;
	if (0 > Items[0].priorityTask)			return;
	
	VoidFunc_ptr AFunc = nullptr;
	TaskElement *AElem = nullptr;
	
	if (&FuncElementStatic == Items[0].callingTask)		// если на очереди - функция из статической области
	{
		
		if (nullptr == FuncElementStatic._queue_func[0].CallingFunc)	return;
		if (0 > FuncElementStatic._queue_func[0].priorityFunc)			return;
		
		AFunc = FuncElementStatic._queue_func[0].CallingFunc;
		
		
		Items[0].callingTask	= nullptr;
		Items[0].priorityTask	= -1;	

		FuncElementStatic._queue_func[0].CallingFunc    = nullptr;
		FuncElementStatic._queue_func[0].priorityFunc  = -1;
		FuncElementStatic.OrganisateQueueFunc();					// упорядочиваем очередь функций в статической области FuncElementStatic
		
		int count_func = 0;
		
		for (int i = 0; i < MAX_QUEUE; i++)		// если в очереди еще есть функции из статической области
		{
			if (nullptr != FuncElementStatic._queue_func[0].CallingFunc) count_func++;
			
		}
		
		if (count_func > 0) Add(FuncElementStatic._queue_func[0].priorityFunc, FuncElementStatic);		// ставим экземпляр со статическими полями в очередь с приоритетом
		
	}
	else
	{
		AElem = Items[0].callingTask;
		
		Items[0].callingTask	= nullptr;
		Items[0].priorityTask	= -1;
		
	}
	
	
	OrganisateQueue();
	
	INTERRUPTS_ON;		//разрешение прерываний
	
	
	
	
	if (nullptr != AElem) AElem->Step();		//исполнение очередной задачи
	if (nullptr != AFunc) AFunc();				//исполнение очередной задачи
	
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
	
	INTERRUPTS_OFF;		//запрет прерываний на время работы с очередью задач

	for (int i = 0; i < MAX_QUEUE; i++)	
	{
		Items[i].callingTask	= nullptr;
		Items[i].priorityTask	= -1;

	}

	INTERRUPTS_ON;		//разрешение прерываний
}

void	Queue_List::OrganisateQueue()		//должна вызываться только в блоке между INTERRUPTS_OFF и INTERRUPTS_ON
{
	
////////удаляем некорректные элементы из очереди:	
	
	for (int i = 0; i < MAX_QUEUE; i++)	
	{		
		if (Items[i].priorityTask < 0)
		{
			Items[i].callingTask = nullptr;
			Items[i].priorityTask = -1;
		}	
		
	}
	
	
////////удаляем пустые места из очереди:

	int j = 0;							// индекс текущего пустого места
	
	for (int i = 0; i < MAX_QUEUE; i++)	// ищем первое пустое место
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

	
	
///////сортируем задачи по приоритету (используем Гномью сортировку):	

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







