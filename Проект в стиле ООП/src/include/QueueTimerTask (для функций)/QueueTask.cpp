#include "QueueTask.h"


QueueFunc_List::QueueFunc_List()		// конструктор класса
{

	cli;		//запрет прерываний на время работы с очередью задач
	
	for (int i = 0; i < MAX_QUEUE; i++)	// первоначальная инициализация, заполнение списка нулями
	{
		Items[i].CallingFunc    = nullptr;
		Items[i].priority_Func    = 0;

	}
	
	active	= true;
	
	error	= 0;
	
	sei;		//разрешение прерываний
}


bool	QueueFunc_List::Add(VoidFunc_ptr AFunc, int priority)
{
	
	cli;		//запрет прерываний на время работы с очередью задач
	
	if(nullptr != Items[MAX_QUEUE-1].CallingFunc) 
	{
		sei;		//разрешение прерываний
		return false;
	}

	if(nullptr == Items[0].CallingFunc)	//если первая позиция в очереди не занята, добавляем задачу и выходим
	{
		Items[0].CallingFunc    = AFunc;
		Items[0].priority_Func  = priority;
		
		sei;		//разрешение прерываний
		return true;
	}

	
	for (int i = 0; i < MAX_QUEUE; i++)	// ищем место под очередную задачу
	{
		
		if(Items[i].priority_Func >= priority) continue;	// если очередная позиция имеет больший или равный приоритет - идем на следующую итерацию

		
		if(nullptr == Items[i].CallingFunc)	//если очередная позиция в очереди не занята, добавляем задачу и выходим из цикла
		{
			Items[i].CallingFunc    = AFunc;
			Items[i].priority_Func  = priority;
			break;
		}	
		
		for (int j = i; j < MAX_QUEUE; j++)		// ищем окончание хвоста очереди
		{
			if(nullptr == Items[j].CallingFunc) 
			{
			
				for (j; j > i; j--)	// перемещаем хвост очереди на один элемент
				{
					Items[j].CallingFunc    = Items[j-1].CallingFunc;
					Items[j].priority_Func  = Items[j-1].priority_Func;
				}
				
				break;
			}
		}
		
		Items[i].CallingFunc    = AFunc;
		Items[i].priority_Func  = priority;
		break;
		
		

	}	
	

	sei;		//разрешение прерываний
	return true;
	
}


bool	QueueFunc_List::IsActive()
{
	return active;
}


void	QueueFunc_List::Delete(VoidFunc_ptr AFunc)
{
	
	cli;		//запрет прерываний на время работы с очередью задач

	for (int i = 0; i < MAX_QUEUE; i++)	// ищем требуемый указатель
	{
		if (Items[i].CallingFunc == AFunc)
		{
			Items[i].CallingFunc = nullptr;
			Items[i].priority_Func  = 0;
		}
		
	}

	OrganisateQueue();	
	
	sei;		//разрешение прерываний
}



void	QueueFunc_List::Step()
{

	if (false == active) return;


	cli;		//запрет прерываний на время работы с очередью задач
	
	VoidFunc_ptr AFunc;
	
	AFunc = Items[0].CallingFunc;
	
	Items[0].CallingFunc    = nullptr;
	Items[0].priority_Func    = 0;
	
	
	OrganisateQueue();
	
	
	sei;		//разрешение прерываний
	
	
	
	
	if (nullptr != AFunc) AFunc();		//исполнение очередной задачи
	
}


void	QueueFunc_List::Start()
{
	active = true;
}



void	QueueFunc_List::Stop()
{
	active = false;
}



void	QueueFunc_List::Clean()
{
	
	cli;		//запрет прерываний на время работы с очередью задач

	for (int i = 0; i < MAX_QUEUE; i++)	// первоначальная инициализация, заполнение списка нулями
	{
		Items[i].CallingFunc    = nullptr;
		Items[i].priority_Func    = 0;

	}

	sei;		//разрешение прерываний
}



void	QueueFunc_List::OrganisateQueue()		//должна вызываться только в блоке между cli и sei
{
	
////////удаляем пустые места из очереди:

	int j = 0;						// индекс текущего пустого места
	
	for (int i = 0; i < MAX_QUEUE; i++)	// ищем первое пустое место
	{		
		if (nullptr == Items[i].CallingFunc)
		{
			j = i;
			break;
		}
		
	}

	
	for (int i = (j+1); i < MAX_QUEUE; i++)
	{
		if (nullptr != Items[i].CallingFunc)
		{
			Items[j].CallingFunc = Items[i].CallingFunc;
			Items[j].priority_Func = Items[i].priority_Func;
			
			Items[i].CallingFunc = nullptr;
			Items[i].priority_Func = 0;
						
			j++;
		}
	
	}		

	
	
///////сортируем задачи по приоритету (используем Гномью сортировку):	

	for (int i = 0; i < (j-1); i++)
	{
		if (Items[i].priority_Func < Items[i+1].priority_Func)
		{
			VoidFunc_ptr AFunc = Items[i].CallingFunc;
			int prior = Items[i].priority_Func;
			
			Items[i].CallingFunc = Items[i+1].CallingFunc;
			Items[i].priority_Func = Items[i+1].priority_Func;
			
			Items[i+1].CallingFunc = AFunc;
			Items[i+1].priority_Func = prior;
			
			if (0 != i ) i = i - 2;
		}
	
	}
	
}


int		QueueFunc_List::Error()
{
	return error;
	
}

