#ifndef __QUEUETIMERTASKFORCLASSES_H_INCLUDED
#define __QUEUETIMERTASKFORCLASSES_H_INCLUDED



using VoidFunc_ptr = void(*)(void);			// псевдоним указателя на функцию без параметров, ничего не возвращающую: void AnyFunc(void)  

const int		MAXTIMERSCOUNT	= 16;		// максимальное число зарегистрированных таймеров (для отдельных ФУНКЦИЙ)
const int 		MAX_QUEUE		= 30;		// максимальная длина очереди задач

class TaskElement;							// предварительное объявление класса


//=========================== ТАЙМЕР

class TimerListForClasses
{
	private:
		void			Init();
		bool			_active;
		int				_error;

	public:
		TimerListForClasses();

		
	// для ФУНКЦИЙ:
		// перегрузка функций для исполнения по истечению таймера (без добавления в диспетчер задач):
		int		AddMilSec(VoidFunc_ptr AFunc, long timeMS);					// ссылка на функцию AFunc, которую надо вызвать через timeMS миллисекунд
		int		AddSeconds(VoidFunc_ptr AFunc, int timeSec);					// то же, только интервал задается в секундах. 
		int		AddMinutes(VoidFunc_ptr AFunc, int timeMin);					// то же, только интервал задается в минутах. 
		// перегрузка функций для помещения задач в диспетчер задач с приоритетами:
		int		AddMilSec(VoidFunc_ptr AFunc, int priority, long timeMS);		// ссылка на функцию AFunc, которую надо поместить в очередь через timeMS миллисекунд
		int		AddSeconds(VoidFunc_ptr AFunc, int priority, int timeSec);		// то же, только интервал задается в секундах. 
		int		AddMinutes(VoidFunc_ptr AFunc, int priroity, int timeMin);		// то же, только интервал задается в минутах. 

	
	// для МЕТОДОВ класса TaskElement:
		// перегрузка функций для исполнения по истечению таймера (без добавления в диспетчер задач):
		int		AddMilSec(TaskElement &AElem, long timeMS);						// ссылка на экземпляр TaskElement, метод Step() которого нужно вызвать через timeMS миллисекунд
		int		AddSeconds(TaskElement &AElem, int timeSec);					// то же, только интервал задается в секундах. 
		int		AddMinutes(TaskElement &AElem, int timeMin);					// то же, только интервал задается в минутах. 
		// перегрузка функций для помещения задач в диспетчер задач с приоритетами:
		int		AddMilSec(TaskElement &AElem, int priority, long timeMS);		// ссылка на экземпляр TaskElement, метод Step() которого нужно поместить в очередь через timeMS миллисекунд
		int		AddSeconds(TaskElement &AElem, int priority, int timeSec);		// то же, только интервал задается в секундах. 
		int		AddMinutes(TaskElement &AElem, int priroity, int timeMin);		// то же, только интервал задается в минутах. 


	
																				
																				
		bool	CanAdd();					// если можно добавить таймер, вернет true
		bool	IsActive();					// true, если хоть один таймер запущен
		void	Delete(int hnd);			// остановить и обнулить таймер hnd
		void	Delete(TaskElement *AElem);	// остановить и обнулить таймер по указателю AElem
		void	DeleteAll();				// остановить и обнулить все таймеры
		
		void	Step(void);					// вызывается каждую 1 мс ( _1MSCONST )
		void	Start();					// запустить рабочий цикл перебора таймеров
		void	Stop();						// TimerList выключен, все таймеры приостановлены

		void	TimerStop(int hnd);			// остановить отдельный таймер по его хэндлу
		bool	TimerActive(int hnd);		// запущен ли конкретный таймер
		void	TimerStart(int hnd);		// запустить отдельный таймер (после остановки) 
											// счет не продолжается, а начинается сначала

		void	TimerNewInterval(int hnd, long newinterval);	// устанавливает новый интервал срабатывания для таймера hnd
                                                                // и запускает его (если он был остановлен)
																
		int		Error();					// вернуть ошибки таймера????
};



extern TimerListForClasses TimerList;				// экземпляр для работы с таймером (переменная будет видна глобально)






//=========================== ЭЛЕМЕНТ ОЧЕРЕДИ ЗАДАЧ

struct TaskStructFunc						// внутренняя структура для хранения таймера (для отдельных ФУНКЦИЙ)
{
	
