#ifndef __QUEUETIMERTASK_H_INCLUDED
#define __QUEUETIMERTASK_H_INCLUDED

#include "QueueTask.h"


using PVoidFunc = void(*)(void);      // псевдоним указателя на функцию без параметров, ничего не возвращающую: void AnyFunc(void)  

const int MAXTIMERSCOUNT = 16;		// 	Максимальное число зарегистрированных таймеров

extern bool InRange(int,int,int);



#pragma pack(push,1)		//выравнивание полей в памяти кратно 1 байт для структур

struct TCallStruct					    // внутренняя структура для хранения КАЖДОГО таймера
{
	PVoidFunc	CallingFunc;			// функция, которую нужно вызвать при срабатывании
	int			PriorityFunc;			// приоритет выполнения функции для очереди задач
	long		InitCounter;			// заданное кол-во миллисекунд
	long		WorkingCounter;			// рабочий счетчик. после начала счёта сюда копируется значение из InitCounter
	bool        Active;	  			    // и каждую миллисекунду уменьшается на 1. При достиженнии 0 вызывается функция 
										// CallingFunc, и снова записывается значение из InitCounter. Счёт начинается сначала. :)
};

#pragma pack(pop)			// восстановление выравнивания полей в памяти по умолчанию





class TTimerList
{
	private:
		TCallStruct		Items[MAXTIMERSCOUNT];
		void			Init();
		bool			active;
		int				count;
		int				error;

	public:
		TTimerList();
			
		int		Add(PVoidFunc AFunc, long timeMS);				// функция AFunc, которую надо вызвать через timeMS миллисекунд
		int		AddSeconds(PVoidFunc AFunc, int timeSec);		// то же, только интервал задается в секундах. 
		int		AddMinutes(PVoidFunc AFunc, int timeMin);		// то же, только интервал задается в минутах. 
																				// исключительно для удобства создания длинных интервалов 
		// перегрузка функций для помещения задач в диспетчер задач с приоритетами:
		int		Add(PVoidFunc AFunc, int priority, long timeMS);				// функция AFunc, которую надо вызвать через timeMS миллисекунд
		int		AddSeconds(PVoidFunc AFunc, int priority, int timeSec);			// то же, только интервал задается в секундах. 
		int		AddMinutes(PVoidFunc AFunc, int priroity, int timeMin);			// то же, только интервал задается в минутах. 
																				// исключительно для удобства создания длинных интервалов
																				
																				
																				
		bool	CanAdd();					// если можно добавить таймер, вернет true
		bool	IsActive();					// true, если хоть один таймер запущен
		void	Delete(int hnd);			// удалить таймер hnd
		void	Step(void);					// вызывается каждую 1 мс ( _1MSCONST )
		void	Start();					// запустить рабочий цикл перебора таймеров
		void	Stop();						// TimerList выключен, все таймеры остановлены
		int		Count();					// счетчик добавленных таймеров, всего. Можно не использовать, просто проверять возможность
											// добавления, вызывая ф-цию CanAdd();
		void	TimerStop(int hnd);			// остановить отдельный таймер по его хэндлу
		bool	TimerActive(int hnd);		// запущен ли конкретный таймер
		void	TimerStart(int hnd);		// запустить отдельный таймер (после остановки) 
											// счет не продолжается, а начинается сначала

		void	TimerNewInterval(int hnd, long newinterval);	// устанавливает новый интервал срабатывания для таймера hnd
                                                                // и запускает его (если он был остановлен)
																
		int		Error();					// вернуть ошибки таймера
};

extern TTimerList TimerList;				// переменная будет видна в основном скетче


#endif

