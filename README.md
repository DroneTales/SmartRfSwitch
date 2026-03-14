# Умный беспроводной выключатель для Apple Home

Этот репозиторий содержит прошивку и схемы для умного беспроводного выключателя для Apple Home, о котором рассказывается в [этом видео](https://youtu.be/C4awq37Im1w).  

[Статья на Boosty](https://boosty.to/drone_tales/posts/d840bf25-aa16-4bdf-92cc-8fc180fbdb59?share=post_link) о переделке китайского беспроводного выключателя в умный.

**Используемые компоненты**

- Комплект из бепроводного выключателя и реле
- ESP32C3FN4 Super Mini
- Транзистор 2N3904 - 2 шт.
- Оптопара NEC2561 - 2 шт.
- Кнопка
- Резистор 1K  - 4 шт.
- Резистор 10K - 3 шт.
- Резистор 150Ом
- Рещистор 330Ом
- Внешний источник питания на 5V 1A
 
**Используемые библиотеки Arduino**

- esp32 by Espressif Systems (board) 3.3.7
- HomeSpan 2.1.7
 
**Настройки Arduino IDE**

- Board: ESP32C3 Dev BModule
- ESP CDC On Boot: Enabled
- CPU Frequency: 80MHz (WiFi)
- Core Debug Level: None
- Erase All Flash Before Sketch Upload: Disabled
- Flash frequency: 80Mhz
- Flash Mode: QIO
- Flash Size: 4MB (32Mb)
- JTAG Adapter: Disabled
- Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS)
- Upload Speed: 921600
- Zigbee Mode: Disabled
- Programmer: Esptool

**Поддержать автора**

Если вам интересно то, что я делаю, вы можете помочь выходу новых видео, поддержав меня любым удобным вам способом по ссылкам ниже:

**BuyMeACoffee**: https://buymeacoffee.com/dronetales  
**Boosty**: https://boosty.to/drone_tales/donate  
  
**BTC**: bitcoin:1A1WM3CJzdyEB1P9SzTbkzx38duJD6kau  
**BCH**: bitcoincash:qre7s8cnkwx24xpzvvfmqzx6ex0ysmq5vuah42q6yz  
**ETH**: 0xf780b3B7DbE2FC74b5F156cBBE51F67eDeAd8F9a  
