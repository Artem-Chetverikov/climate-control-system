//
//	  Тестирование производилось на Arduino IDE 1.8.7
//	  Дата тестирования 15.12.2019г.

//Скетч использует 54660 байт (21%) памяти устройства. Всего доступно 253952 байт.
//Глобальные переменные используют 3523 байт (43%) динамической памяти, оставляя 4669 байт для локальных переменных. Максимум: 8192 байт.

//	?? - вопросы

//Подключаемые библиотеки:

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <SdFat.h>
#include <avr/wdt.h>				   //подключаем библиотеку WatchDog таймера

//////////////////////////////////////////////////////////////////////////////////////////////////////



//Подключение к пинам Arduino:

#define ONE_WIRE_BUS		5			// датчики DS18B20 (шина данных)
#define LED_GREEN			54			// пин зеленой лампочки
#define LED_RED				55			// пин красной лампочки
#define DHT21_1_PIN			15			// пин датчик DHT 21 (шина данных)
#define DADA_PIN			56			// пин датчика SHT1x (шина данных)
#define CLOCK_PIN			57			// пин датчика SHT1x (синхоимпульс)
#define MOTOR_PIN_1			30			// обмотка StepMotor
#define MOTOR_PIN_2			31			// обмотка StepMotor
#define MOTOR_PIN_3			32			// обмотка StepMotor
#define MOTOR_PIN_4			33			// обмотка StepMotor
#define MOTOR_PIN_5			28			// E1 driver StepMotor
#define MOTOR_PIN_6			29			// E2 driver StepMotor
#define RELE_CO2			25			// реле вентилятор приточный 	R1
#define RELE_HIDRO			24			// реле увлажнитель				R2
#define RELE_HOT			9			// реле нагреватель				R3
#define RELE_PUMP			34			// реле насоса					R4
#define RELE_COLD			6			// реле холодильник				R5
#define RELE_POWER			58			// реле питания					R6
#define RELE_LIGHT			8			// реле освещения				R7
#define DC_MOTOR_1_A		26			// "+" DC-мотора №1
#define DC_MOTOR_1_B		27			// "-" DC-мотора №1
#define RELE_5V				44			// реле питания датчиков, часов, SD-card, концевиков   R8
#define BUTTON_PIN_DOOR_	7			// кнопка дверцы холодильника
#define BUTTON_PIN_WATER_	59			// датчик уровня воды в УЗУ
#define BUTTON_PIN_1_		18			// кнопка S1  (прерывание 4)
#define BUTTON_PIN_2_		19			// кнопка S2  (прерывание 5)
#define BUTTON_PIN_3_		3			// кнопка S3  (прерывание 0)
#define BUTTON_PIN_4_		2			// кнопка S4  (прерывание 1)




// Объявляем переменные для работы с датчиками температуры DS18b20:

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
const uint8_t insideThermometer1[8] = { 0x28, 0xFF, 0xD3, 0x59, 0x58, 0x16, 0x04, 0x6A };	// тут будет хранится адрес нашего датчика DS18B20 №1
const uint8_t insideThermometer2[8] = { 0x28, 0xFF, 0xF4, 0x5E, 0x55, 0x16, 0x03, 0xD6 };	// тут будет хранится адрес нашего датчика DS18B20 №2
const uint8_t insideThermometer3[8] = { 0x28, 0xFF, 0xBB, 0x28, 0x55, 0x16, 0x03, 0xF1 };	// тут будет хранится адрес нашего датчика DS18B20 №3
const uint8_t insideThermometer4[8] = { 0x28, 0xFF, 0x7B, 0xBA, 0x58, 0x16, 0x04, 0xA6 };	// тут будет хранится адрес нашего датчика DS18B20 №4
long tempC1;			//текущая температура х100  _почвы_
long tempC2;			//текущая температура х100  _в нагревателе_
long tempC3;			//текущая температура х100  _в морозильнике_
long tempC4;			//текущая температура х100  _окружающей среды_
int read_DS18b20_error = 0;												 //счетчик ошибок чтения с датчиков DS18b20









// Объявляем переменные для работы с часами RCT DS1307:

tmElements_t tm;					 //класс библиотеки TimeLib.h, содержащий значения г, м, д, ч, мин, с
unsigned int last_time_sec_1 = 0;		// для опроса датчиков
unsigned int last_time_min_1 = 0;		 // для записи данных на SD-card

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};




//Объявляем переменные для индикатора работы:

#define DELAY_GREEN_0 1000							   //количество циклов loop(){} на импульс
#define DELAY_GREEN_PAUSE 10000						   //количество циклов loop(){} на паузу
long step_green = 0;								   //счетчик циклов loop(){}




// Объявляем переменные для обработчика ошибок:

volatile unsigned int array_position = 0;				//индекс массива длительностей и массива импульсов
volatile unsigned int error_step = 0;					//текущий номер ошибки
#define ARRAY_LENGH 150									//длина массива длительностей и массива импульсов
#define ARRAY_LENGH_N 16								//длина массива наличия ошибок (количество ошибок = ARRAY_LENGH_N - 1)
volatile unsigned int error_array_N[ARRAY_LENGH_N];		//массив наличия ошибок
volatile unsigned int error_array_delay[ARRAY_LENGH];	//массив длительностей импульсов
volatile unsigned int error_array_set[ARRAY_LENGH];		//массив порядка следования импульсов
#define DELAY_RED_0 4000								//длительность нулевого сигнала, ~ мс , MAX 4190 мс
#define DELAY_RED_PAUSE 3000							//пауза между ошибками, ~ мс, MAX 4190 мс
#define DELAY_RED_PULSE 500								//длительность одного импульса, ~ мс, MAX 4190 мс

//номера ошибок
int error_SDcard				= 1;		// *							//при ошибках инициализации и чтения
int error_RCT					= 2;		// *_*							//при ошибках инициализации и настройки времени
int error_DHT21					= 3;		// *_*_*						//при ошибках чтения данных с датчика
int error_MHZ19					= 4;		// *_*_*_*						//при ошибках чтения с датчика
int error_DS18b20				= 5;		// *_*_*_*_*					//при ошибках чтения с датчиков
//int error_temp					= 6;		// *_*_*_*_*_*					//при отклонении температуры в камере больше заданной величины
//int error_ppm					= 7;		// *_*_*_*_*_*_*				//при отклонении концентрации СО2 в камере больше заданной величины
//int error_hidro					= 8;		// *_*_*_*_*_*_*_*				//при отклонении влажности в камере больше заданной величины
volatile int error_DCmotor		= 9;		// *_*_*_*_*_*_*_*_*			//при несрабатывании концевика дольше заданного времени
volatile int error_StepMotor	= 10;		// *_*_*_*_*_*_*_*_*_*			//при невЫключении концевика дольше заданного времени при сходе с позиции
int error_SHT1x					= 11;		// *_*_*_*_*_*_*_*_*_*_*		//при ошибках чтения данных с датчика SHT1x
volatile int error_task			= 12;		// *_*_*_*_*_*_*_*_*_*_*_*		//при переполнении очереди задач
int error_pump					= 13;		// *_*_*_*_*_*_*_*_*_*_*_*_*	//при ошибках в системе УЗУ
/*14
15
*/





// Объявляем переменные для работы с датчиком DHT 21:

bool	   _firstreading = true;
unsigned long	_lastreadtime = 0;
uint8_t	  _bit ;
uint8_t	  _port ;
unsigned long  _maxcycles ;
volatile bool _lastresult;
long temp_c_DHT21_1 = 0;					//текущее значение температуры
long humidity_DHT21_1 = 0;					//текущее значение влажности
uint8_t data[6];							//массив для хранения ответа датчика DHT 21
int read_DHT_error = 0;					//счетчик ошибок чтения с датчика DHT 21
unsigned long currenttime;
uint32_t cycles[83];						//буфер для хранения приходящих битов от DHT 21

int read_DHT21_step = 1;						//этап опроса датчика DHT21




// Объявляем переменные для работы с датчиком SHT1x:

int temp_SHT1x = 1111;									// текущее значение температуры
int humidity_SHT1x = 1111;									// текущее значение влажности

int read_SHT1x_step = 1;						//этап опроса датчика SHT1x




// Объявляем переменные для работы с датчиком CO2  MH-Z19:

byte cmd[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};	// команда запроса уровня СО2
unsigned char response[9];												// массив для ответа датчика СО2
int ppm;																// уровень СО2, ppm
int read_MHZ19_error = 0;												//счетчик ошибок чтения с датчика MH-Z19
unsigned long time_out_MHZ19;											// таймаут ожидания ответа с датчика MH-Z19




//Объявляем переменные для управления насосом
//unsigned long last_time_sec_9 = 0;			// для проверки длительности работы насоса
unsigned long last_time_sec_10 = 0;			// для проверки длительности работы насоса
int pump_step_1 = 1;					// для проверки длительности работы насоса
int pump_step_2 = 1;					// для проверки длительности работы насоса
int water_level;						// уровень воды в емкости УЗУ
#define WATER_NORM	1					// нормальный уровень воды
#define WATER_LOW	0					// низкий уровень воды






// Объявляем переменные для работы с SD-card 
//(подключена к pin miso - 50, mosi - 51, sck - 52, CS - 53):

SdFat sd;	// класс библиотеки SdFat.h

FatFile myFile;											// класс библиотеки SdFat.h

#define SD_CHIP_SELECT_PIN 53							// CS-выход SD-card
String name_file;										// имя создаваемого файла
String YY_1;											// часть имени создаваемого файла
String MM_1;											// часть имени создаваемого файла
String DD_1;											// часть имени создаваемого файла
String TXT_1;											// часть имени создаваемого файла
String NN_1;
int nn1 = 0;											// счетчик файлов одного дня
int file_open_error = 5;								// количество попыток открытия файла со счетчиком nn1
int last_SD_hour;										// для разбиения файлов через отведенное время
#define SD_HOUR 2										// новый файл каждые 2 часа
int last_time_hour_3;									// последний час создания файла



// Объявляем переменные для работы с StepMotor:

const int stepsPerRevolution = 200;		// количество шагов за оборот шагового двигателя
volatile long step_number = 0;			// текуший номер шага
volatile long steps_left;				// количество шагов до окончания при заданном количестве оборотов (доступна по прерыванию)
long step_delay_default;				// длительность одного шага, в мкс, зависит от скорости об/мин (вычисляется по ходу)
volatile long step_delay;				// длительность одного шага при разгоне и торможении, в мкс, зависит от скорости об/мин (вычисляется по ходу)
const int steps_acceleration = 60;		// количество шагов за оборот шагового двигателя
volatile long abs_number_of_steps;		// без учета направления
long grad_10;							// количество	  градусов * 10	   для поворота исходя из заданной позиции
int whatSpeed;							// максимальная скорость StepMotor об/мин (опытная величина)
int direction;							// направление вращения StepMotor
int last_direction;						// направление при предыдущем перемещении
int pre_direction = -1;					// множитель для калибровки StepMotor (полярность обмоток) 1 или -1
int angle_10;							// требуемый угол *10 поворота заслонки
int this_angle_10;						// текущий угол *10 заслонки
volatile unsigned long last_time_sec_4 = 0;	// для проверки при невЫключении концевика дольше заданного времени при сходе с позиции
volatile int val_19_1_go = 0;			// для начала счета шагов вновь от заданной величины при сходе с позиции
volatile int val_19_9_go = 0;			// для начала счета шагов вновь от заданной величины при сходе с позиции
volatile unsigned int time_out_StepMotor = 1500;	// тайм-аут схода заслонки с концевика, мс (доступна по прерыванию)
long time_out_StepMotor_1 = 300;		// тайм-аут проверки на дребезг, мс (доступна по прерыванию) MAX 4190 мс

volatile int this_position_12 = 2;		// текушая позиция заслонки (от 0 до 1)(доступна по прерыванию)
volatile int this_position_19;			// текушая позиция заслонки (от 1 до 9)(доступна по прерыванию)
int this_position_19_angle;				// буфер для текущей позиции заслонки
int position_19;						// требуемая позиция заслонки
int delta_angle_StepMotor = 30;			// для корректировки гистерезиса от люфта заслонки, ° * 10
int StepMotor_last_position = 1;		// последняя заданная позиция
int calibration_StepMotor_step = 1;		// шаг калибровки StepMotor
int position_colibration_SM;			// позиция в начале калибровкиStepMotor
unsigned long last_time_sec_13 = 0;		// для калибровки StepMotor

//углы поворота заслонки  ()° * 10
int angle_1_10 = 0;
int angle_2_10 = 50;
int angle_3_10 = 125;
int angle_4_10 = 250;
int angle_5_10 = 450;
int angle_6_10 = 650;
int angle_7_10 = 775;
int angle_8_10 = 850;
int angle_9_10 = 900;





// Объявляем переменные для работы с DC-Motor:

bool direction_DC = 0;						// направление вращения DC-мотора №1
long time_out_DCmotor_1 = 300;				// тайм-аут проверки на дребезг, мс (доступна по прерыванию) MAX 4190 мс
long time_out_DCmotor_2 = 3000;				// тайм-аут проверки срабатывания концевика DC-мотора после хода, мс
int position_colibration_DC;				// позиция в начале калибровки DC-мотора
int DCmotor_last_position = 0;				// последняя заданная позиция DC-мотора
int calibration_DC_motor_step = 1;			// шаг калибровки DC-мотора
unsigned long last_time_sec_12 = 0;			// для калибровки DC-мотора
int flag_calibration_DC_motor = 1;			// флаг выполнения Начальной калибровки





//Объявляем переменные концевых выключателей

volatile int val_12_1_1 = 0;		// текуший уровень сигнала на pin19 (доступна по прерыванию)
volatile int val_12_2_1 = 0;		// текуший уровень сигнала на pin18 (доступна по прерыванию)
volatile int val_19_1_1 = 0;		// текуший уровень сигнала на pin2  (доступна по прерыванию)
volatile int val_19_9_1 = 0;		// текуший уровень сигнала на pin3  (доступна по прерыванию)
int val_12_1_2 = 0;					// текуший уровень сигнала на pin19 (по второму вызову)
int val_12_2_2 = 0;					// текуший уровень сигнала на pin18 (по второму вызову)
int val_19_1_2 = 0;					// текуший уровень сигнала на pin2  (по второму вызову)
int val_19_9_2 = 0;					// текуший уровень сигнала на pin3 ( по второму вызову)


int this_position_12_end;				// позиция заслонки на концевике( 1 или 2)
int this_position_19_end;				// позиция заслонки на концевике( 1 или 9)



//переменные для управления перезагрузкой по WatchDog при возникновении ошибок
unsigned long last_time_sec_5 = 0;
int reboot_DHT21 = 0;				//управление перезагрузкой по ошибкам DHT21

unsigned long last_time_sec_7 = 0;
int reboot_MHZ19 = 0;				//управление перезагрузкой по ошибкам MHZ19

unsigned long last_time_sec_8 = 0;
int reboot_SHT1x = 0;				//управление перезагрузкой по ошибкам SHT1x

int reboot_SDcard = 0;				//управление перезагрузкой по ошибкам SDcard

int level_error_reboot = 0;			//количество выполненных перезагрузок




//	Асинхронный таймер с шагом ~1 мс

#define AND &&
#define OR	||
#define NOT !

using	PVoidFunc = void(*)(void);		// указатель на функцию без параметров, ничего не возвращающую: void AnyFunc(void)

#define		MAXTIMERSCOUNT		20		//	Максимальное число зарегистрированных таймеров для Mega2560 - 16

#define		_1MSCONST			1		//	задержка 1 миллисекунда	 на таймере Timer0

extern bool InRange(int,int,int);

using THandle = int8_t;					// порядковый номер асинхронного таймера

struct TCallStruct {					// внутренняя структура для хранения КАЖДОГО таймера

	PVoidFunc	CallingFunc;			// функция, которую нужно вызвать при срабатывании
	long		InitCounter;			// заданное кол-во миллисекунд
	long		WorkingCounter;			// рабочий счетчик. после начала счёта сюда копируется значение из InitCounter
	bool		Active;					// и каждую миллисекунду уменьшается на 1. При достиженнии 0 вызывается функция
										// CallingFunc, и снова записывается значение из InitCounter. Счёт начинается сначала. :)
};

class TTimerList {
	private:
		TCallStruct		Items[MAXTIMERSCOUNT];
		void			Init();
		bool			active;
		byte			count;

	public:
		TTimerList();

		THandle		 Add(PVoidFunc AFunc, long timeMS);			// функция AFunc, которую надо вызвать через timeMS миллисекунд
		THandle		 AddSeconds(PVoidFunc AFunc, word timeSec); // то же, только интервал задается в секундах.
		THandle		 AddMinutes(PVoidFunc AFunc, word timeMin); // то же, только интервал задается в минутах.
																// исключительно для удобства создания длинных интервалов

		bool		 CanAdd();							// если можно добавить таймер, вернет true

		bool		 IsActive();						// true, если хоть один таймер запущен

		void		 Delete(THandle hnd);				// удалить таймер hnd

		void		 Step(void);

		void		 Start();							// запустить рабочий цикл перебора таймеров

		void		 Stop();							// TimerList выключен, все таймеры остановлены

		byte		 Count();							// счетчик добавленных таймеров, всего. Можно не использовать, просто проверять возможность
														// добавления, вызывая ф-цию CanAdd();

		void		 TimerStop(THandle hnd);			// остановить отдельный таймер по его хэндлу

		bool		 TimerActive(THandle hnd);			// запущен ли конкретный таймер

		void		 TimerStart(THandle hnd);			// запустить отдельный таймер (после остановки)
														// счет не продолжается, а начинается сначала

		void		 TimerNewInterval(THandle hnd, long newinterval); // устанавливает новый интервал срабатывания для таймера hnd
																	  // и запускает его (если он был остановлен)
};

TTimerList TimerList;

volatile THandle THandle_DHT21;				//внутренний номер таймера для опроса датчика DHT21
volatile THandle THandle_DC_motor_task;		//внутренний номер таймера на запуск calibration_DC_motor()
volatile THandle THandle_Step_motor_task;	//внутренний номер таймера на запуск calibration_Step_motor()
volatile THandle THandle_SHT1x;				//внутренний номер таймера для опроса датчика SHT1x
volatile THandle THandle_SHT1x_1_task;		//внутренний номер таймера для опроса датчика SHT1x
volatile THandle THandle_SHT1x_2_task;		//внутренний номер таймера для опроса датчика SHT1x
volatile THandle THandle_MHZ19;				//внутренний номер таймера для опроса датчика MH-Z19
volatile THandle THandle_DS18b20;			//внутренний номер таймера для опроса датчика DS18b20
volatile THandle THandle_CO2;				//внутренний номер таймера для регулирования CO2
volatile THandle THandle_hidro;				//внутренний номер таймера для регулирования hidro
volatile THandle THandle_Temp;				//внутренний номер таймера для регулирования Temp
volatile THandle THandle_Hot_Cold;			//внутренний номер таймера для регулирования Hot_Cold
volatile THandle THandle_water;				//внутренний номер таймера для регулирования water
volatile THandle THandle_light;				//внутренний номер таймера для регулирования light
volatile THandle THandle_SDcard;			//внутренний номер таймера для записи на SDcard
volatile THandle THandle_RCT;				//внутренний номер таймера для опроса часов RCT DS1307
volatile THandle THandle_DC_motor_failure;	//внутренний номер таймера для аварийного отключения DC_motor



// очередь вызова функций:
#define ARRAY_FLAGS_LENGH 30								//длина массива флагов
volatile int array_flags[ARRAY_FLAGS_LENGH];

#define READ_DHT_FLAG				1	//флаг на запуск read_DHT()
#define CALIBRATION_DC_MOTOR_FLAG	2	//флаг на запуск calibration_DC_motor()
#define CALIBRATION_STEP_MOTOR_FLAG	3	//флаг на запуск calibration_Step_motor()
#define READ_SHT1X_FLAG				4	//флаг на запуск read_SHT1x()
#define READ_SHT1X_1_FLAG			5	//флаг на запуск read_SHT1x_1()
#define READ_SHT1X_2_FLAG			6	//флаг на запуск read_SHT1x_2()
#define READ_MHZ19_FLAG				7	//флаг на запуск read_MHZ19()
#define READ_DS18B20_FLAG			8	//флаг на запуск read_DS18b20()
#define READ_RCT_FLAG				9	//флаг на запуск read_RCT()
#define REGULATE_CO2_FLAG			10	//флаг на запуск regulate_CO2()
#define REGULATE_HIDRO_FLAG			11	//флаг на запуск regulate_hidro()
#define REGULATE_TEMP_FLAG			12	//флаг на запуск regulate_Temp()
#define REGULATE_HOT_COLD_FLAG		13	//флаг на запуск regulate_Hot_Cold()
#define REGULATE_WATER_FLAG			14	//флаг на запуск regulate_water()
#define REGULATE_LIGHT_FLAG			15	//флаг на запуск regulate_light()
#define WRITE_FILE_FLAG				16	//флаг на запуск write_file()





//>>>>>>>>>>>>>>>_____переменные управления микроклиматом:______<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


unsigned long last_time_sec_2 = 0;		// для проверки открытия дверцы
unsigned long last_time_sec_3 = 0;		// для проверки открытия дверцы
unsigned long last_time_sec_6 = 0;		// для проверки закрытия дверцы
unsigned long last_time_sec_14 = 0;		// для проверки закрытия дверцы

#define DOOR_OPEN	1					// Внутренняя дверь Открыта
#define DOOR_CLOSE	0					// все двери Закрыты
#define DOOR_PRE_OPEN 2					// Наружная дверь Открыта
int door_position;						// начальное положение дверцы (0 - закрыта)

#define DOOR_OPEN_ONE	1				// первый момент Открытия двери
#define DOOR_CLOSE_ONE	0				// первый момент Закрытия двери
int door_position_one = 0;					// начальное положение дверцы (0 - закрыта)

int door_step_1 = 1;					// флаг при закрывании двери
int door_step_2 = 1;					// флаг при закрывании двери

