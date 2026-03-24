#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "gfc_vector.h"
#include "gfc_shape.h"

/**
*@brief get the Cameras posititon in world 
* @return the postion  
*/

GFC_Vector2D camera_get_postition();

/**
*@brief get the Cameras size
* @return the size
*/

void camera_set_size(GFC_Vector2D size);

/**
* @brief get the offset to draw things relative to the camera
* @return the offset
*/

GFC_Vector2D camera_get_offset();

/**
* @brief set the camrea postions in the world space
*/

void camera_set_postition(GFC_Vector2D postition);

/**
* @brief snap the camera into world bounds
*/

void camera_apply_bounds();


void camera_set_bounds(GFC_Rect bounds);

void camera_enable_binding(Bool bindCamera);

void camera_center_on(GFC_Vector2D target);


#endif
