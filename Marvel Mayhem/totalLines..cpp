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
#ifndef ARCADE_ACTION_HPP
#define ARCADE_ACTION_HPP

#include "character.hpp"
#include "menu.hpp"
#include <stdio.h>

extern char playerName[100];
extern bool playerWonArcade;

//Health Bar
void drawHealthBars(const Character& player, const Character& ai) {
	const int barWidth = 400;
	const int barHeight = 30;
	const int yPos = SCREEN_HEIGHT - 60;
	const int textYPos = yPos + barHeight + 10;
	char hpText[10];

	//Player Health Bar(Left side)
	const int playerBarX = 40;
	iText(playerBarX, textYPos, playerName, GLUT_BITMAP_HELVETICA_18);

	//red background of the health bar
	iSetColor(200, 0, 0);
	iFilledRectangle(playerBarX, yPos, barWidth, barHeight);

	// Calculate the width of the green health portion
	//double playerHealthPercent = 1;
	double playerHealthPercent = player.hp / 1000.0;
	if (playerHealthPercent < 0) playerHealthPercent = 0;

	// Draw the green health portion
	iSetColor(0, 200, 0);
	iFilledRectangle(playerBarX, yPos, barWidth * playerHealthPercent, barHeight);

	// white border
	iSetColor(255, 255, 255);
	iRectangle(playerBarX, yPos, barWidth, barHeight);


	//Health Percentage 
	int playerHpInt = playerHealthPercent * 100;
	sprintf_s(hpText, "%d%%", playerHpInt);
	iSetColor(255, 255, 255);
	iText(playerBarX + 180, yPos + 10, hpText, GLUT_BITMAP_HELVETICA_18);


	//AI Health Bar(Right side)
	const int aiBarX = SCREEN_WIDTH - barWidth - 100;
	iText(aiBarX, textYPos, "AI", GLUT_BITMAP_HELVETICA_18);


	//red background
	iSetColor(200, 0, 0);
	iFilledRectangle(aiBarX, yPos, barWidth, barHeight);

	// Calculate the width of the green health portion

	//double aiHealthPercent = 1;
	double aiHealthPercent = ai.hp / 1000.0;
	if (aiHealthPercent < 0) aiHealthPercent = 0;

	// Draw the green health portion
	iSetColor(0, 200, 0);
	iFilledRectangle(aiBarX, yPos, barWidth * aiHealthPercent, barHeight);

	//white border
	iSetColor(255, 255, 255);
	iRectangle(aiBarX, yPos, barWidth, barHeight);

	// Health Percentage
	int aiHpInt = aiHealthPercent * 100;
	sprintf_s(hpText, "%d%%", aiHpInt);
	iSetColor(255, 255, 255);
	iText(aiBarX + 180, yPos + 10, hpText, GLUT_BITMAP_HELVETICA_18);

	// Player Energy Bar

	const int energyBarWidth = 400;
	const int energyBarHeight = 12;
	const int energyBarY = yPos - 30; // Position below the health bar

	// Background
	iSetColor(60, 60, 60);
	iFilledRectangle(playerBarX, energyBarY, energyBarWidth, energyBarHeight);

	// Energy fill based on player.energy
	int energyWidth = (int)(energyBarWidth * (player.energy / 100.0));
	iSetColor(0, 120, 255); // Blue energy color
	if (energyWidth > 0) {
		iFilledRectangle(playerBarX, energyBarY, energyWidth, energyBarHeight);
	}

	// Outline
	iSetColor(200, 200, 200);
	iRectangle(playerBarX, energyBarY, energyBarWidth, energyBarHeight);

}

void addPlayerEnergy(Character &player) {
	if (!player.alive) {
		return;
	}

	player.energy += 25;
	if (player.energy >= 100) {
		player.energy = 100;
		player.canUltimate = true;
	}
}

//Collision and Damage
void checkCollisionAndApplyDamage(Character &attacker, Character &defender) {
	// check korbe beche ache naki 
	if (!attacker.alive || !defender.alive || attacker.characterState != ATTACK) {
		return;
	}

	int hitFrame = attacker.attackCount_R / 2;
	if (attacker.currentFrame != hitFrame) {
		return;
	}

	// distance-based collision check
	bool isColliding = false;
	int attackReach = 110;
	int distance = abs(attacker.moveX - defender.moveX);

	if (distance < attackReach) {

		if (attacker.facingRight && defender.moveX > attacker.moveX) {
			isColliding = true;
			
		}
		else if (!attacker.facingRight && defender.moveX < attacker.moveX) {
			isColliding = true;
			
		}
	}



	//damage
	if (isColliding) {
		int damage = 8 + (rand() % 3);
		defender.hp -= damage;

		// health does not fall below zero
		if (defender.hp < 0) {
			defender.hp = 0;
		}
	}



}


// Game Over
void checkGameOver(Character &player, Character &ai) {
	if (currentScreen == 40) return; 

	if ((player.hp <= 0 || ai.hp <= 0) && player.alive && ai.alive) {
		player.alive = false;
		ai.alive = false;

		if (player.hp <= 0) {
			playerWonArcade = false; // Player lost
		}
		else {
			playerWonArcade = true; // Player won
		}
		currentScreen = 40; // end screen
		isArcadeTimerRunning = false;

		//file

		FILE* scoreFile;
		fopen_s(&scoreFile, "arcade_scores.txt", "a");
		if (scoreFile != NULL) {
			fprintf(scoreFile, "%d\n", arcadeTimerSeconds);
			fclose(scoreFile);
		}
	}
   //show dead sprite

	if (player.hp <= 0) {
		player.setState(DEAD);
	}
	if (ai.hp <= 0) {
		ai.setState(DEAD);
	}
}

#endif
#ifndef ARCADE_END_SCREEN_HPP
#define ARCADE_END_SCREEN_HPP

#include "menu.hpp"
#include "pause.hpp"


#define END_SCREEN_BUTTON_COUNT 3

//variables for the end screen assets
int winScreenBackground;
int loseScreenBackground;
int endScreenButtonImage; 
Button endScreenButtons[END_SCREEN_BUTTON_COUNT];
int hoveredEndScreenButtonIndex = -1;

// Function
void loadArcadeEndScreenAssets();
void showArcadeEndScreen(bool playerWon);
int handleArcadeEndScreenClick(int mx, int my);
void handleArcadeEndScreenHover(int mx, int my);

// Loads image for the arcade mode end screen.
void loadArcadeEndScreenAssets() {
	winScreenBackground = iLoadImage("BG/win.jpg"); 
	loseScreenBackground = iLoadImage("BG/lost.jpg"); 
	endScreenButtonImage = iLoadImage("Images/buttons.png"); 

	//  buttons on the end screen
	int startY = 300;
	for (int i = 0; i < END_SCREEN_BUTTON_COUNT; i++) {
		endScreenButtons[i].width = 300;
		endScreenButtons[i].height = 80;
		endScreenButtons[i].x = (SCREEN_WIDTH - endScreenButtons[i].width) / 2;
		endScreenButtons[i].y = startY - i * 90;
	}
}

// win or lose screen UI.
void showArcadeEndScreen(bool playerWon) {
	if (playerWon) {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, winScreenBackground);
		//iSetColor(0, 255, 0); 
		//iText((SCREEN_WIDTH / 2) - 100, SCREEN_HEIGHT - 150, "VICTORY!", GLUT_BITMAP_TIMES_ROMAN_24);
	}
	else {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, loseScreenBackground);
		//iSetColor(255, 0, 0); 
		//iText((SCREEN_WIDTH / 2) - 100, SCREEN_HEIGHT - 150, "DEFEATED!", GLUT_BITMAP_TIMES_ROMAN_24);
	}

	
	char buttonLabels[END_SCREEN_BUTTON_COUNT][20] = { "Play Again", "Main Menu" , "Show Score"};
	const int labelXOffsets[] = { 115, 100 ,85};

	//  buttons
	for (int i = 0; i < END_SCREEN_BUTTON_COUNT; i++) {
		if (i == hoveredEndScreenButtonIndex) {
			continue; 
		}
		// Button Image
		iShowImage(endScreenButtons[i].x, endScreenButtons[i].y, endScreenButtons[i].width, endScreenButtons[i].height, endScreenButtonImage);
		// Button Text
		iSetColor(255, 255, 255);
		iText(endScreenButtons[i].x + labelXOffsets[i], endScreenButtons[i].y + 30, buttonLabels[i], GLUT_BITMAP_HELVETICA_18);
	}

	// hover zoom effect
	if (hoveredEndScreenButtonIndex != -1) {
		int i = hoveredEndScreenButtonIndex;
		double zoomFactor = 1.1; 

		int newWidth = endScreenButtons[i].width * zoomFactor;
		int newHeight = endScreenButtons[i].height * zoomFactor;
		int newX = endScreenButtons[i].x - (newWidth - endScreenButtons[i].width) / 2;
		int newY = endScreenButtons[i].y - (newHeight - endScreenButtons[i].height) / 2;

		iShowImage(newX, newY, newWidth, newHeight, endScreenButtonImage);
		//iSetColor(255, 255, 0); // zoom howar por color
		iText(endScreenButtons[i].x + labelXOffsets[i], endScreenButtons[i].y + 30, buttonLabels[i], GLUT_BITMAP_HELVETICA_18);
	}

	// loaded time on the screen
	
}

// Checks click on any of the end screen buttons.
int handleArcadeEndScreenClick(int mx, int my) {
	for (int i = 0; i < END_SCREEN_BUTTON_COUNT; i++) {
		if (mx >= endScreenButtons[i].x && mx <= endScreenButtons[i].x + endScreenButtons[i].width &&
			my >= endScreenButtons[i].y && my <= endScreenButtons[i].y + endScreenButtons[i].height) {
			return i;
		}
		
	}
	return -1; // no clicked
}

// Handles the hover effect for end screen buttons.
void handleArcadeEndScreenHover(int mx, int my) {
	hoveredEndScreenButtonIndex = -1; // Reset hover state
	for (int i = 0; i < END_SCREEN_BUTTON_COUNT; i++) {
		if (mx >= endScreenButtons[i].x && mx <= endScreenButtons[i].x + endScreenButtons[i].width &&
			my >= endScreenButtons[i].y && my <= endScreenButtons[i].y + endScreenButtons[i].height) {
			hoveredEndScreenButtonIndex = i;
			break;
		}
	}
}

#endif 


#ifndef ARCADE_END_SCREEN_HPP
#define ARCADE_END_SCREEN_HPP

#include "menu.hpp"
#include "pause.hpp"


#define END_SCREEN_BUTTON_COUNT 3

//variables for the end screen assets
int winScreenBackground;
int loseScreenBackground;
int endScreenButtonImage; 
Button endScreenButtons[END_SCREEN_BUTTON_COUNT];
int hoveredEndScreenButtonIndex = -1;

// Function
void loadArcadeEndScreenAssets();
void showArcadeEndScreen(bool playerWon);
int handleArcadeEndScreenClick(int mx, int my);
void handleArcadeEndScreenHover(int mx, int my);

// Loads image for the arcade mode end screen.
void loadArcadeEndScreenAssets() {
	winScreenBackground = iLoadImage("BG/win.jpg"); 
	loseScreenBackground = iLoadImage("BG/lost.jpg"); 
	endScreenButtonImage = iLoadImage("Images/buttons.png"); 

	//  buttons on the end screen
	int startY = 300;
	for (int i = 0; i < END_SCREEN_BUTTON_COUNT; i++) {
		endScreenButtons[i].width = 300;
		endScreenButtons[i].height = 80;
		endScreenButtons[i].x = (SCREEN_WIDTH - endScreenButtons[i].width) / 2;
		endScreenButtons[i].y = startY - i * 90;
	}
}

// win or lose screen UI.
void showArcadeEndScreen(bool playerWon) {
	if (playerWon) {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, winScreenBackground);
		//iSetColor(0, 255, 0); 
		//iText((SCREEN_WIDTH / 2) - 100, SCREEN_HEIGHT - 150, "VICTORY!", GLUT_BITMAP_TIMES_ROMAN_24);
	}
	else {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, loseScreenBackground);
		//iSetColor(255, 0, 0); 
		//iText((SCREEN_WIDTH / 2) - 100, SCREEN_HEIGHT - 150, "DEFEATED!", GLUT_BITMAP_TIMES_ROMAN_24);
	}

	
	char buttonLabels[END_SCREEN_BUTTON_COUNT][20] = { "Play Again", "Main Menu" , "Show Score"};
	const int labelXOffsets[] = { 115, 100 ,85};

	//  buttons
	for (int i = 0; i < END_SCREEN_BUTTON_COUNT; i++) {
		if (i == hoveredEndScreenButtonIndex) {
			continue; 
		}
		// Button Image
		iShowImage(endScreenButtons[i].x, endScreenButtons[i].y, endScreenButtons[i].width, endScreenButtons[i].height, endScreenButtonImage);
		// Button Text
		iSetColor(255, 255, 255);
		iText(endScreenButtons[i].x + labelXOffsets[i], endScreenButtons[i].y + 30, buttonLabels[i], GLUT_BITMAP_HELVETICA_18);
	}

	// hover zoom effect
	if (hoveredEndScreenButtonIndex != -1) {
		int i = hoveredEndScreenButtonIndex;
		double zoomFactor = 1.1; 

		int newWidth = endScreenButtons[i].width * zoomFactor;
		int newHeight = endScreenButtons[i].height * zoomFactor;
		int newX = endScreenButtons[i].x - (newWidth - endScreenButtons[i].width) / 2;
		int newY = endScreenButtons[i].y - (newHeight - endScreenButtons[i].height) / 2;

		iShowImage(newX, newY, newWidth, newHeight, endScreenButtonImage);
		//iSetColor(255, 255, 0); // zoom howar por color
		iText(endScreenButtons[i].x + labelXOffsets[i], endScreenButtons[i].y + 30, buttonLabels[i], GLUT_BITMAP_HELVETICA_18);
	}

	// loaded time on the screen
	
}

