# libtrl

## Overview

C library for controlling traffic lights on GlobalLogic automotive hackathon
(see [1]).

Traffic lights are wired to interface board [3]. The chain is next:

    PC -> interface board -> traffic light boards

So by issuing this lib API routines you can switch traffic lights colors
(red/green). Basically, when running this library functions, you specify
desired color for chosen traffic light number. The library will generate
correct serial code, send it via FT232 GPIO lines to shift register on
interface board, which in turn will enable corresponding LED lines, so that
desired traffic light displays desired color.

Hardware files for interface board and traffic light PCB can be found here: [3].

## Prebuilt packages

TODO

## Building the library

1. Install dependencies:

```
    $ sudo aptitude install libftdi1-2 libftdi1-dev devscripts pbuilder
```

2. Build and install Debian package:

```
    $ make debian
    $ sudo dpkg -i ../*.deb
```

## udev rules

TODO

## Authors

**Sam Protsenko**

## License

The project is licensed under the GPLv2.

## References

[1] https://www.globallogic.com/ua/news/globallogic-kyiv-smart-city-hackathon/

[2] https://hackaday.com/2009/09/22/introduction-to-ftdi-bitbang-mode/

[3] https://github.com/joe-skb7/traffic-light-pcb
