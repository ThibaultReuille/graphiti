
CC=clang++
CFLAGS=-W -Wall -ansi -Wno-missing-field-initializers -m64 -O2 -Wno-deprecated -g

DUETTO_CC=../duetto/bin/clang++
DUETTO_CFLAGS=-O3

EM_CC=../emscripten/1.16.0/em++
EM_CFLAGS=-std=c++11

INCLUDES=-I./ -I../ -I../raindance -I../raindance/Lib -I../raindance/Lib/glm-0.9.5.4

PYTHON_CFLAGS = `python2.7-config --cflags | sed s/"-mno-fused-madd"//g`
PYTHON_LDFLAGS = `python2.7-config --ldflags`

RESOURCES=resources

NATIVE_OUT=graphiti
WEB_OUT=graphiti.js

DIST=$(NATIVE_OUT)-$(shell date +"%Y%m%d")

JS_EXPORT="\
[\
        '_create', '_destroy', '_initialize', '_start',\
	'_registerScript',\
	\
	'_setAttribute',\
	\
	'_addNode', '_removeNode', '_countNodes', '_getNodeID',\
	'_setNodeLabel', '_getNodeLabel',\
	'_setNodeMark', '_getNodeMark',\
	'_setNodeWeight', '_getNodeWeight',\
	'_setNodeAttribute',\
	\
	'_addLink', '_removeLink', '_countLinks', '_getLinkID',\
	'_getLinkNode1', '_getLinkNode2',\
	'_setLinkAttribute',\
	\
	'_countSelectedNodes', '_getSelectedNode',\
]"

