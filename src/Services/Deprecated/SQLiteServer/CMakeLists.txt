project( SQLiteServer ) 
cmake_minimum_required( VERSION 2.8.7 )

#Server applications need all the protection we can get..!
set(CMAKE_C_FLAGS "-fPIE -pie -s -Wl,-z,relro,-z,now -fstack-protector -O2")

# Extend cmake's module path
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules ${CMAKE_MODULE_PATH})
   
add_executable(SQLiteServer main.c sqlite.c )  
set_target_properties(SQLiteServer PROPERTIES DEBUG_POSTFIX "D") 
target_link_libraries(SQLiteServer sqlite3  pthread rt AmmarServer )
add_dependencies(SQLiteServer AmmarServer)  

    set_target_properties(SQLiteServer PROPERTIES 
                          DEBUG_POSTFIX "D"
                          ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                          LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                         )

#    install(TARGETS AmmarServer
#                    LIBRARY DESTINATION lib
#                    ARCHIVE DESTINATION lib
#                    RUNTIME DESTINATION bin)

