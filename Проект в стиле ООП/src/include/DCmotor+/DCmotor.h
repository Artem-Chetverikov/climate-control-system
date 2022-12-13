#ifndef __DCMOTOR_H_INCLUDED
#define __DCMOTOR_H_INCLUDED



class DCmotor
{
private:


protected:

	int					_pin_A;			// вывод 1 подключения мотора
	int					_pin_B;			// вывод 2 подключения мотора
	int					_pin_En;		// вывод 3 разрешения вращения на драйвер
	
	bool 				_isPWM;			// true, если применяется ШИМ регулирование
	
	int					_preDir;		// предустановка направления вращения

	bool 				_isActiv;		// true, если работа мотора разрешена
	
	DCmotor				*next_ptr;		// указатель на следующий экземпляр класса
	DCmotor				*pre_ptr;		// указатель на предыдущий экземпляр класса
	static DCmotor		*start_ptr;		// указатель на первый созданный экземпляр класса
	static DCmotor		*end_ptr;		// указатель на последний созданный экземпляр класса

	
public:

	DCmotor(int pinA, int pinB);					// 
	
	DCmotor(int pinA, int pinB, int pinEn);			// pinEn - выход ШИМ
	
	void					Init();					// начальная инициализация
	
	void					Start(int speed);		// запуск мотора в текущем направлении вращения (при ШИМ - speed значение от -100 до + 100 (в %))
	void					Stop();					// останов мотора
	
	void					PWM_ON();				// управление с использованием ШИМ
	void					PWM_OFF();				// управление без ШИМ
	
	void					ChangePreDir();			// изменить предустановку направления вращения

	bool					IsActiv();				// true, если работа мотора разрешена
	void					Activ();				// работа мотора Разрешена
	void					Deactiv();				// работа мотора Запрещена
	
	

	DCmotor*				Return_next();			// вернуть указатель на следующий экземпляр класса
	DCmotor*				Return_pre();			// вернуть указатель на предыдущий экземпляр класса
	static DCmotor*			Return_start();			// вернуть указатель на первый созданный экземпляр класса
    static DCmotor*			Return_end();			// вернуть указатель на последний созданный экземпляр класса
	
};






#endif
