# MakeItRain
This is a project for a DIY multinode watering system like rainbird or whatever the other ones are. Using MQTT to control the valves or reading the local inputs from the device. will work with 12 or 24 volt (DC) systems. These can be set up remotely with a solar panel and a decent sized battery (solar charger not included) 


Some basic documentation. Currently this is beta/dev FW and hardware that after I get working enough and integrated into my own systems, I will get some broader system integration going. Some Goals are to have this integrate with Homeassistant ~~via a custom integration~~ via MQTT and i will be adding support for self contained schedules and timers. Limited as they will be it is on my roadmap that this does not have to be connected to the internet or any Wifi to function. Those are optional extras if you want. Other things will be to have OTA FW updates, Web page control and possibly Telnet/SSH control if i get around to it. 

Hardware 
1. ESP32 - https://www.amazon.com/gp/product/B07QCP2451/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1
2. Valves - https://www.amazon.com/gp/product/B004RUH7A0/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1
3. Battery - any 12Vdc with >=7.5 Ah will work for a few months or depending on how often you water may be less. 

Documentation will be coming once i fix a few more issues. 
