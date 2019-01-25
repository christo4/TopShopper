


#ifndef LOADINGMANAGER_H_
#define LOADINGMANAGER_H_

class Broker;

class LoadingManager {
public:
	LoadingManager(Broker* broker);
	virtual ~LoadingManager();
	void init();
	void updateMilliseconds(double deltaTime);
private:
	Broker* _broker = nullptr;
};









#endif // LOADINGMANAGER_H_