// Checks click on any of the end screen buttons.
int handleArcadeEndScreenClick(int mx, int my) {
	for (int i = 0; i < END_SCREEN_BUTTON_COUNT; i++) {
		if (mx >= endScreenButtons[i].x && mx <= endScreenButtons[i].x + endScreenButtons[i].width &&
			my >= endScreenButtons[i].y && my <= endScreenButtons[i].y + endScreenButtons[i].height) {
			return i;
		}
		
	}
	return -1; // no clicked
}

// Handles the hover effect for end screen buttons.
void handleArcadeEndScreenHover(int mx, int my) {
	hoveredEndScreenButtonIndex = -1; // Reset hover state
	for (int i = 0; i < END_SCREEN_BUTTON_COUNT; i++) {
		if (mx >= endScreenButtons[i].x && mx <= endScreenButtons[i].x + endScreenButtons[i].width &&
			my >= endScreenButtons[i].y && my <= endScreenButtons[i].y + endScreenButtons[i].height) {
			hoveredEndScreenButtonIndex = i;
			break;
		}
	}
}

#endif 
//#include "iGraphics.h"
#ifndef CHARACTER_HPP
#define CHARACTER_HPP
#include "menu.hpp"
#include <cstdlib> // For rand()
#include <ctime>   // For srand()
#include <iostream>
#include <string>

using namespace std;

const int MAX_FRAMES = 30;

enum AnimationState {
	IDLE,
	WALK,
	JUMP,
	ATTACK,
	SKILL,
	ULTIMATE,
	DEAD
};

struct Character{
	
	AnimationState characterState;
	string name;
	double hp;
	int energy;
	bool canUltimate;
	bool alive;
	int moveX;
	int moveY;
	int baseY; // fixed ground Y
	
	// Animation data(hold Image ID) 
	int idleSprites_R[MAX_FRAMES], idleSprites_L[MAX_FRAMES];
	int walkSprites_R[MAX_FRAMES], walkSprites_L[MAX_FRAMES];
	int jumpSprites_R[MAX_FRAMES], jumpSprites_L[MAX_FRAMES];
	int attackSprites_R[MAX_FRAMES], attackSprites_L[MAX_FRAMES];
	int skillSprites_R[MAX_FRAMES], skillSprites_L[MAX_FRAMES];
	int ultimateSprites_R[MAX_FRAMES], ultimateSprites_L[MAX_FRAMES];
	int deadSprites_R[MAX_FRAMES], deadSprites_L[MAX_FRAMES];              

	// Frame counts
	int idleCount_R, idleCount_L;
	int walkCount_R, walkCount_L;
	int attackCount_R, attackCount_L;
	int skillCount_R, skillCount_L;
	int ultimateCount_R, ultimateCount_L;
	int deadCount_R, deadCount_L;
	int jumpCount_R, jumpCount_L;

	// Current animation frame
	int currentFrame;

	//direction of facing
	bool facingRight;
	
	// Constructor
	Character()
		: name("Captain America"),
		characterState(IDLE),
		hp(1000),
		alive(true),
		moveX(100),
		moveY(113),
		baseY(113),
		facingRight(true),
		currentFrame(0),
		idleCount_R(0), idleCount_L(0),
		walkCount_R(0), walkCount_L(0),
		jumpCount_R(0), jumpCount_L(0),
		attackCount_R(0), attackCount_L(0),
		skillCount_R(0), skillCount_L(0),
		ultimateCount_R(0), ultimateCount_L(0),
		deadCount_R(0), deadCount_L(0)
	{}  // initializer list has been used so the body stays empty

	//initiazile maxFrame for each AnimationState update frames
	//initiazile maxFrame for each AnimationState update frames
	void update() {
		int maxFrame = 1;
		switch (characterState) {
		case IDLE:     maxFrame = facingRight ? idleCount_R : idleCount_L; break;
		case WALK:     maxFrame = facingRight ? walkCount_R : walkCount_L; break;
		case JUMP:     maxFrame = facingRight ? jumpCount_R : jumpCount_L; break;
		case ATTACK:   maxFrame = facingRight ? attackCount_R : attackCount_L; break;
		case SKILL:    maxFrame = facingRight ? skillCount_R : skillCount_L; break;
		case ULTIMATE: maxFrame = facingRight ? ultimateCount_R : ultimateCount_L; break;
		case DEAD:
			maxFrame = facingRight ? deadCount_R : deadCount_L;
			// For death animation, increment frame but don't loop
			if (maxFrame > 0) {
				if (currentFrame < maxFrame - 1) {
					currentFrame++; // Advance to next frame
				}
				// Stay on last frame once reached
			}
			return; // Exit early for DEAD state
		}

		// Updates the current frame for non-DEAD states
		if (maxFrame > 0) currentFrame = (currentFrame + 1) % maxFrame;
		else currentFrame = 0;
	}
	
	// a pointer is declared and initialized as a null pointer 
	// this pointer points to each array address in case of each AnimationState
	void draw() {
		int* spriteArray = nullptr;
		int count = 0;

		switch (characterState) {
		case IDLE:
			spriteArray = facingRight ? idleSprites_R : idleSprites_L;
			count = facingRight ? idleCount_R : idleCount_L;
			break;
		case WALK:
			spriteArray = facingRight ? walkSprites_R : walkSprites_L;
			count = facingRight ? walkCount_R : walkCount_L;
			break;
		case JUMP:
			spriteArray = facingRight ? jumpSprites_R : jumpSprites_L;
			count = facingRight ? jumpCount_R : jumpCount_L;
			break;
		case ATTACK:
			spriteArray = facingRight ? attackSprites_R : attackSprites_L;
			count = facingRight ? attackCount_R : attackCount_L;
			break;
		case SKILL:
			spriteArray = facingRight ? skillSprites_R : skillSprites_L;
			count = facingRight ? skillCount_R : skillCount_L;
			break;
		case ULTIMATE:
			spriteArray = facingRight ? ultimateSprites_R : ultimateSprites_L;
			count = facingRight ? ultimateCount_R : ultimateCount_L;
			break;
		case DEAD:
			spriteArray = facingRight ? deadSprites_R : deadSprites_L;
			count = facingRight ? deadCount_R : deadCount_L;
			break;
		}

		if (spriteArray && spriteArray[currentFrame] >= 0) {
			iShowImage(moveX, moveY, 100, 140, spriteArray[currentFrame]);
		}
		else {
			printf("Invalid image at frame %d\n", currentFrame);
		}
	}
	
	// Changes the character's animation state and resets the frame to zero to start animation of the state from beggining again
	void setState(AnimationState newState) {
		if (characterState != newState) {
			characterState = newState;
			currentFrame = 0;
		}
	}

	bool jumpInProgress = false;
	void handleInputMovement();
	void handleJump();
	void handleAttack();
	void handleDefaultState();
	void updateBackgroundScroll(Character &player);
};

//Character c1, c2;

// this fuction loads sprites in sequence from a folder
// *arr points to int array that hold ID of loaded sprites  
// count is passed by ref for the func to change the original value
// *folder is a pointer to string that hold path of my sprites folder
// *prefix is pointer to string that hold common name of all the sprites in a folder
//										[walk001.p, walk002.p, ..... etc]
void loadSprites(int *arr, int &count, const char *folder, const char *prefix, int frameCount){
	char path[200];  // holds the file path of each image
	count = 0;  
	for (int i = 0; i < frameCount; i++) {
		// Build "folder/prefix1.png", "folder/prefix2.png", …
		sprintf_s(path, sizeof(path), "%s/%s%d.png", folder, prefix, i + 1);

		// Load once and check for failure
		int imageID = iLoadImage(path);
		if (imageID < 0) {
			printf("Failed to load sprite: %s\n", path);
			continue; // skip invalid image
		}
		arr[count++] = imageID;
	}
}

void loadCaptainAmerica(Character &ca) {
	loadSprites(ca.idleSprites_R, ca.idleCount_R, "SPRITE/CaptainAmerica/IDLE_R", "idle", 1);
	loadSprites(ca.idleSprites_L, ca.idleCount_L, "SPRITE/CaptainAmerica/IDLE_L", "idle", 1);

	loadSprites(ca.walkSprites_R, ca.walkCount_R, "SPRITE/CaptainAmerica/WALK_R", "walk", 12);
	loadSprites(ca.walkSprites_L, ca.walkCount_L, "SPRITE/CaptainAmerica/WALK_L", "walk", 12);

	loadSprites(ca.jumpSprites_R, ca.jumpCount_R, "SPRITE/CaptainAmerica/JUMP_R", "jump", 6);
	loadSprites(ca.jumpSprites_L, ca.jumpCount_L, "SPRITE/CaptainAmerica/JUMP_L", "jump", 6);

	loadSprites(ca.attackSprites_R, ca.attackCount_R, "SPRITE/CaptainAmerica/ATTACK_R", "attack", 8);
	loadSprites(ca.attackSprites_L, ca.attackCount_L, "SPRITE/CaptainAmerica/ATTACK_L", "attack", 8);

	//loadSprites(ca.skillSprites_R, ca.skillCount_R, "SPRITE/CaptainAmerica/SKILL_R", "skill", 12);
	//loadSprites(ca.skillSprites_L, ca.skillCount_L, "SPRITE/CaptainAmerica/SKILL_L", "skill", 12);

	loadSprites(ca.ultimateSprites_R, ca.ultimateCount_R, "SPRITE/CaptainAmerica/ULTIMATE_R", "ult", 6);
	loadSprites(ca.ultimateSprites_L, ca.ultimateCount_L, "SPRITE/CaptainAmerica/ULTIMATE_L", "ult", 6);

	loadSprites(ca.deadSprites_R, ca.deadCount_R, "SPRITE/CaptainAmerica/DEAD_R", "dead", 7);
	loadSprites(ca.deadSprites_L, ca.deadCount_L, "SPRITE/CaptainAmerica/DEAD_L", "dead", 7);
}

void loadThor(Character &thor) {
	loadSprites(thor.idleSprites_R, thor.idleCount_R, "SPRITE/Thor/IDLE_R", "idle", 1);
	loadSprites(thor.idleSprites_L, thor.idleCount_L, "SPRITE/Thor/IDLE_L", "idle", 1);

	loadSprites(thor.walkSprites_R, thor.walkCount_R, "SPRITE/Thor/WALK_R", "walk", 8);
	loadSprites(thor.walkSprites_L, thor.walkCount_L, "SPRITE/Thor/WALK_L", "walk", 8);

	loadSprites(thor.jumpSprites_R, thor.jumpCount_R, "SPRITE/Thor/JUMP_R", "jump", 6);
	loadSprites(thor.jumpSprites_L, thor.jumpCount_L, "SPRITE/Thor/JUMP_L", "jump", 6);

	loadSprites(thor.attackSprites_R, thor.attackCount_R, "SPRITE/Thor/ATTACK_R", "attack", 11);
	loadSprites(thor.attackSprites_L, thor.attackCount_L, "SPRITE/Thor/ATTACK_L", "attack", 11);

	//loadSprites(thor.skillSprites_R, thor.skillCount_R, "SPRITE/Thor/SKILL_R", "skill", 12);
	//loadSprites(thor.skillSprites_L, thor.skillCount_L, "SPRITE/Thor/SKILL_L", "skill", 12);

	//loadSprites(thor.ultimateSprites_R, thor.ultimateCount_R, "SPRITE/Thor/ULTIMATE_R", "ult", 6);
	//loadSprites(thor.ultimateSprites_L, thor.ultimateCount_L, "SPRITE/Thor/ULTIMATE_L", "ult", 6);

	loadSprites(thor.deadSprites_R, thor.deadCount_R, "SPRITE/Thor/DEAD_R", "dead", 1);
	loadSprites(thor.deadSprites_L, thor.deadCount_L, "SPRITE/Thor/DEAD_L", "dead", 1);
}


void handleInputMovement(Character &c1, bool moveRight, bool moveLeft) {
	// Don't allow movement if character is dead
	if (c1.characterState == DEAD || !c1.alive) return;

	if (moveLeft) {
		c1.facingRight = false;
		c1.moveX -= 5;
	}
	if (moveRight) {
		c1.facingRight = true;
		c1.moveX += 5;
	}
}

