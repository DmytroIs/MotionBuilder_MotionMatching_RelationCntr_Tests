
/**	\file	orboxvector_box.cxx
*	Definition of a simple box class.
*	Definition of the FBSimpleBox class, which demonstrates the use
*	of the real-time function calls, splitting a vector into three channels.
*/

//--- Class declaration
#include "mm_anim_posedatalist_gen_box.h"
#include <math.h>

//--- Registration defines
#define ORBOXVECTOR__CLASS		ORBOXVECTOR__CLASSNAME
#define ORBOXVECTOR__NAME		ORBOXVECTOR__CLASSSTR
#define	ORBOXVECTOR__LOCATION	"MotionMatching_Test1"
#define ORBOXVECTOR__LABEL		"mm_anim_posedatalist_gen"
#define	ORBOXVECTOR__DESC		"mm_anim_posedatalist_gen"

//--- implementation and registration
FBBoxImplementation	(	ORBOXVECTOR__CLASS		);		// Box class name
FBRegisterBox		(	ORBOXVECTOR__NAME,				// Unique name to register box.
						ORBOXVECTOR__CLASS,				// Box class name
						ORBOXVECTOR__LOCATION,			// Box location ('plugins')
						ORBOXVECTOR__LABEL,				// Box label (name of box to display)
						ORBOXVECTOR__DESC,			// Box long description.
						FB_DEFAULT_SDK_ICON		);	// Icon filename (default=Open Reality icon)

/************************************************
 *	Creation
 ************************************************/
bool ORBoxVector::FBCreate()
{
	// Input Node
	mRecordToggle	= AnimationNodeInCreate  ( 0, "[0] process to file; bool toggle", ANIMATIONNODE_TYPE_NUMBER );
	mTimeFrame		= AnimationNodeInCreate  ( 0, "[0] frame number", ANIMATIONNODE_TYPE_NUMBER );
    mCharRootPos	= AnimationNodeInCreate  ( 0, "[1-0] character root pos; to record root velocity vector", ANIMATIONNODE_TYPE_VECTOR );
	mCharRootRotY	= AnimationNodeInCreate  ( 0, "[1-1] character root rot(Y); to record root rotation delta in 0.5sec", ANIMATIONNODE_TYPE_NUMBER );
	mHipsPos		= AnimationNodeInCreate  ( 0, "[2-0] hips pos; to record pos in frame(i), posOffset in 0.5sec and 2sec toward root in frame(i), velocity (interpolation of +-2frames) for t0, 0.5sec and 2sec", ANIMATIONNODE_TYPE_VECTOR );
	mHipsRotY		= AnimationNodeInCreate  ( 0, "[2-1] hips rot(Y); to record rotation delta (interpolation of +-2frames) for t0, 0.5sec and 2sec", ANIMATIONNODE_TYPE_NUMBER );
	mFootLPos		= AnimationNodeInCreate  ( 0, "[3-0] foot left pos; pose matching data along with hips pos and rot", ANIMATIONNODE_TYPE_VECTOR );
	mFootRPos		= AnimationNodeInCreate  ( 0, "[3-1] foot right pos; pose matching data along with hips pos and rot", ANIMATIONNODE_TYPE_VECTOR );
	mElbowLPos		= AnimationNodeInCreate  ( 0, "[3-2] elbow left pos; pose matching data along with hips pos and rot", ANIMATIONNODE_TYPE_VECTOR );
	mElbowRPos		= AnimationNodeInCreate  ( 0, "[3-3] elbow right pos; pose matching data along with hips pos and rot", ANIMATIONNODE_TYPE_VECTOR );
	mFinalizeCacheCompute		= AnimationNodeInCreate  ( 0, "[4] finilize cache computation; bool toggle, to compute data cache data in second itteration", ANIMATIONNODE_TYPE_NUMBER );
	mOutputFinilizedDebugInfo	= AnimationNodeInCreate ( 0, "[5] output finilized pose data for debug", ANIMATIONNODE_TYPE_NUMBER );
	TimesliderUpdate =1; Itteration2Done = 0;
	// Output Nodes
    mIsRecorded			= AnimationNodeOutCreate ( 0, "[0] Recording to 'D:\AnimDatabaseCache.xml' ", ANIMATIONNODE_TYPE_NUMBER );
	mCharRootPosDebug	= AnimationNodeOutCreate ( 0, "[01] CharRootPos",	ANIMATIONNODE_TYPE_VECTOR );
	mCharRootRotYDebug	= AnimationNodeOutCreate ( 0, "[02] CharRootRotY",	ANIMATIONNODE_TYPE_NUMBER );
	mHipsPosDebug		= AnimationNodeOutCreate ( 0, "[03] HipsPos",		ANIMATIONNODE_TYPE_VECTOR );
	mHipsRotYDebug		= AnimationNodeOutCreate ( 0, "[04] HipsRotY' ",	ANIMATIONNODE_TYPE_NUMBER );
	mFootLPosDebug		= AnimationNodeOutCreate ( 0, "[05] FootLPos",		ANIMATIONNODE_TYPE_VECTOR );
	mFootRPosDebug		= AnimationNodeOutCreate ( 0, "[06] FootRPos",		ANIMATIONNODE_TYPE_VECTOR );
	mElbowLPosDebug		= AnimationNodeOutCreate ( 0, "[07] ElbowLPos",		ANIMATIONNODE_TYPE_VECTOR );
	mElbowRPosDebug		= AnimationNodeOutCreate ( 0, "[08] ElbowRPos",		ANIMATIONNODE_TYPE_VECTOR );
	mRootRotDeltaDebug	= AnimationNodeOutCreate ( 0, "[09] RootRotDelta",	ANIMATIONNODE_TYPE_NUMBER );
	mHipRotDeltaDebug	= AnimationNodeOutCreate ( 0, "[10] HipRotDelta",	ANIMATIONNODE_TYPE_NUMBER );
	mCharRootSpeedDebug	= AnimationNodeOutCreate ( 0, "[11] CharRootSpeed", ANIMATIONNODE_TYPE_NUMBER );
	mHipsSpeedDebug		= AnimationNodeOutCreate ( 0, "[12] HipsSpeed",		ANIMATIONNODE_TYPE_NUMBER );
	mFootLSpeedDebug	= AnimationNodeOutCreate ( 0, "[13] FootLSpeed",	ANIMATIONNODE_TYPE_NUMBER );
	mFootRSpeedDebug	= AnimationNodeOutCreate ( 0, "[14] FootRSpeed",	ANIMATIONNODE_TYPE_NUMBER );
	mElbowLSpeedDebug	= AnimationNodeOutCreate ( 0, "[15] ElbowLSpeed",	ANIMATIONNODE_TYPE_NUMBER );
	mElbowRSpeedDebug	= AnimationNodeOutCreate ( 0, "[16] ElbowRSpeed",	ANIMATIONNODE_TYPE_NUMBER );
	mCharRootVelocityVec_F0Debug	= AnimationNodeOutCreate ( 0, "[17] CharRootVelocityVec_+F01",		ANIMATIONNODE_TYPE_VECTOR );
	mCharRootVelocityVec_F15Debug	= AnimationNodeOutCreate ( 0, "[18] CharRootVelocityVec_+F15",		ANIMATIONNODE_TYPE_VECTOR );
	mCharRootVelocityVec_F60Debug	= AnimationNodeOutCreate ( 0, "[19] CharRootVelocityVec_+F60",		ANIMATIONNODE_TYPE_VECTOR );

 //  mY = AnimationNodeOutCreate ( 0, "Y", ANIMATIONNODE_TYPE_NUMBER );
 //  mZ = AnimationNodeOutCreate ( 0, "Z", ANIMATIONNODE_TYPE_NUMBER );

//-------------xml initialization---------------
	hr = CoInitialize(NULL);
    if (SUCCEEDED(hr))
    {
        try
        {
            hr = xmlDoc.CreateInstance(__uuidof(MSXML::DOMDocument60), NULL, CLSCTX_INPROC_SERVER);			
			RecordingStatusOutput = 1;
		}
        catch (_com_error &e)
        {
           printf("ERROR: %ws\n", e.ErrorMessage());
        }
	}
	else
	{
		RecordingStatusOutput = 0;
	}

//-----------------------------------------------
	return true;
}
/************************************************
 *	Destruction.
 ************************************************/
