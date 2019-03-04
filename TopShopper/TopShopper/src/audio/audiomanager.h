#ifndef AUDIOMANAGER_H_
#define AUDIOMANAGER_H_

#include <SDL.h>
#include <SDL_mixer.h>
#include <map>
#include <vector>
#include <string>
#undef main

class Broker;

enum SoundEffectTypes {
	ROLL_SOUND,
	HITWALL_SOUND,
	PICKITEM_SOUND,
	DROPITEM_SOUND,
	TURBO_SOUND
};

class AudioManager {
public:
	struct SoundEffect {
		std::string filename;
		Mix_Chunk* sfx;
		//map<string, Mix_Chunk*> mySFX; // <full path, acutal chunk>
		Uint8 distance = 200;  // 0 at your loaction
		Sint16 angle = 0;  // 0 at front, 90 at right, 180 at back, 270 at left
		int time = 1500; // time to play in millisecond also is the time delay
		int channel; // the channel that play the sound effect (channel >= 0)
		int loop = 0; // -1 for infinite loop, 0 for play once, more than 2 for number of rounds
	};
	
	struct Music {
		std::string filename = "../TopShopper/resources/sfx/elevatorMusic.mp3";
		Mix_Music* music;
		int volume = MIX_MAX_VOLUME / 32;  // only work for mod, mp3 and ogg files
		int loop = -1; // number of times to play, -1 forever, bigger than 0(number of times)
	};

	AudioManager(Broker *broker);
	virtual ~AudioManager();
	void init();
	void updateSeconds(double variableDeltaTime);

	SoundEffect* getSoundEffect(SoundEffectTypes sound);
	void playMusic(Music * myMusic);
	void pauseMusic();
	void resumeMusic();

	void playSFX(SoundEffect *mySfx);

	Mix_Music* loadMusic(std::string filename);
	//Mix_Chunk* loadSFX(SoundEffect *mySfx);
	Mix_Chunk* loadSFX(std::string filename);
	//SoundEffect testSoundEffect;
private:
	Broker *_broker = nullptr;

	std::map<std::string, Mix_Music*> mMusic;
	std::map<std::string, Mix_Chunk*> mSFX;

	Music bgm;
	std::vector<std::string> filenames = { "../TopShopper/resources/sfx/test2.wav" };
	std::vector<SoundEffect> soundEffects;
};



#endif // AUDIOMANAGER_H_
