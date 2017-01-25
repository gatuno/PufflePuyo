# Automake file for Puffle Puyo

gamedatadir = $(pkgdatadir)/data

bin_PROGRAMS = puffle_puyo
puffle_puyo_SOURCES = puyo.cpp path.c path.h engine.cpp library.cpp map.cpp map.h falling.cpp falling.h sdl_point.h

if MINGW32
puffle_puyo_SOURCES += 
endif

EXTRA_DIST = puffle.rc

if MINGW32
puffle_ico.o: puffle.rc ../data/puffle.ico
	$(WINDRES) $(srcdir)/puffle.rc -O coff -o puffle_ico.o

mingw_ldadd = puffle_ico.o

else # NOT MINGW
mingw_ldadd =
endif

puffle_puyo_CPPFLAGS = -DGAMEDATA_DIR=\"$(gamedatadir)/\" -DLOCALEDIR=\"$(localedir)\" $(AM_CPPFLAGS)
puffle_puyo_CXXFLAGS = $(SDL_CFLAGS) $(SDL_image_CFLAGS) $(SDL_mixer_CFLAGS) $(SDL_ttf_CFLAGS) $(AM_CXXFLAGS)
if MACOSX
# En MAC OS X, hay que ligar/compilar contra los frameworks
puffle_puyo_LDFLAGS = -Wl,-rpath,@loader_path/../Frameworks
else
puffle_puyo_LDADD = $(SDL_LIBS) $(SDL_image_LIBS) $(SDL_mixer_LIBS) $(SDL_ttf_LIBS) $(mingw_ldadd)
endif
LDADD = $(LIBINTL)

#------------------ Packaging rules for Mac OSX ------------------------

bundle_root = $(top_builddir)/etc/macfiles
bundle_name = $(bundle_root)/PufflePuyo.app
# Special target to copy all needed game files to PaddlePuffle.app/Contents/Resources.
# For Mac OS X bundle.
.PHONY: bundle
bundle:
	-rm -rf $(bundle_root)
	mkdir -p $(bundle_name)/Contents/MacOS
	mkdir -p $(bundle_name)/Contents/Resources/data/
	echo "APPL????" > $(bundle_name)/Contents/PkgInfo
	cp $(top_builddir)/etc/Info.plist $(bundle_name)/Contents/
	cp $(top_builddir)/etc/puffle.icns $(bundle_name)/Contents/Resources/
	mkdir -p $(bundle_name)/Contents/Resources/data/images
	cp -R $(top_builddir)/data/images/* $(bundle_name)/Contents/Resources/data/images
	mkdir -p $(bundle_name)/Contents/Resources/data/music
	cp -R $(top_builddir)/data/music/* $(bundle_name)/Contents/Resources/data/music
	mkdir -p $(bundle_name)/Contents/Resources/data/sounds
	cp -R $(top_builddir)/data/sounds/* $(bundle_name)/Contents/Resources/data/sounds
	cp $(top_builddir)/data/*.ttf $(bundle_name)/Contents/Resources/data
	for cat in $(ALL_LINGUAS); do \
	  dir=$(bundle_name)/Contents/Resources/locale/$$cat/LC_MESSAGES; \
	  mkdir -p $$dir; \
	  echo "installing $$cat as $$dir/puffle_puyo.mo"; \
	  cp $(top_builddir)/po/$$cat.gmo $$dir/puffle_puyo.mo; \
	done

# Special target to copy the binary to PaddlePuffle.app/Contents/MacOS.
.PHONY: bundle-fw
bundle-fw: bundle
	cp puffle_puyo $(bundle_name)/Contents/MacOS/puffle_puyo
	mkdir  $(bundle_name)/Contents/Frameworks
	for i in SDL SDL_mixer SDL_image SDL_ttf ; do \
	  cp -a /Library/Frameworks/$${i}.framework $(bundle_name)/Contents/Frameworks/; \
	done
	strip $(bundle_name)/Contents/MacOS/puffle_puyo

# Special target to make a disk image of the bundled app
.PHONY: bundle-dmg
bundle-dmg: bundle-fw
	ln -s /Applications $(bundle_root)/Applications
	hdiutil create -ov -srcfolder $(bundle_root) -volname PufflePuyo -imagekey zlib-level=6 $(top_builddir)/etc/puffle_puyo.dmg