void handleJump(Character &c1, bool wPressed) {
	// Don't allow jumping if character is dead
	if (c1.characterState == DEAD || !c1.alive) return;

	if (wPressed && !c1.jumpInProgress) {
		c1.setState(JUMP);
		c1.currentFrame = 0;
		c1.jumpInProgress = true;
	}

	if (c1.jumpInProgress) {
		int f = c1.currentFrame;
		if (f == 0) c1.moveY += 14;
		else if (f == 1) c1.moveY += 10;
		else if (f == 2) c1.moveY += 6;
		else if (f == 3) c1.moveY -= 6;
		else if (f == 4) c1.moveY -= 10;
		else if (f == 5) c1.moveY -= 14;

		int maxJump = c1.facingRight ? c1.jumpCount_R : c1.jumpCount_L;
		if (c1.currentFrame >= maxJump - 1) {
			c1.moveY = c1.baseY;
			c1.jumpInProgress = false;
			c1.setState(IDLE);
		}
	}
}

void handleAttack(Character &c1, bool fPressed) {
	// Don't allow attacking if character is dead
	if (c1.characterState == DEAD || !c1.alive) return;

	if (c1.characterState == ATTACK) {
		int maxAtk = c1.facingRight ? c1.attackCount_R : c1.attackCount_L;
		if (c1.currentFrame >= maxAtk - 1) {
			if (fPressed) {
				c1.currentFrame = 0;  // holding f attack animation infinite loop
			}
			else {
				c1.setState(IDLE);  // release f to stop attacking
			}
		}
		return;
	}
	else if (fPressed) {
		c1.setState(ATTACK);
	}
}

void handleUltimate(Character &c1, bool qPressed) {
	// Don't allow ultimate if character is dead
	if (c1.characterState == DEAD || !c1.alive) return;

	if (c1.characterState == ULTIMATE) {
		int maxUlt = c1.facingRight ? c1.ultimateCount_R : c1.ultimateCount_L;
		if (c1.currentFrame >= maxUlt - 1) {
			if (qPressed) {
				c1.currentFrame = 0;
			}
			else {
				c1.setState(IDLE);
			}
		}
		return;
	}
	else if (qPressed && c1.canUltimate) {
		c1.setState(ULTIMATE);
	}
}

void handleDefaultState(Character &c1, bool moveRight, bool moveLeft) {
	// Don't override state if character is dead
	if (c1.characterState == DEAD || !c1.alive) return;

	if (!c1.jumpInProgress && c1.characterState != ATTACK && c1.characterState != ULTIMATE) {
		if (moveRight || moveLeft) {
			c1.setState(WALK);
		}
		else {
			c1.setState(IDLE);
		}

		// Ground check
		if (c1.moveY < c1.baseY) {
			c1.moveY = c1.baseY;
		}
	}
}

#endif






// --------------- might use later -------------------------
/*
void loadCaptainAmerica(Character& ca) {
ca.name = "Captain America";

loadAnimation(ca.idleSprites, ca.idleCount, "Characters/CaptainAmerica/Idle/ca_idle_", 1);
loadAnimation(ca.moveSprites, ca.moveCount, "Characters/CaptainAmerica/Walk/ca_walk_", 12);
loadAnimation(ca.moveBackSprites, ca.moveBackCount, "Characters/CaptainAmerica/WalkBack/ca_back_", 12); // optional
loadAnimation(ca.jumpSprites, ca.jumpCount, "Characters/CaptainAmerica/Jump/ca_jump_", 8);
loadAnimation(ca.attackSprites, ca.attackCount, "Characters/CaptainAmerica/Attack/ca_atk_", 10);
loadAnimation(ca.skillSprites, ca.skillCount, "Characters/CaptainAmerica/Skill/ca_skill_", 15);
loadAnimation(ca.ultimateSprites, ca.ultimateCount, "Characters/CaptainAmerica/Ultimate/ca_ult_", 20);
loadAnimation(ca.deadSprites, ca.deadCount, "Characters/CaptainAmerica/Dead/ca_dead_", 10);
}

void draw() {
	int* spriteArray = nullptr;
	int count = 0;

	switch (characterState) {
	case IDLE:
		if (facingRight) {
			spriteArray = idleSprites_R;
			count = idleCount_R;
		}
		else {
			spriteArray = idleSprites_L;
			count = idleCount_L;
		} break;
	case WALK:
		if (facingRight) {
			spriteArray = walkSprites_R;
			count = walkCount_R;
		}
		else {
			spriteArray = walkSprites_L;
			count = walkCount_L;
		} break;

	case JUMP:
		if (facingRight) {
			spriteArray = jumpSprites_R;
			count = jumpCount_R;
		}
		else {
			spriteArray = jumpSprites_L;
			count = jumpCount_L;
		} break;

	case ATTACK:
		if (facingRight) {
			spriteArray = attackSprites_R;
			count = attackCount_R;
		}
		else {
			spriteArray = attackSprites_L;
			count = attackCount_L;
		} break;

	case SKILL:
		if (facingRight) {
			spriteArray = skillSprites_R;
			count = skillCount_R;
		}
		else {
			spriteArray = skillSprites_L;
			count = skillCount_L;
		} break;

	case ULTIMATE:
		if (facingRight) {
			spriteArray = ultimateSprites_R;
			count = ultimateCount_R;
		}
		else {
			spriteArray = ultimateSprites_L;
			count = ultimateCount_L;
		} break;

	case DEAD:
		if (facingRight) {
			spriteArray = deadSprites_R;
			count = deadCount_R;
		}
		else {
			spriteArray = deadSprites_L;
			count = deadCount_L;
		} break;
	}

	if (spriteArray && spriteArray[currentFrame] >= 0) {
		iShowImage(moveX, moveY, 100, 140, spriteArray[currentFrame]);
	}
	else {
		printf("Invalid image at frame %d\n", currentFrame);
	}
}

*/
#ifndef COMBAT_HPP
#define COMBAT_HPP

#include "character.hpp" 
#include "menu.hpp"
#include <cmath>

// Combat constants
const int ATTACK_DAMAGE = 100;
const int ULTIMATE_DAMAGE = 200;
const int HEALTH_BAR_WIDTH = 350;
const int HEALTH_BAR_HEIGHT = 25;
const int ATTACK_RANGE = 100;
const int ULTIMATE_RANGE = 130;

// Health bar positions
const int P1_HEALTH_X = 80;                                    
const int P1_HEALTH_Y = SCREEN_HEIGHT - 80;                   
const int P2_HEALTH_X = SCREEN_WIDTH - HEALTH_BAR_WIDTH - 100;  
const int P2_HEALTH_Y = SCREEN_HEIGHT - 80;                   

// for Winner screen
bool showMatchEndTimer = false;
int matchEndCountdown = 5;
int victoryScreenImage;
int retryButtonImage;
int exitButtonImage;
bool retryButtonHover = false;
bool exitButtonHover = false;
bool victoryTextBlink = true;

void resetVictorySystem();

// Victory screen button structures
Button retryButton;
Button exitButton;

enum GameResult {
	GAME_ONGOING,
	PLAYER1_WINS,
	PLAYER2_WINS,
	GAME_DRAW
};

GameResult gameResult = GAME_ONGOING;
bool matchEnded = false;

bool p1AttackHit = false;
bool p2AttackHit = false;
bool p1UltimateHit = false;
bool p2UltimateHit = false;

// Simple range-based collision detection
bool isTargetInRange(const Character& attacker, const Character& target, int range) {
	int horizontalDistance = abs(attacker.moveX - target.moveX);
	// Use baseY (ground position) instead of current moveY for vertical check
	int verticalDistance = abs(attacker.baseY - target.baseY);

	bool inHorizontalRange = horizontalDistance <= range;
	bool inVerticalRange = verticalDistance <= 40; // Keep original tolerance
	bool facingTarget = (attacker.facingRight && attacker.moveX < target.moveX) || (!attacker.facingRight && attacker.moveX > target.moveX);

	return inHorizontalRange && inVerticalRange && facingTarget;
}

void addEnergy(Character &attacker) {
	attacker.energy += 25;
	if (attacker.energy >= 100) {
		attacker.energy = 100;
		attacker.canUltimate = true;
	}
}

// Apply damage to a character
void applyDamageToCharacter(Character& target, int damage) {
	if (target.hp > 0) {
		target.hp -= damage;

		printf("HIT success!\n");

		if (target.hp <= 0) {
			target.hp = 0;
			target.alive = false;
			target.setState(DEAD); // Set to DEAD state to show dead sprites
			printf("PLAYER DEFEATED!\n");
		}
	}
}

// Handle attack collisions between two characters
void handleCombatCollisions(Character& player1, Character& player2) {

	// Player 1 attacking Player 2
	if (player1.characterState == ATTACK && player2.alive) {
		int hitFrame = (player1.facingRight ? player1.attackCount_R : player1.attackCount_L) / 2;

		// Reset hit flag when animation restarts (frame 0)
		if (player1.currentFrame == 0) p1AttackHit = false;

		if (player1.currentFrame == hitFrame && !p1AttackHit) {
			if (isTargetInRange(player1, player2, ATTACK_RANGE)) {
				applyDamageToCharacter(player2, ATTACK_DAMAGE);
				addEnergy(player1);   // Giving energy to P1
				p1AttackHit = true; // Prevents multiple hits on same frame
			}
		}
	}
	else if (player1.characterState != ATTACK) {
		p1AttackHit = false;
	}

	// Player 1 ultimate on Player 2
	if (player1.characterState == ULTIMATE && player2.alive) {
		int hitFrame = (player1.facingRight ? player1.ultimateCount_R : player1.ultimateCount_L) / 2;

		// Reset hit flag when animation restarts
		if (player1.currentFrame == 0) p1UltimateHit = false;

		if (player1.currentFrame == hitFrame && !p1UltimateHit) {
			if (isTargetInRange(player1, player2, ULTIMATE_RANGE)) {
				applyDamageToCharacter(player2, ULTIMATE_DAMAGE);
				player1.energy = 0; // Consumed energy
				player1.canUltimate = false;
				p1UltimateHit = true;
			}
		}
	}
	else if (player1.characterState != ULTIMATE) {
		p1UltimateHit = false;
	}

	// Player 2 attacking Player 1
	if (player2.characterState == ATTACK && player1.alive) {
		int hitFrame = (player2.facingRight ? player2.attackCount_R : player2.attackCount_L) / 2;

		if (player2.currentFrame == 0) {
			p2AttackHit = false;
		}

		if (player2.currentFrame == hitFrame && !p2AttackHit) {
			if (isTargetInRange(player2, player1, ATTACK_RANGE)) {
				applyDamageToCharacter(player1, ATTACK_DAMAGE);
				addEnergy(player2); // Giving energy to P2
				p2AttackHit = true;
			}
		}
	}
	else if (player2.characterState != ATTACK) {
		p2AttackHit = false;
	}

	// Player 2 ultimate on Player 1
	if (player2.characterState == ULTIMATE && player1.alive) {
		int hitFrame = (player2.facingRight ? player2.ultimateCount_R : player2.ultimateCount_L) / 2;

		if (player2.currentFrame == 0) {
			p2UltimateHit = false;
		}

		if (player2.currentFrame == hitFrame && !p2UltimateHit) {
			if (isTargetInRange(player2, player1, ULTIMATE_RANGE)) {
				applyDamageToCharacter(player1, ULTIMATE_DAMAGE);
				player2.energy = 0; // Consumed energy
				player2.canUltimate = false;
				p2UltimateHit = true;
			}
		}
	}
	else if (player2.characterState != ULTIMATE) {
		p2UltimateHit = false;
	}
}

// Prevent characters from walking through each other
void handleCharacterSeparation(Character& char1, Character& char2) {
	const int MIN_SEPARATION = 85;
	int distance = abs(char1.moveX - char2.moveX);

	if (distance < MIN_SEPARATION && abs(char1.moveY - char2.moveY) < 30) {
		int pushAmount = (MIN_SEPARATION - distance) / 2;

		if (char1.moveX < char2.moveX) {
			char1.moveX -= pushAmount;
			char2.moveX += pushAmount;
		}
		else {
			char1.moveX += pushAmount;
			char2.moveX -= pushAmount;
		}

		// Keep characters within screen boundaries
		if (char1.moveX < 0) char1.moveX = 0;
		if (char2.moveX < 0) char2.moveX = 0;
		if (char1.moveX > SCREEN_WIDTH - 100) char1.moveX = SCREEN_WIDTH - 100;
		if (char2.moveX > SCREEN_WIDTH - 100) char2.moveX = SCREEN_WIDTH - 100;
	}
}

// Check for victory conditions
// Check for victory conditions
void checkVictoryConditions(const Character& p1, const Character& p2) {
	if (!matchEnded) {
		if (!p1.alive && !p2.alive) {
			gameResult = GAME_DRAW;
			matchEnded = true;
			showMatchEndTimer = true;
		}
		else if (!p1.alive) {
			gameResult = PLAYER2_WINS;
			matchEnded = true;
			showMatchEndTimer = true;
		}
		else if (!p2.alive) {
			gameResult = PLAYER1_WINS;
			matchEnded = true;
			showMatchEndTimer = true;
		}
	}
}

