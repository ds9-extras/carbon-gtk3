# For config.h to be available
include_directories( ${CMAKE_BINARY_DIR} )

########### next target ###############
set( carbon_gtk_demo_SOURCES
    carbonbuttondemowidget.cpp
    carbondemodialog.cpp
    carbondemowidget.cpp
    carbonframedemowidget.cpp
    carboninfowidget.cpp
    carboninputdemowidget.cpp
    carbonlistdemowidget.cpp
    carbonsignal.cpp
    carbonsignalhandler.cpp
    carbonsliderdemowidget.cpp
    carbontimer.cpp
    carbontabdemowidget.cpp
    carbonversion.cpp
    carbongtkdemo_main.cpp
 )

IF( WIN32 )
  add_executable( carbon-gtk3-demo WIN32 ${carbon_gtk_demo_SOURCES} )
ELSE( WIN32 )
  add_executable( carbon-gtk3-demo ${carbon_gtk_demo_SOURCES} )
ENDIF( WIN32 )

target_link_libraries( carbon-gtk3-demo ${GTK_LIBRARIES} ${CAIRO_LIBRARIES} ${X11_X11_LIB} )
install( TARGETS carbon-gtk3-demo DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )

# ########### next target ###############
# set( deco_test_SOURCES carbonversion.cpp carbongtkdeco_main.cpp )
# add_executable( carbon-gtk-deco ${deco_test_SOURCES} )
# target_link_libraries( carbon-gtk-deco ${GTK_LIBRARIES} ${CAIRO_LIBRARIES} ${X11_X11_LIB} ${CMAKE_DL_LIBS} )
# install( TARGETS carbon-gtk-deco DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
