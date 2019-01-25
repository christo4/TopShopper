


#ifndef AUDIOMANAGER_H_
#define AUDIOMANAGER_H_

class Broker;

class AudioManager {
public:
	AudioManager(Broker* broker);
	virtual ~AudioManager();
	void init();
	void updateMilliseconds(double deltaTime);
private:
	Broker* _broker = nullptr;
};




















#endif // AUDIOMANAGER_H_
