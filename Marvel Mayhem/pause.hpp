#ifndef PAUSE_HPP
#define PAUSE_HPP

#include "character.hpp" 
#include "menu.hpp"      
#define PAUSE_BUTTON_COUNT 3

enum GameState {
	PLAYING,
	PAUSED
};

// Global variable to track the current state
GameState currentGameState = PLAYING;

// Pause menu assets and state
int pauseMenuBackground;
int pauseButtonImage;
Button pauseButton;
Button pauseMenuButtons[PAUSE_BUTTON_COUNT];
int hoveredPauseButtonIndex = -1;

// Function Prototypes
void loadPauseAssets();
void showPauseMenu();
int handlePauseMenuClick(int mx, int my);
void handlePauseHoverAnimation(int mx, int my);
void resetCharacters(Character &p1, Character &p2);
void resetArcadePlayer(Character &player, Character &ai);


// Loads all necessary image assets for the pause menu and pause button.
// function called once during game initialization.


void loadPauseAssets() {
	pauseMenuBackground = iLoadImage("BG/pauseScreen3.png");
	pauseButtonImage = iLoadImage("UiElements/setting_Icon2.png");

	// Define the position and size of the main pause button in the arena
	pauseButton.x = SCREEN_WIDTH - 80;
	pauseButton.y = SCREEN_HEIGHT - 70;
	pauseButton.width = 50;
	pauseButton.height = 50;

	// Define positions and sizes for the buttons inside the pause menu popup
	int startY = 400;
	for (int i = 0; i < PAUSE_BUTTON_COUNT; i++) {
		pauseMenuButtons[i].width = 300;
		pauseMenuButtons[i].height = 80;
		pauseMenuButtons[i].x = (SCREEN_WIDTH - pauseMenuButtons[i].width) / 2;
		pauseMenuButtons[i].y = startY - i * 100;
	}
}


//the pause menu UI on the screen.
//This includes the semi-transparent background and the option buttons.

void showPauseMenu() {
	// Draw a semi-transparent overlay
	//iSetColor(0, 0, 0);
	//iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Show the pause menu background image (the popup)
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, pauseMenuBackground);

	// Set text color for buttons
	iSetColor(255, 255, 255);

	// Draw text for each button
	iText(pauseMenuButtons[0].x + 110, pauseMenuButtons[0].y + 30, "Resume", GLUT_BITMAP_HELVETICA_18);
	iText(pauseMenuButtons[1].x + 115, pauseMenuButtons[1].y + 30, "Restart", GLUT_BITMAP_HELVETICA_18);
	iText(pauseMenuButtons[2].x + 60, pauseMenuButtons[2].y + 30, "Character Selection", GLUT_BITMAP_HELVETICA_18);

	// Apply hover effect
	if (hoveredPauseButtonIndex != -1) {
		int i = hoveredPauseButtonIndex;
		iSetColor(255, 255, 0); // Yellow highlight for hover
		iRectangle(pauseMenuButtons[i].x, pauseMenuButtons[i].y, pauseMenuButtons[i].width, pauseMenuButtons[i].height);
	}
}


// Checks if a click occurred on any of the pause menu buttons.

int handlePauseMenuClick(int mx, int my) {
	for (int i = 0; i < PAUSE_BUTTON_COUNT; i++) {
		if (mx >= pauseMenuButtons[i].x && mx <= pauseMenuButtons[i].x + pauseMenuButtons[i].width &&
			my >= pauseMenuButtons[i].y && my <= pauseMenuButtons[i].y + pauseMenuButtons[i].height) {
			return i;
		}
	}
	return -1; // No button clicked
}


//Handles the hover effect for pause menu buttons.

void handlePauseHoverAnimation(int mx, int my) {
	hoveredPauseButtonIndex = -1; // Reset hover state
	for (int i = 0; i < PAUSE_BUTTON_COUNT; i++) {
		if (mx >= pauseMenuButtons[i].x && mx <= pauseMenuButtons[i].x + pauseMenuButtons[i].width &&
			my >= pauseMenuButtons[i].y && my <= pauseMenuButtons[i].y + pauseMenuButtons[i].height) {
			hoveredPauseButtonIndex = i;
			break;
		}
	}
}


//Resets the state and position of both characters.
//p1 Reference to the Player 1 character object.
//p2 Reference to the Player 2 character object.

void resetCharacters(Character &p1, Character &p2) {
	// Player 1 Reset
	p1.moveX = 200;
	p1.moveY = p1.baseY;
	p1.facingRight = true;
	p1.hp = 1000;
	p1.alive = true;
	p1.jumpInProgress = false;
	p1.setState(IDLE);

	// Player 2 Reset
	p2.moveX = 980;
	p2.moveY = p2.baseY;
	p2.facingRight = false;
	p2.hp = 1000;
	p2.alive = true;
	p2.jumpInProgress = false;
	p2.setState(IDLE);
}


//Resets the state and position of the arcade player and AI.
//player Reference to the arcade player character object.
//ai Reference to the AI character object.

void resetArcadePlayer(Character &player, Character &ai) {
	player.moveX = 200;
	player.moveY = player.baseY;
	player.facingRight = true;
	player.hp = 1000;
	player.alive = true;
	player.jumpInProgress = false;
	player.setState(IDLE);

	ai.moveX = 980;
	ai.moveY = ai.baseY;
	ai.facingRight = false;
	ai.hp = 1000;
	ai.alive = true;
	ai.jumpInProgress = false;
	ai.setState(IDLE);
}


#endif 