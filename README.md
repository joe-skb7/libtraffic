# libtrl

## Overview

C library for controlling traffic lights on GlobalLogic automotive hackathon
(see [1]).

Hardware design files for traffic lights and interface boards can be found
in project [3].

Connect traffic light to your PC like this:

    PC -> interface board -> traffic light boards (x10 pcs.)

So by issuing this lib API routines you can switch traffic lights colors
(red/green). Basically, when running this library functions, you specify
desired color for chosen traffic light number. The whole chain is functioning
as follows:

- User tells to the library which traffic light he wants to set to which color
- The library will generate correct serial code for shift registers on interface
  board (for chosen colors on traffic lights)
- This code will be sent via USB to FTDI module
- FTDI module works in bit-bang mode, so it will issue corresponding signals to
  its GPIO lines, sending serial code to shift registers
- After receiving that serial code and latch signal, shift registers will
  provide corresponding parallel code on their output pins
- Each output line of shift registers controls one traffic light: 0V will switch
  traffic light to green color, 5V will switch traffic light to red color
- Then each output line from shift register goes to corresponding LED driver,
  turning "green" channel to one state, and "red" channel to another state
- This way desired traffic light displays desired color

Hardware files for interface board and traffic light PCB can be found here: [3].

## Building and installing

1. Install dependencies:

```
$ sudo aptitude install libftdi1-2 libftdi1-dev devscripts pbuilder debhelper
```

2. Build and install Debian package for this library:

```
$ make debian
$ sudo dpkg -i ../*.deb
```

3. Add your user to `dialout` group:

```
$ sudo usermod -a -G dialout $USER
```

and re-login after that.

4. Add udev rules for FTDI chip:

Create `/etc/udev/rules.d/99-usb-serial.rules` file with next content:

```
SUBSYSTEM=="usb", ATTR{idVendor}=="0403", ATTR{idProduct}=="6001", MODE="0660", GROUP="dialout"
```

Now re-load rules:

```
$ sudo udevadm control --reload-rules
$ sudo udevadm trigger
```

Now your library is installed and FTDI will work without root privileges.

See `example/*` for insights on how to use `libtrl` in your app.

## Authors

**Sam Protsenko**

## License

The project is licensed under the GPLv2.

## References

[1] https://www.globallogic.com/ua/news/globallogic-kyiv-smart-city-hackathon/

[2] https://hackaday.com/2009/09/22/introduction-to-ftdi-bitbang-mode/

[3] https://github.com/joe-skb7/traffic-light-pcb

[4] https://www.ftdichip.com/Support/Documents/AppNotes/AN_232R-01_Bit_Bang_Mode_Available_For_FT232R_and_Ft245R.pdf

[5] https://electronics.stackexchange.com/questions/7525/how-can-i-control-128-or-more-solenoids

[6] https://www.arduino.cc/en/Tutorial/ShiftOut
