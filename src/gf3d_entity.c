#include <stdlib.h>
#include <string.h>
#include "gf3d_entity.h"
#include "gf3d_model.h"
#include "gf3d_matrix.h"
#include "gf3d_vgraphics.h"
#include "SDL.h"
#include "simple_logger.h"

typedef struct
{
	Uint32 maxEntities;
	Entity *entityList;
	Uint64 autoincrement;
}EntityManager;

static EntityManager entity_manager = { 0 };

Uint8 * e_keys;
float testStuff;
testStuff = 0;

void gf3d_entity_system_close()
{
	int i = 0;
	if (entity_manager.entityList != NULL)
	{
		for (i = 0; i < entity_manager.maxEntities; i++)
		{
			gf3d_entity_free(&entity_manager.entityList[i]);
		}
		free(entity_manager.entityList);
	}
	memset(&entity_manager, 0, sizeof(EntityManager));
	slog("entity system closed");
}

void gf3d_entity_system_init(Uint32 maxEntities)
{
	if (!maxEntities)
	{
		slog("cannot initialize entity system for zero entities");
		return;
	}
	memset(&entity_manager, 0, sizeof(EntityManager));

	entity_manager.entityList = (Entity*)malloc(sizeof(Entity)*maxEntities);
	if (!entity_manager.entityList)
	{
		slog("failed to allocate entity list");
		gf3d_entity_system_close();
		return;
	}

	memset(entity_manager.entityList, 0, sizeof(Entity)*maxEntities);
	entity_manager.maxEntities = maxEntities;
	atexit(gf3d_entity_system_close);
	slog("entity system initialized");
}

void gf3d_entity_free(Entity *self)
{
	if (!self)return;
	if (self->free)self->free(self);
	/*
	for (i = 0; i < EntitySoundMax; i++)
	{
		gf2d_sound_free(self->sound[i]);
	}
	gf2d_actor_free(&self->actor);
	gf2d_particle_emitter_free(self->pe);
	*/
	memset(self, 0, sizeof(Entity));
}

void gf3d_entity_free_all()
{
	int i;
	for (i = 0; i < entity_manager.maxEntities; i++)
	{
		gf3d_entity_free(&entity_manager.entityList[i]);
	}
}

Entity *gf3d_entity_new()
{
	int i;
	for (i = 0; i < entity_manager.maxEntities; i++)
	{
		if (entity_manager.entityList[i].inuse == 0)
		{
			memset(&entity_manager.entityList[i], 0, sizeof(Entity));
			entity_manager.entityList[i].id = entity_manager.autoincrement++;
			entity_manager.entityList[i].inuse = 1;
			//vector3d_set(entity_manager.entityList[i].scale, 1, 1);
			//entity_manager.entityList[i].actor.color = vector4d(1, 1, 1, 1);// no color shift, opaque
			return &entity_manager.entityList[i];
		}
	}
	return NULL;
}

Entity *gf3d_entity_load(char * model, char * texture)
{
	Entity *ent;
	ent = gf3d_entity_new();
	if (!ent)return;

	ent->model = gf3d_model_load(model, texture);
	//essentially gf3d_vgraphics_init code
	
	gf3d_matrix_identity(ent->ubo.model);
	
	
	//gf3d_model_manager_init(1024, gf3d_swapchain_get_swap_image_count(), device);
	
	gf3d_matrix_identity(ent->ubo.view);
	gf3d_matrix_identity(ent->ubo.proj);
	gf3d_matrix_view(
		ent->ubo.view,
		vector3d(2, 20, 2),
		vector3d(0, 0, 0),
		vector3d(0, 0, 1)
	);
	gf3d_matrix_perspective(
		ent->ubo.proj,
		45 * GF3D_DEGTORAD,
		1600 / (float)900,
		0.1f,
		100
	);
	ent->ubo.proj[1][1] *= -1;
	gf3d_mesh_init(1024);//TODO: pull this from a parameter
	gf3d_texture_init(1024);
	
	//need to be able to increase and decrease size of model
	ent->scale = 1.0;

	return ent;
}

