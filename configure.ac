AC_INIT([pidget],[0.1],[garrettmarcinak@gmail.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/main.c])

# Checks for libraries
PKG_CHECK_MODULES([XCB], [xcb-icccm xcb-image xcb-aux xcb-errors])
PKG_CHECK_MODULES([PNG], [libpng])
#PKG_CHECK_MODULES([EV], [libev])
PKG_CHECK_MODULES([CYAML], [libcyaml])

# Export flags
AC_SUBST([XCB_CFLAGS])
AC_SUBST([XCB_LIBS])
AC_SUBST([PNG_CFLAGS])
AC_SUBST([PNG_LIBS])
#AC_SUBST([EV_CFLAGS])
#AC_SUBST([EV_LIBS])
AC_SUBST([CYAML_CFLAGS])
AC_SUBST([CYAML_LIBS])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

