#ifndef AUDIOMANAGER_H_
#define AUDIOMANAGER_H_

class Broker;

class AudioManager {
public:
	AudioManager(Broker *broker);
	virtual ~AudioManager();
	void init();
	void updateSeconds(double variableDeltaTime);
private:
	Broker *_broker = nullptr;
};




















#endif // AUDIOMANAGER_H_
