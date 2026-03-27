############################################################################
#
#   Copyright (c) 2025 PX4 Development Team. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 3. Neither the name PX4 nor the names of its contributors may be
#    used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
############################################################################
add_compile_options(-Wno-float-equal)
get_directory_property(FLAGS COMPILE_OPTIONS)
list(REMOVE_ITEM FLAGS "-Wcast-align")
set_directory_properties(PROPERTIES COMPILE_OPTIONS "${FLAGS}")

# When ON (default), the TFLite model is compiled into the firmware binary.
# Set to OFF to load the model from /fs/microsd/nn_control/control_net.tflite at runtime.
set(MC_NN_EMBED_MODEL ON CACHE BOOL "Embed neural network model in firmware (disable to load from SD card)")

if(MC_NN_EMBED_MODEL)
	set(NN_MODEL_SRCS control_net.cpp control_net.hpp)
else()
	set(NN_MODEL_SRCS "")
endif()

px4_add_module(
	MODULE mc_nn_control
	MAIN mc_nn_control
	COMPILE_FLAGS
	SRCS
		mc_nn_control.cpp
		mc_nn_control.hpp
		${NN_MODEL_SRCS}
	MODULE_CONFIG
		mc_nn_control_params.yaml
	DEPENDS
		tensorflow_lite_micro
		px4_work_queue
		mathlib
)
target_link_libraries(mc_nn_control PRIVATE tensorflow_lite_micro)
target_include_directories(mc_nn_control PRIVATE
	${CMAKE_SOURCE_DIR}/src/lib/tensorflow_lite_micro)

if(MC_NN_EMBED_MODEL)
	target_compile_definitions(mc_nn_control PRIVATE MC_NN_EMBED_MODEL)
endif()
