#include "sokol_engine.h"


#include "math/v3d.h"
#include "math/mat4.h"


//for time
#include <ctime>


#include "texture.h"



struct Demo : SokolEngine {


	

	sg_sampler sampler;
	


	float color_timer=0;
	const float color_period=5;
	sg_color prev_col, next_col, curr_col;

#pragma region SETUP HELPERS
	
	sg_view getTexture(const std::string& filename)
	{
		
		
			sg_view tex_checker;
			sg_view tex_blank = makeBlankTexture();
			sg_view tex_uv = makeUVTexture(1024, 1024);
			if (filename == " ")
			{
				tex_checker = tex_blank;
			}
			else
			{
				makeTextureFromFile(tex_checker, filename);
			}

			return tex_checker;
		
		
	}

	

		
	

	void setupPipeline() {
		sg_pipeline_desc pipeline_desc{};
		pipeline_desc.shader=sg_make_shader(shd_shader_desc(sg_query_backend()));
		pipeline_desc.layout.attrs[ATTR_shd_pos].format=SG_VERTEXFORMAT_FLOAT3;
		pipeline_desc.layout.attrs[ATTR_shd_norm].format=SG_VERTEXFORMAT_FLOAT3;
		pipeline_desc.layout.attrs[ATTR_shd_uv].format=SG_VERTEXFORMAT_SHORT2N;
		pipeline_desc.index_type=SG_INDEXTYPE_UINT32;//use the index buffer
		pipeline_desc.cull_mode=SG_CULLMODE_FRONT;

		pipeline_desc.depth.write_enabled=true;//use depth buffer
		pipeline_desc.depth.compare=SG_COMPAREFUNC_LESS_EQUAL;
		pipeline=sg_make_pipeline(pipeline_desc);
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


		//updateColor(dt);
	}

#pragma region RENDER HELPERS
	
#pragma endregion

	void userRender() {
		//still not sure what a pass is...
		sg_pass pass{};
		pass.action.colors[0].load_action=SG_LOADACTION_CLEAR;
		pass.action.colors[0].clear_value=curr_col;
		pass.swapchain=sglue_swapchain();
		sg_begin_pass(pass);

		

		sg_end_pass();
		sg_commit();
	}
};