#define DOOR_ARRAY_LENGTH	4			// длина массива нажатий (4 - 2 нажатия, 6 - 3 нажатия)
unsigned long array_door_button[DOOR_ARRAY_LENGTH];		// массив для подсчета нажатий при переводе в режим загрузки камеры
int index_door_button;					// индекс элемента массива нажатий


int level_CO2;					//заданный уровень СО2
int close_CO2 = 0;
int open_CO2  = 1;

int level_hidro;				//заданный уровень влажности в камере   (с SHT1x)
int level_Temp;					//заданный уровень температуры в камере (с SHT1x)
int level_Temp_1;				//заданная температура воздуха  _в морозильнике_ или в _в нагревателе_  (одна переменная для обоих)

unsigned long last_time_hour_1 = 1;		//для вычисления параметров на следующие сутки
int last_time_hour_2 = 0;		//для плавного изменения температуры в камере (не более 2°С в час)

unsigned long last_time_sec_11 = 0;	// для плавного изменения температуры в камере (не более 2°С в час)
long level_Temp_dynamic;			//для плавного изменения температуры в камере (не более 2°С в час)
int step_regulate_Temp;			//шаг регулировки температуры Temp (для определения параметров текущего шага регулировки)
long hidro_in_timeperiod = 3;	//сек., время работы УЗУ в цикл
long timeperiod = 60;			//сек., цикл работы УЗУ
int hidro_iter_1 = 0;			//для увеличения рабочего времени УЗУ в цикле
int hidro_iter_2 = 0;			//для увеличения рабочего времени УЗУ в цикле



//				шаг регулирования:	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35
int array_step_Temp[36]			= {	2,	2,	4,	4,	6,	6,	8,	8,	10,	10,	12,	12,	14,	14,	16,	16,	17,	17,	18,	18,	30,	30,	40,	40,	50,	50,	60,	60,	70,	70,	80,	80,	90,	90,	95,	100};
int array_step_piosition[36]	= {	9,	8,	8,	7,	7,	6,	6,	5,	5,	4,	4,	3,	3,	2,	2,	1,	1,	1,	1,	2,	2,	3,	3,	4,	4,	5,	5,	6,	6,	7,	7,	8,	8,	9,	9,	9};


//			циклограмма температуры в камере
//цикл 1
#define DAY_CYCLE_1	   1				//дней в 1 цикле
#define TEMP_CYCLE_1   14				//температура в 1 цикле 
#define HIDRO_CYCLE_1  75				//влажность в 1 цикле
#define CO2_CYCLE_1	   1000				//уровень СО2 в 1 цикле
#define LIGHT_CYCLE_1  0				//свет не нужен

//цикл 2
#define DAY_CYCLE_2	   1				//дней в 2 цикле
#define TEMP_CYCLE_2   3				//температура в 2 цикле 
#define HIDRO_CYCLE_2  75				//влажность в 2 цикле
#define CO2_CYCLE_2	   2000				//уровень СО2 в 2 цикле
#define LIGHT_CYCLE_2  0				//свет не нужен

//цикл 3
#define DAY_CYCLE_3	   1				//дней в 3 цикле
#define TEMP_CYCLE_3   22				//температура в 3 цикле  
#define HIDRO_CYCLE_3  75				//влажность в 3 цикле
#define CO2_CYCLE_3	   500				//уровень СО2 в 3 цикле
#define LIGHT_CYCLE_3  1				//свет вКлючен

#define CYCLE_ARRAY_LENGTH	3			//(DAY_CYCLE_1 + DAY_CYCLE_2 + DAY_CYCLE_3)	


volatile unsigned int cycle_array_day[CYCLE_ARRAY_LENGTH];	   //массив текущего дня циклограммы
volatile unsigned int cycle_array_temp[CYCLE_ARRAY_LENGTH];	   //массив температур циклограммы
volatile unsigned int cycle_array_hidro[CYCLE_ARRAY_LENGTH];   //массив уровней влажности циклограммы
volatile unsigned int cycle_array_CO2[CYCLE_ARRAY_LENGTH];	   //массив уровней CO2
volatile unsigned int cycle_array_ligh[CYCLE_ARRAY_LENGTH];	   //массив освещенности циклограммы


//день начала цикла <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<!!!!!!!!
int day_cycle = 1;					// (1 - первый день)  (элемент массива cycle_array_da[0])






//*****************************************-----------ПРОТОТИПЫ функций-----------*******

void task_manager();

void read_DHT_in_task();
void calibration_DC_motor_in_task();
void calibration_Step_motor_in_task();
void read_SHT1x_in_task();
void read_SHT1x_1_in_task();
void read_SHT1x_2_in_task();
void read_MHZ19_in_task();
void read_DS18b20_in_task();
void read_RCT_in_task();
void regulate_CO2_in_task();
void regulate_hidro_in_task();
void regulate_Temp_in_task();
void regulate_Hot_Cold_in_task();
void regulate_water_in_task();
void regulate_light_in_task();
void write_file_in_task();



void calibration_motor();
void calibration_DC_motor();
void calibration_Step_motor();
void error_calibration_DC_motor();
void error_calibration_Step_motor();
void DC_Motor_Go(int position_12);
void DC_motor_failure();
void read_Button();
void read_pin_door();
void StepMotor_Go(int position_19_next);
void stepMotor_Step(int thisStep);
void Timer1_init();
void position_12_1_stop();
void position_12_2_stop();
void position_19_1_stop();
void position_19_9_stop();
void Timer4_init();
void Timer5_init();
void read_MHZ19();
void read_DS18b20();
void read_RCT();
void error();
void Timer3_init();
void flash_green();
void write_file();
void error_SD();
void post_reboot_system();
void crash_RCT();
void regulate_CO2();
void regulate_hidro();
void regulate_Temp();
void regulate_Hot_Cold();
void regulate_water();
void regulate_light();
void reboot_system();
void reboot_5V();
void power_down();

void read_DHT();
uint32_t expectPulse(bool level);

void read_SHT1x();
void read_SHT1x_1();
void read_SHT1x_2();
void sendCommandSHT(int _command, int _DADA_PIN, int _CLOCK_PIN);
void skipCrcSHT(int _DADA_PIN, int _CLOCK_PIN);
int getData16SHT(int _DADA_PIN, int _CLOCK_PIN);
int shiftIn(int _DADA_PIN, int _CLOCK_PIN, int _numBits);





//-------------------------------------------------	   SETUP
void setup() {
//	Serial.begin(9600);

	wdt_enable(WDTO_8S);						 //запуск WatchDog таймера на 8 сек

	//перехват управления питанием системы
	pinMode(RELE_POWER, OUTPUT);
	digitalWrite(RELE_POWER, HIGH);
	
	
	// установка пинов Arduino цветовой сигнализации  на "вывод":
	pinMode(LED_RED, OUTPUT);
	pinMode(LED_GREEN, OUTPUT);

	// пины Arduino к которым подключены светодиоды цветовой сигнализации:


	for (int e1 = 0; e1 < 60; e1++){							 // мигание при запуске (Приветствие)
		digitalWrite(LED_RED, (e1 % 2));
		digitalWrite(LED_GREEN, (e1 % 2));
		delay(digitalRead(LED_GREEN) ? 100 : 400);
		wdt_reset();	 //сброс WatchDog таймера
	}

	digitalWrite(LED_RED, LOW);
	digitalWrite(LED_GREEN, LOW);
		

	
	
	cli();			 //вЫкючаем прерывания



	// Обнуление таймера 1 (StepMotor)
	TCCR1B = 0;
	TCCR1A = 0;
	TCNT1 = 0;

	// Обнуление таймера 2 (асинхронный таймер)
	TCCR2B = 0;
	TCCR2A = 0;
	TCNT2 = 0;

	// Обнуление таймера 3 (индикатор ошибок)
	TCCR3B = 0;
	TCCR3A = 0;
	TCNT3 = 0;

	// Обнуление таймера 4(концевики 1 и 2)
	TCCR4B = 0;
	TCCR4A = 0;
	TCNT4 = 0;

	// Обнуление таймера 5(концевики 3 и 4)
	TCCR5B = 0;
	TCCR5A = 0;
	TCNT5 = 0;

	sei();			 //вКлючаем прерывания


	
	
	
	//обнуление очереди задач
	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {
		array_flags[i] = 0;
	}



	//установка дневных параметров цикла
	for (int i = 0; i < CYCLE_ARRAY_LENGTH; i++) {

		if (i < DAY_CYCLE_1) {
			cycle_array_temp[i]	 = TEMP_CYCLE_1;
			cycle_array_hidro[i] = HIDRO_CYCLE_1;
			cycle_array_CO2[i]	 = CO2_CYCLE_1;
			cycle_array_ligh[i]	 = LIGHT_CYCLE_1;
			cycle_array_day[i]	 = 0;
		}
		if ((i >= DAY_CYCLE_1) && (i < (DAY_CYCLE_1 + DAY_CYCLE_2))) {
			cycle_array_temp[i]	 = TEMP_CYCLE_2;
			cycle_array_hidro[i] = HIDRO_CYCLE_2;
			cycle_array_CO2[i]	 = CO2_CYCLE_2;
			cycle_array_ligh[i]	 = LIGHT_CYCLE_2;
			cycle_array_day[i]	 = 0;
		}
		if (i >= (DAY_CYCLE_1 + DAY_CYCLE_2)) {
			cycle_array_temp[i]	 = TEMP_CYCLE_3;
			cycle_array_hidro[i] = HIDRO_CYCLE_3;
			cycle_array_CO2[i]	 = CO2_CYCLE_3;
			cycle_array_ligh[i]	 = LIGHT_CYCLE_3;
			cycle_array_day[i]	 = 0;
		}
	}

	

	//установка текущего дня цикла в массиве			  (0 - первый день в массиве)
	for (int i = 0; i < day_cycle; i++) {
		cycle_array_day[i] = 1;
	}


	level_Temp	= cycle_array_temp[day_cycle - 1];		// температура в камере на текущий день
	level_CO2	= cycle_array_CO2[day_cycle - 1];		// уровень СО2 в камере на текущий день
	level_hidro = cycle_array_hidro[day_cycle - 1];		// влажность в камере на текущий день


	
	//подключаем последовательный порт №2 для датчика CO2  MH-Z19
	Serial2.begin(9600);
	

	pinMode(RELE_5V, OUTPUT);	// управление питанием датчиков, часов, SD-card, концевиков

	
	//установка пина датчика уровня воды
	pinMode(BUTTON_PIN_WATER_, INPUT_PULLUP);
	//проверка начального уровня воды в емкости УЗУ
	if (digitalRead(BUTTON_PIN_WATER_) == HIGH){
		water_level = WATER_NORM;
	}	
	if (digitalRead(BUTTON_PIN_WATER_) == LOW){
		water_level = WATER_LOW;
	}

	
	

	//установка пина кнопки двери камеры
	pinMode(BUTTON_PIN_DOOR_, INPUT_PULLUP);
	// проверка начального положения двери
	if ((digitalRead(BUTTON_PIN_DOOR_) == HIGH)) {	  // если дверь Закрыта
		door_position = DOOR_CLOSE;
	}
	if ((digitalRead(BUTTON_PIN_DOOR_) == LOW)) {	 // если дверь Открыта
		door_position = DOOR_PRE_OPEN;
	}
	//обнуление массива нажатий
	for (int i = 0; i < DOOR_ARRAY_LENGTH; i++) {
		array_door_button[i] = 0;
	}
	index_door_button = 0;


	
	// Определяем переменные для работы с датчиком DHT21:
	_bit = digitalPinToBitMask(DHT21_1_PIN);
	_port = digitalPinToPort(DHT21_1_PIN);
	_maxcycles = (16000000/1000/1000)*200;						// циклов микроконтроллера за 1 мкс.
	pinMode(DHT21_1_PIN, INPUT);							   //датчик	 DHT21
	digitalWrite(DHT21_1_PIN, HIGH);						   //датчик	 DHT21




	// Определяем переменные для работы с датчиком SHT1x:
/*
	int readStatus_SHT1x  = 0b00000111;	  //команда на чтение статусного регистра
	int writeStatus_SHT1x = 0b00000110;	  //команда на запись статусного регистра
	int softReset_SHT1x	  = 0b00011110;	  //команда на перезагрузку датчика SHT1x (установка флагов регистра по умолчанию)
	int ask_SHT1x;

	sendCommandSHT(softReset_SHT1x, DADA_PIN, CLOCK_PIN);

	delay (100);

	sendCommandSHT(readStatus_SHT1x, DADA_PIN, CLOCK_PIN);

	ask_SHT1x = getData16SHT(DADA_PIN, CLOCK_PIN);
	skipCrcSHT(DADA_PIN, CLOCK_PIN);

*/



	sensors.begin();								// готовность датчиков DS18b10
	sensors.setResolution(insideThermometer1, 10);	// устанавливаем разрешение датчика на 10 бит (либо 9 бит)
	sensors.setResolution(insideThermometer2, 10);	// устанавливаем разрешение датчика на 10 бит (либо 9 бит)
	sensors.setResolution(insideThermometer3, 10);	// устанавливаем разрешение датчика на 10 бит (либо 9 бит)
	sensors.setResolution(insideThermometer4, 10);	// устанавливаем разрешение датчика на 10 бит (либо 9 бит)



	wdt_reset();	 //сброс WatchDog таймера
	

	// установка пинов Arduino реле № 1 - 5	 и 6 на "вывод":
	pinMode(RELE_CO2, OUTPUT);
	pinMode(RELE_LIGHT, OUTPUT);
	pinMode(RELE_COLD, OUTPUT);
	pinMode(RELE_HOT, OUTPUT);
	pinMode(RELE_HIDRO, OUTPUT);

	pinMode(RELE_PUMP, OUTPUT);

	// установка пинов DC-motor Arduino на "вывод":
	pinMode(DC_MOTOR_1_A, OUTPUT);
	pinMode(DC_MOTOR_1_B, OUTPUT);

	// установка пинов StepMotor Arduino на "вывод":
	pinMode(MOTOR_PIN_1, OUTPUT);
	pinMode(MOTOR_PIN_2, OUTPUT);
	pinMode(MOTOR_PIN_3, OUTPUT);
	pinMode(MOTOR_PIN_4, OUTPUT);
	pinMode(MOTOR_PIN_5, OUTPUT);
	pinMode(MOTOR_PIN_6, OUTPUT);

	digitalWrite(MOTOR_PIN_5, LOW);
	digitalWrite(MOTOR_PIN_6, LOW);
	digitalWrite(MOTOR_PIN_1, LOW);
	digitalWrite(MOTOR_PIN_2, LOW);
	digitalWrite(MOTOR_PIN_3, LOW);
	digitalWrite(MOTOR_PIN_4, LOW);

	//установка функций обработки прерываний на внешнее прерывание по S1, S2, S3, S4
	pinMode(BUTTON_PIN_1_, INPUT_PULLUP);
	pinMode(BUTTON_PIN_2_, INPUT_PULLUP);
	pinMode(BUTTON_PIN_3_, INPUT_PULLUP);
	pinMode(BUTTON_PIN_4_, INPUT_PULLUP);
	
	cli();			 //вЫкючаем прерывания
	EIFR |= (1 << INTF2);	// (установка флагов срабатывания в "0" при первом объявлении)
	EIFR |= (1 << INTF3);
	EIFR |= (1 << INTF4);
	EIFR |= (1 << INTF5);
	sei();			 //вКлючаем прерывания
	
	attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_1_), position_12_1_stop, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_2_), position_12_2_stop, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_3_), position_19_1_stop, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_4_), position_19_9_stop, FALLING);

	


	
	
	

	delay(2000);
	wdt_reset();	 //сброс WatchDog таймера	
	
	read_DS18b20();		//запрос температуры с датчиков DS18b20
	
	
	//изменение температуры не более чем на 2°С в час (Начальное значение )
	level_Temp_dynamic = tempC1;

	last_time_sec_11 = millis();


	//подстраивание Начального шага регулирования под текущую темперануру
	for (int i = 0; i < 35; i++) {
		
		if (((array_step_Temp[i] * 100) < tempC1) && ((array_step_Temp[i + 1] * 100) > tempC1)){
			step_regulate_Temp = i;
			i = 35;
		}		
	}


	if (!RTC.read(tm)) {													 // проверка часов RTC на ошибки
		if (RTC.chipPresent()) {

			// индикация ошибок
			error_array_N[error_RCT] = 1;
			error();			 // световая сигнализация
		}
		else {

			// индикация ошибок
			error_array_N[error_RCT] = 1;
			error();			 // световая сигнализация
		}
		
		tm.Year 	= 1;
		tm.Month 	= 1;
		tm.Day 		= 1;
		tm.Hour 	= 1;
		tm.Minute 	= 1;
		tm.Second 	= 1;
		
		
	}	//if (RTC.read(tm))

	last_time_hour_3 = tm.Hour + 1;
		
		
		
		
	pinMode(SD_CHIP_SELECT_PIN, OUTPUT);
	
	if (!sd.begin(SD_CHIP_SELECT_PIN, SPI_HALF_SPEED)) {			  // проверка на ошибки SD-card


		int i_sd = 0;
		while (!sd.begin(SD_CHIP_SELECT_PIN, SPI_HALF_SPEED)){
			i_sd++;
			delay(100);

			if (i_sd > 5) {  //при превышении числа ошибок
			
				// индикация ошибок
				error_SD();
				return;
			}
		}
	}


	last_SD_hour = tm.Hour;										// для разбиения файлов через отведенное время

	
		
	post_reboot_system();			  //восстановление параметров системы после перезагрузки


	delay(300);

	
	calibration_motor();				// калибровка моторов и концевиков

	
	

	//запуск вызова функций регулирования по асинхронному таймеру
	THandle_DHT21			= TimerList.Add(read_DHT_in_task, 5500);					//запуск опроса DHT21		1 раз в ~5 сек.
	THandle_DC_motor_task	= TimerList.Add(calibration_DC_motor_in_task, 903500);		//запуск калибровки			1 раз в ~15 мин.
	THandle_Step_motor_task	= TimerList.Add(calibration_Step_motor_in_task, 905500);	//запуск калибровки			1 раз в ~15 мин.
	THandle_SHT1x			= TimerList.Add(read_SHT1x_in_task, 5000);					//запуск опроса SHT1x		1 раз в 5 сек.
	THandle_MHZ19			= TimerList.Add(read_MHZ19_in_task, 6100);					//запуск опроса MH-Z19		1 раз в ~6 сек.
	THandle_DS18b20			= TimerList.Add(read_DS18b20_in_task, 3000);				//запуск опроса DS18b20		1 раз в 3 сек.
	THandle_RCT				= TimerList.Add(read_RCT_in_task, 500);						//запуск опроса RCT			2 раз в 1 сек.
	THandle_CO2				= TimerList.Add(regulate_CO2_in_task, 4800);				//запуск регулирования CO2	1 раз в ~5 сек.
	THandle_hidro			= TimerList.Add(regulate_hidro_in_task, 4700);				//запуск hidro				1 раз в ~5 сек.
	THandle_Temp			= TimerList.Add(regulate_Temp_in_task, 300000);				//запуск Temp				1 раз в 5 мин.
	THandle_Hot_Cold		= TimerList.Add(regulate_Hot_Cold_in_task, 2850);			//запуск Hot_Cold			1 раз в ~3 сек.
	THandle_water			= TimerList.Add(regulate_water_in_task, 300);				//запуск water				3 раза в ~1 сек.
	THandle_light			= TimerList.Add(regulate_light_in_task, 610000);			//запуск light				1 раз в ~10 мин.
	THandle_SDcard			= TimerList.Add(write_file_in_task, 60000);					//запуск записи на SDcard	1 раз в 1 мин.
	
	
	
	
	delay(300);

	wdt_reset();	 //сброс WatchDog таймера
	
	write_file();

}



