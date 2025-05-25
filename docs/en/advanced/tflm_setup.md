# Setup of TFLM

Building PX4 with TensorFlow Lite Micro breaks some of the other standard builds since it requires a change of the toolchain. Therefore it does not build directly when you clone it, but this step-by-step guide will take you through how to build PX4 with TFLM on your own computer.

:::warning
This is an experimental setup. It might break other parts of PX4. All flying at your own risk.
:::

:::info
This guide assumes that you can build PX4 locally from before. So if you have not installed the standard toolchain, please do so first: [Initial Setup](../dev_setup/config_initial.md)
:::

1. First you need to add TFLM as a submodule:

	```sh
	git submodule add -b main https://github.com/tensorflow/tflite-micro.git src/lib/tflm/tflite_micro/
	```

1. Then we need to install the TFLM dependencies. This is automatically done when you build it as a static library, enter the tflite-micro folder and do the following command:

	```sh
	cd src/lib/tflm/tflite_micro
	```
	```sh
	make -f tensorflow/lite/micro/tools/make/Makefile TARGET=cortex_m_generic TARGET_ARCH=cortex-m7 microlite
	```

1. While this is building (it can take a couple of minutes) we can some other changes. The toolchain file in platforms/nuttx/cmake/Toolchain-arm-none-eabi.cmake needs to be edited. In this file you need to add your local path to the PX4-Autopilot repo. This line is marked with a TODO comment.

1. PX4 excludes standard libraries by default, if they are enabled they will break the nuttx build. To get around this we extract some of the standard library header files. This needs to be done after the TFLM make command is finished.
	```sh
	cd src/lib/tflm
	cp -r tflite_micro/tensorflow/lite/micro/tools/make/downloads/gcc_embedded/arm-none-eabi/include/c++/13.2.1/ include/13.2.1
	rm include/13.2.1/arm-none-eabi/bits/ctype_base.h
	cp ../../modules/mc_nn_control/setup/ctype_base.h include/13.2.1/arm-none-eabi/bits/
	cd ../../..
	```

1. If you want to include the neural network controller module onto a new board, add:

	```
	CONFIG_LIB_TFLM=y
	CONFIG_MODULES_MC_NN_CONTROL=y
	```

	to your .px4board file. There are three pre-made board config files where other modules are removed to make sure the entire executable fits in the flash memory of the boards. These are: px4_sitl_neural, px4_fmu-v6c_neural and mro_pixracerpro_neural

1. Now everything should be set up and you can build it using the standard make commands:

	```sh
	make px4_sitl_neural
	```
