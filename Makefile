
.PHONY: all clean web

all:  build/web/index.html


build/semafor.kicad_pcb: 
	kikit panelize grid --space 2.5 --gridsize 4 1 --tabwidth 8 --tabheight 3 \
		--htabs 1 --vtabs 2 --mousebites 0.5 1 0.25 --radius 1 --tolerance 30  \
		ele/semaforBoard/semaforBoard.kicad_pcb build/semaforBoard-panel.kicad_pcb


%-gerber: %.kicad_pcb
	kikit export gerber $< $@

%-gerber.zip: %-gerber
	zip -j $@ `find $<`

web: build/web/index.html

build:
	mkdir -p build

build/web: build
	mkdir -p build/web

build/web/index.html: build/web build/semafor.kicad_pcb
	kikit present boardpage \
		-d README.md \
		--name "Semafor" \
		-b "Semafor Board" " " ele/semaforBoard/semaforBoard.kicad_pcb \
		-b "Semafor Board Panelized" " " build/semaforBoard-panel.kicad_pcb  \
		--repository 'https://github.com/RoboticsBrno/semafor' \
		build/web


clean:
	rm -r build