//-------------------------------------------------	   LOOP
void loop() {
	
	power_down();				// отключение питания при критических ошибках

	read_pin_door();			// проверка состояния двери холодильника (Открыта/Закрыта)


	if (door_position == DOOR_OPEN) {

	//регулирование работы морозильника и нагревателя

		steps_left = 0;								//останов текущего движения StepMotor
		StepMotor_Go(9);							//открытие заслонки в режим максимального проветривания
		
		
		if (tempC4 > 2000) {
			digitalWrite(RELE_COLD, HIGH);			 //вКлючение морозильника
			digitalWrite(RELE_HOT, LOW);			 //отключение нагревателя
		}
		else if (tempC4 < 500) {
			digitalWrite(RELE_COLD, LOW);			 //отключение морозильника
			digitalWrite(RELE_HOT, HIGH);			 //вКлючение нагревателя
		} else {
			digitalWrite(RELE_COLD, LOW);			 //отключение морозильника
			digitalWrite(RELE_HOT, LOW);			 //отключение нагревателя			
		}

		
		digitalWrite(RELE_CO2, HIGH);
		DC_Motor_Go(open_CO2);
		
		digitalWrite(RELE_LIGHT, HIGH);
		
		
		digitalWrite(RELE_PUMP, LOW);
		digitalWrite(RELE_HIDRO, LOW);


		digitalWrite(LED_GREEN, HIGH);
		digitalWrite(LED_RED, HIGH);

		
		task_manager();				//запуск очередной задачи из очереди
		
		wdt_reset();	 //сброс WatchDog таймера
		
		delay(10);		//просто замедление общего цикла при открытой двери
			
	
	}


	
	if (door_position == DOOR_PRE_OPEN) {

	//регулирование работы морозильника и нагревателя


		
		digitalWrite(RELE_LIGHT, HIGH);
		

		digitalWrite(LED_GREEN, HIGH);

		
		task_manager();				//запуск очередной задачи из очереди
		
		wdt_reset();	 //сброс WatchDog таймера
		
	
	}
	
	

	if (door_position == DOOR_CLOSE) {



	
		if (door_position_one == DOOR_CLOSE_ONE) {				//в момент закрытия дверцы

			digitalWrite(LED_GREEN, LOW);
			digitalWrite(LED_RED, LOW);
	
		
			// вычисление текущего дня цикла и параметров на день
			for (int i = 0; i < CYCLE_ARRAY_LENGTH; i++) {
				if ((cycle_array_day[i] == 1) && (cycle_array_day[i + 1] == 0)) {
					day_cycle = i+1;
					i = CYCLE_ARRAY_LENGTH;
				}
			}

			level_Temp	= cycle_array_temp[day_cycle - 1];		// температура в камере на текущий день
			level_CO2	= cycle_array_CO2[day_cycle - 1];		// уровень СО2 в камере на текущий день
			level_hidro = cycle_array_hidro[day_cycle - 1];		// влажность в камере на текущий день

			door_position_one = 2;						// сброс момента закрытия двери
			
			DC_Motor_Go(close_CO2);						//закрытие притока
			digitalWrite(RELE_COLD, LOW);				//отключение морозильника
			digitalWrite(RELE_HOT, LOW);				//отключение нагревателя			
			digitalWrite(RELE_CO2, LOW);				//отключение вентилятора СО2
			digitalWrite(RELE_LIGHT, LOW);				//выключение света
			digitalWrite(RELE_HIDRO, LOW);				//отключение УЗУ
			digitalWrite(LED_GREEN, LOW);				//отключение зеленого индикатора
			
			steps_left = 0;								//останов текущего движения StepMotor
			//возвращаемся в исходное состояние
			StepMotor_Go(array_step_piosition[step_regulate_Temp]);

		}


		
		unsigned long remaining_time = 0;					// остаток времени при переполнении millis()
		
		if (millis() < last_time_hour_1) {					// при переполнении millis()
			remaining_time = ULONG_MAX - last_time_hour_1;
			last_time_hour_1 = 0;	//ULONG_MAX - максимальное значение переменной типа unsigned long
		}

		if ((millis() - last_time_hour_1 + remaining_time) > (24L*60L*60L*1000L)) {			  //корректировка каждые 24 часа

			// установка следующего дня цикла	 (1 - первый день)
			for (int i = 1; i < CYCLE_ARRAY_LENGTH; i++) {
				if ((cycle_array_day[i - 1] == 1) && (cycle_array_day[i] == 0)) {
					cycle_array_day[i] = 1;
					day_cycle = i;
					i = CYCLE_ARRAY_LENGTH;
				}

			}

			level_Temp	= cycle_array_temp[day_cycle - 1];		// температура в камере на текущий день
			level_CO2	= cycle_array_CO2[day_cycle - 1];		// уровень СО2 в камере на текущий день
			level_hidro = cycle_array_hidro[day_cycle - 1];		// влажность в камере на текущий день

			//калибровка положения StepMotor и DC-Motor
			calibration_motor();			
		
			last_time_hour_1 = millis();   //запуск нового цикла 24 часа
		}


	
		task_manager();				//запуск очередной задачи из очереди
				
		flash_green();				// индикатор работы (ставить в конце LOOP):

		reboot_system();			//перезагрузка системы при зависании датчиков

		wdt_reset();	 			//сброс WatchDog таймера		
		
	}





} //void loop()







//*****************************************-----------ФУНКЦИИ-----------*******



//--------------------------------------------------------------------------------------
// менеджер очереди задач
void task_manager(){

	int summ_array_flags = 0;		//сумма всех элементов очереди задач
	int index_zero = 0;				//индекс первого с начала массива нулевого элемента


	cli();
	
	//проверка наличия ненулевых элементов в массиве:
	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {				
		summ_array_flags = summ_array_flags + array_flags[i];
	}

	if (summ_array_flags !=0){
		for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {	
			if ((array_flags[i] == 0) && (index_zero == 0)){
				index_zero = i + 1;								//чтобы при i = 0   index_zero  не был равен 0
			}	
			if ((array_flags[i] != 0) && (index_zero > 0)) {
				array_flags[index_zero - 1] = array_flags[i];
				array_flags[i] = 0;
				index_zero++;
			}
		}		
	}

	sei();

	
	
	//исполнение очередного элемента из очереди задач:
	if (array_flags[0] != 0){

		switch (array_flags[0]) {
			case READ_DHT_FLAG:	 //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				read_DHT();

				break;
			case CALIBRATION_DC_MOTOR_FLAG:  //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				calibration_DC_motor();

				break;				
			case CALIBRATION_STEP_MOTOR_FLAG:  //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				calibration_Step_motor();

				break;
			case READ_SHT1X_FLAG:  //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				read_SHT1x();

				break;
			case READ_SHT1X_1_FLAG:	 //
				cli();			 //вЫкючаем прерывания
				TimerList.TimerStop(THandle_SHT1x_1_task);	   //останов таймера
				TimerList.Delete(THandle_SHT1x_1_task);		   //удаление таймера
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				read_SHT1x_1();

				break;
			case READ_SHT1X_2_FLAG:	 //
				cli();			 //вЫкючаем прерывания
				TimerList.TimerStop(THandle_SHT1x_2_task);	   //останов таймера
				TimerList.Delete(THandle_SHT1x_2_task);		   //удаление таймера
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				read_SHT1x_2();

				break;
			case READ_MHZ19_FLAG:  //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				read_MHZ19();

				break;
			case READ_DS18B20_FLAG:	 //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				read_DS18b20();

				break;
			case READ_RCT_FLAG:	 //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				read_RCT();

				break;
			case REGULATE_CO2_FLAG:	 //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				regulate_CO2();

				break;
			case REGULATE_HIDRO_FLAG:  //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				regulate_hidro();

				break;
			case REGULATE_TEMP_FLAG:  //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				regulate_Temp();

				break;
			case REGULATE_HOT_COLD_FLAG:  //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				regulate_Hot_Cold();

				break;
			case REGULATE_WATER_FLAG:  //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				regulate_water();

				break;
			case REGULATE_LIGHT_FLAG:  //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				regulate_light();

				break;
			case WRITE_FILE_FLAG:  //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				write_file();

				break;
			case 17:  //
				cli();			 //вЫкючаем прерывания
				array_flags[0] = 0;
				
				//сдвиг очереди задач
				for (int i = 0; i < (ARRAY_FLAGS_LENGH - 1); i++) {
					if ((array_flags[i] == 0) && (!(array_flags[i + 1] == 0))) {
						array_flags[i] = array_flags[i + 1];
						array_flags[i + 1] = 0;
					} else {
						break;
					}
				}				
				
				sei();			 //вКлючаем прерывания

				power_down();

				break;
		}
	} else {
		return;
	}





	
	if (error_array_N[error_task] == 1){  //при переполнении очереди задач в прерываниях
		error();
	}
	

}



//функции постановки задач в очередь
void read_DHT_in_task(){

	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

		if (array_flags[i] == READ_DHT_FLAG) {
			return;
		}
		if (array_flags[i] == 0) {
			array_flags[i] = READ_DHT_FLAG;
			error_array_N[error_task] = 0;
			return;
		}
		if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == READ_DHT_FLAG)){	//при переполнении очереди задач
			error_array_N[error_task] = 1;
			return;
		}
	}
}
  
void calibration_DC_motor_in_task(){

	if (error_array_N[error_DCmotor] == 1) {
		
		for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

			if (array_flags[i] == CALIBRATION_DC_MOTOR_FLAG) {
				return;
			}
			if (array_flags[i] == 0) {
				array_flags[i] = CALIBRATION_DC_MOTOR_FLAG;
				error_array_N[error_task] = 0;
				return;
			}
			if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == CALIBRATION_DC_MOTOR_FLAG)){	//при переполнении очереди задач
				error_array_N[error_task] = 1;
				return;
			}
		}
	}
}

void calibration_Step_motor_in_task(){

	if (error_array_N[error_StepMotor] == 1){
		
		for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

			if (array_flags[i] == CALIBRATION_STEP_MOTOR_FLAG) {
				return;
			}
			if (array_flags[i] == 0) {
				array_flags[i] = CALIBRATION_STEP_MOTOR_FLAG;
				error_array_N[error_task] = 0;
				return;
			}
			if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == CALIBRATION_STEP_MOTOR_FLAG)){	//при переполнении очереди задач
				error_array_N[error_task] = 1;
				return;
			}
		}
	}
}

void read_SHT1x_in_task(){

	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

		if (array_flags[i] == READ_SHT1X_FLAG) {
			return;
		}
		if (array_flags[i] == 0) {
			array_flags[i] = READ_SHT1X_FLAG;
			error_array_N[error_task] = 0;
			return;
		}
		if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == READ_SHT1X_FLAG)){  //при переполнении очереди задач
			error_array_N[error_task] = 1;
			return;
		}
	}
}

void read_SHT1x_1_in_task(){

	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

		if (array_flags[i] == READ_SHT1X_1_FLAG) {
			return;
		}
		if (array_flags[i] == 0) {
			array_flags[i] = READ_SHT1X_1_FLAG;
			error_array_N[error_task] = 0;
			return;
		}
		if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == READ_SHT1X_1_FLAG)){	//при переполнении очереди задач
			error_array_N[error_task] = 1;
			return;
		}
	}
}

void read_SHT1x_2_in_task(){

	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

		if (array_flags[i] == READ_SHT1X_2_FLAG) {
			return;
		}
		if (array_flags[i] == 0) {
			array_flags[i] = READ_SHT1X_2_FLAG;
			error_array_N[error_task] = 0;
			return;
		}
		if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == READ_SHT1X_2_FLAG)){	//при переполнении очереди задач
			error_array_N[error_task] = 1;
			return;
		}
	}
}

void read_MHZ19_in_task(){

	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

		if (array_flags[i] == READ_MHZ19_FLAG) {
			return;
		}
		if (array_flags[i] == 0) {
			array_flags[i] = READ_MHZ19_FLAG;
			error_array_N[error_task] = 0;
			return;
		}
		if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == READ_MHZ19_FLAG)){  //при переполнении очереди задач
			error_array_N[error_task] = 1;
			return;
		}
	}
}

void read_DS18b20_in_task(){

	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

		if (array_flags[i] == READ_DS18B20_FLAG) {
			return;
		}
		if (array_flags[i] == 0) {
			array_flags[i] = READ_DS18B20_FLAG;
			error_array_N[error_task] = 0;
			return;
		}
		if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == READ_DS18B20_FLAG)){	//при переполнении очереди задач
			error_array_N[error_task] = 1;
			return;
		}
	}
}

void read_RCT_in_task(){

	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

		if (array_flags[i] == READ_RCT_FLAG) {
			return;
		}
		if (array_flags[i] == 0) {
			array_flags[i] = READ_RCT_FLAG;
			error_array_N[error_task] = 0;
			return;
		}
		if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == READ_RCT_FLAG)){	//при переполнении очереди задач
			error_array_N[error_task] = 1;
			return;
		}
	}
}

void regulate_CO2_in_task(){

	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

		if (array_flags[i] == REGULATE_CO2_FLAG) {
			return;
		}
		if (array_flags[i] == 0) {
			array_flags[i] = REGULATE_CO2_FLAG;
			error_array_N[error_task] = 0;
			return;
		}
		if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == REGULATE_CO2_FLAG)){	//при переполнении очереди задач
			error_array_N[error_task] = 1;
			return;
		}
	}
}

void regulate_hidro_in_task(){

	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

		if (array_flags[i] == REGULATE_HIDRO_FLAG) {
			return;
		}
		if (array_flags[i] == 0) {
			array_flags[i] = REGULATE_HIDRO_FLAG;
			error_array_N[error_task] = 0;
			return;
		}
		if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == REGULATE_HIDRO_FLAG)){  //при переполнении очереди задач
			error_array_N[error_task] = 1;
			return;
		}
	}
}

void regulate_Temp_in_task(){

	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

		if (array_flags[i] == REGULATE_TEMP_FLAG) {
			return;
		}
		if (array_flags[i] == 0) {
			array_flags[i] = REGULATE_TEMP_FLAG;
			error_array_N[error_task] = 0;
			return;
		}
		if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == REGULATE_TEMP_FLAG)){	 //при переполнении очереди задач
			error_array_N[error_task] = 1;
			return;
		}
	}
}

void regulate_Hot_Cold_in_task(){

	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

		if (array_flags[i] == REGULATE_HOT_COLD_FLAG) {
			return;
		}
		if (array_flags[i] == 0) {
			array_flags[i] = REGULATE_HOT_COLD_FLAG;
			error_array_N[error_task] = 0;
			return;
		}
		if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == REGULATE_HOT_COLD_FLAG)){	 //при переполнении очереди задач
			error_array_N[error_task] = 1;
			return;
		}
	}
}

void regulate_water_in_task(){

	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

		if (array_flags[i] == REGULATE_WATER_FLAG) {
			return;
		}
		if (array_flags[i] == 0) {
			array_flags[i] = REGULATE_WATER_FLAG;
			error_array_N[error_task] = 0;
			return;
		}
		if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == REGULATE_WATER_FLAG)){  //при переполнении очереди задач
			error_array_N[error_task] = 1;
			return;
		}
	}
}

void regulate_light_in_task(){

	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

		if (array_flags[i] == REGULATE_LIGHT_FLAG) {
			return;
		}
		if (array_flags[i] == 0) {
			array_flags[i] = REGULATE_LIGHT_FLAG;
			error_array_N[error_task] = 0;
			return;
		}
		if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == REGULATE_LIGHT_FLAG)){  //при переполнении очереди задач
			error_array_N[error_task] = 1;
			return;
		}
	}
}

void write_file_in_task(){

	for (int i = 0; i < ARRAY_FLAGS_LENGH; i++) {

		if (array_flags[i] == WRITE_FILE_FLAG) {
			return;
		}
		if (array_flags[i] == 0) {
			array_flags[i] = WRITE_FILE_FLAG;
			error_array_N[error_task] = 0;
			return;
		}
		if ((i == (ARRAY_FLAGS_LENGH - 1)) && !(array_flags[ARRAY_FLAGS_LENGH - 1] == WRITE_FILE_FLAG)){  //при переполнении очереди задач
			error_array_N[error_task] = 1;
			return;
		}
	}
}



//---------------------------------------------------------------------------------------

void calibration_motor(){

	for (int e1 = 0; e1 < 11; e1++){							 // мигание 
		digitalWrite(LED_RED, ((e1+1) % 2));
		digitalWrite(LED_GREEN, (e1 % 2));
		delay(300);
		wdt_reset();	 //сброс WatchDog таймера
	}

	digitalWrite(LED_RED, LOW);
	digitalWrite(LED_GREEN, LOW);
	
	
//калибровка DC_motor:
	calibration_DC_motor_step = 101;

	
	while (calibration_DC_motor_step > 0){
		error_array_N[error_DCmotor] = 1;
		calibration_DC_motor();
		delay(5);
	}

	flag_calibration_DC_motor = 0;
	
	delay(1500);
	
	
	for (int e1 = 0; e1 < 11; e1++){							 // мигание 
		digitalWrite(LED_RED, ((e1+1) % 2));
		digitalWrite(LED_GREEN, (e1 % 2));
		delay(300);
		wdt_reset();	 //сброс WatchDog таймера
	}

	digitalWrite(LED_RED, LOW);
	digitalWrite(LED_GREEN, LOW);
	
	
	
	
	
//калибровка Step_motor:	
	calibration_StepMotor_step = 101;
	
	while (calibration_StepMotor_step > 0){
		error_array_N[error_StepMotor] = 1;
		calibration_Step_motor();
		delay(5);
	}

	delay(1500);
	
	for (int e1 = 0; e1 < 11; e1++){							 // мигание 
		digitalWrite(LED_RED, ((e1+1) % 2));
		digitalWrite(LED_GREEN, (e1 % 2));
		delay(300);
		wdt_reset();	 //сброс WatchDog таймера
	}

	digitalWrite(LED_RED, LOW);
	digitalWrite(LED_GREEN, LOW);	
	
	
	
}

void calibration_DC_motor(){							//калибровка DC-мотора и концевиков

	if (error_array_N[error_DCmotor] == 1){
			
		error_array_N[error_DCmotor] = 0;
		
		
		wdt_reset();	 //сброс WatchDog таймера

		
		if (calibration_DC_motor_step == 0) {		//перевод в режим калибровки
			calibration_DC_motor_step = 1;
		}
		
		if (calibration_DC_motor_step >= 100) {		//в период калибровки не выполнять другие задания
			calibration_DC_motor_step = calibration_DC_motor_step - 100L;
		}

////1		
		if (calibration_DC_motor_step == 1) {
			
			//отключение на время калибровки
			digitalWrite(RELE_COLD, LOW);				//отключение морозильника
			digitalWrite(RELE_HOT, LOW);				//отключение нагревателя
			digitalWrite(RELE_CO2, LOW);				//отключение вентилятора СО2
			
			//останов текущего движения DC-motor
			digitalWrite(DC_MOTOR_1_A, LOW);
			digitalWrite(DC_MOTOR_1_B, LOW);

			read_Button();		

			
			position_colibration_DC = 111;
			
			if (this_position_12_end == 2) {	   //если заслонка между концевиками
				DC_Motor_Go(open_CO2);
				position_colibration_DC = 1;
			}

			if (this_position_12_end == close_CO2) {	// если заслонка на одном из концевиков
				DC_Motor_Go(open_CO2);
				position_colibration_DC = 2;
			} 

			if (this_position_12_end == open_CO2) {	// если заслонка на одном из концевиков
				DC_Motor_Go(close_CO2);
				position_colibration_DC = 3;
			}

		
			calibration_DC_motor_step = 102;	//в период калибровки не выполнять другие задания
			last_time_sec_12 = millis();		//для следующего шага


			if (flag_calibration_DC_motor != 1) {	//если не запущена начальная калибровка
			
				TimerList.Delete(THandle_DC_motor_task);		   //удаление таймера
				THandle_DC_motor_task	= TimerList.Add(calibration_DC_motor_in_task, 50);		//запуск функции через каждые 50 мсек.
			}
			
					
			return;
			
		}
		
		
		if (last_time_sec_12 > millis()) {   //при переполнении millis()
			last_time_sec_12 = 0;
		}
		
////2		
		if ((calibration_DC_motor_step == 2) && ((millis() - last_time_sec_12) > 2000)) {
			
			//отключение на время калибровки
			digitalWrite(RELE_COLD, LOW);				//отключение морозильника
			digitalWrite(RELE_HOT, LOW);				//отключение нагревателя
			digitalWrite(RELE_CO2, LOW);				//отключение вентилятора СО2
			
			read_Button();	
			
		
			if (position_colibration_DC == 1) {	   //если заслонка была между концевиками

				if ((digitalRead(DC_MOTOR_1_A) == HIGH) && (digitalRead(DC_MOTOR_1_B) == HIGH)) {	// проверка срабатывания концевика (если не сработал)
					error_calibration_DC_motor();
					return;
				}
				
				if (this_position_12_end != open_CO2) {

					digitalWrite(DC_MOTOR_1_A, LOW);
					digitalWrite(DC_MOTOR_1_B, LOW);
					
					direction_DC = !direction_DC;
					
					DC_Motor_Go(open_CO2);

				}

				
			}

			if (position_colibration_DC == 2) {	// если заслонка была close_CO2

				if ((digitalRead(DC_MOTOR_1_A) == HIGH) && (digitalRead(DC_MOTOR_1_B) == HIGH)) {	// проверка срабатывания концевика (если не сработал)
					error_calibration_DC_motor();
					return;
				}
				
				if (this_position_12_end == close_CO2) {
					digitalWrite(DC_MOTOR_1_A, LOW);
					digitalWrite(DC_MOTOR_1_B, LOW);

					direction_DC = !direction_DC;	  //меняем полярность	
					
					DC_Motor_Go(open_CO2);					
				}
				
				
			} 

			if (position_colibration_DC == 3) {	// если заслонка была open_CO2

				if ((digitalRead(DC_MOTOR_1_A) == HIGH) && (digitalRead(DC_MOTOR_1_B) == HIGH)) {	// проверка срабатывания концевика (если не сработал)
					error_calibration_DC_motor();
					return;
				}
				
				if (this_position_12_end == open_CO2) {
					digitalWrite(DC_MOTOR_1_A, LOW);
					digitalWrite(DC_MOTOR_1_B, LOW);

					direction_DC = !direction_DC;	  //меняем полярность	
					
					DC_Motor_Go(close_CO2);

				}

			
			}

		
			calibration_DC_motor_step = 103;	//в период калибровки не выполнять другие задания
			last_time_sec_12 = millis();		//для следующего шага
			
			return;
		}	
		
////3		
		if ((calibration_DC_motor_step == 3) && ((millis() - last_time_sec_12) > 2000)) {
			
			//отключение на время калибровки
			digitalWrite(RELE_COLD, LOW);				//отключение морозильника
			digitalWrite(RELE_HOT, LOW);				//отключение нагревателя
			digitalWrite(RELE_CO2, LOW);				//отключение вентилятора СО2
			
			read_Button();
			
		
			if (position_colibration_DC == 1) {	   //если заслонка была между концевиками

				if (this_position_12_end != open_CO2) {

					error_calibration_DC_motor();
					return;

				}		
				
				DC_Motor_Go(close_CO2);
				
			}

			if (position_colibration_DC == 2) {		// если заслонка была close_CO2
			
				if (this_position_12_end != open_CO2) {
					error_calibration_DC_motor();
					return;					
					
				}
			
				DC_Motor_Go(close_CO2);
				
			} 

			if (position_colibration_DC == 3) {		// если заслонка была open_CO2
				
				if (this_position_12_end != close_CO2) {
					error_calibration_DC_motor();
					return;
					
					
				}		

				DC_Motor_Go(open_CO2);
				
			}
			
			
			delay(750);
			
			digitalWrite(DC_MOTOR_1_A, LOW);
			digitalWrite(DC_MOTOR_1_B, LOW);
			
			delay(500);
			
			//возвращаемся в положение до калибровки

			DC_Motor_Go(close_CO2);


			calibration_DC_motor_step = 0;		//окончание калибровки
			
			if (error_array_N[error_DCmotor] == 1) {
				error_array_N[error_DCmotor] = 0;
			
				if (flag_calibration_DC_motor != 1) {
					error();
				}			
			}


			
			TimerList.Delete(THandle_DC_motor_task);
			THandle_DC_motor_task	= TimerList.Add(calibration_DC_motor_in_task, 903500);		//запуск калибровки	1 раз в ~15 мин.
			
			return;
		}	
		
		

	}
	
}

