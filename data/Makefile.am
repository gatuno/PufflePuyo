gamedatadir = $(pkgdatadir)/data

nobase_dist_gamedata_DATA = images/azul_fall.png \
	images/azul_idle_1.png \
	images/azul_idle_2.png \
	images/azul_idle_3.png \
	images/azul_idle_4.png \
	images/azul_put.png \
	images/bubble.png \
	images/pop_1.png \
	images/pop_2.png \
	images/put_1.png \
	images/put_2.png \
	images/put_3.png \
	images/put_4.png \
	images/put_5.png \
	images/put_6.png \
	images/put_7.png \
	images/put_8.png \
	images/put_9.png \
	images/rojo_fall.png \
	images/rojo_idle_1.png \
	images/rojo_idle_2.png \
	images/rojo_idle_3.png \
	images/rojo_idle_4.png \
	images/rojo_put.png \
	images/verde_fall.png \
	images/verde_idle_1.png \
	images/verde_idle_2.png \
	images/verde_idle_3.png \
	images/verde_idle_4.png \
	images/verde_put.png \
	images/amarillo_fall.png \
	images/amarillo_idle_1.png \
	images/amarillo_idle_2.png \
	images/amarillo_idle_3.png \
	images/amarillo_idle_4.png \
	images/amarillo_put.png \
	images/logo.png \
	ccfacefront.ttf

# Instalar los archivos .desktop e iconos
applicationsdir = $(datadir)/applications
applications_DATA = desktop/puffle.desktop

icons16dir = $(datadir)/icons/hicolor/16x16/apps
icons16_DATA = desktop/16x16/puffle.png

icons32dir = $(datadir)/icons/hicolor/32x32/apps
icons32_DATA = desktop/32x32/puffle.png

icons48dir = $(datadir)/icons/hicolor/48x48/apps
icons48_DATA = desktop/48x48/puffle.png

icons64dir = $(datadir)/icons/hicolor/64x64/apps
icons64_DATA = desktop/64x64/puffle.png

icons128dir = $(datadir)/icons/hicolor/128x128/apps
icons128_DATA = desktop/128x128/puffle.png

icons256dir = $(datadir)/icons/hicolor/256x256/apps
icons256_DATA = desktop/256x256/puffle.png

# touch the toplevel icon directory as required by the icon theme spec
install-data-hook:
	touch $(DESTDIR)$(datadir)/icons/hicolor

EXTRA_DIST = \
	desktop/puffle.desktop	\
	desktop/16x16/puffle.png		\
	desktop/32x32/puffle.png		\
	desktop/48x48/puffle.png		\
	desktop/64x64/puffle.png		\
	desktop/128x128/puffle.png		\
	desktop/256x256/puffle.png		\
	puffle.ico
