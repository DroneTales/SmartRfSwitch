# Smart Wireless Switch for Apple Home

Here you will find the firmware and schematic of a smart wireless switch for Apple Home. For any questions, welcome to my [Telegram channel](t.me/drone_tales).

**Components used**

- Wireless switch and relay - 1 pc.
- ESP32C3FN4 Super Mini - 1 pc.
- 2N3904 transistor - 2 pcs.
- NEC2561 optocoupler - 2 pcs.
- Button - 1 pc.
- 1K resistor - 4 pcs.
- 10K resistor - 3 pcs.
- 150 Ohm resistor - 1 pc.
- 330 Ohm resistor - 1 pc.
- 5V 1A power supply - 1 pc.

**Arduino libraries used**

- esp32 by Espressif Systems (board) 3.3.7
- HomeSpan 2.1.7

**IDE settings**

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

## Introduction

Smart light bulbs, smart relays, and other "smart" devices in the house are cool. But! As always, there is that damn "but". At 3 a.m., when everyone is asleep, shouting "Hey Siri! Turn on the kitchen light" is not the best idea. And constantly reaching for a smartphone to turn something on or off is also not great. You could, of course, install motion sensors, but there are nuances with them too. So you can't do without switches.

The first natural impulse was to buy a switch compatible with my smart home system (which, as a reminder, is Apple Home). I looked at what was on the market. Firstly, most were from the Chinese and required their own app that somehow cleverly bridged something somewhere. Secondly, and quite sadly, most required pre-installed wiring or a ZigBee gateway, or both. So that option fell through for several reasons.

The second thought (which comes afterwards) was to buy any switch at all and bridge it to Apple Home via HomeBridge. This idea again hit a wall with ZigBee hubs and wiring.

After spitting on all this nonsense and racking my brains (no, not in that sense, not around the apartment), I suddenly remembered that I am, after all, a programmer. What's to stop us from building a switch ourselves? Nothing. We take a radio relay and build it. I got stuck back then because I was terribly lazy to print a proper switch button. So the idea was shelved in a not very long, but still, box.

And so, not long ago, I was looking for something on Ozon and decided to check out what was available in the smart switch technology space. And I found a cool thing: a full-size switch with a 433 MHz relay. And the main thing is that this switch (the button itself) runs on a battery, and the relay connects to the wires of the chandelier (or light bulb). Thus, no special wiring is needed for it.

*Let me clarify right away: by "special wiring" I mean bringing phase and neutral to the light switch. That is, you should have three wires coming to the switch: phase from the panel (which is switched), phase to the chandelier, and neutral from the panel. Usually, standard wiring to a switch is just two wires: phase from the panel and the same phase going to the chandelier (phase break). There is no neutral there.*

So, here is what arrived.

<img width="1200" height="774" alt="4427c5d0-34c0-4a41-b96e-95a5544ea0b3" src="https://github.com/user-attachments/assets/71024160-a91a-4c7d-8ef9-c77a1872dfa5" />

The only thing left was to befriend this switch with Apple Home. Naturally, I will use an ESP32 paired with the Arduino HomeSpan library for this. Fortunately, I have been friends with this library for quite a while and already have devices built on it. I also already had experience "adapting" a radio doorbell (also 433 MHz). No problems were expected.

Before gutting the relay, I looked at the instructions. And there I found a mention that more than one button can be paired with the relay. Interesting, I thought, so the buttons are somehow distinguished. Well then, time to open this wonderful specimen of Chinese industry. The button doesn't interest me much, it's a regular RF transmitter, it works and that's fine. But the relay should be studied in detail. Fortunately, the case is not glued, not soldered, and generally opens easily. Here is what I extracted from this white box.

<img width="1200" height="1041" alt="fa4724e8-bb1b-4a3b-9f20-01ba59bede23" src="https://github.com/user-attachments/assets/cde664a8-2a51-459b-aa31-f9d86a19ea73" />

On the right side of the board, you can see a transformerless (note that!) power supply built on a diode bridge BD1. Some IC U2 (I suspect it's a regulator for some voltage) marked IVAP302 (couldn't find what that is, if anyone knows — write in the comments), diodes D2 and D3, and some passives (capacitors, resistors). Several components are unmarked and soldered on the back side of the board. Specifically: a ballast resistor and a couple of smoothing capacitors. Nothing interesting in general. The only thing that interested me here was the regulator. Because it determines the supply voltage of the rest of the circuit and, consequently, the options for interfacing it (the circuit) with the ESP32. You could just measure the output voltage from the power supply, or you could later figure it out by indirect signs. I settled on the second option, since I still need to figure out the "protocol" of communication between the button and the relay.

On the left side of the board is the relay control circuit itself, consisting of two ICs U4 (name erased) and U3 (marked FMD FT60E011A F269RKE), some passives, transistor Q1, and the relay itself. Relay marking: BRD-SS-105LMF. This indicates that the relay is a 5-volt one. Which is very good and hints that the circuit operates from 5 volts. It's not certain, of course, that everything else is powered by 5 volts, since the relay is switched through a transistor, but it's something. It's unlikely there will be more.

A quick Google search for FT60E011A gives us a link to a Data sheet from which it follows that it's a simple microcontroller. This is also indicated by the fact that this IC (U3) is connected to: a button, an LED, the base of transistor Q1 (through current-limiting resistor R3), and the second IC (U4). Therefore, U4 is an RF chip. And it sends the received signal to the microcontroller. The RF chip itself and its supporting circuitry do not interest me in this case, so I won't describe what's there and how. It's just a bunch of capacitors in the supporting circuitry and an antenna.

