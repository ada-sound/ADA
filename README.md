# the ADA project

**ADA** - **A**ll **D**igital **A**mp - aims to design a **premium quality hi-fi audio amplifier** (hardware and software) based on full digital amplifier ('**fda**') chips

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

The board must be seen by a computer as a 2 channels 192kHz (or higher) sound card

It shall allow these lossless non-compressed audio formats
- AAC, WMA, OGG, WMA-L, ALAC, OPUS
- high resolution MQA, DSD, FLAC, WAV, AIFF

For later:
- Bluetooth AptX
- HDMI + CEC (TV remote-> amp)
- HDMI + HEC (TV ethernet-> amp)

# first mockup
Built on
- an ST **STM32F4 Discovery** eval board based on an **STM32F4** microcontroller
- a Texas Instrument **TAS3251EVM** eval board, contaning a **TAS3251** FDA chip

See [the mockup description](hardware/mockup-1/README.md) and [the software sources and documentation](software/mockup-1)