pack:
	( cd ../raindance && make )

	@echo "--- Packing Resources ---"

	mkdir -p Pack
	mkdir -p Pack/Hud

	xxd -i Resources/Hud/pointer.png > Pack/Hud/pointer.png
	xxd -i Resources/Hud/play.png > Pack/Hud/play.png
	xxd -i Resources/Hud/pause.png > Pack/Hud/pause.png
	xxd -i Resources/Hud/expand.png > Pack/Hud/expand.png
	xxd -i Resources/Hud/graph.png > Pack/Hud/graph.png
	xxd -i Resources/Hud/text.png > Pack/Hud/text.png
	xxd -i Resources/Hud/label.png > Pack/Hud/label.png
	xxd -i Resources/Hud/node.png > Pack/Hud/node.png
	xxd -i Resources/Hud/loupe.png > Pack/Hud/loupe.png
	xxd -i Resources/Hud/flag.png > Pack/Hud/flag.png
	xxd -i Resources/Hud/marker.png > Pack/Hud/marker.png
	xxd -i Resources/Hud/bug.png > Pack/Hud/bug.png
	xxd -i Resources/Hud/spheres.png > Pack/Hud/spheres.png
	xxd -i Resources/Hud/eye.png > Pack/Hud/eye.png
	xxd -i Resources/Hud/white-disk.png > Pack/Hud/white-disk.png
	xxd -i Resources/background.png > Pack/background.png

	xxd -i Resources/cube.vert > Pack/cube.vert
	xxd -i Resources/cube.frag > Pack/cube.frag

	xxd -i Resources/quad.vert > Pack/quad.vert
	xxd -i Resources/quad.frag > Pack/quad.frag

	xxd -i Resources/sphere.vert > Pack/sphere.vert
	xxd -i Resources/sphere.frag > Pack/sphere.frag

	xxd -i Resources/globe.vert > Pack/globe.vert
	xxd -i Resources/globe.frag > Pack/globe.frag

	xxd -i Resources/graph.vert > Pack/graph.vert
	xxd -i Resources/graph.frag > Pack/graph.frag

	xxd -i Resources/curve.vert > Pack/curve.vert
	xxd -i Resources/curve.frag > Pack/curve.frag

	xxd -i Resources/world.vert > Pack/world.vert
	xxd -i Resources/world.frag > Pack/world.frag

	xxd -i Resources/disk-white.png > Pack/disk-white.png
	xxd -i Resources/square-white.png > Pack/square-white.png

	xxd -i Resources/sphere.png > Pack/sphere.png
	xxd -i Resources/circle.png > Pack/circle.png
	xxd -i Resources/target.png > Pack/target.png

	mkdir -p Pack/SpaceView
	xxd -i Resources/SpaceView/node-activity.png > Pack/SpaceView/node-activity.png

	mkdir -p Pack/SpaceView/EdgeStyles
	xxd -i Resources/SpaceView/EdgeStyles/circles.png > Pack/SpaceView/EdgeStyles/circles.png
	xxd -i Resources/SpaceView/EdgeStyles/cross.png > Pack/SpaceView/EdgeStyles/cross.png
	xxd -i Resources/SpaceView/EdgeStyles/dashed.png > Pack/SpaceView/EdgeStyles/dashed.png
	xxd -i Resources/SpaceView/EdgeStyles/dots.png > Pack/SpaceView/EdgeStyles/dots.png
	xxd -i Resources/SpaceView/EdgeStyles/zigzag.png > Pack/SpaceView/EdgeStyles/zigzag.png
	xxd -i Resources/SpaceView/EdgeStyles/solid.png > Pack/SpaceView/EdgeStyles/solid.png
	xxd -i Resources/SpaceView/EdgeStyles/squares.png > Pack/SpaceView/EdgeStyles/squares.png
	xxd -i Resources/SpaceView/EdgeStyles/triangles.png > Pack/SpaceView/EdgeStyles/triangles.png

	mkdir -p Pack/WorldView
	xxd -i Resources/WorldView/geoline.vert > Pack/WorldView/geoline.vert
	xxd -i Resources/WorldView/geoline.frag > Pack/WorldView/geoline.frag

	mkdir -p Pack/ParticleView
	xxd -i Resources/ParticleView/node.vert > Pack/ParticleView/node.vert
	xxd -i Resources/ParticleView/node.frag > Pack/ParticleView/node.frag
	xxd -i Resources/ParticleView/physics.cl > Pack/ParticleView/physics.cl

	mkdir -p Pack/Earth
	xxd -i Resources/Earth/black-and-blue.jpg > Pack/Earth/black-and-blue.jpg
	xxd -i Resources/Earth/world-2048x1024.jpg > Pack/Earth/world-2048x1024.jpg
	xxd -i Resources/Earth/miller-2048x1502.jpg > Pack/Earth/miller-2048x1502.jpg
	xxd -i Resources/Earth/miller-2048x1502-bw.jpg > Pack/Earth/miller-2048x1502-bw.jpg
	xxd -i Resources/Earth/globe.png > Pack/Earth/globe.png
	xxd -i Resources/Earth/world-map-icon.png > Pack/Earth/world-map-icon.png

	mkdir -p Pack/Spheres
	xxd -i Resources/Spheres/starfield.png > Pack/Spheres/starfield.png
	xxd -i Resources/Spheres/perlin.jpg > Pack/Spheres/perlin.jpg

	mkdir -p Pack/Particle
	xxd -i Resources/Particle/ball.png > Pack/Particle/ball.png
	xxd -i Resources/Particle/metaball.png > Pack/Particle/metaball.png

	mkdir -p Pack/Countries
	xxd -i Resources/Countries/ad.png > Pack/Countries/ad.png
	xxd -i Resources/Countries/ae.png > Pack/Countries/ae.png
	xxd -i Resources/Countries/af.png > Pack/Countries/af.png
	xxd -i Resources/Countries/ag.png > Pack/Countries/ag.png
	xxd -i Resources/Countries/ai.png > Pack/Countries/ai.png
	xxd -i Resources/Countries/al.png > Pack/Countries/al.png
	xxd -i Resources/Countries/am.png > Pack/Countries/am.png
	xxd -i Resources/Countries/an.png > Pack/Countries/an.png
	xxd -i Resources/Countries/ao.png > Pack/Countries/ao.png
	xxd -i Resources/Countries/ar.png > Pack/Countries/ar.png
	xxd -i Resources/Countries/as.png > Pack/Countries/as.png
	xxd -i Resources/Countries/at.png > Pack/Countries/at.png
	xxd -i Resources/Countries/au.png > Pack/Countries/au.png
	xxd -i Resources/Countries/aw.png > Pack/Countries/aw.png
	xxd -i Resources/Countries/ax.png > Pack/Countries/ax.png
	xxd -i Resources/Countries/ba.png > Pack/Countries/ba.png
	xxd -i Resources/Countries/bb.png > Pack/Countries/bb.png
	xxd -i Resources/Countries/bd.png > Pack/Countries/bd.png
	xxd -i Resources/Countries/be.png > Pack/Countries/be.png
	xxd -i Resources/Countries/bf.png > Pack/Countries/bf.png
	xxd -i Resources/Countries/bg.png > Pack/Countries/bg.png
	xxd -i Resources/Countries/bh.png > Pack/Countries/bh.png
	xxd -i Resources/Countries/bi.png > Pack/Countries/bi.png
	xxd -i Resources/Countries/bj.png > Pack/Countries/bj.png
	xxd -i Resources/Countries/bm.png > Pack/Countries/bm.png
	xxd -i Resources/Countries/bn.png > Pack/Countries/bn.png
	xxd -i Resources/Countries/bo.png > Pack/Countries/bo.png
	xxd -i Resources/Countries/br.png > Pack/Countries/br.png
	xxd -i Resources/Countries/bs.png > Pack/Countries/bs.png
	xxd -i Resources/Countries/bt.png > Pack/Countries/bt.png
	xxd -i Resources/Countries/bw.png > Pack/Countries/bw.png
	xxd -i Resources/Countries/by.png > Pack/Countries/by.png
	xxd -i Resources/Countries/bz.png > Pack/Countries/bz.png
	xxd -i Resources/Countries/ca.png > Pack/Countries/ca.png
	xxd -i Resources/Countries/cd.png > Pack/Countries/cd.png
	xxd -i Resources/Countries/cf.png > Pack/Countries/cf.png
	xxd -i Resources/Countries/cg.png > Pack/Countries/cg.png
	xxd -i Resources/Countries/ch.png > Pack/Countries/ch.png
	xxd -i Resources/Countries/ci.png > Pack/Countries/ci.png
	xxd -i Resources/Countries/ck.png > Pack/Countries/ck.png
	xxd -i Resources/Countries/cl.png > Pack/Countries/cl.png
	xxd -i Resources/Countries/cm.png > Pack/Countries/cm.png
	xxd -i Resources/Countries/cn.png > Pack/Countries/cn.png
	xxd -i Resources/Countries/co.png > Pack/Countries/co.png
	xxd -i Resources/Countries/cr.png > Pack/Countries/cr.png
	xxd -i Resources/Countries/cu.png > Pack/Countries/cu.png
	xxd -i Resources/Countries/cv.png > Pack/Countries/cv.png
	xxd -i Resources/Countries/cx.png > Pack/Countries/cx.png
	xxd -i Resources/Countries/cy.png > Pack/Countries/cy.png
	xxd -i Resources/Countries/cz.png > Pack/Countries/cz.png
	xxd -i Resources/Countries/de.png > Pack/Countries/de.png
	xxd -i Resources/Countries/dj.png > Pack/Countries/dj.png
	xxd -i Resources/Countries/dk.png > Pack/Countries/dk.png
	xxd -i Resources/Countries/dm.png > Pack/Countries/dm.png
	xxd -i Resources/Countries/do.png > Pack/Countries/do.png
	xxd -i Resources/Countries/dz.png > Pack/Countries/dz.png
	xxd -i Resources/Countries/ec.png > Pack/Countries/ec.png
	xxd -i Resources/Countries/ee.png > Pack/Countries/ee.png
	xxd -i Resources/Countries/eg.png > Pack/Countries/eg.png
	xxd -i Resources/Countries/er.png > Pack/Countries/er.png
	xxd -i Resources/Countries/es.png > Pack/Countries/es.png
	xxd -i Resources/Countries/et.png > Pack/Countries/et.png
	xxd -i Resources/Countries/eu.png > Pack/Countries/eu.png
	xxd -i Resources/Countries/fi.png > Pack/Countries/fi.png
	xxd -i Resources/Countries/fj.png > Pack/Countries/fj.png
	xxd -i Resources/Countries/fk.png > Pack/Countries/fk.png
	xxd -i Resources/Countries/fm.png > Pack/Countries/fm.png
	xxd -i Resources/Countries/fo.png > Pack/Countries/fo.png
	xxd -i Resources/Countries/fr.png > Pack/Countries/fr.png
	xxd -i Resources/Countries/ga.png > Pack/Countries/ga.png
	xxd -i Resources/Countries/gd.png > Pack/Countries/gd.png
	xxd -i Resources/Countries/ge.png > Pack/Countries/ge.png
	xxd -i Resources/Countries/gg.png > Pack/Countries/gg.png
	xxd -i Resources/Countries/gh.png > Pack/Countries/gh.png
	xxd -i Resources/Countries/gi.png > Pack/Countries/gi.png
	xxd -i Resources/Countries/gl.png > Pack/Countries/gl.png
	xxd -i Resources/Countries/gm.png > Pack/Countries/gm.png
	xxd -i Resources/Countries/gn.png > Pack/Countries/gn.png
	xxd -i Resources/Countries/gq.png > Pack/Countries/gq.png
	xxd -i Resources/Countries/gr-cy.png > Pack/Countries/gr-cy.png
	xxd -i Resources/Countries/gr.png > Pack/Countries/gr.png
	xxd -i Resources/Countries/gs.png > Pack/Countries/gs.png
	xxd -i Resources/Countries/gt.png > Pack/Countries/gt.png
	xxd -i Resources/Countries/gu.png > Pack/Countries/gu.png
	xxd -i Resources/Countries/gw.png > Pack/Countries/gw.png
	xxd -i Resources/Countries/gy.png > Pack/Countries/gy.png
	xxd -i Resources/Countries/hk.png > Pack/Countries/hk.png
	xxd -i Resources/Countries/hn.png > Pack/Countries/hn.png
	xxd -i Resources/Countries/hr.png > Pack/Countries/hr.png
	xxd -i Resources/Countries/ht.png > Pack/Countries/ht.png
	xxd -i Resources/Countries/hu.png > Pack/Countries/hu.png
	xxd -i Resources/Countries/id.png > Pack/Countries/id.png
	xxd -i Resources/Countries/ie.png > Pack/Countries/ie.png
	xxd -i Resources/Countries/il.png > Pack/Countries/il.png
	xxd -i Resources/Countries/im.png > Pack/Countries/im.png
	xxd -i Resources/Countries/in.png > Pack/Countries/in.png
	xxd -i Resources/Countries/io.png > Pack/Countries/io.png
	xxd -i Resources/Countries/iq.png > Pack/Countries/iq.png
	xxd -i Resources/Countries/ir.png > Pack/Countries/ir.png
	xxd -i Resources/Countries/is.png > Pack/Countries/is.png
	xxd -i Resources/Countries/it.png > Pack/Countries/it.png
	xxd -i Resources/Countries/je.png > Pack/Countries/je.png
	xxd -i Resources/Countries/jm.png > Pack/Countries/jm.png
	xxd -i Resources/Countries/jo.png > Pack/Countries/jo.png
	xxd -i Resources/Countries/jp.png > Pack/Countries/jp.png
	xxd -i Resources/Countries/ke.png > Pack/Countries/ke.png
	xxd -i Resources/Countries/kg.png > Pack/Countries/kg.png
	xxd -i Resources/Countries/kh.png > Pack/Countries/kh.png
	xxd -i Resources/Countries/ki.png > Pack/Countries/ki.png
	xxd -i Resources/Countries/km.png > Pack/Countries/km.png
	xxd -i Resources/Countries/kn.png > Pack/Countries/kn.png
	xxd -i Resources/Countries/kp.png > Pack/Countries/kp.png
	xxd -i Resources/Countries/kr.png > Pack/Countries/kr.png
	xxd -i Resources/Countries/kw.png > Pack/Countries/kw.png
	xxd -i Resources/Countries/ky.png > Pack/Countries/ky.png
	xxd -i Resources/Countries/kz.png > Pack/Countries/kz.png
	xxd -i Resources/Countries/la.png > Pack/Countries/la.png
	xxd -i Resources/Countries/lb.png > Pack/Countries/lb.png
	xxd -i Resources/Countries/lc.png > Pack/Countries/lc.png
	xxd -i Resources/Countries/li.png > Pack/Countries/li.png
	xxd -i Resources/Countries/lk.png > Pack/Countries/lk.png
	xxd -i Resources/Countries/lr.png > Pack/Countries/lr.png
	xxd -i Resources/Countries/ls.png > Pack/Countries/ls.png
	xxd -i Resources/Countries/lt.png > Pack/Countries/lt.png
	xxd -i Resources/Countries/lu.png > Pack/Countries/lu.png
	xxd -i Resources/Countries/lv.png > Pack/Countries/lv.png
	xxd -i Resources/Countries/ly.png > Pack/Countries/ly.png
	xxd -i Resources/Countries/ma.png > Pack/Countries/ma.png
	xxd -i Resources/Countries/mc.png > Pack/Countries/mc.png
	xxd -i Resources/Countries/md.png > Pack/Countries/md.png
	xxd -i Resources/Countries/me.png > Pack/Countries/me.png
	xxd -i Resources/Countries/mg.png > Pack/Countries/mg.png
	xxd -i Resources/Countries/mh.png > Pack/Countries/mh.png
	xxd -i Resources/Countries/ml.png > Pack/Countries/ml.png
	xxd -i Resources/Countries/mn.png > Pack/Countries/mn.png
	xxd -i Resources/Countries/mo.png > Pack/Countries/mo.png
	xxd -i Resources/Countries/mp.png > Pack/Countries/mp.png
	xxd -i Resources/Countries/mq.png > Pack/Countries/mq.png
	xxd -i Resources/Countries/mr.png > Pack/Countries/mr.png
	xxd -i Resources/Countries/ms.png > Pack/Countries/ms.png
	xxd -i Resources/Countries/mt.png > Pack/Countries/mt.png
	xxd -i Resources/Countries/mu.png > Pack/Countries/mu.png
	xxd -i Resources/Countries/mv.png > Pack/Countries/mv.png
	xxd -i Resources/Countries/mw.png > Pack/Countries/mw.png
	xxd -i Resources/Countries/mx.png > Pack/Countries/mx.png
	xxd -i Resources/Countries/my.png > Pack/Countries/my.png
	xxd -i Resources/Countries/mz.png > Pack/Countries/mz.png
	xxd -i Resources/Countries/na.png > Pack/Countries/na.png
	xxd -i Resources/Countries/ne.png > Pack/Countries/ne.png
	xxd -i Resources/Countries/nf.png > Pack/Countries/nf.png
	xxd -i Resources/Countries/ng.png > Pack/Countries/ng.png
	xxd -i Resources/Countries/ni.png > Pack/Countries/ni.png
	xxd -i Resources/Countries/nl.png > Pack/Countries/nl.png
	xxd -i Resources/Countries/nm.png > Pack/Countries/nm.png
	xxd -i Resources/Countries/no.png > Pack/Countries/no.png
	xxd -i Resources/Countries/np.png > Pack/Countries/np.png
	xxd -i Resources/Countries/nr.png > Pack/Countries/nr.png
	xxd -i Resources/Countries/nu.png > Pack/Countries/nu.png
	xxd -i Resources/Countries/nz.png > Pack/Countries/nz.png
	xxd -i Resources/Countries/om.png > Pack/Countries/om.png
	xxd -i Resources/Countries/pa.png > Pack/Countries/pa.png
	xxd -i Resources/Countries/pf.png > Pack/Countries/pf.png
	xxd -i Resources/Countries/pg.png > Pack/Countries/pg.png
	xxd -i Resources/Countries/ph.png > Pack/Countries/ph.png
	xxd -i Resources/Countries/pk.png > Pack/Countries/pk.png
	xxd -i Resources/Countries/pl.png > Pack/Countries/pl.png
	xxd -i Resources/Countries/pm.png > Pack/Countries/pm.png
	xxd -i Resources/Countries/pn.png > Pack/Countries/pn.png
	xxd -i Resources/Countries/pr.png > Pack/Countries/pr.png
	xxd -i Resources/Countries/pt.png > Pack/Countries/pt.png
	xxd -i Resources/Countries/pw.png > Pack/Countries/pw.png
	xxd -i Resources/Countries/py.png > Pack/Countries/py.png
	xxd -i Resources/Countries/qa.png > Pack/Countries/qa.png
	xxd -i Resources/Countries/ro.png > Pack/Countries/ro.png
	xxd -i Resources/Countries/rs.png > Pack/Countries/rs.png
	xxd -i Resources/Countries/ru.png > Pack/Countries/ru.png
	xxd -i Resources/Countries/rw.png > Pack/Countries/rw.png
	xxd -i Resources/Countries/sa.png > Pack/Countries/sa.png
	xxd -i Resources/Countries/sb.png > Pack/Countries/sb.png
	xxd -i Resources/Countries/sc.png > Pack/Countries/sc.png
	xxd -i Resources/Countries/sd.png > Pack/Countries/sd.png
	xxd -i Resources/Countries/se.png > Pack/Countries/se.png
	xxd -i Resources/Countries/sg.png > Pack/Countries/sg.png
	xxd -i Resources/Countries/sh.png > Pack/Countries/sh.png
	xxd -i Resources/Countries/si.png > Pack/Countries/si.png
	xxd -i Resources/Countries/sk.png > Pack/Countries/sk.png
	xxd -i Resources/Countries/sl.png > Pack/Countries/sl.png
	xxd -i Resources/Countries/sm.png > Pack/Countries/sm.png
	xxd -i Resources/Countries/sn.png > Pack/Countries/sn.png
	xxd -i Resources/Countries/so.png > Pack/Countries/so.png
	xxd -i Resources/Countries/sr.png > Pack/Countries/sr.png
	xxd -i Resources/Countries/st.png > Pack/Countries/st.png
	xxd -i Resources/Countries/sv.png > Pack/Countries/sv.png
	xxd -i Resources/Countries/sy.png > Pack/Countries/sy.png
	xxd -i Resources/Countries/sz.png > Pack/Countries/sz.png
	xxd -i Resources/Countries/tc.png > Pack/Countries/tc.png
	xxd -i Resources/Countries/td.png > Pack/Countries/td.png
	xxd -i Resources/Countries/tg.png > Pack/Countries/tg.png
	xxd -i Resources/Countries/th.png > Pack/Countries/th.png
	xxd -i Resources/Countries/tj.png > Pack/Countries/tj.png
	xxd -i Resources/Countries/tl.png > Pack/Countries/tl.png
	xxd -i Resources/Countries/tm.png > Pack/Countries/tm.png
	xxd -i Resources/Countries/tn.png > Pack/Countries/tn.png
	xxd -i Resources/Countries/to.png > Pack/Countries/to.png
	xxd -i Resources/Countries/tr.png > Pack/Countries/tr.png
	xxd -i Resources/Countries/tt.png > Pack/Countries/tt.png
	xxd -i Resources/Countries/tv.png > Pack/Countries/tv.png
	xxd -i Resources/Countries/tw.png > Pack/Countries/tw.png
	xxd -i Resources/Countries/tz.png > Pack/Countries/tz.png
	xxd -i Resources/Countries/ua.png > Pack/Countries/ua.png
	xxd -i Resources/Countries/ug.png > Pack/Countries/ug.png
	xxd -i Resources/Countries/uk.png > Pack/Countries/uk.png
	xxd -i Resources/Countries/us.png > Pack/Countries/us.png
	xxd -i Resources/Countries/uy.png > Pack/Countries/uy.png
	xxd -i Resources/Countries/uz.png > Pack/Countries/uz.png
	xxd -i Resources/Countries/vc.png > Pack/Countries/vc.png
	xxd -i Resources/Countries/ve.png > Pack/Countries/ve.png
	xxd -i Resources/Countries/vg.png > Pack/Countries/vg.png
	xxd -i Resources/Countries/vh.png > Pack/Countries/vh.png
	xxd -i Resources/Countries/vi.png > Pack/Countries/vi.png
	xxd -i Resources/Countries/vn.png > Pack/Countries/vn.png
	xxd -i Resources/Countries/vu.png > Pack/Countries/vu.png
	xxd -i Resources/Countries/wf.png > Pack/Countries/wf.png
	xxd -i Resources/Countries/ws.png > Pack/Countries/ws.png
	xxd -i Resources/Countries/ye.png > Pack/Countries/ye.png
	xxd -i Resources/Countries/yt.png > Pack/Countries/yt.png
	xxd -i Resources/Countries/yu.png > Pack/Countries/yu.png
	xxd -i Resources/Countries/za.png > Pack/Countries/za.png
	xxd -i Resources/Countries/zm.png > Pack/Countries/zm.png
	xxd -i Resources/Countries/zw.png > Pack/Countries/zw.png

