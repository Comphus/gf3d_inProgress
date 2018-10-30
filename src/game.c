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
	int zoomFov = 0;
	int mx, my;
    const Uint8 * keys;
    Uint32 bufferFrame = 0;
    VkCommandBuffer commandBuffer;
    Model *model;
    Model *model2;
    
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
	//model = gf3d_entity_load("agumon", "nrt");
	model2 = gf3d_entity_load("lumberJack", "cube");
	while (!done)
	{
		SDL_PumpEvents();   // update SDL's internal event structures
		keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
		//update game things here

		gf3d_vgraphics_rotate_cameraX(0.001);

		// configure render command for graphics command pool
		// for each mesh, get a command and configure it from the pool
		bufferFrame = gf3d_vgraphics_render_begin();
		commandBuffer = gf3d_command_rendering_begin(bufferFrame);
		//gf3d_entity_update_all(); not needed yet
		gf3d_entity_draw_all(bufferFrame, commandBuffer);

		/*
		if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
			gf3d_model_draw(model2, bufferFrame, commandBuffer);
		}
		else{
			gf3d_model_draw(model, bufferFrame, commandBuffer);
		}
		*/
		//gf3d_vgraphics_rotate_cameraX(SDL_GetRelativeMouseState)

		gf3d_command_rendering_end(commandBuffer);
		gf3d_vgraphics_render_end(bufferFrame);


		/*rotation stuff*/
		if (keys[SDL_SCANCODE_A])gf3d_vgraphics_rotate_cameraX(0.1);
		if (keys[SDL_SCANCODE_D])gf3d_vgraphics_rotate_cameraX(-0.1);

		if (keys[SDL_SCANCODE_W])gf3d_vgraphics_rotate_cameraY(0.1);
		if (keys[SDL_SCANCODE_S])gf3d_vgraphics_rotate_cameraY(-0.1);

		if (keys[SDL_SCANCODE_Q])gf3d_vgraphics_rotate_cameraZ(0.1);
		if (keys[SDL_SCANCODE_E])gf3d_vgraphics_rotate_cameraZ(-0.1);
		
		if (keys[SDL_SCANCODE_R]){
			zoomCamera++;
			zoom(zoomCamera);
		}
		if (keys[SDL_SCANCODE_F]){
			zoomCamera--;
			zoom(zoomCamera);
		}


		if (keys[SDL_SCANCODE_T]){
			zoomFov++;
			fovZoom(zoomFov);
		}
		if (keys[SDL_SCANCODE_G]){
			zoomFov--;
			fovZoom(zoomFov);
		}
		if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
    }    
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
