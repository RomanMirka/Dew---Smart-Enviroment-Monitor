# Dew_Project
Dew is a small (cat) temperature and humidity monitor with a distinctive character!!!<br>
The idea behind this project is a small ESP32-based device that monitors the temperature and humidity in a room, displays the data on an OLED screen, and sends it via MQTT to the specified destination. <br> 
<b>Dew is more than just a sensor with a display.</b><br> 
## Hardware 
| Component | Purpose |
|-----------|---------|
| ESP32 | Main microcontroller |
| DHT22 | Temperature and humidity measurements |
| SH1106 OLED 128x64 | Display output |
| Push Button | User control |
## Design 
Everyone loves cats, but cats love to sleep. Dew is a very lazy and sleepy cat, he can fall asleep at any moment. But if you give him a little push, he’s right there, wide awake and ready to do his job. Until the next time it gets too quiet. <br>
<img width="256" height="128" alt="Dew_sleeps" src="https://github.com/user-attachments/assets/790dad6a-5f8b-4728-99d4-f788b2f4e64a"/> <img width="256" height="128" alt="Dew_sleeps(1)" src="https://github.com/user-attachments/assets/bd45cd44-cbf3-49b6-a4a7-6071135be257"/> <img width="256" height="128" alt="Dew_sleeps(2)" src="https://github.com/user-attachments/assets/72d3abe0-3bf5-4383-8c71-6b4c9d1325f4" /> <br>
The app’s magic is in its reactions. Dew is like a Tamagotchi. It don’t just show numbers — it reacts to them.<br> 
Too hot? It feels it.<br>
<img width="256" height="128" alt="Dew_hot" src="https://github.com/user-attachments/assets/778dd12c-f9b9-4eb7-9b33-5f43ac5a43ae" /><br>
Stuffy? It knows that, too.<br>
<img width="256" height="128" alt="Dew_stuffy" src="https://github.com/user-attachments/assets/4f46a036-cab6-4478-b3ba-89cc429c86ba" /><br>
Cold or dry? It’ll notice that, too.<br>
<img width="256" height="128" alt="Dew_cold" src="https://github.com/user-attachments/assets/6775024c-3947-4006-ad28-bdc221e3c343"/> <img width="256" height="128" alt="Dew_dry" src="https://github.com/user-attachments/assets/38a3e3a5-0d11-4087-8424-34b5d987b7e9"/><br>
When you first power it on, it introduces itself, walks you through the basics, and casually warns you it might fall asleep at any moment.
Depending on what’s going on around it, Dew changes - and you immediately know something’s wrong, even without looking at the readings. It’s not a sensor with a display, but a little living creature!
