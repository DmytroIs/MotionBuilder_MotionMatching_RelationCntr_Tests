#ifndef __ORBOX_VECTOR_BOX_H__
#define __ORBOX_VECTOR_BOX_H__

 

//--- SDK include
#include <fbsdk/fbsdk.h>
#include <iostream>
#include <string>
#import <msxml6.dll> rename_namespace(_T("MSXML"))

//--- Registration defines
#define	ORBOXVECTOR__CLASSNAME		ORBoxVector
#define ORBOXVECTOR__CLASSSTR		"ORBoxVector3"


//---------------- frame data structure-------------------------------------------------------------------
class FramePoseData 
{
public:
	FramePoseData ();
	FramePoseData (double TimeFrame, double CharRootPos[3], double CharRootRotY, double HipsPos[3], double HipsRot[3], double FootLPos[3], double FootRPos[3], double ElbowLPos[3], double ElbowRPos[3] , double HipsPos_F15[3],	double FootLPos_F15[3],	double FootRPos_F15[3],	double ElbowLPos_F15[3], double ElbowRPos_F15[3], double CharRootVelocityVec_F0[3],	double CharRootVelocityVec_F15[3],	double CharRootVelocityVec_F60[3], double RootRotDelta, double HipRotDelta, double CharRootSpeed, double HipsSpeed, double FootLSpeed, double FootRSpeed, double ElbowLSpeed, double ElbowRSpeed);
	double	lTimeFrame, lCharRootPos[3], lCharRootRotY, lHipsPos[3], lHipsRot[3], lFootLPos[3], lFootRPos[3], lElbowLPos[3], lElbowRPos[3];
	double  l2RootRotDelta, l2HipRotDelta, l2CharRootSpeed, l2HipsSpeed, l2FootLSpeed, l2FootRSpeed, l2ElbowLSpeed, l2ElbowRSpeed, l2CharRootVelocityVec_F0[3], l2CharRootVelocityVec_F15[3], l2CharRootVelocityVec_F60[3];	
	double	l2HipsPos_F15[3], l2FootLPos_F15[3], l2FootRPos_F15[3], l2ElbowLPos_F15[3], l2ElbowRPos_F15[3];
};

//-------------input data structure ---------------------------------------------------------------------
class GamepadInputFrameData
{
public:
	GamepadInputFrameData();
	GamepadInputFrameData(double TimeFrame, double CharRootPos[3], double CharRootRotY, double CharRootRotationDelta, double CharRootVelocityVec_F0[3], double CharRootVelocityVec_F15[3], double CharRootVelocityVec_F60[3], double CharRootSpeed);
	double  lTimeFrame, lCharRootPos[3], lCharRootRotY;
	double lCharRootRotationDelta, lCharRootVelocityVec_F0[3], lCharRootVelocityVec_F15[3], lCharRootVelocityVec_F60[3], lCharRootSpeed;
};

//-------------motion matching settings data structure ---------------------------------------------------------------------
class MotionMatchingSettings
{
public:
	MotionMatchingSettings();

	double FrameRate; //3 frames
	double Deviation_RootSpeed;
	double Deviation_RootRotDelta;
	double Deviation_Hips;

	double MatchingWeight_RootSpeed;
	double MatchingWeight_RootRotDelta;
	double MatchingWeight_Hips;

};

//--------------------------------------------------------------------------------------------------------
class ORBoxVector : public FBBox
{
	//--- box declaration.
	FBBoxDeclare( ORBoxVector, FBBox );

public:
	virtual bool FBCreate();		//!< creation function.
	virtual void FBDestroy();		//!< destruction function.

	//! Overloaded FBBox real-time evaluation function.
	virtual bool AnimationNodeNotify(FBAnimationNode* pAnimationNode,FBEvaluateInfo* pEvaluateInfo);


private:
	double lTriggerCalculate, lTriggerPastePoses, lTriggerLoadPoseDatabase, lTriggerLoadInputData, lOutputCalculateSucceded, lOutputPastePosesSucceded;
	FBAnimationNode* mTriggerCalculate, *mTriggerPastePoses, *mTriggerLoadPoseDatabase, *mTriggerLoadInputData ;
	FBAnimationNode* mOutputCalculateSucceded, *mOutputPastePosesSucceded;
	

    FramePoseData PoseDatabase [20000]; //how'bout'dat?
	GamepadInputFrameData InputDatabase [20000];
	MotionMatchingSettings MMSettings;
	long ResultFrameSequence [20000];

	double isLoadedToMemory, isInputDBLoadedToMemory;

	bool ReadFromFileToMemory_PoseDatabase (BSTR  lXmlFileName);
	bool ReadFromFileToMemory_InputDatabase (BSTR  lXmlFileName);
	long ReturnPoseMatchFrame (GamepadInputFrameData InputData);
	bool CalculatePoseMatchSequence ();
	void InitMatchingSettings ();

	MSXML::IXMLDOMDocument2Ptr xmlDoc;
	HRESULT hr;
};

#endif /* __ORBOX_VECTOR_BOX_H__ */
