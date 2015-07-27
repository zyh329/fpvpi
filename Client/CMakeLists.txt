project(fpvpi_client)
cmake_minimum_required(VERSION 2.8)

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

include_directories (${GTKMM_INCLUDE_DIRS})
link_directories (${GTKMM_LIBRARY_DIRS})
add_executable(fpvpi_client main.cpp)
#add_definitions (${GTK2_CFLAGS_OTHER})
target_link_libraries(fpvpi_client ${OpenCV_LIBS})
target_link_libraries(fpvpi_client ${GTKMM_LIBRARIES})