void ORBoxVector::FBDestroy()
{
	CoUninitialize();
}
/************************************************
 *	Real-time engine evaluation
 ************************************************/
bool ORBoxVector::AnimationNodeNotify( FBAnimationNode* pAnimationNode, FBEvaluateInfo* pEvaluateInfo )
{
	bool	lStatus;	// Status of input node
	// Read the data from the input node
	lStatus = mRecordToggle->ReadData( &lRecordToggle, pEvaluateInfo );						   // TODO: lStatus proper check
	lStatus = mTimeFrame->ReadData( &lTimeFrame, pEvaluateInfo );							   // TODO: lStatus proper check
	lStatus = mCharRootPos->ReadData( lCharRootPos, pEvaluateInfo );						   // TODO: lStatus proper check
	lStatus = mCharRootRotY->ReadData( &lCharRootRotY, pEvaluateInfo );						   // TODO: lStatus proper check
	lStatus = mHipsPos->ReadData( lHipsPos, pEvaluateInfo );								   // TODO: lStatus proper check
	lStatus = mHipsRotY->ReadData( &lHipsRotY, pEvaluateInfo );								   // TODO: lStatus proper check
	lStatus = mFootLPos->ReadData( lFootLPos, pEvaluateInfo );								   // TODO: lStatus proper check
	lStatus = mFootRPos->ReadData( lFootRPos, pEvaluateInfo );								   // TODO: lStatus proper check
	lStatus = mElbowLPos->ReadData( lElbowLPos, pEvaluateInfo );							   // TODO: lStatus proper check
	lStatus = mElbowRPos->ReadData( lElbowRPos, pEvaluateInfo );							   // TODO: lStatus proper check
	lStatus = mFinalizeCacheCompute->ReadData( &lFinalizeCacheCompute, pEvaluateInfo );		   // TODO: lStatus proper check
	lStatus = mOutputFinilizedDebugInfo->ReadData( &lOutputFinilizedDebugInfo, pEvaluateInfo );
	// If the read was not from a dead node.
    if( lStatus )
	{
	RecordingStatusOutput = 0;
	if (lRecordToggle && (!lFinalizeCacheCompute))
	{
		RecordData_Pass1 (L"D:/AnimDatabaseCache.xml",1);
	}
	if (lFinalizeCacheCompute && (!lRecordToggle))
	{
		RecordData_Pass2 (L"D:/AnimDatabaseCache.xml",1);				
	}

	mIsRecorded->WriteData( &RecordingStatusOutput, pEvaluateInfo);
	DebugPoseDataOutput (pEvaluateInfo);  // D:/AnimDatabaseCache.xml wstring inside 


	//	mY->WriteData( &lVector[1], pEvaluateInfo);
	//	mZ->WriteData( &lVector[2], pEvaluateInfo);
	    return true;
	}
    return false;
}
//-----------------------------------------
bool ORBoxVector::RecordData_Pass1 (std::wstring lFileName, bool RecordingToggle)
{
	std::wstring wstrFrame = L"frame ";
	BSTR bstrFileName = SysAllocStringLen(lFileName.data(), lFileName.size());
	BSTR bstrFrame = SysAllocStringLen(wstrFrame.data(), wstrFrame.size());
	if (TimesliderUpdate!= lTimeFrame)
	{
		TimesliderUpdate = lTimeFrame;
		long i = (long)lTimeFrame;
		std::wstring wstrFrameNumbered = wstrFrame + std::to_wstring(i);	
		BSTR bstrFrameNumbered = SysAllocStringLen(wstrFrameNumbered.data(), wstrFrameNumbered.size());
		if (!CheckFrameDataExistInXml (bstrFileName, bstrFrameNumbered))
		{
			WriteFrameToXml (bstrFileName, bstrFrameNumbered);
		};
	}
	return 1;
}
//-----------------------------------------
bool ORBoxVector:: WriteFrameToXml (BSTR  lXmlFileName, BSTR  lFrameNumber)
{
       if (xmlDoc->load(_T(lXmlFileName)) != VARIANT_TRUE)
       {
			printf("Unable to load input.xml\n");
			RecordingStatusOutput = 0;
       }
       else
       {
	   //---------create frame node---------------------
           xmlDoc->setProperty("SelectionLanguage", "XPath");
           MSXML::IXMLDOMNodePtr FrameNode;
		   FrameNode = xmlDoc->createNode(MSXML::NODE_ELEMENT, _T("frame"), _T(""));
		   FrameNode->text = _T(lFrameNumber);

		   //--------create Character Root Pos--------------
		   MSXML::IXMLDOMElementPtr element01;		 
		   MSXML::IXMLDOMAttributePtr attribute01_1;  
		   MSXML::IXMLDOMAttributePtr attribute01_2; 
		   MSXML::IXMLDOMAttributePtr attribute01_3; 
		   element01 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("CharRootPos"), _T(""));
		   attribute01_1 = xmlDoc->createAttribute (_T("x"));
		   attribute01_2 = xmlDoc->createAttribute (_T("y"));
		   attribute01_3 = xmlDoc->createAttribute (_T("z"));
		   attribute01_1->value = _T(lCharRootPos[0]);
		   attribute01_2->value = _T(lCharRootPos[1]);
		   attribute01_3->value = _T(lCharRootPos[2]);
		   element01->setAttributeNode (attribute01_1);
		   element01->setAttributeNode (attribute01_2);
		   element01->setAttributeNode (attribute01_3);

		   //--------create Character Root Rot--------------
		   MSXML::IXMLDOMElementPtr element02;		 
		   MSXML::IXMLDOMAttributePtr attribute02_1;  
		   element02 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("CharRootRot"), _T(""));
		   attribute02_1 = xmlDoc->createAttribute (_T("y"));
		   attribute02_1->value = _T(lCharRootRotY);
		   element02->setAttributeNode (attribute02_1);

		   //--------create Hips Pos--------------
		   MSXML::IXMLDOMElementPtr element03;		 
		   MSXML::IXMLDOMAttributePtr attribute03_1;  
		   MSXML::IXMLDOMAttributePtr attribute03_2; 
		   MSXML::IXMLDOMAttributePtr attribute03_3; 
		   element03 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("HipsPos"), _T(""));
		   attribute03_1 = xmlDoc->createAttribute (_T("x"));
		   attribute03_2 = xmlDoc->createAttribute (_T("y"));
		   attribute03_3 = xmlDoc->createAttribute (_T("z"));
		   attribute03_1->value = _T(lHipsPos[0]);
		   attribute03_2->value = _T(lHipsPos[1]);
		   attribute03_3->value = _T(lHipsPos[2]);
		   element03->setAttributeNode (attribute03_1);
		   element03->setAttributeNode (attribute03_2);
		   element03->setAttributeNode (attribute03_3);

		   //--------create Hips Rot --------------
		   MSXML::IXMLDOMElementPtr element04;		 
		   MSXML::IXMLDOMAttributePtr attribute04_1;  
		   element04 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("HipsRot"), _T(""));
		   attribute04_1 = xmlDoc->createAttribute (_T("y"));
		   attribute04_1->value = _T(lHipsRotY);
		   element04->setAttributeNode (attribute04_1);

		   //--------create  FootLPos --------------
		   MSXML::IXMLDOMElementPtr element05;		 
		   MSXML::IXMLDOMAttributePtr attribute05_1;  
		   MSXML::IXMLDOMAttributePtr attribute05_2; 
		   MSXML::IXMLDOMAttributePtr attribute05_3; 
		   element05 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("FootLPos"), _T(""));
		   attribute05_1 = xmlDoc->createAttribute (_T("x"));
		   attribute05_2 = xmlDoc->createAttribute (_T("y"));
		   attribute05_3 = xmlDoc->createAttribute (_T("z"));
		   attribute05_1->value = _T(lFootLPos[0]);
		   attribute05_2->value = _T(lFootLPos[1]);
		   attribute05_3->value = _T(lFootLPos[2]);
		   element05->setAttributeNode (attribute05_1);
		   element05->setAttributeNode (attribute05_2);
		   element05->setAttributeNode (attribute05_3);

		   //--------create  FootRPos--------------
		   MSXML::IXMLDOMElementPtr element06;		 
		   MSXML::IXMLDOMAttributePtr attribute06_1;  
		   MSXML::IXMLDOMAttributePtr attribute06_2; 
		   MSXML::IXMLDOMAttributePtr attribute06_3; 
		   element06 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("FootRPos"), _T(""));
		   attribute06_1 = xmlDoc->createAttribute (_T("x"));
		   attribute06_2 = xmlDoc->createAttribute (_T("y"));
		   attribute06_3 = xmlDoc->createAttribute (_T("z"));
		   attribute06_1->value = _T(lFootRPos[0]);
		   attribute06_2->value = _T(lFootRPos[1]);
		   attribute06_3->value = _T(lFootRPos[2]);
		   element06->setAttributeNode (attribute06_1);
		   element06->setAttributeNode (attribute06_2);
		   element06->setAttributeNode (attribute06_3);

		   //--------create  ElbowLPos--------------
		   MSXML::IXMLDOMElementPtr element07;		 
		   MSXML::IXMLDOMAttributePtr attribute07_1;  
		   MSXML::IXMLDOMAttributePtr attribute07_2; 
		   MSXML::IXMLDOMAttributePtr attribute07_3; 
		   element07 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("ElbowLPos"), _T(""));
		   attribute07_1 = xmlDoc->createAttribute (_T("x"));
		   attribute07_2 = xmlDoc->createAttribute (_T("y"));
		   attribute07_3 = xmlDoc->createAttribute (_T("z"));
		   attribute07_1->value = _T(lElbowLPos[0]);
		   attribute07_2->value = _T(lElbowLPos[1]);
		   attribute07_3->value = _T(lElbowLPos[2]);
		   element07->setAttributeNode (attribute07_1);
		   element07->setAttributeNode (attribute07_2);
		   element07->setAttributeNode (attribute07_3);

		   //--------create  ElbowRPos--------------
		   MSXML::IXMLDOMElementPtr element08;		 
		   MSXML::IXMLDOMAttributePtr attribute08_1;  
		   MSXML::IXMLDOMAttributePtr attribute08_2; 
		   MSXML::IXMLDOMAttributePtr attribute08_3; 
		   element08 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("ElbowRPos"), _T(""));
		   attribute08_1 = xmlDoc->createAttribute (_T("x"));
		   attribute08_2 = xmlDoc->createAttribute (_T("y"));
		   attribute08_3 = xmlDoc->createAttribute (_T("z"));
		   attribute08_1->value = _T(lElbowRPos[0]);
		   attribute08_2->value = _T(lElbowRPos[1]);
		   attribute08_3->value = _T(lElbowRPos[2]);
		   element08->setAttributeNode (attribute08_1);
		   element08->setAttributeNode (attribute08_2);
		   element08->setAttributeNode (attribute08_3);

		   //---------compose frame node-----------------
		   FrameNode->appendChild (element01);
		   FrameNode->appendChild (element02);
		   FrameNode->appendChild (element03);
		   FrameNode->appendChild (element04);
		   FrameNode->appendChild (element05);
		   FrameNode->appendChild (element06);
		   FrameNode->appendChild (element07);
		   FrameNode->appendChild (element08);
           xmlDoc->documentElement->appendChild(FrameNode);
		   hr = xmlDoc->save(_T(lXmlFileName));
		   RecordingStatusOutput = 1;
        //   if (SUCCEEDED(hr))
        //       printf("successfully saved\n");
       }
   
  
    return RecordingStatusOutput;

}
//--------------------------------------
bool ORBoxVector::CheckFrameDataExistInXml ( BSTR  lXmlFileName, BSTR  lFrameNumber)
{
	bool Result=0;
    if (xmlDoc->load(_T(lXmlFileName)) != VARIANT_TRUE)
    {
		printf("Unable to load input.xml\n");
		RecordingStatusOutput = 0;
    }
    else
    {
		std::wstring FrameNodeTextSearch = std::wstring(lFrameNumber, SysStringLen(lFrameNumber));
		xmlDoc->setProperty("SelectionLanguage", "XPath");
		MSXML::IXMLDOMNodeListPtr lFrameNodeList;
		lFrameNodeList = xmlDoc->selectNodes("//frame");
		MSXML::IXMLDOMNodePtr lFrameIterator;
		long NodeCount = lFrameNodeList->Getlength ();
		for (long i=0; i<NodeCount; i++)
		{
			lFrameIterator = lFrameNodeList->Getitem (i);
			BSTR FrameNodeTextCurrent = lFrameIterator->Gettext();
			std::wstring wsFrameNodeTextCurrent = std::wstring(FrameNodeTextCurrent, SysStringLen(FrameNodeTextCurrent));
			if (FrameNodeTextSearch == wsFrameNodeTextCurrent) Result = 1;
		};
		RecordingStatusOutput = 1;
		/* --------- reading single node--------
		MSXML::IXMLDOMNodePtr lFrameNode;	
		lFrameNode = xmlDoc->selectSingleNode ("//frame");
		BSTR CheckText = lFrameNode->Gettext(); */

		/* -------------reading multiple nodes--------
		MSXML::IXMLDOMNodeListPtr lFrameNodeList;
		lFrameNodeList = xmlDoc->selectNodes("//frame");
		MSXML::IXMLDOMNodePtr lFrameIterator;
			lFrameIterator = lFrameNodeList->Getitem (i);
			BSTR FrameNodeTextCurrent = lFrameIterator->Gettext();*/

		/*   -----Reading attributes----------
		MSXML::IXMLDOMElementPtr lElem1;
		lElem1 = xmlDoc->selectSingleNode ("//frame/RootPos");
		BSTR CheckText2 = lElem1->Gettext ();
		VARIANT CheckText3 = lElem1->getAttribute ("x");*/

		/*  -----------Convert BSTR to string and vice versa----------
		std::wstring wstrFrameNumber = std::wstring(lFrameNumber, SysStringLen(lFrameNumber));
		BSTR bstrNodeFullPath = SysAllocStringLen(wstrFrameNumber.data(), wstrFrameNumber.size());
		*/
 	} 
	 	return Result;
}
//------------------------------------------------------
FramePoseData ORBoxVector::ReadFrameData (BSTR  lXmlFileName, BSTR  lFrameNumber)
{
	FramePoseData lFramePoseData;
	double ArrayReadBuff [256]; 	for (int i=0;i<256;i++) ArrayReadBuff[i]=0;  //i was young, i had no time
	if (xmlDoc->load(_T(lXmlFileName)) != VARIANT_TRUE)
    {
		printf("Unable to load input.xml\n");
		RecordingStatusOutput = 0;
    }
    else
    {
		std::wstring FrameNodeTextSearch = std::wstring(lFrameNumber, SysStringLen(lFrameNumber));
		xmlDoc->setProperty("SelectionLanguage", "XPath");
		MSXML::IXMLDOMNodeListPtr lFrameNodeList;
		lFrameNodeList = xmlDoc->selectNodes("//frame");
		MSXML::IXMLDOMNodePtr lFrameIterator;
		long NodeCount = lFrameNodeList->Getlength ();
		for (long i=0; i<NodeCount; i++)
		{
			lFrameIterator = lFrameNodeList->Getitem (i);
			BSTR FrameNodeTextCurrent = lFrameIterator->Gettext();
			std::wstring wsFrameNodeTextCurrent = std::wstring(FrameNodeTextCurrent, SysStringLen(FrameNodeTextCurrent));
			if (FrameNodeTextSearch == wsFrameNodeTextCurrent) 
			{				
				MSXML::IXMLDOMElementPtr element_buff;
				MSXML::IXMLDOMNodeListPtr lChildNodeList = lFrameIterator->GetchildNodes();
				long ChildNodesCount = lChildNodeList->Getlength();
				_variant_t DataReadBuff;
				for (long j=1,k=0;j<ChildNodesCount;j++)
				{
					element_buff = lChildNodeList->Getitem (j);
					BSTR ChildNodeName = element_buff->GetnodeName();
					_variant_t ExistCheck = element_buff->getAttribute ("x");
					if (SysStringLen(ExistCheck.bstrVal))
					{
						DataReadBuff = element_buff->getAttribute ("x");
						ArrayReadBuff[k++]=double(DataReadBuff);
						DataReadBuff = element_buff->getAttribute ("y");
						ArrayReadBuff[k++]=double(DataReadBuff);
						DataReadBuff = element_buff->getAttribute ("z");
						ArrayReadBuff[k++]=double(DataReadBuff);
					}
					else
					{
						ExistCheck = element_buff->getAttribute ("y");
						if (SysStringLen(ExistCheck.bstrVal))
						{
							DataReadBuff = element_buff->getAttribute ("y");
							ArrayReadBuff[k++]=double(DataReadBuff);
						};
					}		
					ExistCheck = element_buff->getAttribute ("delta");
					if (SysStringLen(ExistCheck.bstrVal))
					{
						DataReadBuff = element_buff->getAttribute ("delta");
						ArrayReadBuff[k++]=double(DataReadBuff);
					};
					ExistCheck = element_buff->getAttribute ("speed");
					if (SysStringLen(ExistCheck.bstrVal))
					{
						DataReadBuff = element_buff->getAttribute ("speed");
						ArrayReadBuff[k++]=double(DataReadBuff);
					};

				}
			break;
			}
		}
	}
	std::wstring FrameNodeTextSearch = std::wstring(lFrameNumber, SysStringLen(lFrameNumber));
	std::wstring FrameNumberDigit = FrameNodeTextSearch.erase(0,6);
	lFramePoseData.lTimeFrame = stod(FrameNumberDigit);
	int tmpCounter = 0;
	lFramePoseData.lCharRootPos	[0]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lCharRootPos	[1]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lCharRootPos	[2]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lCharRootRotY	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lHipsPos	  [0]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lHipsPos	  [1]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lHipsPos	  [2]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lHipsRotY		= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lFootLPos  [0]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lFootLPos  [1]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lFootLPos  [2]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lFootRPos  [0]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lFootRPos  [1]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lFootRPos  [2]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lElbowLPos [0]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lElbowLPos [1]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lElbowLPos [2]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lElbowRPos [0]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lElbowRPos [1]	= ArrayReadBuff[tmpCounter++];
	lFramePoseData.lElbowRPos [2]	= ArrayReadBuff[tmpCounter++];

	lFramePoseData.l2CharRootVelocityVec_F0[0]   = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2CharRootVelocityVec_F0[1]   = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2CharRootVelocityVec_F0[2]   = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2CharRootVelocityVec_F15[0]  = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2CharRootVelocityVec_F15[1]  = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2CharRootVelocityVec_F15[2]  = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2CharRootVelocityVec_F60[0]  = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2CharRootVelocityVec_F60[1]  = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2CharRootVelocityVec_F60[2]  = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2RootRotDelta				 = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2HipRotDelta				 = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2CharRootSpeed				 = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2HipsSpeed					 = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2FootLSpeed					 = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2FootRSpeed					 = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2ElbowLSpeed				 = ArrayReadBuff[tmpCounter++];
	lFramePoseData.l2ElbowRSpeed				 = ArrayReadBuff[tmpCounter++];


	return lFramePoseData;
}
//----------------------------------------------------------------------------------------
double ORBoxVector::CalculateSpeedInterpolated (BSTR  lXmlFileName, BSTR  lFrameNumber, int BodyTypeEnum)
{
	double Interpolated1FrameDistance =0 ;
	FramePoseData F0, F1, F2, Fn1;
	double dblFrameF0=0, dblFrameF1=0, dblFrameF2=0, dblFrameFn1=0;
	std::wstring FrameNodeTextSearch = std::wstring(lFrameNumber, SysStringLen(lFrameNumber));
	std::wstring FrameNumberDigit = FrameNodeTextSearch.erase(0,6);
	dblFrameF0 = stod(FrameNumberDigit);

	std::wstring wstrFrame = L"frame ";
	std::wstring wstrFrameNumberedFn1 = wstrFrame + std::to_wstring(int(dblFrameF0-1));	
	BSTR bstrFrameNumberedFn1 = SysAllocStringLen(wstrFrameNumberedFn1.data(), wstrFrameNumberedFn1.size());
	std::wstring wstrFrameNumberedF1 = wstrFrame + std::to_wstring(int(dblFrameF0+1));	
	BSTR bstrFrameNumberedF1 = SysAllocStringLen(wstrFrameNumberedF1.data(), wstrFrameNumberedF1.size());
	std::wstring wstrFrameNumberedF2 = wstrFrame + std::to_wstring(int(dblFrameF0+2));	
	BSTR bstrFrameNumberedF2 = SysAllocStringLen(wstrFrameNumberedF2.data(), wstrFrameNumberedF2.size());

	if (dblFrameF0>0) {dblFrameFn1 = dblFrameF0 - 1;}	else {dblFrameFn1 = dblFrameF0;};
	if (CheckFrameDataExistInXml (lXmlFileName, bstrFrameNumberedF1)) {dblFrameF1 = dblFrameF0+1;} else {dblFrameF1 = dblFrameF0;};
	if (CheckFrameDataExistInXml (lXmlFileName, bstrFrameNumberedF2)) {dblFrameF2 = dblFrameF0+2;} else {dblFrameF2 = dblFrameF0;};

	F0 = ReadFrameData (lXmlFileName, lFrameNumber);
	if (dblFrameFn1 == dblFrameF0) {Fn1 = ReadFrameData (lXmlFileName, lFrameNumber);} else {Fn1 = ReadFrameData (lXmlFileName, bstrFrameNumberedFn1);};
	if (dblFrameF1 == dblFrameF0) {F1 = ReadFrameData (lXmlFileName, lFrameNumber);}   else	{F1 =  ReadFrameData (lXmlFileName, bstrFrameNumberedF1) ;};
	if (dblFrameF2 == dblFrameF0) {F2 = ReadFrameData (lXmlFileName, lFrameNumber);}   else {F2 =  ReadFrameData (lXmlFileName, bstrFrameNumberedF2) ;};

	switch (BodyTypeEnum)
	{
	case 1:
		{
		double distance [3];
		double f0x,f0y,f0z,f1x,f1y,f1z,f2x,f2y,f2z,fn1x,fn1y,fn1z;
		f0x=F0.lCharRootPos[0];f0y=F0.lCharRootPos[1];f0z=F0.lCharRootPos[2];f1x=F1.lCharRootPos[0];f1y=F1.lCharRootPos[1];f1z=F1.lCharRootPos[2];
		f2x=F2.lCharRootPos[0];f2y=F2.lCharRootPos[1];f2z=F2.lCharRootPos[2];fn1x=Fn1.lCharRootPos[0];fn1y=Fn1.lCharRootPos[1];fn1z=Fn1.lCharRootPos[2];
		distance [0] = sqrt (pow((f0x-f1x),2.0)+pow((f0y-f1y),2.0)+pow((f0z-f1z),2.0));
		distance [1] = sqrt (pow((f1x-f2x),2.0)+pow((f1y-f2y),2.0)+pow((f1z-f2z),2.0));
		distance [2] = sqrt (pow((fn1x-f0x),2.0)+pow((fn1y-f0y),2.0)+pow((fn1z-f0z),2.0));
		Interpolated1FrameDistance = (distance[0]+distance[1]+distance[2])/3.0;
		break; 
		}
	case 2:
		{
		double distance [3];
		double f0x,f0y,f0z,f1x,f1y,f1z,f2x,f2y,f2z,fn1x,fn1y,fn1z;
		f0x=F0.lHipsPos[0];f0y=F0.lHipsPos[1];f0z=F0.lHipsPos[2];f1x=F1.lHipsPos[0];f1y=F1.lHipsPos[1];f1z=F1.lHipsPos[2];
		f2x=F2.lHipsPos[0];f2y=F2.lHipsPos[1];f2z=F2.lHipsPos[2];fn1x=Fn1.lHipsPos[0];fn1y=Fn1.lHipsPos[1];fn1z=Fn1.lHipsPos[2];
		distance [0] = sqrt (pow((f0x-f1x),2.0)+pow((f0y-f1y),2.0)+pow((f0z-f1z),2.0));
		distance [1] = sqrt (pow((f1x-f2x),2.0)+pow((f1y-f2y),2.0)+pow((f1z-f2z),2.0));
		distance [2] = sqrt (pow((fn1x-f0x),2.0)+pow((fn1y-f0y),2.0)+pow((fn1z-f0z),2.0));
		Interpolated1FrameDistance = (distance[0]+distance[1]+distance[2])/3.0;	
		break; 
		}
	case 3:
		{
		double distance [3];
		double f0x,f0y,f0z,f1x,f1y,f1z,f2x,f2y,f2z,fn1x,fn1y,fn1z;
		f0x=F0.lFootLPos[0];f0y=F0.lFootLPos[1];f0z=F0.lFootLPos[2];f1x=F1.lFootLPos[0];f1y=F1.lFootLPos[1];f1z=F1.lFootLPos[2];
		f2x=F2.lFootLPos[0];f2y=F2.lFootLPos[1];f2z=F2.lFootLPos[2];fn1x=Fn1.lFootLPos[0];fn1y=Fn1.lFootLPos[1];fn1z=Fn1.lFootLPos[2];
		distance [0] = sqrt (pow((f0x-f1x),2.0)+pow((f0y-f1y),2.0)+pow((f0z-f1z),2.0));
		distance [1] = sqrt (pow((f1x-f2x),2.0)+pow((f1y-f2y),2.0)+pow((f1z-f2z),2.0));
		distance [2] = sqrt (pow((fn1x-f0x),2.0)+pow((fn1y-f0y),2.0)+pow((fn1z-f0z),2.0));
		Interpolated1FrameDistance = (distance[0]+distance[1]+distance[2])/3.0;		
		break; 
		}
	case 4:
		{
		double distance [3];
		double f0x,f0y,f0z,f1x,f1y,f1z,f2x,f2y,f2z,fn1x,fn1y,fn1z;
		f0x=F0.lFootRPos[0];f0y=F0.lFootRPos[1];f0z=F0.lFootRPos[2];f1x=F1.lFootRPos[0];f1y=F1.lFootRPos[1];f1z=F1.lFootRPos[2];
		f2x=F2.lFootRPos[0];f2y=F2.lFootRPos[1];f2z=F2.lFootRPos[2];fn1x=Fn1.lFootRPos[0];fn1y=Fn1.lFootRPos[1];fn1z=Fn1.lFootRPos[2];
		distance [0] = sqrt (pow((f0x-f1x),2.0)+pow((f0y-f1y),2.0)+pow((f0z-f1z),2.0));
		distance [1] = sqrt (pow((f1x-f2x),2.0)+pow((f1y-f2y),2.0)+pow((f1z-f2z),2.0));
		distance [2] = sqrt (pow((fn1x-f0x),2.0)+pow((fn1y-f0y),2.0)+pow((fn1z-f0z),2.0));
		Interpolated1FrameDistance = (distance[0]+distance[1]+distance[2])/3.0;		
		break; 
		}
	case 5:
		{
		double distance [3];
		double f0x,f0y,f0z,f1x,f1y,f1z,f2x,f2y,f2z,fn1x,fn1y,fn1z;
		f0x=F0.lElbowLPos[0];f0y=F0.lElbowLPos[1];f0z=F0.lElbowLPos[2];f1x=F1.lElbowLPos[0];f1y=F1.lElbowLPos[1];f1z=F1.lElbowLPos[2];
		f2x=F2.lElbowLPos[0];f2y=F2.lElbowLPos[1];f2z=F2.lElbowLPos[2];fn1x=Fn1.lElbowLPos[0];fn1y=Fn1.lElbowLPos[1];fn1z=Fn1.lElbowLPos[2];
		distance [0] = sqrt (pow((f0x-f1x),2.0)+pow((f0y-f1y),2.0)+pow((f0z-f1z),2.0));
		distance [1] = sqrt (pow((f1x-f2x),2.0)+pow((f1y-f2y),2.0)+pow((f1z-f2z),2.0));
		distance [2] = sqrt (pow((fn1x-f0x),2.0)+pow((fn1y-f0y),2.0)+pow((fn1z-f0z),2.0));
		Interpolated1FrameDistance = (distance[0]+distance[1]+distance[2])/3.0;	
		break; 
		}
	case 6:
		{
		double distance [3];
		double f0x,f0y,f0z,f1x,f1y,f1z,f2x,f2y,f2z,fn1x,fn1y,fn1z;
		f0x=F0.lElbowRPos[0];f0y=F0.lElbowRPos[1];f0z=F0.lElbowRPos[2];f1x=F1.lElbowRPos[0];f1y=F1.lElbowRPos[1];f1z=F1.lElbowRPos[2];
		f2x=F2.lElbowRPos[0];f2y=F2.lElbowRPos[1];f2z=F2.lElbowRPos[2];fn1x=Fn1.lElbowRPos[0];fn1y=Fn1.lElbowRPos[1];fn1z=Fn1.lElbowRPos[2];
		distance [0] = sqrt (pow((f0x-f1x),2.0)+pow((f0y-f1y),2.0)+pow((f0z-f1z),2.0));
		distance [1] = sqrt (pow((f1x-f2x),2.0)+pow((f1y-f2y),2.0)+pow((f1z-f2z),2.0));
		distance [2] = sqrt (pow((fn1x-f0x),2.0)+pow((fn1y-f0y),2.0)+pow((fn1z-f0z),2.0));
		Interpolated1FrameDistance = (distance[0]+distance[1]+distance[2])/3.0;		
		break; 
		}
	default: 
		{
		Interpolated1FrameDistance = 0;
		}
	};
	return Interpolated1FrameDistance;
}
//----------------------------------------------------------------------------------------
double ORBoxVector::CalculateAngleChangeInterpolated (BSTR  lXmlFileName, BSTR  lFrameNumber, int BodyTypeEnum)
{
	double Interpolated1FrameAngleDelta =0 ;
	FramePoseData F0, F1, F2, Fn1;
	double dblFrameF0=0, dblFrameF1=0, dblFrameF2=0, dblFrameFn1=0;
	std::wstring FrameNodeTextSearch = std::wstring(lFrameNumber, SysStringLen(lFrameNumber));
	std::wstring FrameNumberDigit = FrameNodeTextSearch.erase(0,6);
	dblFrameF0 = stod(FrameNumberDigit);

	std::wstring wstrFrame = L"frame ";
	std::wstring wstrFrameNumberedFn1 = wstrFrame + std::to_wstring(int(dblFrameF0-1));	
	BSTR bstrFrameNumberedFn1 = SysAllocStringLen(wstrFrameNumberedFn1.data(), wstrFrameNumberedFn1.size());
	std::wstring wstrFrameNumberedF1 = wstrFrame + std::to_wstring(int(dblFrameF0+1));	
	BSTR bstrFrameNumberedF1 = SysAllocStringLen(wstrFrameNumberedF1.data(), wstrFrameNumberedF1.size());
	std::wstring wstrFrameNumberedF2 = wstrFrame + std::to_wstring(int(dblFrameF0+2));	
	BSTR bstrFrameNumberedF2 = SysAllocStringLen(wstrFrameNumberedF2.data(), wstrFrameNumberedF2.size());

	if (dblFrameF0>0) {dblFrameFn1 = dblFrameF0 - 1;}	else {dblFrameFn1 = dblFrameF0;};
	if (CheckFrameDataExistInXml (lXmlFileName, bstrFrameNumberedF1)) {dblFrameF1 = dblFrameF0+1;} else {dblFrameF1 = dblFrameF0;};
	if (CheckFrameDataExistInXml (lXmlFileName, bstrFrameNumberedF2)) {dblFrameF2 = dblFrameF0+2;} else {dblFrameF2 = dblFrameF0;};

	F0 = ReadFrameData (lXmlFileName, lFrameNumber);
	if (dblFrameFn1 == dblFrameF0) {Fn1 = ReadFrameData (lXmlFileName, lFrameNumber);} else {Fn1 = ReadFrameData (lXmlFileName, bstrFrameNumberedFn1);};
	if (dblFrameF1 == dblFrameF0) {F1 = ReadFrameData (lXmlFileName, lFrameNumber);}   else	{F1 =  ReadFrameData (lXmlFileName, bstrFrameNumberedF1) ;};
	if (dblFrameF2 == dblFrameF0) {F2 = ReadFrameData (lXmlFileName, lFrameNumber);}   else {F2 =  ReadFrameData (lXmlFileName, bstrFrameNumberedF2) ;};

	switch (BodyTypeEnum)
	{
	case 1:
		{
		double angleDelta [3];
		double f0_angle = F0.lCharRootRotY, f1_angle = F1.lCharRootRotY, f2_angle = F2.lCharRootRotY, fn1_angle = Fn1.lCharRootRotY;
		//TODO: add Gimbal unbinding for clear values
		angleDelta [0] = f1_angle-f0_angle;
	 	angleDelta [1] = f2_angle-f1_angle;
	 	angleDelta [2] = f0_angle-fn1_angle;
		Interpolated1FrameAngleDelta = (angleDelta[0]+angleDelta[1]+angleDelta[2])/3.0;
		break; 
		}
	case 2:
		{
		double angleDelta [3];
		double f0_angle = F0.lHipsRotY, f1_angle = F1.lHipsRotY, f2_angle = F2.lHipsRotY, fn1_angle = Fn1.lHipsRotY;
		//TODO: add Gimbal unbinding for clear values
		angleDelta [0] = f1_angle-f0_angle;
	 	angleDelta [1] = f2_angle-f1_angle;
	 	angleDelta [2] = f0_angle-fn1_angle;
		Interpolated1FrameAngleDelta = (angleDelta[0]+angleDelta[1]+angleDelta[2])/3.0;
		break; 
		}
	default: 
		{
		Interpolated1FrameAngleDelta = 0;
		}
	};
	return Interpolated1FrameAngleDelta;
}
//----------------------------------------------------------------------------------------
FramePoseData ORBoxVector::CalculateRootVelocityAt3Points (BSTR  lXmlFileName, BSTR  lFrameNumber)
{
	FramePoseData CurrentFrame = ReadFrameData (lXmlFileName, lFrameNumber);
	FramePoseData F0, F1, F15, F60;
	double dblFrameF0=0, dblFrameF1=0, dblFrameF15=0, dblFrameF60=0;
	std::wstring FrameNodeTextSearch = std::wstring(lFrameNumber, SysStringLen(lFrameNumber));
	std::wstring FrameNumberDigit = FrameNodeTextSearch.erase(0,6);
	dblFrameF0 = stod(FrameNumberDigit);

	std::wstring wstrFrame = L"frame ";
	std::wstring wstrFrameNumberedF1 = wstrFrame + std::to_wstring(int(dblFrameF0+1));	
	BSTR bstrFrameNumberedF1 = SysAllocStringLen(wstrFrameNumberedF1.data(), wstrFrameNumberedF1.size());
	std::wstring wstrFrameNumberedF15 = wstrFrame + std::to_wstring(int(dblFrameF0+15));	
	BSTR bstrFrameNumberedF15 = SysAllocStringLen(wstrFrameNumberedF15.data(), wstrFrameNumberedF15.size());
	std::wstring wstrFrameNumberedF60 = wstrFrame + std::to_wstring(int(dblFrameF0+60));	
	BSTR bstrFrameNumberedF60 = SysAllocStringLen(wstrFrameNumberedF60.data(), wstrFrameNumberedF60.size());

	//if (dblFrameF0>0) {dblFrameFn1 = dblFrameF0 - 1;}	else {dblFrameFn1 = dblFrameF0;};
	if (CheckFrameDataExistInXml (lXmlFileName, bstrFrameNumberedF1)) {dblFrameF1 = dblFrameF0+1;} else {dblFrameF1 = dblFrameF0;};
	if (CheckFrameDataExistInXml (lXmlFileName, bstrFrameNumberedF15)) {dblFrameF15 = dblFrameF0+15;} else {dblFrameF15 = dblFrameF0;};  //TODO proper iteration cut to get final frame value, otherwise 2 last seconds of database is waste
	if (CheckFrameDataExistInXml (lXmlFileName, bstrFrameNumberedF60)) {dblFrameF60 = dblFrameF0+60;} else {dblFrameF60 = dblFrameF0;};  //TODO proper iteration cut to get final frame value, otherwise 2 last seconds of database is waste

	F0 = ReadFrameData (lXmlFileName, lFrameNumber);
	if (dblFrameF1 == dblFrameF0)  {F1 = ReadFrameData (lXmlFileName, lFrameNumber);}  else	{F1 =  ReadFrameData (lXmlFileName, bstrFrameNumberedF1) ;};
	if (dblFrameF15 == dblFrameF0) {F15 = ReadFrameData (lXmlFileName, lFrameNumber);} else {F15 = ReadFrameData (lXmlFileName, bstrFrameNumberedF15);};
	if (dblFrameF60 == dblFrameF0) {F60 = ReadFrameData (lXmlFileName, lFrameNumber);} else {F60 = ReadFrameData (lXmlFileName, bstrFrameNumberedF60);};

	for (int i=0; i<3;i++) {CurrentFrame.l2CharRootVelocityVec_F0[i] = F1.lCharRootPos[i] - F0.lCharRootPos[i];};
	for (int i=0; i<3;i++) {CurrentFrame.l2CharRootVelocityVec_F15[i] = F15.lCharRootPos[i] - F0.lCharRootPos[i];};
	for (int i=0; i<3;i++) {CurrentFrame.l2CharRootVelocityVec_F60[i] = F60.lCharRootPos[i] - F0.lCharRootPos[i];};

	return CurrentFrame;
}
//----------------------------------------------------------------------------------------
bool ORBoxVector::Append2ndIttDataFrame (BSTR  lXmlFileName, BSTR  lFrameNumber)
{
	   if (xmlDoc->load(_T(lXmlFileName)) != VARIANT_TRUE)
       {
			printf("Unable to load input.xml\n");
			RecordingStatusOutput = 0;
			return RecordingStatusOutput;
       }
       else
		{
		std::wstring FrameNodeTextSearch = std::wstring(lFrameNumber, SysStringLen(lFrameNumber));
		xmlDoc->setProperty("SelectionLanguage", "XPath");
		MSXML::IXMLDOMNodeListPtr lFrameNodeList;
		lFrameNodeList = xmlDoc->selectNodes("//frame");
		MSXML::IXMLDOMNodePtr lFrameIterator;
		long NodeCount = lFrameNodeList->Getlength ();
		for (long i=0; i<NodeCount; i++)
		{
			lFrameIterator = lFrameNodeList->Getitem (i);
			BSTR FrameNodeTextCurrent = lFrameIterator->Gettext();
			std::wstring wsFrameNodeTextCurrent = std::wstring(FrameNodeTextCurrent, SysStringLen(FrameNodeTextCurrent));
			if (FrameNodeTextSearch == wsFrameNodeTextCurrent) 
			{		
					FramePoseData FrameData2ndItt = CalculateRootVelocityAt3Points (lXmlFileName, lFrameNumber);
					FrameData2ndItt.l2RootRotDelta = CalculateAngleChangeInterpolated (lXmlFileName, lFrameNumber, 1);
					FrameData2ndItt.l2HipRotDelta =  CalculateAngleChangeInterpolated(lXmlFileName, lFrameNumber, 2);
					FrameData2ndItt.l2CharRootSpeed = CalculateSpeedInterpolated (lXmlFileName, lFrameNumber, 1);
					FrameData2ndItt.l2HipsSpeed = CalculateSpeedInterpolated (lXmlFileName, lFrameNumber, 2);
					FrameData2ndItt.l2FootLSpeed = CalculateSpeedInterpolated (lXmlFileName, lFrameNumber, 3);
					FrameData2ndItt.l2FootRSpeed = CalculateSpeedInterpolated (lXmlFileName, lFrameNumber, 4);
					FrameData2ndItt.l2ElbowLSpeed = CalculateSpeedInterpolated (lXmlFileName, lFrameNumber, 5);
					FrameData2ndItt.l2ElbowRSpeed = CalculateSpeedInterpolated (lXmlFileName, lFrameNumber, 6);

					 MSXML::IXMLDOMElementPtr element01;		 
					 MSXML::IXMLDOMAttributePtr attribute01_1;  
					 MSXML::IXMLDOMAttributePtr attribute01_2; 
					 MSXML::IXMLDOMAttributePtr attribute01_3; 
					 element01 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("CharRootVelocityVec_F0"), _T(""));
					 attribute01_1 = xmlDoc->createAttribute (_T("x"));
					 attribute01_2 = xmlDoc->createAttribute (_T("y"));
					 attribute01_3 = xmlDoc->createAttribute (_T("z"));
					 attribute01_1->value = _T(FrameData2ndItt.l2CharRootVelocityVec_F0[0]);
					 attribute01_2->value = _T(FrameData2ndItt.l2CharRootVelocityVec_F0[1]);
					 attribute01_3->value = _T(FrameData2ndItt.l2CharRootVelocityVec_F0[2]);
					 element01->setAttributeNode (attribute01_1);
					 element01->setAttributeNode (attribute01_2);
					 element01->setAttributeNode (attribute01_3);

					 MSXML::IXMLDOMElementPtr element02;		 
					 MSXML::IXMLDOMAttributePtr attribute02_1;  
					 MSXML::IXMLDOMAttributePtr attribute02_2; 
					 MSXML::IXMLDOMAttributePtr attribute02_3; 
					 element02 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("CharRootVelocityVec_F15"), _T(""));
					 attribute02_1 = xmlDoc->createAttribute (_T("x"));
					 attribute02_2 = xmlDoc->createAttribute (_T("y"));
					 attribute02_3 = xmlDoc->createAttribute (_T("z"));
					 attribute02_1->value = _T(FrameData2ndItt.l2CharRootVelocityVec_F15[0]);
					 attribute02_2->value = _T(FrameData2ndItt.l2CharRootVelocityVec_F15[1]);
					 attribute02_3->value = _T(FrameData2ndItt.l2CharRootVelocityVec_F15[2]);
					 element02->setAttributeNode (attribute02_1);
					 element02->setAttributeNode (attribute02_2);
					 element02->setAttributeNode (attribute02_3);

					 MSXML::IXMLDOMElementPtr element03;		 
					 MSXML::IXMLDOMAttributePtr attribute03_1;  
					 MSXML::IXMLDOMAttributePtr attribute03_2; 
					 MSXML::IXMLDOMAttributePtr attribute03_3; 
					 element03 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("CharRootVelocityVec_F60"), _T(""));
					 attribute03_1 = xmlDoc->createAttribute (_T("x"));
					 attribute03_2 = xmlDoc->createAttribute (_T("y"));
					 attribute03_3 = xmlDoc->createAttribute (_T("z"));
					 attribute03_1->value = _T(FrameData2ndItt.l2CharRootVelocityVec_F60[0]);
					 attribute03_2->value = _T(FrameData2ndItt.l2CharRootVelocityVec_F60[1]);
					 attribute03_3->value = _T(FrameData2ndItt.l2CharRootVelocityVec_F60[2]);
					 element03->setAttributeNode (attribute03_1);
					 element03->setAttributeNode (attribute03_2);
					 element03->setAttributeNode (attribute03_3);

					 MSXML::IXMLDOMElementPtr element04;		 
					 MSXML::IXMLDOMAttributePtr attribute04;  
					 element04 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("CharRootRotationDelta"), _T(""));
					 attribute04 = xmlDoc->createAttribute (_T("delta"));
					 attribute04->value = _T(FrameData2ndItt.l2RootRotDelta);
					 element04->setAttributeNode (attribute04);

					 MSXML::IXMLDOMElementPtr element05;		 
					 MSXML::IXMLDOMAttributePtr attribute05;  
					 element05 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("HipsRotationDelta"), _T(""));
					 attribute05 = xmlDoc->createAttribute (_T("delta"));
					 attribute05->value = _T(FrameData2ndItt.l2HipRotDelta);
					 element05->setAttributeNode (attribute05);

					 MSXML::IXMLDOMElementPtr element06;		 
					 MSXML::IXMLDOMAttributePtr attribute06;  
					 element06 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("CharRootSpeed"), _T(""));
					 attribute06 = xmlDoc->createAttribute (_T("speed"));
					 attribute06->value = _T(FrameData2ndItt.l2CharRootSpeed);
					 element06->setAttributeNode (attribute06);

					 MSXML::IXMLDOMElementPtr element07;		 
					 MSXML::IXMLDOMAttributePtr attribute07;  
					 element07 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("HipsSpeed"), _T(""));
					 attribute07 = xmlDoc->createAttribute (_T("speed"));
					 attribute07->value = _T(FrameData2ndItt.l2HipsSpeed);
					 element07->setAttributeNode (attribute07);

					 MSXML::IXMLDOMElementPtr element08;		 
					 MSXML::IXMLDOMAttributePtr attribute08;  
					 element08 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("FootLSpeed"), _T(""));
					 attribute08 = xmlDoc->createAttribute (_T("speed"));
					 attribute08->value = _T(FrameData2ndItt.l2FootLSpeed);
					 element08->setAttributeNode (attribute08);

					 MSXML::IXMLDOMElementPtr element09;		 
					 MSXML::IXMLDOMAttributePtr attribute09;  
					 element09 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("FootRSpeed"), _T(""));
					 attribute09 = xmlDoc->createAttribute (_T("speed"));
					 attribute09->value = _T(FrameData2ndItt.l2FootRSpeed);
					 element09->setAttributeNode (attribute09);

					 MSXML::IXMLDOMElementPtr element10;		 
					 MSXML::IXMLDOMAttributePtr attribute10;  
					 element10 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("ElbowLSpeed"), _T(""));
					 attribute10 = xmlDoc->createAttribute (_T("speed"));
					 attribute10->value = _T(FrameData2ndItt.l2ElbowLSpeed);
					 element10->setAttributeNode (attribute10);

					 MSXML::IXMLDOMElementPtr element11;		 
					 MSXML::IXMLDOMAttributePtr attribute11;  
					 element11 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("ElbowRSpeed"), _T(""));
					 attribute11 = xmlDoc->createAttribute (_T("speed"));
					 attribute11->value = _T(FrameData2ndItt.l2ElbowRSpeed);
					 element11->setAttributeNode (attribute11);

					 //---------compose frame node-----------------
					 lFrameIterator->appendChild (element01);
					 lFrameIterator->appendChild (element02);
					 lFrameIterator->appendChild (element03);
					 lFrameIterator->appendChild (element04);
					 lFrameIterator->appendChild (element05);
					 lFrameIterator->appendChild (element06);
					 lFrameIterator->appendChild (element07);
					 lFrameIterator->appendChild (element08);
					 lFrameIterator->appendChild (element09);
					 lFrameIterator->appendChild (element10);
					 lFrameIterator->appendChild (element11);
					// xmlDoc->documentElement->replaceChild(lFrameIterator,lOldFrame);
					 xmlDoc->documentElement->appendChild(lFrameIterator);					 
					 hr = xmlDoc->save(_T(lXmlFileName));

					 RecordingStatusOutput = 1;
					 return RecordingStatusOutput;
			}
		}
	}
	return RecordingStatusOutput;
}
//----------------------------------------------------------------------------------------
bool ORBoxVector::RecordData_Pass2 (std::wstring lFileName, bool RecordingToggle)
{
	if (!Itteration2Done)
	{
		BSTR bstrFileName = SysAllocStringLen(lFileName.data(), lFileName.size());
		std::wstring wstrFrame = L"frame ";
		if (xmlDoc->load(_T(bstrFileName)) != VARIANT_TRUE)
		{
			printf("Unable to load input.xml\n");
			RecordingStatusOutput = 0;
			return RecordingStatusOutput;
		}
		else
		{
			MSXML::IXMLDOMNodeListPtr lFrameNodeList;
			lFrameNodeList = xmlDoc->selectNodes("//frame");
			long NodeCount = lFrameNodeList->Getlength ();
		 	for (int i=0; i<=NodeCount; i++)
			{
				std::wstring wstrFrameNumbered = wstrFrame + std::to_wstring(i);
				BSTR bstrFrame = SysAllocStringLen(wstrFrameNumbered.data(), wstrFrameNumbered.size());
				Append2ndIttDataFrame (bstrFileName, bstrFrame);
			}	 
			lFrameNodeList = xmlDoc->selectNodes("//frame");
			for (int i=0; i<i<=(NodeCount*0.5); i++)  
			{
				MSXML::IXMLDOMNodePtr lFrameIterator;
				lFrameIterator = lFrameNodeList->Getitem (i);
				xmlDoc->documentElement->removeChild(lFrameIterator);					 				
			}
			hr = xmlDoc->save(_T(bstrFileName));
			
		};
		Itteration2Done =1;
		return 1;
	}
	else
	{
		return 0;
	};

	//	std::wstring lFileName = L"D:/AnimDatabaseCache.xml";
	//	std::wstring wstrFrame = L"frame 3";
	//	BSTR bstrFileName = SysAllocStringLen(lFileName.data(), lFileName.size());
	//	BSTR bstrFrame = SysAllocStringLen(wstrFrame.data(), wstrFrame.size());
	//	if (CheckFrameDataExistInXml (bstrFileName, bstrFrame))
	//	{
	//
	//		FramePoseData FrameData2ndItt = CalculateRootVelocityAt3Points (bstrFileName, bstrFrame);
	//		Append2ndIttDataFrame (bstrFileName, bstrFrame);
	//	}	
	//
	//
	//B
	//B
}
//----------------------------------------------------------------------------------------
bool ORBoxVector::DebugPoseDataOutput (FBEvaluateInfo* pEvaluateInfo)
{
	if ((TimesliderUpdate!= lTimeFrame)&&(lOutputFinilizedDebugInfo))
	{
		TimesliderUpdate = lTimeFrame;

		std::wstring lFileName = L"D:/AnimDatabaseCache.xml";
		std::wstring wstrFrame = L"frame " + std::to_wstring((int)lTimeFrame);
		BSTR bstrFileName = SysAllocStringLen(lFileName.data(), lFileName.size());
		BSTR bstrFrame = SysAllocStringLen(wstrFrame.data(), wstrFrame.size());
		FramePoseData CurrentFrame = ReadFrameData (bstrFileName, bstrFrame);

		//mY->WriteData( &lVector[1], pEvaluateInfo);
		mCharRootPosDebug			   ->WriteData(  CurrentFrame.lCharRootPos,		pEvaluateInfo);
		mCharRootRotYDebug			   ->WriteData( &CurrentFrame.lCharRootRotY,	pEvaluateInfo);
		mHipsPosDebug				   ->WriteData(  CurrentFrame.lHipsPos,			pEvaluateInfo);
		mHipsRotYDebug				   ->WriteData( &CurrentFrame.lHipsRotY,		pEvaluateInfo);
		mFootLPosDebug				   ->WriteData(  CurrentFrame.lFootLPos,		pEvaluateInfo);
		mFootRPosDebug				   ->WriteData(  CurrentFrame.lFootRPos,		pEvaluateInfo);
		mElbowLPosDebug				   ->WriteData(  CurrentFrame.lElbowLPos,		pEvaluateInfo);
		mElbowRPosDebug				   ->WriteData(  CurrentFrame.lElbowRPos,		pEvaluateInfo);
		mRootRotDeltaDebug			   ->WriteData( &CurrentFrame.l2RootRotDelta,	pEvaluateInfo);
		mHipRotDeltaDebug			   ->WriteData( &CurrentFrame.l2HipRotDelta,	pEvaluateInfo);
		mCharRootSpeedDebug			   ->WriteData( &CurrentFrame.l2CharRootSpeed,	pEvaluateInfo);
		mHipsSpeedDebug				   ->WriteData( &CurrentFrame.l2HipsSpeed,		pEvaluateInfo);
		mFootLSpeedDebug			   ->WriteData( &CurrentFrame.l2FootLSpeed,		pEvaluateInfo);
		mFootRSpeedDebug			   ->WriteData( &CurrentFrame.l2FootRSpeed,		pEvaluateInfo);
		mElbowLSpeedDebug			   ->WriteData( &CurrentFrame.l2ElbowLSpeed,	pEvaluateInfo);
		mElbowRSpeedDebug			   ->WriteData( &CurrentFrame.l2ElbowRSpeed,	pEvaluateInfo);
		mCharRootVelocityVec_F0Debug   ->WriteData(  CurrentFrame.l2CharRootVelocityVec_F0,		pEvaluateInfo);
		mCharRootVelocityVec_F15Debug  ->WriteData(  CurrentFrame.l2CharRootVelocityVec_F15,	pEvaluateInfo);
		mCharRootVelocityVec_F60Debug  ->WriteData(  CurrentFrame.l2CharRootVelocityVec_F60,	pEvaluateInfo);

	};

	return true;
}
//----------------------------------------------------------------------------------------









