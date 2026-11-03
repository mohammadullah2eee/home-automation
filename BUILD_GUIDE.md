# Build Guide: ESP32 Wi-Fi Home Automation

Follow these steps to build the same project. Read the safety notes first.

## 0. Safety

- This build uses **5V loads only** (a DC cooling fan and LEDs). Do not connect mains AC voltage to this wiring unless you are trained to work with it. Mains voltage can kill.
- **Handle Li-ion batteries with care.** Check the polarity before you connect them. Never short them. Charge them only with a proper charger. Do not leave them charging unattended.
- Three cells in series reach about **12.6V** when full. Check the maximum input voltage of your breadboard power supply module (many are rated for 12V or less) before you connect the pack.
- Always put a resistor in series with an LED.
- Unplug the battery before you change any wiring.

## 1. What you need

| No. | Component | Quantity |
|---|---|---|
| 1 | ESP32 development board, 38-pin, with USB cable | 1 |
| 2 | 2-channel relay module (5V) | 1 |
| 3 | 5V DC cooling fan | 1 |
| 4 | LED (red and green) | 2 |
| 5 | Jumper wire set (30 pieces: 10 male-male, 10 male-female, 10 female-female) | 1 set |
| 6 | 3.7V Li-ion battery | 3 |
| 7 | 3-cell battery holder | 1 |
| 8 | Breadboard power supply module | 1 |
| 9 | DC barrel jack (5.5 mm × 2.1 mm) | 1 |
| 10 | Breadboard (830 points) | 1 |
| 11 | Resistors: 100 ohm and 220 ohm | 2 of each |

You also need a computer with the free Arduino IDE: https://www.arduino.cc/en/software

## 2. Set up the Arduino IDE for ESP32

1. Open the Arduino IDE and go to **Tools → Board → Boards Manager**.
2. Search for `esp32` and install **esp32 by Espressif Systems**.
3. If you cannot find it, open **File → Preferences** and add this address to "Additional boards manager URLs", then search again:
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
4. Go to **Tools → Board** and choose **ESP32 Dev Module** (or the exact name of your board).

The `WiFi.h` library comes with the ESP32 package, so you do not need to install any other library.

## 3. Build the power supply

1. Put the three 3.7V cells into the 3-cell battery holder (check the polarity marks).
2. Connect the holder's wires to the DC barrel jack, then plug it into the breadboard power supply module.
3. Set the module's output jumper to **5V**.
4. Place the power module on the breadboard so that its 5V and GND rails feed the breadboard rails.

Check the output with a multimeter before you connect anything else. You should read about 5V.

## 4. Wire the circuit

**Control signals**

| ESP32 pin | Relay module pin |
|---|---|
| 5V (VIN) or the 5V rail | VCC |
| GND | GND |
| GPIO16 | IN1 (fan) |
| GPIO17 | IN2 (LED) |

**Power**

- Power the ESP32 either from its USB cable or from the 5V rail. Do not connect both at the same time unless your board is designed for it.
- Join **all grounds**: the power module GND, the relay module GND and the ESP32 GND.

**Loads** (see `images/wiring-diagram.svg`)

1. **Fan:** 5V rail (+) to **COM1**. **NO1** to the fan (+). Fan (-) to GND.
2. **LED:** 5V rail (+) to **COM2**. **NO2** to a **220 ohm** resistor, then to the LED anode (long leg). The LED cathode (short leg) goes to GND.

With NO (normally open) wiring, a load is off until its relay is switched on.

**About the resistors:** a 220 ohm resistor limits an LED to roughly 14 mA at 5V, which is safe. A 100 ohm resistor at 5V would pass about 30 mA, which is more than a small LED should carry. Use the 100 ohm resistors only at a lower voltage such as 3.3V.

If you use an ESP32 **WROVER** board, GPIO16 and GPIO17 are used by its extra memory. Pick other free pins in the sketch (for example 26 and 27) and wire to those.

## 5. Upload the code

1. Open `HomeAutomation_ESP32/HomeAutomation_ESP32.ino` in the Arduino IDE. The sketch folder name must match the file name.
2. Connect the ESP32 with the USB cable and choose its port in **Tools → Port**.
3. Click **Upload**. If you see "Connecting......", press and hold the **BOOT** button on the board until the upload starts.
4. Open **Tools → Serial Monitor** and set the speed to **115200**. After a reset you should see the board's address, normally `192.168.4.1`.

## 6. Use it

1. On your phone or laptop open the Wi-Fi settings and join **ESP_HomeAutomation** (no password in the original code).
2. If your phone says there is no internet, choose to **stay connected**.
3. Open a browser and type `http://192.168.4.1` (use `http`, not `https`).
4. Press **ON** for the fan or the LED. The relay should click and the load should switch.
5. Press **OFF** to switch it off.

## 7. Make it safer: add a password

In the sketch, change this line:

```cpp
const char* password = "";
```

to a password of at least 8 characters, for example:

```cpp
const char* password = "ChooseYourOwn123";
```

Upload again. Your phone will now ask for the password when you join the network. Choose your own password and do not publish it.

## 8. How the code works

- `WiFi.softAP(ssid, password)` makes the ESP32 create its own Wi-Fi network.
- `WiFiServer server(80)` starts a web server on port 80.
- In `loop()` the board waits for a browser to connect and reads its request.
- If the request contains `GET /fan/on` it sets the fan relay pin LOW. `GET /fan/off` sets it HIGH. The LED works the same way with `/led/on` and `/led/off`.
- The board then sends back a small HTML page. Each card shows the button for the **next** action (ON when the load is off and OFF when it is on).
- The relay module is active-LOW. If your module is active-HIGH, swap `HIGH` and `LOW` in the sketch.

## 9. Troubleshooting

- **Upload fails:** hold the BOOT button while "Connecting..." appears. Check the cable (some cables only charge) and install the USB driver for your board (CP210x or CH340).
- **No Wi-Fi network appears:** reset the board and watch the Serial Monitor. Move closer.
- **The page does not open:** use `http://192.168.4.1`. On Android turn off mobile data, because the phone may try to use it instead of the ESP32 network.
- **The relay does not click:** check VCC and GND. Some modules need the jumper between VCC and JD-VCC in place.
- **The relay clicks but the load does not switch:** re-check COM, NO and the load's ground connection.
- **ON and OFF are reversed:** your relay is active-HIGH. Swap `HIGH` and `LOW`.
- **The board restarts when the fan starts:** the 5V supply is too weak or too noisy. Use a supply that can deliver more current, or power the ESP32 separately from the fan.
- **The power module gets hot or the voltage is wrong:** disconnect the battery at once and check the input voltage and polarity.

## 10. Ideas to extend it

- Add more relays (define more pins and more `/name/on` and `/name/off` links).
- Join your home Wi-Fi in station mode so the board has a normal address.
- Add a timer or schedule.
- Add sensors such as temperature or motion for automatic control.
- Use MQTT to control it from anywhere (this needs an internet connection and careful security).

## 11. Learn more

- ESP32 Arduino core: https://github.com/espressif/arduino-esp32
- Arduino documentation: https://docs.arduino.cc
