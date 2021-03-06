#!/bin/sh

RESOURCES="Assets"

rm -rf Pack.hh
echo "/*" >> Pack.hh
echo " * --- Packed Resources ---" >> Pack.hh
echo " */" >> Pack.hh
echo "#pragma once" >> Pack.hh

xxd -i $RESOURCES/Hud/pointer.png >> Pack.hh
xxd -i $RESOURCES/Hud/play.png >> Pack.hh
xxd -i $RESOURCES/Hud/pause.png >> Pack.hh
xxd -i $RESOURCES/Hud/expand.png >> Pack.hh
xxd -i $RESOURCES/Hud/graph.png >> Pack.hh
xxd -i $RESOURCES/Hud/text.png >> Pack.hh
xxd -i $RESOURCES/Hud/label.png >> Pack.hh
xxd -i $RESOURCES/Hud/node.png >> Pack.hh
xxd -i $RESOURCES/Hud/loupe.png >> Pack.hh
xxd -i $RESOURCES/Hud/flag.png >> Pack.hh
xxd -i $RESOURCES/Hud/marker.png >> Pack.hh
xxd -i $RESOURCES/Hud/bug.png >> Pack.hh
xxd -i $RESOURCES/Hud/spheres.png >> Pack.hh
xxd -i $RESOURCES/Hud/eye.png >> Pack.hh
xxd -i $RESOURCES/Hud/white-disk.png >> Pack.hh
xxd -i $RESOURCES/background.png >> Pack.hh

xxd -i $RESOURCES/cube.vert >> Pack.hh
xxd -i $RESOURCES/cube.frag >> Pack.hh

xxd -i $RESOURCES/quad.vert >> Pack.hh
xxd -i $RESOURCES/quad.frag >> Pack.hh

xxd -i $RESOURCES/sphere.vert >> Pack.hh
xxd -i $RESOURCES/sphere.frag >> Pack.hh

xxd -i $RESOURCES/graph.vert >> Pack.hh
xxd -i $RESOURCES/graph.frag >> Pack.hh

xxd -i $RESOURCES/curve.vert >> Pack.hh
xxd -i $RESOURCES/curve.frag >> Pack.hh

xxd -i $RESOURCES/world.vert >> Pack.hh
xxd -i $RESOURCES/world.frag >> Pack.hh

xxd -i $RESOURCES/disk-white.png >> Pack.hh
xxd -i $RESOURCES/square-white.png >> Pack.hh

xxd -i $RESOURCES/sphere.png >> Pack.hh
xxd -i $RESOURCES/circle.png >> Pack.hh
xxd -i $RESOURCES/target.png >> Pack.hh

# SpaceView

xxd -i $RESOURCES/SpaceView/node-activity.png >> Pack.hh

xxd -i $RESOURCES/SpaceView/EdgeStyles/circles.png >> Pack.hh
xxd -i $RESOURCES/SpaceView/EdgeStyles/cross.png >> Pack.hh
xxd -i $RESOURCES/SpaceView/EdgeStyles/dashed.png >> Pack.hh
xxd -i $RESOURCES/SpaceView/EdgeStyles/dots.png >> Pack.hh
xxd -i $RESOURCES/SpaceView/EdgeStyles/zigzag.png >> Pack.hh
xxd -i $RESOURCES/SpaceView/EdgeStyles/solid.png >> Pack.hh
xxd -i $RESOURCES/SpaceView/EdgeStyles/squares.png >> Pack.hh
xxd -i $RESOURCES/SpaceView/EdgeStyles/triangles.png >> Pack.hh

# WorldView

xxd -i $RESOURCES/WorldView/geoline.vert >> Pack.hh
xxd -i $RESOURCES/WorldView/geoline.frag >> Pack.hh

# ParticleView

xxd -i $RESOURCES/ParticleView/node.vert >> Pack.hh
xxd -i $RESOURCES/ParticleView/node.frag >> Pack.hh
xxd -i $RESOURCES/ParticleView/physics.cl >> Pack.hh

xxd -i $RESOURCES/Earth/black-and-blue.jpg >> Pack.hh
xxd -i $RESOURCES/Earth/world-2048x1024.jpg >> Pack.hh
xxd -i $RESOURCES/Earth/miller-2048x1502.jpg >> Pack.hh
xxd -i $RESOURCES/Earth/miller-2048x1502-bw.jpg >> Pack.hh
xxd -i $RESOURCES/Earth/globe.png >> Pack.hh
xxd -i $RESOURCES/Earth/world-map-icon.png >> Pack.hh

