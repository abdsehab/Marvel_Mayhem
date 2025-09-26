#ifndef AI_HPP
#define AI_HPP

#include "character.hpp"

// Handles the AI logic for movement and attacking
void handleAI(Character &ai, Character &player) {

	//AI Decision Making

	// If the player is jumping and the AI is on the ground, make the AI jump too.
	if (player.jumpInProgress && !ai.jumpInProgress) {
		ai.setState(JUMP);
		ai.jumpInProgress = true;
		ai.currentFrame = 0;
	}

	// If the AI is currently jumping, let the main game loop handle the physics.
	// Don't try to walk or attack while in the air.
	if (ai.jumpInProgress) {
		return;
	}

	// Calculate the distance between the AI and the player on the X-axis
	int distance = player.moveX - ai.moveX;

	// If the player is far to the right, move towards them
	if (distance > 50) {
		ai.facingRight = true;
		ai.moveX += 3; // Move right
		ai.setState(WALK);
	}
	// If the player is far to the left, move towards them
	else if (distance < -50) {
		ai.facingRight = false;
		ai.moveX -= 3; // Move left
		ai.setState(WALK);
	}
	// If the player is close enough, attack
	else {
		ai.setState(ATTACK);
	}

	// State Management

	// If the AI is in the ATTACK state, check if the animation is finished
	if (ai.characterState == ATTACK) {
		// Determine the maximum number of frames for the attack animation
		int maxAtk = ai.facingRight ? ai.attackCount_R : ai.attackCount_L;
		// If the animation is on its last frame, switch back to IDLE
		if (ai.currentFrame >= maxAtk - 1) {
			ai.setState(IDLE);
		}
	}
}

#endif 
