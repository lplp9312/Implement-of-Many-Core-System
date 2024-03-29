# DNN_Conv_Acc_HLS

Design Exploration of Cluster-Based Architecture for DNN Convolution Accelerator with High-Level Synthesis

## Getting started

1. `$ source /usr/cadtool/user_setup/01-cadence_license_set.cshset`  
2. `$ source /usr/cad/cadence/cic_setup/stratus.cshrc`  
3. `$ source /usr/cad/cadence/cic_setup/incisiv.cshrc`  


### Add stratus_env.sh

1. `$ cd $project/`
2. `$ code $project/stratus_env.sh`
```
source /usr/cadtool/user_setup/01-cadence_license_set.cshset  
source /usr/cadtool/user_setup/03-stratus.csh  
source /usr/cad/cadence/cic_setup/incisiv.cshrc  
```
### Run HLS


1. `$ cd stratus/`

#### If Makefile.prj was created with a different version of Stratus,please run 'bdw_makegen' command to re-generate Makefile.prj.

2. `$ make sim_B`
3. `$ make sim_V_DPA`

# NNarch

Zhong-Jing Yan's work [Link](https://gitlab.larc-nthu.net/a1245967/tvm-esl/-/tree/hls_testcase)