void calibration_Step_motor(){							//калибровка шагового мотора и концевиков

	if (error_array_N[error_StepMotor] == 1){
			
		wdt_reset();	 //сброс WatchDog таймера
		
		if (calibration_StepMotor_step == 0) {		//перевод в режим калибровки
			calibration_StepMotor_step = 1;
		}
		
		if (calibration_StepMotor_step >= 100) {		//в период калибровки не выполнять другие задания
			calibration_StepMotor_step = calibration_StepMotor_step - 100L;
		}

////1	
		if (calibration_StepMotor_step == 1) {
			
			//останов текущего движения StepMotor
			steps_left = 0;

			read_Button();		

			
			position_colibration_SM = 111;
			
			if (this_position_19_end == 0) {	//если заслонка между концевиками

				this_position_19 = 1;			// установка максимально возможной текущей позиции для калибровки

				StepMotor_Go(9);	  			// перевод StepMotor в крайнее положение (калибровка)
				
				position_colibration_SM = 1;
			}

			if (this_position_19_end == 1) {	// если заслонка в позиции 1
				StepMotor_Go(9);
				position_colibration_SM = 2;
			} 

			if (this_position_19_end == 9) {	// если заслонка в позиции 9
				StepMotor_Go(1);
				position_colibration_SM = 3;
			}

		
			calibration_StepMotor_step = 102;	//в период калибровки не выполнять другие задания со StepMotor
			last_time_sec_13 = millis();		//для следующего шага
			
			TimerList.Delete(THandle_Step_motor_task);
			THandle_Step_motor_task	= TimerList.Add(calibration_Step_motor_in_task, 50);		//запуск функции через каждые 50 мсек.
			
					
			return;
			
		}
		
		
		if (last_time_sec_13 > millis()) {   //при переполнении millis()
			last_time_sec_13 = 0;
		}
		
////2		
		if ((calibration_StepMotor_step == 2) && ((millis() - last_time_sec_13) > 3000)) {
			
			read_Button();	
			
		
			if (position_colibration_SM == 1) {	   //если заслонка была между концевиками

			}

			if (position_colibration_SM == 2) {	// если заслонка была в позиции 1
			
				if (this_position_19_end == 1) {
					//останов текущего движения StepMotor
					steps_left = 0;
					
					pre_direction = -pre_direction;		  //меняем полярность обмоток StepMotor	
					
					StepMotor_Go(9);
					
				}
				
			
			} 

			if (position_colibration_SM == 3) {	// если заслонка была в позиции 9
				
				if (this_position_19_end == 9) {
					//останов текущего движения StepMotor
					steps_left = 0;
					
					pre_direction = -pre_direction;		  //меняем полярность обмоток StepMotor
					
					StepMotor_Go(1);
				}

			
			}

		
			calibration_StepMotor_step = 103;	//в период калибровки не выполнять другие задания со StepMotor
			
			return;
		}	
		
////3		
		if ((calibration_StepMotor_step == 3) && (steps_left == 0)) {
			
			read_Button();	
			
		
			if (position_colibration_SM == 1) {	   //если заслонка была между концевиками

				//если заслонка двигалась в обратном направлении
				if (this_position_19_end == 1){

					pre_direction = -pre_direction;	   //меняем полярность обмоток StepMotor

					StepMotor_Go(9);
				}			
				
				//если заслонка двигалась в правильном направлении
				if (this_position_19_end == 9){

					StepMotor_Go(1);
				}
				
					
				
			}

			if (position_colibration_SM == 2) {	// если заслонка была в позиции 1
			
				if (this_position_19_end == 1) {

					error_calibration_Step_motor();
					return;
					
					
				}
			} 

			if (position_colibration_SM == 3) {	// если заслонка была в позиции 9
				
				if (this_position_19_end == 9) {

					error_calibration_Step_motor();	
					return;
					
					
				}		
			}
			

			if ((this_position_19_end == 10) || (this_position_19_end == 0)) {

				error_calibration_Step_motor();
				return;
			}

			calibration_StepMotor_step = 104;	//в период калибровки не выполнять другие задания со StepMotor
			
			return;


		}	
		
////4

		if ((calibration_StepMotor_step == 4) && (steps_left == 0)) {
			
			read_Button();	
			
		
			if (position_colibration_SM == 1) {	   //если заслонка была между концевиками

				//если заслонка не дошла до позиции
				if (this_position_19_end != 1){
					
					error_calibration_Step_motor();
					return;
				}
				
					
				
			}

			if (position_colibration_SM == 2) {	// если заслонка была в позиции 1
			
				//если заслонка не дошла до позиции
				if (this_position_19_end != 9){
					
					error_calibration_Step_motor();
					return;
				}
			} 

			if (position_colibration_SM == 3) {	// если заслонка была в позиции 9
				
				//если заслонка не дошла до позиции
				if (this_position_19_end != 1){
					
					error_calibration_Step_motor();
					return;
				}		
			}
			

			if ((this_position_19_end == 10) || (this_position_19_end == 0)) {

				error_calibration_Step_motor();
				return;
			}


		
			calibration_StepMotor_step = 0;		//окончание калибровки
			error_array_N[error_StepMotor] = 0;
			
			//возвращаемся в исходное состояние
			StepMotor_Go(array_step_piosition[step_regulate_Temp]);
			
			
			
			TimerList.Delete(THandle_Step_motor_task);
			THandle_Step_motor_task	= TimerList.Add(calibration_Step_motor_in_task, 903500);		//запуск калибровки	1 раз в ~15 мин.
			
			return;


		}


	}
	
}

void error_calibration_DC_motor(){						//обработчик ошибок при калибровке DC-motor

	digitalWrite(DC_MOTOR_1_A, LOW);
	digitalWrite(DC_MOTOR_1_B, LOW);
	
	calibration_DC_motor_step = 0;
	
	TimerList.Delete(THandle_DC_motor_task);
	THandle_DC_motor_task	= TimerList.Add(calibration_DC_motor_in_task, 903500);		//запуск калибровки	1 раз в ~15 мин.
	
	// индикация ошибок
	error_array_N[error_DCmotor] = 1;
	error();			 // световая сигнализация
	
}

void error_calibration_Step_motor(){					//обработчик ошибок при калибровке Step-motor
	
	TCCR1B = 0;
	TCCR1A = 0;
	TIMSK1 = 0;
	TCNT1 = 0;	   //сброс счетчика шагов
	
	steps_left = 0;
	
	calibration_StepMotor_step = 0;
	
	TimerList.Delete(THandle_Step_motor_task);
	THandle_Step_motor_task	= TimerList.Add(calibration_Step_motor_in_task, 903500);		//запуск калибровки	1 раз в ~15 мин.
	
	// индикация ошибок
	error_array_N[error_StepMotor] = 1;
	error();			 // световая сигнализация
	
	
}


//--------------------------------------------------------------------------------------
void DC_Motor_Go(int position_12) {					//установка DC_Motor в требуемое положение


	
	if (calibration_DC_motor_step == 0) {			//когда калибровка не выполняется

		if (error_array_N[error_DCmotor] == 1) {		// проверка на наличие ошибок в работе DC-motor
			return;
		}
		
		if (!((position_12 == 0) || (position_12 == 1))) {		//при неправильном задании положения

			// индикация ошибок при неправильном
			//error_array_N[error_DCmotor] = 1;
			//error();			 // световая сигнализация
			
			return;

		}	

		if ( position_12 == DCmotor_last_position) {		// если Новая и Последняя заданные позиции совпадают
			return;
		}
		else {
			//останов текущего движения DC-motor
			digitalWrite(DC_MOTOR_1_A, LOW);
			digitalWrite(DC_MOTOR_1_B, LOW);
		}

		read_Button();							   // проверка концевых выключателей


		if (this_position_12_end == position_12) {	 // выход из функции если заданная и текущая позиции совпадают
			return;
		}


		DCmotor_last_position = position_12;		// запоминание последней заданной позиции

		
		if (direction_DC == 0){

			switch (position_12) {
				case 0:
				digitalWrite(DC_MOTOR_1_A, HIGH);
				digitalWrite(DC_MOTOR_1_B, LOW);
				break;
				case 1:
				digitalWrite(DC_MOTOR_1_A, LOW);
				digitalWrite(DC_MOTOR_1_B, HIGH);
				break;
			}
		}

		if (direction_DC == 1){

			switch (position_12)	{
				case 0:
				digitalWrite(DC_MOTOR_1_A, LOW);
				digitalWrite(DC_MOTOR_1_B, HIGH);
				break;
				case 1:
				digitalWrite(DC_MOTOR_1_A, HIGH);
				digitalWrite(DC_MOTOR_1_B, LOW);
				break;
			}
		}

		
		//запуск аварийного таймера для DC_motor
		THandle_DC_motor_failure = TimerList.Add(DC_motor_failure, time_out_DCmotor_2);	//запуск проверки остановки DCmotor
	
		
		
	}

	else if ((calibration_DC_motor_step >= 1) && (calibration_DC_motor_step <= 3))	{	//если выполняется калибровка
		
		
		if (direction_DC == 0){

			switch (position_12) {
				case 0:
				digitalWrite(DC_MOTOR_1_A, HIGH);
				digitalWrite(DC_MOTOR_1_B, LOW);
				break;
				case 1:
				digitalWrite(DC_MOTOR_1_A, LOW);
				digitalWrite(DC_MOTOR_1_B, HIGH);
				break;
			}
		}

		if (direction_DC == 1){

			switch (position_12)	{
				case 0:
				digitalWrite(DC_MOTOR_1_A, LOW);
				digitalWrite(DC_MOTOR_1_B, HIGH);
				break;
				case 1:
				digitalWrite(DC_MOTOR_1_A, HIGH);
				digitalWrite(DC_MOTOR_1_B, LOW);
				break;
			}
		}

	}	
	
}


void DC_motor_failure(){							//аварийная остановка DC_motor при несрабатывании концевика дольше заданного времени
	
	TimerList.Delete(THandle_DC_motor_failure);		//останов таймера проверки остановки DCmotor
	
	digitalWrite(DC_MOTOR_1_A, LOW);
	digitalWrite(DC_MOTOR_1_B, LOW);
	
	
	// индикация ошибки
	error_array_N[error_DCmotor] = 1;
	error();			 // световая сигнализация
	
	

}



//-----------------------------------------------------
//функции хода stepMotor
void StepMotor_Go(int position_19_next) {			 //установка stepMotor в требуемое положение


	if (calibration_StepMotor_step == 0) {			//когда калибровка не выполняется
	
		position_19 = position_19_next;

		if (error_array_N[error_StepMotor] == 1) {		// проверка на наличие ошибок в работе StepMotor
			return;
		}

		if (steps_left > 0) {							//если процесс перевода заслонки запущен
			
			if ( position_19_next == StepMotor_last_position) {		// если Новая и Последняя заданные позиции совпадают
				return;
			}
			else {
				//останов текущего движения Step-motor
				steps_left = 0;
			}

		}

		read_Button();									// проверка концевых выключателей


		if (this_position_19_end == 1) {
			this_position_19 = 1;
		}

		if (this_position_19_end == 9) {
			this_position_19 = 9;
		}




		if (this_position_19 == position_19 || position_19 > 9 || position_19 < 1) {	   // выход из функции если заданная и текущая позиции совпадают
			return;
		}

		StepMotor_last_position = position_19_next;		// запоминание последней заданной позиции
		
		
		
		switch (this_position_19) {		//текущий угол *10 заслонки
			case 1:
			this_angle_10 = angle_1_10;
			break;
			case 2:
			this_angle_10 = angle_2_10;
			break;
			case 3:
			this_angle_10 = angle_3_10;
			break;
			case 4:
			this_angle_10 = angle_4_10;
			break;
			case 5:
			this_angle_10 = angle_5_10;
			break;
			case 6:
			this_angle_10 = angle_6_10;
			break;
			case 7:
			this_angle_10 = angle_7_10;
			break;
			case 8:
			this_angle_10 = angle_8_10;
			break;
			case 9:
			this_angle_10 = angle_9_10;
			break;
		}

		switch (position_19) {			 //требуемый угол *10 заслонки
			case 1:
			angle_10 = angle_1_10;
			break;
			case 2:
			angle_10 = angle_2_10;
			break;
			case 3:
			angle_10 = angle_3_10;
			break;
			case 4:
			angle_10 = angle_4_10;
			break;
			case 5:
			angle_10 = angle_5_10;
			break;
			case 6:
			angle_10 = angle_6_10;
			break;
			case 7:
			angle_10 = angle_7_10;
			break;
			case 8:
			angle_10 = angle_8_10;
			break;
			case 9:
			angle_10 = angle_9_10;
			break;
		}

		// разрешение на вращение StepMotor:
		digitalWrite(MOTOR_PIN_5, HIGH);	//E1 driver StepMotor
		digitalWrite(MOTOR_PIN_6, HIGH);	//E2 driver StepMotor


		whatSpeed = 30L;		   // !!_________________!!	 ЗАДАЙ требуемую скорость об/мин  StepMotor	 !!_________________!!
		// передаточное число редуктора 1:50

		grad_10 =  this_angle_10 - angle_10 ;   // количество   градусов * 10	для поворота StepMotor исходя из заданной позиции заслонки

		grad_10 = pre_direction * grad_10;		//предустановка направления вращения при калибровке
		
		// направление вращения:
		if (grad_10 > 0) {
			direction = 1;
		}
		if (grad_10 < 0) {
			direction = 0;
		}	

		grad_10 = abs(grad_10);		
		
		if(last_direction != direction){grad_10 = grad_10 + delta_angle_StepMotor;}			//компенсация гестерезиса заслонки
		
		last_direction = direction;
		
		if((position_19_next == 1) || (position_19_next == 9)){grad_10 = grad_10 + 300L;}	//для гарантии наезда на концевик
		
		abs_number_of_steps = grad_10 * 50L / 18L;	// вычисление количества шагов для заданного количества оборотов  (один шаг StepMotor - 1,8°) *10
		
		steps_left = abs_number_of_steps;					// сколько шагов нужно сделать (без учета направления)

		step_delay_default = 60L * 1000L * 1000L / stepsPerRevolution / whatSpeed;	//вычисление продолжительности одного шага, мкс (при 30 об/мин = 10000 мкс)




		// вычисление длительности шага №1 для плавного разгона и останова StepMotor:
		if ( abs_number_of_steps >= 300L ) {
			if (steps_left > (abs_number_of_steps - steps_acceleration) ) {
				step_delay = step_delay_default + (step_delay_default * (steps_acceleration - (abs_number_of_steps - steps_left)) / steps_acceleration);
			}
			else if (steps_left < steps_acceleration ) {
				step_delay = step_delay_default + (step_delay_default * (steps_acceleration - steps_left)/steps_acceleration) ;
			}
			else {
				step_delay = step_delay_default;
			}
		}
		else {
			step_delay = 60L * 1000L * 1000L / stepsPerRevolution / 10L;  //вычисление продолжительности одного шага (до 300 шагов), 20 об/мин	(при 20 об/мин = 15000 мкс)
		}


		//запоминаем текущее состояние концевиков

		if (digitalRead(BUTTON_PIN_3_) == LOW){
			val_19_1_go = 1;
		}
		if (digitalRead(BUTTON_PIN_4_) == LOW){
			val_19_9_go = 1;
		}


		// приращение или уменьшение шагов в зависимоти от направления вращения
		if (direction == 1){
			step_number = 0;
		}
		else {

			switch (abs_number_of_steps % 4) {     //чтобы первый шаг был нулевым
				case 3:	 //0
				step_number = abs_number_of_steps + 1;
				break;
				case 2:	 //-1
				step_number = abs_number_of_steps + 2;
				break;
				case 1:	 //-2
				step_number = abs_number_of_steps + 3;
				break;
				case 0:	 //-3
				step_number = abs_number_of_steps;
				break;
			}
		}


		
		



		
		stepMotor_Step(step_number);	// установка обмоток шагового двигателя под текущий шаг


		last_time_sec_4 = millis();


		//приращение шагов, движение на один шаг за раз:
		Timer1_init();
		
	}

	if ((calibration_StepMotor_step >= 1) && (calibration_StepMotor_step <= 4))	{	//если выполняется калибровка
	
		
		position_19 = position_19_next;

		StepMotor_last_position = position_19_next;		// запоминание последней заданной позиции
		
		
		
		switch (this_position_19) {		//текущий угол *10 заслонки
			case 1:
			this_angle_10 = angle_1_10;
			break;
			case 2:
			this_angle_10 = angle_2_10;
			break;
			case 3:
			this_angle_10 = angle_3_10;
			break;
			case 4:
			this_angle_10 = angle_4_10;
			break;
			case 5:
			this_angle_10 = angle_5_10;
			break;
			case 6:
			this_angle_10 = angle_6_10;
			break;
			case 7:
			this_angle_10 = angle_7_10;
			break;
			case 8:
			this_angle_10 = angle_8_10;
			break;
			case 9:
			this_angle_10 = angle_9_10;
			break;
		}

		switch (position_19) {			 //требуемый угол *10 заслонки
			case 1:
			angle_10 = angle_1_10;
			break;
			case 2:
			angle_10 = angle_2_10;
			break;
			case 3:
			angle_10 = angle_3_10;
			break;
			case 4:
			angle_10 = angle_4_10;
			break;
			case 5:
			angle_10 = angle_5_10;
			break;
			case 6:
			angle_10 = angle_6_10;
			break;
			case 7:
			angle_10 = angle_7_10;
			break;
			case 8:
			angle_10 = angle_8_10;
			break;
			case 9:
			angle_10 = angle_9_10;
			break;
		}

		// разрешение на вращение StepMotor:
		digitalWrite(MOTOR_PIN_5, HIGH);	//E1 driver StepMotor
		digitalWrite(MOTOR_PIN_6, HIGH);	//E2 driver StepMotor


		whatSpeed = 30L;		   // !!_________________!!	 ЗАДАЙ требуемую скорость об/мин  StepMotor	 !!_________________!!
		// передаточное число редуктора 1:50

		grad_10 =  this_angle_10 - angle_10 ;   // количество   градусов * 10	для поворота StepMotor исходя из заданной позиции заслонки

		grad_10 = pre_direction * grad_10;		//предустановка направления вращения при калибровке
		
		// направление вращения:
		if (grad_10 > 0) {
			direction = 1;
		}
		if (grad_10 < 0) {
			direction = 0;
		}	

		grad_10 = abs(grad_10);		
		
		if(last_direction != direction){grad_10 = grad_10 + delta_angle_StepMotor;}			//компенсация гестерезиса заслонки
		
		last_direction = direction;
		
		if((position_19_next == 1) || (position_19_next == 9)){grad_10 = grad_10 + 300L;}	//для гарантии наезда на концевик
		
		abs_number_of_steps = grad_10 * 50L / 18L;	// вычисление количества шагов для заданного количества оборотов  (один шаг StepMotor - 1,8°) *10
		
		steps_left = abs_number_of_steps;					// сколько шагов нужно сделать (без учета направления)

		step_delay_default = 60L * 1000L * 1000L / stepsPerRevolution / whatSpeed;	//вычисление продолжительности одного шага, мкс (при 30 об/мин = 10000 мкс)




		// вычисление длительности шага №1 для плавного разгона и останова StepMotor:
		if ( abs_number_of_steps >= 300L ) {
			if (steps_left > (abs_number_of_steps - steps_acceleration) ) {
				step_delay = step_delay_default + (step_delay_default * (steps_acceleration - (abs_number_of_steps - steps_left)) / steps_acceleration);
			}
			else if (steps_left < steps_acceleration ) {
				step_delay = step_delay_default + (step_delay_default * (steps_acceleration - steps_left)/steps_acceleration) ;
			}
			else {
				step_delay = step_delay_default;
			}
		}
		else {
			step_delay = 60L * 1000L * 1000L / stepsPerRevolution / 10L;  //вычисление продолжительности одного шага (до 300 шагов), 20 об/мин	(при 20 об/мин = 15000 мкс)
		}



		// приращение или уменьшение шагов в зависимоти от направления вращения
		if (direction == 1){
			step_number = 0;
		}
		else {

			switch (abs_number_of_steps % 4) {     //чтобы первый шаг был нулевым
				case 3:	 //0
				step_number = abs_number_of_steps + 1;
				break;
				case 2:	 //-1
				step_number = abs_number_of_steps + 2;
				break;
				case 1:	 //-2
				step_number = abs_number_of_steps + 3;
				break;
				case 0:	 //-3
				step_number = abs_number_of_steps;
				break;
			}
		}


		
		



		
		stepMotor_Step(step_number);	// установка обмоток шагового двигателя под текущий шаг


		last_time_sec_4 = millis();


		//приращение шагов, движение на один шаг за раз:
		Timer1_init();
	
	}
	
	
}

void stepMotor_Step(int thisStep) {					 //включение обмоток stepMotor
	//	 Установка обмоток шагового двигателя под шаг.


	switch (thisStep) {
		case 0:	 // 1010
		digitalWrite(MOTOR_PIN_1, HIGH);
		digitalWrite(MOTOR_PIN_2, LOW);
		digitalWrite(MOTOR_PIN_3, HIGH);
		digitalWrite(MOTOR_PIN_4, LOW);
		break;
		case 1:	 // 0110
		digitalWrite(MOTOR_PIN_1, LOW);
		digitalWrite(MOTOR_PIN_2, HIGH);
		digitalWrite(MOTOR_PIN_3, HIGH);
		digitalWrite(MOTOR_PIN_4, LOW);
		break;
		case 2:	 //0101
		digitalWrite(MOTOR_PIN_1, LOW);
		digitalWrite(MOTOR_PIN_2, HIGH);
		digitalWrite(MOTOR_PIN_3, LOW);
		digitalWrite(MOTOR_PIN_4, HIGH);
		break;
		case 3:	 //1001
		digitalWrite(MOTOR_PIN_1, HIGH);
		digitalWrite(MOTOR_PIN_2, LOW);
		digitalWrite(MOTOR_PIN_3, LOW);
		digitalWrite(MOTOR_PIN_4, HIGH);
		break;
	}

}

