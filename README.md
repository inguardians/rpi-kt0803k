# rpi-kt0803k

Configure the KT0803 and KT0803K FM radios over i2c on the Raspberry Pi.

## Build and Install

Before building, you will need to install wiringpi

    sudo apt-get update && sudo apt-get install wiringpi

Then run make:

    make
    sudo make install

You should now be able to run the `rpi-kt0803k` command.

## Usage

See the internal help page for usage information:

    rpi-kt0803k --help
