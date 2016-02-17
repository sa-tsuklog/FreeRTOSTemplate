/*
 * GliderControl.cpp
 *
 *  Created on: 2015/04/05
 *      Author: sa
 */
#include <pch.h>

#include "GliderControl.h"
#include "GliderControlTestBench.h"
#include "GliderServoControl.h"
#include "ControlState/BoostPhase0Control.h"
#include "ControlState/BoostPhaseControl.h"
#include "ControlState/GlidePhaseControl.h"
#include "ControlState/GlidePhaseHeadingHold.h"
#include "ControlState/ManualControl.h"
#include "ControlState/EmergencyControl.h"
#include "ControlState/PitchAndHeadingHold.h"

#include "Common/Quaternion.h"
#include "MyLib/Gains/Gains.h"
#include "MyLib/Gains/Driver/Gps/Gps.h"
#include "MyLib/Gains/Driver/DummyGps/DummyGps.h" 
#include "MyLib/Servo/Servo.h"
#include "MyLib/Util/Util.h"
#include "MyLib/Util/UserFlashData.h"
#include "ControlParams.h"

/* Servo Ch List
 * 
 * ServoCh0: MainWingOpen
 * ServoCh4: Rudder
 * ServoCh5: RightElevator
 * ServoCh6: LeftElevator
 */



GliderControl::GliderControl(){
	guidance = GpsGuidance();
	controlParamsQueue = xQueueCreate(1,sizeof(ControlParams));
	vQueueAddToRegistry(controlParamsQueue,"control");
	
	printModeQueue = xQueueCreate(1,sizeof(GliderPrintMode::Mode));
	vQueueAddToRegistry(printModeQueue,"gliderPrintMode");
	printMode = GliderPrintMode::GPAIO;
	
	Servo::GetInstance();
	
	testBench = GliderControlTestBench();
	
	controlWatchdog = 0;
	controlState = ControlState::LAUNCH_STANDBY;
	controlStateReturnPoint = ControlState::LAUNCH_STANDBY;
	
	log0 = fopen("/log0","w");
}

GpsGuidance* GliderControl::getGpsGuidance(){
	return &guidance;
}

void GliderControl::gliderControlTask(){
	float radHeadingAtLaunch=0.0;
	float radPitchAtLaunch = 0.0;
	
	Servo::GetInstance()->start();
	ControlParams params = ControlParams(0,0,0,0,0,0,0,0,0,0);
	
	BoostPhase0Control boostPhase0Control = BoostPhase0Control();
	BoostPhaseControl boostPhaseControl = BoostPhaseControl();
	//GlidePhaseControl glidePhaseControl = GlidePhaseControl(&guidance); 
	GlidePhaseHeadingHold glidePhaseControl = GlidePhaseHeadingHold(&guidance);
	EmergencyControl emergencyControl = EmergencyControl();
	ManualControl manualControl = ManualControl();
	PitchAndHeadingHold pitchHeadingHold = PitchAndHeadingHold();
	
	while(1){
		//testBench.testEntry(&guidance);
		/////////////////////////////////////
		// get control parameters
		/////////////////////////////////////
		ControlParams tmpControlParams = ControlParams(0,0,0,0,0,0,0,0,0,0);
		if(xQueueReceive(controlParamsQueue,&tmpControlParams,0) == pdTRUE){
			params = tmpControlParams;
			controlWatchdog = 0;
		}else{
			controlWatchdog += MS_CONTROL_INTERVAL;
		}
		
		/////////////////////////////////////
		// update state
		/////////////////////////////////////
		Quaternion reletivePos = Gains::GetInstance()->getMRelativePos();
		guidance.updateNextWaypoint(reletivePos.x,reletivePos.y);
		controlStateUpdate(&params);
		
		if(params.mode & ControlParams::CONTROL_STATE_RESET){
			boostPhaseControl.reset();
			glidePhaseControl.reset();
			emergencyControl.reset();
			manualControl.reset();
			pitchHeadingHold.reset();
		}
		
		/////////////////////////////////////
		// control
		/////////////////////////////////////
		if(controlState == ControlState::LAUNCH_STANDBY){
			GliderServoControl::mainWingLatch();
			GliderServoControl::setPos(0,0,0);
			radHeadingAtLaunch = Gains::GetInstance()->getAttitude().getRadHeading();
		}else if(controlState == ControlState::BOOST_PHASE0){
			boostPhase0Control.control(radPitchAtLaunch,radHeadingAtLaunch);
		}else if(controlState == ControlState::BOOST_PHASE){
			boostPhaseControl.control(radHeadingAtLaunch);
		}else if(controlState == ControlState::GLIDE_PHASE){
			//glidePhaseControl.control();
			glidePhaseControl.control(radHeadingAtLaunch);
		}else if(controlState == ControlState::MANUAL_CONTROL){
			manualControl.control(&params);
		}else if(controlState == ControlState::PITCH_HEADING_HOLD){
			pitchHeadingHold.control();
		}else{//EMERGENCY
			emergencyControl.control();
		}
		
		/////////////////////////////////////
		// telemetry and logging
		/////////////////////////////////////
		print();
		
		vTaskDelay(MS_CONTROL_INTERVAL);
	}
}

