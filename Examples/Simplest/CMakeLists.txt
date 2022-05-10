cmake_minimum_required(VERSION 3.19.3)
project(simplest CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUERIRED ON)

## 0.0. 
SET(${PHYSX_ROOT_DIR} "PHYSX_ROOT_DIR")


## 1.0. HEADERS 
include_directories("${PHYSX_ROOT_DIR}/install/linux/PhysX/include")
include_directories("${PHYSX_ROOT_DIR}/install/linux/PxShared/include")

## 2.0. LIBRARIES 
link_directories("${PHYSX_ROOT_DIR}/PhysX/bin/linux.clang/release") 

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
   list(append LIBRARIES "{DUMMY}_static_64")
endforeach()

## 3.0. EXECUTABLE 
set(SRCS "../simplest.cpp") 
add_executable(simplest.x ${SRCS})

target_link_libraries(simplest.x ${LIBRARIES})

