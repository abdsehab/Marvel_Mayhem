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
