AC_PREREQ(2.53)
AC_INIT(marble-game, 0.0.1, http://www.TODO.org/)
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR(src/main.cc)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AC_PROG_CC_STDC
AC_STDC_HEADERS
AC_PROG_LIBTOOL
AC_PROG_INTLTOOL([0.35.0])

dnl i18n support
GETTEXT_PACKAGE=AC_PACKAGE_NAME
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package])
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT

#PKG_CHECK_MODULES(DEPENDENCIES, libSDL-1.2.0)

AC_SUBST(DEPENDENCIES)

AC_OUTPUT([
Makefile
src/Makefile
data/Makefile
po/Makefile.in
])

