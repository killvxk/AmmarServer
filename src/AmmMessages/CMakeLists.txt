project( AmmMessages ) 
cmake_minimum_required( VERSION 2.8.7 )

set(CMAKE_CXX_FLAGS "-fPIC -march=native -mtune=native -O3 -fexpensive-optimizations -s") 
# Extend cmake's module path
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules ${CMAKE_MODULE_PATH})
   
add_executable(AmmMessages 
main.c 
messageGenerator.c 
messageGenerator.h
centralIncludeGenerator.c
centralIncludeGenerator.h 
../StringRecognizer/fastStringParser.c 
../StringRecognizer/fastStringParser.h
../InputParser/InputParser_C.c
../InputParser/InputParser_C.h
)  
#set_target_properties(AmmMessages PROPERTIES DEBUG_POSTFIX "D") 
        

set_target_properties(AmmMessages PROPERTIES 
                       #DEBUG_POSTFIX "D"
                       ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                       LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                       RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                     )


add_custom_command(TARGET AmmMessages
        POST_BUILD
        COMMAND /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/generateSampleOutput.sh
        )

add_subdirectory (output)

#    install(TARGETS AmmarServer
#                    LIBRARY DESTINATION lib
#                    ARCHIVE DESTINATION lib
#                    RUNTIME DESTINATION bin)

