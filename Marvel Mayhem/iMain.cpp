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
		handleUltimate(thorP1, isKeyPressed('q'));

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
		handleUltimate(thorP2, isKeyPressed('0'));

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
		handleUltimate(thorP1, isKeyPressed('q'));

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
