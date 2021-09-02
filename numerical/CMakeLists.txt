file(GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach(source ${SOURCE_FILES})
    string(REPLACE ".cpp" "" target ${source})
    add_executable(${target} ${source})
endforeach(source)