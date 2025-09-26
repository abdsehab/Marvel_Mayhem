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

	if (scoreFile != NULL){
		scoreCount = 0;
		while (scoreCount < MAX_SCORES && fscanf_s(scoreFile, "%d", &sortedScores[scoreCount]) == 1) {
			scoreCount++;
		}
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
		if (sortedScores[j] > sortedScores[j + 1]) {
		int temp = sortedScores[j];
		sortedScores[j] = sortedScores[j + 1];
		sortedScores[j + 1] = temp;
		}
		}
		}

		*/
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