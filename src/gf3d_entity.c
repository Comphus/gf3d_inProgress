#include <stdlib.h>
#include <string.h>
#include "gf3d_entity.h"
#include "gf3d_model.h"
#include "simple_logger.h"

typedef struct
{
	Uint32 maxEntities;
	Entity *entityList;
	Uint64 autoincrement;
}EntityManager;

static EntityManager entity_manager = { 0 };

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
	//int i;     -- will re implement this after common deliverables are due
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

void *gf3d_entity_load(char * model, char * texture)
{
	Entity *ent;
	ent = gf3d_entity_new();
	if (!ent)return;

	ent->model = gf3d_model_load(model, texture);
	return ent;
}

/*update and update all are copied to see how moving the model would work*/
void gf3d_entity_update(Entity *self)
{
	if (!self)return;
	if (!self->inuse)return;

	gf3d_matrix_identity(self->ubo.model);
	gf3d_matrix_make_translation(self->ubo.model, self->position);

	gf3d_matrix_rotate(self->ubo.model, self->ubo.model, 0.05f, self->rotation);

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

void gf3d_entity_draw(Entity *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer)
{
	if (!self)return;

	gf3d_model_draw(self->model, bufferFrame, commandBuffer);
}

void gf3d_entity_draw_all(Uint32 bufferFrame, VkCommandBuffer commandBuffer)
{
	int i;

	for (i = 0; i < entity_manager.maxEntities; i++)
	{
		if (entity_manager.entityList[i].inuse == 0) continue;
			if (!entity_manager.entityList[i].model) continue;
				gf3d_entity_draw(&entity_manager.entityList[i], bufferFrame, commandBuffer);
	}

	//entity_render_end_all();
}