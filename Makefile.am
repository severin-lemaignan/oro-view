ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = oroview

oroview_SOURCES = \
	src/core/bounds.h \
	src/core/camera.cpp src/core/camera.h \
	src/core/display.cpp src/core/display.h \
	src/core/extensions.cpp src/core/extensions.h \
	src/core/frustum.cpp src/core/frustum.h \
	src/core/fxfont.cpp src/core/fxfont.h \
	src/core/logger.cpp src/core/logger.h \
	src/core/pi.h \
	src/core/plane.cpp src/core/plane.h \
	src/core/quadtree.cpp src/core/quadtree.h \
	src/core/resource.cpp src/core/resource.h \
	src/core/sdlapp.cpp src/core/sdlapp.h \
	src/core/seeklog.cpp src/core/seeklog.h \
	src/core/stringhash.cpp src/core/stringhash.h \
	src/core/texture.cpp src/core/texture.h \
	src/core/utf8/checked.h \
	src/core/utf8/core.h \
	src/core/utf8/unchecked.h \
	src/core/utf8/utf8.h \
	src/core/vectors.h \
	src/node.cpp src/node.h \
	src/node_renderer.cpp src/node_renderer.h \
	src/oroview.cpp src/oroview.h \
	src/main.cpp \
	src/edge.cpp src/edge.h \
	src/edge_renderer.cpp src/edge_renderer.h \
	src/graph.cpp src/graph.h \
	src/zoomcamera.cpp src/zoomcamera.h

CPPFLAGS = -DSDLAPP_RESOURCE_DIR=\"$(pkgdatadir)\"

dist_pkgdata_DATA = data/beam.png data/bloom.tga data/file.png data/no_photo.png data/gource.style

install-data-hook:
	mkdir -p -m 755 ${DESTDIR}/$(mandir)/man1
	gzip -cf9 data/oroview.1 > $(DESTDIR)$(mandir)/man1/oroview.1.gz

uninstall-hook:
	rm -f $(DESTDIR)$(mandir)/man1/oroview.1.gz

if FONTDIR
CPPFLAGS += -DSDLAPP_FONT_DIR=\"$(oroviewfontdir)\"
else
fontsdir = $(pkgdatadir)/fonts
dist_fonts_DATA = data/fonts/README data/fonts/FreeSans.ttf
endif

