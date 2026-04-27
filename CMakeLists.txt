#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(myDSSSD)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})
#JMQ 20052014
execute_process(COMMAND echo Geant4_USE_FILE=${Geant4_USE_FILE})
execute_process(COMMAND echo PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${Geant4_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#-------------------------------------------------------------------------------
#JMQ 23032014 para que me cree el ejecutable en el $PATH
#
#set(G4WORKDIR $ENV{G4WORKDIR})
#if(DEFINED G4WORKDIR)
#  set(EXECUTABLE_OUTPUT_PATH $ENV{G4WORKDIR}/bin/Linux-g++)
#  execute_process(COMMAND echo EXECUTABLE_OUTPUT_PATH=${EXECUTABLE_OUTPUT_PATH})
#endif(DEFINED G4WORKDIR)


##Added by Valcayne to run in local even when G4WORKDIR is defined
set(MyDSSSD_SCRIPTS)

#
#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(myDSSSD myDSSSD.cc ${sources} ${headers})
target_link_libraries(myDSSSD ${Geant4_LIBRARIES} )
execute_process(COMMAND echo Geant4_LIBRARIES=${Geant4_LIBRARIES})
#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build myC6D6. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(myDSSSD_SCRIPTS
    vis.mac run.mac
     )
# exo1.mac exo2.mac gui.mac newgeom.mac rndmSeed.mac run1.mac run2.mac vis.mac icons.mac
#    visTutor/exN03Tree0.mac visTutor/exN03Vis12.loop visTutor/exN03Vis12.mac visTutor/exN03Vis13.loop visTutor/exN03Vis13.mac visTutor/exN03Vis5.mac visTutor/g4Xt.xrm visTutor/heprep.mac
 # )

foreach(_script ${MyDSSSD_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Add program to the project targets
# (this avoids the need of typing the program name after make)
#
add_custom_target(DSSSD DEPENDS myDSSSD)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS myDSSSD DESTINATION bin)
