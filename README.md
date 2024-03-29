Данный проект разрабатывался для управления климатической установкой широкого назначения (например, для выращивания грибов или зелени).
Проект выполнен на базе устройства Arduino Mega 2560 (на основе микроконтроллера ATmega2560).

Функционал, заложенный в программе:
 - измерение текущей температуры в 6 точках (четыре датчика DS18B20, один датчик DHT 21, один датчик SHT11)
 - измерение текущей влажности в 2 точках (один датчик DHT 21, один датчик SHT11)
 - измерение текущего уровня углекислого газа (датчик CO2  MH-Z19)
 - управление 9-ю модулями реле (реле вентилятора приточного, реле увлажнителя, реле нагревателя, реле насоса, реле компрессора охладителя, реле общего питания, реле освещения, реле питания датчиков, часов, SD-card, концевиков)
 - управление мотором постоянного тока (2-х-позиционная заслонка приточной вентиляции)
 - управление шаговым двигателем (9-х-позиционная заслонка управления потоком воздуха)
 - отслеживание состояния 5-и концевых выключателей (4-е концевых выключателя на двух заслонках (включение - по внутренним прерываниям, выключение - с использованием внутренних таймеров), 1 концевой выключатель на двери)
 - отслеживание состояния герконового выключателя (датчик уровня воды в увлажнителе)
 - индикация рабочего состояния и наличия ошибок (два светодиода)
 - отслеживание текущего времени (модуль часов RCT DS1307)
 - логирование работы системы на накопитель SD-card, перезапуск с последних настроек, зафиксированных на карте памяти
 - перезапуск системы при зависании (используется WatchDog)
 - диспетчер задач
 - постановка в очередь диспетчера задач отслеживания и регулирования климатической системы по настраиваемому асинхронному программному таймеру
 - несколько последовательных циклов со своими настройками параметров микроклимата
 - калибровка приводов заслонок в момент запуска системы
 - при возникновении ошибок по датчикам, часам, карте памяти производится перезагрузка данных устройств по питанию
 -------------
