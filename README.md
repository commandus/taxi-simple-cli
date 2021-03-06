taxi-simple-cli
===============

taxi-simple.cfg
===============
Конфигурационный файл содержит значения по умолчанию, перекрываются указанием соответствующей опции.
Ищется в домашнем каталоге приложения.

объект client:

Соединение:
 * host = по умолчанию localhost!
 * port = по умолчанию 9090
 
Объект client.credentials
Аутентификация: 
 * role = admin|manager|customer|dispatcher|driver|master|operator|passenger|guest
 * phone = номер телефона
 * password = номер телефона
 
Объект service относится к сервису!

Пример
------
client =
{
	host = "localhost";
	port = 9090;
	credentials =
	{
		role = "admin";
		phone = "77777777";
		password = "1111111";
	};
}

Команды
=======
--license	исп. для проверки вывода символов кириллицы
--add		добавление
--rm		удаление
--get		один объект
--list		поиск, --offset 0 --count 100 по умолчанию
-?		список команд и опций

Объекты
=======
--object
Список объектов см. taxi.thrift

Опции
=====
Опции --*name при поиске (идентификации объекта пор имени), как правило, позволяют ввести начало названия.
--help		Список опций
Если отсутствует обязательная опция, выдается сообщение об ошибке.
Опции даты и времени в Windows могут не работать корректно.

Опции соединения
================
-h		109.120.190.73
-p		9090

Опции аутентификации
====================
-myrole			admin|manager|customer|dispatcher|driver|master|operator|passenger|guest
-myphone		77777777777
-mypassword		11111111111

Добавление
==========
-a, --add

Добавление города
-----------------
taxi-simple-cli --add --object city --name Yakutsk --latitude 62.03389 --longitude 129.73306 --notes "-" --areaid 1 --tag 1

Добавление банка
-----------------
taxi-simple-cli --add --object bank --name "Банк 1" --notes "-" --areaid 1 --tag 1

Добавление торговой марки автомобиля
------------------------------------
taxi-simple-cli --add --object vehiclebrand --name Acura

Добавление модели автомобиля
----------------------------
taxi-simple-cli --add --object vehiclemodel --brandname "Toyot" --name Acura

Добавление названия цвета автомобиля
------------------------------------
taxi-simple-cli --add --object vehiclecolor --name "черный" --value 0

Добавление автомобиля
------------------------------------
taxi-simple-cli --add --object vehicle 
--brand 
--model
--color название или число(десятичное!)
--plate
--platenumber
--vin

Добавление организации
----------------------
taxi-simple-cli --add --object org --name "ООО 123" 
Необязательные опции:
--bank-acc			Идентификатор банка
--orgrole			0- Потенциальный клиент (по умолчанию), 1- Сервис (город), 2- Таксомоторная служба, 3- Клиент, 4- Банк, 5- РКО, Процессинг карт
--orgtype			1- правительственная организация, 2- некоммереческая организация, 3- коммерческоре предприятие(по умолчанию), 4-крестьянское (фермерское) хозяйство, 5- другие организации
--fullname			Полное наименование
--shortname			Аббревиатура или короткое наименование
--inn				ИНН
--kpp				КПП
--orgn				ОГРН
--phone				Телефон +79140001111
--email				a@acme.com
--currentaccount		Р/С
--correspondentaccount		К/C
--description			Описание
--note				Заметка

Добавление клиента
------------------
taxi-simple-cli --add --object customer --nickname "ООО 1" --cityid 1
--nickname		внутреннее имя организации
--cityid		идентификатор города
--active		1- активный (по умолчанию)
--enabled		1- обслуживается (по умолчанию)
--taxtype		тип налогообложения: 1- организация (по умолчанию), 2- ИП, 3- фермерское хозяйство, 4- адвокат, 5- нотариус
--preferreddriverid	идентификатор предпочитаемого водителя

Добавление водителя
-------------------
taxi-simple-cli --add --object driver --phone 79141112222 --nickname "Вася Пупкин" --cityid 1
--phone			79141112222
--pwd			пароль
--nickname		ник
--firstname		имя (необязательно)
--lastname		фамилия (необязательно)
--middlename		отчество (необязательно)
--callsign		позывной (номер)
--cityid		идентификатор города
--online		0- не на линии (по умолчанию), 1- на линии
--empstatus		1- работает (по умолчанию) 2- в отпуске 3- уволен
--svccarpoolid	идентификатор таксопарка
--svcdispatchid	идентификатор диспетчерской службы такси

