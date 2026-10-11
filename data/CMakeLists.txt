install(
    FILES menubar.xsd settings.xsd
    DESTINATION ${CMAKE_INSTALL_DATADIR}/xml/easyqt
)
install(
    FILES example-menubar.xml example-settings.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/easyqt
)
