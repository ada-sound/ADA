# the ADA project

**ADA** - **A**ll **D**igital **A**mp - aims to design a **premium quality hi-fi audio amplifier** (hardware and software) based on the full digital amplifier ('**fda**') chip **TAS3251** from Texas Instruments

The first requirements are:
- 2-way amplifier, about 60W at 8 ohms per voice
- digital sources *only*
  * USB Audio 2.0
  * SPDIF optical
  * SPDIF coaxial
  * HDMI-ARC
- auto-sleep
- auto-select source
- minimal MMI: power-on pushbutton, volume rotactor, audio-source rotactor, LCD panel, status LED

The board must be seen by a computer as a 2 channels 96kHz (or higher) sound card
It shall allow these lossless non-compressed audio formats: WAV, DCD, FLAC, ALAC, AIFF

For later:
- Bluetooth AptX
- HDMI + CEC (TV remote-> amp)
- HDMI + HEC (TV ethernet-> amp)

# first proto

Made from
- a [TAS3251](http://www.ti.com/product/TAS3251) out from a [TAS3251EVM eval board](http://www.ti.com/tool/TAS3251EVM)
- a [STM32F4](https://www.st.com/en/microcontrollers-microprocessors/stm32f4-series.html) out from a [STM32F4 Discovery board](https://www.st.com/en/evaluation-tools/stm32f4discovery.html)
- some goodies like a rotactor, an LCD panel etc.

# roadmap

## before the proto
Make work this:
1. [x] ST->i2c->TAS
2. [x] ST->gpio->DAC, RESET
3. [x] ST->i2s->TAS, TAS in the good mode
4. [ ] ST USB Audio 2
