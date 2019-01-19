#ifndef __ORBOX_VECTOR_BOX_H__
#define __ORBOX_VECTOR_BOX_H__



//--- SDK include
#include <fbsdk/fbsdk.h>
#include <iostream>
#include <string>
#import <msxml6.dll> rename_namespace(_T("MSXML"))


//--- Registration defines
#define	ORBOXVECTOR__CLASSNAME		ORBoxVector
#define ORBOXVECTOR__CLASSSTR		"ORBoxVector"

/**	Vector to number conversion box..
*	Create a vector to number converter as an operator
*	for relations constraints.
*/

//------------------ frame data structure-------------------
class FramePoseData 
{
public:
	FramePoseData ();
	FramePoseData (double TimeFrame, double CharRootPos[3], double CharRootRotY, double HipsPos[3], double HipsRot[3], double FootLPos[3], double FootRPos[3], double ElbowLPos[3], double ElbowRPos[3] , double HipsPos_F15[3],	double FootLPos_F15[3],	double FootRPos_F15[3],	double ElbowLPos_F15[3], double ElbowRPos_F15[3], double CharRootVelocityVec_F0[3],	double CharRootVelocityVec_F15[3],	double CharRootVelocityVec_F60[3], double RootRotDelta, double HipRotDelta, double CharRootSpeed, double HipsSpeed, double FootLSpeed, double FootRSpeed, double ElbowLSpeed, double ElbowRSpeed);
	double	lTimeFrame, lCharRootPos[3], lCharRootRotY, lHipsPos[3], lHipsRot[3], lFootLPos[3], lFootRPos[3], lElbowLPos[3], lElbowRPos[3];
	double  l2RootRotDelta, l2HipRotDelta, l2CharRootSpeed, l2HipsSpeed, l2FootLSpeed, l2FootRSpeed, l2ElbowLSpeed, l2ElbowRSpeed, l2CharRootVelocityVec_F0[3], l2CharRootVelocityVec_F15[3], l2CharRootVelocityVec_F60[3];	
	double	l2HipsPos_F15[3], l2FootLPos_F15[3], l2FootRPos_F15[3], l2ElbowLPos_F15[3], l2ElbowRPos_F15[3];
};


//------------------ MB relation constraint node--------------------
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
	//------------MB read/record variables--------------------
	FBAnimationNode* 	mTimeFrame, *mCharRootPos, *mCharRootRotY, *mHipsPos, *mHipsRot, *mFootLPos, *mFootRPos, *mElbowLPos, *mElbowRPos;		//!< Input node:	Vector
	FBAnimationNode*	mProcessToMemory, *mCalculateIteration2, *mSaveToFile, *mReadFromFile, *mDebugOutput;
	FBAnimationNode*	mIsInMemory;// Output nodes:	X,Y,Z
	FBAnimationNode*	mCharRootPosDebug, *mCharRootRotYDebug, *mHipsPosDebug, *mHipsRotDebug, *mFootLPosDebug, *mFootRPosDebug, *mElbowLPosDebug, *mElbowRPosDebug, *mRootRotDeltaDebug, *mHipRotDeltaDebug, *mCharRootSpeedDebug, *mHipsSpeedDebug, *mFootLSpeedDebug, *mFootRSpeedDebug, *mElbowLSpeedDebug, *mElbowRSpeedDebug;
	FBAnimationNode*    mCharRootVelocityVec_F0Debug, *mCharRootVelocityVec_F15Debug, *mCharRootVelocityVec_F60Debug, *mHipsPos_F15Debug, *mFootLPos_F15Debug, *mFootRPos_F15Debug, *mElbowLPos_F15Debug, *mElbowRPos_F15Debug;
	//----------- inner data holders-------------------------- 
	double	lRecordToggle, lTimeFrame, lCharRootPos[3], lCharRootRotY, lHipsPos[3], lHipsRot[3], lFootLPos[3], lFootRPos[3], lElbowLPos[3], lElbowRPos[3], lFinalizeCacheCompute, lOutputFinilizedDebugInfo;
	double RecordingStatusOutput, isLoadedToMemory, lProcessToMemory, lCalculateIteration2, lSaveToFile, lReadFromFile, lDebugOutput;
	double TimesliderUpdate, Itteration2Done, fileSaved;

	FramePoseData PoseDatabase [20000]; //how'bout'dat?
	// ver 1 functions
//	bool RecordData_Pass1 (std::wstring lFileName, bool RecordingToggle);
	bool WriteFrameToXml (BSTR  lXmlFileName, BSTR  lFrameNumber);
	bool CheckFrameDataExistInXml (BSTR  lXmlFileName, BSTR  lFrameNumber);
	FramePoseData ReadFrameData (BSTR  lXmlFileName, BSTR  lFrameNumber);
	double CalculateSpeedInterpolated (int lFrameNumber, int BodyTypeEnum);
	double CalculateAngleChangeInterpolated (int lFrameNumber, int BodyTypeEnum);
	FramePoseData CalculateVelocityAt3Points (int lFrameNumber); //velocity vector in F0, f15 and f60
//	bool Append2ndIttDataFrame (BSTR  lXmlFileName, BSTR  lFrameNumber);
//	bool RecordData_Pass2 (std::wstring lFileName, bool RecordingToggle);
//	bool DebugPoseDataOutput (FBEvaluateInfo* pEvaluateInfo);
	//ver 2 functions
	bool ProcessToMemory_Ver2 ();
	bool Calculate2ndIterationToMemory_Ver2 ();
	bool SaveToFile_Ver2 (BSTR  lXmlFileName);
	bool ReadFromFileToMemory_Ver2 (BSTR  lXmlFileName);
	bool DebugOutput_Ver2	(FBEvaluateInfo* pEvaluateInfo);
	bool DebugOutput_zeros	(FBEvaluateInfo* pEvaluateInfo);


	MSXML::IXMLDOMDocument2Ptr xmlDoc;
	HRESULT hr;
};

#endif /* __ORBOX_VECTOR_BOX_H__ */



