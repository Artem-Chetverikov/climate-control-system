#ifndef __NOISEFILTER_H_INCLUDED
#define __NOISEFILTER_H_INCLUDED


//#include <math.h>
#include <cstdint>						// подключаем фиксированные типы данных (типа uint32_t и др.)


class NoiseFilter
{
private:
	
	
	
	
public:
	virtual float filtered (float);		//

};














///////////////////// упрощённый фильтр Калмана для одномерного случая
class FilterKalman : public NoiseFilter
{
private:
	float _err_measure = 0.0;
	float _err_estimate = 0.0;
	float _q = 0.0;
	float _last_estimate = 0.0;
	
	
	
public:
	// разброс измерения, разброс оценки, скорость изменения значений
	FilterKalman(float mea_e, float est_e, float q) { setParameters(mea_e, est_e, q); }
	
	// разброс измерения, скорость изменения значений (разброс измерения принимается равным разбросу оценки)
	FilterKalman(float mea_e, float q) {setParameters(mea_e, mea_e, q);}
	
	// разброс измерения, разброс оценки, скорость изменения значений
	void setParameters(float mea_e, float est_e, float q) {
		_err_measure = mea_e;
		_err_estimate = est_e;
		_q = q;
	}
	
	// разброс измерения, скорость изменения значений (разброс измерения принимается равным разбросу оценки)
	void setParameters(float mea_e, float q) {setParameters(mea_e, mea_e, q);}
	
	// возвращает фильтрованное значение
	float filtered(float value) {		
		float _kalman_gain, _current_estimate;
		_kalman_gain = _err_estimate / (_err_estimate + _err_measure);
		_current_estimate = _last_estimate + _kalman_gain * (value - _last_estimate);
		
		float abs = (_last_estimate-_current_estimate) > 0 ? (_last_estimate-_current_estimate): -(_last_estimate-_current_estimate);
		_err_estimate =  (1.0 - _kalman_gain)*_err_estimate + abs*_q;
		_last_estimate=_current_estimate;
		return _current_estimate;
	}	
	
	
	
};


/*
///////////////////// альфа-бета фильтр
class FilterAB : public NoiseFilter
{
private:
	float dt;
	float xk_1, vk_1, a, b;
	float xk, vk, rk;
	float xm;
	
	
	
public:
	// период дискретизации (измерений), process variation, noise variation
	FilterAB(float delta, float sigma_process, float sigma_noise) {setParameters(delta, sigma_process, sigma_noise);}
	
	// период дискретизации (измерений), process variation, noise variation
	void setParameters(float delta, float sigma_process, float sigma_noise) {
		dt = delta;
		float lambda = (float)sigma_process * dt * dt / sigma_noise;
		float r = (4 + lambda - (float)sqrt(8 * lambda + lambda * lambda)) / 4;
		a = (float)1 - r * r;
		b = (float)2 * (2 - a) - 4 * (float)sqrt(1 - a);
	}
	
	// возвращает фильтрованное значение
	float filtered(float value) {				
		xm = value;
		xk = xk_1 + ((float) vk_1 * dt );
		vk = vk_1;
		rk = xm - xk;
		xk += (float)a * rk;
		vk += (float)( b * rk ) / dt;
		xk_1 = xk;
		vk_1 = vk;
		return xk_1;
	}	
	
	
	
};*/






///////////////////// быстрый медианный фильтр 3-го порядка

class FilterMedian3 : public NoiseFilter
{
private:
	float buffer[3];
	uint8_t _counter = 0;
	
	
	
public:
	float filtered(float value) {	// возвращает фильтрованное значение
		buffer[_counter] = value;
		if (++_counter > 2) _counter = 0;
		
		float middle;	
		
		if ((buffer[0] <= buffer[1]) && (buffer[0] <= buffer[2])) {
			middle = (buffer[1] <= buffer[2]) ? buffer[1] : buffer[2];
		}
		else {
			if ((buffer[1] <= buffer[0]) && (buffer[1] <= buffer[2])) {
				middle = (buffer[0] <= buffer[2]) ? buffer[0] : buffer[2];
			}
			else {
				middle = (buffer[0] <= buffer[1]) ? buffer[0] : buffer[1];
			}
		}
		return middle;
	}
	
	
	
};







///////////////////// медианный фильтр N-го порядка

template < int SIZE = 3 >

class FilterMedian : public NoiseFilter
{
private:
	float buffer[SIZE];
	uint8_t _count = 0;
	uint8_t _numRead = SIZE;	
	
	
	
public:
	float filtered(float newVal) {
		buffer[_count] = newVal;
		if ((_count < SIZE - 1) && (buffer[_count] > buffer[_count + 1])) {
			for (int i = _count; i < SIZE - 1; i++) {
				if (buffer[i] > buffer[i + 1]) {
					float buff = buffer[i];
					buffer[i] = buffer[i + 1];
					buffer[i + 1] = buff;
				}
			}
		} else {
			if ((_count > 0) and (buffer[_count - 1] > buffer[_count])) {
				for (int i = _count; i > 0; i--) {
					if (buffer[i] < buffer[i - 1]) {
						float buff = buffer[i];
						buffer[i] = buffer[i - 1];
						buffer[i - 1] = buff;
					}
				}
			}
		}
		if (++_count >= SIZE) _count = 0;
		return buffer[(int)SIZE / 2];
	}	
	
	
	
};





///////////////////// линейная аппроксимация методом наименьших квадратов

class FilterSqLinear : public NoiseFilter
{
private:
	float a, b, delta;
	
	
	
public:
	FilterSqLinear(){};
	void compute(float *x_array, float *y_array, int arrSize) {		// аппроксимировать
		int32_t sumX = 0, sumY = 0, sumX2 = 0, sumXY = 0;	
		arrSize /= sizeof(int);
		for (int i = 0; i < arrSize; i++) {		// для всех элементов массива
			sumX += x_array[i];
			sumY += (long)y_array[i];
			sumX2 += x_array[i] * x_array[i];
			sumXY += (long)y_array[i] * x_array[i];
		}
		a = (long)arrSize * sumXY;             // расчёт коэффициента наклона приямой
		a = a - (long)sumX * sumY;
		a = (float)a / (arrSize * sumX2 - sumX * sumX);
		b = (float)(sumY - (float)a * sumX) / arrSize;
		delta = a * arrSize;					// расчёт изменения
	}
	float getA() {return a;}			// получить коэффициент А
	float getB() {return b;}			// получить коэффициент В
	float getDelta() {return delta;}	// получить аппроксимированное изменение
	
	
	
};



///////////////////// фильтр
class FilterExpAverage : public NoiseFilter
{
private:
	float _coef = 0, _lastValue = 0;
	uint32_t _filterTimer = 0;
	uint16_t _filterInterval = 0;	
	
	
	
public:
FilterExpAverage() {}

FilterExpAverage(float coef, uint16_t interval) {
	_coef = coef;
	_filterInterval = interval;
}

FilterExpAverage(float coef) {
	_coef = coef;
}

void setCoef(float coef) {
	_coef = coef;
}
void setStep(uint16_t interval) {
	_filterInterval = interval;
}

float filteredTime(float value) {
	if (millis() - _filterTimer >= _filterInterval) {
		_filterTimer = millis();
		return filtered(value);
	}
}

float filtered(float value) {
	_lastValue += (float)(value - _lastValue) * _coef;
	return _lastValue;
}	
	
	
	
};







#endif