/*update and update all are copied to see how moving the model would work*/
void gf3d_entity_update(Entity *self)
{
	if (!self)return;
	if (!self->inuse)return;

	self->ubo.model[0][0] *= self->scale;
	self->ubo.model[1][1] *= self->scale;
	self->ubo.model[2][2] *= self->scale;
	self->scale = 1;

	//model position
	self->ubo.model[3][0] = self->position.x;
	self->ubo.model[3][1] = self->position.y;
	self->ubo.model[3][2] = self->position.z;

	//model rotations
	gf3d_matrix_rotate(self->ubo.model, self->ubo.model, self->currentRotation.x, vector3d(1, 0, 0));
	gf3d_matrix_rotate(self->ubo.model, self->ubo.model, self->currentRotation.y, vector3d(0, 1, 0));
	gf3d_matrix_rotate(self->ubo.model, self->ubo.model, self->currentRotation.z, vector3d(0, 0, 1));

	memset(&self->currentRotation, 0, sizeof(Vector3D));

	if (self->update)
	{
		self->update(self);
	}
}

void gf3d_entity_update_all()
{
	int i;
	for (i = 0; i < entity_manager.maxEntities; i++)
	{
		if (entity_manager.entityList[i].inuse == 1)
		{
			gf3d_entity_update(&entity_manager.entityList[i]);
		}
	}
}

void gf3d_entity_draw(Entity *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer, Uint32 imageIndex)
{
	if (!self)return;
	/*
	//taken from matrix.c
	//model scale
	self->ubo.model[0][0] *= self->scale;
	self->ubo.model[1][1] *= self->scale;
	self->ubo.model[2][2] *= self->scale;
	self->scale = 1;

	//model position
	self->ubo.model[3][0] = testStuff;
	self->ubo.model[3][1] = self->position.y;
	self->ubo.model[3][2] = self->position.z;

	//model rotations
	gf3d_matrix_rotate(self->ubo.model, self->ubo.model, testStuff, vector3d(1, 0, 0));
	gf3d_matrix_rotate(self->ubo.model, self->ubo.model, self->currentRotation.y, vector3d(0, 1, 0));
	gf3d_matrix_rotate(self->ubo.model, self->ubo.model, self->currentRotation.z, vector3d(0, 0, 1));

	memset(&self->currentRotation, 0, sizeof(Vector3D));
	/*having trouble manipulating model positions and such
	
	gf3d_vgraphics_update_uniform_buffer(imageIndex);
	*/
	//create model
	gf3d_model_draw(self->model, bufferFrame, commandBuffer);
	//testStuff += 50.0;
}

void gf3d_entity_draw_all(Uint32 bufferFrame, VkCommandBuffer commandBuffer, Uint32 imageIndex)
{
	int i;
	for (i = 0; i < entity_manager.maxEntities; i++)
	{
		if (entity_manager.entityList[i].inuse == 0) continue;
			if (!entity_manager.entityList[i].model) continue;
				gf3d_entity_draw(&entity_manager.entityList[i], bufferFrame, commandBuffer, imageIndex);
	}

	//entity_render_end_all();
}

void gf3d_entity_think(Entity *self, Uint8 * key){
	
	
	if (!self)return;
	
	SDL_PumpEvents();   // update SDL's internal event structures
	key = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

	if (e_keys[SDL_SCANCODE_Z]){
		gf3d_matrix_rotate(
			self->ubo.model,
			self->ubo.model,
			testStuff,
			vector3d(0, 0, 1));
	}
	if (e_keys[SDL_SCANCODE_D])gf3d_vgraphics_rotate_cameraX(-0.1);

	if (e_keys[SDL_SCANCODE_W])gf3d_vgraphics_rotate_cameraY(0.1);
	if (e_keys[SDL_SCANCODE_S])gf3d_vgraphics_rotate_cameraY(-0.1);

	//if (e_keys[SDL_SCANCODE_Q])gf3d_vgraphics_rotate_cameraZ(ent, 0.1);
	//if (e_keys[SDL_SCANCODE_E])gf3d_vgraphics_rotate_cameraZ(ent, -0.1);

	if (self->think)
	{
		self->think(self);
	}

	testStuff += 0.03;
}

void gf3d_entity_think_all(Uint8 * key)
{
	int i;
	e_keys = key;
	for (i = 0; i < entity_manager.maxEntities; i++)
	{
		if (entity_manager.entityList[i].inuse == 0)continue;
		if (entity_manager.entityList[i].think != NULL)
		{
			gf3d_entity_think(&entity_manager.entityList[i], e_keys);
			entity_manager.entityList[i].think(&entity_manager.entityList[i]);
			
		}
	}
}