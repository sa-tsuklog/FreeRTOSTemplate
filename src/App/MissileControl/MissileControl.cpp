/*
 * MissileControl.cpp
 *
 *  Created on: 2016/05/12
 *      Author: sa
 */

#include "FreeRTOS.h"
#include "task.h"

#include "math.h"

#include "MissileControl.h"
#include "ControlElement.h"
#include "ControlParams.h"

#include "MyLib/Gains/Gains.h"
#include "MyLib/Gains/Driver/Gps/Gps.h"
#include "MyLib/Gains/Driver/DummyGps/DummyGps.h"
#include "MyLib/Util/Util.h"
#include "MyLib/Seeker/Seeker.h"

MissileControl::MissileControl(){
	controlParamsQueue = xQueueCreate(1,sizeof(ControlParams));
	if(controlParamsQueue == NULL){
		while(1);
	}
	vQueueAddToRegistry(controlParamsQueue,"control");
	
	printModeQueue = xQueueCreate(1,sizeof(MissilePrintMode::Mode));
	if(printModeQueue == NULL){
		while(1);
	}
	
	vQueueAddToRegistry(printModeQueue,"missilePrintMode");
	printMode = MissilePrintMode::NONE;
	
	controlState = MissileControlState::LAUNCH_STANDBY;
	controlStateReturnPoint = MissileControlState::LAUNCH_STANDBY;
	
	radHeadingToTarget = 0;
	radPitchAtLaunch = 0;
	
	log0 = fopen("/log0","w");
}

void MissileControl::MissileControlTask(){
	portTickType lastWakeTime;
	lastWakeTime = xTaskGetTickCount();
	ControlParams params = ControlParams(0,0,0,0,0,0,0,0,0,0);
	ControlElement controlElement = ControlElement();
	float seekerUpDownSlowPrevious,seekerUpDownFastPrevious,seekerLeftRightSlowPrevious,seekerLeftRightFastPrevious;
	
	seekerUpDownSlowPrevious = seekerUpDownFastPrevious = seekerLeftRightSlowPrevious = seekerLeftRightFastPrevious = 0;
	
	while(1){
		/////////////////////////////////////
		// get control parameters
		/////////////////////////////////////
		ControlParams tmpControlParams = ControlParams(0,0,0,0,0,0,0,0,0,0);
		if(xQueueReceive(controlParamsQueue,&tmpControlParams,0) == pdTRUE){
			params = tmpControlParams;
		}
		
		
		/////////////////////////////////////
		// update state
		/////////////////////////////////////
		Quaternion reletivePos = Gains::GetInstance()->getMRelativePos();
		controlStateUpdate(&params);
		
		
		/////////////////////////////////////
		// set control surfaces
		/////////////////////////////////////
		if(controlState == MissileControlState::LAUNCH_STANDBY){
			radHeadingToTarget = Gains::GetInstance()->getAttitude().getRadHeading();
			radPitchAtLaunch = Gains::GetInstance()->getAttitude().getRadPitch(radHeadingToTarget);
			
			controlElement.controlSurfaceIdle();
		}else if(controlState == MissileControlState::BOOST_PHASE1){
			controlElement.controlSurfaceIdle();
		}else if(controlState == MissileControlState::BOOST_PHASE2){
			controlElement.holdAttitude(radPitchAtLaunch,radHeadingToTarget);
		}else if(controlState == MissileControlState::TERMINAL){
			float seekerUpDownSlow,seekerUpDownFast,seekerLeftRightSlow,seekerLeftRightFast,intensitySlow,intensityFast;
			Seeker::GetInstance()->getDirectionSlow(&seekerUpDownSlow,&seekerLeftRightSlow,&intensitySlow);
			Seeker::GetInstance()->getDirectionFast(&seekerUpDownFast,&seekerLeftRightFast,&intensityFast);
			
			if(intensityFast > Seeker::GetInstance()->getNoiseFloorFast()){
				//detected by fast seeker
				float seekerUpDownFastDelta = (seekerUpDownFast - seekerUpDownFastPrevious)/MS_CONTROL_INTERVAL;
				float seekerLeftRightFastDelta = (seekerLeftRightFast - seekerLeftRightFastPrevious)/MS_CONTROL_INTERVAL;
				
				radHeadingToTarget = Gains::GetInstance()->getAttitude().getRadHeading();
				controlElement.irTerminalGuidance(seekerUpDownFast,seekerLeftRightFast,seekerUpDownFastDelta,seekerLeftRightFastDelta,true);
			}else if(intensitySlow > Seeker::GetInstance()->getNoiseFloorSlow()){
				//detected by slow(high process gain) seeker
				float seekerUpDownSlowDelta = (seekerUpDownSlow - seekerUpDownSlowPrevious)/MS_CONTROL_INTERVAL;
				float seekerLeftRightSlowDelta = (seekerLeftRightSlow - seekerLeftRightSlowPrevious)/MS_CONTROL_INTERVAL;
				
				radHeadingToTarget = Gains::GetInstance()->getAttitude().getRadHeading();
				
				controlElement.irTerminalGuidance(seekerUpDownSlow,seekerLeftRightSlow,seekerUpDownSlowDelta,seekerLeftRightSlowDelta,false);
				
			}else{
				//not detected.
				controlElement.holdAttitude(0,radHeadingToTarget);
			}
			
			
			seekerUpDownSlowPrevious = seekerUpDownSlow;
			seekerUpDownFastPrevious = seekerUpDownFast;
			seekerLeftRightSlowPrevious = seekerLeftRightSlow;
			seekerLeftRightFastPrevious = seekerLeftRightFast;
			
		}else if(controlState == MissileControlState::MANUAL_CONTROL){
			controlElement.manualControl(&params);
		}
		
		
		
		if(controlState == MissileControlState::LAUNCH_STANDBY){
			printf("launch standby\r\n");
		}else if(controlState == MissileControlState::BOOST_PHASE1){
			printf("boost phase1\r\n");
		}else if(controlState == MissileControlState::BOOST_PHASE2){
			printf("boost phase2\r\n");
		}else if(controlState == MissileControlState::TERMINAL){
			float seekerUpDownSlow,seekerUpDownFast,seekerLeftRightSlow,seekerLeftRightFast,intensitySlow,intensityFast;
			Seeker::GetInstance()->getDirectionSlow(&seekerUpDownSlow,&seekerLeftRightSlow,&intensitySlow);
			Seeker::GetInstance()->getDirectionFast(&seekerUpDownFast,&seekerLeftRightFast,&intensityFast);
			
			if(intensityFast > Seeker::GetInstance()->getNoiseFloorFast()){
				printf("terminal - fast\r\n");
			}else if(intensitySlow > Seeker::GetInstance()->getNoiseFloorSlow()){
				printf("terminal - slow\r\n");
			}else{
				printf("terminal - not detected\r\n");
			}
			
		}else if(controlState == MissileControlState::MANUAL_CONTROL){
			printf("manual control\r\n");
		}else if(controlState == MissileControlState::EMERGENCY){
			printf("emergency\r\n");
		}
		
		
		
		/////////////////////////////////////
		// telemetry and logging
		/////////////////////////////////////
		print();
		
		
		vTaskDelayUntil(&lastWakeTime,MS_CONTROL_INTERVAL);
	}
}

