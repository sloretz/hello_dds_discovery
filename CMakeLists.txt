project(hello_dds_discovery LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.16)

find_package(CycloneDDS REQUIRED)

idlc_generate(TARGET hw_idl_cyclonedds FILES "HelloWorld.idl")

add_executable(cyclonedds_publisher
    cyclonedds/cyclonedds_publisher.c)
add_executable(cyclonedds_subscriber
    cyclonedds/cyclonedds_subscriber.c)

target_link_libraries(cyclonedds_publisher hw_idl_cyclonedds CycloneDDS::ddsc)
target_link_libraries(cyclonedds_subscriber hw_idl_cyclonedds CycloneDDS::ddsc)

install(TARGETS cyclonedds_publisher cyclonedds_subscriber DESTINATION bin)


find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)

add_library(hw_idl_fastdds STATIC
    fastdds/HelloWorld.cxx
    fastdds/HelloWorldPubSubTypes.cxx
)
target_link_libraries(hw_idl_fastdds PUBLIC
    fastrtps
    fastcdr
)

add_executable(fastdds_publisher
    fastdds/fastdds_publisher.cpp)
target_compile_definitions(fastdds_publisher PRIVATE
    $<$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">:__DEBUG>
)
target_link_libraries(fastdds_publisher fastrtps fastcdr hw_idl_fastdds)

add_executable(fastdds_subscriber
    fastdds/fastdds_subscriber.cpp)
target_compile_definitions(fastdds_subscriber PRIVATE
    $<$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">:__DEBUG>
)
target_link_libraries(fastdds_subscriber fastrtps fastcdr hw_idl_fastdds)

install(TARGETS fastdds_publisher fastdds_subscriber DESTINATION bin)
