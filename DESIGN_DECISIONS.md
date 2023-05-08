# LumenPnP Feeder Design Decisions

This page aims to illustrate the design decisions behind the LumenPnP Feeders. The design process started in early 2020, and has seen hundreds of iterations and approaches. This page gives background about why things are done the way they are, to help communicate to folks new to the project what has been tried, and why it was not chosen.

## Overview

The LumenPnP feeders are a mostly 3D printed tape feeder capable of reliably feeding 0402 components. They are designed to easily mount to a 20mm x 20mm v-slot aluminum extrusion. There are two motors in the system: a drive motor and a peel motor. The drive motor is a right-angle N20 motor with an encoder, giving us incredibly high resolution control of the motor's position. The peel motor is a standard N20 motor that peels film using a few 3D printed gears. The feeder has two buttons for tape loading and manual control. A third button puts the feeder into bootloader mode, allowing anyone to flash new firmware using a common FTDI USB to UART converter.

Feeders connect to power and communication when mounted to a rail through a "slot." Slots all connect back to a LumenPnP motherboard using an IDC cable. Each slot has a 1-Wire EEPROM onboard which holds the slot's address. This address is used to tell OpenPnP where a certain feeder has been mounted. All communication back to the motherboard happens over RS-485 using the Photon Protocol.

## Drive Subsystem

The Drive Subsystem is in charge of moving the tape forward at very precise increments. A right-angle N20 motor with a 1:1030 ratio gearbox was chosen for this task. This gear ratio provide a tremendous amount of torque for the size of the motor. An encoder on the back shaft of the motor has 14 ticks per revolution, which results in a theoretical 8.8 micron tape movement resolution. Of course, there are many other factors at play that reduce how precise the feeder is actually able to position tape, but the encoder is plenty precise enough for our application.

### Why not stepper motors?

While stepper motors are great for high-precision movement in systems like a 3D printer, they're less optimal for a small device like a feeder.

One of the main considerations with a feeder design is its width. The thinner a feeder is, the more of them you can fit on a given machine, and the more useful they become. Steppers thin enough to fit within a reasonably sized feeder suffer from a lack of torque, and would likely skip steps trying to advance tape. This is especially true when using microstepping, which would likely be necessary for the small movements required in a feeder.

Steppers also lack absolute positioning. All of their movements are relative, and without an encoder or a driver that supports skipped-step detection, we wouldn't know if the stepper has skipped steps.

It's possible to use a pancake stepper motor with a high-torque gearbox and a driver like the TMC2209 for skipped step detection, but a small DC motor with an encoder used fewer parts and had fewer points of failure.

### Why not an external gearbox/gear system?

Many feeder designs utilize a custom gear system that is mounted within the feeder itself. This can provide incredibly high torque if the gearing ratio is high enough.

We chose to avoid this for two main reasons. The first is that we trust the metal gearbox in a right-angle N20 motor much more than a (likely plastic) gear system that we designed. The N20 "standard" is ubiquitous and thoroughly tested. If we can keep our entire gearing system in metal gears, and in an easily sourced, highly tested part, it's a huge win for accessibility and reliability.

The second reason is ease of sourcing/building. If we used "off the shelf" nylon gears, not only would using a pre-existing gear be a huge design limitation, but also they might be difficult to source for the hobbyist or builder. If we went with totally custom gears, this makes the sourcing issue even worse. And even if folks could print them at home, the resolution required for the precise tape movement would be so high that the print would likely have to be SLA.

### Why not an encoder on the drive wheel itself?

In early versions of the feeder, we used a reflective sensor to watch for voids in the drive wheel. Putting our closed-loop feedback system on the last step of the drivetrain is great in theory. Monitoring as close to the actual metric you're controlling will provide a tighter coupling between your independent and dependent variables. There are three reasons we ended up switching to a magnetic encoder on the motor's back shaft.

The first is the low precision of the reflective sensor. The low/high and high/low threshold transition point of the reflective sensor was entirely dependent on how precise the cutout of the drive wheel was. Given that the wheels are made from PCBs which do not have a high tolerance with edge cuts, precision was very low. The sensor itself was also very noisy, requiring lots of signal processing to provide a useful signal. Early tests showed that *each feeder* would need its own custom calibration, which would be a tremendous effort, and discourage hacking.

On the other hand, a magnetic encoder provides 14 ticks per revolution of the motor. Because we are gearing down the motor with such a high ratio, we get an astronomically high precision of our gearbox output.

`14 ticks / 1 motor revolution * 1030 motor revolution / 1 gearbox revolution = 14420 ticks / 1 gearbox revolution`