Добавление пассажира (оператора)
--------------------------------
taxi-simple-cli --add --object passenger --city 1 --customerid 1
--personid		идентификатор персоны
--cityid		идентификатор города
--customerid		идентификатор клиента- организации
--isoperator		1- оператор, 0- не оператор
--isvip			1- требует особого внимания
--empstatus		1- работает 2- в отпуске 3- уволен
--tag			номер пользовательской классификации (не используется)

Добавление тарифного плана
--------------------------
taxi-simple-cli --add --object tariffplan --cityid 1 -- orgserviceid 1 --notes "plan A"

Добавление тарифа
-----------------
taxi-simple-cli --add --object rate --name "150+10" -- tariffplanid 1 --priceboarding 150 --priceminute 10 --timedelayfree 10 --timedelayfree

Списки
======
-l, --list

Список городов
--------------
taxi-simple-cli --list --object city --name Yakut --offset 0 --count 100

Список банков
--------------
taxi-simple-cli --list --object bank --name Росбан

Список торговых марок автомобилей
---------------------------------
taxi-simple-cli --list --object vehiclebrand --name Toy

Список цветов автомобилей
-------------------------
taxi-simple-cli --list --object vehiclecolor --name "чер"

Список водителей
----------------
taxi-simple-cli --list --object driver 
--id			идентификатор водителя
--nickname		ник
--callsign		позывной (номер)
--cityid		идентификатор города
--online		0- не на линии, 1- на линии
--empstatus		1- работает 2- в отпуске 3- уволен
--svccarpoolid	идентификатор таксопарка
--svcdispatchid	идентификатор диспетчерской службы такси

Список клиентов
---------------
taxi-simple-cli --list --object customer
--id			идентификатор клиента
--nickname		внутреннее имя организации
--cityid		идентификатор города
--active		1- активный
--enabled		1- обслуживается
--taxtype		тип налогообложения: 1- организация, 2- ИП, 3- фермерское хозяйство, 4- адвокат, 5- нотариус
--preferreddriverid	идентификатор предпочитаемого водителя

Список пассажиров (операторов)
------------------------------
taxi-simple-cli --list --object passenger  
--id			идентификатор пассажира (оператора)
--personid		идентификатор персоны
--cityid		идентификатор города
--customerid	идентификатор клиента- организации
--isoperator	1- оператор, 0- не оператор
--isvip			1- требует особого внимания
--empstatus		1- работает 2- в отпуске 3- уволен
--tag			номер пользовательской классификации (не используется)

Заказ водителя
==============

Изменение статуса заказа водителем (диспетчером, пассажиром)
=============================================================

taxi-simple-cli --order <состояние> --id <номер заказа> [--minutes <время подъезда>]
где состояние: take,wait,start,stop,complete,fail,cancel

cancel- делается пассажиром, другие- водителем или дисптечером

например, взять заказ с примерным временем прибытия 5 минут (по умолчанию 10 минут)
taxi-simple-cli --order take --id 12 --minutes 5

Водитель подъехал и ждет
taxi-simple-cli --order wait --id 12 

Водитель начал везти
taxi-simple-cli --order start --id 12 

Водитель не дождался пассажира, не смог выполнить заказ
taxi-simple-cli --order fail --id 12 

Водитель успешно закончил исполнять заказ
taxi-simple-cli --order complete --id 12 

Пассажир отказался заказ
taxi-simple-cli --order cancel --id 12 

Водитель на линии или ушел с линии
==================================
Начал смену
taxi-simple-cli ... --myrole driver --online 1

Закончил смену
taxi-simple-cli ... --myrole driver --online 0

Заказ пассажира
===============
taxi-simple-cli	... --myrole passenger --add --object serviceorder --cityid <№город> [--ordertype <1..2> --ordertimetype <1..3> --datestart datetime] --notes "Примечание"]
--ordertype 1- такси (по умолчанию) 2- курьер
--ordertimetype 1- срочный (по умолчанию) 2- Запланированный в точно указанное время в течение суток 3-Запланированный в точно указанное время ежедневно в заданные дни недели