void renderEnergyBar(const Character& character, int x, int y, bool isPlayerOne = true) {
	int barWidth = HEALTH_BAR_WIDTH;
	int barHeight = 12;

	// Energy Label positioning based on player
	int labelX, labelY, labelWidth = 60, labelHeight = 22;

	if (isPlayerOne) {
		// Player 1: Position label at the right end of the energy bar
		labelX = x + barWidth - labelWidth - 10;
		labelY = y + 8;
	}
	else {
		// Player 2: Keep label at the left (or adjust as needed)
		labelX = x + 10;
		labelY = y + 8;
	}

	// Energy Label - Dark background
	iSetColor(0, 0, 0);
	iFilledRectangle(labelX, labelY, labelWidth, labelHeight);

	// Energy Label - White text
	iSetColor(255, 255, 255);
	iText(labelX + 10, labelY + 9, "Energy", GLUT_BITMAP_HELVETICA_12);

	// Background
	iSetColor(60, 60, 60);
	iFilledRectangle(x, y, barWidth, barHeight);

	// Energy fill
	int energyWidth = (int)((barWidth)* (character.energy / 100.0));

	iSetColor(0, 120, 255); // Blue energy
	if (energyWidth > 0) {
		iFilledRectangle(x, y, energyWidth, barHeight);
	}

	// Outline
	iSetColor(200, 200, 200);
	iRectangle(x, y, barWidth, barHeight);
}
/*
void renderEnergyBar(const Character& character, int x, int y) {
	int barWidth = HEALTH_BAR_WIDTH;
	int barHeight = 12;

	// Energy Label - Dark background
	iSetColor(0, 0, 0);
	iFilledRectangle(x + 10, y + 8, 60, 22);

	// Energy Label - White text
	iSetColor(255, 255, 255);
	iText(x + 20, y + 17, "Energy", GLUT_BITMAP_HELVETICA_12);

	// Background
	iSetColor(60, 60, 60);
	iFilledRectangle(x, y, barWidth, barHeight);

	// Energy fill
	int energyWidth = (int)((barWidth)* (character.energy / 100.0));

	iSetColor(0, 120, 255); // Blue energy
	if (energyWidth > 0) {
		iFilledRectangle(x, y, energyWidth, barHeight);
	}

	// Outline
	iSetColor(200, 200, 200);
	iRectangle(x, y, barWidth, barHeight);
}
*/

// Draw health bars
void renderHealthBar(const Character& character, int x, int y, bool isPlayerOne) {
	// Outer border/shadow for depth
	iSetColor(30, 30, 30);
	iFilledRectangle(x - 3, y - 3, HEALTH_BAR_WIDTH + 6, HEALTH_BAR_HEIGHT + 6);

	// Background
	iSetColor(60, 60, 60);
	iFilledRectangle(x, y, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT);

	// Inner border
	iSetColor(200, 200, 200);
	iRectangle(x + 2, y + 2, HEALTH_BAR_WIDTH - 4, HEALTH_BAR_HEIGHT - 4);

	// Calculate health percentage
	double healthPercent = character.hp / 1000.0;
	int healthWidth = (int)((HEALTH_BAR_WIDTH - 4) * healthPercent);

	// Set health bar color with gradient effect
	if (healthPercent > 0.6) iSetColor(50, 255, 50); // Bright green
	else if (healthPercent > 0.3) iSetColor(255, 180, 0); // Orange
	else iSetColor(255, 50, 50); // Bright red

	// Draw health fill with padding
	if (healthWidth > 0) iFilledRectangle(x + 2, y + 2, healthWidth, HEALTH_BAR_HEIGHT - 4);
	
	// Draw player label with proper positioning
	if (isPlayerOne) {
		// Player 1: Dark background for text on the left
		iSetColor(0, 0, 0);
		iFilledRectangle(x+10, y - 22, 80, 20);

		// White text
		iSetColor(255, 255, 255);
		iText(x + 25, y - 15, "Player 1", GLUT_BITMAP_HELVETICA_12);
	}
	else {
		// Player 2: Dark background for text on the right side of health bar
		iSetColor(0, 0, 0);
		iFilledRectangle(x + HEALTH_BAR_WIDTH - 90, y - 22, 80, 20);

		// White text
		iSetColor(255, 255, 255);
		iText(x + HEALTH_BAR_WIDTH - 75, y - 15, "Player 2", GLUT_BITMAP_HELVETICA_12);
	}

	// Health percentage display - BLACK TEXT inside health bar
	char healthText[20];
	sprintf_s(healthText, sizeof(healthText), "%.0f%%", healthPercent * 100);
	iSetColor(0, 0, 0); // Changed to black for better visibility

	if (isPlayerOne) {
		iText(x + HEALTH_BAR_WIDTH - 50, y + 8, healthText, GLUT_BITMAP_8_BY_13);
	}
	else {
		iText(x + 10, y + 8, healthText, GLUT_BITMAP_8_BY_13);
	}
}

// Draw both players' health bars
void drawBothHealthBars(const Character& player1, const Character& player2) {
	renderHealthBar(player1, P1_HEALTH_X, P1_HEALTH_Y, true);
	renderHealthBar(player2, P2_HEALTH_X, P2_HEALTH_Y, false);
	// Energy bars
	renderEnergyBar(player1, P1_HEALTH_X, P1_HEALTH_Y + 30, true);   // Player 1
	renderEnergyBar(player2, P2_HEALTH_X, P2_HEALTH_Y + 30, false);  // Player 2
}

// Main combat update - call this in your game loop
void processCombatSystem(Character& p1, Character& p2) {
	if (currentScreen == 20 && !matchEnded) {
		handleCharacterSeparation(p1, p2);
		handleCombatCollisions(p1, p2);
		checkVictoryConditions(p1, p2);
	}
}

// Reset the combat system and characters
void resetCombatSystem(Character& p1, Character& p2) {
	// Reset combat tracking
	gameResult = GAME_ONGOING;
	matchEnded = false;
	p1AttackHit = false;
	p2AttackHit = false;
	p1UltimateHit = false;
	p2UltimateHit = false;

	// Reset character health and status
	p1.hp = 1000;
	p1.alive = true;
	p1.setState(IDLE);
	p1.energy = 0;           
	p1.canUltimate = false;  

	p2.hp = 1000;
	p2.alive = true;
	p2.setState(IDLE);
	p2.energy = 0;           
	p2.canUltimate = false;  

	// Reset positions to starting positions
	p1.moveX = 200;
	p1.moveY = p1.baseY;
	p1.facingRight = true;

	p2.moveX = 980;
	p2.moveY = p2.baseY;
	p2.facingRight = false;

	// Reset any ongoing animations
	p1.currentFrame = 0;
	p2.currentFrame = 0;
	p1.jumpInProgress = false;
	p2.jumpInProgress = false;

	resetVictorySystem();
}


// Handle restart and menu navigation inputs - no functionality cause no end screen
bool processCombatInput(unsigned char key, Character& p1, Character& p2) {
	// make logic later
	return false;
}

// Load victory screen assets
void loadVictoryScreenAssets() {
	victoryScreenImage = iLoadImage("BG/win3.png"); // Your victory background
	retryButtonImage = iLoadImage("UiElements/retryButton.png");
	exitButtonImage = iLoadImage("UiElements/button4.png");

	// Position buttons
	retryButton.x = 400;
	retryButton.y = 90;
	retryButton.width = 200;
	retryButton.height = 80;

	exitButton.x = 680;
	exitButton.y = 90;
	exitButton.width = 200;
	exitButton.height = 80;
}

// Show victory screen
void showVictoryScreen() {
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, victoryScreenImage);

	// Show winner text
	if (victoryTextBlink) {
		iSetColor(216, 217, 218); // Red when blinking on
	}
	else {
		iSetColor(0, 0, 0); // White when blinking off
	}

	if (gameResult == PLAYER1_WINS) {
		iText(SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT - 180, "PLAYER 1 WINS!", GLUT_BITMAP_HELVETICA_18);
	}
	else if (gameResult == PLAYER2_WINS) {
		iText(SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT - 180, "PLAYER 2 WINS!", GLUT_BITMAP_HELVETICA_18);
	}
	else if (gameResult == GAME_DRAW) {
		iText(SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT - 180, "IT'S A DRAW!", GLUT_BITMAP_HELVETICA_18);
	}

	// Show retry button with hover effect
	if (retryButtonHover) {
		double zoomFactor = 1.1;
		int newWidth = retryButton.width * zoomFactor;
		int newHeight = retryButton.height * zoomFactor;
		int newX = retryButton.x - (newWidth - retryButton.width) / 2;
		int newY = retryButton.y - (newHeight - retryButton.height) / 2;
		iShowImage(newX, newY, newWidth, newHeight, retryButtonImage);
	}
	else {
		iShowImage(retryButton.x, retryButton.y, retryButton.width, retryButton.height, retryButtonImage);
	}

	// Show exit button with hover effect
	if (exitButtonHover) {
		double zoomFactor = 1.1;
		int newWidth = exitButton.width * zoomFactor;
		int newHeight = exitButton.height * zoomFactor;
		int newX = exitButton.x - (newWidth - exitButton.width) / 2;
		int newY = exitButton.y - (newHeight - exitButton.height) / 2;
		iShowImage(newX, newY, newWidth, newHeight, exitButtonImage);
	}
	else {
		iShowImage(exitButton.x, exitButton.y, exitButton.width, exitButton.height, exitButtonImage);
	}
}

// Handle victory screen hover
void handleVictoryScreenHover(int mx, int my) {
	retryButtonHover = false;
	exitButtonHover = false;

	if (mx >= retryButton.x && mx <= retryButton.x + retryButton.width &&
		my >= retryButton.y && my <= retryButton.y + retryButton.height) {
		retryButtonHover = true;
	}

	if (mx >= exitButton.x && mx <= exitButton.x + exitButton.width &&
		my >= exitButton.y && my <= exitButton.y + exitButton.height) {
		exitButtonHover = true;
	}
}

// Handle victory screen clicks
int handleVictoryScreenClick(int mx, int my) {
	if (mx >= retryButton.x && mx <= retryButton.x + retryButton.width &&
		my >= retryButton.y && my <= retryButton.y + retryButton.height) {
		return 0; // Retry clicked
	}

	if (mx >= exitButton.x && mx <= exitButton.x + exitButton.width &&
		my >= exitButton.y && my <= exitButton.y + exitButton.height) {
		return 1; // Exit clicked
	}

	return -1; // No button clicked
}

// Update match end countdown
void updateMatchEndTimer() {
	if (showMatchEndTimer && matchEndCountdown > 0) {
		matchEndCountdown--;
		if (matchEndCountdown <= 0) {
			showMatchEndTimer = false;
			currentScreen = 25; // Victory screen
		}
	}
}

// Reset victory system
void resetVictorySystem() {
	showMatchEndTimer = false;
	matchEndCountdown = 5;
	retryButtonHover = false;
	exitButtonHover = false;
	victoryTextBlink = true; // Add this line
}

#endif









// old Collision func

/*
void handleCombatCollisions(Character& player1, Character& player2) {
// Player 1 attacks Player 2
if (player1.characterState == ATTACK && player2.alive) {
int maxAtk = player1.facingRight ? player1.attackCount_R : player1.attackCount_L;

// Reset hit flag when animation cycles (starts new cycle)
if (player1.currentFrame == 0) {
p1AttackHit = false;
}

if (isTargetInRange(player1, player2, ATTACK_RANGE) && !p1AttackHit) {
applyDamageToCharacter(player2, ATTACK_DAMAGE);
p1AttackHit = true;
}
}
else if (player1.characterState != ATTACK) {
p1AttackHit = false; // Reset when not attacking
}

// Player 2 attacks Player 1
if (player2.characterState == ATTACK && player1.alive) {
int maxAtk = player2.facingRight ? player2.attackCount_R : player2.attackCount_L;

// Reset hit flag when animation cycles (starts new cycle)
if (player2.currentFrame == 0) {
p2AttackHit = false;
}

if (isTargetInRange(player2, player1, ATTACK_RANGE) && !p2AttackHit) {
applyDamageToCharacter(player1, ATTACK_DAMAGE);
p2AttackHit = true;
}
}
else if (player2.characterState != ATTACK) {
p2AttackHit = false; // Reset when not attacking
}

// Player 1 ultimate on Player 2
if (player1.characterState == ULTIMATE && player2.alive) {
int maxUlt = player1.facingRight ? player1.ultimateCount_R : player1.ultimateCount_L;

// Reset hit flag when animation cycles (starts new cycle)
if (player1.currentFrame == 0) {
p1UltimateHit = false;
}

if (isTargetInRange(player1, player2, ULTIMATE_RANGE) && !p1UltimateHit) {
applyDamageToCharacter(player2, ULTIMATE_DAMAGE);
p1UltimateHit = true;
}
}
else if (player1.characterState != ULTIMATE) {
p1UltimateHit = false;
}

// Player 2 ultimate on Player 1
if (player2.characterState == ULTIMATE && player1.alive) {
int maxUlt = player2.facingRight ? player2.ultimateCount_R : player2.ultimateCount_L;

// Reset hit flag when animation cycles (starts new cycle)
if (player2.currentFrame == 0) {
p2UltimateHit = false;
}

if (isTargetInRange(player2, player1, ULTIMATE_RANGE) && !p2UltimateHit) {
applyDamageToCharacter(player1, ULTIMATE_DAMAGE);
p2UltimateHit = true;
}
}
else if (player2.characterState != ULTIMATE) {
p2UltimateHit = false;
}
}

*/
#ifndef MENU_HPP
#define MENU_HPP

