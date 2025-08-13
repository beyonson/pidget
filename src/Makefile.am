bin_PROGRAMS = pidget

pidget_SOURCES = \
    common.c \
    config_parser.c \
    image_proc.c \
    logger.c \
    main.c \
    xcb.c

AM_CFLAGS = \
    -I$(top_srcdir)/include \
    $(XCB_CFLAGS) $(PNG_CFLAGS) $(CYAML_CFLAGS)

LDADD = \
    $(XCB_LIBS) $(PNG_LIBS) $(CYAML_LIBS) -lm -lev
