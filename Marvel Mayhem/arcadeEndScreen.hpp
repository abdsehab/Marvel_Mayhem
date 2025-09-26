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
		iShowImage(endScreenButtons[i].x, endScreenButtons[i].y - 30, endScreenButtons[i].width, endScreenButtons[i].height, endScreenButtonImage);
		// Button Text
		iSetColor(255, 255, 255);
		iText(endScreenButtons[i].x + labelXOffsets[i], endScreenButtons[i].y , buttonLabels[i], GLUT_BITMAP_HELVETICA_18);
	}

	// hover zoom effect
	if (hoveredEndScreenButtonIndex != -1) {
		int i = hoveredEndScreenButtonIndex;
		double zoomFactor = 1.1; 

		int newWidth = endScreenButtons[i].width * zoomFactor;
		int newHeight = endScreenButtons[i].height * zoomFactor;
		int newX = endScreenButtons[i].x - (newWidth - endScreenButtons[i].width) / 2;
		int newY = (endScreenButtons[i].y -30) - (newHeight - endScreenButtons[i].height) / 2;

		iShowImage(newX, newY, newWidth, newHeight, endScreenButtonImage);
		//iSetColor(255, 255, 0); // zoom howar por color
		iText(endScreenButtons[i].x + labelXOffsets[i], endScreenButtons[i].y, buttonLabels[i], GLUT_BITMAP_HELVETICA_18);
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


