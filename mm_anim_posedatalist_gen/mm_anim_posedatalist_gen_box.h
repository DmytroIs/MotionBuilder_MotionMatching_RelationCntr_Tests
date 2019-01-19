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
	FramePoseData (double lTimeFrame, double lCharRootPos[3], double lCharRootRotY, double lHipsPos[3], double lHipsRotY, double lFootLPos[3], double lFootRPos[3], double lElbowLPos[3], double lElbowRPos[3]);
	double	lTimeFrame, lCharRootPos[3], lCharRootRotY, lHipsPos[3], lHipsRotY, lFootLPos[3], lFootRPos[3], lElbowLPos[3], lElbowRPos[3];
	double  l2RootRotDelta, l2HipRotDelta, l2CharRootSpeed, l2HipsSpeed, l2FootLSpeed, l2FootRSpeed, l2ElbowLSpeed, l2ElbowRSpeed, l2CharRootVelocityVec_F0[3], l2CharRootVelocityVec_F15[3], l2CharRootVelocityVec_F60[3];	
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
	FBAnimationNode* 	mRecordToggle, *mTimeFrame, *mCharRootPos, *mCharRootRotY, *mHipsPos, *mHipsRotY, *mFootLPos, *mFootRPos, *mElbowLPos, *mElbowRPos, *mFinalizeCacheCompute, *mOutputFinilizedDebugInfo;		//!< Input node:	Vector
	FBAnimationNode*	mIsRecorded;// Output nodes:	X,Y,Z
	FBAnimationNode*	mCharRootPosDebug, *mCharRootRotYDebug, *mHipsPosDebug, *mHipsRotYDebug, *mFootLPosDebug, *mFootRPosDebug, *mElbowLPosDebug, *mElbowRPosDebug, *mRootRotDeltaDebug, *mHipRotDeltaDebug, *mCharRootSpeedDebug, *mHipsSpeedDebug, *mFootLSpeedDebug, *mFootRSpeedDebug, *mElbowLSpeedDebug, *mElbowRSpeedDebug, *mCharRootVelocityVec_F0Debug, *mCharRootVelocityVec_F15Debug, *mCharRootVelocityVec_F60Debug;
	//----------- inner data holders-------------------------- 
	double	lRecordToggle, lTimeFrame, lCharRootPos[3], lCharRootRotY, lHipsPos[3], lHipsRotY, lFootLPos[3], lFootRPos[3], lElbowLPos[3], lElbowRPos[3], lFinalizeCacheCompute, lOutputFinilizedDebugInfo;
	double RecordingStatusOutput;
	double TimesliderUpdate, Itteration2Done;

	bool RecordData_Pass1 (std::wstring lFileName, bool RecordingToggle);
	bool WriteFrameToXml (BSTR  lXmlFileName, BSTR  lFrameNumber);
	bool CheckFrameDataExistInXml (BSTR  lXmlFileName, BSTR  lFrameNumber);
	FramePoseData ReadFrameData (BSTR  lXmlFileName, BSTR  lFrameNumber);
	double CalculateSpeedInterpolated (BSTR  lXmlFileName, BSTR  lFrameNumber, int BodyTypeEnum);
	double CalculateAngleChangeInterpolated (BSTR  lXmlFileName, BSTR  lFrameNumber, int BodyTypeEnum);
	FramePoseData CalculateRootVelocityAt3Points (BSTR  lXmlFileName, BSTR  lFrameNumber); //velocity vector in F0, f15 and f60
	bool Append2ndIttDataFrame (BSTR  lXmlFileName, BSTR  lFrameNumber);
	bool RecordData_Pass2 (std::wstring lFileName, bool RecordingToggle);
	bool DebugPoseDataOutput (FBEvaluateInfo* pEvaluateInfo);


	MSXML::IXMLDOMDocument2Ptr xmlDoc;
	HRESULT hr;
};

#endif /* __ORBOX_VECTOR_BOX_H__ */