#include <stdlib.h>
#include <stdio.h>
#include "character.hpp"
#include "music.hpp"
//#include "character.hpp"

#define BUTTON_COUNT 4
#define CHARACTER_COUNT 6
#define ARENA_COUNT 1
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720


extern char playerName[100];
extern int nameInputMode;
extern bool playerWonArcade;
extern int arcadeTimerSeconds;
extern bool isArcadeTimerRunning;

// Main Menu Screen Value = -1
// Sub Menu Screen Value = 0
// Settings Screen = 1
// About Screen = 2
// 1v1 Character Selection Screen value = 10
// Arcade Mode Character Selection Screen value = 11
// 1v1 Arena Screen value = 20
// Arcade Arena Screen value = 30
// Arcade End Screen value = 40
// 1v1 Victory Screen value = 25

int selectedCharacterIndexP1 = 2; //default // index for spawning character in areana
int selectedCharacterIndexP2 = 2; //default
// 1 for ironman
// 2 for cap
// 3 for thor
// 4 for thanos
// 5 for logan
// 6 for hulk

int currentScreen = -1;

// Main menu variables
int mainMenuScreen = -1;
int buttonImages[BUTTON_COUNT];
const char* buttonNames[BUTTON_COUNT] = { "Play", "Settings", "About", "Exit" };
int hoveredButtonIndex = -1;

//  Variables for the two options after clicking "Play"

int optionButtonImages[2];
int hoveredOptionButtonIndex = -1;
int subMenuBackground;

// Variables for Character Selection
int characterImages[CHARACTER_COUNT];
int characterSelectionBackground;
int selectedCharacter = -1;
int selectedCharacterP1 = -1;
int selectedCharacterP2 = -1;
int selectionImages[2];

// Variables for Arena
int arenaImages[ARENA_COUNT];

//variables for dynamicBackground
int dynamicBgImage;
float bgX1 = 0;
float bgX2 = SCREEN_WIDTH - 20;
float bgScrollSpeed = 10.0f;

// Back button variables
int backButtonImage;
bool backButtonHover = false;

struct Button {
	int x;
	int y;
	int width;
	int height;
};

Button buttons[BUTTON_COUNT];
Button backButton;
Button optionButtons[2];
Button characterButtons[CHARACTER_COUNT]; // Arcade mode buttons
Button characterButtonsP1[CHARACTER_COUNT]; // Player 1 buttons
Button characterButtonsP2[CHARACTER_COUNT]; // Player 2 buttons

// menu 
void showMenu() {
	// Music Management
	int lastScreen = -2; // tracking last screen to prevent repeated music calls
	if (currentScreen != lastScreen) {
		// Screens for Main Menu music
		if (currentScreen == -1 || currentScreen == 0 || currentScreen == 1 || currentScreen == 2 || currentScreen == 10 || currentScreen == 11) {
			stopArenaMusic();
			startMainMenuMusic();
		}
		// Screens for Arena music
		else if (currentScreen == 20 || currentScreen == 30) {
			stopMainMenuMusic();
			startArenaMusic();
		}
		lastScreen = currentScreen;
	}

	if (currentScreen == -1) { // On the Main Menu
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, mainMenuScreen);
		//startMainMenuMusic(); // Starting Main Menu Music

		// main menu buttons 
		for (int i = 0; i < BUTTON_COUNT; i++) {
			iShowImage(buttons[i].x, buttons[i].y, buttons[i].width, buttons[i].height, buttonImages[i]);
		}
		if (hoveredButtonIndex != -1) {
			int i = hoveredButtonIndex;
			double zoomFactor = 1.1;
			int newWidth = buttons[i].width * zoomFactor;
			int newHeight = buttons[i].height * zoomFactor;
			int newX = buttons[i].x - (newWidth - buttons[i].width) / 2;
			int newY = buttons[i].y - (newHeight - buttons[i].height) / 2;
			iShowImage(newX, newY, newWidth, newHeight, buttonImages[i]);
		}
	}
	else if (currentScreen == 0) { // Play Sub-Menu
		//startMainMenuMusic(); // Starting Main Menu Music

		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, subMenuBackground);
		iSetColor(253, 240, 213);
		iText(590, 680, "Select Mode", GLUT_BITMAP_TIMES_ROMAN_24);
		iText(300, 580, "1 V 1", GLUT_BITMAP_HELVETICA_18);
		iText(920, 580, "Arcade", GLUT_BITMAP_HELVETICA_18);

		//two option buttons
		for (int i = 0; i < 2; i++) {
			iShowImage(optionButtons[i].x, optionButtons[i].y, optionButtons[i].width, optionButtons[i].height, optionButtonImages[i]);
		}

		//  hover effect for option buttons
		if (hoveredOptionButtonIndex != -1) {
			int i = hoveredOptionButtonIndex;
			double zoomFactor = 1.06;
			int newWidth = optionButtons[i].width * zoomFactor;
			int newHeight = optionButtons[i].height * zoomFactor;
			int newX = optionButtons[i].x - (newWidth - optionButtons[i].width) / 2;
			int newY = optionButtons[i].y - (newHeight - optionButtons[i].height) / 2;
			iShowImage(newX, newY, newWidth, newHeight, optionButtonImages[i]);
		}

		//  Back Button on this screen
		if (backButtonHover) {
			double zoomFactor = 1.1;
			int newWidth = backButton.width * zoomFactor;
			int newHeight = backButton.height * zoomFactor;
			int newX = backButton.x - (newWidth - backButton.width) / 2;
			int newY = backButton.y - (newHeight - backButton.height) / 2;
			iShowImage(newX, newY, newWidth, newHeight, backButtonImage);
		}
		else {
			iShowImage(backButton.x, backButton.y, backButton.width, backButton.height, backButtonImage);
		}
	}
	else if (currentScreen == 1 || currentScreen == 2) {
		iSetColor(255, 255, 255);
		iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		iSetColor(0, 0, 0);
		//iText(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, buttonNames[currentScreen], GLUT_BITMAP_TIMES_ROMAN_24);

		// Back Button
		if (backButtonHover) {
			double zoomFactor = 1.1;
			int newWidth = backButton.width * zoomFactor;
			int newHeight = backButton.height * zoomFactor;
			int newX = backButton.x - (newWidth - backButton.width) / 2;
			int newY = backButton.y - (newHeight - backButton.height) / 2;
			iShowImage(newX, newY, newWidth, newHeight, backButtonImage);
		}
		else {
			iShowImage(backButton.x, backButton.y, backButton.width, backButton.height, backButtonImage);
		}
	}
	else if (currentScreen == 10 || currentScreen == 11) { // play options selection
		//stopArenaMusic(); // Stopping Arena Music
		//startMainMenuMusic(); // staring main menu musicf

		// Display which option was chosen
		if (currentScreen == 10) {
			iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, characterSelectionBackground);

			// Titles
			iSetColor(253, 240, 213);
			iText(175, SCREEN_HEIGHT - 70, "Player 1: Choose Your Character", GLUT_BITMAP_HELVETICA_18);
			iText(SCREEN_WIDTH - 425, SCREEN_HEIGHT - 70, "Player 2: Choose Your Character", GLUT_BITMAP_HELVETICA_18);

			// Player 1 Side (Left)
			for (int i = 0; i < CHARACTER_COUNT; i++) {
				iShowImage(characterButtonsP1[i].x, characterButtonsP1[i].y, characterButtonsP1[i].width, characterButtonsP1[i].height, characterImages[i]);
			}

			// Player 2 Side (Right)
			for (int i = 0; i < CHARACTER_COUNT; i++) {
				iShowImage(characterButtonsP2[i].x, characterButtonsP2[i].y, characterButtonsP2[i].width, characterButtonsP2[i].height, characterImages[i]);
			}

			// Show border around selected characters
			if (selectedCharacterP1 != -1) {
				iShowImage(characterButtonsP1[selectedCharacterP1].x - 30, characterButtonsP1[selectedCharacterP1].y - 30, 160, 185, selectionImages[0]);
			}
			if (selectedCharacterP2 != -1) {
				iShowImage(characterButtonsP2[selectedCharacterP2].x - 20, characterButtonsP2[selectedCharacterP2].y - 20, 140, 165, selectionImages[1]);
			}
			// Start Button on Character Selection
			if (selectedCharacterP1 != -1 && selectedCharacterP2 != -1) {
				iShowImage(520, 50, 250, 100, buttonImages[0]);
			}

			if (hoveredButtonIndex != -1) {
				int i = hoveredButtonIndex;
				double zoomFactor = 1.1;
				int newWidth = 250 * zoomFactor;
				int newHeight = 100 * zoomFactor;
				int newX = 520 - (newWidth - 250) / 2;
				int newY = 50 - (newHeight - 100) / 2;
				iShowImage(newX, newY, newWidth, newHeight, buttonImages[0]);
			}
		}

		else if (currentScreen == 11) {
			iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, characterSelectionBackground);

			iSetColor(253, 240, 213);
			iText(550, SCREEN_HEIGHT - 70, "Choose Your Character", GLUT_BITMAP_HELVETICA_18); // Title

			
			// Showing Character images
			for (int i = 0; i < CHARACTER_COUNT; i++){
				iShowImage(characterButtons[i].x, characterButtons[i].y, characterButtons[i].width, characterButtons[i].height, characterImages[i]);
			}

			// Show border around selected characters and start button;
			if (selectedCharacter != -1) {
				iShowImage(characterButtons[selectedCharacter].x - 40, characterButtons[selectedCharacter].y - 40, 210, 235, selectionImages[0]);
				iShowImage(520, 60, 250, 100, buttonImages[0]);
			}
			// Hover effect
			if (hoveredButtonIndex != -1) {
				int i = hoveredButtonIndex;
				double zoomFactor = 1.1;
				int newWidth = 250 * zoomFactor;
				int newHeight = 100 * zoomFactor;
				int newX = 520 - (newWidth - 250) / 2;
				int newY = 60 - (newHeight - 100) / 2;
				iShowImage(newX, newY, newWidth, newHeight, buttonImages[0]);
			}
		}

		// Back Button
		if (backButtonHover) {
			double zoomFactor = 1.1;
			int newWidth = backButton.width * zoomFactor;
			int newHeight = backButton.height * zoomFactor;
			int newX = backButton.x - (newWidth - backButton.width) / 2;
			int newY = backButton.y - (newHeight - backButton.height) / 2;
			iShowImage(newX, newY, newWidth, newHeight, backButtonImage);
		}
		else {
			iShowImage(backButton.x, backButton.y, backButton.width, backButton.height, backButtonImage);
		}
	}
	else if (currentScreen == 30) {
		//stopMainMenuMusic(); // stoping main meny music
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, arenaImages[0]);
		//startArenaMusic(); // Starting Arena Music
	}
}

void handleMenuClick(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (currentScreen == -1) { // On the main menu
			for (int i = 0; i < BUTTON_COUNT; i++) {
				if (mx >= buttons[i].x && mx <= buttons[i].x + buttons[i].width &&
					my >= buttons[i].y && my <= buttons[i].y + buttons[i].height) {
					if (i == 3) { exit(0); } // Exit button
					playClickSound(); // playing click sound
					currentScreen = i; // Sets screen to 0 (Play), 1 (Settings), or 2 (About)
					break;
				}
			}
		}
		else if (currentScreen == 0) { // On the Play Sub-Menu
			// Check for clicks on the two option buttons
			for (int i = 0; i < 2; i++) {
				if (mx >= optionButtons[i].x && mx <= optionButtons[i].x + optionButtons[i].width &&
					my >= optionButtons[i].y && my <= optionButtons[i].y + optionButtons[i].height) {
					playClickSound(); // playing click sound
					currentScreen = 10 + i; // Set screen to 10 or 11
					return; // Exit function after handling click
				}
			}
			// Check for click on the back button
			if (mx >= backButton.x && mx <= backButton.x + backButton.width &&
				my >= backButton.y && my <= backButton.y + backButton.height) {
				playClickSound(); // playing click sound
				currentScreen = -1; // Go back to the main menu
			}
		}
		else if (currentScreen == 1 || currentScreen == 2) { // On Settings or About

			// Check for back button click
			if (mx >= backButton.x && mx <= backButton.x + backButton.width &&
				my >= backButton.y && my <= backButton.y + backButton.height) {
				playClickSound(); // playing click sound
				currentScreen = -1; // Go back to the main menu
			}
		}
		else if (currentScreen == 10 || currentScreen == 11) { // On final placeholder screens
			// Check for back button click
			if (mx >= backButton.x && mx <= backButton.x + backButton.width &&
				my >= backButton.y && my <= backButton.y + backButton.height) {
				playClickSound(); // playing click sound
				currentScreen = 0; // Go back to the Play Sub-Menu
			}
			// 1v1 character selection
			if (currentScreen == 10) {
				for (int i = 0; i < CHARACTER_COUNT; i++) {
					// Player 1 selection
					if (mx >= characterButtonsP1[i].x && mx <= characterButtonsP1[i].x + characterButtonsP1[i].width &&
						my >= characterButtonsP1[i].y && my <= characterButtonsP1[i].y + characterButtonsP1[i].height) {
						playCharClickSound(); // playing Character Selection sound
						selectedCharacterP1 = i;
						selectedCharacterIndexP1 = i + 1;
					}
					// Player 2 selection
					if (mx >= characterButtonsP2[i].x && mx <= characterButtonsP2[i].x + characterButtonsP2[i].width &&
						my >= characterButtonsP2[i].y && my <= characterButtonsP2[i].y + characterButtonsP2[i].height) {
						playCharClickSound(); // playing Character Selection sound
						selectedCharacterP2 = i;
						selectedCharacterIndexP2 = i + 1;
					}
					// Entering Arena Screen
					if (selectedCharacterP1 != -1 && selectedCharacterP2 != -1) {
						if (mx >= 520 && mx <= 520 + 250 && my >= 50 && my <= 50 + 100) {
							playClickSound(); // playing click sound
							currentScreen = 20;
						}
					}
				}
			}
			// Arcade Mode Character Selection
			if (currentScreen == 11) {
			
				for (int i = 0; i < CHARACTER_COUNT; i++) {
					// Character selection
					if (mx >= characterButtons[i].x && mx <= characterButtons[i].x + characterButtons[i].width &&
						my >= characterButtons[i].y && my <= characterButtons[i].y + characterButtons[i].height) {
						playCharClickSound(); // playing Character Selection sound
						selectedCharacter = i;
						selectedCharacterIndexP1 = i + 1;
					}

					// Entering Arena Screen
					if (selectedCharacter != -1) {
						if (mx >= 520 && mx <= 520 + 250 && my >= 60 && my <= 60 + 100) {
							playClickSound(); // playing click sound
							currentScreen = 30;

							arcadeTimerSeconds = 0;
							isArcadeTimerRunning = true;
						}
					}
				}
			}
		}
	}
}

