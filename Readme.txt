################# 1 #########################
Официальную инструкцию см. здесь: https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html

$ sudo apt-get install gcc git wget make libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-cryptography python-future

$ wget https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz

$ sudo mkdir -p /opt/esp
$ sudo chmod 0777 /opt/esp 
$ cd /opt/esp/
$ tar -xzf /mnt/devel/ster/home/gps_speedmeter_on_mcu/installers/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
$ echo 'export PATH=\"/opt/esp/xtensa-esp32-elf/bin:$PATH\"' >>  /home/ster/.profile


echo 'export IDF_PATH=/mnt/devel/ster/home/gps_speedmeter_on_mcu/esp-idf' >>  /home/ster/.profile

################# 2 ########################
ESP-IDF версии 3.2



Клонировать репозиторий
cd gps_speedmeter_on_mcu

git clone --recursive https://github.com/espressif/esp-idf.git
cd ./esp-idf

git submodule update --init --recursive

##### https://docs.espressif.com/projects/esp-idf/en/stable/versions.html#updating-to-stable-release

git fetch
git checkout v3.1.2
git submodule update --init --recursive




###################### 

## обновление пакетов python
/usr/bin/python -m pip install --user -r /mnt/devel/ster/home/gps_speedmeter_on_mcu/esp-idf/requirements.txt


# build
./fw_build

### flashing ###
./fw_flash --port /dev/ttyUSB0
./fw_flash --port /dev/ttyUSB1


####################################################################
##################

cd /mnt/devel/ster/home/gps_speedmeter_on_mcu

export IDF_PATH=$(pwd)/esp-idf

make

#############################################
################## JTAG #####################
#############################################
CJMCU pins        ESP-BOARD(ESP32-WROM-32) 
AD0(TCK)          D13(IO13)
AD1(TDI)          D12(IO12)
AD2(TDO)          D15(IO15)
AD3(TMS)          D14(IO14)
+5V 		      VIN 
GND 		      GND

>>Настройка

>>>Установка OpenOCD для ESP-32

Тут можно смело ссылаться на доки самой esp-idf:

sudo apt-get install make libtool pkg-config autoconf automake texinfo libusb-1.0 libftdi1-2 git
git clone --recursive https://github.com/espressif/openocd-esp32.git # Или скачиваем релиз из https://github.com/espressif/openocd-esp32/releases
cd openocd-esp32
./bootstrap
./configure # Убеждаемся, что FTDI-based adapters включены
make -j6
sudo make install


Отдельно отмечу, что я ставил эту версию OpenOCD в систему (sudo make install), что неприемлемо, когда у вас разные OpenOCD для разных устройств.

>>>Настройка OpenOCD под нас

На этом этапе я разделил конфиг для платы отладки и самой ESP-32. Получилось так:
board.cfg:

transport select jtag
adapter_khz 20000 # С этим можно поиграться
# set ESP32_ONLYCPU 1 # Только первое ядро
# set ESP32_RTOS none # Убрать поддержку RTOS
source [find target/esp32.cfg]


interface.cfg:

interface ftdi
ftdi_vid_pid 0x0403 0x6014
ftdi_layout_init 0x0c08 0x0f1b


Пишем правило Udev

Из-за проблем с правами мы не сможем получить доступ к USB-устройству. Это можно исправить одной строчкой правила udev:

echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="0403", ATTR{idProduct}=="6014", GROUP="plugdev", MODE="0666"' | sudo tee /etc/udev/rules.d/99-JTAG.rules
sudo systemctl restart udev.service
sudo udevadm control --reload-rules


Вспомогательный скрипт

Кроме всего прочего, весьма удобен скрипт вроде такого:

openocd_command:

#!/bin/bash
echo "$@" | telnet localhost 4444


Он позволит быстро посылать команды напрямую демону OpenOCD. Но об этом — позже.

>> Проверяем OpenOCD

>>>Запускаем OpenOCD так:

openocd -s $(pwd)/ocd_cfg/  -f interface.cfg -f board.cfg


Если всё прошло успешно, вам покажут что-то вроде этого:

Open On-Chip Debugger 0.10.0-dev (2018-08-22-14:47)
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
adapter speed: 20000 kHz
esp32 interrupt mask on
Info : ftdi: if you experience problems at higher adapter clocks, try the command "ftdi_tdo_sample_edge falling"
Info : clock speed 20000 kHz
Info : JTAG tap: esp32.cpu0 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
Info : JTAG tap: esp32.cpu1 tap/device found: 0x120034e5 (mfg: 0x272 (Tensilica), part: 0x2003, ver: 0x1)
Info : esp32: Debug controller was reset (pwrstat=0x5F, after clear 0x0F).
Info : esp32: Core was reset (pwrstat=0x5F, after clear 0x0F).
Info : Detected debug stubs @ 3ffb3134 on core0 of target 'esp32'
cpu0: Current bits set: BreakIn BreakOut RunStallIn
cpu1: Current bits set: BreakIn BreakOut RunStallIn


Это говорит о том, что всё заработало!

Важно: процесс OpenOCD должен крутиться всё время, пока вы заняты отладкой.

>> other
Забудьте про кнопку EN аки reset на плате. Перезапуск делайте только через "Terminate and relaunch".

>>> For open openOCD shell we shold be using the command:
telnet localhost 4444


>>>Для прошивки используйте
./openocd_command "program_esp32 $(pwd)/build/gps_speedmeter_on_mcu.bin 0x10000 verify"


>> Задействуем Eclipse

>>>Тут есть целый ряд шагов, который прекрасно описан в официальной документации, и я их не буду повторять.
Look those: https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/jtag-debugging/using-debugger.html#jtag-debugging-using-debugger-eclipse

>>> Советы по отладке в Eclipse

    >>>>Забудьте про кнопку EN аки reset на плате. Перезапуск делайте только через "Terminate and relaunch".

    >>>>Для прошивки используйте

    openocd_command 'program_esp32 абсолютный_путь/файл.bin 0x10000 verify'


    >>>>Можете добавить это как цель сборки, поставив openocd_command кастомной командой, а в поле "имя цели" вписать аргументы (без кавычек).

    >>>>Вы можете получить не то, что ожидали, после остановки и возобновления. Моя библиотека для воспроизведения звука после этого тараторит как ненормальная из-за приколов с таймером.


    
#################################################################################
Хороший продукт, просто позаботьтесь об использовании хорошего конструктора, если вы используете его с u8g2 library для Arduino/ESP: /Не используйте SSD1306: /U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2 (U8G2_R0, SCL, SDA, U8X8_PIN_NONE); 
/... Но Используйте SH1106; U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2 (U8G2_R0, SCL, SDA, U8X8_PIN_NONE); 
## for esp32
https://github.com/nkolban/esp32-snippets/tree/master/hardware/displays/U8G2

https://www.winstar.com.tw/ru/products/oled-module/graphic-oled-display/sh1106-oled.html

https://motorsmarine.ru/spidometr/spidometr-elektronnyj-shema.html
// SDA - GPIO21
#define PIN_SDA 21

// SCL - GPIO22
#define PIN_SCL 22


################### GPS #############################



