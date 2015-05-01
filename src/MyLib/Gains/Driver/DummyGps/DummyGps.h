
class DummyGps {
	// Singleton pattern definition
private:
	DummyGps();
	DummyGps(const DummyGps& rhs);
	DummyGps& operator=(const DummyGps& rhs);
	virtual ~DummyGps() {}
public:
	static DummyGps* GetInstance() {
		static DummyGps instance;
		return &instance;
	}

	// Class definition
private:
	void dummyGpsTask();
public:
	int available;
	static void prvDummyGpsTask(void* pvParameters);
	
};