This means that with a 128mm wheel circumference (32 teeth with 4mm spacing), we have a theoretical 8.877 micron tape positioning resolution. In practice, small tolerance stackups in the gearbox and drive wheel geometry mean we aren't this precise with actual tape position, but it's much better than the 200-500 or so micron resolution we saw with the reflective sensor.

The second reason is that any backlash error in the N20 gearbox can be accounted for by only positioning **from one side**. Backlash is only evident when there exists a reversal of the axis direction. If we only ever approach our position from one direction, backlash does not play into the tape positioning. Even when feeding backwards, we simply overshoot the final position, then approach again from the same direction.

The third reason is that with an encoder, we can move arbitrary distances. With a reflective sensor/slotted drive wheel, our resolution is limited to how far apart we've spaced the slots. With an encoder, we can command the feeder to move effectively any distance (provided it's a multiple of 8.877 microns!). This means feeding 1mm increments is fundamentally the same as feeding 4mm increments.

However, this all assumes that the encoder on the output wheel is comprised of a slotted PCB with a reflective sensor. Very thin encoder wheels and sensors exist, and might be a reasonable option for feeder tape positioning. We decided against this approach due to the lower resolution, cost, and likely need for customization that would make homebrew builds more difficult. They're also susceptible to errors due to light bleed.

## Peel Subsystem

The Peel Subsystem is comprised of a standard N20 motor with a 1:210 gear ratio, with a printed worm gear attached to the output shaft. This worm gear interfaces with a gearbox used for peeling the film. The two straight gears that perform the peeling are mounted with a positive interface, ensuring the frame holding them in place is constantly applying pressure between them. This interface is what grips the film and pulls it away from the tape.

### Why not passive peeling?

Passive peeling mechanisms have been shown to work in many instances. Allowing the force of the drive motor to remove and/or guide the film away from the tape is a great way to reduce the part count. We ultimately decided to use a dedicated peel motor instead of a passive approach for a number of reasons.

First, in our tests we found that passive film peeling methods were only reliable with highly-tuned 3D printer settings to make a working geometry. Not only do we want the design to be possible to be built at home, but it also needs to be straightforward to build in a scaled-up manufacturing setting. Scrapping a high volume of prints because of not hitting a very tight dimensional tolerance would be a frustrating and expensive experience for builders and Opulo alike.

Second, we found that passive film peeling worked better with certain types of tape, and worse with others. In situations where the film is lightly attached to the tape, depending on the method of passive film removal, *both* sides of the film could come unstuck from the tape, jamming the feeder. In the situation where the adhesion was high between the film and tape, the drive motor had to work much, much harder to both drive the tape and remove the film.

Our approach not only ensures there's plenty of force present pulling the film from the tape, but it also accommodates any type of tape as defined by the EIA-418 standard. Even if the film is strongly adhered to the tape, or stretches easily and requires more peeling, the flexture holding the peel gears together allows film to slide between the gears when overdriven.

### Aren't you worried about wear/creep with plastic gears and tensioning?

At first, we were. But the benefits of being printable are high, allowing the design to be more accessible. We ran lifetime tests with the peel mechanism and had very good results.

We tested the peel gear box to a quarter million feed cycles, resulting in a kilometer of tape fed, and saw no failure of the gearbox's ability to peel film. There is slight initial wear of the gears that is present within the first 10k cycles, but after this the system hits steady-state.

Even then, we ensured that this part is easily replaceable if need be. It's easily printable at home, and can be replaced in less than a minute with only two screws.

### Why no tension sensor?

We found that it wasn't necessary. Many feeder designs use a switch to detect when the film has reached a sufficient tension, but with our flextured peel gear box, we can simply overdrive the peel mechanism and the film will slip through the gears once it's been peeled fully and can't move any more.

## Voltage Rails

The feeders are powered by 24 volts on the slot. They step this down to 10 volts using a buck converter, then down to 3.3 volts using an LDO.

### Why 24 Volts Bus Voltage?

24 volts was chosen for the bus voltage for a few reasons. First, it's already on the motherboard given that it's the voltage of the power supply for the machine. Second, a higher voltage allows us to send more power over thinner wire. We wanted to use IDC connectors on the slot harness due to the form factor of the slot, and need for many connectors on a single cable. Using 24 volts lets us send more power for a given current limit of the cable.

Next comes the 10 volt buck converter. The N20 motors accept up to 12 volts, but the DRV8837 H-Bridge motor driver chips only accept up to 11 volts. This chip was chosen as the motor driver due to its cost and availability. Other chips with a higher acceptable voltage of course exist, but are much more expensive or can be hard to source. We also looked for 24 volts motors, but the N20 standard does not seem to accept a voltage higher than 12 volts.

Lastly is the 3.3 volt LDO. This is used for the microcontroller, encoder, switches, LEDs, etc.

## Slot Addressing

