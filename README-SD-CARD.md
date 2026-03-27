CMakeLists.txt — MC_NN_EMBED_MODEL cache variable (default ON). When ON, control_net.cpp is compiled in and the MC_NN_EMBED_MODEL preprocessor flag is defined. When OFF, neither     
  happens.                                                                                                                                                                              
                                                                                                                                                                                        
  mc_nn_control.hpp — control_net.hpp include is guarded by #ifdef MC_NN_EMBED_MODEL. A _model_buffer heap pointer is added for the SD card path.                                       
                                                                                                                                                                                      
  mc_nn_control.cpp — InitializeNetwork() branches at compile time: embedded path is unchanged, SD card path reads /fs/microsd/nn_control/control_net.tflite into _model_buffer and     
  passes it to tflite::GetModel(). Buffer is freed in the destructor.                                                                                                                 
                                                                                                                                                                                        
  To build with SD card loading:                                                                                                                                                        
  cmake -DMC_NN_EMBED_MODEL=OFF ...                                                                                                                                                   
  # or in the build directory:                                                                                                                                                          
  cmake -DMC_NN_EMBED_MODEL=OFF -B build/px4_sitl_default                                                                                                                               
                                                                                                                                                                                      
  Place the .tflite file on the SD card (or in SITL's virtual FS) at nn_control/control_net.tflite. 
