#include "sokol_engine.h"
#include "math/v3d.h"
#include "math/mat4.h"
#include <vector>


//for time
#include <ctime>


//#include "texture.h"



struct Demo : SokolEngine {

	sg_pipeline pipeline;
	sg_shader shd;
	sg_image image;
	sg_sampler linear_sampler;
	sg_image perlin_image;
	


	float color_timer=0;
	const float color_period=5;
	sg_color prev_col, next_col, curr_col;

#pragma region SETUP HELPERS


	sg_image load_image(const char* filename)
	{
		int width, height, channels;
		uint8_t* data = stbi_load(filename, &width, &height, &channels, 4);
		sg_image img = { SG_INVALID_ID };
		if (!data) { return img; }

		sg_image_desc image_desc = { 0 };
		image_desc.width = width;
		image_desc.height = height;
		image_desc.data.mip_levels[0].ptr = data;
		image_desc.data.mip_levels[0].size = (size_t)(width * height * 4);
		image = sg_make_image(&image_desc);
		stbi_image_free(data);

		return img;
	}

	void setupPipeline() {
		sg_desc sgdesc = { 0 };
		sgdesc.environment = sglue_environment();
		sg_setup(&sgdesc);
		if (!sg_isvalid()) {
			fprintf(stderr, "Failed to create Sokol GFX context!\n");
			exit(-1);
		}

		// initialize Sokol GP
		sgp_desc sgpdesc = { 0 };
		sgp_setup(&sgpdesc);
		if (!sgp_is_valid()) {
			fprintf(stderr, "Failed to create Sokol GP context: %s\n", sgp_get_error_message(sgp_get_last_error()));
			exit(-1);
		}

		image = load_image("lpc_winter_preview.png");
		perlin_image = load_image("lpc_winter_preview.png");
		if (sg_query_image_state(perlin_image) != SG_RESOURCESTATE_VALID) {
			fprintf(stderr, "failed to load images");
			exit(-1);
		}
		if (sg_query_image_state(image) != SG_RESOURCESTATE_VALID) {
			fprintf(stderr, "failed to load images");
			exit(-1);
		}

		sg_sampler_desc linear_sampler_desc;
		linear_sampler_desc.min_filter = SG_FILTER_LINEAR;
		linear_sampler_desc.mag_filter = SG_FILTER_LINEAR;
		linear_sampler_desc.wrap_u = SG_WRAP_REPEAT;
		linear_sampler_desc.wrap_v = SG_WRAP_REPEAT;

		linear_sampler = sg_make_sampler(&linear_sampler_desc);
		if (sg_query_sampler_state(linear_sampler) != SG_RESOURCESTATE_VALID) {
			fprintf(stderr, "failed to create linear sampler");
			exit(-1);
		}

		// initialize shader
		shd = sg_make_shader(effect_program_shader_desc(sg_query_backend()));
		if (sg_query_shader_state(shd) != SG_RESOURCESTATE_VALID) {
			fprintf(stderr, "failed to make custom pipeline shader\n");
			exit(-1);
		}

		sgp_pipeline_desc pip_desc = { 0 };
		pip_desc.shader = shd;
		pip_desc.has_vs_color = true;
		pipeline = sgp_make_pipeline(&pip_desc);
		if (sg_query_pipeline_state(pipeline) != SG_RESOURCESTATE_VALID) {
			fprintf(stderr, "failed to make custom pipeline\n");
			exit(-1);
		}


	}
#pragma endregion

	void userCreate() override {
		app_title="Solids of Revolution & Billboarding Demo";

		std::srand(std::time(0));

		
	
		setupPipeline();

		
	}

#pragma region UPDATE HELPERS



	

	

	void handleUserInput(float dt) {
		

	

		if(getKey(SAPP_KEYCODE_F11).pressed) {
			sapp_toggle_fullscreen();
		}

		
	}


	//void updateColor(float dt) {
	//	if(color_timer>color_period) {
	//		color_timer-=color_period;
	//
	//		prev_col=next_col;
	//		next_col=randomPastel();
	//	}
	//
	//	//lerp between prev & next
	//	float t=color_timer/color_period;
	//	curr_col=prev_col+t*(next_col-prev_col);
	//
	//	color_timer+=dt;
	//}
#pragma endregion

	void userUpdate(float dt) {
	

		handleUserInput(dt);

		


		
	}

#pragma region RENDER HELPERS
	
#pragma endregion

	void userRender() {

		int window_width = sapp_width(), window_height = sapp_height();
		sgp_begin(window_width, window_height);

		float secs = sapp_frame_count() * sapp_frame_duration();
		sg_image_desc image_desc = sg_query_image_desc(image);
		float window_ratio = window_width / (float)window_height;
		float image_ratio = image_desc.width / (float)image_desc.height;

		effect_fs_uniforms_t uniforms = { 0 };
		uniforms.iVelocity.x = 0.02f;
		uniforms.iVelocity.y = 0.01f;
		uniforms.iPressure = 0.3f;
		uniforms.iTime = secs;
		uniforms.iWarpiness = 0.2f;
		uniforms.iRatio = image_ratio;
		uniforms.iZoom = 0.4f;
		uniforms.iLevel = 1.0f;

		sgp_set_pipeline(pipeline);
		sgp_set_uniform(NULL, 0, &uniforms, sizeof(effect_fs_uniforms_t));
		sgp_set_image(VIEW_effect_iTexChannel0, image);
		sgp_set_image(VIEW_effect_iTexChannel1, perlin_image);
		sgp_set_sampler(SMP_effect_iSmpChannel0, linear_sampler);
		sgp_set_sampler(SMP_effect_iSmpChannel1, linear_sampler);
		float width = (window_ratio >= image_ratio) ? window_width : image_ratio * window_height;
		float height = (window_ratio >= image_ratio) ? window_width / image_ratio : window_height;
		sgp_draw_filled_rect(0, 0, width, height);
		sgp_reset_image(VIEW_effect_iTexChannel0);
		sgp_reset_image(VIEW_effect_iTexChannel1);
		sgp_reset_sampler(SMP_effect_iSmpChannel0);
		sgp_reset_sampler(SMP_effect_iSmpChannel1);
		sgp_reset_pipeline();


		//still not sure what a pass is...
		sg_pass pass{};
		pass.action.colors[0].load_action=SG_LOADACTION_CLEAR;
		pass.action.colors[0].clear_value=curr_col;
		pass.swapchain=sglue_swapchain();
		sg_begin_pass(pass);
		sgp_flush();
		sgp_end();

		sg_end_pass();
		sg_commit();
	}

	void userDestroy() override
	{
		sg_destroy_image(image);
		sg_destroy_image(perlin_image);
		sg_destroy_pipeline(pipeline);
		sg_destroy_shader(shd);
		sgp_shutdown();
		sg_shutdown();
	}
};