Each feeder is loaded into a slot, which is a small 3D print and PCB with a 1-Wire EEPROM. When the feeder is loaded, it reads the slot address from the EEPROM. This is what the host uses to address the feeder in most instances.

### Why is this even necessary?

This system isn't strictly necessary for feeders to operate. But it does allow the host to be much more intelligent about how it handles feeders.

If we neglected to have a slot address, then we'd be operating purely on the feeder's UUID. Assuming we've solved the problem of feeder discovery once inserted, the host would have no idea *where* the feeder is after it's mounted. This means that the user needs to manually jog the head to the feeder every time it's inserted in order to set pick position.

If we neglected to have a feeder UUID and only knew what location a feeder was mounted into, we wouldn't be able to keep track of what part the feeder has; we'd just know where it is.

By having both a feeder UUID, and an address for each location, we can dynamically find where a feeder has been inserted and know what part is at that location.

### Why not use voltage dividers?

Voltage dividers are a very inexpensive method of setting a specific value for a microcontroller. The EEPROM might seem like overkill to hold a single address. We chose to avoid voltage dividers for a couple reasons.

Firstly, the likelihood of error was very high. If we want to support up to 255 devices on the bus, we'd need that many addresses, and that many discrete voltages from the divider. With a 3.3 volt starting voltage, that means we have 12.9 millivolts per address. The precision of the resistors used would have to be incredibly high to support this resolution, and we'd be assuming no voltage loss in the spring-pin interface.

Voltage dividers also require that the feeder has an ADC as opposed to just a GPIO pin used for 1-Wire communication.

### Why not use an I2C/SPI EEPROM?

Provided that we were able to find a spring pin connector with two more wires, we absolutely could have! The benefit of 1-Wire is that it only requires one pin for both communication *and* power. I2C devices would require three dedicated pins: SDA, SCL, and VCC. The spring pin interface we chose that allows the entire feeder geometry to work only comes with up to 5 pins. With four already used for RS-485 and power, the one left is allocated to the EEPROM.

## Host Communication

At the top of the communication stack is the host. The host is what decides which feeder needs to feed, and when. OpenPnP acts as the host in this scenario. OpenPnP sends M485 commands to Marlin, and gets data from feeders back as a result. A custom OpenPnP feeder type `PhotonFeeder` supports common Photon commands such as feed, self-identify, initialize, or check for feed status.

Next comes Marlin. Marlin is the firmware on the LumenPnP and relays commands to feeders along an RS-485 bus that connects to every slot. It sends data using the Photon Protocol. A new M command for Marlin's gcode interface, M485, allows a host to send data down the RS-485 bus using Photon, and thereby communicating with feeders.

### Why RS-485?

RS-485 is an electrical standard that is widely used, particularly in industrial environments. Being a differential signal, electromagnetic interference that affects both data lines does not affect the signal integrity.

### Why not CAN?

CAN would have also been a great option for feeder communication. The decision to stick with a custom protocol over RS-485 ended up coming down to the large size of a CAN frame. The arbitration system is a bit overkill for a system with only two types of devices. Bit stuffing, plus a long CRC and end of frame section make the frame lengthy, keeping the bus noisier than necessary. CAN would have worked just fine, we just preferred to spin our own protocol that was exactly what we needed.

### Why not MODBUS?

MODBUS is a great, royalty-free protocol, and does fulfill many project requirements. There are two main reasons we decided to implement our own protocol.

The first is that MODBUS only supports the host initiating a message. We wanted to allow the possibility that feeders themselves can initiate communication back to the host, instead of only ever waiting to be queried.

The second reason is that we are not only working with a device ID, we are working with its slot address, which acts as its location on the machine. Although we could store address data in the feeder and query it with MODBUS, we couldn't scan every possible feeder ID easily. With Photon, we simply ping each of the 50 possible addresses and see which IDs come back. MODBUS is designed for systems where the ID of each device on the bus is known, and likely immutable. Feeders are easily swapped, and need to be easily recognized once inserted. This flexibility is what Photon provides.

## Microcontroller

An STM32F031C6T6 was chosen for the feeder's microcontroller. There were a few requirements of the chip in order to hit the project goals:

* Available and generally low cost (two variables that are a moving target in our modern semiconductor market)
* At least 1 UART for RS-485
* Sufficient GPIO for buttons, LED, encoder, etc.
* Internal UUID for identification by the host

As a bonus, the chip's sibling, the STM32F031K6U6, has the same die as the C6T6 but with a smaller package. This means that by using the smaller chip's GPIO count as a constraint, we have a binary-compatible drop in replacement part to help alleviate availability issues.

Although a separate EEPROM chip or a software-defined UUID could solve the same problem as having a factory-burned UUID, having it in the chip prevents UUID erasure upon reprogramming, and fewer parts in the BOM.