void Timer1_init() {								 //запуск Timer1 (StepMotor)

	if ((TCCR1A == 0) && (TCCR1B == 0)) {
		TIMSK1 = 0;
		TCCR1B |= (1 << WGM12);					 // Режим CTC (сброс по совпадению)
		//Преддилитель
		//TCCR1B |= (1 << CS10);				 // CLK/1
		//TCCR1B |= (1<<CS11);					 // CLK/8		  1 такт часов = 0,5 мкс
		TCCR1B |= (1 << CS10) | (1 << CS11);   // CLK/64	  1 такт часов = 4 мкс
		//TCCR1B |= (1<<CS12);					 // CLK/256
		//TCCR1B |= (1<<CS10)|(1<<CS12);		 // CLK/1024

		// Верхняя граница счета. Диапазон от 0 до 65535.
		unsigned int step_delay_1;
		step_delay_1 = step_delay / 4;
		
		OCR1A = step_delay_1;					  // Частота прерываний A будет = Fclk/(N*(1+OCR1A))
		//OCR1B = 2678;									  // Частота прерываний A будет = Fclk/(N*(1+OCR1B))
		// где N - коэф. предделителя (1, 8, 64, 256 или 1024)
		TIMSK1 |= (1 << OCIE1A);				  // Разрешить прерывание по совпадению A
		//TIMSK1 |= (1 << OCIE1B);					 // Разрешить прерывание по совпадению B
		//TIMSK1 |= (1 << TOIE1);					 // Разрешить прерывание по переполнению
	}
}

ISR (TIMER1_COMPA_vect)		{						 // Обработчик прерывания таймера 1 по совпадению A (StepMotor)

	TCCR1B = 0;		// сброс предделителя, остановка таймера
	TCCR1A = 0;		// запрет на ШИМ и контроль пинов
	TIMSK1 = 0;		// запрет прерываний по счетчикам А и В, а также по переполнению таймера
	TCNT1 = 0;		//сброс счетчика

	if (calibration_StepMotor_step == 0) {			//когда калибровка не выполняется

		// приращение шагов, движение на один шаг за раз:
		if ((steps_left > 0) && (error_array_N[error_StepMotor] == 0))	{


			unsigned long millis_inter;
			millis_inter = millis();

			//проверка схода с концевика
			if (millis_inter < last_time_sec_4) {
				last_time_sec_4 = 0;
			}

			if ((val_19_1_go == 1) && (digitalRead(BUTTON_PIN_3_) == LOW) && ((millis_inter - last_time_sec_4) > time_out_StepMotor)) {		  // проверка схода с концевика
				// индикация ошибок
				error_array_N[error_StepMotor] = 1;
				error();
				steps_left = 0;
				digitalWrite(MOTOR_PIN_5, LOW);
				digitalWrite(MOTOR_PIN_6, LOW);
				digitalWrite(MOTOR_PIN_1, LOW);
				digitalWrite(MOTOR_PIN_2, LOW);
				digitalWrite(MOTOR_PIN_3, LOW);
				digitalWrite(MOTOR_PIN_4, LOW);
				val_19_1_go = 0;
				return;
			}

			if ((val_19_9_go == 1) && (digitalRead(BUTTON_PIN_4_) == LOW) && ((millis_inter - last_time_sec_4) > time_out_StepMotor)) {		  // проверка схода с концевика
				// индикация ошибок
				error_array_N[error_StepMotor] = 1;
				error();
				steps_left = 0;
				digitalWrite(MOTOR_PIN_5, LOW);
				digitalWrite(MOTOR_PIN_6, LOW);
				digitalWrite(MOTOR_PIN_1, LOW);
				digitalWrite(MOTOR_PIN_2, LOW);
				digitalWrite(MOTOR_PIN_3, LOW);
				digitalWrite(MOTOR_PIN_4, LOW);
				val_19_9_go = 0;
				return;
			}


			if ((val_19_1_go == 1) && (digitalRead(BUTTON_PIN_3_) == HIGH)) {		 // обнуляем при сходе с  концевика

				val_19_1_go = 0;
				
				long steps_left_0 = steps_left;
				steps_left = abs_number_of_steps;						 // сколько шагов нужно сделать (без учета направления)
				abs_number_of_steps = (2L * abs_number_of_steps) - steps_left_0;			
			}

			if ((val_19_9_go == 1) && (digitalRead(BUTTON_PIN_4_) == HIGH)) {		 // обнуляем при сходе с  концевика

				val_19_9_go = 0;
				
				long steps_left_0 = steps_left;
				steps_left = abs_number_of_steps;						 // сколько шагов нужно сделать (без учета направления)
				abs_number_of_steps = (2L * abs_number_of_steps) - steps_left_0;
			}



			// вычисление длительности шага для плавного разгона и останова StepMotor:
			if ( abs_number_of_steps >= 300L ) {
				if (steps_left > (abs_number_of_steps - steps_acceleration) ) {
					step_delay = step_delay_default + (2L * step_delay_default * (steps_acceleration - (abs_number_of_steps - steps_left)) / steps_acceleration);
				}
				else if (steps_left < steps_acceleration ) {
					step_delay = step_delay_default + (2L * step_delay_default * (steps_acceleration - steps_left)/steps_acceleration) ;
				}
				else {
					step_delay = step_delay_default;
				}
			}
			else {
				step_delay = 60L * 1000L * 1000L / stepsPerRevolution / 10L;  //вычисление продолжительности одного шага (до 300 шагов), 20 об/мин	(при 20 об/мин = 15000 мкс)
			}




			// приращение или уменьшение шагов в зависимоти от направления вращения
			if (direction == 1){
				step_number++;
			}
			else {
				step_number--;
			}




			// установка обмоток шагового двигателя под текущий шаг
			switch (step_number % 4) {
				case 0:	 // 1010
				digitalWrite(MOTOR_PIN_1, HIGH);
				digitalWrite(MOTOR_PIN_2, LOW);
				digitalWrite(MOTOR_PIN_3, HIGH);
				digitalWrite(MOTOR_PIN_4, LOW);
				break;
				case 1:	 // 0110
				digitalWrite(MOTOR_PIN_1, LOW);
				digitalWrite(MOTOR_PIN_2, HIGH);
				digitalWrite(MOTOR_PIN_3, HIGH);
				digitalWrite(MOTOR_PIN_4, LOW);
				break;
				case 2:	 //0101
				digitalWrite(MOTOR_PIN_1, LOW);
				digitalWrite(MOTOR_PIN_2, HIGH);
				digitalWrite(MOTOR_PIN_3, LOW);
				digitalWrite(MOTOR_PIN_4, HIGH);
				break;
				case 3:	 //1001
				digitalWrite(MOTOR_PIN_1, HIGH);
				digitalWrite(MOTOR_PIN_2, LOW);
				digitalWrite(MOTOR_PIN_3, LOW);
				digitalWrite(MOTOR_PIN_4, HIGH);
				break;
			}

			steps_left--;						// уменьшение шагов


			Timer1_init();						//следующий шаг
		}

		else {

			steps_left = 0;
			this_position_19 = position_19;								   // запоминание текущей позиции

			digitalWrite(MOTOR_PIN_5, LOW);
			digitalWrite(MOTOR_PIN_6, LOW);
			digitalWrite(MOTOR_PIN_1, LOW);
			digitalWrite(MOTOR_PIN_2, LOW);
			digitalWrite(MOTOR_PIN_3, LOW);
			digitalWrite(MOTOR_PIN_4, LOW);
		}

	}

	if (((calibration_StepMotor_step >= 1) && (calibration_StepMotor_step <= 4)) || ((calibration_StepMotor_step >= 101) && (calibration_StepMotor_step <= 104)))	{	//если выполняется калибровка
	
		// приращение шагов, движение на один шаг за раз:
		if (steps_left > 0)	{


			// вычисление длительности шага для плавного разгона и останова StepMotor:
			if ( abs_number_of_steps >= 300L ) {
				if (steps_left > (abs_number_of_steps - steps_acceleration) ) {
					step_delay = step_delay_default + (2L * step_delay_default * (steps_acceleration - (abs_number_of_steps - steps_left)) / steps_acceleration);
				}
				else if (steps_left < steps_acceleration ) {
					step_delay = step_delay_default + (2L * step_delay_default * (steps_acceleration - steps_left)/steps_acceleration) ;
				}
				else {
					step_delay = step_delay_default;
				}
			}
			else {
				step_delay = 60L * 1000L * 1000L / stepsPerRevolution / 10L;  //вычисление продолжительности одного шага (до 300 шагов), 20 об/мин	(при 20 об/мин = 15000 мкс)
			}




			// приращение или уменьшение шагов в зависимоти от направления вращения
			if (direction == 1){
				step_number++;
			}
			else {
				step_number--;
			}




			// установка обмоток шагового двигателя под текущий шаг
			switch (step_number % 4) {
				case 0:	 // 1010
				digitalWrite(MOTOR_PIN_1, HIGH);
				digitalWrite(MOTOR_PIN_2, LOW);
				digitalWrite(MOTOR_PIN_3, HIGH);
				digitalWrite(MOTOR_PIN_4, LOW);
				break;
				case 1:	 // 0110
				digitalWrite(MOTOR_PIN_1, LOW);
				digitalWrite(MOTOR_PIN_2, HIGH);
				digitalWrite(MOTOR_PIN_3, HIGH);
				digitalWrite(MOTOR_PIN_4, LOW);
				break;
				case 2:	 //0101
				digitalWrite(MOTOR_PIN_1, LOW);
				digitalWrite(MOTOR_PIN_2, HIGH);
				digitalWrite(MOTOR_PIN_3, LOW);
				digitalWrite(MOTOR_PIN_4, HIGH);
				break;
				case 3:	 //1001
				digitalWrite(MOTOR_PIN_1, HIGH);
				digitalWrite(MOTOR_PIN_2, LOW);
				digitalWrite(MOTOR_PIN_3, LOW);
				digitalWrite(MOTOR_PIN_4, HIGH);
				break;
			}

			steps_left--;						// уменьшение шагов


			Timer1_init();						//следующий шаг
		}

		else {

			steps_left = 0;
			this_position_19 = position_19;								   // запоминание текущей позиции

			digitalWrite(MOTOR_PIN_5, LOW);
			digitalWrite(MOTOR_PIN_6, LOW);
			digitalWrite(MOTOR_PIN_1, LOW);
			digitalWrite(MOTOR_PIN_2, LOW);
			digitalWrite(MOTOR_PIN_3, LOW);
			digitalWrite(MOTOR_PIN_4, LOW);
		}
	
	
	
	
	}
	
	
}





//----------------------------------------------------------------------------------------
//функции опроса концевиков
void read_Button() {								 //опрос концевиков




	val_12_1_2 = 0;
	val_12_2_2 = 0;

	val_19_1_2 = 0;
	val_19_9_2 = 0;




	if (digitalRead(BUTTON_PIN_1_) == 0){
		val_12_1_2 = 1;
	}
	else {
		val_12_1_1 = 0;
	}
	if (digitalRead(BUTTON_PIN_2_) == 0){
		val_12_2_2 = 1;
	}
	else {
		val_12_2_1 = 0;
	}


	if (digitalRead(BUTTON_PIN_3_) == 0){
		val_19_1_2 = 1;
	}
	else {
		val_19_1_1 = 0;
	}
	if (digitalRead(BUTTON_PIN_4_) == 0){
		val_19_9_2 = 1;
	}
	else {
		val_19_9_1 = 0;
	}




	// проверка положения концевиков S1 , S2
	if (val_12_1_2 == 0 && val_12_2_2 == 0) {
		this_position_12_end = 2;
	}
	if (val_12_1_2 == 1 && val_12_2_2 == 0) {
		digitalWrite(DC_MOTOR_1_A, LOW);
		digitalWrite(DC_MOTOR_1_B, LOW);
		this_position_12_end = open_CO2;
	}
	if (val_12_1_2 == 0 && val_12_2_2 == 1) {
		digitalWrite(DC_MOTOR_1_A, LOW);
		digitalWrite(DC_MOTOR_1_B, LOW);
		this_position_12_end = close_CO2;
	}
	if (val_12_1_2 == 1 && val_12_2_2 == 1) {

		digitalWrite(DC_MOTOR_1_A, LOW);
		digitalWrite(DC_MOTOR_1_B, LOW);
		// индикация ошибок
		error_array_N[error_DCmotor] = 1;
		error();			 // световая сигнализация
	}



	// проверка положения концевиков S3 , S4
	if (val_19_1_2 == 0 && val_19_9_2 == 0) {
		this_position_19_end = 0;
	}
	if (val_19_1_2 == 1 && val_19_9_2 == 0) {
		// Останов таймера таймера 1 по совпадению A (StepMotor)
		TCCR1B = 0;
		TCCR1A = 0;
		TIMSK1 = 0;
		TCNT1 = 0;	   //сброс счетчика

		this_position_19_end = 1;
		steps_left = 0;
	}
	if (val_19_1_2 == 0 && val_19_9_2 == 1) {
		// Останов таймера таймера 1 по совпадению A (StepMotor)
		TCCR1B = 0;
		TCCR1A = 0;
		TIMSK1 = 0;
		TCNT1 = 0;	   //сброс счетчика

		this_position_19_end = 9;
		steps_left = 0;
	}
	if (val_19_1_2 == 1 && val_19_9_2 == 1) {
		// Останов таймера таймера 1 по совпадению A (StepMotor)
		TCCR1B = 0;
		TCCR1A = 0;
		TIMSK1 = 0;
		TCNT1 = 0;	   //сброс счетчика

		steps_left = 0;

		this_position_19_end = 10;

		error_array_N[error_StepMotor] = 1;
		error();			 // световая сигнализация
	}




	val_12_1_2 = 0;
	val_12_2_2 = 0;


	val_19_1_2 = 0;
	val_19_9_2 = 0;



}  //read_Button()

void position_12_1_stop() {							 //останов DC_Motor в крайней (1) позиции по прерыванию

		val_12_1_1 = 1;						//флаг проверки на дребезг
		Timer4_init();						//запуск таймера проверки на дребезг

}

void position_12_2_stop() {							 //останов DC_Motor в крайней (2) позиции по прерыванию

		val_12_2_1 = 1;						//флаг проверки на дребезг
		Timer4_init();						//запуск таймера проверки на дребезг

}

void position_19_1_stop() {							 //останов StepMotor в крайней (1) позиции по прерыванию

		val_19_1_1 = 1;						//флаг проверки на дребезг
		Timer5_init();						//запуск таймера проверки на дребезг
	
}

void position_19_9_stop() {							 //останов StepMotor в крайней (9) позиции по прерыванию

		val_19_9_1 = 1;						//флаг проверки на дребезг
		Timer5_init();						//запуск таймера проверки на дребезг

}

void Timer4_init() {								 //запуск Timer4 (концевики 1 и 2)

	if ((TCCR4A == 0) && (TCCR4B == 0)) {
		TCCR4B |= (1 << WGM42);					 // Режим CTC (сброс по совпадению)
		// Предделитель :
		//TCCR4B |= (1 << CS40);				 // CLK/1
		//TCCR4B |= (1 << CS41);				 // CLK/8
		//TCCR4B |= (1 << CS40)|(1 << CS41);	 // CLK/64
		//TCCR4B |= (1 << CS42);				 // CLK/256
		TCCR4B |= (1 << CS40)|(1 << CS42);	   // CLK/1024

		// Верхняя граница счета. Диапазон от 0 до 65535.
		
		long time_out_DCmotor_1_1;
		time_out_DCmotor_1_1= time_out_DCmotor_1 * 15625L / 1000L;
		
		OCR4A = time_out_DCmotor_1_1;			// Частота прерываний A будет = Fclk/(N*(1+OCR5A))
		//OCR4B = 1500;								// Частота прерываний B будет = Fclk/(N*(1+OCR5B))
		// где N - коэф. предделителя (1, 8, 64, 256 или 1024)
		TIMSK4 |= (1 << OCIE4A);					// Разрешить прерывание по совпадению A
		//TIMSK4 |= (1 << OCIE4B);				   // Разрешить прерывание по совпадению B
		//TIMSK4 |= (1 << TOIE4);				   // Разрешить прерывание по переполнению
	}
}

ISR (TIMER4_COMPA_vect) {							 // Обработчик прерывания таймера 4 по совпадению A (концевики 1 и 2)

	// Останов таймера
	TCCR4B = 0;
	TCCR4A = 0;
	TCNT4 = 0;	   //сброс счетчика

	if ((val_12_1_1 == 1) && (digitalRead(BUTTON_PIN_1_) == 0)) {
		digitalWrite(DC_MOTOR_1_A, LOW);
		digitalWrite(DC_MOTOR_1_B, LOW);
		
		TimerList.Delete(THandle_DC_motor_failure);		//останов таймера проверки остановки DCmotor
		
		EIFR |= (1 << INTF2);	  // (установка флагов срабатывания в "0" при включении прерывания 1)  ??
	}

	if ((val_12_2_1 == 1) && (digitalRead(BUTTON_PIN_2_) == 0)) {
		digitalWrite(DC_MOTOR_1_A, LOW);
		digitalWrite(DC_MOTOR_1_B, LOW);
		
		TimerList.Delete(THandle_DC_motor_failure);		//останов таймера проверки остановки DCmotor
		
		EIFR |= (1 << INTF3);	  // (установка флагов срабатывания в "0" при включении прерывания 2)??
	}



	if (digitalRead(BUTTON_PIN_1_) == 1) {		//если нажатие не подтвердилось после проверки на дребезг
		val_12_1_1 = 0;
	}

	if (digitalRead(BUTTON_PIN_2_) == 1) {		//если нажатие не подтвердилось после проверки на дребезг
		val_12_2_1 = 0;
	}


}

void Timer5_init() {								 //запуск Timer5 (концевики 3 и 4)

	if ((TCCR5A == 0) && (TCCR5B == 0)) {
		TCCR5B |= (1 << WGM52);					 // Режим CTC (сброс по совпадению)
		// Предделитель
		//TCCR5B |= (1 << CS50);					// CLK/1
		//TCCR5B |= (1 << CS51);					// CLK/8
		//TCCR5B |= (1 << CS50)|(1 << CS51);		// CLK/64
		//TCCR5B |= (1 << CS52);					// CLK/256
		TCCR5B |= (1 << CS50)|(1 << CS52);		  // CLK/1024

		// Верхняя граница счета. Диапазон от 0 до 65535.
		
		
		long time_out_StepMotor_1_1;
		time_out_StepMotor_1_1 = time_out_StepMotor_1 * 15625L / 1000L;
		
		OCR5A = time_out_StepMotor_1_1;			  // Частота прерываний A будет = Fclk/(N*(1+OCR5A))
		//OCR5B = 1500;								// Частота прерываний B будет = Fclk/(N*(1+OCR5B))
		// где N - коэф. предделителя (1, 8, 64, 256 или 1024)
		TIMSK5 |= (1 << OCIE5A);				 // Разрешить прерывание по совпадению A
		//TIMSK5 |= (1 << OCIE5B);				   // Разрешить прерывание по совпадению B
		//TIMSK5 |= (1 << TOIE5);				   // Разрешить прерывание по переполнению
	}
}

ISR (TIMER5_COMPA_vect) {							 // Обработчик прерывания таймера 5 по совпадению A	 (концевики 3 и 4)


	// Останов таймера таймера 5 по совпадению A  (концевики 3 и 4)
	TCCR5B = 0;
	TCCR5A = 0;
	TCNT5 = 0;	   //сброс счетчика




	if ((val_19_1_1 == 1) && (digitalRead(BUTTON_PIN_3_) == 0)) {

		steps_left = 0;

		this_position_19 = 1;

		EIFR |= (1 << INTF4);	// (установка флагов срабатывания в "0" при включении прерывания 3)	  ??
	}

	if ((val_19_9_1 == 1) && (digitalRead(BUTTON_PIN_4_) == 0)) {

		steps_left = 0;

		this_position_19 = 9;

		EIFR |= (1 << INTF5);	// (установка флагов срабатывания в "0" при включении прерывания 4)	  ??
	}


	if (digitalRead(BUTTON_PIN_3_) == 1) {
		val_19_1_1 = 0;
	}

	if (digitalRead(BUTTON_PIN_4_) == 1) {
		val_19_9_1 = 0;
	}

}





