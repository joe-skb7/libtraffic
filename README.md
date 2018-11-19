# libtrl

## Description

C library for controlling traffic lights on GlobalLogic automotive hackathon.

Traffic lights are wired to interface board (it's in development right now,
will be published as Open Hardware on GitHub later). The chain is next:

    PC -> FT232 cable -> interface board -> traffic lights

So by issuing this lib API routines you can switch traffic lights colors
(red/green). Basically, when running this library functions, you specify
desired color for chosen traffic light number. The library will generate
correct serial code, send it via FT232 cable GPIO lines to shift register on
interface board, which in turn will enable corresponding LED lines, so that
desired traffic light displays desired color.

Details will come further.

## Links

[1] https://www.globallogic.com/ua/news/globallogic-kyiv-smart-city-hackathon/

[2] https://hackaday.com/2009/09/22/introduction-to-ftdi-bitbang-mode/
