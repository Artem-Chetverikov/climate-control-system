#ifndef __QUEUETIMERTASK_H_INCLUDED
#define __QUEUETIMERTASK_H_INCLUDED

#include "QueueTask.h"


using PVoidFunc = void(*)(void);      // ��������� ��������� �� ������� ��� ����������, ������ �� ������������: void AnyFunc(void)  

const int MAXTIMERSCOUNT = 16;		// 	������������ ����� ������������������ ��������

extern bool InRange(int,int,int);



#pragma pack(push,1)		//������������ ����� � ������ ������ 1 ���� ��� ��������

struct TCallStruct					    // ���������� ��������� ��� �������� ������� �������
{
	PVoidFunc	CallingFunc;			// �������, ������� ����� ������� ��� ������������
	int			PriorityFunc;			// ��������� ���������� ������� ��� ������� �����
	long		InitCounter;			// �������� ���-�� �����������
	long		WorkingCounter;			// ������� �������. ����� ������ ����� ���� ���������� �������� �� InitCounter
	bool        Active;	  			    // � ������ ������������ ����������� �� 1. ��� ����������� 0 ���������� ������� 
										// CallingFunc, � ����� ������������ �������� �� InitCounter. ���� ���������� �������. :)
};

#pragma pack(pop)			// �������������� ������������ ����� � ������ �� ���������





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
			
		int		Add(PVoidFunc AFunc, long timeMS);				// ������� AFunc, ������� ���� ������� ����� timeMS �����������
		int		AddSeconds(PVoidFunc AFunc, int timeSec);		// �� ��, ������ �������� �������� � ��������. 
		int		AddMinutes(PVoidFunc AFunc, int timeMin);		// �� ��, ������ �������� �������� � �������. 
																				// ������������� ��� �������� �������� ������� ���������� 
		// ���������� ������� ��� ��������� ����� � ��������� ����� � ������������:
		int		Add(PVoidFunc AFunc, int priority, long timeMS);				// ������� AFunc, ������� ���� ������� ����� timeMS �����������
		int		AddSeconds(PVoidFunc AFunc, int priority, int timeSec);			// �� ��, ������ �������� �������� � ��������. 
		int		AddMinutes(PVoidFunc AFunc, int priroity, int timeMin);			// �� ��, ������ �������� �������� � �������. 
																				// ������������� ��� �������� �������� ������� ����������
																				
																				
																				
		bool	CanAdd();					// ���� ����� �������� ������, ������ true
		bool	IsActive();					// true, ���� ���� ���� ������ �������
		void	Delete(int hnd);			// ������� ������ hnd
		void	Step(void);					// ���������� ������ 1 �� ( _1MSCONST )
		void	Start();					// ��������� ������� ���� �������� ��������
		void	Stop();						// TimerList ��������, ��� ������� �����������
		int		Count();					// ������� ����������� ��������, �����. ����� �� ������������, ������ ��������� �����������
											// ����������, ������� �-��� CanAdd();
		void	TimerStop(int hnd);			// ���������� ��������� ������ �� ��� ������
		bool	TimerActive(int hnd);		// ������� �� ���������� ������
		void	TimerStart(int hnd);		// ��������� ��������� ������ (����� ���������) 
											// ���� �� ������������, � ���������� �������

		void	TimerNewInterval(int hnd, long newinterval);	// ������������� ����� �������� ������������ ��� ������� hnd
                                                                // � ��������� ��� (���� �� ��� ����������)
																
		int		Error();					// ������� ������ �������
};

extern TTimerList TimerList;				// ���������� ����� ����� � �������� ������


#endif

