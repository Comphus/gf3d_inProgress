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
	const Uint8 keys;
	Uint32 id;

	TextLine Name;
	EntityState state;
	float scale;

	/*physics*/
	Vector3D position;
	Vector3D rotation;
	Vector3D currentRotation;
	Vector3D velocity;
	Vector3D acceleration;
	Vector3D axis;

	/*model and mesh*/
	Uint32 bufferFrame;
	VkCommandBuffer commandBuffer;
	Model *model;
	UniformBufferObject ubo;

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

/**
* @brief load entity data into entity system
* @returns entity object
* @param model the model being loaded in
* @param texture the texture being loaded in
*/
Entity *gf3d_entity_load(char *model, char *texture);

/**
* @brief frees an entity
*/
void gf3d_entity_free(Entity *self);

/**
* @brief frees all entities in the entity system
*/
void gf3d_entity_free_all();

/**
* @brief draws an entity to be displayed on screen
* @param self the entity being passed
* @param bufferFrame the buffer frame
* @param commandBuffer the command buffer
*/
void gf3d_entity_draw(Entity *self, Uint32 bufferFrame, VkCommandBuffer commandBuffer);

/**
* @brief draws all entities in the entity system system
*/
void gf3d_entity_draw_all(Uint32 bufferFrame, VkCommandBuffer commandBuffer);



void gf3d_entity_think(Entity *self);
/**
* @brief not implemented yet, takes in input for the entity to respond to
*/
void gf3d_entity_think_all(Uint8 key);

/**
* @brief updates the positions and axes of all entities
*/
void gf3d_entity_update_all();



#endif