//---------------------------------------------------------------------------------------
//функция опроса концевика двери холодильника
void read_pin_door() {


////при закрытии только внешней двери

	if ((digitalRead(BUTTON_PIN_DOOR_) == HIGH) && (door_position == DOOR_PRE_OPEN) && (door_step_1 == 1)) {	  // если дверь Закрывается
		
		last_time_sec_14 = millis();
		door_step_1 = 2;
	}
	
	if (millis() < last_time_sec_14) {					// при переполнении millis()
		last_time_sec_14 = 0;
	}


	if (((millis() - last_time_sec_14) >= 4000) && (door_step_1 == 2)) {	// проверка на дребезг при Закрытии двери + выдержка от проверки перехода в режим загрузки камеры

		if (digitalRead(BUTTON_PIN_DOOR_) == HIGH){
			
			door_position = DOOR_CLOSE;
			
			digitalWrite(LED_GREEN, LOW);
			digitalWrite(LED_RED, LOW);
			digitalWrite(RELE_LIGHT, LOW);
			
			//сброс процедуры ручного нажатия
			index_door_button = 0;			
			//обнуление массива нажатий
			for (int i = 0; i < DOOR_ARRAY_LENGTH; i++) {
				array_door_button[i] = 0;
			}
			
		}
			door_step_1 = 1;
	}



////при закрытии внешней и внутренней дверей

	if ((digitalRead(BUTTON_PIN_DOOR_) == HIGH) && (door_position == DOOR_OPEN) && (door_step_1 == 1)) {	  // если дверь Закрывается
		
		last_time_sec_6 = millis();
		door_step_1 = 3;
	}

	
	if (millis() < last_time_sec_6) {					// при переполнении millis()
		last_time_sec_6 = 0;
	}


	if (((millis() - last_time_sec_6) >= 4000) && (door_step_1 == 3)) {	// проверка на дребезг при Закрытии двери + выдержка от проверки перехода в режим загрузки камеры

		if (digitalRead(BUTTON_PIN_DOOR_) == HIGH){
			
			door_position = DOOR_CLOSE;
			door_position_one = DOOR_CLOSE_ONE;
			
			digitalWrite(LED_GREEN, LOW);
			digitalWrite(LED_RED, LOW);
			digitalWrite(RELE_LIGHT, LOW);
			
			
			//сброс процедуры ручного нажатия
			index_door_button = 0;			
			//обнуление массива нажатий
			for (int i = 0; i < DOOR_ARRAY_LENGTH; i++) {
				array_door_button[i] = 0;
			}
			
		}
			door_step_1 = 1;
	}




////при открытии внешней двери

	if ((digitalRead(BUTTON_PIN_DOOR_) == LOW) && (door_position == DOOR_CLOSE) && (door_step_2 == 1)) {	 // если дверь Открывается
		last_time_sec_2 = millis();
		door_step_2 = 2;
	}


	if (millis() < last_time_sec_2) {					// при переполнении millis()
		last_time_sec_2 = 0;
	}

	if (((millis() - last_time_sec_2) >= 300) && (door_step_2 == 2)) {	// проверка на дребезг при Открытии двери

		if (digitalRead(BUTTON_PIN_DOOR_) == LOW){
			
			door_position = DOOR_PRE_OPEN;			//флаг открытия внешней двери
			door_position_one = DOOR_OPEN_ONE;		//флаг первого момента открытия двери
			
			
			//обнуление массива нажатий
			for (int i = 0; i < DOOR_ARRAY_LENGTH; i++) {
				array_door_button[i] = 0;
			}
			index_door_button = 0;
	
		}
		door_step_2 = 1;
	}

	
////при открытии внутренней двери
		
	if (door_position == DOOR_PRE_OPEN) {	  // если наружная дверь в состоянии ОТКРЫТО

		if ((digitalRead(BUTTON_PIN_DOOR_) == HIGH) && (index_door_button == 0)){	//если кнопка нажата вручную в первый раз
			array_door_button[index_door_button] = millis();
			index_door_button++;
		}

		if ((digitalRead(BUTTON_PIN_DOOR_) == HIGH) && (index_door_button % 2 == 0) && ((millis() - array_door_button[index_door_button - 1]) > 100)&& (index_door_button != 0)){	//если кнопка нажата вручную
			array_door_button[index_door_button] = millis();
			index_door_button++;
		}
		
		if ((digitalRead(BUTTON_PIN_DOOR_) == LOW) && (index_door_button % 2 == 1) && ((millis() - array_door_button[index_door_button - 1]) > 100)){	//если кнопка нажата вручную
			array_door_button[index_door_button] = millis();
			index_door_button++;
		}

		
		if (index_door_button  == DOOR_ARRAY_LENGTH){					//проверка массива нажатий на корректность

			int index_click = 0;
		
			for (int i = 0; i < (DOOR_ARRAY_LENGTH - 1); i++) {
				if(((array_door_button[i + 1] - array_door_button[i]) <= 1000) &&
					((array_door_button[i + 1] - array_door_button[i]) >= 100)){
				
					index_click++;
				
				}
			}			

			
			if (index_click == (DOOR_ARRAY_LENGTH - 2)){

				door_position = DOOR_OPEN;
			}
			
			index_door_button = 0;
			
			//обнуление массива нажатий
			for (int i = 0; i < DOOR_ARRAY_LENGTH; i++) {
				array_door_button[i] = 0;
			}				
			
			
		}


		
		if (((millis() - array_door_button[0]) > 3000) && (array_door_button[0] != 0)){	//если кнопка нажата не в установленном порядке
		
			//обнуление массива нажатий
			for (int i = 0; i < DOOR_ARRAY_LENGTH; i++) {
				array_door_button[i] = 0;
			}
			
			index_door_button = 0;			
			
			
		}
		

	}
	
		
}





//---------------------------------------------------------------------------------------
//функции датчика DHT21
void read_DHT() {


	if (read_DHT_error >= 10) {
		error_array_N[error_DHT21] = 1;
		error();
	}


	currenttime = millis();
	if (currenttime < _lastreadtime) {
		_lastreadtime = 0;
	}

	if (!_firstreading && ((currenttime - _lastreadtime) < 2000)) {
		return;
	}

	_firstreading = false;
	_lastreadtime = millis();

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;
	
	for (int i = 0; i < 83; i++)	 {
		cycles[i]	= 0;
	}
	
	
	pinMode(DHT21_1_PIN, INPUT);
	digitalWrite(DHT21_1_PIN, HIGH);
	delay(5);
	
	pinMode(DHT21_1_PIN, OUTPUT);
	digitalWrite(DHT21_1_PIN, LOW);
	
	//далее - ждем от 0,8 до 20 мс
	
	delay(5);

	pinMode(DHT21_1_PIN, INPUT);
	digitalWrite(DHT21_1_PIN, HIGH);

	
	cli();		//запрещаем прерывания на время получения ответа с датчика

	cycles[0] = expectPulse(HIGH);
	
	if (0 == cycles[0])	 {
		_lastresult = false;
		read_DHT_error++;
		return;
	}

	
	cycles[1] = expectPulse(LOW);
	
	if (0 == cycles[1])	 {
		_lastresult = false;
		read_DHT_error++;
		return;
	}	
	
	cycles[2] = expectPulse(HIGH);
	
	if (0 == cycles[2])	 {
		_lastresult = false;
		read_DHT_error++;
		return;
	}	
	


	for (int i = 3; i < 83; i += 2)	 {

		cycles[i]	= expectPulse(LOW);
		if (cycles[i] == 0){
			read_DHT_error++;
			_lastresult = false;
			return;
		}

		cycles[i + 1] = expectPulse(HIGH);
		if (cycles[i + 1] == 0){
			read_DHT_error++;
			_lastresult = false;
			return;
		}
	}

	sei();			//разрешаем прерывания

	for (int i = 0; i < 40; ++i)  {
		uint32_t lowCycles	= cycles[2 * i + 3];
		uint32_t highCycles = cycles[2 * i + 1 + 3];
		if ((lowCycles == 0) || (highCycles == 0))	 {
			_lastresult = false;
			read_DHT_error++;
			return;
		}
		data[i / 8] <<= 1;

		if (highCycles > lowCycles)	  {
			data[i / 8] |= 1;
		} 
	}


	if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))  {

		//	вычисление температуры с датчика  DHT21:
		temp_c_DHT21_1 = data[2] & 0x7F;
		temp_c_DHT21_1 *= 256;
		temp_c_DHT21_1 += data[3];
		temp_c_DHT21_1 *= 10;
		if (data[2] & 0x80){
			temp_c_DHT21_1 *= -1;
		}
		//	вычисление влажности с датчика	DHT21:
		humidity_DHT21_1 = data[0];
		humidity_DHT21_1 *= 256;
		humidity_DHT21_1 += data[1];
		humidity_DHT21_1 *= 10;

		read_DHT_error = 0;

		if (error_array_N[error_DHT21] == 1) {
			error_array_N[error_DHT21] = 0;
			error();
		}

		_lastresult = true;
		return;
	}
	else {
		read_DHT_error++;
		_lastresult = false;
		return;
	}
	
	

}

uint32_t expectPulse(bool level) {					 //датчик  DHT21

	uint32_t count1 = 0;

	while (digitalRead(DHT21_1_PIN) == level) {
		if (count1++ >= _maxcycles) {
			return 0;
		}

	}
	return count1;
} //expectPulse(bool level)






//---------------------------------------------------------------------------
// функции датчика SHT1x
void read_SHT1x(){
	// отправка команды на конвертацию температуры
	
	int gTempCmd  = 0b00000011;

	sendCommandSHT(gTempCmd, DADA_PIN, CLOCK_PIN);
	
	read_SHT1x_step = 1;

	THandle_SHT1x_1_task = TimerList.Add(read_SHT1x_1_in_task, 2);		 //запуск асинхронного таймера на 2 мс
}

void read_SHT1x_1(){

	if (read_SHT1x_step != 1) {			//если не очередной шаг
		return;
	}


	if (digitalRead(DADA_PIN)) {			//если датчик НЕ ответил, оставляем опрос по асинхронному таймеру
		THandle_SHT1x_1_task = TimerList.Add(read_SHT1x_1_in_task, 2);		 //запуск асинхронного таймера на 2 мс
		return;
	}
	

	int _val;

	// Conversion coefficients from SHT15 datasheet
	const float D1 = -40.0;	 // for 14 Bit @ 5V
	const float D2 =   0.01; // for 14 Bit DEGC

	_val = getData16SHT(DADA_PIN, CLOCK_PIN);
	skipCrcSHT(DADA_PIN, CLOCK_PIN);

	// Convert raw value to degrees Celsius
	temp_SHT1x = ((_val * D2) + D1)*100.0;
		
	if (temp_SHT1x <= -6000L || temp_SHT1x >= 15000L){			//если возвращаемая температура меньше 60°С и больше 150°С
		error_array_N[error_SHT1x] = 1;
		error();
	}

	// отправка команды на конвертацию влажности
	int gHumidCmd = 0b00000101;

	// Fetch the value from the sensor
	sendCommandSHT(gHumidCmd, DADA_PIN, CLOCK_PIN);

	read_SHT1x_step = 2;

	THandle_SHT1x_2_task = TimerList.Add(read_SHT1x_2_in_task, 2);		 //запуск асинхронного таймера на 2 мс

}

void read_SHT1x_2(){

	if (read_SHT1x_step != 2) {			//если не очередной шаг
		return;
	}

	if (digitalRead(DADA_PIN)) {			//если датчик НЕ ответил, оставляем опрос по асинхронному таймеру
		THandle_SHT1x_2_task = TimerList.Add(read_SHT1x_2_in_task, 2);		 //запуск асинхронного таймера на 2 мс
		return;
	}

	int _val;					 // Raw humidity value returned from sensor
	float _linearHumidity;		 // Humidity with linear correction applied

	// Conversion coefficients from SHT15 datasheet
	const float C1 = -4;				// for 12 Bit
	const float C2 =  0.0405;			// for 12 Bit
	const float C3 = -0.0000028;		// for 12 Bit
	const float T1 =  0.01;				// for 12 Bit
	const float T2 =  0.00008;			// for 12 Bit

	_val = getData16SHT(DADA_PIN, CLOCK_PIN);
	skipCrcSHT(DADA_PIN, CLOCK_PIN);

	// Apply linear conversion to raw value
	_linearHumidity = C1 + C2 * _val + C3 * _val * _val;

	// Correct humidity value for current temperature
	humidity_SHT1x = ((temp_SHT1x/100L - 25.0 ) * (T1 + T2 * _val) + _linearHumidity)*100.0;

	if (humidity_SHT1x <= 500L || humidity_SHT1x >= 9900L){			//если возвращаемая влажность меньше 5% и больше 99%
		error_array_N[error_SHT1x] = 1;
		error();
	}	
	
	read_SHT1x_step = 0;
	
	if (error_array_N[error_SHT1x] == 1){
		error_array_N[error_SHT1x] = 0;
		error();
	}
	

}

void sendCommandSHT(int _command, int _DADA_PIN, int _CLOCK_PIN){
  int ack;

  // Transmission Start
  pinMode(_DADA_PIN, OUTPUT);
  pinMode(_CLOCK_PIN, OUTPUT);
  digitalWrite(_DADA_PIN, HIGH);
  digitalWrite(_CLOCK_PIN, HIGH);
  digitalWrite(_DADA_PIN, LOW);
  digitalWrite(_CLOCK_PIN, LOW);
  digitalWrite(_CLOCK_PIN, HIGH);
  digitalWrite(_DADA_PIN, HIGH);
  digitalWrite(_CLOCK_PIN, LOW);

  // The command (3 msb are address and must be 000, and last 5 bits are command)
  shiftOut(_DADA_PIN, _CLOCK_PIN, MSBFIRST, _command);

  // Verify we get the correct ack
  digitalWrite(_CLOCK_PIN, HIGH);
  pinMode(_DADA_PIN, INPUT);
  ack = digitalRead(_DADA_PIN);
  if (ack != LOW) {
	//Serial.println("Ack Error 0");
  }
  digitalWrite(_CLOCK_PIN, LOW);
  ack = digitalRead(_DADA_PIN);
  if (ack != HIGH) {
	//Serial.println("Ack Error 1");
  }
}

void skipCrcSHT(int _DADA_PIN, int _CLOCK_PIN){
  // Skip acknowledge to end trans (no CRC)
  pinMode(_DADA_PIN, OUTPUT);
  pinMode(_CLOCK_PIN, OUTPUT);

  digitalWrite(_DADA_PIN, HIGH);
  digitalWrite(_CLOCK_PIN, HIGH);
  digitalWrite(_CLOCK_PIN, LOW);
}

int getData16SHT(int _DADA_PIN, int _CLOCK_PIN){
  int val;

  // Get the most significant bits
  pinMode(_DADA_PIN, INPUT);
  pinMode(_CLOCK_PIN, OUTPUT);
  val = shiftIn(_DADA_PIN, _CLOCK_PIN, 8);
  val *= 256;

  // Send the required ack
  pinMode(_DADA_PIN, OUTPUT);
  digitalWrite(_DADA_PIN, HIGH);
  digitalWrite(_DADA_PIN, LOW);
  digitalWrite(_CLOCK_PIN, HIGH);
  digitalWrite(_CLOCK_PIN, LOW);

  // Get the least significant bits
  pinMode(_DADA_PIN, INPUT);
  val |= shiftIn(_DADA_PIN, _CLOCK_PIN, 8);

  return val;
}

int shiftIn(int _DADA_PIN, int _CLOCK_PIN, int _numBits){
  int ret = 0;
  int i;

  for (i=0; i<_numBits; ++i)
  {
	 digitalWrite(_CLOCK_PIN, HIGH);
	 delay(5);								  // I don't know why I need this, but without it I don't get my 8 lsb of temp
	 ret = ret*2 + digitalRead(_DADA_PIN);
	 digitalWrite(_CLOCK_PIN, LOW);
  }

  return(ret);
}







//---------------------------------------------------------------------------------
void read_MHZ19() {									 // опрос датчика СО2 MHZ19

	// опрос датчика СО2 MHZ19
	Serial2.write(cmd, 9);							//отправка команды в датчик СО2
	
	int i;
	
	for (i = 0; i < 9 ; i++){		//обнуление массива ответа
		response[i] = 0;
	}
	

	time_out_MHZ19 = millis();

	i = 0;
	
	while (i < 9) {
		
		if(Serial2.available()){
			response[i] = Serial2.read();
			i++;
		}
		
		if (millis() < time_out_MHZ19) {					// при переполнении millis()
			time_out_MHZ19 = 0;
		}

		
		if((millis() - time_out_MHZ19) > 100){
			
				// индикация ошибок
				error_array_N[error_MHZ19] = 1;
				error();			 // световая сигнализация
				return;
		}
		
	}
	
		
	
	//проверка контрольной суммы ответа датчика СО2:
	byte crc = 0;
	for (int i = 1; i < 8; i++) {
		crc += response[i];
	}
	crc = 255 - crc;
	crc++;

	//вычисление уровня СО2 из ответа датчика:
	if ( !(response[0] == 0xFF && response[1] == 0x86 && response[8] == crc) ) {

		read_MHZ19_error++;
		if (read_MHZ19_error > 5) {
			read_MHZ19_error = 0;
			// индикация ошибок
			error_array_N[error_MHZ19] = 1;
			error();			 // световая сигнализация
			return;
		}
	}
	else {
		unsigned int responseHigh = (unsigned int) response[2];
		unsigned int responseLow = (unsigned int) response[3];
		ppm = (256 * responseHigh) + responseLow;
		read_MHZ19_error = 0;
		
		if (error_array_N[error_MHZ19] == 1){
			error_array_N[error_MHZ19] = 0;
			error();
		}
		
		return;
	}

}


//-------------------------------------------------------------------------------
void read_DS18b20() {								 // опрос датчиков DS18b20

	sensors.requestTemperatures();							  // Отправка команды на получение температуры
	tempC1 = sensors.getTempC(insideThermometer1) * 100L;			  // температура с датчика DS18B20 №1
	tempC2 = sensors.getTempC(insideThermometer2) * 100L;			  // температура с датчика DS18B20 №2
	tempC3 = sensors.getTempC(insideThermometer3) * 100L;			  // температура с датчика DS18B20 №3
	tempC4 = sensors.getTempC(insideThermometer4) * 100L;			  // температура с датчика DS18B20 №4

	if ((tempC1 < -10000) || (tempC2 < -10000) || (tempC3 < -10000) || (tempC4 < -10000)) {
		read_DS18b20_error ++;
	}

	if (read_DS18b20_error > 10) {
		// индикация ошибок
		error_array_N[error_DS18b20] = 1;
		error();			 // световая сигнализация
	}

	if ((tempC1 > -5000) && (tempC2 > -5000) && (tempC3 > -5000) && (tempC4 > -5000)) {
		read_DS18b20_error = 0;
		
		if (error_array_N[error_DS18b20] == 1){
			error_array_N[error_DS18b20] = 0;
			error();
		}
	}
}



//-----------------------------------------------------------------
//функция запроса времени с часов
void read_RCT() {

			// запрос текущего времени с часов RCT DS1307 и проверка часов RTC на ошибки:
		if (!RTC.read(tm)) {
			if (RTC.chipPresent()) {
				// индикация ошибок
				error_array_N[error_RCT] = 1;
				error();			   // световая сигнализация

			}
			else {
				// индикация ошибок
				error_array_N[error_RCT] = 1;
				error();			 // световая сигнализация

			}

			crash_RCT();										// аварийный режим при неработающих часах
			return;												// дальнейший код не выполняется
		} 
		else {
			if (error_array_N[error_RCT] == 1){
				error_array_N[error_RCT] = 0;
				error();
			}
		}

}




//--------------------------------------------------------------------------------------
//функции обработчика ошибок
void error() {										 //обработчик ошибок

	cli();			 //вЫкючаем прерывания

	for (int i = 0; i < ARRAY_LENGH; i++) {
		error_array_set[i] = 0;
		error_array_delay[i] = 0;
	}


	error_array_N[0] = 0;						//не используемый элемент
	error_array_delay[0] = DELAY_RED_0;
	error_array_delay[1] = DELAY_RED_PAUSE;
	error_array_set[0] = 1;




	for (int i1 = 1; i1 < ARRAY_LENGH_N; i1++) {

		if (error_array_N[i1] != 0) {

		
			for (int i = 1; i < ARRAY_LENGH; i++) {
				if (error_array_set[i] == error_array_set[i - 1]) {
					array_position = i;
					i = ARRAY_LENGH;
				}
			}

			for ( int i = array_position; i <= (array_position + i1 * 2 - 1); i++) {
				if (i == (array_position + i1 * 2 - 1)) {
					error_array_delay[i] = DELAY_RED_PAUSE;
				}
				else {
					error_array_delay[i] = DELAY_RED_PULSE;
				}

				if (i % 2 == 0) {
					error_array_set[i] = 1;
				}
				else {
					error_array_set[i] = 0;
				}
			}

		}

	}

	sei();				 //вКлючаем прерывания

	Timer3_init();


}

void Timer3_init() {								 //запуск Timer3  (индикатор ошибок)

	if ((TCCR3A == 0) && (TCCR3B == 0)) {
		TCCR3B |= (1 << WGM32);					 // Режим CTC (сброс по совпадению)
		// Предделитель :
		//TCCR3B |= (1 << CS30);				 // CLK/1
		//TCCR3B |= (1 << CS31);				 // CLK/8
		//TCCR3B |= (1 << CS30)|(1 << CS31);	 // CLK/64
		//TCCR3B |= (1 << CS32);				 // CLK/256
		TCCR3B |= (1 << CS30) | (1 << CS32);   // CLK/1024

		// Верхняя граница счета. Диапазон от 0 до 65535.
		OCR3A = error_array_delay[error_step] * 15625L / 1000L;			// Частота прерываний A будет = Fclk/(N*(1+OCR5A))
		//OCR3B = 1500;								// Частота прерываний B будет = Fclk/(N*(1+OCR5B))
		// где N - коэф. предделителя (1, 8, 64, 256 или 1024)
		TIMSK3 |= (1 << OCIE3A);				 // Разрешить прерывание по совпадению A
		//TIMSK3 |= (1 << OCIE3B);				   // Разрешить прерывание по совпадению B
		//TIMSK3 |= (1 << TOIE3);				   // Разрешить прерывание по переполнению
	}
}

ISR (TIMER3_COMPA_vect) {							 // Обработчик прерывания таймера 4 по совпадению A (индикатор ошибок)

	// Останов таймера
	TCCR3B = 0;
	TCCR3A = 0;
	TCNT3 = 0;


	if (error_array_set[error_step] == error_array_set[error_step + 1]) {
		digitalWrite(LED_RED, error_array_set[error_step]);
		Timer3_init();
		error_step = 0;
	}
	else {
		digitalWrite(LED_RED, error_array_set[error_step]);
		Timer3_init();
		error_step++;
	}
}




//---------------------------------------------------------------------------------------------
void flash_green() {								 //индикатор работы
	if (step_green < DELAY_GREEN_0) {								   // подобрать значение для частоты мигания
		digitalWrite(LED_GREEN, HIGH);
	}
	else if (step_green < DELAY_GREEN_PAUSE)  {
		digitalWrite(LED_GREEN, LOW);
	}
	else if (step_green >= DELAY_GREEN_PAUSE) {
		digitalWrite(LED_GREEN, LOW);
		step_green = 0;
	}
	step_green++;
}