native: pack
	@echo "--- Compiling Native ---"
	$(CC) $(CFLAGS) $(INCLUDES) $(PYTHON_CFLAGS) $(PYTHON_LDFLAGS) -framework GLUT -framework OpenGL -framework OpenCL -std=c++11 -stdlib=libc++ Main.cc -o $(NATIVE_OUT)

web: pack
	@echo "--- Compiling with Emscripten ---"
	$(EM_CC) $(EM_CFLAGS) $(INCLUDES) Main.cc -s FULL_ES2=1 -s TOTAL_MEMORY=268435456 -o Web/$(WEB_OUT) -s EXPORTED_FUNCTIONS=$(JS_EXPORT)

duetto: pack
	@echo "--- Compiling with Duetto ---"
	$(DUETTO_CC) $(DUETTO_CFLAGS) $(INCLUDES) -target duetto Main.cc -o Web/$(WEB_OUT)

clean:
	@echo "--- Cleaning ---"
	( cd ../raindance && make clean )
	rm -rf *~
	rm -rf */*~
	rm -rf *.pyc */*.pyc
	rm -rf Pack
	rm -rf Web/$(WEB_OUT)
	rm -rf Web/$(WEB_OUT).map
	rm -rf $(NATIVE_OUT)
	rm -rf $(NATIVE_OUT).dSYM

dist: clean native
	@echo "--- Making distributable tarball ---"
	rm -rf $(DIST)
	mkdir $(DIST)
	cp ${NATIVE_OUT} $(DIST)/
	cp -r Scripts $(DIST)/
	rm -rf $(DIST)/Scripts/*.pyc ${DIST}/.DS_Store
	cp LICENSE $(DIST)/

	@echo "--- Copying external libraries ---"
	mkdir $(DIST)/Lib
	cp Lib/pygeoip-0.2.7.tar.gz $(DIST)/Lib
	cp Lib/GeoLiteCity.dat $(DIST)/Lib
	cp Lib/networkx-1.8.1.zip $(DIST)/Lib

	@echo "--- Removing temporary files ---"
	zip -r $(DIST).zip $(DIST)
	rm -rf $(DIST)
