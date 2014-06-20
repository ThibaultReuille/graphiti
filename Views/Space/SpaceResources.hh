#pragma once

#include "Pack.hh"

class SpaceResources
{
public:
	enum NodeMode { ALL, COLORS, MARKS, NONE };
	enum LinkMode { NODE_COLOR, LINK_COLOR };

	SpaceResources()
	{
	    Model = NULL;

		// Parameters
		{
			NodeIconSize = 1.0f;
			ShowNodeShapes = ALL;
			ShowNodeLabels = true;
			ShowNodeActivity = true;
			EdgeSize = 1.0f;
			ShowEdges = true;
			ShowEdgeActivity = true;
			ShowSpheres = true;
			ShowLabels = true;
			ShowDebug = false;

			LODFrame = glm::vec2(0.0, 1.0);
			LODSlice = 0.05;
			ShowNodeLOD = false;
			ShowLinkLOD = false;

			m_LinkMode = NODE_COLOR;
		}

		// Nodes
		{
			NodeIcon = new Icon();

			NodeIcon->load("shapes/disk", Resources_Particle_ball_png, sizeof(Resources_Particle_ball_png));

            NodeIcon->load("shapes/cloud", Resources_Textures_Shapes_cloud_png, sizeof(Resources_Textures_Shapes_cloud_png));
            NodeIcon->load("shapes/cross", Resources_Textures_Shapes_cross_png, sizeof(Resources_Textures_Shapes_cross_png));
            // NodeIcon->load("shapes/disk", Resources_Textures_Shapes_disk_png, sizeof(Resources_Textures_Shapes_disk_png));
            NodeIcon->load("shapes/forbidden", Resources_Textures_Shapes_forbidden_png, sizeof(Resources_Textures_Shapes_forbidden_png));
            NodeIcon->load("shapes/heart", Resources_Textures_Shapes_heart_png, sizeof(Resources_Textures_Shapes_heart_png));
            NodeIcon->load("shapes/hexagon", Resources_Textures_Shapes_hexagon_png, sizeof(Resources_Textures_Shapes_hexagon_png));
            NodeIcon->load("shapes/house", Resources_Textures_Shapes_house_png, sizeof(Resources_Textures_Shapes_house_png));
            NodeIcon->load("shapes/losange", Resources_Textures_Shapes_losange_png, sizeof(Resources_Textures_Shapes_losange_png));
            NodeIcon->load("shapes/octagon", Resources_Textures_Shapes_octagon_png, sizeof(Resources_Textures_Shapes_octagon_png));
            NodeIcon->load("shapes/patch", Resources_Textures_Shapes_patch_png, sizeof(Resources_Textures_Shapes_patch_png));
            NodeIcon->load("shapes/pentagon", Resources_Textures_Shapes_pentagon_png, sizeof(Resources_Textures_Shapes_pentagon_png));
            NodeIcon->load("shapes/semicircle", Resources_Textures_Shapes_semicircle_png, sizeof(Resources_Textures_Shapes_semicircle_png));
            NodeIcon->load("shapes/square", Resources_Textures_Shapes_square_png, sizeof(Resources_Textures_Shapes_square_png));
            NodeIcon->load("shapes/star", Resources_Textures_Shapes_star_png, sizeof(Resources_Textures_Shapes_star_png));
            NodeIcon->load("shapes/triangle", Resources_Textures_Shapes_triangle_png, sizeof(Resources_Textures_Shapes_triangle_png)); // TODO : Adjust/Center PNG
            NodeIcon->load("shapes/triangle1", Resources_Textures_Shapes_triangle1_png, sizeof(Resources_Textures_Shapes_triangle1_png));
            NodeIcon->load("shapes/triangle2", Resources_Textures_Shapes_triangle2_png, sizeof(Resources_Textures_Shapes_triangle2_png));

			NodeIcon->load("countries/ad", Resources_Countries_ad_png, sizeof(Resources_Countries_ad_png));
			NodeIcon->load("countries/ae", Resources_Countries_ae_png, sizeof(Resources_Countries_ae_png));
			NodeIcon->load("countries/af", Resources_Countries_af_png, sizeof(Resources_Countries_af_png));
			NodeIcon->load("countries/ag", Resources_Countries_ag_png, sizeof(Resources_Countries_ag_png));
			NodeIcon->load("countries/ai", Resources_Countries_ai_png, sizeof(Resources_Countries_ai_png));
			NodeIcon->load("countries/al", Resources_Countries_al_png, sizeof(Resources_Countries_al_png));
			NodeIcon->load("countries/am", Resources_Countries_am_png, sizeof(Resources_Countries_am_png));
			NodeIcon->load("countries/an", Resources_Countries_an_png, sizeof(Resources_Countries_an_png));
			NodeIcon->load("countries/ao", Resources_Countries_ao_png, sizeof(Resources_Countries_ao_png));
			NodeIcon->load("countries/ar", Resources_Countries_ar_png, sizeof(Resources_Countries_ar_png));
			NodeIcon->load("countries/as", Resources_Countries_as_png, sizeof(Resources_Countries_as_png));
			NodeIcon->load("countries/at", Resources_Countries_at_png, sizeof(Resources_Countries_at_png));
			NodeIcon->load("countries/au", Resources_Countries_au_png, sizeof(Resources_Countries_au_png));
			NodeIcon->load("countries/aw", Resources_Countries_aw_png, sizeof(Resources_Countries_aw_png));
			NodeIcon->load("countries/ax", Resources_Countries_ax_png, sizeof(Resources_Countries_ax_png));
			NodeIcon->load("countries/ba", Resources_Countries_ba_png, sizeof(Resources_Countries_ba_png));
			NodeIcon->load("countries/bb", Resources_Countries_bb_png, sizeof(Resources_Countries_bb_png));
			NodeIcon->load("countries/bd", Resources_Countries_bd_png, sizeof(Resources_Countries_bd_png));
			NodeIcon->load("countries/be", Resources_Countries_be_png, sizeof(Resources_Countries_be_png));
			NodeIcon->load("countries/bf", Resources_Countries_bf_png, sizeof(Resources_Countries_bf_png));
			NodeIcon->load("countries/bg", Resources_Countries_bg_png, sizeof(Resources_Countries_bg_png));
			NodeIcon->load("countries/bh", Resources_Countries_bh_png, sizeof(Resources_Countries_bh_png));
			NodeIcon->load("countries/bi", Resources_Countries_bi_png, sizeof(Resources_Countries_bi_png));
			NodeIcon->load("countries/bj", Resources_Countries_bj_png, sizeof(Resources_Countries_bj_png));
			NodeIcon->load("countries/bm", Resources_Countries_bm_png, sizeof(Resources_Countries_bm_png));
			NodeIcon->load("countries/bn", Resources_Countries_bn_png, sizeof(Resources_Countries_bn_png));
			NodeIcon->load("countries/bo", Resources_Countries_bo_png, sizeof(Resources_Countries_bo_png));
			NodeIcon->load("countries/br", Resources_Countries_br_png, sizeof(Resources_Countries_br_png));
			NodeIcon->load("countries/bs", Resources_Countries_bs_png, sizeof(Resources_Countries_bs_png));
			NodeIcon->load("countries/bt", Resources_Countries_bt_png, sizeof(Resources_Countries_bt_png));
			NodeIcon->load("countries/bw", Resources_Countries_bw_png, sizeof(Resources_Countries_bw_png));
			NodeIcon->load("countries/by", Resources_Countries_by_png, sizeof(Resources_Countries_by_png));
			NodeIcon->load("countries/bz", Resources_Countries_bz_png, sizeof(Resources_Countries_bz_png));
			NodeIcon->load("countries/ca", Resources_Countries_ca_png, sizeof(Resources_Countries_ca_png));
			NodeIcon->load("countries/cd", Resources_Countries_cd_png, sizeof(Resources_Countries_cd_png));
			NodeIcon->load("countries/cf", Resources_Countries_cf_png, sizeof(Resources_Countries_cf_png));
			NodeIcon->load("countries/cg", Resources_Countries_cg_png, sizeof(Resources_Countries_cg_png));
			NodeIcon->load("countries/ch", Resources_Countries_ch_png, sizeof(Resources_Countries_ch_png));
			NodeIcon->load("countries/ci", Resources_Countries_ci_png, sizeof(Resources_Countries_ci_png));
			NodeIcon->load("countries/ck", Resources_Countries_ck_png, sizeof(Resources_Countries_ck_png));
			NodeIcon->load("countries/cl", Resources_Countries_cl_png, sizeof(Resources_Countries_cl_png));
			NodeIcon->load("countries/cm", Resources_Countries_cm_png, sizeof(Resources_Countries_cm_png));
			NodeIcon->load("countries/cn", Resources_Countries_cn_png, sizeof(Resources_Countries_cn_png));
			NodeIcon->load("countries/co", Resources_Countries_co_png, sizeof(Resources_Countries_co_png));
			NodeIcon->load("countries/cr", Resources_Countries_cr_png, sizeof(Resources_Countries_cr_png));
			NodeIcon->load("countries/cu", Resources_Countries_cu_png, sizeof(Resources_Countries_cu_png));
			NodeIcon->load("countries/cv", Resources_Countries_cv_png, sizeof(Resources_Countries_cv_png));
			NodeIcon->load("countries/cx", Resources_Countries_cx_png, sizeof(Resources_Countries_cx_png));
			NodeIcon->load("countries/cy", Resources_Countries_cy_png, sizeof(Resources_Countries_cy_png));
			NodeIcon->load("countries/cz", Resources_Countries_cz_png, sizeof(Resources_Countries_cz_png));
			NodeIcon->load("countries/de", Resources_Countries_de_png, sizeof(Resources_Countries_de_png));
			NodeIcon->load("countries/dj", Resources_Countries_dj_png, sizeof(Resources_Countries_dj_png));
			NodeIcon->load("countries/dk", Resources_Countries_dk_png, sizeof(Resources_Countries_dk_png));
			NodeIcon->load("countries/dm", Resources_Countries_dm_png, sizeof(Resources_Countries_dm_png));
			NodeIcon->load("countries/do", Resources_Countries_do_png, sizeof(Resources_Countries_do_png));
			NodeIcon->load("countries/dz", Resources_Countries_dz_png, sizeof(Resources_Countries_dz_png));
			NodeIcon->load("countries/ec", Resources_Countries_ec_png, sizeof(Resources_Countries_ec_png));
			NodeIcon->load("countries/ee", Resources_Countries_ee_png, sizeof(Resources_Countries_ee_png));
			NodeIcon->load("countries/eg", Resources_Countries_eg_png, sizeof(Resources_Countries_eg_png));
			NodeIcon->load("countries/er", Resources_Countries_er_png, sizeof(Resources_Countries_er_png));
			NodeIcon->load("countries/es", Resources_Countries_es_png, sizeof(Resources_Countries_es_png));
			NodeIcon->load("countries/et", Resources_Countries_et_png, sizeof(Resources_Countries_et_png));
			NodeIcon->load("countries/eu", Resources_Countries_eu_png, sizeof(Resources_Countries_eu_png));
			NodeIcon->load("countries/fi", Resources_Countries_fi_png, sizeof(Resources_Countries_fi_png));
			NodeIcon->load("countries/fj", Resources_Countries_fj_png, sizeof(Resources_Countries_fj_png));
			NodeIcon->load("countries/fk", Resources_Countries_fk_png, sizeof(Resources_Countries_fk_png));
			NodeIcon->load("countries/fm", Resources_Countries_fm_png, sizeof(Resources_Countries_fm_png));
			NodeIcon->load("countries/fo", Resources_Countries_fo_png, sizeof(Resources_Countries_fo_png));
			NodeIcon->load("countries/fr", Resources_Countries_fr_png, sizeof(Resources_Countries_fr_png));
			NodeIcon->load("countries/ga", Resources_Countries_ga_png, sizeof(Resources_Countries_ga_png));
			NodeIcon->load("countries/gd", Resources_Countries_gd_png, sizeof(Resources_Countries_gd_png));
			NodeIcon->load("countries/ge", Resources_Countries_ge_png, sizeof(Resources_Countries_ge_png));
			NodeIcon->load("countries/gg", Resources_Countries_gg_png, sizeof(Resources_Countries_gg_png));
			NodeIcon->load("countries/gh", Resources_Countries_gh_png, sizeof(Resources_Countries_gh_png));
			NodeIcon->load("countries/gi", Resources_Countries_gi_png, sizeof(Resources_Countries_gi_png));
			NodeIcon->load("countries/gl", Resources_Countries_gl_png, sizeof(Resources_Countries_gl_png));
			NodeIcon->load("countries/gm", Resources_Countries_gm_png, sizeof(Resources_Countries_gm_png));
			NodeIcon->load("countries/gn", Resources_Countries_gn_png, sizeof(Resources_Countries_gn_png));
			NodeIcon->load("countries/gq", Resources_Countries_gq_png, sizeof(Resources_Countries_gq_png));
			NodeIcon->load("countries/gr", Resources_Countries_gr_png, sizeof(Resources_Countries_gr_png));
			// NodeIcon->load("countries/gr-cy", Resources_Countries_gr_cy_png, sizeof(Resources_Countries_gr_cy_png));
			NodeIcon->load("countries/gs", Resources_Countries_gs_png, sizeof(Resources_Countries_gs_png));
			NodeIcon->load("countries/gt", Resources_Countries_gt_png, sizeof(Resources_Countries_gt_png));
			NodeIcon->load("countries/gu", Resources_Countries_gu_png, sizeof(Resources_Countries_gu_png));
			NodeIcon->load("countries/gw", Resources_Countries_gw_png, sizeof(Resources_Countries_gw_png));
			NodeIcon->load("countries/gy", Resources_Countries_gy_png, sizeof(Resources_Countries_gy_png));
			NodeIcon->load("countries/hk", Resources_Countries_hk_png, sizeof(Resources_Countries_hk_png));
			NodeIcon->load("countries/hn", Resources_Countries_hn_png, sizeof(Resources_Countries_hn_png));
			NodeIcon->load("countries/hr", Resources_Countries_hr_png, sizeof(Resources_Countries_hr_png));
			NodeIcon->load("countries/ht", Resources_Countries_ht_png, sizeof(Resources_Countries_ht_png));
			NodeIcon->load("countries/hu", Resources_Countries_hu_png, sizeof(Resources_Countries_hu_png));
			NodeIcon->load("countries/id", Resources_Countries_id_png, sizeof(Resources_Countries_id_png));
			NodeIcon->load("countries/ie", Resources_Countries_ie_png, sizeof(Resources_Countries_ie_png));
			NodeIcon->load("countries/il", Resources_Countries_il_png, sizeof(Resources_Countries_il_png));
			NodeIcon->load("countries/im", Resources_Countries_im_png, sizeof(Resources_Countries_im_png));
			NodeIcon->load("countries/in", Resources_Countries_in_png, sizeof(Resources_Countries_in_png));
			NodeIcon->load("countries/io", Resources_Countries_io_png, sizeof(Resources_Countries_io_png));
			NodeIcon->load("countries/iq", Resources_Countries_iq_png, sizeof(Resources_Countries_iq_png));
			NodeIcon->load("countries/ir", Resources_Countries_ir_png, sizeof(Resources_Countries_ir_png));
			NodeIcon->load("countries/is", Resources_Countries_is_png, sizeof(Resources_Countries_is_png));
			NodeIcon->load("countries/it", Resources_Countries_it_png, sizeof(Resources_Countries_it_png));
			NodeIcon->load("countries/je", Resources_Countries_je_png, sizeof(Resources_Countries_je_png));
			NodeIcon->load("countries/jm", Resources_Countries_jm_png, sizeof(Resources_Countries_jm_png));
			NodeIcon->load("countries/jo", Resources_Countries_jo_png, sizeof(Resources_Countries_jo_png));
			NodeIcon->load("countries/jp", Resources_Countries_jp_png, sizeof(Resources_Countries_jp_png));
			NodeIcon->load("countries/ke", Resources_Countries_ke_png, sizeof(Resources_Countries_ke_png));
			NodeIcon->load("countries/kg", Resources_Countries_kg_png, sizeof(Resources_Countries_kg_png));
			NodeIcon->load("countries/kh", Resources_Countries_kh_png, sizeof(Resources_Countries_kh_png));
			NodeIcon->load("countries/ki", Resources_Countries_ki_png, sizeof(Resources_Countries_ki_png));
			NodeIcon->load("countries/km", Resources_Countries_km_png, sizeof(Resources_Countries_km_png));
			NodeIcon->load("countries/kn", Resources_Countries_kn_png, sizeof(Resources_Countries_kn_png));
			NodeIcon->load("countries/kp", Resources_Countries_kp_png, sizeof(Resources_Countries_kp_png));
			NodeIcon->load("countries/kr", Resources_Countries_kr_png, sizeof(Resources_Countries_kr_png));
			NodeIcon->load("countries/kw", Resources_Countries_kw_png, sizeof(Resources_Countries_kw_png));
			NodeIcon->load("countries/ky", Resources_Countries_ky_png, sizeof(Resources_Countries_ky_png));
			NodeIcon->load("countries/kz", Resources_Countries_kz_png, sizeof(Resources_Countries_kz_png));
			NodeIcon->load("countries/la", Resources_Countries_la_png, sizeof(Resources_Countries_la_png));
			NodeIcon->load("countries/lb", Resources_Countries_lb_png, sizeof(Resources_Countries_lb_png));
			NodeIcon->load("countries/lc", Resources_Countries_lc_png, sizeof(Resources_Countries_lc_png));
			NodeIcon->load("countries/li", Resources_Countries_li_png, sizeof(Resources_Countries_li_png));
			NodeIcon->load("countries/lk", Resources_Countries_lk_png, sizeof(Resources_Countries_lk_png));
			NodeIcon->load("countries/lr", Resources_Countries_lr_png, sizeof(Resources_Countries_lr_png));
			NodeIcon->load("countries/ls", Resources_Countries_ls_png, sizeof(Resources_Countries_ls_png));
			NodeIcon->load("countries/lt", Resources_Countries_lt_png, sizeof(Resources_Countries_lt_png));
			NodeIcon->load("countries/lu", Resources_Countries_lu_png, sizeof(Resources_Countries_lu_png));
			NodeIcon->load("countries/lv", Resources_Countries_lv_png, sizeof(Resources_Countries_lv_png));
			NodeIcon->load("countries/ly", Resources_Countries_ly_png, sizeof(Resources_Countries_ly_png));
			NodeIcon->load("countries/ma", Resources_Countries_ma_png, sizeof(Resources_Countries_ma_png));
			NodeIcon->load("countries/mc", Resources_Countries_mc_png, sizeof(Resources_Countries_mc_png));
			NodeIcon->load("countries/md", Resources_Countries_md_png, sizeof(Resources_Countries_md_png));
			NodeIcon->load("countries/me", Resources_Countries_me_png, sizeof(Resources_Countries_me_png));
			NodeIcon->load("countries/mg", Resources_Countries_mg_png, sizeof(Resources_Countries_mg_png));
			NodeIcon->load("countries/mh", Resources_Countries_mh_png, sizeof(Resources_Countries_mh_png));
			NodeIcon->load("countries/ml", Resources_Countries_ml_png, sizeof(Resources_Countries_ml_png));
			NodeIcon->load("countries/mn", Resources_Countries_mn_png, sizeof(Resources_Countries_mn_png));
			NodeIcon->load("countries/mo", Resources_Countries_mo_png, sizeof(Resources_Countries_mo_png));
			NodeIcon->load("countries/mp", Resources_Countries_mp_png, sizeof(Resources_Countries_mp_png));
			NodeIcon->load("countries/mq", Resources_Countries_mq_png, sizeof(Resources_Countries_mq_png));
			NodeIcon->load("countries/mr", Resources_Countries_mr_png, sizeof(Resources_Countries_mr_png));
			NodeIcon->load("countries/ms", Resources_Countries_ms_png, sizeof(Resources_Countries_ms_png));
			NodeIcon->load("countries/mt", Resources_Countries_mt_png, sizeof(Resources_Countries_mt_png));
			NodeIcon->load("countries/mu", Resources_Countries_mu_png, sizeof(Resources_Countries_mu_png));
			NodeIcon->load("countries/mv", Resources_Countries_mv_png, sizeof(Resources_Countries_mv_png));
			NodeIcon->load("countries/mw", Resources_Countries_mw_png, sizeof(Resources_Countries_mw_png));
			NodeIcon->load("countries/mx", Resources_Countries_mx_png, sizeof(Resources_Countries_mx_png));
			NodeIcon->load("countries/my", Resources_Countries_my_png, sizeof(Resources_Countries_my_png));
			NodeIcon->load("countries/mz", Resources_Countries_mz_png, sizeof(Resources_Countries_mz_png));
			NodeIcon->load("countries/na", Resources_Countries_na_png, sizeof(Resources_Countries_na_png));
			NodeIcon->load("countries/ne", Resources_Countries_ne_png, sizeof(Resources_Countries_ne_png));
			NodeIcon->load("countries/nf", Resources_Countries_nf_png, sizeof(Resources_Countries_nf_png));
			NodeIcon->load("countries/ng", Resources_Countries_ng_png, sizeof(Resources_Countries_ng_png));
			NodeIcon->load("countries/ni", Resources_Countries_ni_png, sizeof(Resources_Countries_ni_png));
			NodeIcon->load("countries/nl", Resources_Countries_nl_png, sizeof(Resources_Countries_nl_png));
			NodeIcon->load("countries/nm", Resources_Countries_nm_png, sizeof(Resources_Countries_nm_png));
			NodeIcon->load("countries/no", Resources_Countries_no_png, sizeof(Resources_Countries_no_png));
			NodeIcon->load("countries/np", Resources_Countries_np_png, sizeof(Resources_Countries_np_png));
			NodeIcon->load("countries/nr", Resources_Countries_nr_png, sizeof(Resources_Countries_nr_png));
			NodeIcon->load("countries/nu", Resources_Countries_nu_png, sizeof(Resources_Countries_nu_png));
			NodeIcon->load("countries/nz", Resources_Countries_nz_png, sizeof(Resources_Countries_nz_png));
			NodeIcon->load("countries/om", Resources_Countries_om_png, sizeof(Resources_Countries_om_png));
			NodeIcon->load("countries/pa", Resources_Countries_pa_png, sizeof(Resources_Countries_pa_png));
			NodeIcon->load("countries/pf", Resources_Countries_pf_png, sizeof(Resources_Countries_pf_png));
			NodeIcon->load("countries/pg", Resources_Countries_pg_png, sizeof(Resources_Countries_pg_png));
			NodeIcon->load("countries/ph", Resources_Countries_ph_png, sizeof(Resources_Countries_ph_png));
			NodeIcon->load("countries/pk", Resources_Countries_pk_png, sizeof(Resources_Countries_pk_png));
			NodeIcon->load("countries/pl", Resources_Countries_pl_png, sizeof(Resources_Countries_pl_png));
			NodeIcon->load("countries/pm", Resources_Countries_pm_png, sizeof(Resources_Countries_pm_png));
			NodeIcon->load("countries/pn", Resources_Countries_pn_png, sizeof(Resources_Countries_pn_png));
			NodeIcon->load("countries/pr", Resources_Countries_pr_png, sizeof(Resources_Countries_pr_png));
			NodeIcon->load("countries/pt", Resources_Countries_pt_png, sizeof(Resources_Countries_pt_png));
			NodeIcon->load("countries/pw", Resources_Countries_pw_png, sizeof(Resources_Countries_pw_png));
			NodeIcon->load("countries/py", Resources_Countries_py_png, sizeof(Resources_Countries_py_png));
			NodeIcon->load("countries/qa", Resources_Countries_qa_png, sizeof(Resources_Countries_qa_png));
			NodeIcon->load("countries/ro", Resources_Countries_ro_png, sizeof(Resources_Countries_ro_png));
			NodeIcon->load("countries/rs", Resources_Countries_rs_png, sizeof(Resources_Countries_rs_png));
			NodeIcon->load("countries/ru", Resources_Countries_ru_png, sizeof(Resources_Countries_ru_png));
			NodeIcon->load("countries/rw", Resources_Countries_rw_png, sizeof(Resources_Countries_rw_png));
			NodeIcon->load("countries/sa", Resources_Countries_sa_png, sizeof(Resources_Countries_sa_png));
			NodeIcon->load("countries/sb", Resources_Countries_sb_png, sizeof(Resources_Countries_sb_png));
			NodeIcon->load("countries/sc", Resources_Countries_sc_png, sizeof(Resources_Countries_sc_png));
			NodeIcon->load("countries/sd", Resources_Countries_sd_png, sizeof(Resources_Countries_sd_png));
			NodeIcon->load("countries/se", Resources_Countries_se_png, sizeof(Resources_Countries_se_png));
			NodeIcon->load("countries/sg", Resources_Countries_sg_png, sizeof(Resources_Countries_sg_png));
			NodeIcon->load("countries/sh", Resources_Countries_sh_png, sizeof(Resources_Countries_sh_png));
			NodeIcon->load("countries/si", Resources_Countries_si_png, sizeof(Resources_Countries_si_png));
			NodeIcon->load("countries/sk", Resources_Countries_sk_png, sizeof(Resources_Countries_sk_png));
			NodeIcon->load("countries/sl", Resources_Countries_sl_png, sizeof(Resources_Countries_sl_png));
			NodeIcon->load("countries/sm", Resources_Countries_sm_png, sizeof(Resources_Countries_sm_png));
			NodeIcon->load("countries/sn", Resources_Countries_sn_png, sizeof(Resources_Countries_sn_png));
			NodeIcon->load("countries/so", Resources_Countries_so_png, sizeof(Resources_Countries_so_png));
			NodeIcon->load("countries/sr", Resources_Countries_sr_png, sizeof(Resources_Countries_sr_png));
			NodeIcon->load("countries/st", Resources_Countries_st_png, sizeof(Resources_Countries_st_png));
			NodeIcon->load("countries/sv", Resources_Countries_sv_png, sizeof(Resources_Countries_sv_png));
			NodeIcon->load("countries/sy", Resources_Countries_sy_png, sizeof(Resources_Countries_sy_png));
			NodeIcon->load("countries/sz", Resources_Countries_sz_png, sizeof(Resources_Countries_sz_png));
			NodeIcon->load("countries/tc", Resources_Countries_tc_png, sizeof(Resources_Countries_tc_png));
			NodeIcon->load("countries/td", Resources_Countries_td_png, sizeof(Resources_Countries_td_png));
			NodeIcon->load("countries/tg", Resources_Countries_tg_png, sizeof(Resources_Countries_tg_png));
			NodeIcon->load("countries/th", Resources_Countries_th_png, sizeof(Resources_Countries_th_png));
			NodeIcon->load("countries/tj", Resources_Countries_tj_png, sizeof(Resources_Countries_tj_png));
			NodeIcon->load("countries/tl", Resources_Countries_tl_png, sizeof(Resources_Countries_tl_png));
			NodeIcon->load("countries/tm", Resources_Countries_tm_png, sizeof(Resources_Countries_tm_png));
			NodeIcon->load("countries/tn", Resources_Countries_tn_png, sizeof(Resources_Countries_tn_png));
			NodeIcon->load("countries/to", Resources_Countries_to_png, sizeof(Resources_Countries_to_png));
			NodeIcon->load("countries/tr", Resources_Countries_tr_png, sizeof(Resources_Countries_tr_png));
			NodeIcon->load("countries/tt", Resources_Countries_tt_png, sizeof(Resources_Countries_tt_png));
			NodeIcon->load("countries/tv", Resources_Countries_tv_png, sizeof(Resources_Countries_tv_png));
			NodeIcon->load("countries/tw", Resources_Countries_tw_png, sizeof(Resources_Countries_tw_png));
			NodeIcon->load("countries/tz", Resources_Countries_tz_png, sizeof(Resources_Countries_tz_png));
			NodeIcon->load("countries/ua", Resources_Countries_ua_png, sizeof(Resources_Countries_ua_png));
			NodeIcon->load("countries/ug", Resources_Countries_ug_png, sizeof(Resources_Countries_ug_png));
			NodeIcon->load("countries/uk", Resources_Countries_uk_png, sizeof(Resources_Countries_uk_png));
			NodeIcon->load("countries/us", Resources_Countries_us_png, sizeof(Resources_Countries_us_png));
			NodeIcon->load("countries/uy", Resources_Countries_uy_png, sizeof(Resources_Countries_uy_png));
			NodeIcon->load("countries/uz", Resources_Countries_uz_png, sizeof(Resources_Countries_uz_png));
			NodeIcon->load("countries/vc", Resources_Countries_vc_png, sizeof(Resources_Countries_vc_png));
			NodeIcon->load("countries/ve", Resources_Countries_ve_png, sizeof(Resources_Countries_ve_png));
			NodeIcon->load("countries/vg", Resources_Countries_vg_png, sizeof(Resources_Countries_vg_png));
			NodeIcon->load("countries/vh", Resources_Countries_vh_png, sizeof(Resources_Countries_vh_png));
			NodeIcon->load("countries/vi", Resources_Countries_vi_png, sizeof(Resources_Countries_vi_png));
			NodeIcon->load("countries/vn", Resources_Countries_vn_png, sizeof(Resources_Countries_vn_png));
			NodeIcon->load("countries/vu", Resources_Countries_vu_png, sizeof(Resources_Countries_vu_png));
			NodeIcon->load("countries/wf", Resources_Countries_wf_png, sizeof(Resources_Countries_wf_png));
			NodeIcon->load("countries/ws", Resources_Countries_ws_png, sizeof(Resources_Countries_ws_png));
			NodeIcon->load("countries/ye", Resources_Countries_ye_png, sizeof(Resources_Countries_ye_png));
			NodeIcon->load("countries/yt", Resources_Countries_yt_png, sizeof(Resources_Countries_yt_png));
			NodeIcon->load("countries/yu", Resources_Countries_yu_png, sizeof(Resources_Countries_yu_png));
			NodeIcon->load("countries/za", Resources_Countries_za_png, sizeof(Resources_Countries_za_png));
			NodeIcon->load("countries/zm", Resources_Countries_zm_png, sizeof(Resources_Countries_zm_png));
			NodeIcon->load("countries/zw", Resources_Countries_zw_png, sizeof(Resources_Countries_zw_png));

			NodeMarkIcon = new Icon();
			NodeMarkIcon->load("mark", Resources_Textures_mark_png, sizeof(Resources_Textures_mark_png));
			NodeTargetIcon = new Icon();
			NodeTargetIcon->load("target", Resources_target_png, sizeof(Resources_target_png));
			//m_NodeSphereTexture = ResourceManager::getInstance().loadTexture("perlin", Resources_Spheres_perlin_jpg, sizeof(Resources_Spheres_perlin_jpg));

			NodeFont = new Font();
			NodeActivityIcon = new Icon();
			NodeActivityIcon->load("node_activity", Resources_SpaceView_node_activity_png, sizeof(Resources_SpaceView_node_activity_png));
		}

		// Links
		{
			LinkShader = ResourceManager::getInstance().loadShader("graph", Resources_graph_vert, sizeof(Resources_graph_vert), Resources_graph_frag, sizeof(Resources_graph_frag));
			LinkActivityIcon = new Icon();
			LinkActivityIcon->load("link_activity", Resources_Particle_metaball_png, sizeof(Resources_Particle_metaball_png));
		}

		// Spheres
		{
			SphereIcon = new Icon();
			SphereIcon->load("circle", Resources_circle_png, sizeof(Resources_circle_png));
		}

		// Others
		{
		    m_WallpaperTexture = ResourceManager::getInstance().loadTexture("background", Resources_background_png, sizeof(Resources_background_png));
		    m_Wallpaper = new Wallpaper();
		    m_Wallpaper->setTexture(m_WallpaperTexture);
		}
	}