void GliderControl::controlStateUpdate(ControlParams* params){
	static int msBoostPhase0Time=0;
	
	Quaternion mPosition = Gains::GetInstance()->getMRelativePos();
	float mDistanceFromCource = guidance.mDistanceFromCurrentCourse(mPosition.x,mPosition.y); 
	
	if((controlWatchdog > MS_CONTROL_WATCHDOG_MAX)){
		//controlState = ControlState::EMERGENCY;
	}else if((params->mode & ControlParams::BIT_CONTROL_MODE) == ControlParams::CONTROL_MODE_MANUAL){
		if(controlState != ControlState::MANUAL_CONTROL){
			controlStateReturnPoint = controlState;
		}
		controlState = ControlState::MANUAL_CONTROL;
	}else if(((params->mode & ControlParams::BIT_CONTROL_MODE) == ControlParams::CONTROL_MODE_PITCH_HEADING_HOLD)){
		if(controlState != ControlState::PITCH_HEADING_HOLD){
			controlStateReturnPoint = controlState;
		}
		controlState = ControlState::PITCH_HEADING_HOLD;
	}else if(params->mode & ControlParams::BIT_EMERGENCY){
		controlState = ControlState::EMERGENCY;
	}else if((mDistanceFromCource > M_DISTANCE_LIMIT_FROM_COURCE )||
			(mDistanceFromCource < -M_DISTANCE_LIMIT_FROM_COURCE)){
		//controlState = ControlState::EMERGENCY;
	}
	
	if((params->mode & ControlParams::CONTROL_STATE_RESET)){
		controlState = ControlState::LAUNCH_STANDBY;
	}
	
	
	if(controlState == ControlState::LAUNCH_STANDBY){
		msBoostPhase0Time = 0;
		Quaternion acl = Gains::GetInstance()->getMpspsAcl();
		if(acl.abs() > 9.8 * 3.0){
			controlState = ControlState::BOOST_PHASE0;
		}
	}else if(controlState == ControlState::BOOST_PHASE0){
		msBoostPhase0Time += MS_CONTROL_INTERVAL;
		if(msBoostPhase0Time > MS_BOOST_PHASE0_TIME){
			controlState = ControlState::BOOST_PHASE;
		}
	}else if(controlState == ControlState::BOOST_PHASE){
		Quaternion attitude = Gains::GetInstance()->getAttitude();
		float pitch,roll,heading;
		attitude.getRadPitchRollHeading(&pitch,&roll,&heading);
		if(pitch < 0){
			controlState = ControlState::GLIDE_PHASE;
		}
	}else if(controlState == ControlState::GLIDE_PHASE){
	}else if(controlState == ControlState::MANUAL_CONTROL){
		if((params->mode & ControlParams::BIT_CONTROL_MODE) != ControlParams::CONTROL_MODE_MANUAL){
			controlState = controlStateReturnPoint;
		}
	}else if(controlState == ControlState::EMERGENCY){
	}
}

void GliderControl::print(){
	static int decimator = 0;
	decimator = (decimator+1)%100;
	
	GliderPrintMode::Mode printMode;
	if(xQueueReceive(printModeQueue,&printMode,0) == pdTRUE){
		this->printMode = printMode;
	}
	
	if(this->printMode == GliderPrintMode::GPAIO){
		if(decimator % 10 == 0){
			printGpaio(stdout);
		}
	}
	
	if(decimator % 5 == 1){
		Quaternion rpsRate = Gains::GetInstance()->getRpsRate();
		Quaternion mpspsAccel = Gains::GetInstance()->getMpspsAcl();
		Quaternion uTCmps = Gains::GetInstance()->getUtCmps();
		float paPressure = Gains::GetInstance()->getPaPressure();
		int isPressureValid = Gains::GetInstance()->isPressureValid();
		
		fprintf(log0,"$GPINS,");
		fprintf(log0,"%.3f,%.3f,%.3f,",mpspsAccel.x,mpspsAccel.y,mpspsAccel.z);
		fprintf(log0,"%.3f,%.3f,%.3f,",rpsRate.x,rpsRate.y,rpsRate.z);
		fprintf(log0,"%.3f,%.3f,%.3f,",uTCmps.x,uTCmps.y,uTCmps.z);
		fprintf(log0,"%.3f,%d\r\n",paPressure,isPressureValid);
	}
	
	if(decimator % 5 == 2){
		printGpaio(log0);
	}
	
}

