DCmotor

Библиотека для работы с двигателем постоянного тока


Пример:

#include "DCmotor.h"

DCmotor AA1(6, 7);

DCmotor AA2(3, 4, 5);

AA1.Init();
AA2.Init();

AA1.Start(-1);
AA1.Stop();
AA1.PWM_ON();
AA1.Start(25);
AA1.Stop();


AA2.PWM_ON();
AA2.Start(-50);
AA2.Stop();

------------------------------------------------------------------




Протестировано 30.10.2020 - успешно