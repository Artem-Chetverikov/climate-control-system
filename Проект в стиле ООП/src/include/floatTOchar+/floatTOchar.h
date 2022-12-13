#ifndef __FLOATTOCHAR_H_INCLUDED
#define __FLOATTOCHAR_H_INCLUDED



// преобразование float-числа в строку char в формате DNN.FFF
int	floatTOchar(double val, int DDD, int FFF, char *str, int len);

// преобразование float-числа в строку char в формате NN.FFF
int	floatTOchar(double val, int FFF, char *str, int len);


// преобразование float-числа в строку char в формате N.FFFe+DD
int	floatTOcharExp(double val, int FFF, char *str, int len);

int	floatTOcharExp(double val, char *str, int len);		//FFF = 3


// преобразование long-числа в строку в формате 000NNN (0NNNNN; d - количество знаков, если в NNN меньше знаков, то дополняется нулями)
int	longTOstr(long NNN, int d, char *str, int len);


// обращает указанную строку указанной длины
void	reverse_string( char *str, int start, int len);



#endif