void GliderControl::printGpaio(FILE* fp){
	static int tmp=0;
	tmp = (tmp+1)%20;
	//$GPAIO,Latitude,N/S,Longitude,E/W,height,HDOP,pitch,roll,yaw,SpeedX,SpeedY,SpeedZ,GpsValid,waypointId,controlState,checksum
	Quaternion attitude = Gains::GetInstance()->getAttitude();
	float pitch,roll,heading;
	attitude.getRadPitchRollHeading(&pitch,&roll,&heading);
	
	Quaternion mRelativePos = Gains::GetInstance()->getMRelativePos();
	Quaternion mpsSpeed = Gains::GetInstance()->getMpsSpeed();
	float batteryVoltage = Util::GetInstance()->getVoltInputVoltage();
	
	if(Util::GetInstance()->flashData.gpsType == GpsType::USART_GPS){
		int degX1MLattitude = Gps::GetInstance()->mPosXToDegX1M_Latitude(mRelativePos.x);
		int degX1MLongitude = Gps::GetInstance()->mPosYToDegX1M_Longitude(mRelativePos.y);				
		
		
		float height =  mRelativePos.z;
		int gpsValid = Gains::GetInstance()->isGpsValid();
		
		fprintf(fp,"$GPAIO,%d%.6f,N,%d%.6f,E,%.2f,1,",degX1MLattitude/1000000,60.0*(degX1MLattitude%1000000)/1000000,degX1MLongitude/1000000,60.0*(degX1MLongitude%1000000)/1000000,height);//lattitude,longitude,height,HDOP
		fprintf(fp,"%.3f,%.3f,%.3f,",pitch*180/M_PI,roll*180/M_PI,heading*180/M_PI);//pitch,roll,heading
		fprintf(fp,"%.3f,%.3f,%.3f,",mpsSpeed.x,mpsSpeed.y,mpsSpeed.z);//SpeedX,Y,Z
		fprintf(fp,"%d,",gpsValid);//GpsValid
		fprintf(fp,"%d,",guidance.getNextWaypointId());
		fprintf(fp,"%d,",controlState);
		fprintf(fp,"%.2f,",batteryVoltage);
		fprintf(fp,"%02d\r\n",tmp);//checksum
	}else{
		int degX1MLattitude = DummyGps::GetInstance()->mPosXToDegX1M_Latitude(mRelativePos.x);
		int degX1MLongitude = DummyGps::GetInstance()->mPosYToDegX1M_Longitude(mRelativePos.y);
		float height =  mRelativePos.z;
		
		//fprintf(fp,"$GPAIO,%d%.6f,N,%d%.6f,E,%.2f,1,",0,0.0,0,0.0,0.0);//lattitude,longitude,height,HDOP
		fprintf(fp,"$GPAIO,%d%.6f,N,%d%.6f,E,%.2f,1,",degX1MLattitude/1000000,60.0*(degX1MLattitude%1000000)/1000000,degX1MLongitude/1000000,60.0*(degX1MLongitude%1000000)/1000000,height);//lattitude,longitude,height,HDOP
		fprintf(fp,"%.3f,%.3f,%.3f,",pitch*180/M_PI,roll*180/M_PI,heading*180/M_PI);//pitch,roll,heading
		fprintf(fp,"%.3f,%.3f,%.3f,",mpsSpeed.x,mpsSpeed.y,mpsSpeed.z);//SpeedX,Y,Z
		fprintf(fp,"%d,",1);//GpsValid
		fprintf(fp,"%d,",guidance.getNextWaypointId());
		fprintf(fp,"%d,",controlState);
		fprintf(fp,"%.2f,",batteryVoltage);
		fprintf(fp,"00\r\n");//checksum
	}
}

void GliderControl::setControlParams(ControlParams* controlParams){
	if(controlParamsQueue != NULL){
		xQueueOverwrite(controlParamsQueue,controlParams);
	}
}

void GliderControl::setTestNumer(int testNumber){
	testBench.setTestNumber(testNumber);
}

void GliderControl::setPrintMode(GliderPrintMode::Mode mode){
	if(printModeQueue != NULL){
		xQueueOverwrite(printModeQueue,&mode);
	}
}

void GliderControl::gliderControlTaskEntry(void *pvParameters){
	GliderControl::GetInstance()->gliderControlTask();
}
void GliderControl::initGliderControl(){
	xTaskCreate(&GliderControl::gliderControlTaskEntry,"gldr",2048,NULL,2,NULL);
}
