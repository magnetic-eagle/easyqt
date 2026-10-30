add_library(easyqt
	${EASYQT_MOC_HEADERS}
	application.cxx
	commands.cxx
	viewedflowlayout.cxx
	flowlayout.cxx
	logging.cxx
	menubar.cxx
	pathentry.cxx
	scrollarea.cxx
	settingsdialog.cxx
	spinner.cxx
	uiloader.cxx
	utils.cxx
)
add_library(easyqt::easyqt ALIAS easyqt)
target_link_libraries(
	easyqt
	PUBLIC
	Qt6::Core
	Qt6::Widgets
	Qt6::UiTools
	pugixml::pugixml
)

target_include_directories(
	easyqt
	PUBLIC
		"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
		"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/easyqt>"
		"$<INSTALL_INTERFACE:include>"
)

set_target_properties(easyqt PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION 1
	EXPORT_NAME easyqt
)

install(
	TARGETS easyqt
	EXPORT easyqt_targets
	ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(EXPORT easyqt_targets
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/easyqt
	NAMESPACE easyqt::
)
