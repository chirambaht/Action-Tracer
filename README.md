# Collection and Modeling

This unit is used to show how data can both be collected from the Pi and sent to the host server on the machine to model the collected data.

## Collection (thread-timing)

This is the code used by the Raspberry Pi code as well as wiring diagram.

## Running Action Tracer

In order to run this software, a few system configurations need to be set. Some of these are mandatory, while others may be ommited. Be sure to install all the dependencies first.

1.  Install all the [dependencies](#dependancies)

2.  Enable the I2C bus:
    `sudo raspi-config` > _Interface Options_ > _I2C_ > _Enable_

3.  (Optionall) I2C speed. This is done by editing your Pi config file as follows:

    > `sudo nano /boot/config.txt`
    > Add the following line to the bottom of the file, if not there:
    > `dtparam=i2c_baudrate=BAUDRATE`, where `BAUDRATE` is your speed (recommended 400000 = 400kHz)
    > If you are using a Pi 3B, 3B+ and Zero W, you need to also add:

        `core_freq=250`

    > Reboot the sytem with `sudo reboot`

## Dependancies

- git: `sudo apt install git`
- Make: `sudo apt install build-essential`
- Protobuf compiler: `sudo apt install protobuf-compiler`
- Python3: `sudo apt install python3`
- SMBus: `sudo apt install python3-smbus python3-dev i2c-tools`
- Wiring Pi: `wget https://github.com/WiringPi/WiringPi/releases/download/2.61-1/wiringpi-2.61-1-armhf.deb` > `sudo dpkg -i wiringpi-2.61-1-armhf.deb`

or just run the dependencies installer, `chmod +x dependancies.sh` > `sudo ./dependancies.sh`.

## Documentation

[![Updating documentation](https://github.com/chirambaht/Action-Tracer/actions/workflows/makefile.yml/badge.svg?branch=collection&event=push)](https://github.com/chirambaht/Action-Tracer/actions/workflows/makefile.yml)
[![Documentation Page](https://github.com/chirambaht/Action-Tracer/actions/workflows/pages/pages-build-deployment/badge.svg?branch=collection)](https://github.com/chirambaht/Action-Tracer/actions/workflows/pages/pages-build-deployment)

View the [documentation](https://chirambaht.github.io/Action-Tracer/).
