cmake_minimum_required(VERSION 3.16)
project(simplest CXX)

## 0.0. Setting... 
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUERIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-as-needed -ldl -pthread ")

#set(PHYSX_ROOT_DIR "/DUMMY/physx")
set(PHYSX_ROOT_DIR "/home/jovyan/work/PHYSX41/physx") 


## 1.0. HEADERS 
include_directories("${PHYSX_ROOT_DIR}/install/linux/PhysX/include")    #  
include_directories("${PHYSX_ROOT_DIR}/install/linux/PxShared/include") # foundation


## 2.0. LIBRARIES 
link_directories("${PHYSX_ROOT_DIR}/install/linux/PhysX/bin/linux.clang/release") 

set(DUMMIES 
    PhysXExtensions 
    PhysX 
    PhysXPvdSDK 
    PhysXVehicle 
    PhysXCharacterKinematic 
    PhysXCooking 
    PhysXCommon 
    PhysXFoundation 
   )

foreach(DUMMY ${DUMMIES})
   list(APPEND LIBRARIES "${DUMMY}_static_64")
endforeach()


## 3.0. EXECUTABLE 
set(SRCS "../simplest.cpp") 
add_executable(simplest.x ${SRCS})

target_link_libraries(simplest.x ${LIBRARIES})


## 4.0. TESTING  
enable_testing()
add_test(NAME simplest COMMAND simplest.x)
