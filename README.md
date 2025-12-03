# PX4 Drone Autopilot with Neural Control

This repository contains the Neural extension to PX4, the docs can be found in docs/advanced. This version does not contain the newest PX4 changes, but works for Ubuntu versions before 24 (only tested with 22)


## Setup SITL

To setup standard PX4 before begining the docs/en/advanced/tflm-setup:

git clone --recurse-submodules -b for_paper https://github.com/SindreMHegre/PX4-Autopilot-public.git

cd PX4-Autopilot-public/

git remote add upstream https://github.com/PX4/PX4-Autopilot.git

git fetch upstream --tags

bash ./Tools/setup/ubuntu.sh

Test that everything works with this command:

make px4_sitl

## For flight controllers
see: docs/en/advanced/tflm-setup in this repo (not the PX4 docs webpage!)
