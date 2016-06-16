
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
	static const float M_EARTH_RADIUS = 6371000;
	void dummyGpsTask();
	
	static const int degX1MLattitudeRef = 35707909; 
	static const int degX1MLongitudeRef = 139760745;
public:
	int available;
	int DummyGps::mPosXToDegX1M_Latitude(float mPosX);
	int DummyGps::mPosYToDegX1M_Longitude(float mPosY);
	static void prvDummyGpsTask(void* pvParameters);
	
};
