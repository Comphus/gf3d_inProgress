#include <SDL.h>            

#include "simple_logger.h"
#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_matrix.h"
#include "gf3d_camera.h"
#include "gf3d_vector.h"
#include "gf3d_texture.h"
#include "gf3d_entity.h"

int main(int argc,char *argv[])
{
    int done = 0;
	int zoomCamera = 0;
	float zoomFov = 0.001;
	float spaceBar = 0.0;
	int spaceDown = 0;
	float mx, my;
	float sens;
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;
    Entity *ent;
    Entity *ent2;
	Entity *ent3;
	Model *model;
	SDL_Event event;
    
    init_logger("gf3d.log");    
    slog("gf3d begin");
    gf3d_vgraphics_init(
        "gf3d",                 //program name
        1600,                   //screen width
        900,                    //screen height
        vector4d(0.51,0.75,1,1),//background color
        0,                      //fullscreen
        1                       //validation
    );
	gf3d_entity_system_init(5);
    
    // main game loop
    slog("gf3d main loop begin");
	/*
    model = gf3d_model_load("agumon","nrt");
    model2 = gf3d_model_load("lumberJack","cube");
	*/
	ent = gf3d_entity_load("agumon", "nrt");
	ent2 = gf3d_entity_load("lumberJack", "cube");
	//ent3 = gf3d_entity_load("woodenbox", "lumber");
	
	sens = 0.04;
	mx = 0.0;
	my = 0.0;
	while (!done)
	{
		SDL_PumpEvents();   // update SDL's internal event structures
		keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
		SDL_PollEvent(&event);
		//update game things here

		//gf3d_vgraphics_rotate_cameraX(0.001);

		// configure render command for graphics command pool
		// for each mesh, get a command and configure it from the pool
		bufferFrame = gf3d_vgraphics_render_begin();
		commandBuffer = gf3d_command_rendering_begin(bufferFrame);
		gf3d_entity_update_all();
		gf3d_entity_draw_all(bufferFrame, commandBuffer, bufferFrame);
		gf3d_entity_think_all(keys);

		/*
		if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		gf3d_model_draw(model2, bufferFrame, commandBuffer);
		}
		else{
		gf3d_model_draw(model, bufferFrame, commandBuffer);
		}
		*/
		//gf3d_vgraphics_rotate_cameraX(SDL_GetRelativeMouseState)
		if (keys[SDL_SCANCODE_T]){
			gf3d_vgraphics_rotate_modelY(ent, zoomFov, zoomFov, zoomFov);
			//model->ubo.model[3][0] += 1.0;
			//model->position.x += 1.0;
			//model->ubo.model[3][0] = zoomFov;
			//model->ubo.model[3][1] = zoomFov;
			//model->ubo.model[3][2] = zoomFov;
			zoomFov += 0.01;
		}
		if (keys[SDL_SCANCODE_G]){
			gf3d_vgraphics_rotate_modelX(ent->ubo.model, zoomFov, zoomFov, zoomFov);
			//model->ubo.model[3][0] += 1.0;
			//model->position.x += 1.0;
			//model->ubo.model[3][0] = zoomFov;
			//model->ubo.model[3][1] = zoomFov;
			//model->ubo.model[3][2] = zoomFov;
			zoomFov -= 0.01;
		}
		if (spaceBar != 0.0){
			
			if (spaceBar < 0.60 && spaceDown == 0){
				spaceBar += 0.01;
			}
			if (spaceBar >= 0.60){
				spaceDown = 1;
			}
			if (spaceDown == 1){
				spaceBar -= 0.01;
				if (spaceBar <= 0.0){
					spaceDown = 0;
					spaceBar = 0.0;
				}
			}
			gf3d_vgraphics_rotate_modelX(ent->ubo.model, spaceBar, spaceBar, spaceBar);
		}
		if (keys[SDL_SCANCODE_SPACE] && spaceBar <= 0.0){
			spaceBar += 0.02;
		}
		//gf3d_model_draw(model, bufferFrame, commandBuffer);

		gf3d_command_rendering_end(commandBuffer);
		gf3d_vgraphics_render_end(bufferFrame);


		/*rotation stuff*/
		//mx = event.motion.xrel;
		//my = event.motion.yrel;
		if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
			if (event.motion.xrel > mx){
				gf3d_vgraphics_rotate_cameraX(sens);
				//mx = event.motion.xrel;
			}
			if (event.motion.xrel < mx){
				gf3d_vgraphics_rotate_cameraX(-1 * sens);
				//mx = event.motion.xrel;
			}
			if (event.motion.yrel > my){
				gf3d_vgraphics_rotate_cameraY(sens);
				//my = event.motion.y;
			}
			if (event.motion.yrel < my){
				gf3d_vgraphics_rotate_cameraY(-1 * sens);
				//my = event.motion.y;
			}
		}
		if (keys[SDL_SCANCODE_LEFTBRACKET])sens -= 0.005;
		if (keys[SDL_SCANCODE_RIGHTBRACKET])sens += 0.005;

		if (keys[SDL_SCANCODE_A])gf3d_vgraphics_rotate_cameraX(sens);
		if (keys[SDL_SCANCODE_D])gf3d_vgraphics_rotate_cameraX(-1 * sens);

		if (keys[SDL_SCANCODE_W])gf3d_vgraphics_rotate_cameraY(sens);
		if (keys[SDL_SCANCODE_S])gf3d_vgraphics_rotate_cameraY(-1 * sens);

		if (keys[SDL_SCANCODE_Q])gf3d_vgraphics_rotate_cameraZ(ent, 0.1);
		if (keys[SDL_SCANCODE_E])gf3d_vgraphics_rotate_cameraZ(ent, -0.1);
		
		if (event.type == SDL_MOUSEWHEEL){
			if (event.wheel.y < 0){
					zoomCamera++;
					zoom(zoomCamera);
				}
			if (event.wheel.y > 0){
					zoomCamera--;
					zoom(zoomCamera);
				}
			event.wheel.y = 0;
		}

		/*
		if (keys[SDL_SCANCODE_G]){
			gf3d_entity_free(model);
		}
		*/
		if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
    }    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
