# Search Everywhere

Поиск данной подстроки во всех словах словаря.
Вычисление и вывод в список выполняются в разных тредах, поэтому это не мешает вводу.
Показывается статус поиска: сколько найдено подходящих слов и завршен ли поиск.
Есть билд для win10_64. 

Если захотите собирать сами под win10 то стоит запустить `windeployqt.exe` с параметром свежескомпиленного экзешника. Если будет ошибка `0xc000007b` или будет требовать какие-то `.dll`-ки то стоит заменить `.dll` на те, которые из папки `DLLs for windows`. 

**Build project:**
```
mkdir build && cd build && cmake .. && make && cd ..
```
**Dependencies:** Qt6

**Ограничения**: макс размер слова в словаре 40, словарь должен помещаться в оперативку **машины, на которой будет производиться предподсчет** (на машине **пользователя** **не** хранится весь словарь в оперативке)

**Алгоритм:** 
Подготавливаем словарь: для каждого слова добавляем всё суффиксы и сортируем, работает за `O(N * log(N) * MAX_WORD_LEN)`. В итоге, получится файл размером в 40 (на тестовом словаре в 13) раз больше. При запросе делаем бинпоиск по подготовленному файлу, при помощи `seekg()`, работает за `O(log(MAX_WORD_LEN * N) * MAX_WORD_LEN)`, где `N` количество слов в изначальном словаре.