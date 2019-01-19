#ifndef __ORBOX_VECTOR_BOX_H__
#define __ORBOX_VECTOR_BOX_H__


//--- SDK include
#include <fbsdk/fbsdk.h>
#include <iostream>
#include <string>
#import <msxml6.dll> rename_namespace(_T("MSXML"))

//--- Registration defines
#define	ORBOXVECTOR__CLASSNAME		ORBoxVector
#define ORBOXVECTOR__CLASSSTR		"ORBoxVector1"

class GamepadInputFrameData
{
public:
	GamepadInputFrameData();
	GamepadInputFrameData(double TimeFrame, double CharRootPos[3], double CharRootRotY, double CharRootRotationDelta, double CharRootVelocityVec_F0[3], double CharRootVelocityVec_F15[3], double CharRootVelocityVec_F60[3], double CharRootSpeed);
	double  lTimeFrame, lCharRootPos[3], lCharRootRotY;
	double lCharRootRotationDelta, lCharRootVelocityVec_F0[3], lCharRootVelocityVec_F15[3], lCharRootVelocityVec_F60[3], lCharRootSpeed;
};




class ORBoxVector : public FBBox
{
	//--- box declaration.
	FBBoxDeclare( ORBoxVector, FBBox );

public:
	virtual bool FBCreate();		//!< creation function.
	virtual void FBDestroy();		//!< destruction function.
	       
	
	//! Overloaded FBBox real-time evaluation function.
	virtual bool AnimationNodeNotify(FBAnimationNode* pAnimationNode,FBEvaluateInfo* pEvaluateInfo);

	GamepadInputFrameData lCurrentFrameInputData;
	void RecordFrameToFile (std::wstring lFileName);
	bool CheckFrameDataExistInXml (BSTR  lXmlFileName, BSTR  lFrameNumber);
	bool RecordFrameDataStructure (BSTR  lXmlFileName, BSTR  lFrameNumber);
	GamepadInputFrameData ReadFrameData (BSTR  lXmlFileName, BSTR  lFrameNumber);
	double CalculateSpeedInterpolated (BSTR  lXmlFileName, BSTR  lFrameNumber);
	double CalculateAngleChangeInterpolated (BSTR  lXmlFileName, BSTR  lFrameNumber);
	GamepadInputFrameData CalculateRootVelocityAt3Points (BSTR  lXmlFileName, BSTR  lFrameNumber); //velocity vector in F0, f15 and f60
	bool Append2ndIttDataFrame (BSTR  lXmlFileName, BSTR  lFrameNumber);
	bool RecordData_Pass2 (std::wstring lFileName, bool RecordingToggle);
	bool DebugPoseDataOutput (FBEvaluateInfo* pEvaluateInfo);

private:
	FBAnimationNode* 	mRecordToggle, *mPostProcessToggle, *mDebugToggle, *mTimeFrame, *mCharRootPos, *mCharRootRotY;			//!< Input node:	Vector
	FBAnimationNode*	mIsRecorded, *mDebugCharRootPos, *mDebugCharRootRotY, *mCharRootRotationDelta, *mCharRootVelocityVec_F0, *mCharRootVelocityVec_F15, *mCharRootVelocityVec_F60, *mCharRootSpeed;							    //!< Output nodes:	X,Y,Z
	double	lRecordToggle, lPostProcessToggle, lDebugToggle, lTimeFrame, lCharRootPos[3], lCharRootRotY, lCharRootRotationDelta, lCharRootVelocityVec_F0[3], lCharRootVelocityVec_F15[3], lCharRootVelocityVec_F60[3], lCharRootSpeed;						    //outpun holders
	double TimesliderUpdate, Itteration2Done;																				//aux

	MSXML::IXMLDOMDocument2Ptr xmlDoc;
	HRESULT hr;
};

#endif /* __ORBOX_VECTOR_BOX_H__ */