	~SpaceResources()
	{
		delete NodeIcon;
		delete NodeMarkIcon;
		delete NodeTargetIcon;
		delete NodeFont;
		delete NodeActivityIcon;

		ResourceManager::getInstance().unload(LinkShader);
		delete LinkActivityIcon;

		delete SphereIcon;

        delete m_Wallpaper;
		ResourceManager::getInstance().unload(m_WallpaperTexture);
	}

	float getTentFilter(float t, float amplitude, float CurrentLOD, float LODSlice)
	{
        if (t < CurrentLOD)
            return amplitude * ((1 / LODSlice) * t + (1 - CurrentLOD / LODSlice));
        else
            return amplitude * ((-1 / LODSlice) * t + (1 + CurrentLOD / LODSlice));
	}

	// Parameters
	float NodeIconSize;
	NodeMode ShowNodeShapes;
	bool ShowNodeLabels;
	bool ShowNodeActivity;
	float EdgeSize;
	bool ShowEdges;
	bool ShowEdgeActivity;
	bool ShowSpheres;
	bool ShowLabels;
	bool ShowDebug;

    bool ShowNodeLOD;
    bool ShowLinkLOD;
	float CurrentLOD;
	glm::vec2 LODFrame;
	float LODSlice;

	GraphModel* Model;

	// Nodes
	Icon* NodeIcon;
	Icon* NodeMarkIcon;
	Icon* NodeTargetIcon;
	Font* NodeFont;
	Icon* NodeActivityIcon;

	// Links
	Shader::Program* LinkShader;
	Geometry LinkGeometry;
	LinkMode m_LinkMode;
	Icon* LinkActivityIcon;

	// Spheres
	Icon* SphereIcon;

	// Other
	Wallpaper* m_Wallpaper;
	Texture* m_WallpaperTexture;
};

SpaceResources* g_SpaceResources = NULL;