//----------------------------------------------------------------------------------
void write_file() {									 //запись данных в файл на SD-card

	// получение имени текущего дневного файла
	for (int i_file = 0; i_file < file_open_error; i_file++){

		if ((tm.Hour != last_time_hour_3) && (tm.Hour % SD_HOUR = 0)){
			
			last_time_hour_3 = tm.Hour;
			nn1++;
			
		}
		
		YY_1 = String(tmYearToCalendar(tm.Year) - 2000);
		MM_1   = String(tm.Month);
		if (tm.Month<10) {MM_1 = String(0) + MM_1;}
		DD_1   = String(tm.Day);
		if (tm.Day<10) {DD_1 = String(0) + DD_1;}
		NN_1 = String(nn1);
		if (nn1<10) {NN_1 = String(0) + NN_1;}
		TXT_1  = String(".txt");
		name_file = String(YY_1 +  MM_1 + DD_1 + NN_1  + TXT_1);

		unsigned int length_name_file;
		
		length_name_file = name_file.length();
		
		length_name_file = length_name_file + 1;
		
		char name_file_char[length_name_file];

		name_file.toCharArray(name_file_char, length_name_file);
		

		if (myFile.open(name_file_char, O_RDWR | O_CREAT)) {
			
			i_file = file_open_error;
			
			
		}
		
		else {			//если файл с текущим именем не открылся
			
			if (i_file == (file_open_error - 1)){	//если файл с текущим именем не открылся 5 раз
				nn1++;
				i_file =0;	
			}

			if (nn1 > 20){
				
				nn1 = 0;
				// индикация ошибок
				error_SD();
				return;			
			}
			
			delay(10);
			
		}

	}

	


	
	myFile.seekSet(myFile.fileSize());           //установка позиции курсора в конец файла


	uint32_t min_size_ = 10;						//минимальный размер файла в байтах

	if (myFile.fileSize() <= min_size_){		   //запись в файл заголовков столбцов

	
		char charBufVar_1[] = "Час;Мин;Сек;Шаг_регулирования;Temp_C1;Temp_C2;Temp_C3;Temp_C4;Влажность;Temp;Влажность1;Temp1;CO2;Ошибки 1;2;3;4;5;6;7;8;9;10;11;12;13;14;15;День цикла";	// ; x 28
		int size_charBufVar_1 = sizeof(charBufVar_1);

		const char *index_1 = (const char*)charBufVar_1;    //+  
		
		myFile.write(13);
		myFile.write(10);
		myFile.write(index_1, size_charBufVar_1);
		myFile.write(13);
		myFile.write(10);

		
		if (!myFile.timestamp(T_CREATE, tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second)) {  //установка времени создания файла
			error_SD();
			return;
		}
	
	}


	String Write_data;			//буфер для записываемых данных
	


	
	Write_data = String(tm.Hour) + ";"
				+ String(tm.Minute) + ";"
				+ String(tm.Second) + ";"
				+ String(step_regulate_Temp) + ";"	
			+ String(tempC1) + ";"
			 + String(tempC2) + ";"
			 + String(tempC3) + ";"
			 + String(tempC4) + ";"
			 + String(humidity_SHT1x) + ";" 
			+ String(temp_SHT1x) + ";"
			 + String(/*humidity_DHT21_1*/level_Temp_dynamic) + ";"
			 + String(/*temp_c_DHT21_1*/level_Temp * 100L) + ";"
			 + String(ppm) + ";";



	//текущие ошибки
	for (int i = 1; i < ARRAY_LENGH_N; i++) {
		Write_data = Write_data + String(error_array_N[i])+ ";";
	}



	Write_data = Write_data + String(day_cycle);
	

	
	unsigned int length_Write_data;
	
	length_Write_data = Write_data.length();
	
	length_Write_data = length_Write_data + 1;
	
	char charBufVar[length_Write_data];

	Write_data.toCharArray(charBufVar, length_Write_data);
	
	
	//вывод буфера в файл
	const char *index_2 = (const char*)charBufVar;    //+  

	if (myFile.write(index_2, length_Write_data) != length_Write_data) {
		error_SD();
		return;
	}
	
	myFile.write(13);
	myFile.write(10);



	
	  // установка времени изменения файла
	if (!myFile.timestamp(T_WRITE, tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second)) {
		error_SD();
		return;
	}	
	
  
	if (!myFile.close()) {
		error_SD();
		return;
	}


	// при успешном выполнении операции записи
	reboot_SDcard = 0;
	if (error_array_N[error_SDcard] == 1){
		error_array_N[error_SDcard] = 0;
		error();
	}

}


//--------------------------------------------------------------------------------------
void error_SD() {									//обработчик ошибок SDcard
	
	reboot_SDcard++;
	error_array_N[error_SDcard] = 1;
	error();
	
}


//--------------------------------------------------------------------------------------
void post_reboot_system(){							 // установка параметров системы после перезагрузки

	
	for (int i_file = 0; i_file < file_open_error; i_file++){


		// получение имени текущего дневного файла
		YY_1 = String(tmYearToCalendar(tm.Year) - 2000);
		MM_1   = String(tm.Month);
		if (tm.Month<10) {MM_1 = String(0) + MM_1;}
		DD_1   = String(tm.Day);
		if (tm.Day<10) {DD_1 = String(0) + DD_1;}
		NN_1 = String(nn1);
		if (nn1<10) {NN_1 = String(0) + NN_1;}
		TXT_1  = String(".txt");
		name_file = String(YY_1 +  MM_1 + DD_1 + NN_1  + TXT_1);

		unsigned int length_name_file;
		
		length_name_file = name_file.length();
		
		length_name_file = length_name_file + 1;
		
		char name_file_char[length_name_file];

		name_file.toCharArray(name_file_char, length_name_file);
		

		if (myFile.open(name_file_char, O_RDWR | O_CREAT)) {break;}
	
		delay(10);


		if (i_file == (file_open_error - 1)){
			nn1++;
			i_file =0;	
		}

		if (nn1 > 10){
			
			nn1 = 0;
			// индикация ошибок
			error_SD();
			return;			
		}
	}	
	
	


	
	
	uint32_t min_size_ = 10;						//минимальный размер файла в байтах

	if (myFile.fileSize() <= min_size_){		   
		myFile.close();								// сохранить файл и закрыть сеанс записи
		return;
	}
	
	
	int cursor_position;			  //текущая позиция курсора

	int byte_read;					  //текущий прочитанный байт в int

	int symbol_n = 0;				   //символ '\n'   №10

	int symbol_r = 0;				   //символ '\r'   №13

	int symbol_semicolon = 0;		   //символ ';'	   №59

	int num_1 = 0;

	int num_2 = 0;


	myFile.seekSet(myFile.fileSize() - 1);					 //установка позиции курсора


	//проверка структуры файла на ошибочное занесение информации:
	if (myFile.read() == 10) {								 //проверка символа '\n'
		myFile.seekSet(myFile.fileSize() - 2);				 //установка позиции курсора
		if (myFile.read() == 13) {							 //проверка символа '\r'

			for (int i4 = 1; i4 <= 200; i4++){

				myFile.seekSet(myFile.fileSize() - i4);					  //установка позиции курсора

				byte_read = myFile.read();
				
				if (byte_read == 10){symbol_n++;}
				if (byte_read == 13){symbol_r++;}
				if (byte_read == 59){symbol_semicolon++;}

				if ((symbol_n == 2) && (symbol_r == 2)){  //при достижении второго переноса строки
					i4 = 201;
				}
			}
		}
	wdt_reset();	 //сброс WatchDog таймера

	}
	else {
		
		for (int i4 = 1; i4 <= 200; i4++){

			myFile.seekSet(myFile.fileSize() - i4);					  //установка позиции курсора

			byte_read = myFile.read();

			if (byte_read == 10){symbol_n++;}
			if (byte_read == 13){symbol_r++;}
			if (byte_read == 59){symbol_semicolon++;}

			if ((symbol_n == 1) && (symbol_r == 1)){  //при достижении первого переноса строки
				i4 = 201;
			}

		}
	wdt_reset();	 //сброс WatchDog таймера

	}


	if (symbol_semicolon != 28){		  //если неправильная структура данных (символов ";" в строке д.б. 28 шт.)


		day_cycle = 1;
		
		String Write_data;			//буфер для записываемых данных
	
		Write_data = "Включение... Ошибка структуры файла. День цикла: ";
	
		for (int i = 1; i <= 28; i++) {
			Write_data = Write_data + ";";
		}

		Write_data = Write_data + String(day_cycle);
		

		
		unsigned int length_Write_data;
		
		length_Write_data = Write_data.length();
		
		length_Write_data = length_Write_data + 1;
		
		char charBufVar[length_Write_data];

		Write_data.toCharArray(charBufVar, length_Write_data);
		
		
		//вывод буфера в файл
		const char *index_1 = (const char*)charBufVar;    //+  
		
		myFile.write(13);
		myFile.write(10);
		myFile.write(index_1, length_Write_data);
		myFile.write(13);
		myFile.write(10);
			
		myFile.close();

		delay(50);
		return;
	}








	for (int i4 = 1; i4 <= 100; i4++){

		myFile.seekSet(myFile.fileSize() - i4);					  //установка позиции курсора

		byte_read = myFile.read();

		if (byte_read == 59){		   //при достижении ';'
			cursor_position = myFile.fileSize() - i4 + 1;
			i4 = 101;
		}

	}

	myFile.seekSet(cursor_position);				   //установка позиции курсора
	num_1 = myFile.read();

	myFile.seekSet(cursor_position + 1);		  //установка позиции курсора
	num_2 = myFile.read();



	if (num_1 > 47 && num_1 < 58 ) {

		switch (num_1) {
			case 48:
			num_1 = 0;
			break;
			case 49:
			num_1 = 1;
			break;
			case 50:
			num_1 = 2;
			break;
			case 51:
			num_1 = 3;
			break;
			case 52:
			num_1 = 4;
			break;
			case 53:
			num_1 = 5;
			break;
			case 54:
			num_1 = 6;
			break;
			case 55:
			num_1 = 7;
			break;
			case 56:
			num_1 = 8;
			break;
			case 57:
			num_1 = 9;
			break;
		}

	}
	else {
		num_1 = -1;
	}


	if (num_2 > 47 && num_2 < 58) {

		switch (num_2) {
			case 48:
			num_2 = 0;
			break;
			case 49:
			num_2 = 1;
			break;
			case 50:
			num_2 = 2;
			break;
			case 51:
			num_2 = 3;
			break;
			case 52:
			num_2 = 4;
			break;
			case 53:
			num_2 = 5;
			break;
			case 54:
			num_2 = 6;
			break;
			case 55:
			num_2 = 7;
			break;
			case 56:
			num_2 = 8;
			break;
			case 57:
			num_2 = 9;
			break;
		}

	}
	else {
		num_2 = -1;
	}


	if (num_1 >= 0 && num_2 < 0){
		day_cycle = num_1;
	}
	
	
	if (num_1 >= 0 && num_2 >= 0){
		day_cycle = (num_1 * 10L) + num_2;
	}
	
	
	if (num_1 < 0){
		day_cycle = 1;
	}
	
	if (day_cycle > CYCLE_ARRAY_LENGTH){
		day_cycle = 1;
	}	
	
	

	//установка текущего дня цикла в массиве			  (0 - первый день в массиве)
	for (int i = 0; i < day_cycle; i++) {
		cycle_array_day[i] = 1;
	}


	level_Temp	= cycle_array_temp[day_cycle - 1];		// температура в камере на текущий день
	level_CO2	= cycle_array_CO2[day_cycle - 1];		// уровень СО2 в камере на текущий день
	level_hidro = cycle_array_hidro[day_cycle - 1];		// влажность в камере на текущий день
	
	
	
	

	myFile.seekSet(myFile.fileSize());           //установка позиции курсора в конец файла

	wdt_reset();	 //сброс WatchDog таймера

	String Write_data_1;			//буфер для записываемых данных

	Write_data_1 = "Включение... День цикла: ";

	for (int i = 1; i <= 28; i++) {
		Write_data_1 = Write_data_1 + ";";
	}

	Write_data_1 = Write_data_1 + String(day_cycle);
	

	
	unsigned int length_Write_data_1;
	
	length_Write_data_1 = Write_data_1.length();
	
	length_Write_data_1 = length_Write_data_1 + 1;
	
	char charBufVar_1[length_Write_data_1];

	Write_data_1.toCharArray(charBufVar_1, length_Write_data_1);
	
	
	//вывод буфера в файл
	const char *index_2 = (const char*)charBufVar_1;    //+  
	
	myFile.write(13);
	myFile.write(10);
	myFile.write(index_2, length_Write_data_1);
	myFile.write(13);
	myFile.write(10);

	  // установка времени изменения файла
	if (!myFile.timestamp(T_WRITE, tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second)) {
		error_SD();
		return;
	}	
	
  
	if (!myFile.close()) {
		error_SD();
		return;
	}


	// при успешном выполнении операции записи
	reboot_SDcard = 0;
	if (error_array_N[error_SDcard] == 1){
		error_array_N[error_SDcard] = 0;
		error();
	}

}


//--------------------------------------------------------------------------------------
void crash_RCT() {									 // аварийный режим при неработающих часах

	tm.Year 	= 1;
	tm.Month 	= 1;
	tm.Day 		= 1;
	tm.Hour 	= 1;
	tm.Minute 	= 1;
	tm.Second 	= 1;
	
}


//------------------------------------------------------------
void regulate_CO2() {								 //регулирование уровня СО2

	if (door_position == DOOR_OPEN) {
		return;
	}


	if (error_array_N[error_MHZ19] == 0) {			//если нет ошибок по датчику СО2, продолжаем

	
		if (ppm > (level_CO2 + 50L)) {		//если уровень СО2 ВЫШЕ заданного на 50ррм
			digitalWrite(RELE_CO2, HIGH);
			DC_Motor_Go(open_CO2);
		}

		if (ppm < (level_CO2 - 20L)) {		//если уровень СО2 НИЖЕ заданного на 20ррм
			DC_Motor_Go(close_CO2);
			digitalWrite(RELE_CO2, LOW);
		}
		
	}

	else {
		digitalWrite(RELE_CO2, LOW);
		DC_Motor_Go(open_CO2);
	}
}


//------------------------------------------------------------
void regulate_hidro() {								 //регулирование уровня влажности

	if (door_position == DOOR_OPEN) {
		return;
	}


	if (!error_array_N[error_SHT1x]) {			//если нет ошибок по датчику


	
		if (humidity_SHT1x > (level_hidro * 100L + 50L)) {			//если влажность отличается от заданной более чем на 0,5% в большую сторону
			digitalWrite(RELE_HIDRO, LOW);
			hidro_in_timeperiod = 3L;
			hidro_iter_1 = 0;
			hidro_iter_2 = 0;
		}

		if (humidity_SHT1x < (level_hidro * 100L - 150L)) {			//если влажность отличается от заданной более чем на 1,5% в меньшую сторону

			if ((millis() % (timeperiod * 1000L) <= hidro_in_timeperiod*1000L) && (digitalRead(RELE_HIDRO) == LOW)) {
				digitalWrite(RELE_HIDRO, HIGH);
				hidro_iter_1 ++;
			}
			if ((millis() % (timeperiod * 1000L) > hidro_in_timeperiod*1000L) && (digitalRead(RELE_HIDRO) == HIGH)) {
				digitalWrite(RELE_HIDRO, LOW);
			}

			if (hidro_iter_1 == (hidro_iter_2 + 2)){				//увеличиваем период работу УЗУ, если не хватает мощности УЗУ
				if (hidro_in_timeperiod < timeperiod){
					hidro_in_timeperiod = hidro_in_timeperiod + 2L;
				}

				hidro_iter_2 = hidro_iter_1;

			}


		}
	}

	else {										// если ошибки в работе датчика

		if (tempC4 <= 0L) {
			digitalWrite(RELE_HIDRO, LOW);
		}

		if (tempC4 > 0L && tempC4 < 2000L) {
			if ((millis() % (timeperiod * 1000L) <= hidro_in_timeperiod*1000L) && (digitalRead(RELE_HIDRO) == LOW)) {
				digitalWrite(RELE_HIDRO, HIGH);
			}
			if ((millis() % (timeperiod * 1000L) > hidro_in_timeperiod*1000L) && (digitalRead(RELE_HIDRO) == HIGH)) {
				digitalWrite(RELE_HIDRO, LOW);
			}
		}

		if (tempC4 > 2000L) {
			if ((millis() % (2L * timeperiod * 1000L) <= hidro_in_timeperiod*1000L) && (digitalRead(RELE_HIDRO) == LOW)) {
				digitalWrite(RELE_HIDRO, HIGH);
			}
			if ((millis() % (2L * timeperiod * 1000L) > hidro_in_timeperiod*1000L) && (digitalRead(RELE_HIDRO) == HIGH)) {
				digitalWrite(RELE_HIDRO, LOW);
			}
		}

	}

}


//--------------------------------------------------------------
void regulate_Temp() {								 //регулирование температуры в камере


	if (door_position == DOOR_OPEN) {
		return;
	}

	
	//регулирование температуры в камере
	if ((error_array_N[error_SHT1x] == 0) && (error_array_N[error_DS18b20] == 0)) {		 //если нет ошибок по датчику SHT1x и по датчикам DS18b20


	
	
		//изменение температуры в камере не более, чем на 2°С в час:
		if (abs(temp_SHT1x - (level_Temp*100L)) > (2L * 100L)) {
			
			
			if (millis() < last_time_sec_11) {					// при переполнении millis()
				last_time_sec_11 = 0;    
			}
	
			if ((millis() - last_time_sec_11) > (1L*60L*60L*1000L)) {			  //корректировка каждый час

				if (temp_SHT1x > level_Temp*100L) {
					level_Temp_dynamic = temp_SHT1x - (2L * 100L);
				}
				else {
					level_Temp_dynamic = temp_SHT1x + (2L * 100L);
				}

				last_time_sec_11 = millis();
			}

		}
		else {
			level_Temp_dynamic = level_Temp * 100L;
		}




		if (temp_SHT1x > (level_Temp_dynamic + 50L)) {		//при превышении температуры больше, чем на 0,5°С от заданной

			step_regulate_Temp --;

			if (step_regulate_Temp < 0) {
				step_regulate_Temp = 0;
			}


		}


		if (temp_SHT1x < (level_Temp_dynamic - 50L)) {	   //при понижении температуры больше, чем на 0,5°С от заданной

			step_regulate_Temp ++;

			if (step_regulate_Temp > 35) {
				step_regulate_Temp = 35;
			}

			


		}

		int pos = array_step_piosition[step_regulate_Temp];
		
		StepMotor_Go(pos);
		
		
	}


	else {											  // если ошибки в работе датчиков DS18b20 и SHT1x
		StepMotor_Go(1);
		step_regulate_Temp = 17;
	}

}


//-------------------------------------------------------------
void regulate_Hot_Cold() {							 // регулирование работы морозильника и нагревателя


	if (door_position == DOOR_OPEN) {
		return;
	}


	if (!error_array_N[error_DS18b20]) {		 //если нет ошибок по датчикам DS18b20
	
		level_Temp_1 = array_step_Temp[step_regulate_Temp] * 100L;
		
		if (level_Temp_1 > (temp_SHT1x + 50L)) {			 //если заданная температура выше температуры в камере +0,5°С
			digitalWrite(RELE_COLD, LOW);				 //отключение морозильника
			if (tempC2 >= level_Temp_1) {
				digitalWrite(RELE_HOT, LOW);			 //вЫключение нагревателя
			}
			else {
				digitalWrite(RELE_HOT, HIGH);			 //вКючение нагревателя
			}
		}

		else if ((level_Temp_1 <= (temp_SHT1x + 50L)) && (level_Temp_1 >= (temp_SHT1x - 50L))) {	 //если заданная температура ±0,5°С от температуры камеры
			digitalWrite(RELE_COLD, LOW);				 //отключение морозильника
			digitalWrite(RELE_HOT, LOW);				 //отключение нагревателя
		}

		else if (level_Temp_1 < (temp_SHT1x - 50L)) {		 //если заданная температура ниже температуры в камере -0,5°С
			digitalWrite(RELE_HOT, LOW);				 //отключение нагревателя
			if (tempC3 >= level_Temp_1) {
				digitalWrite(RELE_COLD, HIGH);			 //вКлючение морозильника
			}
			else {
				digitalWrite(RELE_COLD, LOW);			 //вЫкючение морозильника
			}
		}
	} 
	
	else {
		digitalWrite(RELE_COLD, LOW);			 //вЫкючение морозильника
		digitalWrite(RELE_HOT, LOW);			 //вЫключение нагревателя
	}
	

}


//------------------------------------------------------------------------
void regulate_water() {								 //регулирование уровня воды в емкости УЗУ


	
	
	if (digitalRead(BUTTON_PIN_WATER_) == LOW) {	  // если уровень воды начал СНИЖАТЬСЯ

		pump_step_1++;
		
		if (pump_step_1 >= 2){			//при двухкратном прохождении очереди задач (защита от дребезга)
		
			water_level = WATER_LOW;
			
			pump_step_2 = 0;			//обнуляем цикл высокого уровня воды
		}	
	}

	
	if (pump_step_1 >= 66) {       // если уровень воды не поднимается более 20 сек. (~300мс х 66)
		digitalWrite(RELE_PUMP, LOW);

		error_array_N[error_pump]++;
		error();

		pump_step_1 = 2;			//продолжаем отсчет до следующей ошибки
		
	}

	
	if (digitalRead(BUTTON_PIN_WATER_) == HIGH) {	  // если уровень воды начал ПОВЫШАТЬСЯ

		pump_step_2++;
		
		if (pump_step_2 >= 2){			//при двухкратном прохождении очереди задач (защита от дребезга)
		
			water_level = WATER_NORM;
			
			pump_step_1 = 0;			//обнуляем цикл низкого уровня воды
			pump_step_2 = 2;			//чтобы pump_step_2 не набирала большие значения
			
			if (error_array_N[error_pump] > 0){
				error_array_N[error_pump] = 0;
				error();
			}
		}	
	}

	
	
	
	if (error_array_N[error_pump] <= 2){		//если ошибок по уровню воды не более 2, управляем насосом
		
		if (water_level == WATER_LOW) {			//если уровень воды в емкости УЗУ НИЗКИЙ

			digitalWrite(RELE_PUMP, HIGH);

		} else {
			digitalWrite(RELE_PUMP, LOW);
		}
		
	} else {									//если ошибка по УЗУ, насос выключаем
		digitalWrite(RELE_PUMP, LOW);
	}
	
}


