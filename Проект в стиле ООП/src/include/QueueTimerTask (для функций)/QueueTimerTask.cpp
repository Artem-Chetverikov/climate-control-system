
#include "QueueTimerTask.h"


#define AND &&
#define OR  ||
#define NOT !

TTimerList TimerList;		//������� ���������� ������ TTimerList


/// ���������� ��������� ��� ������������ ������ � ���������� TimerList.Step() � ������ ���������� �������

/// ��������� �������� ��� ������� �������������
/// �� ������������ ������ 1 ������������. 

		void TTimerList::Init()   //(���������� ����������� ������ ������)
		{
			
			int _1MSCONST = 249;	// ��������� �� ���������� � �������� 1 ��		 
			
		cli();

			TCCR2A = 0;
			TCCR2B = 0;
			TCCR2A |= (1 << WGM21);									 // ����� CTC (����� �� ����������)
			//������������
			//TCCR2B |= (1 << CS20);						  // CLK/1
			//TCCR2B = (1<<CS21);							  // CLK/8
			//TCCR2B = (1<<CS20)|(1<<CS21);					  // CLK/32
			TCCR2B = (1<<CS22);							  // CLK/64
			//TCCR2B = (1<<CS20)|(1<<CS22);					// CLK/128
			//TCCR2B = (1<<CS21)|(1<<CS22);					  // CLK/256
			//TCCR2B = (1<<CS20)|(1<<CS21)|(1<<CS22);		  // CLK/1024

			// ������� ������� �����. �������� �� 0 �� 255.
			OCR2A = _1MSCONST;										// ������� ���������� A ����� = Fclk/(N*(1+OCR2A))
			//OCR2B = 249;													// ������� ���������� B ����� = Fclk/(N*(1+OCR2B))
			// ��� N - ����. ������������ (1, 8, 32, 64, 128, 256 ��� 1024)
			TIMSK2 |= (1 << OCIE2A);								  // ��������� ���������� �� ���������� A
			//TIMSK2 |= (1 << OCIE2B);									 // ��������� ���������� �� ���������� B
			//TIMSK2 |= (1 << TOIE2);									 // ��������� ���������� �� ������������

		sei();
			
			
			
		}


// ���������� ���������� ����������� ������� (TIMER5_COMPA_vect   ���������� ����������� ��� ������ ������)
ISR(TIMER2_COMPA_vect)
{
	TimerList.Step();
}		
		






TTimerList::TTimerList()			// ����������� ������
{
	
	count = 0;
	for (int i = 0; i < MAXTIMERSCOUNT; i++)  // �������������� �������������, ���������� ������ ������
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
		if (Items[i].CallingFunc == nullptr)         // ����������� �� ������ ��������
		{                                         // ���� ����� ������ �����, ��������� ������
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
	return -1;                                   // ���� ��� - ������ ��� ������ (-1) 
}

int TTimerList::AddSeconds(PVoidFunc AFunc, int timeSec)
{
	return Add(AFunc, 1000L*timeSec);          // ���������� ��� ������
}

int TTimerList::AddMinutes(PVoidFunc AFunc, int timeMin)
{
	return Add(AFunc, timeMin*60L*1000L);     // ���������� ��� �����
}





int TTimerList::Add(PVoidFunc AFunc, int priority, long timeMS)
{
	for (int i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (Items[i].CallingFunc == nullptr)         // ����������� �� ������ ��������
		{                                         // ���� ����� ������ �����, ��������� ������
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
	return -1;                                   // ���� ��� - ������ ��� ������ (-1) 
}

int TTimerList::AddSeconds(PVoidFunc AFunc, int priority, int timeSec)
{
	return Add(AFunc, priority, 1000L*timeSec);          // ���������� ��� ������
}

int TTimerList::AddMinutes(PVoidFunc AFunc, int priority, int timeMin)
{
	return Add(AFunc, priority, timeMin*60L*1000L);     // ���������� ��� �����
}



bool TTimerList::CanAdd()
{
	for (int i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (Items[i].CallingFunc != nullptr) continue;  // ���� � ������ ���� ������ �����, ���� ����� �������� ������
		return true;                                 // ������ true
	}
	return false;                                    // ���� ��� - �� false
}

bool TTimerList::IsActive()
{
	return active;                                   // ���� ���� ���� ������ �������, ������ true, ���� ��� ����������� - false
}

void TTimerList::Delete(int hnd)					// ������� ������ � ������� hnd.     
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
		if (count==0) Stop();                        // ���� ��� ������� �������, ���������� � ���� �������� ��������
		sei();
	}
}

/// ��� ������� ���������� ��� ���������� ������������ ������� ������ ������������
/// �������� ���� ���������� ���������� ������������

void TTimerList::Step(void)
{
	if (NOT active) return;                         // ���� ��� ������� �����������, �� � ������ ���
	cli();                                          // ���� ����� �� ������� �������� ���������, ��������� ����������

	for (int i = 0; i < MAXTIMERSCOUNT; i++)		// ��������� �� ����� ������ ��������
	{
		if (nullptr == Items[i].CallingFunc) continue;		// ���� �������-���������� �� ���������, ������ �� ��������� ���� 
		if (NOT Items[i].Active) continue;					// ���� ������ ����������, ������ �� ��������� ����
		if (--Items[i].WorkingCounter > 0) continue;		// ��������� �� 1 ������� �������
		Items[i].WorkingCounter = Items[i].InitCounter;		// ���� ������ 0 ���������� � ������� ������� ��������� �������� ��� ����� �������
		
		
		if (0 < Items[i].PriorityFunc)						// ���� ������ � ������� ����� � ���� �� ��������� ����
		{
			QueueList.Add(Items[i].CallingFunc,Items[i].PriorityFunc);
			continue;
		}		
		
		Items[i].CallingFunc();								// ���� �������� �������-����������
		
	}
	
	sei();											// ������ � ���������� ����� ���������
}

void TTimerList::Start()                              
{
	if (NOT active) Init();                              // ��� ���������� ������� �������, �������������� ���������� ������
	active = true;
}

void TTimerList::Stop()
{
	active = false;                                   // ���������� ��� �������
}

int TTimerList::Count()
{
	return count;                                     // ������� ����������� ��������
}

void TTimerList::TimerStop(int hnd)
{
	if (InRange(hnd, 0, MAXTIMERSCOUNT-1))
	{
		Items[hnd].Active = false;                    // ���������� ������ ����� hnd
	}
}

bool TTimerList::TimerActive(int hnd)
{
	if (NOT InRange(hnd, 0, MAXTIMERSCOUNT - 1)) return false;
	else return Items[hnd].Active;
}

void TTimerList::TimerStart(int hnd)              // ��������� ������������� ������ ����� hnd
{
	if (NOT InRange(hnd,0,MAXTIMERSCOUNT-1)) return;
	if (Items[hnd].CallingFunc == nullptr)      return;
	if (Items[hnd].Active)                   return;
	cli();
	Items[hnd].WorkingCounter = Items[hnd].InitCounter;  // � ������ ������ ��������� �������
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

bool InRange(int value, int min, int max)               // ������ true, ���� value ����� � ��������� �� min �� max (������������)
{
	return (value >= min) AND (value <= max);
}



