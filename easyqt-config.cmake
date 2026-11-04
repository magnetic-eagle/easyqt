find_package(Qt6 COMPONENTS Core Widgets UiTools REQUIRED)
find_package(pugixml REQUIRED)

include(${CMAKE_CURRENT_LIST_DIR}/easyqt_targets.cmake)

if(NOT TARGET easyqt::easyqt)
	message(FATAL_ERROR "easyqt::easyqt target not found. Ensure the library is installed and installed targets are exported.")
endif()
