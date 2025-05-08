/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/
/**
 * @file mc_nn_test.cpp
 * Multicopter Neural Network Testing module, from position setpoints to actuator motors.
 *
 * @author Sindre Meyer Hegre <sindre.hegre@gmail.com>
 */
#include "mc_nn_testing.hpp"

MulticopterNeuralNetworkTesting::MulticopterNeuralNetworkTesting() :
	ModuleParams(nullptr),
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::test1)
{
}

 MulticopterNeuralNetworkTesting::~MulticopterNeuralNetworkTesting()
 {
	perf_free(_loop_perf);
	perf_free(_loop_interval_perf);
 }


 bool MulticopterNeuralNetworkTesting::init()
 {
	ScheduleOnInterval(100000); // 100 000 us interval, 10 Hz rate

	return true;
 }

void MulticopterNeuralNetworkTesting::PopulateSetpoints()
{

	if (_parameter_update_sub.updated()) {
		parameter_update_s param_update;
		_parameter_update_sub.copy(&param_update);
		updateParams();
	}


	// This function populates the setpoints for the controller.
	float distance = _param_dist.get();

	trajectory_setpoint_s trajectory_setpoint0;
	trajectory_setpoint0.position[0] = 0.0f;
	trajectory_setpoint0.position[1] = 0.0f;
	trajectory_setpoint0.position[2] = -1.0f;

	trajectory_setpoint_s trajectory_setpoint1;
	trajectory_setpoint1.position[0] = distance;
	trajectory_setpoint1.position[1] = distance;
	trajectory_setpoint1.position[2] = -1.0f;

	trajectory_setpoint_s trajectory_setpoint2;
	trajectory_setpoint2.position[0] = distance;
	trajectory_setpoint2.position[1] = -distance;
	trajectory_setpoint2.position[2] = -1.0f;

	trajectory_setpoint_s trajectory_setpoint3;
	trajectory_setpoint3.position[0] = -distance;
	trajectory_setpoint3.position[1] = -distance;
	trajectory_setpoint3.position[2] = -1.0f;

	trajectory_setpoint_s trajectory_setpoint4;
	trajectory_setpoint4.position[0] = -distance;
	trajectory_setpoint4.position[1] = distance;
	trajectory_setpoint4.position[2] = -1.0f;

	_trajectory_setpoints[0] = trajectory_setpoint0;
	_trajectory_setpoints[1] = trajectory_setpoint1;
	_trajectory_setpoints[2] = trajectory_setpoint2;
	_trajectory_setpoints[3] = trajectory_setpoint3;
	_trajectory_setpoints[4] = trajectory_setpoint4;

}

 int MulticopterNeuralNetworkTesting::task_spawn(int argc, char *argv[])
 {
	// This function loads the model, sets up the interpreter, allocates memory for the model's tensors, and prepares the input data.
	MulticopterNeuralNetworkTesting *instance = new MulticopterNeuralNetworkTesting();

	if (instance) {
		 _object.store(instance);
		 _task_id = task_id_is_work_queue;


		 if (instance->init()) {

			instance->PopulateSetpoints();
			 return PX4_OK;

		 } else {
			 PX4_ERR("init failed");
		 }

	} else {
		 PX4_ERR("alloc failed");
	}

	delete instance;
	_object.store(nullptr);
	_task_id = -1;

	return PX4_ERROR;
 }

 void MulticopterNeuralNetworkTesting::Run()
 {
	perf_begin(_loop_perf);

	if (_first_run == 0) {
		_first_run = hrt_absolute_time();
	}

	int time_since_start = hrt_absolute_time() - _first_run;

	int32_t time = _param_time.get();

	if (should_exit()) {
		ScheduleClear();
		exit_and_cleanup();
		return;
	}

	int direction[4] = {1, 2, 3, 4};
	if ( _param_direction.get() == 0) {
		direction[0] = 4;
		direction[1] = 3;
		direction[2] = 2;
		direction[3] = 1;
	}

	trajectory_setpoint_s trajectory_setpoint;
	trajectory_setpoint.timestamp = hrt_absolute_time();
	trajectory_setpoint.yaw = 0.0f;
	trajectory_setpoint.yawspeed = 0.0f;

	if (time_since_start < time*2 || _param_hover.get() == 1) {
		trajectory_setpoint.position[0] = _trajectory_setpoints[0].position[0];
		trajectory_setpoint.position[1] = _trajectory_setpoints[0].position[1];
		trajectory_setpoint.position[2] = _trajectory_setpoints[0].position[2];
	} else if (time_since_start < time*3) {
		trajectory_setpoint.position[0] = _trajectory_setpoints[direction[0]].position[0];
		trajectory_setpoint.position[1] = _trajectory_setpoints[direction[0]].position[1];
		trajectory_setpoint.position[2] = _trajectory_setpoints[direction[0]].position[2];
	} else if (time_since_start < time*4) {
		trajectory_setpoint.position[0] = _trajectory_setpoints[direction[1]].position[0];
		trajectory_setpoint.position[1] = _trajectory_setpoints[direction[1]].position[1];
		trajectory_setpoint.position[2] = _trajectory_setpoints[direction[1]].position[2];
	} else if (time_since_start < time*5) {
		trajectory_setpoint.position[0] = _trajectory_setpoints[direction[2]].position[0];
		trajectory_setpoint.position[1] = _trajectory_setpoints[direction[2]].position[1];
		trajectory_setpoint.position[2] = _trajectory_setpoints[direction[2]].position[2];
	} else if (time_since_start < time*6) {
		trajectory_setpoint.position[0] = _trajectory_setpoints[direction[3]].position[0];
		trajectory_setpoint.position[1] = _trajectory_setpoints[direction[3]].position[1];
		trajectory_setpoint.position[2] = _trajectory_setpoints[direction[3]].position[2];
	} else {
		trajectory_setpoint.position[0] = _trajectory_setpoints[0].position[0];
		trajectory_setpoint.position[1] = _trajectory_setpoints[0].position[1];
		trajectory_setpoint.position[2] = _trajectory_setpoints[0].position[2];
	}


	_trajectory_setpoint_pub.publish(trajectory_setpoint);

	perf_end(_loop_perf);
 }

 int MulticopterNeuralNetworkTesting::custom_command(int argc, char *argv[])
 {
	 return print_usage("unknown command");
 }

 int MulticopterNeuralNetworkTesting::print_usage(const char *reason)
 {
	 if (reason) {
		 PX4_ERR("%s", reason);
	 }

	 PRINT_MODULE_DESCRIPTION(
		 R"DESCR_STR(
### Description
Multicopter Neural Network Testing module.
Provides 6 setpoints for the controller to follow.
)DESCR_STR");

	 PRINT_MODULE_USAGE_NAME("mc_nn_testing", "test");
	 PRINT_MODULE_USAGE_COMMAND("start");
	 PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	 return 0;
 }



 extern "C" __EXPORT int mc_nn_testing_main(int argc, char *argv[])
 {
	 return MulticopterNeuralNetworkTesting::main(argc, argv);
 }
