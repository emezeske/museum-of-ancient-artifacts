vpath %.cc /home/emezeske/src/c++/common

INCLUDES = -I$(top_srcdir)

AM_CPPFLAGS = \
  -Wall -W -Wshadow -Wpointer-arith -Wcast-qual -Wwrite-strings -Wconversion -Wredundant-decls -Wno-unused \
  -DPKGDATADIR=\"$(pkgdatadir)\" \
  -DDATADIR=\""$(datadir)"\" -D_GNU_SOURCE \
  -DTARGET_FLOAT32_IS_FIXED \
  -isystem/usr/local/openmoko/arm/arm-angstrom-linux-gnueabi/usr/include/c++/4.1.2 \
  -isystem/usr/local/openmoko/arm/arm-angstrom-linux-gnueabi/usr/include/c++/4.1.2/arm-angstrom-linux-gnueabi \
  -isystem/home/emezeske/src/c++/common \ 
  -isystem/home/emezeske/src/openmoko/Box2D/Include

AM_CFLAGS = -Wall -pedantic -std=c99 @DEPENDENCIES_CFLAGS@

AM_LDFLAGS = -L/home/emezeske/src/openmoko/Box2D/Source/Gen/fixed/ -lbox2d

# add the name of your application
bin_PROGRAMS = marble-game

# add the sources to compile for your application
marble_game_SOURCES = \
  main.cc accelerometer.cc vibrator.cc utilities.cc sdl_interface.cc game_application.cc

#marble_game_LDADD = @DEPENDENCIES_LIBS@ -lSDL
marble_game_LDADD = -lSDL -lSDL_image

MAINTAINERCLEANFILES = config.h.in Makefile.in