FramePoseData::FramePoseData ()
{
	lTimeFrame = lCharRootRotY = lHipsRotY = 0 ;
	for (int i=0; i<3; i++)
	{
		lCharRootPos[i]	= 0;
		lHipsPos[i] = 0;
		lFootLPos[i] = 0;
		lFootRPos[i] = 0;
		lElbowLPos[i] = 0;
		lElbowRPos[i] = 0;

		l2CharRootVelocityVec_F0[i]  =0;
		l2CharRootVelocityVec_F15[i] =0;
		l2CharRootVelocityVec_F60[i] =0;	
	};
	l2RootRotDelta 		 =0;
	l2HipRotDelta		 =0;
	l2CharRootSpeed 	 =0;
	l2HipsSpeed 		 =0;
	l2FootLSpeed 		 =0;
	l2FootRSpeed 		 =0;
	l2ElbowLSpeed 		 =0;
	l2ElbowRSpeed		 =0;
}

FramePoseData::FramePoseData (double TimeFrame, double CharRootPos[3], double CharRootRotY, double HipsPos[3], double HipsRotY, double FootLPos[3], double FootRPos[3], double ElbowLPos[3], double ElbowRPos[3])
{
	lTimeFrame = TimeFrame;
	lCharRootRotY = CharRootRotY;
	lHipsRotY = HipsRotY;
	for (int i=0; i<3; i++)
	{
		lCharRootPos[i]	= CharRootPos[i] ;
		lHipsPos[i]		= HipsPos[i]	 ;
		lFootLPos[i]	= FootLPos[i]	 ;
		lFootRPos[i]	= FootRPos[i]	 ;
		lElbowLPos[i]	= ElbowLPos[i]	 ;
		lElbowRPos[i]	= ElbowRPos[i]	 ;
	};
	l2RootRotDelta 				 =0;
	l2HipRotDelta				 =0;
	l2CharRootSpeed 			 =0;
	l2HipsSpeed 				 =0;
	l2FootLSpeed 				 =0;
	l2FootRSpeed 				 =0;
	l2ElbowLSpeed 				 =0;
	l2ElbowRSpeed				 =0;
	l2CharRootVelocityVec_F0[3]  =0;
	l2CharRootVelocityVec_F15[3] =0;
	l2CharRootVelocityVec_F60[3] =0;
	
}