//------------------------------------------------------------------------
void regulate_light() {								 //регулирование освещенности


	if (door_position == DOOR_OPEN) {
		return;
	}
		
		
	if (!error_array_N[error_RCT]) {		  //если нет ошибок по часам

	
		// освещение с 09:00 до 18:00 вКлючено
		if (cycle_array_ligh[day_cycle] && tm.Hour >= 9 && tm.Hour < 18 &&	(digitalRead(RELE_LIGHT) == LOW)) {
			digitalWrite(RELE_LIGHT, HIGH);
		} else if (cycle_array_ligh[day_cycle] && tm.Hour < 9 && tm.Hour >= 18 && (digitalRead(RELE_LIGHT) == HIGH)) {
			digitalWrite(RELE_LIGHT, LOW);
		}

	} else {
		digitalWrite(RELE_LIGHT, LOW);
	}

}


//--------------------------------------------
//	функции Асинхронного таймера

void TTimerList::Init(){

	cli();

	TCCR2A = 0;
	TCCR2B = 0;
	TCCR2A |= (1 << WGM21);									 // Режим CTC (сброс по совпадению)
	//Преддилитель
	//TCCR2B |= (1 << CS20);						  // CLK/1
	//TCCR2B = (1<<CS21);							  // CLK/8
	//TCCR2B = (1<<CS20)|(1<<CS21);					  // CLK/32
	//TCCR2B = (1<<CS22);							  // CLK/64
	TCCR2B = (1<<CS20)|(1<<CS22);					// CLK/128
	//TCCR2B = (1<<CS21)|(1<<CS22);					  // CLK/256
	//TCCR2B = (1<<CS20)|(1<<CS21)|(1<<CS22);		  // CLK/1024

	// Верхняя граница счета. Диапазон от 0 до 255.
	OCR2A = _1MSCONST * 124;										// Частота прерываний A будет = Fclk/(N*(1+OCR2A))	  x	 =	мс
	//OCR2B = 249;													   // Частота прерываний B будет = Fclk/(N*(1+OCR2B))
	// где N - коэф. предделителя (1, 8, 32, 64, 128, 256 или 1024)
	TIMSK2 |= (1 << OCIE2A);								  // Разрешить прерывание по совпадению A
	//TIMSK2 |= (1 << OCIE2B);									 // Разрешить прерывание по совпадению B
	//TIMSK2 |= (1 << TOIE2);									 // Разрешить прерывание по переполнению

	sei();
}

TTimerList::TTimerList(){

	count = 0;
	for (byte i = 0; i < MAXTIMERSCOUNT; i++)  // первоначальная инициализация, заполнение списка нулями
	{
		Items[i].CallingFunc	= NULL;
		Items[i].InitCounter	= 0;
		Items[i].WorkingCounter = 0;
		Items[i].Active			 = false;
	}
	active = false;
}

THandle TTimerList::Add(PVoidFunc AFunc, long timeMS){

	for (THandle i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (Items[i].CallingFunc == NULL)		  // пробегаемся по списку таймеров
		{										  // если нашли пустую дырку, добавляем таймер
			cli();
			Items[i].CallingFunc	= AFunc;
			Items[i].InitCounter	= timeMS;
			Items[i].WorkingCounter = timeMS;
			Items[i].Active			 = true;
			count++;
			if (NOT active)	 Start();
			sei();
			return i;
		}
	}
	return -1;									 // если нет - вернем код ошибки (-1)
}

THandle TTimerList::AddSeconds(PVoidFunc AFunc, word timeSec){

	return Add(AFunc, 1000L*timeSec);		   // оналогично для секунд
}

THandle TTimerList::AddMinutes(PVoidFunc AFunc, word timeMin){
	return Add(AFunc, timeMin*60L*1000L);	  // оналогично для минут
}

bool TTimerList::CanAdd(){
	for (byte i = 0; i < MAXTIMERSCOUNT; i++)
	{
		if (Items[i].CallingFunc != NULL) continue;	 // если в списке есть пустые места, куда можно добавить таймер
		return true;								 // вернем true
	}
	return false;									 // если нет - то false
}

bool TTimerList::IsActive(){
	return active;									 // если хоть один таймер активен, вернет true, если все остановлены - false
}

void TTimerList::Delete(THandle hnd){				  // удалить таймер с хэндлом hnd.

	if (InRange(hnd, 0, MAXTIMERSCOUNT - 1))
	{
		cli();
		Items[hnd].CallingFunc		= NULL;
		Items[hnd].InitCounter		= 0;
		Items[hnd].WorkingCounter	= 0;
		Items[hnd].Active			= false;
		if (count > 0)	count--;
		if (count==0) Stop();						 // если все таймеры удалены, остановить и цикл перебора таймеров
		sei();
	}
}

/// эта функция вызывается при аппаратном срабатывании таймера каждую миллисекунду
/// запуская цикл декремента интервалов срабатывания

void TTimerList::Step(void){
	if (NOT active) return;							// если все таймеры остановлены, то и смысла нет
	cli();											// чтоб никто не помешал изменить интервалы, запрещаем прерывания
// сопсно рабочий цикл
	for (THandle i = 0; i < MAXTIMERSCOUNT; i++)   // пробегаем по всему списку таймеров
	{
		if (Items[i].CallingFunc == NULL)  continue;  // если функция-обрабоччик не назначена, уходим на следующий цикл
		if (NOT Items[i].Active)		   continue; // если таймер остановлен - тоже
		if (--Items[i].WorkingCounter > 0) continue;  // уменьшаем на 1 рабочий счетчик
		Items[i].CallingFunc();						  // если достиг 0, вызываем функцию-обработчик
		Items[i].WorkingCounter = Items[i].InitCounter; // и записываем в рабочий счетчик начальное значение для счета сначала
	}
	sei();											  // теперь и прерывания можно разрешить
}

void TTimerList::Start(){
	if (NOT active) Init();								 // при добавлении первого таймера, инициализируем аппаратный таймер
	active = true;
}

void TTimerList::Stop(){
	active = false;									  // остановить все таймеры
}

byte TTimerList::Count(){
	return count;									  // счетчик добавленных таймеров
}

void TTimerList::TimerStop(THandle hnd){
	if (InRange(hnd, 0, MAXTIMERSCOUNT-1))
	{
		Items[hnd].Active = false;					  // остановить таймер номер hnd
	}
}

bool TTimerList::TimerActive(THandle hnd){
	if (NOT InRange(hnd, 0, MAXTIMERSCOUNT - 1)) return false;
	else return Items[hnd].Active;
}

void TTimerList::TimerStart(THandle hnd){			   // запустить остановленный таймер номер hnd

	if (NOT InRange(hnd,0,MAXTIMERSCOUNT-1)) return;
	if (Items[hnd].CallingFunc == NULL)		 return;
	if (Items[hnd].Active)					 return;
	cli();
	Items[hnd].WorkingCounter = Items[hnd].InitCounter;	 // и начать отсчет интервала сначала
	Items[hnd].Active = true;
	sei();
}

void TTimerList::TimerNewInterval(THandle hnd, long newinterval){	// новый интервал таймера hnd
	if (NOT InRange(hnd, 0, MAXTIMERSCOUNT - 1)) return;
	cli();
	Items[hnd].InitCounter = newinterval;
	Items[hnd].WorkingCounter = newinterval;
	if (NOT Items[hnd].Active) TimerStart(hnd);
	sei();
}

 // обработчик прерывания аппаратного таймера
ISR(TIMER2_COMPA_vect){

	TimerList.Step();
	
}

bool InRange(int value, int min, int max){				 // отдает true, если value лежит в диапазоне от min до max (включительно)

	return (value >= min) AND (value <= max);
}





//--------------------------------------------------------------------
void reboot_system() {								//перезагрузка переферии при критических ошибках



	//при зависании датчика DHT21 более 5 мин или неправильных показаниях (вл.<10% и темп.>100°C)
	if ((humidity_DHT21_1 < 1000L) || (humidity_DHT21_1 > 10000L) || error_array_N[error_DHT21]){

		if (reboot_DHT21 == 0) {
			last_time_sec_5 = millis();
			reboot_DHT21 = 1;
		}
		
		if (millis() < last_time_sec_5) {
			last_time_sec_5 = 0;
		}

		if (((millis() - last_time_sec_5) > 5L * 60L * 1000L) && (reboot_DHT21 == 1)){

			
			for (int i_file = 0; i_file < 5; i_file++){


				// получение имени текущего дневного файла
				YY_1 = String(tmYearToCalendar(tm.Year) - 2000);
				MM_1   = String(tm.Month);
				if (tm.Month<10) {MM_1 = String(0) + MM_1;}
				DD_1   = String(tm.Day);
				if (tm.Day<10) {DD_1 = String(0) + DD_1;}
				NN_1 = String(nn1);
				if (nn1<10) {NN_1 = String(0) + NN_1;}
				TXT_1  = String(".txt");
				name_file = String(YY_1 +  MM_1 + DD_1 + NN_1  + TXT_1);

				unsigned int length_name_file;
				
				length_name_file = name_file.length();
				
				length_name_file = length_name_file + 1;
				
				char name_file_char[length_name_file];

				name_file.toCharArray(name_file_char, length_name_file);
				
				if (myFile.open(name_file_char, O_RDWR | O_CREAT)) {break;}
			
				delay(10);


				if (i_file == 4){
					nn1++;
					i_file =0;	
				}

				if (nn1 > 10){
					
					nn1 = 0;
					// индикация ошибок
					error_SD();
								
				}
			}

			if (error_array_N[error_SDcard] == 0) {
				
				myFile.seekSet(myFile.fileSize());           //установка позиции курсора в конец файла




				// запись данных:
				String Write_data_1;			//буфер для записываемых данных

				Write_data_1 = "Перезагрузка по DHT21... День цикла: ";

				for (int i = 1; i <= 28; i++) {
					Write_data_1 = Write_data_1 + ";";
				}

				Write_data_1 = Write_data_1 + String(day_cycle);
				

				
				unsigned int length_Write_data_1;
				
				length_Write_data_1 = Write_data_1.length();
				
				length_Write_data_1 = length_Write_data_1 + 1;
				
				char charBufVar_1[length_Write_data_1];

				Write_data_1.toCharArray(charBufVar_1, length_Write_data_1);
				
				
				//вывод буфера в файл
				const char *index_1 = (const char*)charBufVar_1;    //+  
				
				myFile.write(13);
				myFile.write(10);
				myFile.write(index_1, length_Write_data_1);
				myFile.write(13);
				myFile.write(10);
					
					
					
					

				// установка времени изменения файла
				if (!myFile.timestamp(T_WRITE, tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second)) {
					error_SD();
				}	


				myFile.close();											  // сохранить файл и закрыть сеанс записи
			}



			reboot_5V();		//перезагрузка переферии по питанию (датчиков, часов, SD-card, концевиков)
			return;
		}

	}
	
	else {
	reboot_DHT21 = 0;
	}





	//при зависании датчика SHT1x более 5 мин или неправильных показаниях (вл.<10% и темп.>100°C)
	if ((humidity_SHT1x < 1000L) || (humidity_SHT1x > 10000L) || error_array_N[error_SHT1x]){

		if (reboot_SHT1x == 0) {
			last_time_sec_8 = millis();
			reboot_SHT1x = 1;
		}
		
		if (millis() < last_time_sec_8) {
			last_time_sec_8 = 0;
		}

		if (((millis() - last_time_sec_8) > 5L * 60L * 1000L) && (reboot_SHT1x == 1)){


			for (int i_file = 0; i_file < 5; i_file++){


				// получение имени текущего дневного файла
				YY_1 = String(tmYearToCalendar(tm.Year) - 2000);
				MM_1   = String(tm.Month);
				if (tm.Month<10) {MM_1 = String(0) + MM_1;}
				DD_1   = String(tm.Day);
				if (tm.Day<10) {DD_1 = String(0) + DD_1;}
				NN_1 = String(nn1);
				if (nn1<10) {NN_1 = String(0) + NN_1;}
				TXT_1  = String(".txt");
				name_file = String(YY_1 +  MM_1 + DD_1 + NN_1  + TXT_1);

				unsigned int length_name_file;
				
				length_name_file = name_file.length();
				
				length_name_file = length_name_file + 1;
				
				char name_file_char[length_name_file];

				name_file.toCharArray(name_file_char, length_name_file);
				
				if (myFile.open(name_file_char, O_RDWR | O_CREAT)) {break;}
			
				delay(10);


				if (i_file == 4){
					nn1++;
					i_file =0;	
				}

				if (nn1 > 10){
					
					nn1 = 0;
					// индикация ошибок
					error_SD();
							
				}
			}
			
			if (error_array_N[error_SDcard] == 0) {
				myFile.seekSet(myFile.fileSize());           //установка позиции курсора в конец файла




				String Write_data_1;			//буфер для записываемых данных

				Write_data_1 = "Перезагрузка по SHT1x... День цикла: ";

				for (int i = 1; i <= 28; i++) {
					Write_data_1 = Write_data_1 + ";";
				}

				Write_data_1 = Write_data_1 + String(day_cycle);
				

				
				unsigned int length_Write_data_1;
				
				length_Write_data_1 = Write_data_1.length();
				
				length_Write_data_1 = length_Write_data_1 + 1;
				
				char charBufVar_1[length_Write_data_1];

				Write_data_1.toCharArray(charBufVar_1, length_Write_data_1);
				
				
				//вывод буфера в файл
				const char *index_1 = (const char*)charBufVar_1;    //+  
				
				myFile.write(13);
				myFile.write(10);
				myFile.write(index_1, length_Write_data_1);
				myFile.write(13);
				myFile.write(10);
					
					
					
					

				// установка времени изменения файла
				if (!myFile.timestamp(T_WRITE, tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second)) {
					error_SD();
				}	


				myFile.close();											  // сохранить файл и закрыть сеанс записи

			}


			reboot_5V();		//перезагрузка переферии по питанию (датчиков, часов, SD-card, концевиков)
			return;

		}
	}
	
	else {
	reboot_SHT1x = 0;
	}






	//при зависании датчика MHZ19 более 5 мин. или неправильных показаниях (ppm<100 или ppm>3900°C)
	if ((ppm < 100L) || (ppm > 3900L) || error_array_N[error_MHZ19]){

		if (reboot_MHZ19 == 0) {
			last_time_sec_7 = millis();
			reboot_MHZ19 = 1;
		}
		
		if (millis() < last_time_sec_7) {
			last_time_sec_7 = 0;
		}

		if (((millis() - last_time_sec_7) > 5L * 60L * 1000L) && (reboot_MHZ19 == 1)){



			for (int i_file = 0; i_file < 5; i_file++){


				// получение имени текущего дневного файла
				YY_1 = String(tmYearToCalendar(tm.Year) - 2000);
				MM_1   = String(tm.Month);
				if (tm.Month<10) {MM_1 = String(0) + MM_1;}
				DD_1   = String(tm.Day);
				if (tm.Day<10) {DD_1 = String(0) + DD_1;}
				NN_1 = String(nn1);
				if (nn1<10) {NN_1 = String(0) + NN_1;}
				TXT_1  = String(".txt");
				name_file = String(YY_1 +  MM_1 + DD_1 + NN_1  + TXT_1);

				unsigned int length_name_file;
				
				length_name_file = name_file.length();
				
				length_name_file = length_name_file + 1;
				
				char name_file_char[length_name_file];

				name_file.toCharArray(name_file_char, length_name_file);
				
				if (myFile.open(name_file_char, O_RDWR | O_CREAT)) {break;}
			
				delay(10);


				if (i_file == 4){
					nn1++;
					i_file =0;	
				}

				if (nn1 > 10){
					
					nn1 = 0;
					// индикация ошибок
					error_SD();
					return;			
				}
			}
			
			if (error_array_N[error_SDcard] == 0) {
				myFile.seekSet(myFile.fileSize());           //установка позиции курсора в конец файла




				String Write_data_1;			//буфер для записываемых данных

				Write_data_1 = "Перезагрузка по MHZ19... День цикла: ";

				for (int i = 1; i <= 28; i++) {
					Write_data_1 = Write_data_1 + ";";
				}

				Write_data_1 = Write_data_1 + String(day_cycle);
				

				
				unsigned int length_Write_data_1;
				
				length_Write_data_1 = Write_data_1.length();
				
				length_Write_data_1 = length_Write_data_1 + 1;
				
				char charBufVar_1[length_Write_data_1];

				Write_data_1.toCharArray(charBufVar_1, length_Write_data_1);
				
				
				//вывод буфера в файл
				const char *index_1 = (const char*)charBufVar_1;    //+  
				
				myFile.write(13);
				myFile.write(10);
				myFile.write(index_1, length_Write_data_1);
				myFile.write(13);
				myFile.write(10);
					
					
					
					

				// установка времени изменения файла
				if (!myFile.timestamp(T_WRITE, tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second)) {
					error_SD();
				}
				


				myFile.close();											  // сохранить файл и закрыть сеанс записи
			}


			reboot_5V();		//перезагрузка переферии по питанию (датчиков, часов, SD-card, концевиков)
			return;
		}

	}
	
	else {
	reboot_MHZ19 = 0;
	}





	//при зависании SDcard
	if (reboot_SDcard > 10){

			reboot_5V();		//перезагрузка переферии по питанию (датчиков, часов, SD-card, концевиков)
			return;
	}







}


void reboot_5V(){									// перезагрузка переферии по питанию (датчиков, часов, SD-card, концевиков)
	
	//ждем окончание движения моторов
	while (digitalRead(DC_MOTOR_1_A) || digitalRead(DC_MOTOR_1_B)){
		delay(100);
	}

	while (steps_left > 0){
		delay(100);
	}


	
	//отключение питания 5В датчиков, часов, SD-card, концевиков
	digitalWrite(RELE_5V, HIGH);
	
	delay(2000);
	
	digitalWrite(RELE_5V, LOW);
	
	delay(3000);
	
	
	error_array_N[error_RCT] = 0;
	error_array_N[error_DHT21] = 0;
	error_array_N[error_MHZ19] = 0;
	error_array_N[error_DS18b20] = 0;
	error_array_N[error_SHT1x] = 0;


	
	cli();			 //вЫкючаем прерывания

	int i1;
	int i2 = 0;

	for (i1 = 1; i1 < ARRAY_LENGH_N; i1++) {

		i2 = i2 + error_array_N[i1];	//проверка наличия оставшихся ошибок

	}
	
	if (i2 > 0){
		
		error();
		
	}
	else{
		
		// остановить таймер, погасить индикатор ошибок
		TCCR3B = 0;
		TCCR3A = 0;
		TCNT3 = 0;
		digitalWrite(LED_RED, LOW);
		
	}
	

	sei();				 //вКлючаем прерывания

	
	
	
	if (!sd.begin(SD_CHIP_SELECT_PIN, SPI_HALF_SPEED)) {			  // проверка на ошибки SD-card


		int i_sd = 0;
		while (!sd.begin(SD_CHIP_SELECT_PIN, SPI_HALF_SPEED)){
			i_sd++;
			delay(100);

			if (i_sd > 5) {  //при превышении числа ошибок
			
				// индикация ошибок
				error_SD();
				return;
			}
		}
	}

	
	
	sensors.begin();								// готовность датчиков DS18b10
	sensors.setResolution(insideThermometer1, 10);	// устанавливаем разрешение датчика на 10 бит (либо 9 бит)
	sensors.setResolution(insideThermometer2, 10);	// устанавливаем разрешение датчика на 10 бит (либо 9 бит)
	sensors.setResolution(insideThermometer3, 10);	// устанавливаем разрешение датчика на 10 бит (либо 9 бит)
	sensors.setResolution(insideThermometer4, 10);	// устанавливаем разрешение датчика на 10 бит (либо 9 бит)


	
}


void power_down() {									// ОТКЛЮЧЕНИЕ питания всей системы при превышении температуры 120°С по любому датчику


	if ((tempC1 > 5000) || (tempC2 > 12000) || (tempC3 > 5000) || (tempC4 > 5000) || (temp_SHT1x > 5000) ){

	
		digitalWrite(RELE_POWER, LOW);  //отключение питания


	
	
		//запись на SD-card на заряде конденсаторов БП:
		for (int i_file = 0; i_file < 5; i_file++){


			// получение имени текущего дневного файла
			YY_1 = String(tmYearToCalendar(tm.Year) - 2000);
			MM_1   = String(tm.Month);
			if (tm.Month<10) {MM_1 = String(0) + MM_1;}
			DD_1   = String(tm.Day);
			if (tm.Day<10) {DD_1 = String(0) + DD_1;}
			NN_1 = String(nn1);
			if (nn1<10) {NN_1 = String(0) + NN_1;}
			TXT_1  = String(".txt");
			name_file = String(YY_1 +  MM_1 + DD_1 + NN_1  + TXT_1);

			unsigned int length_name_file;
			
			length_name_file = name_file.length();
			
			length_name_file = length_name_file + 1;
			
			char name_file_char[length_name_file];

			name_file.toCharArray(name_file_char, length_name_file);
			
			if (myFile.open(name_file_char, O_RDWR | O_CREAT)) {break;}
		
			delay(5);


		}

		if (error_array_N[error_SDcard] == 0) {
			
			myFile.seekSet(myFile.fileSize());           //установка позиции курсора в конец файла

			// запись данных:
			String Write_data_1;			//буфер для записываемых данных

			Write_data_1 = "Аварийное выключение. Превышение температуры по датчикам";

			for (int i = 1; i <= 28; i++) {
				Write_data_1 = Write_data_1 + ";";
			}

			Write_data_1 = Write_data_1 + String(day_cycle);
			

			
			unsigned int length_Write_data_1;
			
			length_Write_data_1 = Write_data_1.length();
			
			length_Write_data_1 = length_Write_data_1 + 1;
			
			char charBufVar_1[length_Write_data_1];

			Write_data_1.toCharArray(charBufVar_1, length_Write_data_1);
			
			
			//вывод буфера в файл
			const char *index_1 = (const char*)charBufVar_1;    //+  
			
			myFile.write(13);
			myFile.write(10);
			myFile.write(index_1, length_Write_data_1);
			myFile.write(13);
			myFile.write(10);
				
				
				
				

			// установка времени изменения файла
			if (!myFile.timestamp(T_WRITE, tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second)) {
				error_SD();
			}	


			myFile.close();											  // сохранить файл и закрыть сеанс записи
		}

	}
	
}



//----End----
