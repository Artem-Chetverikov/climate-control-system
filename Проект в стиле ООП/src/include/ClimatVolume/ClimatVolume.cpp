#include "ClimatVolume.h"



Volume::Volume()		// конструктор класса
{
	
	for ( int i = 0; i < length; i++)	param[i] = nullptr;
	
	
	if (nullptr == end_ptr) pre_ptr = nullptr; 
	else pre_ptr = end_ptr;
	end_ptr = this;	

	active	= false;
	error	= 0;
	

}

static Volume	*Volume::end_ptr = nullptr;



//------------------------------------------------------------
void	Regulate();					// исполнение очередного шага регулирования климата

bool AddParam(RegulatorParam param);		// добавить контролируемый параметр микроклимата в данном объеме
bool DelParam(RegulatorParam param);		// удалить параметр микроклимата
bool IsControl(RegulatorParam param);		// true, если данный параметр контролируется в данном объеме






bool Volume::IsActive()
{
	return active;
}
		
void Volume::StartClim()
{
	active = true;
}

void Volume::StopClim()
{
	active = false;
}

int Volume::Error()
{
	return error;
}
















