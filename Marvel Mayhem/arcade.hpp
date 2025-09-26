#ifndef ARCADE_HPP
#define ARCADE_HPP

#include "character.hpp"
#include "ai.hpp"
#include "menu.hpp"

Character arcadeAI;

//Loads the AI opponent

void loadArcadeCharacters() {
	// Load the AI's character (currently always Captain America)
	loadCaptainAmerica(arcadeAI);
	// Set the AI's starting position
	arcadeAI.moveX = 980;
	arcadeAI.facingRight = false;
}

//Draws all arcade fighting scene.

void drawArcadeAI() {
	arcadeAI.draw();     // Draw the AI's character
}

//Updates the state of both the player and AI characters.

void updateArcadeCharacters() {
	arcadeAI.update();
}

#endif 