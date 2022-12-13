#ifndef __QUEUETASK_H_INCLUDED
#define __QUEUETASK_H_INCLUDED

using	VoidFunc_ptr = void(*)(void);	// ��������� ��������� �� ������� ��� ����������, ������ �� ������������: void AnyFunc(void)

const int MAX_QUEUE =	30;				// 	������������ ����� ������� �����

#pragma pack(push,1)		//������������ ����� � ������ ������ 1 ���� ��� ��������

struct QueueFunc_Struct					// ���������� ��������� ��� �������� ʊ����� ��������� ������
	{
		VoidFunc_ptr	CallingFunc;	// ��������� �� �������, ������� ����� �������
		int				priority_Func;	// ��������� �������

	};
	
#pragma pack(pop)			// �������������� ������������ ����� � ������ �� ���������
	
	
class QueueFunc_List					// ����� ������� �����
{
	private:
		QueueFunc_Struct		Items[MAX_QUEUE];		// ������ ������� �����
		bool					active;					// ������� ���������� ������� �����
		
		void	OrganisateQueue();						// ������������� ������� ����� ��� ������������� �����
		int		error;									// ������� ������ ������� �����

	public:	
		QueueFunc_List();							//����������� ������

		bool	Add(VoidFunc_ptr AFunc, int priority);	// ��������� ������� AFunc � �������, ������ �� ���������
		bool	IsActive();							// true, ���� ��������� ����� �������
		void	Delete(VoidFunc_ptr AFunc);			// ������� ������� �� �������
		void	Step();								// ���������� ��������� ������� �� �������
		void	Start();							// ��������� ��������� �����
		void	Stop();								// ��������� ����� ��������		
		void	Clean();							// ������ ������� �������		
		int		Error();							// ������� ������ ������� �����

};




#endif