void handleHoverAnimation(int mx, int my){
	// By default, assume nothing is hovered
	hoveredButtonIndex = -1;
	hoveredOptionButtonIndex = -1;
	backButtonHover = false;

	if (currentScreen == -1) { // On the main menu
		for (int i = 0; i < BUTTON_COUNT; i++) {
			if (mx >= buttons[i].x && mx <= buttons[i].x + buttons[i].width &&
				my >= buttons[i].y && my <= buttons[i].y + buttons[i].height) {
				hoveredButtonIndex = i;
				break;
			}
		}
	}
	else if (currentScreen == 0) { // On the Play Sub-Menu
		// hover for the two option buttons
		for (int i = 0; i < 2; i++) {
			if (mx >= optionButtons[i].x && mx <= optionButtons[i].x + optionButtons[i].width &&
				my >= optionButtons[i].y && my <= optionButtons[i].y + optionButtons[i].height) {
				hoveredOptionButtonIndex = i;
				return; // Exit after finding a hover
			}
		}
		//hover for the back button
		if (mx >= backButton.x && mx <= backButton.x + backButton.width &&
			my >= backButton.y && my <= backButton.y + backButton.height) {
			backButtonHover = true;
		}
	}
	else if (currentScreen == 10) { // On Character Selection
		// Back button hover
		if (mx >= backButton.x && mx <= backButton.x + backButton.width &&
			my >= backButton.y && my <= backButton.y + backButton.height) {
			backButtonHover = true;
		}

		// Start button hover 
		if (selectedCharacterP1 != -1 && selectedCharacterP2 != -1) {
			if (mx >= 520 && mx <= 520 + 250 && my >= 50 && my <= 50 + 100) {
				hoveredButtonIndex = 0;
			}
		}
	}
	else if (currentScreen == 11) { // On Arcade mode Character Selection
		// Back button hover
		if (mx >= backButton.x && mx <= backButton.x + backButton.width &&
			my >= backButton.y && my <= backButton.y + backButton.height) {
			backButtonHover = true;
		}

		// Start button hover 
		if (selectedCharacter != -1) {
			if (mx >= 520 && mx <= 520 + 250 && my >= 60 && my <= 60 + 100) {
				hoveredButtonIndex = 0;
			}
		}
	}
	else { // On any other sub-screen (Settings, About, Option 1, Option 2)
		// back button hover
		if (mx >= backButton.x && mx <= backButton.x + backButton.width &&
			my >= backButton.y && my <= backButton.y + backButton.height) {
			backButtonHover = true;
		}
	}
}

// Load all images and set button positions
void loadMenuAssets() {
	mainMenuScreen = iLoadImage("BG/main.png");
	subMenuBackground = iLoadImage("BG/subMenu.jpg");
	characterSelectionBackground = iLoadImage("BG/charSelection4.jpg");

	// Load Back button image and define its properties
	backButtonImage = iLoadImage("UiElements/backButton.png");
	backButton.x = 50;
	backButton.y = 50;
	backButton.width = 195;
	backButton.height = 75;

	optionButtonImages[0] = iLoadImage("BG/modeSelection1.png");
	optionButtonImages[1] = iLoadImage("BG/modeSelection2.jpg");

	// two option buttons
	int optWidth = 600;
	int optHeight = 400;
	int spacing = 100;
	int totalWidth = (optWidth * 2) + spacing;
	int startX = (SCREEN_WIDTH - totalWidth) / 2;

	optionButtons[0].x = startX;
	optionButtons[0].y = (SCREEN_HEIGHT - optHeight) / 2;
	optionButtons[0].width = optWidth;
	optionButtons[0].height = optHeight;

	optionButtons[1].x = startX + optWidth + spacing;
	optionButtons[1].y = (SCREEN_HEIGHT - optHeight) / 2;
	optionButtons[1].width = optWidth;
	optionButtons[1].height = optHeight;

	// main menu buttons
	char imagePath[100];
	int startY = 525;
	for (int i = 0; i < BUTTON_COUNT; i++) {
		sprintf_s(imagePath, sizeof(imagePath), "UiElements/button%d.png", i + 1);
		buttonImages[i] = iLoadImage(imagePath);

		buttons[i].x = 100;
		buttons[i].y = startY - i * 140;
		buttons[i].width = 300;
		buttons[i].height = 117;
	}

}
void loadCharacterAndArenaImages() {
	// Load character images
	for (int i = 0; i < CHARACTER_COUNT; i++) {
		char imagePath[100];
		sprintf_s(imagePath, sizeof(imagePath), "UiElements/Characters/char%d.jpg", i + 1);
		characterImages[i] = iLoadImage(imagePath);
	}
	// Load selection images
	for (int i = 0; i < 2; i++) {
		char imagePath[100];
		sprintf_s(imagePath, sizeof(imagePath), "UiElements/characterSelection%d.png", i + 1);
		selectionImages[i] = iLoadImage(imagePath);
	}
}
void loadCharacterSelectionAssets() {
	// Loading Character and Arena Images
	loadCharacterAndArenaImages();

	// Position settings of characters
	int charWidth = 100, charHeight = 120;
	int spacingX = 80, spacingY = 50;
	int cols = 2, rows = 3;

	// total width and height of the grid
	int totalWidth = 250;
	int totalHeight = 420;

	// Starting positions to center the grid vertically
	int startY = (SCREEN_HEIGHT - totalHeight) / 2;
	int startX_P1 = 170; // Player 1's grid starts 150px from the left
	int startX_P2 = SCREEN_WIDTH - 180 - totalWidth; // Player 2's grid starts 150px from right

	for (int i = 0; i < CHARACTER_COUNT; i++) {
		int row = i / cols;
		int col = i % cols;

		// Player 1 (Left Side)
		characterButtonsP1[i].x = startX_P1 + col * (charWidth + spacingX);
		characterButtonsP1[i].y = startY + (rows - 1 - row) * (charHeight + spacingY);
		characterButtonsP1[i].width = charWidth;
		characterButtonsP1[i].height = charHeight;

		// Player 2 (Right Side)
		characterButtonsP2[i].x = startX_P2 + col * (charWidth + spacingX);
		characterButtonsP2[i].y = characterButtonsP1[i].y; // Align with P1
		characterButtonsP2[i].width = charWidth;
		characterButtonsP2[i].height = charHeight;
	}
}
void loadCharacterSelectionAssetsForArcade() {
	// Loading Character and Arena Images
	loadCharacterAndArenaImages();

	// Position settings of characters
	int charWidth = 130, charHeight = 150;
	int spacingX = 80, spacingY = 50;
	int cols = 3, rows = 2;

	// total width and height of the grid
	int totalWidth = 400;
	int totalHeight = 310;

	// Starting positions to center the grid vertically
	int startY = (SCREEN_HEIGHT - totalHeight) / 2;
	int startX = 370;

	for (int i = 0; i < CHARACTER_COUNT; i++){
		int row = i / cols;
		int col = i % cols;

		characterButtons[i].x = startX + col* (charWidth + spacingX);
		characterButtons[i].y = startY + (rows - 1 - row) * (charHeight + spacingY);
		characterButtons[i].width = charWidth;
		characterButtons[i].height = charHeight;
	}
}

void loadArenaAssets(){
	// Load Arena Images
	arenaImages[0] = iLoadImage("BG/arena1.jpg");
}
void showArenaImages(){
	//stopMainMenuMusic();  // stoping main menu music
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, arenaImages[0]);
	//startArenaMusic(); // Starting Arena Music
}


int bgfix;
// works on arcade mode dynamic BG
void loadDynamicArenaBG(){
	dynamicBgImage = iLoadImage("BG/arena2.jpg");
}

void showDynamicArenaBG(){
	iShowImage(bgX1, 30, SCREEN_WIDTH, SCREEN_HEIGHT, dynamicBgImage);
	iShowImage(bgX2, 30, SCREEN_WIDTH, SCREEN_HEIGHT, dynamicBgImage);

}


void updateBackgroundScroll(Character &player) {
	static int lastPlayerX = player.moveX;

	// detect movement
	int deltaX = player.moveX - lastPlayerX;

	// Only scroll when character moving
	if (deltaX != 0) {
		if (deltaX > 0) {
			bgX1 -= bgScrollSpeed;
			bgX2 -= bgScrollSpeed;

			// Reset positions when off-screen
			if (bgX1 <= -SCREEN_WIDTH) {
				bgX1 = bgX2 + SCREEN_WIDTH - 20;
			}
			if (bgX2 <= -SCREEN_WIDTH) {
				bgX2 = bgX1 + SCREEN_WIDTH - 20;
			}
		}
		else if (deltaX < 0) {
			bgX1 += bgScrollSpeed;
			bgX2 += bgScrollSpeed;

			// Reset positions when off-screen
			if (bgX1 >= SCREEN_WIDTH) {
				bgX1 = bgX2 - SCREEN_WIDTH + 20;
			}
			if (bgX2 >= SCREEN_WIDTH) {
				bgX2 = bgX1 - SCREEN_WIDTH + 20;
			}
		}
	}

	lastPlayerX = player.moveX;
}



#endif 
#ifndef MUSIC_HPP
#define MUSIC_HPP

#include <windows.h>
#include <mmsystem.h>

static bool mainMenuMusicLoaded = false;
static bool mainMenuMusicPlaying = false;

static bool arenaMusicLoaded = false;
static bool arenaMusicPlaying = false;

// Main Menu Music
void startMainMenuMusic() {
	if (!mainMenuMusicLoaded) {
		mciSendString("open \"Audios//mainMenuMusic.mp3\" alias mainMenu", nullptr, 0, nullptr);
		mainMenuMusicLoaded = true;
	}
	if (!mainMenuMusicPlaying) {
		mciSendString("play mainMenu repeat", nullptr, 0, nullptr);
		mainMenuMusicPlaying = true;
	}
}

void stopMainMenuMusic() {
	if (mainMenuMusicPlaying) {
		mciSendString("stop mainMenu", nullptr, 0, nullptr);
		mciSendString("close mainMenu", nullptr, 0, nullptr);
		mainMenuMusicPlaying = false;
		mainMenuMusicLoaded = false;
	}
}

// Arena Music
void startArenaMusic() {
	if (!arenaMusicLoaded) {
		mciSendString("open \"Audios//arenaMusic.mp3\" alias arenaMusic", nullptr, 0, nullptr);
		arenaMusicLoaded = true;
	}
	if (!arenaMusicPlaying) {
		mciSendString("play arenaMusic repeat", nullptr, 0, nullptr);
		arenaMusicPlaying = true;
	}	
}

void stopArenaMusic() {
	if (arenaMusicPlaying) {
		mciSendString("stop arenaMusic", nullptr, 0, nullptr);
		mciSendString("close arenaMusic", nullptr, 0, nullptr);
		arenaMusicPlaying = false;
		arenaMusicLoaded = false;
	}
}

// Character Click sound
void playCharClickSound() {
	static bool musicLoaded = false;

	if (musicLoaded == false) { // Loading music everytime for multiple click
		mciSendString("open \"Audios//charClickSound.mp3\" alias charClick", nullptr, 0, nullptr);
		musicLoaded = true;
	}

	mciSendString("play charClick from 0", nullptr, 0, nullptr);
}

// Click sound
void playClickSound() {
	static bool musicLoaded = false;

	if (musicLoaded == false) { // Loading music everytime for multiple click
		mciSendString("open \"Audios//click.mp3\" alias click", nullptr, 0, nullptr);
		musicLoaded = true;
	}

	mciSendString("play click from 230", nullptr, 0, nullptr);
}

