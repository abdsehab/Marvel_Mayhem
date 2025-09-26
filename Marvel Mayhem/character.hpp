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

	loadSprites(thor.ultimateSprites_R, thor.ultimateCount_R, "SPRITE/Thor/ULTIMATE_R", "ult", 10);
	loadSprites(thor.ultimateSprites_L, thor.ultimateCount_L, "SPRITE/Thor/ULTIMATE_L", "ult", 10);

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