Проталкивание уведомления
=========================
Из строки или файла
--sendgcm --apikey <Google API key> --data <JSON object|array>|--file <JSON file> Id1 Id2 ...
Конвейером
taxi-simple-cli --sendgcm --apikey <Google API key> Id1 Id2 ... < <JSON file>
cat <JSON file> | taxi-simple-cli --sendgcm --apikey <Google API key> Id1 Id2 ...

Id1, Id2... - идентификаторы пользователей

Результат в stdout,  например:
{"multicast_id":5412105220767091569,"success":0,"failure":1,"canonical_ids":0,"results":[{"error":"NotRegistered"}]}
или сообщение об ошибке.
В примере сообщение обработано успешно, но проталкивание не произошло.

Пример
------
--sendgcm --apikey "AIzaSyCIv-PZWoKrEC_f7-gE013v7FI3ehlPhnw" --data "{\"aa\":\"22\"}" "APA91bGxdOKCEyGQHIZ676r7qB31uHh3C3r960mkS-WFrLFFFDmxM9s2RGrt_gePN3zT8NuOXJshZJDfaP59-6Uuca519IsxIq1VH0A1J_0AUMGoqe1UukgG9t3aU6KCiqnOOHQVinEh5aYNJo53FZAWOXxHTnkYvQ"

События
=======
Водителя
taxi-simple-cli -h localhost -p 9090 --myphone 7968... --mypassword ... --myrole driver --list --object events 

Скрипты загрузки из файлов
==========================
Структура каталогов:
./scripts		Perl скрипты
./data			Файлы данных в кодироровке UTF-8 в 

Параметры:
--count 10		загрузить первые 10 строк
--verbose		детальный отчет
--file			файл для загрузки

Загрузка торговых марок автомобилей
-----------------------------------
load-vehiclebrand.pl --file ../data/vehiclebrand.txt --count 2000 --verbose

Загрузка моделей автомобилей
-----------------------------------
load-vehiclemodel.pl --file ../data/vehiclemodel.txt --count 20000 --verbose

Загрузка цветов автомобилей
-----------------------------------
load-color.pl --file ../data/color.txt --count 2000 --verbose

Состояния заказа
================
ORDERNEW	=	0,		//	заказ не обработан
ORDERACCEPT	=	1,		//	заказ принят
ORDERASSIGN	=	2,		//	сервис передал выбранному водителю уведомление, но не получил подтверждения
ORDERASSIGNED	=	3,		//	Не используется. получил подтверждение от водителя, заказ принят
ORDERCONFIRMED	=	4,		//	Пользователь согласился с временем ожидания, выставленного водителем
ORDERDRIVING	=	5,		//	Водитель приступил к исполнению заказа (выехал к Пользователю)
ORDERWAITING	=	6,		//	прибыл на место назначения, ожидает Пользователя
ORDERTRANSPORTING=	7,		//	везет Пользователя к месту назначения
ORDERSTOP	=	8,		//	Временная остановка
ORDERCOMPLETE	=	9,		//	Заказ выполнен
ORDERDECLINED	=	10,		//	заказ отвергнут сервисом (нехороший клиент, нет свободных экипажей, ..)
ORDERCANCELLED	=	11,		//	Заказ отменен Пользователем
ORDERFAILED	=	12		//	Заказ отменен Водителем

FAQ
===
Q:./taxi-simple-cli: error while loading shared libraries: libthrift-1.0.0-dev.so: cannot open shared object file: No such file or directory
A:export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib. Для 64 битных Linux библиотека Thrift не располагается в правильном каталоге

Q:Если крякозябры в выводе консоли Windows. 
A:В свойствах окна консоли Windows должен быть установлен шрифт Lucida Console, а не Consolas. В Linux консоль utf-8

Q: Пересобрать без отладочной информауции
A: в сгенерированном ./configure файле Makefile уберимте -g из флагов компилятору

Q: undefined reference to `pthread_cancel'
A: в Makefile добавьте LIBS -pthread
