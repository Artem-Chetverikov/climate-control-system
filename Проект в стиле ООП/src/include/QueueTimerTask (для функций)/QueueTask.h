#ifndef __QUEUETASK_H_INCLUDED
#define __QUEUETASK_H_INCLUDED

using	VoidFunc_ptr = void(*)(void);	// псевдоним указател€ на функцию без параметров, ничего не возвращающую: void AnyFunc(void)

const int MAX_QUEUE =	30;				// 	максимальная длина очереди задач

#pragma pack(push,1)		//выравнивание полей в памяти кратно 1 байт для структур

struct QueueFunc_Struct					// внутренняя структура для хранения  КАЖДОЙ очередной задачи
	{
		VoidFunc_ptr	CallingFunc;	// указатель на функцию, которую нужно вызвать
		int				priority_Func;	// приоритет функции

	};
	
#pragma pack(pop)			// восстановление выравнивания полей в памяти по умолчанию
	
	
class QueueFunc_List					// класс очереди задач
{
	private:
		QueueFunc_Struct		Items[MAX_QUEUE];		// массив очереди задач
		bool					active;					// признак активности очереди задач
		
		void	OrganisateQueue();						// упорядочивает очередь задач при возникновении сбоев
		int		error;									// счетчик ошибок очереди задач

	public:	
		QueueFunc_List();							//конструктор класса

		bool	Add(VoidFunc_ptr AFunc, int priority);	// поместить функцию AFunc в очередь, указав ее приоритет
		bool	IsActive();							// true, если диспетчер задач запущен
		void	Delete(VoidFunc_ptr AFunc);			// удалить функцию из очереди
		void	Step();								// исполнение очередной функции из очереди
		void	Start();							// запустить диспетчер задач
		void	Stop();								// диспетчер задач выключен		
		void	Clean();							// полная очистка очереди		
		int		Error();							// вернуть ошибки очереди задач

};




#endif