#endif MUSIC_HPP

#ifndef SCOREBOARD_HPP
#define SCOREBOARD_HPP

#include <stdio.h>
#include <stdlib.h> 
#include "menu.hpp"

#define MAX_SCORES 100

int scoreboardBackground;
int sortedScores[MAX_SCORES];
int scoreCount = 0;


// Reads all scores
void readAndSortScores() {
	FILE* scoreFile;
	fopen_s(&scoreFile, "arcade_scores.txt", "r");

	if (scoreFile != NULL) {
		for (scoreCount = 0; scoreCount < MAX_SCORES && fscanf_s(scoreFile, "%d", &sortedScores[scoreCount]) != EOF; scoreCount++) {
		}
		fclose(scoreFile);

		//Bubble Sort.
		for (int i = 0; i < scoreCount - 1; i++) {
			for (int j = 0; j < scoreCount - i - 1; j++) {
				if (sortedScores[j] < sortedScores[j + 1]) {
					int temp = sortedScores[j];
					sortedScores[j] = sortedScores[j + 1];
					sortedScores[j + 1] = temp;
				}
			}
		}

		/*

		for (int i = 0; i < scoreCount - 1; i++) {
		for (int j = 0; j < scoreCount - i - 1; j++) {
		if (sortedScores[j] < sortedScores[j + 1]) {
		int temp = sortedScores[j];
		sortedScores[j] = sortedScores[j + 1];
		sortedScores[j + 1] = temp;
		}
		}
		}

		*/
	}
}

void loadScoreboardAssets() {
	scoreboardBackground = iLoadImage("BG/playerSelectionBg.png");
}

void showScoreboardScreen() {
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, scoreboardBackground);
	iSetColor(255, 215, 0);
	iText(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 100, "High Scores", GLUT_BITMAP_TIMES_ROMAN_24);

	if (scoreCount > 0) {
		//int displayCount = (scoreCount < 10) ? scoreCount : 10; 

		for (int i = 0; i < 10; i++) {
			char scoreText[50];
			//int minutes = sortedScores[i] / 60;
			//int seconds = sortedScores[i] % 60;
			//sprintf_s(scoreText, sizeof(scoreText), "%d.  %02d:%02d", i + 1, minutes, seconds);

			int score = sortedScores[i] % 60;
			sprintf_s(scoreText, sizeof(scoreText), "%d.  %d", i + 1, score * 4);

			iSetColor(255, 255, 255);
			iText(SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT - 180 - (i * 40), scoreText, GLUT_BITMAP_HELVETICA_18);
		}
	}
	else {
		iSetColor(255, 100, 100);
		iText(SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT / 2, "No Scores Yet!", GLUT_BITMAP_TIMES_ROMAN_24);
	}

	//back button 
	iShowImage(backButton.x, backButton.y, backButton.width, backButton.height, backButtonImage);
}

void handleScoreboardClick(int mx, int my) {
	if (mx >= backButton.x && mx <= backButton.x + backButton.width &&
		my >= backButton.y && my <= backButton.y + backButton.height) {
		currentScreen = 40;
		playClickSound();
	}
}

#endif

#include "iGraphics.h"
#include "menu.hpp"
#include "character.hpp"
#include "pause.hpp"
#include "ai.hpp"
#include "arcade.hpp" 
#include "combat.hpp"
#include "arcadeAction.hpp"
#include "arcadeEndScreen.hpp"
#include "scoreboard.hpp" 

Character captainAmericaP1;
Character captainAmericaP2;
Character thorP1;
Character thorP2;

int loadingScreen;

char playerName[100] = "Player"; 
int nameInputMode = 0; 
bool playerWonArcade = false; 

int arcadeTimerSeconds = 0;
bool isArcadeTimerRunning = false;

// Loading screen variables
int loadingBarWidth = 0;
bool loadingDone = false;
bool goToMainMenu = false;
bool blinkTextWhite = true;
static bool assetsLoaded = false;

int previousScreen = -1; 

// had to declare the function definition here otherwise it was not working 
void loadingScreenText();


void iDraw()
{
	iClear();

	if (!goToMainMenu)
	{
		// Show loading screen
		iShowImage(0, 10, SCREEN_WIDTH, SCREEN_HEIGHT, loadingScreen);

		startMainMenuMusic(); // Starting Main Menu Music

		// Loading bar frame and fill
		iSetColor(255, 60, 60);
		iRectangle(390, 35, 500, 25);
		iFilledRectangle(390, 35, loadingBarWidth, 25);
		loadingScreenText();
	}
	else
	{
		// Show the actual menu screen after loading
		showMenu();

		// On the gameplay screen, render Character
		if (currentScreen == 20){
			showArenaImages();

			// For Captain America
			if (selectedCharacterIndexP1 == 2) captainAmericaP1.draw();
			if (selectedCharacterIndexP2 == 2) captainAmericaP2.draw();

			// For Thor
			if (selectedCharacterIndexP1 == 3) thorP1.draw();
			if (selectedCharacterIndexP2 == 3) thorP2.draw();

			if (selectedCharacterIndexP1 == 2 && selectedCharacterIndexP2 == 2) {
				drawBothHealthBars(captainAmericaP1, captainAmericaP2);
			}
			else if (selectedCharacterIndexP1 == 3 && selectedCharacterIndexP2 == 3) {
				drawBothHealthBars(thorP1, thorP2);
			}
			else if (selectedCharacterIndexP1 == 2 && selectedCharacterIndexP2 == 3) {
				drawBothHealthBars(captainAmericaP1, thorP2);
			}
			else if (selectedCharacterIndexP1 == 3 && selectedCharacterIndexP2 == 2) {
				drawBothHealthBars(thorP1, captainAmericaP2);
			}
			// Add this after drawBothHealthBars calls in screen 20
			// Show match end timer
			// Show match end timer - clean text only
			if (showMatchEndTimer) {
				iSetColor(0, 0, 0);
				iText(SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT / 2 + 20, "MATCH ENDS IN", GLUT_BITMAP_HELVETICA_18);
				char countdown[5];
				sprintf_s(countdown, sizeof(countdown), "%d", matchEndCountdown);
				iText(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 - 20, countdown, GLUT_BITMAP_HELVETICA_18);
			}
			iShowImage(pauseButton.x, pauseButton.y, pauseButton.width, pauseButton.height, pauseButtonImage);

			// If the game is paused, show the pause menu
			if (currentGameState == PAUSED) {
				showPauseMenu();
			}
		}
		else if (currentScreen == 25) { // Victory Screen
			showVictoryScreen();
		}
		else if (currentScreen == 30){

			showDynamicArenaBG();

			//health bars

			if (selectedCharacterIndexP1 == 2) {
				drawHealthBars(captainAmericaP1, arcadeAI);
				captainAmericaP1.draw();

			}
			if (selectedCharacterIndexP1 == 3){
				drawHealthBars(thorP1, arcadeAI);
				thorP1.draw();

			}

			drawArcadeAI();

			//timer
			iSetColor(255, 255, 255);
			char timerText[10];
			int minutes = arcadeTimerSeconds / 60;
			int seconds = arcadeTimerSeconds % 60;
			sprintf_s(timerText, sizeof(timerText), "%02d:%02d", minutes, seconds);
			iText(SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT - 60, timerText, GLUT_BITMAP_TIMES_ROMAN_24);

			// paused button
			iShowImage(pauseButton.x, pauseButton.y, pauseButton.width, pauseButton.height, pauseButtonImage);

			// If the game is paused, show the pause menu
			if (currentGameState == PAUSED) {
				showPauseMenu();
			}
		}
		else if (currentScreen == 40) { // Arcade End Screen
			showArcadeEndScreen(playerWonArcade);
		}
		else if (currentScreen == 50) {
			showScoreboardScreen();
		}
	}
}

void iMouseMove(int mx, int my)
{

}

void iPassiveMouseMove(int mx, int my)
{
	if (goToMainMenu) {
		// If in game and paused, handle pause menu hover
		if ((currentScreen == 20 || currentScreen == 30) && currentGameState == PAUSED) {
			handlePauseHoverAnimation(mx, my);
		}
		else if (currentScreen == 40) {
			handleArcadeEndScreenHover(mx, my);
		}
		else if (currentScreen == 25) {
			handleVictoryScreenHover(mx, my);
		}
		else {
			// Handle main menu hover Animation
			handleHoverAnimation(mx, my);
		}
	}
}


void iMouse(int button, int state, int mx, int my)
{
	if (goToMainMenu) {
		if (currentScreen == 20) { // Clicks inside the 1v1 arena screen
			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
				if (currentGameState == PAUSED) {
					// If paused, check for clicks on the pause menu
					int clickedButtonIndex = handlePauseMenuClick(mx, my);
					if (clickedButtonIndex == 0) { // Resume
						currentGameState = PLAYING;
					}
					else if (clickedButtonIndex == 1) { // Restart
						if (selectedCharacterIndexP1 == 2 && selectedCharacterIndexP2 == 2) {
							resetCombatSystem(captainAmericaP1, captainAmericaP2);
						}
						else if (selectedCharacterIndexP1 == 3 && selectedCharacterIndexP2 == 3) {
							resetCombatSystem(thorP1, thorP2);
						}
						else if (selectedCharacterIndexP1 == 2 && selectedCharacterIndexP2 == 3) {
							resetCombatSystem(captainAmericaP1, thorP2);
						}
						else if (selectedCharacterIndexP1 == 3 && selectedCharacterIndexP2 == 2) {
							resetCombatSystem(thorP1, captainAmericaP2);
						}
						currentGameState = PLAYING;
					}
					else if (clickedButtonIndex == 2) { // Character Selection
						if (selectedCharacterIndexP1 == 2 && selectedCharacterIndexP2 == 2) {
							resetCombatSystem(captainAmericaP1, captainAmericaP2);
						}
						else if (selectedCharacterIndexP1 == 3 && selectedCharacterIndexP2 == 3) {
							resetCombatSystem(thorP1, thorP2);
						}
						else if (selectedCharacterIndexP1 == 2 && selectedCharacterIndexP2 == 3) {
							resetCombatSystem(captainAmericaP1, thorP2);
						}
						else if (selectedCharacterIndexP1 == 3 && selectedCharacterIndexP2 == 2) {
							resetCombatSystem(thorP1, captainAmericaP2);
						}
						currentScreen = 10; // Go back to character selection
						currentGameState = PLAYING; // Reset state for next time
					}
				}
				else {
					// If playing, check for clicks on the main pause button
					if (mx >= pauseButton.x && mx <= pauseButton.x + pauseButton.width &&
						my >= pauseButton.y && my <= pauseButton.y + pauseButton.height) {
						currentGameState = PAUSED;
					}
				}
			}
		}
		// Add this after the currentScreen == 20 block in iMouse
		else if (currentScreen == 25) { // Victory Screen
			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
				int clickedButtonIndex = handleVictoryScreenClick(mx, my);
				if (clickedButtonIndex == 0) { // Retry
					// Reset all characters based on selection
					if (selectedCharacterIndexP1 == 2 && selectedCharacterIndexP2 == 2) {
						resetCombatSystem(captainAmericaP1, captainAmericaP2);
					}
					else if (selectedCharacterIndexP1 == 3 && selectedCharacterIndexP2 == 3) {
						resetCombatSystem(thorP1, thorP2);
					}
					else if (selectedCharacterIndexP1 == 2 && selectedCharacterIndexP2 == 3) {
						resetCombatSystem(captainAmericaP1, thorP2);
					}
					else if (selectedCharacterIndexP1 == 3 && selectedCharacterIndexP2 == 2) {
						resetCombatSystem(thorP1, captainAmericaP2);
					}
					currentScreen = 20; // Go back to game
				}
				else if (clickedButtonIndex == 1) { // Exit
					currentScreen = 10; //go to character selection
				}
			}
		}
		// Clicks inside the Arcade Mode arena screen
		else if (currentScreen == 30) {
			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
				if (currentGameState == PAUSED) {
					// If paused, check for clicks on the pause menu
					int clickedButtonIndex = handlePauseMenuClick(mx, my);
					if (clickedButtonIndex == 0) { // Resume
						currentGameState = PLAYING;
					}
					else if (clickedButtonIndex == 1) { // Restart
						resetArcadePlayer(captainAmericaP1, arcadeAI);
						currentGameState = PLAYING;
						arcadeTimerSeconds = 0;
						isArcadeTimerRunning = true;
					}
					else if (clickedButtonIndex == 2) { // Character Selection
						resetArcadePlayer(captainAmericaP1, arcadeAI);
						currentScreen = 11; // Go back to arcade character selection
						currentGameState = PLAYING; // Reset state for next time
						isArcadeTimerRunning = false;
					}
				}
				else {
					// If playing, check for clicks on the main pause button
					if (mx >= pauseButton.x && mx <= pauseButton.x + pauseButton.width &&
						my >= pauseButton.y && my <= pauseButton.y + pauseButton.height) {
						currentGameState = PAUSED;
					}
				}
			}
		}
		// end screen arcade
		else if (currentScreen == 40) { 
			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
				int clickedButtonIndex = handleArcadeEndScreenClick(mx, my);
				if (clickedButtonIndex == 0 || clickedButtonIndex == 1) {
					
				}
				if (clickedButtonIndex == 0) { // Restart
					resetArcadePlayer(captainAmericaP1, arcadeAI);
					resetArcadePlayer(thorP1, arcadeAI);
					currentScreen = 30; 
					arcadeTimerSeconds = 0;
					isArcadeTimerRunning = true;
				}
				else if (clickedButtonIndex == 1) { // Main Menu
					resetArcadePlayer(captainAmericaP1, arcadeAI);
					resetArcadePlayer(thorP1, arcadeAI);
					currentScreen = -1; 
					isArcadeTimerRunning = false;
				}
				else if (clickedButtonIndex == 2) { // High Scores
					readAndSortScores(); // Read the latest scores 
					currentScreen = 50;
				}
			}
		}
		//Handle clicks on the scoreboard screen
		else if (currentScreen == 50) {
			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
				handleScoreboardClick(mx, my);
			}
		}
		else {
			// Handle menu clicks only after the loading screen
			handleMenuClick(button, state, mx, my);
		}
	}
}

