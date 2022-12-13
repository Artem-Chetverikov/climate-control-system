#include "floatTOchar.h"
#include "limits.h"







int	floatTOchar(double val, int FFF, char *str, int len)
{
	return	floatTOchar(val, 0, FFF, str, len);
}


int	floatTOchar(double val, int DDD, int FFF, char *str, int len)
{
    int j = 0;
	while(j < (len-1))
	{
		str[j] ='\0';
		j++;
	}

	long iPart = (long)val;
	double fPart = val - (double)iPart;
	
	

	const int LENGTH = 40;
	
	char strInt[LENGTH];
	char strFloat[LENGTH];

	for (int i = 0; i < LENGTH; i++)
    {
        strInt[i]   = '\0';
        strFloat[i] = '\0';
    }


	int lenI = 0;
	int lenF = 0;

	iPart = (iPart<0)?(-iPart):iPart;
	lenI = longTOstr(iPart, DDD, strInt, LENGTH);

	if(0 != FFF)
	{
		int n = FFF;

		while(n)
		{
			fPart = fPart * 10.0;
			n--;
		}
		fPart = (fPart<0)?(-fPart):fPart;
		
		
		lenF = longTOstr((long)fPart, FFF, strFloat, LENGTH);
	}

	if((1 + lenI + 1 + lenF + 1) > len)		// -DDD.FFF'\0' необходимая длина массива 3+DDD+FFF элементов
	{
		int j = 0;
		while(j < (len-1))
		{
			str[j] ='0';
			j++;
		}
		str[j] ='\0';

		return 0;
	}



	int i = 0;
	j = 0;

	if (0 > val)
    {
        str[j] = '-';
        j++;
    }
	else
	{
		str[j] = '+';
		j++;
	}

	while('\0' != strInt[i])
	{
		str[j] = strInt[i];
		i++;
		j++;
	}


	str[j] = ',';
	j++;

	i = 0;

	while('\0' != strFloat[i])
	{
		str[j] = strFloat[i];
		i++;
		j++;
	}

	while(j <= len)
	{
		str[j] ='\0';
		j++;
	}

	return j;

}


int	floatTOcharExp(double val, char *str, int len)
{
	return floatTOcharExp(val, 3, str, len);
}

int	floatTOcharExp(double val, int FFF, char *str, int len)
{
	int j = 0;
	while(j < (len-1))
	{
		str[j] ='\0';
		j++;
	}

	long iPart = (long)val;

	double fPart = val - (double)iPart;
	
	const int LENGTH = 40;

	char strInt[LENGTH];
	char strFloat[LENGTH];

    for (int i = 0; i < LENGTH; i++)
    {
        strInt[i]   = '\0';
        strFloat[i] = '\0';
    }


	int lenI = 0;
	int lenF = 0;

	iPart = (iPart<0)?(-iPart):iPart;
	lenI = longTOstr(iPart, 0, strInt, LENGTH);

	int n = 0;		//счетчик



    fPart = (fPart<0)?(-fPart):fPart;
	
	while((fPart <= (LONG_MAX/10)) && (0.0 != fPart))
	{
		fPart = fPart * 10.0;
		n++;
	}


	lenF = longTOstr((long)fPart, n, strFloat, LENGTH);


    j = 0;
	if((3 + FFF + 6) > len)  // -0.FFFe+038'\0' необходимая длина массива 9+FFF элементов
	{
		while(j < (len-1))
		{
			str[j] ='0';
			j++;
		}
		str[j] ='\0';

		return 0;
	}

	int Exp = 0;


	if(0 < iPart)
	{
		Exp = lenI - 1;
		
		// оставляем 1 символ в целой части, остальные символы переносим в дробную часть
        int p = 0;
        for (int i = 0; i < LENGTH; i++)
        {

            if((48 < strInt[i]) && (58 > strInt[i]))			// включая символы '1' ... '9'
            {
                strInt[0] = strInt[i];
				
				
				for (int i1 = (i+1); i1 < (LENGTH); i1++)
				{
					if ('\0' == strInt[i1]) break;
					p++;					
				}
				
				for (int j = (LENGTH-2); j >= p; j--)
				{
					strFloat[j] = strFloat[j - p];					
				}
				
				for (int j = 0; j < p; j++)
				{
					strFloat[j] = strInt[i+1];
					i++;					
				}
				
                break;

            }
        }
	
	}

	if(0 == iPart)
	{
		Exp--;
		for (int i = 0; i < LENGTH; i++)
		{
			if('0' == strFloat[i]) Exp--;
			if('0' != strFloat[i]) break;

		}


        // переносим один символ в целую часть
        int p = 0;
        for (int i = 0; i < LENGTH; i++)
        {

            if((48 < strFloat[i]) && (58 > strFloat[i]))			// включая символы '1' ... '9'
            {
                strInt[0] = strFloat[i];
                strFloat[i] = 0;
                p = i;
                break;

            }
        }


        // убираем стоящие впереди нули

        for (int i = 0; i < (LENGTH-p); i++)
        {
            strFloat[i] = strFloat[i + p];
        }

	}



	j = 0;



	if (val < 0)
	{
		str[j] = '-';
		j++;
	}
	else
	{
		str[j] = '+';
		j++;
	}


    str[j] = strInt[0];
    j++;


    //вставляем разделительную точку
	str[j] = ',';

	j++;



	for (int i = 0; i < FFF; i++)
	{

		if((47 < strFloat[i]) && (58 > strFloat[i]))			// включая символ '0' ... '9'
		{
			str[j] = strFloat[i];
			j++;
		}
	}





	str[j] = 'e';

	j++;

	str[j] = (Exp > 0)?'+':'-';

	j++;

	Exp = (Exp < 0)?-Exp:Exp;


	char *ptr = &str[j];

	longTOstr(Exp, 3, ptr, 4);

	j++;
	j++;
	j++;

	while(j <= len)
	{
		str[j] ='\0';
		j++;
	}

	return j;






}


int		longTOstr(long NNN, int d, char *str, int len)
{
	int i = 0;

	if (NNN < 0)
	{
		str[i] = '-';
		i++;
	}

	NNN = (NNN<0)?(-NNN):NNN;

	while((i < len) && NNN > 0)
	{
		str[i] = (NNN%10)+'0';
		NNN = NNN/10;
		i++;
	}



	d = (d<=0)?1:d;
	while((i < d) && (d < len))
	{
		str[i] ='0';
		i++;
	}

	if('-' == str[0]) reverse_string(str, 1, i);
	else reverse_string(str, 0, i);

	int j = i;

	while(i < len)
	{
		str[i] ='\0';
		i++;
	}

	return j;

}


void	reverse_string( char *str, int start, int len)
{
	int i,j,temp;
	i 		= start;
	j 		= len - 1;
	temp 	= 0;

	while (i < j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;

	}
}
