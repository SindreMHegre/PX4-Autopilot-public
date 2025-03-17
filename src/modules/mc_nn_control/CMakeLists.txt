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


px4_add_module(
	MODULE mc_nn_control
	MAIN mc_nn_control
	COMPILE_FLAGS
	SRCS
		mc_nn_control.cpp
		mc_nn_control.hpp
		control_net.cpp
		control_net.hpp
		allocation_net.cpp
		allocation_net.hpp
	DEPENDS
		px4_work_queue
		mathlib
)
target_link_libraries(mc_nn_control PRIVATE libtensorflow-lite-micro.a)
target_include_directories(mc_nn_control PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tflm
    ${CMAKE_CURRENT_SOURCE_DIR}/tflm/flatbuffers_include
    ${CMAKE_CURRENT_SOURCE_DIR}/tflm/ruy
    ${CMAKE_CURRENT_SOURCE_DIR}/tflm/gemmlowp
    ${CMAKE_CURRENT_SOURCE_DIR}/tflm/signal
    ${CMAKE_CURRENT_SOURCE_DIR}/tflm/kissfft
    ${CMAKE_CURRENT_SOURCE_DIR}/tflm/cmsis/Cortex_DFP/Device/ARMCM7/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/tflm/cmsis/CMSIS/Core/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/tflm/cmsis
    ${CMAKE_CURRENT_SOURCE_DIR}/tflm/cmsis_nn
    ${CMAKE_CURRENT_SOURCE_DIR}/tflm/cmsis_nn/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/tflm/include/13.2.1
    ${CMAKE_CURRENT_SOURCE_DIR}/tflm/include/13.2.1/arm-none-eabi
#     ${CMAKE_CURRENT_SOURCE_DIR}/tflm/tensorflow/lite
#     ${CMAKE_CURRENT_SOURCE_DIR}/tflm/tensorflow/lite/micro
#     ${CMAKE_CURRENT_SOURCE_DIR}/tflm/tensorflow/lite/micro/arena_allocator
#     ${CMAKE_CURRENT_SOURCE_DIR}/tflm/tensorflow/lite/micro/memory_planner
#     ${CMAKE_CURRENT_SOURCE_DIR}/tflm/tensorflow/lite/micro/tflite_bridge
#     ${CMAKE_CURRENT_SOURCE_DIR}/tflm/tensorflow/lite/micro/kernels
#     ${CMAKE_CURRENT_SOURCE_DIR}/tflm/tensorflow/lite/kernels
#     ${CMAKE_CURRENT_SOURCE_DIR}/tflm/tensorflow/lite/kernels/internal
#     ${CMAKE_CURRENT_SOURCE_DIR}/tflm/tensorflow/lite/core/api
#     ${CMAKE_CURRENT_SOURCE_DIR}/tflm/tensorflow/lite/core/c
#     ${CMAKE_CURRENT_SOURCE_DIR}/tflm/tensorflow/lite/schema
#     ${CMAKE_CURRENT_SOURCE_DIR}/tflm/tensorflow/compiler/mlir/lite/core/api
#     ${CMAKE_CURRENT_SOURCE_DIR}/tflm/tensorflow/compiler/mlir/lite/schema
)