void MissileControl::controlStateUpdate(ControlParams* params){
	static int msBoost1Time = 0;
	static int msBoost2Time = 0;
	
	/////////////////////////////////////
	// By control command, move to MANUAL or EMERGENCY from any state. 
	/////////////////////////////////////
	if((params->mode & ControlParams::BIT_CONTROL_MODE) == ControlParams::CONTROL_MODE_MANUAL){
		if(controlState != MissileControlState::MANUAL_CONTROL){
			controlStateReturnPoint = controlState;
		}
		controlState = MissileControlState::MANUAL_CONTROL;
		return;
	}else if(params->mode & ControlParams::BIT_EMERGENCY){
		controlState = MissileControlState::EMERGENCY;
		return;
	}
	
	/////////////////////////////////////
	// By Control command, reset state to LAUNCH_STANDBY
	/////////////////////////////////////
	if((params->mode & ControlParams::CONTROL_STATE_RESET)){
		controlState = MissileControlState::LAUNCH_STANDBY;
		return;
	}
	
	/////////////////////////////////////
	// Normal operation. move to next state on each condition. 
	/////////////////////////////////////
	if(controlState == MissileControlState::LAUNCH_STANDBY){
		msBoost1Time = 0;
		msBoost2Time = 0;
		Quaternion mpspsAcl = Gains::GetInstance()->getMpspsAcl();
		
//		if(mpspsAcl.x > MPSPS_LAUNCH_ACCEL_THRETHOLD){
//			controlState = MissileControlState::BOOST_PHASE1;
//		}
		controlState = MissileControlState::TERMINAL;
	}else if(controlState == MissileControlState::BOOST_PHASE1){
		msBoost1Time+=MS_CONTROL_INTERVAL;
		
		if(msBoost1Time > MS_BOOST1_LENGTH){
			controlState = MissileControlState::BOOST_PHASE2;
		}
	}else if(controlState == MissileControlState::BOOST_PHASE2){
		msBoost2Time+=MS_CONTROL_INTERVAL;
				
		if(msBoost2Time > MS_BOOST2_LENGTH){
			controlState = MissileControlState::TERMINAL;
		}
	}else if(controlState == MissileControlState::TERMINAL){
		//do nothing.
	}else if(controlState == MissileControlState::MANUAL_CONTROL){
		if((params->mode & ControlParams::BIT_CONTROL_MODE) != ControlParams::CONTROL_MODE_MANUAL){
			controlState = controlStateReturnPoint;
		}
	}
}

void MissileControl::print(){
	static int decimator = 0;
	decimator = (decimator+1)%100;
	
	MissilePrintMode::Mode printMode;
	if(xQueueReceive(printModeQueue,&printMode,0) == pdTRUE){
		this->printMode = printMode;
	}
	
	if(this->printMode == MissilePrintMode::GPAIO){
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

void MissileControl::printGpaio(FILE* fp){
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
		fprintf(fp,"%d,",0);
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
		fprintf(fp,"%d,",0);
		fprintf(fp,"%d,",controlState);
		fprintf(fp,"%.2f,",batteryVoltage);
		fprintf(fp,"00\r\n");//checksum
	}
}

void MissileControl::setPrintMode(MissilePrintMode::Mode mode){
	if(printModeQueue != NULL){
		xQueueOverwrite(printModeQueue,&mode);
	}
}
void MissileControl::setControlParams(ControlParams* controlParams){
	if(controlParamsQueue != NULL){
		xQueueOverwrite(controlParamsQueue,controlParams);
	}
}

void MissileControl::MissileControlTaskEntry(void *pvParameters){
	MissileControl::GetInstance()->MissileControlTask();
}
void MissileControl::initMissileControl(){
	xTaskCreate(&MissileControl::MissileControlTaskEntry,"ctrl",1024,NULL,2,NULL);
}



