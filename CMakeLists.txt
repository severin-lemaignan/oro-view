cmake_minimum_required(VERSION 2.8.3)
project(oro-view)

add_definitions(-std=c++11)

find_package(PkgConfig REQUIRED)

find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(Boost COMPONENTS program_options system REQUIRED)

pkg_search_module(FTGL REQUIRED ftgl)
pkg_search_module(JSONCPP REQUIRED jsoncpp)
pkg_search_module(LIBORO REQUIRED liboro)

include_directories(${OPENGL_INCLUDE_DIRS}
                    ${SDL_INCLUDE_DIR}
                    ${SDL_IMAGE_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS}
                    ${FTGL_INCLUDE_DIRS} 
                    ${JSONCPP_INCLUDE_DIRS}
                    ${LIBORO_INCLUDE_DIRS}
                    )
link_directories(${LIBORO_LIBRARY_DIRS})

#option(DEBUG "Enable debug visualizations" ON)
#option(WITH_TOOLS "Compile sample tools" ON)
#option(WITH_ROS "Build ROS nodes -- Requires OpenCV2!" OFF)
#
#if(WITH_ROS)
#
#    find_package(catkin REQUIRED COMPONENTS 
#        roscpp 
#        tf
#        std_msgs
#        visualization_msgs
#        sensor_msgs
#        cv_bridge
#        image_transport
#        image_geometry
#        )
#
#    include_directories(${catkin_INCLUDE_DIRS})
#
#    catkin_package(
#    CATKIN_DEPENDS 
#        tf
#    DEPENDS OpenCV
#    LIBRARIES 
#    )
#endif()

file(GLOB_RECURSE SRC src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.hpp)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} 
                        ${OPENGL_LIBRARIES} 
                        ${SDL_LIBRARY} 
                        ${SDL_IMAGE_LIBRARIES} 
                        ${Boost_LIBRARIES} 
                        ${FTGL_LIBRARIES}
                        ${JSONCPP_LIBRARIES}
                        ${LIBORO_LIBRARIES}
                        )

install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES
    ${HEADERS}
    DESTINATION include/${PROJECT_NAME}
)


