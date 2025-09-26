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

void playAttackSound() {
	static bool musicLoaded = false;
	if (musicLoaded == false) {
		mciSendString("open \"Audios//attack.mp3\" alias attack", nullptr, 0, nullptr);
		musicLoaded = true;
	}
	mciSendString("play attack from 0", nullptr, 0, nullptr);
}

void stopAttackSound() {
	mciSendString("stop attack", nullptr, 0, nullptr);
}

#endif MUSIC_HPP