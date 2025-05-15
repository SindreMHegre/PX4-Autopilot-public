# PX4 Drone Autopilot with Neural Control

This repository contains the Neural extension to PX4, the docs can be found in docs/advanced


## Setup

To setup standard PX4 before begining the docs/en/advanced/tflm-setup:

git clone --recurse-submodules -b for_paper https://github.com/SindreMHegre/PX4-Autopilot-public.git

cd PX4-Autopilot-public/

git remote add upstream https://github.com/PX4/PX4-Autopilot.git

git fetch upstream --tags

bash ./Tools/setup/ubuntu.sh

Test that everything works with this command:

make px4_sitl
