#ifndef __GF3D_ENTITY_H__
#define __GF3D_ENTITY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gf3d_vector.h"
#include "gf3d_text.h"
#include "gf3d_matrix.h"
#include "gf3d_model.h"
#include "simple_logger.h"

typedef enum
{
	entityIdle,
	entityFlinch,
	entityStagger,
	entityStun,
	entityKnockBack, 
	entityAirborne,
	entityGrounded,
	entityGroundedRoll,
	entityWallBounce,
	entityDead
}EntityState;


typedef struct Entity_S
{
	Uint8 inuse;
	Uint32 id;

	TextLine Name;
	EntityState state;

	/*physics*/
	Vector3D position;
	Vector3D rotation;
	Vector3D velocity;
	Vector3D acceleration;

	/*model and mesh*/
	Uint32 bufferFrame;
	VkCommandBuffer commandBuffer;
	UniformBufferObject ubo;
	Model *model;

	void(*draw)(struct Entitys_S *self);
	void(*think)(struct Entity_S *self);
	void(*update)(struct Entity_S *self);
	void(*die)(struct Entity_S *self);
	void(*free)(struct Entity_S *self);


	int health;
	int stamina;
	int attacking;


}Entity;


/**
* @brief initialize the entity system
* @param maxEntities max amount of active entities
*/
void gf3d_entity_system_init(Uint32 maxEntities);

/**
* @brief get a new entity from the system
* @returns NULL when no more entities, or a new valid entity ready to be given data
*/
Entity *gf3d_entity_new();

void *gf3d_entity_load(char *model, char *texture);

void gf3d_entity_free(Entity *self);


void gf3d_entity_free_all();


void gf3d_entity_draw(Entity *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer);


void gf3d_entity_draw_all(Uint32 bufferFrame, VkCommandBuffer commandBuffer);


void gf3d_entity_think_all();


void gf3d_entity_update_all();



#endif