*Transistor Q1 in this circuit works as a switch: it allows a small current from the microcontroller to control a relatively large current of the relay coil. Typically, microcontroller outputs are designed for currents of a couple dozen milliamps. But the relay itself consumes a much larger current (can reach hundreds of milliamps). The microcontroller simply cannot output such current on its pins. And if it could, it would most likely just burn out. That's why a transistor switch is used. And diode D1 suppresses the reverse energy surge from the relay coil.*

Now we need to figure out what exactly the RF chip transmits to the microcontroller and how the microcontroller controls the relay. At the same time, I'll look at the signal levels. It's time to poke around there with an oscilloscope.

Before poking with oscilloscope probes, I should solve the power issue. I don't really want to poke around in a device that is directly powered from 220 V without isolation. I decided to supply power right after the diode bridge from a lab power supply. It started up from 12 volts.

Signal from the RF chip to the MCU without a command from the button. Sometimes there is noise. But mostly it's quiet.

<img width="800" height="480" alt="a33f33ec-6ca8-4009-b592-5f8ce8f76c0d" src="https://github.com/user-attachments/assets/9afa8492-b67a-477f-aa81-d77b50be7eec" />

If you press the button, the RF chip starts transmitting something to the microcontroller. I suspect it's the button code.

<img width="800" height="480" alt="d95c067e-4795-40ff-b891-c672eb240a08" src="https://github.com/user-attachments/assets/5a73dc86-0c78-4c6d-baad-56f24bb49a33" />

Not everything fit in the screenshot, but there is a fairly long sequence. It's immediately clear that the signal has a level of 5 volts, which is very, very good. And the relay is 5-volt, as I mentioned above. So there will be no problems with level matching.

I didn't really want to get into decoding and emulating the signal, so I also looked at the relay control signal (on R3 from the microcontroller side). I won't show the oscillogram — everything is as expected. Either a constant high level (5 volts) when the relay turns on (closes the output contacts). Or a constant low level if the output is open.

In general, I decided to connect in the break between the microcontroller and Q1. To do this, I will need to unsolder resistor R3. I sketched out the following connection diagram.

<img width="1200" height="494" alt="5dbec0c2-6881-4383-bcb6-f2fc6c171803" src="https://github.com/user-attachments/assets/923502a1-b7ec-4e81-9c78-a542090c3793" />

The diagram is very simple. Opto-isolation from the relay (since I remember that there is a transformerless power supply there, which does not have galvanic isolation from the 220 V mains). Switch Signal is soldered to the microcontroller output that goes to the top contact of R3. Relay switch — to the bottom lead of resistor R3 (which is unsoldered), to the base of transistor Q1. And ground to any "-" point on the relay board. There is a very convenient one right next to R2. BTN+ and BTN- — you can connect an external button here to control the ESP32 (for details, refer to the HomeSpan documentation). BTN LED+ and BTN LED- — to the indicator LED. I have a button with a built-in LED, which is very convenient. Again — for details, refer to the HomeSpan documentation.

<img width="1200" height="1041" alt="e9042f05-5a32-4acc-851c-255e513fc7ad" src="https://github.com/user-attachments/assets/9198605e-8f63-4c32-ba86-7cc467757a48" />

There is only one problem left — powering the ESP32. The initial idea to power everything from a single 5-volt power supply has so far failed, since the regulator only starts from 12 (more precisely, the power circuit starts from 12, the regulator would probably start from 6 as well). You could, of course, supply power directly, bypassing the entire power circuit, but honestly, I didn't want to unsolder a bunch of components. I wanted to keep the ability to quickly return the relay to its original state.

*If you ever want to power the relay from your own power supply, then 12 volts should be applied to the output (top in the photo) contacts of the diode bridge (BD1). Left is "+", right is "-". The diode bridge must be unsoldered! The circuit consumes less than 100 milliamps when powered from 12 volts.*

In general, I decided to leave the relay power as it is, and use a separate 5-volt power supply for the ESP32. Thus, I only need to remove resistor R3 and solder three wires.

Important point! The power supply used must be fully galvanically isolated from the 220 V mains. Otherwise, all the white smoke will come out very quickly. Fortunately, I have such a power supply.

So, everything is thought out — let's solder. I didn't bother much with the board layout. Then I printed a small box, connected the button. Here is everything assembled.

<img width="1200" height="636" alt="b05cdded-5379-4e8e-be72-37bb3e436af2" src="https://github.com/user-attachments/assets/4875408d-8865-4866-8cbd-5699a69b8152" />

I've dealt with the electrical and electronic parts. Time to prepare the firmware. Well, there are no special problems with that. The logic is quite simple: since the control signal has a logic level and is constant for a given state (always 5V if the relay is on and always 0V if the relay is off), the optimal solution is to use an interrupt on signal level change. It doesn't matter what the relay MCU outputs. What matters is when this signal changes. By the signal change, you can determine that the button was pressed. And what actually needs to be done with the relay (turn on or off) depends solely on the internal state of the ESP32 firmware. This allows controlling the relay independently of the current state of the control signal, both by the button and from Apple Home.

Well, that's basically the whole story. The switch works. It was added to Apple Home. Free time during the holidays was killed. All goals achieved, as they say. If you have any questions — you are welcome to my [Telegram channel](t.me/drone_tales).
