
#include "QueueTimerTask.h"


#define AND &&
#define OR  ||
#define NOT !

TTimerList TimerList;		//создаем переменную класса TTimerList


/// Необходима настройка под используемый таймер и добавление TimerList.Step() в вектор прерывания таймера

/// Настройка таймеров для первого использования
/// на срабатывание каждую 1 миллисекунду. 

		void TTimerList::Init()   //(необходимо настраивать нужный таймер)
		{
			
			int _1MSCONST = 249;	// настройка на прерывание с периодом 1 мс		 
			
		cli();

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

		sei();
			
			
			
		}


// обработчик прерывания аппаратного таймера (TIMER5_COMPA_vect   необходимо настраивать под нужный таймер)
ISR(TIMER2_COMPA_vect)
{
	TimerList.Step();
}		
		






TTimerList::TTimerList()			// конструктор класса
{
	
	count = 0;
	for (int i = 0; i < MAXTIMERSCOUNT; i++)  // первоначальная инициализация, заполнение списка нулями
	{
		Items[i].CallingFunc	= nullptr;
		Items[i].PriorityFunc	= -1;
		Items[i].InitCounter	= 0;
		Items[i].WorkingCounter = 0;
		Items[i].Active			= false;
	}
	active	= false;
	error	=0;
}




int TTimerList::Add(PVoidFunc AFunc, long timeMS)
{
	for (int i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (Items[i].CallingFunc == nullptr)         // пробегаемся по списку таймеров
		{                                         // если нашли пустую дырку, добавляем таймер
			cli();
			Items[i].CallingFunc	= AFunc;
			Items[i].PriorityFunc	= 0;
			Items[i].InitCounter	= timeMS;
			Items[i].WorkingCounter = timeMS;
			Items[i].Active			= true;
			count++;
			if (NOT active)  Start();
			sei();
			return i;
		}
	}
	return -1;                                   // если нет - вернем код ашыпки (-1) 
}

int TTimerList::AddSeconds(PVoidFunc AFunc, int timeSec)
{
	return Add(AFunc, 1000L*timeSec);          // аналогично для секунд
}

int TTimerList::AddMinutes(PVoidFunc AFunc, int timeMin)
{
	return Add(AFunc, timeMin*60L*1000L);     // аналогично для минут
}





int TTimerList::Add(PVoidFunc AFunc, int priority, long timeMS)
{
	for (int i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (Items[i].CallingFunc == nullptr)         // пробегаемся по списку таймеров
		{                                         // если нашли пустую дырку, добавляем таймер
			cli();
			Items[i].CallingFunc	= AFunc;
			Items[i].PriorityFunc	= priority;
			Items[i].InitCounter	= timeMS;
			Items[i].WorkingCounter = timeMS;
			Items[i].Active			= true;
			count++;
			if (NOT active)  Start();
			sei();
			return i;
		}
	}
	return -1;                                   // если нет - вернем код ашыпки (-1) 
}

int TTimerList::AddSeconds(PVoidFunc AFunc, int priority, int timeSec)
{
	return Add(AFunc, priority, 1000L*timeSec);          // аналогично для секунд
}

int TTimerList::AddMinutes(PVoidFunc AFunc, int priority, int timeMin)
{
	return Add(AFunc, priority, timeMin*60L*1000L);     // аналогично для минут
}



bool TTimerList::CanAdd()
{
	for (int i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (Items[i].CallingFunc != nullptr) continue;  // если в списке есть пустые места, куда можно добавить таймер
		return true;                                 // вернем true
	}
	return false;                                    // если нет - то false
}

bool TTimerList::IsActive()
{
	return active;                                   // если хоть один таймер активен, вернет true, если все остановлены - false
}

void TTimerList::Delete(int hnd)					// удалить таймер с хэндлом hnd.     
{
	if (InRange(hnd, 0, MAXTIMERSCOUNT - 1))
	{
		cli();
		Items[hnd].CallingFunc		= nullptr;
		Items[hnd].PriorityFunc		= -1;		
		Items[hnd].InitCounter		= 0;
		Items[hnd].WorkingCounter	= 0;
		Items[hnd].Active           = false;
		if (count > 0)  count--;
		if (count==0) Stop();                        // если все таймеры удалены, остановить и цикл перебора таймеров
		sei();
	}
}

/// эта функция вызывается при аппаратном срабатывании таймера каждую миллисекунду
/// запуская цикл декремента интервалов срабатывания

void TTimerList::Step(void)
{
	if (NOT active) return;                         // если все таймеры остановлены, то и смысла нет
	cli();                                          // чтоб никто не помешал изменить интервалы, запрещаем прерывания

	for (int i = 0; i < MAXTIMERSCOUNT; i++)		// пробегаем по всему списку таймеров
	{
		if (nullptr == Items[i].CallingFunc) continue;		// если функция-обрабоччик не назначена, уходим на следующий цикл 
		if (NOT Items[i].Active) continue;					// если таймер остановлен, уходим на следующий цикл
		if (--Items[i].WorkingCounter > 0) continue;		// уменьшаем на 1 рабочий счетчик
		Items[i].WorkingCounter = Items[i].InitCounter;		// если достиг 0 записываем в рабочий счетчик начальное значение для счета сначала
		
		
		if (0 < Items[i].PriorityFunc)						// либо ставим в очередь задач и идем на следующий цикл
		{
			QueueList.Add(Items[i].CallingFunc,Items[i].PriorityFunc);
			continue;
		}		
		
		Items[i].CallingFunc();								// либо вызываем функцию-обработчик
		
	}
	
	sei();											// теперь и прерывания можно разрешить
}

void TTimerList::Start()                              
{
	if (NOT active) Init();                              // при добавлении первого таймера, инициализируем аппаратный таймер
	active = true;
}

void TTimerList::Stop()
{
	active = false;                                   // остановить все таймеры
}

int TTimerList::Count()
{
	return count;                                     // счетчик добавленных таймеров
}

void TTimerList::TimerStop(int hnd)
{
	if (InRange(hnd, 0, MAXTIMERSCOUNT-1))
	{
		Items[hnd].Active = false;                    // остановить таймер номер hnd
	}
}

bool TTimerList::TimerActive(int hnd)
{
	if (NOT InRange(hnd, 0, MAXTIMERSCOUNT - 1)) return false;
	else return Items[hnd].Active;
}

void TTimerList::TimerStart(int hnd)              // запустить остановленный таймер номер hnd
{
	if (NOT InRange(hnd,0,MAXTIMERSCOUNT-1)) return;
	if (Items[hnd].CallingFunc == nullptr)      return;
	if (Items[hnd].Active)                   return;
	cli();
	Items[hnd].WorkingCounter = Items[hnd].InitCounter;  // и начать отсчет интервала сначала
	Items[hnd].Active = true;
	sei();
}

void TTimerList::TimerNewInterval(int hnd, long newinterval)
{
	if (NOT InRange(hnd, 0, MAXTIMERSCOUNT - 1)) return;
	cli();
	Items[hnd].InitCounter = newinterval;
	Items[hnd].WorkingCounter = newinterval;
	if (NOT Items[hnd].Active) TimerStart(hnd);
	sei();
}

int		TTimerList::Error()
{
	return error;
}

bool InRange(int value, int min, int max)               // отдает true, если value лежит в диапазоне от min до max (включительно)
{
	return (value >= min) AND (value <= max);
}



