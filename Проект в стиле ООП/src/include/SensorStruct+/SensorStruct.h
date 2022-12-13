#ifndef __SENSORSTRUCT_H_INCLUDED
#define __SENSORSTRUCT_H_INCLUDED


class SensorStr				// корневой класс обработки данных с любого датчика
{
	protected:
		
		float		_valueCur;			// последнее полученное значение измеряемой величины

		int 		_error;				// счетчик ошибок чтения с датчика
		
		SensorStr			*next_ptr;			// указатель на следующий экземпляр класса
		SensorStr			*pre_ptr;			// указатель на предыдущий экземпляр класса
		static SensorStr	*start_ptr;			// указатель на первый созданный экземпляр класса
		static SensorStr	*end_ptr;			// указатель на последний созданный экземпляр класса
		
	public:	

		SensorStr();						// конструктор класса
	
		virtual void	Read(void);			// функция опроса датчика (определяестся в каждом дочернем классе)
		float			ReturnVal(void);	// функция запроса текущей измеренной величины
	
		int				ReturnErr();		// вернуть текущее количество ошибок по датчику
		
};	
	
	
#endif