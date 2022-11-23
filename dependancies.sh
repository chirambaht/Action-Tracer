#!/bin/bash 

CYAN='\033[0;36m'
NC='\033[0m' 
 
# Create temp working directory
printf "${CYAN} -----------------------------------\n    Creating temporary working directory \n ----------------------------------- ${NC}\n"
mkdir -p ~/tmp
cd ~/tmp

# Update the Linux environment
printf "${CYAN} -----------------------------------\n    Updating Linux Env \n ----------------------------------- ${NC}\n"
sudo apt update -y

printf "${CYAN} -----------------------------------\n    Downloading Dependencies \n ----------------------------------- ${NC}\n"
# Check if wget and wiringPi are installed
if ! [ -x "$(command -v wget)" ]; then
  printf "Installing wget\n"
  sudo apt install wget -y
fi

if ! [ -x "$(command -v dpkg)" ]; then
  printf "Installing dpkg\n"
  sudo apt install dpkg -y
fi

if ! [ -x "$(command -v gpio)" ]; then
  wget https://github.com/WiringPi/WiringPi/releases/download/2.61-1/wiringpi-2.61-1-armhf.deb -O wiringpi.deb && sudo dpkg -i wiringpi.deb
fi

printf "${CYAN} -----------------------------------\n    Installing Python Packages \n ----------------------------------- ${NC}\n"
# Check python3 is installed
if ! [ -x "$(command -v python3)" ]; then
  printf "Installing python3\n"
  sudo apt install python3 -y
fi

# Check pip3 is installed
if ! [ -x "$(command -v pip3)" ]; then
  printf "Installing pip3\n"
  sudo apt install python3-pip -y
fi

# Install python packages
sudo apt install python3-smbus python3-dev -y

printf "${CYAN} -----------------------------------\n    Downloading Packages \n ----------------------------------- ${NC}\n"
# check if git is installed
if ! [ -x "$(command -v git)" ]; then
  printf "Installing git\n"
  sudo apt install git -y
fi

# check if i2c-tools is installed
if ! [ -x "$(command -v i2cdetect)" ]; then
  printf "Installing i2c-tools\n"
  sudo apt install i2c-tools -y
fi

# check if build-essential is installed
if ! [ -x "$(command -v build-essential)" ]; then
  printf "Installing build-essential\n"
  sudo apt install build-essential -y
fi

# chec if protobuf is installed
if ! [ -x "$(command -v protoc)" ]; then
  printf "Installing protobuf\n"
  sudo apt install protobuf-compiler -y
fi

# clean up
printf "${CYAN} -----------------------------------\n    Cleaning up \n ----------------------------------- ${NC}\n"
sudo apt autoremove -y


printf "${CYAN} \n\nDone, you are ready to use Action Tracer!\n\n"