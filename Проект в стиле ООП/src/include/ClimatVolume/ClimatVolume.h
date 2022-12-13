#ifndef __CLIMATVOLUME_H_INCLUDED
#define __CLIMATVOLUME_H_INCLUDED

#include "ParameterClimat.h"



using	Func_1int_ptr = void(*)(int);	// псевдоним указателя на функцию регулирования (с параметром: требуемое отклонение ±100%), ничего не возвращающую



	
class Volume					// класс управления микроклиматом
{
	private:
	

		const int			length = 4;					// длина массива контролируемых параметров
		RegulatorParam		*param[length];				// массив указателей на контролируемые параметры микроклимата в объеме
		
		Volume				*pre_ptr					// указатель на предыдущий экземпляр параметра
		static Volume		*end_ptr					// указатель на последний созданный экземпляр параметр
		
		bool				active;						// признак активности управления микроклиматом в данном объеме
		int					error;						// счетчик ошибок при управлении микроклиматом
		
	public:	
	
		Volume(RegulatorParam Param_1, ...);					// конструктор класса
		
		void	Regulate();					// исполнение очередного шага регулирования климата
		
		bool AddParam(RegulatorParam param);		// добавить контролируемый параметр микроклимата в данном объеме
		bool DelParam(RegulatorParam param);		// удалить параметр микроклимата
		bool IsControl(RegulatorParam param);		// true, если данный параметр контролируется в данном объеме

		void	StartClim();				// запустить управление микроклиматом в данном объеме
		void	StopClim();					// остановить управление микроклиматом в данном объеме
		bool	IsActive();					// true, если производится управление микроклиматом в данном объеме
		int		Error();					// вернуть ошибки

};


#endif