// This function runs at a fixed interval
// Special Keys:
// GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6, GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12, 
// GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP, GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT

void fixedUpdate() {
	if (!goToMainMenu || currentGameState == PAUSED) return;

	// Versus Mode (screen 20) - Both players
	if (currentScreen == 20) {
		// Player 1 controls (WASD + FEQ)
		handleInputMovement(captainAmericaP1, isKeyPressed('d'), isKeyPressed('a'));
		handleDefaultState(captainAmericaP1, isKeyPressed('d'), isKeyPressed('a'));
		handleJump(captainAmericaP1, isKeyPressed('w'));
		handleAttack(captainAmericaP1, isKeyPressed('f'));
		handleUltimate(captainAmericaP1, isKeyPressed('q'));

		// Player 1 controls for Thor(WASD + FEQ)
		handleInputMovement(thorP1, isKeyPressed('d'), isKeyPressed('a'));
		handleDefaultState(thorP1, isKeyPressed('d'), isKeyPressed('a'));
		handleJump(thorP1, isKeyPressed('w'));
		handleAttack(thorP1, isKeyPressed('f'));

		// Player 2 controls (Arrow keys + )
		handleInputMovement(captainAmericaP2, isSpecialKeyPressed(GLUT_KEY_RIGHT), isSpecialKeyPressed(GLUT_KEY_LEFT));
		handleDefaultState(captainAmericaP2, isSpecialKeyPressed(GLUT_KEY_RIGHT), isSpecialKeyPressed(GLUT_KEY_LEFT));
		handleJump(captainAmericaP2, isSpecialKeyPressed(GLUT_KEY_UP));
		handleAttack(captainAmericaP2, isSpecialKeyPressed(GLUT_KEY_DOWN));
		handleUltimate(captainAmericaP2, isKeyPressed('0'));

		// Player 2 controls for Thor(Arrow keys + )
		handleInputMovement(thorP2, isSpecialKeyPressed(GLUT_KEY_RIGHT), isSpecialKeyPressed(GLUT_KEY_LEFT));
		handleDefaultState(thorP2, isSpecialKeyPressed(GLUT_KEY_RIGHT), isSpecialKeyPressed(GLUT_KEY_LEFT));
		handleJump(thorP2, isSpecialKeyPressed(GLUT_KEY_UP));
		handleAttack(thorP2, isSpecialKeyPressed(GLUT_KEY_DOWN));

		if (selectedCharacterIndexP1 == 2 && selectedCharacterIndexP2 == 2) {
			processCombatSystem(captainAmericaP1, captainAmericaP2);
		}
		else if (selectedCharacterIndexP1 == 3 && selectedCharacterIndexP2 == 3) {
			processCombatSystem(thorP1, thorP2);
		}
		else if (selectedCharacterIndexP1 == 2 && selectedCharacterIndexP2 == 3) {
			processCombatSystem(captainAmericaP1, thorP2);
		}
		else if (selectedCharacterIndexP1 == 3 && selectedCharacterIndexP2 == 2) {
			processCombatSystem(thorP1, captainAmericaP2);
		}

	}

	// Arcade Mode (screen 30) - Only Player 1
	else if (currentScreen == 30) {
		handleInputMovement(captainAmericaP1, isKeyPressed('d'), isKeyPressed('a'));
		handleDefaultState(captainAmericaP1, isKeyPressed('d'), isKeyPressed('a'));
		handleJump(captainAmericaP1, isKeyPressed('w'));
		handleAttack(captainAmericaP1, isKeyPressed('f'));
		handleUltimate(captainAmericaP1, isKeyPressed('q'));

		updateBackgroundScroll(captainAmericaP1);

		handleInputMovement(thorP1, isKeyPressed('d'), isKeyPressed('a'));
		handleDefaultState(thorP1, isKeyPressed('d'), isKeyPressed('a'));
		handleJump(thorP1, isKeyPressed('w'));
		handleAttack(thorP1, isKeyPressed('f'));

		updateBackgroundScroll(thorP1);

		// AI Controls
		handleJump(arcadeAI, false);

		//collision  function added

		if (selectedCharacterIndexP1 == 2) {
			handleAI(arcadeAI, captainAmericaP1);
			checkCollisionAndApplyDamage(captainAmericaP1, arcadeAI); // Player attacks AI
			checkCollisionAndApplyDamage(arcadeAI, captainAmericaP1); // AI attacks Player
			checkGameOver(captainAmericaP1, arcadeAI);
		}
		if (selectedCharacterIndexP1 == 3) {
			handleAI(arcadeAI, thorP1);
			checkCollisionAndApplyDamage(thorP1, arcadeAI); // Player attacks AI
			checkCollisionAndApplyDamage(arcadeAI, thorP1); // AI attacks Player
			checkGameOver(thorP1, arcadeAI);
		}
	}
}

void iKeyboard(unsigned char key) {
	if (goToMainMenu && currentScreen == 20) {
		if (selectedCharacterIndexP1 == 2 && selectedCharacterIndexP2 == 2) {
			processCombatInput(key, captainAmericaP1, captainAmericaP2);
		}
		else if (selectedCharacterIndexP1 == 3 && selectedCharacterIndexP2 == 3) {
			processCombatInput(key, thorP1, thorP2);
		}
		else if (selectedCharacterIndexP1 == 2 && selectedCharacterIndexP2 == 3) {
			processCombatInput(key, captainAmericaP1, thorP2);
		}
		else if (selectedCharacterIndexP1 == 3 && selectedCharacterIndexP2 == 2) {
			processCombatInput(key, thorP1, captainAmericaP2);
		}
	}

	
	
}

// Handles the loading bar animation on loading screen
void loadingBar(){
	if (!loadingDone){
		loadingBarWidth += 15;
		if (loadingBarWidth >= 500){
			loadingBarWidth = 500;
			loadingDone = true;
		}
	}

	else{
		if (isKeyPressed(' ')){
			goToMainMenu = true;
			// Load menu 
			if (mainMenuScreen == -1){
				loadMenuAssets();
				loadCharacterSelectionAssets();
				loadCharacterSelectionAssetsForArcade();
				loadArenaAssets();
				loadPauseAssets();
				loadDynamicArenaBG();
				loadArcadeEndScreenAssets();
				loadScoreboardAssets();
				loadVictoryScreenAssets();
			}
		}
	}
}

void loadingScreenText(){
	// If loading done, show text prompt
	if (loadingDone){
		if (blinkTextWhite) {
			iSetColor(255, 255, 255); // white
		}
		else {
			iSetColor(0, 0, 0); // black
		}
		iText(530, 80, "Press SPACE to continue", GLUT_BITMAP_HELVETICA_18);
	}
	else{
		iSetColor(255, 255, 255);
		iText(390, 70, "Loading...", GLUT_BITMAP_HELVETICA_18);
	}
}

void blinkText() {
	blinkTextWhite = !blinkTextWhite; 
}

void blinkVictoryText() {
	if (currentScreen == 25) { // Only blink when on victory screen
		victoryTextBlink = !victoryTextBlink;
	}
}

//time count 
void updateArcadeTimer() {
	if (isArcadeTimerRunning && currentGameState == PLAYING) {
		arcadeTimerSeconds++;
	}
}

void updateCharacters() {
	// Update character animations if the game is playing OR if character is dead (to show death animation)
	if (currentScreen == 20 && (currentGameState == PLAYING ||
		captainAmericaP1.characterState == DEAD || captainAmericaP2.characterState == DEAD ||
		thorP1.characterState == DEAD || thorP2.characterState == DEAD)){
		if (selectedCharacterIndexP1 == 2) captainAmericaP1.update();
		if (selectedCharacterIndexP2 == 2) captainAmericaP2.update();
		// For Thor
		if (selectedCharacterIndexP1 == 3) thorP1.update();
		if (selectedCharacterIndexP2 == 3) thorP2.update();
		// ironMan.update();
	}

	else if (currentScreen == 30 && currentGameState == PLAYING){
		if (selectedCharacterIndexP1 == 2) captainAmericaP1.update();
		if (selectedCharacterIndexP1 == 3) thorP1.update();
		updateArcadeCharacters();
	}
}

int main()
{
	// Initialize graphics window
	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Marvel Mayhem");

	//loading screen background image
	loadingScreen = iLoadImage("BG/loading3.png");

	loadArcadeCharacters();
	loadScoreboardAssets();
	loadCaptainAmerica(captainAmericaP1);
	loadCaptainAmerica(captainAmericaP2);

	loadThor(thorP1);
	loadThor(thorP2);

	captainAmericaP1.moveX = 200;        // Player 1 starts on the left
	captainAmericaP1.facingRight = true; // Player 1 faces right

	captainAmericaP2.moveX = 980;         // Player 2 starts on the right  
	captainAmericaP2.facingRight = false; // Player 2 faces left

	thorP1.moveX = 200;        // Player 1 starts on the left
	thorP1.moveY = 118;
	thorP2.baseY = 118;
	thorP1.facingRight = true; // Player 1 faces right

	thorP2.moveX = 980;         // Player 2 starts on the right  
	thorP2.moveY = 118;
	thorP2.baseY = 118;
	thorP2.facingRight = false; // Player 2 faces left

	iSetTimer(100, loadingBar);

	iSetTimer(16, fixedUpdate);

	iSetTimer(100, updateCharacters);

	iSetTimer(1000, updateArcadeTimer); //arcadetimer 

	iSetTimer(100, loadArenaAssets);

	iSetTimer(1000, updateMatchEndTimer); // 1 second timer for countdown

	iSetTimer(300, blinkText); // Blink every 500ms (half second)

	iSetTimer(300, blinkVictoryText); // Add this line with your other timers

	//iSetTimer(100, loadDynamicArenaBG);

	iStart(); // Start the graphics engine

	return 0;
}



/*
void fixedUpdate() {

if (!goToMainMenu || currentScreen != 10) return;

static bool jumpInProgress = false;

bool wPressed = isKeyPressed('w');
bool moveRight = isKeyPressed('d');
bool moveLeft = isKeyPressed('a');

// ? Move left
if (moveLeft) {
captainAmerica.facingRight = false;
captainAmerica.moveX -= 5;
}

// ? Move right
if (moveRight) {
captainAmerica.facingRight = true;
captainAmerica.moveX += 5;
}

// Start jump if W is pressed and not already jumping
if (wPressed && !jumpInProgress) {
captainAmerica.setState(JUMP);
captainAmerica.currentFrame = 0;
jumpInProgress = true;
}

// Handle jump animation movement
if (jumpInProgress){
int f = captainAmerica.currentFrame;

// Y axis arc movement (fake)
if (f == 0) captainAmerica.moveY += 12;
else if (f == 1) captainAmerica.moveY += 8;
else if (f == 2) captainAmerica.moveY += 5;
else if (f == 3) captainAmerica.moveY -= 5;
else if (f == 4) captainAmerica.moveY -= 8;
else if (f == 5) captainAmerica.moveY -= 12;

// When jump ends
int maxJump = captainAmerica.facingRight ? captainAmerica.jumpCount_R : captainAmerica.jumpCount_L;
if (captainAmerica.currentFrame >= maxJump - 1) {
captainAmerica.moveY = captainAmerica.baseY; // go back to ground
jumpInProgress = false;
}
}
// If not jumping, pick proper state
if (!jumpInProgress && captainAmerica.characterState != ATTACK) {
if (moveRight || moveLeft) captainAmerica.setState(WALK);
else captainAmerica.setState(IDLE);
// Safety check  never below ground
if (captainAmerica.moveY < captainAmerica.baseY) {
captainAmerica.moveY = captainAmerica.baseY;
}
}

if (captainAmerica.characterState == ATTACK) {
int maxAtk = captainAmerica.facingRight ? captainAmerica.attackCount_R : captainAmerica.attackCount_L;

if (captainAmerica.currentFrame >= maxAtk - 1) {
if (isKeyPressed('f')) {
captainAmerica.currentFrame = 0; // loop attack
}
else {
captainAmerica.setState(IDLE); // go back to idle
}
}
}
else if (isKeyPressed('f')) {
captainAmerica.setState(ATTACK);
}
if (jumpInProgress && isKeyPressed('f')) {
captainAmerica.setState(ATTACK); // mid-air attack
}


}
*/