	bool				Active;				// признак активности текущей задачи
	int					PriorityFunc;		// приоритет выполнения функции для очереди задач
	long				InitCounter;		// заданное количество миллисекунд
	long				WorkingCounter;		// рабочий счетчик. после начала счета сюда копируется значение из InitCounter
											// и каждую миллисекунду уменьшается на 1. При достиженнии 0 вызывается функция 
											// Step(), и снова записывается значение из InitCounter. счет начинаетсЯ сначала
											
											
	VoidFunc_ptr		CallingFunc;		// функция, которую нужно вызвать при срабатывании	
};

struct Queue_StructFunc						// внутренняя структура для хранения  КАЖДОЙ очередной задачи
	{
		VoidFunc_ptr	CallingFunc;		// указатель на функцию, которую нужно вызвать
		int				priorityFunc;		// приоритет задачи

	};


class TaskElement							// родительский класс для хранения КАЖДОЙ задачи (для МЕТОДОВ класса)
{
protected:
	
	bool				_active;			// признак активности текущей задачи
	int					_priority;			// приоритет выполнения функции для очереди задач (0 - самый высокий приоритет)
	long				_initCounter;		// заданное количество миллисекунд
	long				_workingCounter;	// рабочий счетчик. после начала счёта сюда копируется значение из InitCounter
											// и каждую миллисекунду уменьшается на 1. При достиженнии 0 вызывается функция 
											// Step(), и снова записывается значение из InitCounter. Счёт начинается сначала
											
	bool				_static;			// признак обращения к статическим полям класса TaskElement
	
	TaskElement			*next_ptr;			// указатель на следующий экземпляр класса
	TaskElement			*pre_ptr;			// указатель на предыдущий экземпляр класса
	static TaskElement	*start_ptr;			// указатель на первый созданный экземпляр класса
	static TaskElement	*end_ptr;			// указатель на последний созданный экземпляр класса
	

	
public:
	TaskElement();
	
	friend TimerListForClasses;				// класс таймера - дружественный (управляет счетчиками)
	
	
	virtual void		Step();				// функция, которую нужно вызвать при срабатывании (определяется в дочернем классе)
	
		// очередь функций с статической области(хранится в классе TaskElement)
	static bool				_initialisation;					// признак первичной инициализации класса TaskElement
	
	static TaskStructFunc	_array_timers_func[MAXTIMERSCOUNT];	// массив для хранения таймеров (для отдельных ФУНКЦИЙ)
	
	static Queue_StructFunc	_queue_func[MAX_QUEUE];				// внутренняя очередь задач(для отдельных ФУНКЦИЙ)

	void					OrganisateQueueFunc();				// упорядочивает внутреннюю очередь задач по приоритетам (для отдельных ФУНКЦИЙ)

};



extern TaskElement		FuncElementStatic;			// для работы со статическими полями класса TaskElement(переменная будет видна локально)





//=========================== ОЧЕРЕДЬ ЗАДАЧ

struct Queue_Struct							// внутренняя структура для хранения  КАЖДОЙ очередной задачи
	{
		TaskElement		*callingTask;		// указатель на экземпляр TaskElement, метод Step() которого нужно вызвать
		int				priorityTask;		// приоритет задачи (0 - самый высокий приоритет)

	};
	
	
	
class Queue_List							// класс очереди задач
{
	private:
		
		Queue_Struct		Items[MAX_QUEUE];			// массив очереди задач
		bool				active;						// признак активности очереди задач (false - очередь не обрабатывается)
		
		void	OrganisateQueue();						// упорядочивает очередь задач по приоритетам (должна вызываться только в блоке между INTERRUPTS_OFF и INTERRUPTS_ON)
		int		error;									// счетчик ошибок очереди задач

	public:	
		Queue_List();									// конструктор класса

		bool	Add(int priority, VoidFunc_ptr AFunc);	// поместить функцию AFunc в очередь, указав ее приоритет
		void	Delete(VoidFunc_ptr AFunc);				// удалить функцию из очереди
		
		bool	Add(int priority, TaskElement &AElem);	// поместить метод Step() экземпляра класса TaskElement в очередь, указав его приоритет
		void	Delete(TaskElement &AElem);				// удалить метод Step() экземпляра класса TaskElement из очереди
		
		bool	IsActive();								// true, если диспетчер задач запущен
		void	Step();									// исполнение очередной функции из очереди
		void	Start();								// запустить диспетчер задач
		void	Stop();									// диспетчер задач выключен		
		void	Clean();								// полная очистка очереди		
		int		Error();								// вернуть ошибки очереди задач

};

extern Queue_List QueueList;




#endif		//__QUEUETIMERTASKFORCLASSES_H_INCLUDED