xxd -i $RESOURCES/Spheres/starfield.png >> Pack.hh
xxd -i $RESOURCES/Spheres/perlin.jpg >> Pack.hh

xxd -i $RESOURCES/Particle/ball.png >> Pack.hh
xxd -i $RESOURCES/Particle/metaball.png >> Pack.hh

xxd -i $RESOURCES/Countries/ad.png >> Pack.hh
xxd -i $RESOURCES/Countries/ae.png >> Pack.hh
xxd -i $RESOURCES/Countries/af.png >> Pack.hh
xxd -i $RESOURCES/Countries/ag.png >> Pack.hh
xxd -i $RESOURCES/Countries/ai.png >> Pack.hh
xxd -i $RESOURCES/Countries/al.png >> Pack.hh
xxd -i $RESOURCES/Countries/am.png >> Pack.hh
xxd -i $RESOURCES/Countries/an.png >> Pack.hh
xxd -i $RESOURCES/Countries/ao.png >> Pack.hh
xxd -i $RESOURCES/Countries/ar.png >> Pack.hh
xxd -i $RESOURCES/Countries/as.png >> Pack.hh
xxd -i $RESOURCES/Countries/at.png >> Pack.hh
xxd -i $RESOURCES/Countries/au.png >> Pack.hh
xxd -i $RESOURCES/Countries/aw.png >> Pack.hh
xxd -i $RESOURCES/Countries/ax.png >> Pack.hh
xxd -i $RESOURCES/Countries/ba.png >> Pack.hh
xxd -i $RESOURCES/Countries/bb.png >> Pack.hh
xxd -i $RESOURCES/Countries/bd.png >> Pack.hh
xxd -i $RESOURCES/Countries/be.png >> Pack.hh
xxd -i $RESOURCES/Countries/bf.png >> Pack.hh
xxd -i $RESOURCES/Countries/bg.png >> Pack.hh
xxd -i $RESOURCES/Countries/bh.png >> Pack.hh
xxd -i $RESOURCES/Countries/bi.png >> Pack.hh
xxd -i $RESOURCES/Countries/bj.png >> Pack.hh
xxd -i $RESOURCES/Countries/bm.png >> Pack.hh
xxd -i $RESOURCES/Countries/bn.png >> Pack.hh
xxd -i $RESOURCES/Countries/bo.png >> Pack.hh
xxd -i $RESOURCES/Countries/br.png >> Pack.hh
xxd -i $RESOURCES/Countries/bs.png >> Pack.hh
xxd -i $RESOURCES/Countries/bt.png >> Pack.hh
xxd -i $RESOURCES/Countries/bw.png >> Pack.hh
xxd -i $RESOURCES/Countries/by.png >> Pack.hh
xxd -i $RESOURCES/Countries/bz.png >> Pack.hh
xxd -i $RESOURCES/Countries/ca.png >> Pack.hh
xxd -i $RESOURCES/Countries/cd.png >> Pack.hh
xxd -i $RESOURCES/Countries/cf.png >> Pack.hh
xxd -i $RESOURCES/Countries/cg.png >> Pack.hh
xxd -i $RESOURCES/Countries/ch.png >> Pack.hh
xxd -i $RESOURCES/Countries/ci.png >> Pack.hh
xxd -i $RESOURCES/Countries/ck.png >> Pack.hh
xxd -i $RESOURCES/Countries/cl.png >> Pack.hh
xxd -i $RESOURCES/Countries/cm.png >> Pack.hh
xxd -i $RESOURCES/Countries/cn.png >> Pack.hh
xxd -i $RESOURCES/Countries/co.png >> Pack.hh
xxd -i $RESOURCES/Countries/cr.png >> Pack.hh
xxd -i $RESOURCES/Countries/cu.png >> Pack.hh
xxd -i $RESOURCES/Countries/cv.png >> Pack.hh
xxd -i $RESOURCES/Countries/cx.png >> Pack.hh
xxd -i $RESOURCES/Countries/cy.png >> Pack.hh
xxd -i $RESOURCES/Countries/cz.png >> Pack.hh
xxd -i $RESOURCES/Countries/de.png >> Pack.hh
xxd -i $RESOURCES/Countries/dj.png >> Pack.hh
xxd -i $RESOURCES/Countries/dk.png >> Pack.hh
xxd -i $RESOURCES/Countries/dm.png >> Pack.hh
xxd -i $RESOURCES/Countries/do.png >> Pack.hh
xxd -i $RESOURCES/Countries/dz.png >> Pack.hh
xxd -i $RESOURCES/Countries/ec.png >> Pack.hh
xxd -i $RESOURCES/Countries/ee.png >> Pack.hh
xxd -i $RESOURCES/Countries/eg.png >> Pack.hh
xxd -i $RESOURCES/Countries/er.png >> Pack.hh
xxd -i $RESOURCES/Countries/es.png >> Pack.hh
xxd -i $RESOURCES/Countries/et.png >> Pack.hh
xxd -i $RESOURCES/Countries/eu.png >> Pack.hh
xxd -i $RESOURCES/Countries/fi.png >> Pack.hh
xxd -i $RESOURCES/Countries/fj.png >> Pack.hh
xxd -i $RESOURCES/Countries/fk.png >> Pack.hh
xxd -i $RESOURCES/Countries/fm.png >> Pack.hh
xxd -i $RESOURCES/Countries/fo.png >> Pack.hh
xxd -i $RESOURCES/Countries/fr.png >> Pack.hh
xxd -i $RESOURCES/Countries/ga.png >> Pack.hh
xxd -i $RESOURCES/Countries/gd.png >> Pack.hh
xxd -i $RESOURCES/Countries/ge.png >> Pack.hh
xxd -i $RESOURCES/Countries/gg.png >> Pack.hh
xxd -i $RESOURCES/Countries/gh.png >> Pack.hh
xxd -i $RESOURCES/Countries/gi.png >> Pack.hh
xxd -i $RESOURCES/Countries/gl.png >> Pack.hh
xxd -i $RESOURCES/Countries/gm.png >> Pack.hh
xxd -i $RESOURCES/Countries/gn.png >> Pack.hh
xxd -i $RESOURCES/Countries/gq.png >> Pack.hh
xxd -i $RESOURCES/Countries/gr-cy.png >> Pack.hh
xxd -i $RESOURCES/Countries/gr.png >> Pack.hh
xxd -i $RESOURCES/Countries/gs.png >> Pack.hh
xxd -i $RESOURCES/Countries/gt.png >> Pack.hh
xxd -i $RESOURCES/Countries/gu.png >> Pack.hh
xxd -i $RESOURCES/Countries/gw.png >> Pack.hh
xxd -i $RESOURCES/Countries/gy.png >> Pack.hh
xxd -i $RESOURCES/Countries/hk.png >> Pack.hh
xxd -i $RESOURCES/Countries/hn.png >> Pack.hh
xxd -i $RESOURCES/Countries/hr.png >> Pack.hh
xxd -i $RESOURCES/Countries/ht.png >> Pack.hh
xxd -i $RESOURCES/Countries/hu.png >> Pack.hh
xxd -i $RESOURCES/Countries/id.png >> Pack.hh
xxd -i $RESOURCES/Countries/ie.png >> Pack.hh
xxd -i $RESOURCES/Countries/il.png >> Pack.hh
xxd -i $RESOURCES/Countries/im.png >> Pack.hh
xxd -i $RESOURCES/Countries/in.png >> Pack.hh
xxd -i $RESOURCES/Countries/io.png >> Pack.hh
xxd -i $RESOURCES/Countries/iq.png >> Pack.hh
xxd -i $RESOURCES/Countries/ir.png >> Pack.hh
xxd -i $RESOURCES/Countries/is.png >> Pack.hh
xxd -i $RESOURCES/Countries/it.png >> Pack.hh
xxd -i $RESOURCES/Countries/je.png >> Pack.hh
xxd -i $RESOURCES/Countries/jm.png >> Pack.hh
xxd -i $RESOURCES/Countries/jo.png >> Pack.hh
xxd -i $RESOURCES/Countries/jp.png >> Pack.hh
xxd -i $RESOURCES/Countries/ke.png >> Pack.hh
xxd -i $RESOURCES/Countries/kg.png >> Pack.hh
xxd -i $RESOURCES/Countries/kh.png >> Pack.hh
xxd -i $RESOURCES/Countries/ki.png >> Pack.hh
xxd -i $RESOURCES/Countries/km.png >> Pack.hh
xxd -i $RESOURCES/Countries/kn.png >> Pack.hh
xxd -i $RESOURCES/Countries/kp.png >> Pack.hh
xxd -i $RESOURCES/Countries/kr.png >> Pack.hh
xxd -i $RESOURCES/Countries/kw.png >> Pack.hh
xxd -i $RESOURCES/Countries/ky.png >> Pack.hh
xxd -i $RESOURCES/Countries/kz.png >> Pack.hh
xxd -i $RESOURCES/Countries/la.png >> Pack.hh
xxd -i $RESOURCES/Countries/lb.png >> Pack.hh
xxd -i $RESOURCES/Countries/lc.png >> Pack.hh
xxd -i $RESOURCES/Countries/li.png >> Pack.hh
xxd -i $RESOURCES/Countries/lk.png >> Pack.hh
xxd -i $RESOURCES/Countries/lr.png >> Pack.hh
xxd -i $RESOURCES/Countries/ls.png >> Pack.hh
xxd -i $RESOURCES/Countries/lt.png >> Pack.hh
xxd -i $RESOURCES/Countries/lu.png >> Pack.hh
xxd -i $RESOURCES/Countries/lv.png >> Pack.hh
xxd -i $RESOURCES/Countries/ly.png >> Pack.hh
xxd -i $RESOURCES/Countries/ma.png >> Pack.hh
xxd -i $RESOURCES/Countries/mc.png >> Pack.hh
xxd -i $RESOURCES/Countries/md.png >> Pack.hh
xxd -i $RESOURCES/Countries/me.png >> Pack.hh
xxd -i $RESOURCES/Countries/mg.png >> Pack.hh
xxd -i $RESOURCES/Countries/mh.png >> Pack.hh
xxd -i $RESOURCES/Countries/ml.png >> Pack.hh
xxd -i $RESOURCES/Countries/mn.png >> Pack.hh
xxd -i $RESOURCES/Countries/mo.png >> Pack.hh
xxd -i $RESOURCES/Countries/mp.png >> Pack.hh
xxd -i $RESOURCES/Countries/mq.png >> Pack.hh
xxd -i $RESOURCES/Countries/mr.png >> Pack.hh
xxd -i $RESOURCES/Countries/ms.png >> Pack.hh
xxd -i $RESOURCES/Countries/mt.png >> Pack.hh
xxd -i $RESOURCES/Countries/mu.png >> Pack.hh
xxd -i $RESOURCES/Countries/mv.png >> Pack.hh
xxd -i $RESOURCES/Countries/mw.png >> Pack.hh
xxd -i $RESOURCES/Countries/mx.png >> Pack.hh
xxd -i $RESOURCES/Countries/my.png >> Pack.hh
xxd -i $RESOURCES/Countries/mz.png >> Pack.hh
xxd -i $RESOURCES/Countries/na.png >> Pack.hh
xxd -i $RESOURCES/Countries/ne.png >> Pack.hh
xxd -i $RESOURCES/Countries/nf.png >> Pack.hh
xxd -i $RESOURCES/Countries/ng.png >> Pack.hh
xxd -i $RESOURCES/Countries/ni.png >> Pack.hh
xxd -i $RESOURCES/Countries/nl.png >> Pack.hh
xxd -i $RESOURCES/Countries/nm.png >> Pack.hh
xxd -i $RESOURCES/Countries/no.png >> Pack.hh
xxd -i $RESOURCES/Countries/np.png >> Pack.hh
xxd -i $RESOURCES/Countries/nr.png >> Pack.hh
xxd -i $RESOURCES/Countries/nu.png >> Pack.hh
xxd -i $RESOURCES/Countries/nz.png >> Pack.hh
xxd -i $RESOURCES/Countries/om.png >> Pack.hh
xxd -i $RESOURCES/Countries/pa.png >> Pack.hh
xxd -i $RESOURCES/Countries/pf.png >> Pack.hh
xxd -i $RESOURCES/Countries/pg.png >> Pack.hh
xxd -i $RESOURCES/Countries/ph.png >> Pack.hh
xxd -i $RESOURCES/Countries/pk.png >> Pack.hh
xxd -i $RESOURCES/Countries/pl.png >> Pack.hh
xxd -i $RESOURCES/Countries/pm.png >> Pack.hh
xxd -i $RESOURCES/Countries/pn.png >> Pack.hh
xxd -i $RESOURCES/Countries/pr.png >> Pack.hh
xxd -i $RESOURCES/Countries/pt.png >> Pack.hh
xxd -i $RESOURCES/Countries/pw.png >> Pack.hh
xxd -i $RESOURCES/Countries/py.png >> Pack.hh
xxd -i $RESOURCES/Countries/qa.png >> Pack.hh
xxd -i $RESOURCES/Countries/ro.png >> Pack.hh
xxd -i $RESOURCES/Countries/rs.png >> Pack.hh
xxd -i $RESOURCES/Countries/ru.png >> Pack.hh
xxd -i $RESOURCES/Countries/rw.png >> Pack.hh
xxd -i $RESOURCES/Countries/sa.png >> Pack.hh
xxd -i $RESOURCES/Countries/sb.png >> Pack.hh
xxd -i $RESOURCES/Countries/sc.png >> Pack.hh
xxd -i $RESOURCES/Countries/sd.png >> Pack.hh
xxd -i $RESOURCES/Countries/se.png >> Pack.hh
xxd -i $RESOURCES/Countries/sg.png >> Pack.hh
xxd -i $RESOURCES/Countries/sh.png >> Pack.hh
xxd -i $RESOURCES/Countries/si.png >> Pack.hh
xxd -i $RESOURCES/Countries/sk.png >> Pack.hh
xxd -i $RESOURCES/Countries/sl.png >> Pack.hh
xxd -i $RESOURCES/Countries/sm.png >> Pack.hh
xxd -i $RESOURCES/Countries/sn.png >> Pack.hh
xxd -i $RESOURCES/Countries/so.png >> Pack.hh
xxd -i $RESOURCES/Countries/sr.png >> Pack.hh
xxd -i $RESOURCES/Countries/st.png >> Pack.hh
xxd -i $RESOURCES/Countries/sv.png >> Pack.hh
xxd -i $RESOURCES/Countries/sy.png >> Pack.hh
xxd -i $RESOURCES/Countries/sz.png >> Pack.hh
xxd -i $RESOURCES/Countries/tc.png >> Pack.hh
xxd -i $RESOURCES/Countries/td.png >> Pack.hh
xxd -i $RESOURCES/Countries/tg.png >> Pack.hh
xxd -i $RESOURCES/Countries/th.png >> Pack.hh
xxd -i $RESOURCES/Countries/tj.png >> Pack.hh
xxd -i $RESOURCES/Countries/tl.png >> Pack.hh
xxd -i $RESOURCES/Countries/tm.png >> Pack.hh
xxd -i $RESOURCES/Countries/tn.png >> Pack.hh
xxd -i $RESOURCES/Countries/to.png >> Pack.hh
xxd -i $RESOURCES/Countries/tr.png >> Pack.hh
xxd -i $RESOURCES/Countries/tt.png >> Pack.hh
xxd -i $RESOURCES/Countries/tv.png >> Pack.hh
xxd -i $RESOURCES/Countries/tw.png >> Pack.hh
xxd -i $RESOURCES/Countries/tz.png >> Pack.hh
xxd -i $RESOURCES/Countries/ua.png >> Pack.hh
xxd -i $RESOURCES/Countries/ug.png >> Pack.hh
xxd -i $RESOURCES/Countries/uk.png >> Pack.hh
xxd -i $RESOURCES/Countries/us.png >> Pack.hh
xxd -i $RESOURCES/Countries/uy.png >> Pack.hh
xxd -i $RESOURCES/Countries/uz.png >> Pack.hh
xxd -i $RESOURCES/Countries/vc.png >> Pack.hh
xxd -i $RESOURCES/Countries/ve.png >> Pack.hh
xxd -i $RESOURCES/Countries/vg.png >> Pack.hh
xxd -i $RESOURCES/Countries/vh.png >> Pack.hh
xxd -i $RESOURCES/Countries/vi.png >> Pack.hh
xxd -i $RESOURCES/Countries/vn.png >> Pack.hh
xxd -i $RESOURCES/Countries/vu.png >> Pack.hh
xxd -i $RESOURCES/Countries/wf.png >> Pack.hh
xxd -i $RESOURCES/Countries/ws.png >> Pack.hh
xxd -i $RESOURCES/Countries/ye.png >> Pack.hh
xxd -i $RESOURCES/Countries/yt.png >> Pack.hh
xxd -i $RESOURCES/Countries/yu.png >> Pack.hh
xxd -i $RESOURCES/Countries/za.png >> Pack.hh
xxd -i $RESOURCES/Countries/zm.png >> Pack.hh
xxd -i $RESOURCES/Countries/zw.png >> Pack.hh
