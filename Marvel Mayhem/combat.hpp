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
bool p1AttackSoundPlaying = false;
bool p2AttackSoundPlaying = false;

// Modified handleCombatCollisions function - replace your existing one
void handleCombatCollisions(Character& player1, Character& player2) {

	// Player 1 attacking Player 2
	if (player1.characterState == ATTACK && player2.alive) {
		int hitFrame = (player1.facingRight ? player1.attackCount_R : player1.attackCount_L) / 2;

		// Start playing attack sound if not already playing
		if (!p1AttackSoundPlaying) {
			playAttackSound();
			p1AttackSoundPlaying = true;
		}

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
		// Stop attack sound when exiting ATTACK state
		if (p1AttackSoundPlaying) {
			stopAttackSound();
			p1AttackSoundPlaying = false;
		}
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

		// Start playing attack sound if not already playing
		if (!p2AttackSoundPlaying) {
			playAttackSound();
			p2AttackSoundPlaying = true;
		}

		// Reset hit flag when animation restarts (frame 0)
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
		// Stop attack sound when exiting ATTACK state
		if (p2AttackSoundPlaying) {
			stopAttackSound();
			p2AttackSoundPlaying = false;
		}
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

	// Stop any playing attack sounds and reset sound flags
	if (p1AttackSoundPlaying) {
		stopAttackSound();
		p1AttackSoundPlaying = false;
	}
	if (p2AttackSoundPlaying) {
		stopAttackSound();
		p2AttackSoundPlaying = false;
	}

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