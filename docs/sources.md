# Проверенные источники

Проверено 8 сентября 2026. Ссылки на производителей и авторов проектов.

| Источник | Для чего используется |
|---|---|
| [NASA Clipper Duet](https://www.nasamarine.com/product/clipper-duet-system/) | Название и состав штатной системы |
| [NASA Duet manual 2019](https://www.nasamarine.com/wp-content/uploads/2019/07/Clipper-Duet-manual-19.pdf) | Режимы, OUT, offset, калибровка, ограничения установки |
| [NASA spare transducers](https://www.nasamarine.com/shop/page/2/) | Указание 150 кГц для запасного датчика |
| [ClipperDuet2N2k](https://github.com/speters/ClipperDuet2N2k/tree/27619d4b321be46d96fcb02a9dbee318b089d9bc) | Карта LCD и точки подключения конкретной платы; MIT |
| [Open Boat Projects](https://open-boat-projects.org/en/nmea2000-fuer-nasa-clipper-duet-echolot-log/) | Дополнительное описание переделки; прямые входы 5 В из него не повторяются |
| [HT1621](https://www.holtek.com.tw/webapi/11842/HT1621_21Gv340.pdf) | 3-битная команда, 6-битный адрес, память 32×4, фронт WR |
| [ESP32 datasheet](https://documentation.espressif.com/esp32_datasheet_en.pdf) | GPIO, питание и контроллер TWAI |
| [SN74LVC125A](https://www.ti.com/product/SN74LVC125A) | Входы до 5,5 В при питании 3,3 В, распиновка, Ioff |
| [ADM3053](https://www.analog.com/media/en/technical-documentation/data-sheets/ADM3053.pdf) | Изоляция, VCC=5 В, VIO=3,3 В, токи |
| [MIKROE-2627](https://www.mikroe.com/can-isolator-click) | Готовый CAN-модуль, размер и выбор VIO |
| [MIKROE schematic v100](https://download.mikroe.com/documents/add-on-boards/click/can-isolator/can-isolator-click-schematic.pdf) | TXD/RXD, питание, терминация R2/R3, DB9 |
| [Pololu D24V10F5](https://www.pololu.com/product/2831) | Понижение до 5 В и КПД; вход до 36 В |
| [PGA460 TI](https://www.ti.com/lit/ds/symlink/pga460-q1.pdf) | Почему диапазоны не подходят для штатных 150 кГц |
| [NMEA2000 library](https://github.com/ttlappalainen/NMEA2000/tree/5b7b9fc3ccc18e30ebfba92da6486cffc6251595) | Формирование PGN, NA, identity и address claim |
| [NMEA2000_esp32](https://github.com/ttlappalainen/NMEA2000_esp32/tree/15cc30887f0e629fbee0821c5b1de03d130728ac) | CAN-драйвер классического ESP32 |

Опубликованная чужая переделка не подтверждает совместимость другой ревизии NASA.
Синтетические тесты не являются измерениями датчиков. Бюджет мощности в проекте
является расчётом, а не паспортом готового устройства.
