project( v4l2tohttp ) 
cmake_minimum_required( VERSION 2.8.7 ) 

add_subdirectory (v4l2_acquisition_shared_library/) 

#Server applications need all the protection we can get..!
set(CMAKE_C_FLAGS "-fPIE -pie -s -Wl,-z,relro,-z,now -fstack-protector -O2")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules ${CMAKE_MODULE_PATH})
   
add_executable(v4l2tohttp main.c ) 
target_link_libraries(v4l2tohttp pthread rt jpeg AmmarServerDynamic AmmCaptcha V4L2AcquisitionAS )
set_target_properties(v4l2tohttp PROPERTIES DEBUG_POSTFIX "D")
add_dependencies(v4l2tohttp AmmarServer AmmCaptcha)  
       
    set_target_properties(v4l2tohttp PROPERTIES 
                          DEBUG_POSTFIX ""
                          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                         )

#    install(TARGETS AmmarServer
#                    LIBRARY DESTINATION lib
#                    ARCHIVE DESTINATION lib
#                    RUNTIME DESTINATION bin)

