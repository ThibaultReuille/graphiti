#pragma once

class SpaceResources
{
public:
	enum NodeMode { ALL, COLORS, MARKS, NONE };
	enum EdgeMode { WIDE_LINES, LINES, OFF };
	enum EdgeColorMode { NODE_COLOR, LINK_COLOR };

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
			m_EdgeMode = WIDE_LINES;
			ShowEdges = true;
			ShowEdgeActivity = true;
			ShowSpheres = true;
			ShowDebug = false;

			LODSlice = glm::vec2(0.0, 1.0);
			ShowNodeLOD = false;
			ShowEdgeLOD = false;

			m_EdgeColorMode = NODE_COLOR;
		}

		// Nodes
		{
			NodeIcon = new Icon();

			NodeIcon->load("shapes/disk", Assets_Particle_ball_png, sizeof(Assets_Particle_ball_png));

            NodeIcon->load("shapes/cloud", Assets_Textures_Shapes_cloud_png, sizeof(Assets_Textures_Shapes_cloud_png));
            NodeIcon->load("shapes/cross", Assets_Textures_Shapes_cross_png, sizeof(Assets_Textures_Shapes_cross_png));
            // NodeIcon->load("shapes/disk", Assets_Textures_Shapes_disk_png, sizeof(Assets_Textures_Shapes_disk_png));
            NodeIcon->load("shapes/forbidden", Assets_Textures_Shapes_forbidden_png, sizeof(Assets_Textures_Shapes_forbidden_png));
            NodeIcon->load("shapes/heart", Assets_Textures_Shapes_heart_png, sizeof(Assets_Textures_Shapes_heart_png));
            NodeIcon->load("shapes/hexagon", Assets_Textures_Shapes_hexagon_png, sizeof(Assets_Textures_Shapes_hexagon_png));
            NodeIcon->load("shapes/house", Assets_Textures_Shapes_house_png, sizeof(Assets_Textures_Shapes_house_png));
            NodeIcon->load("shapes/losange", Assets_Textures_Shapes_losange_png, sizeof(Assets_Textures_Shapes_losange_png));
            NodeIcon->load("shapes/octagon", Assets_Textures_Shapes_octagon_png, sizeof(Assets_Textures_Shapes_octagon_png));
            NodeIcon->load("shapes/patch", Assets_Textures_Shapes_patch_png, sizeof(Assets_Textures_Shapes_patch_png));
            NodeIcon->load("shapes/pentagon", Assets_Textures_Shapes_pentagon_png, sizeof(Assets_Textures_Shapes_pentagon_png));
            NodeIcon->load("shapes/semicircle", Assets_Textures_Shapes_semicircle_png, sizeof(Assets_Textures_Shapes_semicircle_png));
            NodeIcon->load("shapes/square", Assets_Textures_Shapes_square_png, sizeof(Assets_Textures_Shapes_square_png));
            NodeIcon->load("shapes/star", Assets_Textures_Shapes_star_png, sizeof(Assets_Textures_Shapes_star_png));
            NodeIcon->load("shapes/triangle", Assets_Textures_Shapes_triangle_png, sizeof(Assets_Textures_Shapes_triangle_png)); // TODO : Adjust/Center PNG
            NodeIcon->load("shapes/triangle1", Assets_Textures_Shapes_triangle1_png, sizeof(Assets_Textures_Shapes_triangle1_png));
            NodeIcon->load("shapes/triangle2", Assets_Textures_Shapes_triangle2_png, sizeof(Assets_Textures_Shapes_triangle2_png));

			NodeIcon->load("countries/ad", Assets_Countries_ad_png, sizeof(Assets_Countries_ad_png));
			NodeIcon->load("countries/ae", Assets_Countries_ae_png, sizeof(Assets_Countries_ae_png));
			NodeIcon->load("countries/af", Assets_Countries_af_png, sizeof(Assets_Countries_af_png));
			NodeIcon->load("countries/ag", Assets_Countries_ag_png, sizeof(Assets_Countries_ag_png));
			NodeIcon->load("countries/ai", Assets_Countries_ai_png, sizeof(Assets_Countries_ai_png));
			NodeIcon->load("countries/al", Assets_Countries_al_png, sizeof(Assets_Countries_al_png));
			NodeIcon->load("countries/am", Assets_Countries_am_png, sizeof(Assets_Countries_am_png));
			NodeIcon->load("countries/an", Assets_Countries_an_png, sizeof(Assets_Countries_an_png));
			NodeIcon->load("countries/ao", Assets_Countries_ao_png, sizeof(Assets_Countries_ao_png));
			NodeIcon->load("countries/ar", Assets_Countries_ar_png, sizeof(Assets_Countries_ar_png));
			NodeIcon->load("countries/as", Assets_Countries_as_png, sizeof(Assets_Countries_as_png));
			NodeIcon->load("countries/at", Assets_Countries_at_png, sizeof(Assets_Countries_at_png));
			NodeIcon->load("countries/au", Assets_Countries_au_png, sizeof(Assets_Countries_au_png));
			NodeIcon->load("countries/aw", Assets_Countries_aw_png, sizeof(Assets_Countries_aw_png));
			NodeIcon->load("countries/ax", Assets_Countries_ax_png, sizeof(Assets_Countries_ax_png));
			NodeIcon->load("countries/ba", Assets_Countries_ba_png, sizeof(Assets_Countries_ba_png));
			NodeIcon->load("countries/bb", Assets_Countries_bb_png, sizeof(Assets_Countries_bb_png));
			NodeIcon->load("countries/bd", Assets_Countries_bd_png, sizeof(Assets_Countries_bd_png));
			NodeIcon->load("countries/be", Assets_Countries_be_png, sizeof(Assets_Countries_be_png));
			NodeIcon->load("countries/bf", Assets_Countries_bf_png, sizeof(Assets_Countries_bf_png));
			NodeIcon->load("countries/bg", Assets_Countries_bg_png, sizeof(Assets_Countries_bg_png));
			NodeIcon->load("countries/bh", Assets_Countries_bh_png, sizeof(Assets_Countries_bh_png));
			NodeIcon->load("countries/bi", Assets_Countries_bi_png, sizeof(Assets_Countries_bi_png));
			NodeIcon->load("countries/bj", Assets_Countries_bj_png, sizeof(Assets_Countries_bj_png));
			NodeIcon->load("countries/bm", Assets_Countries_bm_png, sizeof(Assets_Countries_bm_png));
			NodeIcon->load("countries/bn", Assets_Countries_bn_png, sizeof(Assets_Countries_bn_png));
			NodeIcon->load("countries/bo", Assets_Countries_bo_png, sizeof(Assets_Countries_bo_png));
			NodeIcon->load("countries/br", Assets_Countries_br_png, sizeof(Assets_Countries_br_png));
			NodeIcon->load("countries/bs", Assets_Countries_bs_png, sizeof(Assets_Countries_bs_png));
			NodeIcon->load("countries/bt", Assets_Countries_bt_png, sizeof(Assets_Countries_bt_png));
			NodeIcon->load("countries/bw", Assets_Countries_bw_png, sizeof(Assets_Countries_bw_png));
			NodeIcon->load("countries/by", Assets_Countries_by_png, sizeof(Assets_Countries_by_png));
			NodeIcon->load("countries/bz", Assets_Countries_bz_png, sizeof(Assets_Countries_bz_png));
			NodeIcon->load("countries/ca", Assets_Countries_ca_png, sizeof(Assets_Countries_ca_png));
			NodeIcon->load("countries/cd", Assets_Countries_cd_png, sizeof(Assets_Countries_cd_png));
			NodeIcon->load("countries/cf", Assets_Countries_cf_png, sizeof(Assets_Countries_cf_png));
			NodeIcon->load("countries/cg", Assets_Countries_cg_png, sizeof(Assets_Countries_cg_png));
			NodeIcon->load("countries/ch", Assets_Countries_ch_png, sizeof(Assets_Countries_ch_png));
			NodeIcon->load("countries/ci", Assets_Countries_ci_png, sizeof(Assets_Countries_ci_png));
			NodeIcon->load("countries/ck", Assets_Countries_ck_png, sizeof(Assets_Countries_ck_png));
			NodeIcon->load("countries/cl", Assets_Countries_cl_png, sizeof(Assets_Countries_cl_png));
			NodeIcon->load("countries/cm", Assets_Countries_cm_png, sizeof(Assets_Countries_cm_png));
			NodeIcon->load("countries/cn", Assets_Countries_cn_png, sizeof(Assets_Countries_cn_png));
			NodeIcon->load("countries/co", Assets_Countries_co_png, sizeof(Assets_Countries_co_png));
			NodeIcon->load("countries/cr", Assets_Countries_cr_png, sizeof(Assets_Countries_cr_png));
			NodeIcon->load("countries/cu", Assets_Countries_cu_png, sizeof(Assets_Countries_cu_png));
			NodeIcon->load("countries/cv", Assets_Countries_cv_png, sizeof(Assets_Countries_cv_png));
			NodeIcon->load("countries/cx", Assets_Countries_cx_png, sizeof(Assets_Countries_cx_png));
			NodeIcon->load("countries/cy", Assets_Countries_cy_png, sizeof(Assets_Countries_cy_png));
			NodeIcon->load("countries/cz", Assets_Countries_cz_png, sizeof(Assets_Countries_cz_png));
			NodeIcon->load("countries/de", Assets_Countries_de_png, sizeof(Assets_Countries_de_png));
			NodeIcon->load("countries/dj", Assets_Countries_dj_png, sizeof(Assets_Countries_dj_png));
			NodeIcon->load("countries/dk", Assets_Countries_dk_png, sizeof(Assets_Countries_dk_png));
			NodeIcon->load("countries/dm", Assets_Countries_dm_png, sizeof(Assets_Countries_dm_png));
			NodeIcon->load("countries/do", Assets_Countries_do_png, sizeof(Assets_Countries_do_png));
			NodeIcon->load("countries/dz", Assets_Countries_dz_png, sizeof(Assets_Countries_dz_png));
			NodeIcon->load("countries/ec", Assets_Countries_ec_png, sizeof(Assets_Countries_ec_png));
			NodeIcon->load("countries/ee", Assets_Countries_ee_png, sizeof(Assets_Countries_ee_png));
			NodeIcon->load("countries/eg", Assets_Countries_eg_png, sizeof(Assets_Countries_eg_png));
			NodeIcon->load("countries/er", Assets_Countries_er_png, sizeof(Assets_Countries_er_png));
			NodeIcon->load("countries/es", Assets_Countries_es_png, sizeof(Assets_Countries_es_png));
			NodeIcon->load("countries/et", Assets_Countries_et_png, sizeof(Assets_Countries_et_png));
			NodeIcon->load("countries/eu", Assets_Countries_eu_png, sizeof(Assets_Countries_eu_png));
			NodeIcon->load("countries/fi", Assets_Countries_fi_png, sizeof(Assets_Countries_fi_png));
			NodeIcon->load("countries/fj", Assets_Countries_fj_png, sizeof(Assets_Countries_fj_png));
			NodeIcon->load("countries/fk", Assets_Countries_fk_png, sizeof(Assets_Countries_fk_png));
			NodeIcon->load("countries/fm", Assets_Countries_fm_png, sizeof(Assets_Countries_fm_png));
			NodeIcon->load("countries/fo", Assets_Countries_fo_png, sizeof(Assets_Countries_fo_png));
			NodeIcon->load("countries/fr", Assets_Countries_fr_png, sizeof(Assets_Countries_fr_png));
			NodeIcon->load("countries/ga", Assets_Countries_ga_png, sizeof(Assets_Countries_ga_png));
			NodeIcon->load("countries/gd", Assets_Countries_gd_png, sizeof(Assets_Countries_gd_png));
			NodeIcon->load("countries/ge", Assets_Countries_ge_png, sizeof(Assets_Countries_ge_png));
			NodeIcon->load("countries/gg", Assets_Countries_gg_png, sizeof(Assets_Countries_gg_png));
			NodeIcon->load("countries/gh", Assets_Countries_gh_png, sizeof(Assets_Countries_gh_png));
			NodeIcon->load("countries/gi", Assets_Countries_gi_png, sizeof(Assets_Countries_gi_png));
			NodeIcon->load("countries/gl", Assets_Countries_gl_png, sizeof(Assets_Countries_gl_png));
			NodeIcon->load("countries/gm", Assets_Countries_gm_png, sizeof(Assets_Countries_gm_png));
			NodeIcon->load("countries/gn", Assets_Countries_gn_png, sizeof(Assets_Countries_gn_png));
			NodeIcon->load("countries/gq", Assets_Countries_gq_png, sizeof(Assets_Countries_gq_png));
			NodeIcon->load("countries/gr", Assets_Countries_gr_png, sizeof(Assets_Countries_gr_png));
			// NodeIcon->load("countries/gr-cy", Assets_Countries_gr_cy_png, sizeof(Assets_Countries_gr_cy_png));
			NodeIcon->load("countries/gs", Assets_Countries_gs_png, sizeof(Assets_Countries_gs_png));
			NodeIcon->load("countries/gt", Assets_Countries_gt_png, sizeof(Assets_Countries_gt_png));
			NodeIcon->load("countries/gu", Assets_Countries_gu_png, sizeof(Assets_Countries_gu_png));
			NodeIcon->load("countries/gw", Assets_Countries_gw_png, sizeof(Assets_Countries_gw_png));
			NodeIcon->load("countries/gy", Assets_Countries_gy_png, sizeof(Assets_Countries_gy_png));
			NodeIcon->load("countries/hk", Assets_Countries_hk_png, sizeof(Assets_Countries_hk_png));
			NodeIcon->load("countries/hn", Assets_Countries_hn_png, sizeof(Assets_Countries_hn_png));
			NodeIcon->load("countries/hr", Assets_Countries_hr_png, sizeof(Assets_Countries_hr_png));
			NodeIcon->load("countries/ht", Assets_Countries_ht_png, sizeof(Assets_Countries_ht_png));
			NodeIcon->load("countries/hu", Assets_Countries_hu_png, sizeof(Assets_Countries_hu_png));
			NodeIcon->load("countries/id", Assets_Countries_id_png, sizeof(Assets_Countries_id_png));
			NodeIcon->load("countries/ie", Assets_Countries_ie_png, sizeof(Assets_Countries_ie_png));
			NodeIcon->load("countries/il", Assets_Countries_il_png, sizeof(Assets_Countries_il_png));
			NodeIcon->load("countries/im", Assets_Countries_im_png, sizeof(Assets_Countries_im_png));
			NodeIcon->load("countries/in", Assets_Countries_in_png, sizeof(Assets_Countries_in_png));
			NodeIcon->load("countries/io", Assets_Countries_io_png, sizeof(Assets_Countries_io_png));
			NodeIcon->load("countries/iq", Assets_Countries_iq_png, sizeof(Assets_Countries_iq_png));
			NodeIcon->load("countries/ir", Assets_Countries_ir_png, sizeof(Assets_Countries_ir_png));
			NodeIcon->load("countries/is", Assets_Countries_is_png, sizeof(Assets_Countries_is_png));
			NodeIcon->load("countries/it", Assets_Countries_it_png, sizeof(Assets_Countries_it_png));
			NodeIcon->load("countries/je", Assets_Countries_je_png, sizeof(Assets_Countries_je_png));
			NodeIcon->load("countries/jm", Assets_Countries_jm_png, sizeof(Assets_Countries_jm_png));
			NodeIcon->load("countries/jo", Assets_Countries_jo_png, sizeof(Assets_Countries_jo_png));
			NodeIcon->load("countries/jp", Assets_Countries_jp_png, sizeof(Assets_Countries_jp_png));
			NodeIcon->load("countries/ke", Assets_Countries_ke_png, sizeof(Assets_Countries_ke_png));
			NodeIcon->load("countries/kg", Assets_Countries_kg_png, sizeof(Assets_Countries_kg_png));
			NodeIcon->load("countries/kh", Assets_Countries_kh_png, sizeof(Assets_Countries_kh_png));
			NodeIcon->load("countries/ki", Assets_Countries_ki_png, sizeof(Assets_Countries_ki_png));
			NodeIcon->load("countries/km", Assets_Countries_km_png, sizeof(Assets_Countries_km_png));
			NodeIcon->load("countries/kn", Assets_Countries_kn_png, sizeof(Assets_Countries_kn_png));
			NodeIcon->load("countries/kp", Assets_Countries_kp_png, sizeof(Assets_Countries_kp_png));
			NodeIcon->load("countries/kr", Assets_Countries_kr_png, sizeof(Assets_Countries_kr_png));
			NodeIcon->load("countries/kw", Assets_Countries_kw_png, sizeof(Assets_Countries_kw_png));
			NodeIcon->load("countries/ky", Assets_Countries_ky_png, sizeof(Assets_Countries_ky_png));
			NodeIcon->load("countries/kz", Assets_Countries_kz_png, sizeof(Assets_Countries_kz_png));
			NodeIcon->load("countries/la", Assets_Countries_la_png, sizeof(Assets_Countries_la_png));
			NodeIcon->load("countries/lb", Assets_Countries_lb_png, sizeof(Assets_Countries_lb_png));
			NodeIcon->load("countries/lc", Assets_Countries_lc_png, sizeof(Assets_Countries_lc_png));
			NodeIcon->load("countries/li", Assets_Countries_li_png, sizeof(Assets_Countries_li_png));
			NodeIcon->load("countries/lk", Assets_Countries_lk_png, sizeof(Assets_Countries_lk_png));
			NodeIcon->load("countries/lr", Assets_Countries_lr_png, sizeof(Assets_Countries_lr_png));
			NodeIcon->load("countries/ls", Assets_Countries_ls_png, sizeof(Assets_Countries_ls_png));
			NodeIcon->load("countries/lt", Assets_Countries_lt_png, sizeof(Assets_Countries_lt_png));
			NodeIcon->load("countries/lu", Assets_Countries_lu_png, sizeof(Assets_Countries_lu_png));
			NodeIcon->load("countries/lv", Assets_Countries_lv_png, sizeof(Assets_Countries_lv_png));
			NodeIcon->load("countries/ly", Assets_Countries_ly_png, sizeof(Assets_Countries_ly_png));
			NodeIcon->load("countries/ma", Assets_Countries_ma_png, sizeof(Assets_Countries_ma_png));
			NodeIcon->load("countries/mc", Assets_Countries_mc_png, sizeof(Assets_Countries_mc_png));
			NodeIcon->load("countries/md", Assets_Countries_md_png, sizeof(Assets_Countries_md_png));
			NodeIcon->load("countries/me", Assets_Countries_me_png, sizeof(Assets_Countries_me_png));
			NodeIcon->load("countries/mg", Assets_Countries_mg_png, sizeof(Assets_Countries_mg_png));
			NodeIcon->load("countries/mh", Assets_Countries_mh_png, sizeof(Assets_Countries_mh_png));
			NodeIcon->load("countries/ml", Assets_Countries_ml_png, sizeof(Assets_Countries_ml_png));
			NodeIcon->load("countries/mn", Assets_Countries_mn_png, sizeof(Assets_Countries_mn_png));
			NodeIcon->load("countries/mo", Assets_Countries_mo_png, sizeof(Assets_Countries_mo_png));
			NodeIcon->load("countries/mp", Assets_Countries_mp_png, sizeof(Assets_Countries_mp_png));
			NodeIcon->load("countries/mq", Assets_Countries_mq_png, sizeof(Assets_Countries_mq_png));
			NodeIcon->load("countries/mr", Assets_Countries_mr_png, sizeof(Assets_Countries_mr_png));
			NodeIcon->load("countries/ms", Assets_Countries_ms_png, sizeof(Assets_Countries_ms_png));
			NodeIcon->load("countries/mt", Assets_Countries_mt_png, sizeof(Assets_Countries_mt_png));
			NodeIcon->load("countries/mu", Assets_Countries_mu_png, sizeof(Assets_Countries_mu_png));
			NodeIcon->load("countries/mv", Assets_Countries_mv_png, sizeof(Assets_Countries_mv_png));
			NodeIcon->load("countries/mw", Assets_Countries_mw_png, sizeof(Assets_Countries_mw_png));
			NodeIcon->load("countries/mx", Assets_Countries_mx_png, sizeof(Assets_Countries_mx_png));
			NodeIcon->load("countries/my", Assets_Countries_my_png, sizeof(Assets_Countries_my_png));
			NodeIcon->load("countries/mz", Assets_Countries_mz_png, sizeof(Assets_Countries_mz_png));
			NodeIcon->load("countries/na", Assets_Countries_na_png, sizeof(Assets_Countries_na_png));
			NodeIcon->load("countries/ne", Assets_Countries_ne_png, sizeof(Assets_Countries_ne_png));
			NodeIcon->load("countries/nf", Assets_Countries_nf_png, sizeof(Assets_Countries_nf_png));
			NodeIcon->load("countries/ng", Assets_Countries_ng_png, sizeof(Assets_Countries_ng_png));
			NodeIcon->load("countries/ni", Assets_Countries_ni_png, sizeof(Assets_Countries_ni_png));
			NodeIcon->load("countries/nl", Assets_Countries_nl_png, sizeof(Assets_Countries_nl_png));
			NodeIcon->load("countries/nm", Assets_Countries_nm_png, sizeof(Assets_Countries_nm_png));
			NodeIcon->load("countries/no", Assets_Countries_no_png, sizeof(Assets_Countries_no_png));
			NodeIcon->load("countries/np", Assets_Countries_np_png, sizeof(Assets_Countries_np_png));
			NodeIcon->load("countries/nr", Assets_Countries_nr_png, sizeof(Assets_Countries_nr_png));
			NodeIcon->load("countries/nu", Assets_Countries_nu_png, sizeof(Assets_Countries_nu_png));
			NodeIcon->load("countries/nz", Assets_Countries_nz_png, sizeof(Assets_Countries_nz_png));
			NodeIcon->load("countries/om", Assets_Countries_om_png, sizeof(Assets_Countries_om_png));
			NodeIcon->load("countries/pa", Assets_Countries_pa_png, sizeof(Assets_Countries_pa_png));
			NodeIcon->load("countries/pf", Assets_Countries_pf_png, sizeof(Assets_Countries_pf_png));
			NodeIcon->load("countries/pg", Assets_Countries_pg_png, sizeof(Assets_Countries_pg_png));
			NodeIcon->load("countries/ph", Assets_Countries_ph_png, sizeof(Assets_Countries_ph_png));
			NodeIcon->load("countries/pk", Assets_Countries_pk_png, sizeof(Assets_Countries_pk_png));
			NodeIcon->load("countries/pl", Assets_Countries_pl_png, sizeof(Assets_Countries_pl_png));
			NodeIcon->load("countries/pm", Assets_Countries_pm_png, sizeof(Assets_Countries_pm_png));
			NodeIcon->load("countries/pn", Assets_Countries_pn_png, sizeof(Assets_Countries_pn_png));
			NodeIcon->load("countries/pr", Assets_Countries_pr_png, sizeof(Assets_Countries_pr_png));
			NodeIcon->load("countries/pt", Assets_Countries_pt_png, sizeof(Assets_Countries_pt_png));
			NodeIcon->load("countries/pw", Assets_Countries_pw_png, sizeof(Assets_Countries_pw_png));
			NodeIcon->load("countries/py", Assets_Countries_py_png, sizeof(Assets_Countries_py_png));
			NodeIcon->load("countries/qa", Assets_Countries_qa_png, sizeof(Assets_Countries_qa_png));
			NodeIcon->load("countries/ro", Assets_Countries_ro_png, sizeof(Assets_Countries_ro_png));
			NodeIcon->load("countries/rs", Assets_Countries_rs_png, sizeof(Assets_Countries_rs_png));
			NodeIcon->load("countries/ru", Assets_Countries_ru_png, sizeof(Assets_Countries_ru_png));
			NodeIcon->load("countries/rw", Assets_Countries_rw_png, sizeof(Assets_Countries_rw_png));
			NodeIcon->load("countries/sa", Assets_Countries_sa_png, sizeof(Assets_Countries_sa_png));
			NodeIcon->load("countries/sb", Assets_Countries_sb_png, sizeof(Assets_Countries_sb_png));
			NodeIcon->load("countries/sc", Assets_Countries_sc_png, sizeof(Assets_Countries_sc_png));
			NodeIcon->load("countries/sd", Assets_Countries_sd_png, sizeof(Assets_Countries_sd_png));
			NodeIcon->load("countries/se", Assets_Countries_se_png, sizeof(Assets_Countries_se_png));
			NodeIcon->load("countries/sg", Assets_Countries_sg_png, sizeof(Assets_Countries_sg_png));
			NodeIcon->load("countries/sh", Assets_Countries_sh_png, sizeof(Assets_Countries_sh_png));
			NodeIcon->load("countries/si", Assets_Countries_si_png, sizeof(Assets_Countries_si_png));
			NodeIcon->load("countries/sk", Assets_Countries_sk_png, sizeof(Assets_Countries_sk_png));
			NodeIcon->load("countries/sl", Assets_Countries_sl_png, sizeof(Assets_Countries_sl_png));
			NodeIcon->load("countries/sm", Assets_Countries_sm_png, sizeof(Assets_Countries_sm_png));
			NodeIcon->load("countries/sn", Assets_Countries_sn_png, sizeof(Assets_Countries_sn_png));
			NodeIcon->load("countries/so", Assets_Countries_so_png, sizeof(Assets_Countries_so_png));
			NodeIcon->load("countries/sr", Assets_Countries_sr_png, sizeof(Assets_Countries_sr_png));
			NodeIcon->load("countries/st", Assets_Countries_st_png, sizeof(Assets_Countries_st_png));
			NodeIcon->load("countries/sv", Assets_Countries_sv_png, sizeof(Assets_Countries_sv_png));
			NodeIcon->load("countries/sy", Assets_Countries_sy_png, sizeof(Assets_Countries_sy_png));
			NodeIcon->load("countries/sz", Assets_Countries_sz_png, sizeof(Assets_Countries_sz_png));
			NodeIcon->load("countries/tc", Assets_Countries_tc_png, sizeof(Assets_Countries_tc_png));
			NodeIcon->load("countries/td", Assets_Countries_td_png, sizeof(Assets_Countries_td_png));
			NodeIcon->load("countries/tg", Assets_Countries_tg_png, sizeof(Assets_Countries_tg_png));
			NodeIcon->load("countries/th", Assets_Countries_th_png, sizeof(Assets_Countries_th_png));
			NodeIcon->load("countries/tj", Assets_Countries_tj_png, sizeof(Assets_Countries_tj_png));
			NodeIcon->load("countries/tl", Assets_Countries_tl_png, sizeof(Assets_Countries_tl_png));
			NodeIcon->load("countries/tm", Assets_Countries_tm_png, sizeof(Assets_Countries_tm_png));
			NodeIcon->load("countries/tn", Assets_Countries_tn_png, sizeof(Assets_Countries_tn_png));
			NodeIcon->load("countries/to", Assets_Countries_to_png, sizeof(Assets_Countries_to_png));
			NodeIcon->load("countries/tr", Assets_Countries_tr_png, sizeof(Assets_Countries_tr_png));
			NodeIcon->load("countries/tt", Assets_Countries_tt_png, sizeof(Assets_Countries_tt_png));
			NodeIcon->load("countries/tv", Assets_Countries_tv_png, sizeof(Assets_Countries_tv_png));
			NodeIcon->load("countries/tw", Assets_Countries_tw_png, sizeof(Assets_Countries_tw_png));
			NodeIcon->load("countries/tz", Assets_Countries_tz_png, sizeof(Assets_Countries_tz_png));
			NodeIcon->load("countries/ua", Assets_Countries_ua_png, sizeof(Assets_Countries_ua_png));
			NodeIcon->load("countries/ug", Assets_Countries_ug_png, sizeof(Assets_Countries_ug_png));
			NodeIcon->load("countries/uk", Assets_Countries_uk_png, sizeof(Assets_Countries_uk_png));
			NodeIcon->load("countries/gb", Assets_Countries_uk_png, sizeof(Assets_Countries_uk_png)); // Duplicating UK as GB
			NodeIcon->load("countries/us", Assets_Countries_us_png, sizeof(Assets_Countries_us_png));
			NodeIcon->load("countries/uy", Assets_Countries_uy_png, sizeof(Assets_Countries_uy_png));
			NodeIcon->load("countries/uz", Assets_Countries_uz_png, sizeof(Assets_Countries_uz_png));
			NodeIcon->load("countries/vc", Assets_Countries_vc_png, sizeof(Assets_Countries_vc_png));
			NodeIcon->load("countries/ve", Assets_Countries_ve_png, sizeof(Assets_Countries_ve_png));
			NodeIcon->load("countries/vg", Assets_Countries_vg_png, sizeof(Assets_Countries_vg_png));
			NodeIcon->load("countries/vh", Assets_Countries_vh_png, sizeof(Assets_Countries_vh_png));
			NodeIcon->load("countries/vi", Assets_Countries_vi_png, sizeof(Assets_Countries_vi_png));
			NodeIcon->load("countries/vn", Assets_Countries_vn_png, sizeof(Assets_Countries_vn_png));
			NodeIcon->load("countries/vu", Assets_Countries_vu_png, sizeof(Assets_Countries_vu_png));
			NodeIcon->load("countries/wf", Assets_Countries_wf_png, sizeof(Assets_Countries_wf_png));
			NodeIcon->load("countries/ws", Assets_Countries_ws_png, sizeof(Assets_Countries_ws_png));
			NodeIcon->load("countries/ye", Assets_Countries_ye_png, sizeof(Assets_Countries_ye_png));
			NodeIcon->load("countries/yt", Assets_Countries_yt_png, sizeof(Assets_Countries_yt_png));
			NodeIcon->load("countries/yu", Assets_Countries_yu_png, sizeof(Assets_Countries_yu_png));
			NodeIcon->load("countries/za", Assets_Countries_za_png, sizeof(Assets_Countries_za_png));
			NodeIcon->load("countries/zm", Assets_Countries_zm_png, sizeof(Assets_Countries_zm_png));
			NodeIcon->load("countries/zw", Assets_Countries_zw_png, sizeof(Assets_Countries_zw_png));

			NodeMarkIcon = new Icon();
			NodeMarkIcon->load("mark", Assets_Textures_mark_png, sizeof(Assets_Textures_mark_png));
			NodeTargetIcon = new Icon();
			NodeTargetIcon->load("target", Assets_target_png, sizeof(Assets_target_png));
			//m_NodeSphereTexture = ResourceManager::getInstance().loadTexture("perlin", Assets_Spheres_perlin_jpg, sizeof(Assets_Spheres_perlin_jpg));

			NodeFont = new rd::Font();
			NodeActivityIcon = new Icon();
			NodeActivityIcon->load("node_activity", Assets_SpaceView_node_activity_png, sizeof(Assets_SpaceView_node_activity_png));
		}

		// Edges
		{
            EdgeActivityIcon = new Icon();
            EdgeActivityIcon->load("edge_activity", Assets_Particle_metaball_png, sizeof(Assets_Particle_metaball_png));

		    EdgeShader = ResourceManager::getInstance().loadShader("graph:edges",
		            Assets_Shaders_Primitives_wideline_vert, sizeof(Assets_Shaders_Primitives_wideline_vert),
		            Assets_Shaders_Primitives_wideline_frag, sizeof(Assets_Shaders_Primitives_wideline_frag));

		    EdgeStyleIcon = new Icon();
            EdgeStyleIcon->load("styles/solid", Assets_SpaceView_EdgeStyles_solid_png, sizeof(Assets_SpaceView_EdgeStyles_solid_png));
            EdgeStyleIcon->load("styles/circles", Assets_SpaceView_EdgeStyles_circles_png, sizeof(Assets_SpaceView_EdgeStyles_circles_png));
            EdgeStyleIcon->load("styles/cross", Assets_SpaceView_EdgeStyles_cross_png, sizeof(Assets_SpaceView_EdgeStyles_cross_png));
            EdgeStyleIcon->load("styles/dashed", Assets_SpaceView_EdgeStyles_dashed_png, sizeof(Assets_SpaceView_EdgeStyles_dashed_png));
            EdgeStyleIcon->load("styles/dots", Assets_SpaceView_EdgeStyles_dots_png, sizeof(Assets_SpaceView_EdgeStyles_dots_png));
            EdgeStyleIcon->load("styles/triangles", Assets_SpaceView_EdgeStyles_triangles_png, sizeof(Assets_SpaceView_EdgeStyles_triangles_png));
            EdgeStyleIcon->load("styles/squares", Assets_SpaceView_EdgeStyles_squares_png, sizeof(Assets_SpaceView_EdgeStyles_squares_png));
            EdgeStyleIcon->load("styles/zigzag", Assets_SpaceView_EdgeStyles_zigzag_png, sizeof(Assets_SpaceView_EdgeStyles_zigzag_png));
		}

		// Spheres
		{
			SphereIcon = new Icon();
			SphereIcon->load("circle", Assets_circle_png, sizeof(Assets_circle_png));
		}

		// Others
		#ifndef EMSCRIPTEN // NOTE : WebGL doesn't like rectangle images
		    m_WallpaperTexture = ResourceManager::getInstance().loadTexture("background", Assets_background_png, sizeof(Assets_background_png));
		    m_Wallpaper = new Wallpaper();
		    m_Wallpaper->setTexture(m_WallpaperTexture);
		#endif
	}

	~SpaceResources()
	{
		SAFE_DELETE(NodeIcon);
		SAFE_DELETE(NodeMarkIcon);
		SAFE_DELETE(NodeTargetIcon);
		SAFE_DELETE(NodeFont);
		SAFE_DELETE(NodeActivityIcon);

        ResourceManager::getInstance().unload(EdgeShader);
        SAFE_DELETE(EdgeStyleIcon);
        SAFE_DELETE(EdgeActivityIcon);

		SAFE_DELETE(SphereIcon);

		#ifndef EMSCRIPTEN // NOTE : WebGL doesn't like rectangle images
        	SAFE_DELETE(m_Wallpaper);
			ResourceManager::getInstance().unload(m_WallpaperTexture);
		#endif
	}

	inline bool isNodeVisible(float lod)
	{
		if (!ShowNodeLOD)
			return true;
		return (lod < LODSlice[0] || lod > LODSlice[1]) ? false : true;
	}

	inline bool isEdgeVisible(float lod)
	{
		if (!ShowEdgeLOD)
			return true;
		return (lod < LODSlice[0] || lod > LODSlice[1]) ? false : true;
	}

	// Parameters
	float NodeIconSize;
	NodeMode ShowNodeShapes;
	bool ShowNodeLabels;
	bool ShowNodeActivity;

	float EdgeSize;
	EdgeMode m_EdgeMode;
	bool ShowEdges;
	bool ShowEdgeActivity;
	bool ShowSpheres;

	bool ShowDebug;

    bool ShowNodeLOD;
    bool ShowEdgeLOD;
	glm::vec2 LODSlice;

	GraphModel* Model;

	// Nodes
	Icon* NodeIcon;
	Icon* NodeMarkIcon;
	Icon* NodeTargetIcon;
	rd::Font* NodeFont;
	Icon* NodeActivityIcon;

	// Edges
    Shader::Program* EdgeShader;
    Icon* EdgeStyleIcon;
	EdgeColorMode m_EdgeColorMode;
	Icon* EdgeActivityIcon;

	// Spheres
	Icon* SphereIcon;

	// Other
	#ifndef EMSCRIPTEN
		Wallpaper* m_Wallpaper;
		Texture* m_WallpaperTexture;
	#endif
};

SpaceResources* g_SpaceResources = NULL;

