
#include "mm_input_datalist_gen_box.h"
#include <math.h>

//--- Registration defines
#define ORBOXVECTOR__CLASS		ORBOXVECTOR__CLASSNAME
#define ORBOXVECTOR__NAME		ORBOXVECTOR__CLASSSTR
#define	ORBOXVECTOR__LOCATION	"MotionMatching_Test1"
#define ORBOXVECTOR__LABEL		"mm_input_datalist_gen"
#define	ORBOXVECTOR__DESC		"mm_input_datalist_gen"

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
    mRecordToggle	= AnimationNodeInCreate  ( 0, "[0-1] process to file; 'D:\GamepadInputCache.xml'", ANIMATIONNODE_TYPE_NUMBER );
	mTimeFrame		= AnimationNodeInCreate  ( 0, "[0-2] frame number", ANIMATIONNODE_TYPE_NUMBER );
	mCharRootPos	= AnimationNodeInCreate  ( 0, "[1-0] character root pos; to record root velocity vector", ANIMATIONNODE_TYPE_VECTOR );
	mCharRootRotY	= AnimationNodeInCreate  ( 0, "[1-1] character root rot(Y); to record root rotation delta in 0.5sec", ANIMATIONNODE_TYPE_NUMBER );
	mDebugToggle	= AnimationNodeInCreate  ( 0, "[0-4] output debug info from .xml file", ANIMATIONNODE_TYPE_NUMBER );
	mPostProcessToggle = AnimationNodeInCreate  ( 0, "[0-3] postproc recorded .xml data", ANIMATIONNODE_TYPE_NUMBER );

	TimesliderUpdate =1;
	Itteration2Done =0;

	// Output Nodes
    mIsRecorded			= AnimationNodeOutCreate ( 0, "[0] Recording to 'D:\GamepadInputCache.xml' ", ANIMATIONNODE_TYPE_NUMBER );
	mDebugCharRootPos  	= AnimationNodeOutCreate ( 0, "[1] mDebugCharRootPos",				ANIMATIONNODE_TYPE_VECTOR );
	mDebugCharRootRotY  = AnimationNodeOutCreate ( 0, "[2] mDebugCharRootRotY",				ANIMATIONNODE_TYPE_NUMBER ); 
	mCharRootRotationDelta 	   = AnimationNodeOutCreate ( 0, "[3]  Rotation Delta " ,	   ANIMATIONNODE_TYPE_NUMBER );
	mCharRootVelocityVec_F0    = AnimationNodeOutCreate ( 0, "[5]  VelocityVec F0 " ,	   ANIMATIONNODE_TYPE_VECTOR );
	mCharRootVelocityVec_F15   = AnimationNodeOutCreate ( 0, "[6]  VelocityVec F15" ,	   ANIMATIONNODE_TYPE_VECTOR );
	mCharRootVelocityVec_F60   = AnimationNodeOutCreate ( 0, "[7]  VelocityVec F60" ,	   ANIMATIONNODE_TYPE_VECTOR );
	mCharRootSpeed			   = AnimationNodeOutCreate ( 0, "[4]  Speed 		  " ,	   ANIMATIONNODE_TYPE_NUMBER );

//-------------xml initialization---------------
	hr = CoInitialize(NULL);
    if (SUCCEEDED(hr))
    {
        try
        {
            hr = xmlDoc.CreateInstance(__uuidof(MSXML::DOMDocument60), NULL, CLSCTX_INPROC_SERVER);			
			lRecordToggle = 1;
		}
        catch (_com_error &e)
        {
           printf("ERROR: %ws\n", e.ErrorMessage());
        }
	}
	else
	{
		lRecordToggle = 0;
	}
//-----------------------------------------------

	return true;
}


/************************************************
 *	Destruction.
 ************************************************/
void ORBoxVector::FBDestroy()
{
}
//-----------------------------------------------------------------------------------------------------------------
bool ORBoxVector::AnimationNodeNotify( FBAnimationNode* pAnimationNode, FBEvaluateInfo* pEvaluateInfo )
{
	bool	lStatus;	// Status of input node
	lStatus= mRecordToggle	->ReadData(&lRecordToggle, pEvaluateInfo );	
	lStatus= mDebugToggle	->ReadData(&lDebugToggle, pEvaluateInfo );	
	lStatus= mTimeFrame		->ReadData(&lTimeFrame, pEvaluateInfo );
	lStatus= mCharRootPos	->ReadData(lCharRootPos, pEvaluateInfo );
	lStatus= mCharRootRotY	->ReadData(&lCharRootRotY, pEvaluateInfo );
	lStatus= mPostProcessToggle ->ReadData(&lPostProcessToggle, pEvaluateInfo );


	if (lRecordToggle && (!lDebugToggle))
	{
		RecordFrameToFile (L"D:/GamepadInputCache.xml");
	}
	if (lPostProcessToggle && (!lRecordToggle) && (!lDebugToggle))
	{
		RecordData_Pass2 (L"D:/GamepadInputCache.xml",1);
	}
	if (lDebugToggle && (!lRecordToggle) && (!lPostProcessToggle))
	{
		DebugPoseDataOutput (pEvaluateInfo);
	}

    if( lStatus )
	{
		mIsRecorded			->WriteData( &lRecordToggle, pEvaluateInfo);
	//	mDebugCharRootPos	->WriteData( lCurrentFrameInputData.lCharRootPos, pEvaluateInfo);
	//	mDebugCharRootRotY	->WriteData( &lCurrentFrameInputData.lCharRootRotY, pEvaluateInfo);
	//	mY->WriteData( &lVector[1], pEvaluateInfo);
	    return true;
	}
    return false;
}
//-----------------------------------------------------------------------------------------------------------------
void ORBoxVector::RecordFrameToFile (std::wstring lFileName)
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
			RecordFrameDataStructure (bstrFileName, bstrFrameNumbered);
		};

	}

}
//-----------------------------------------------------------------------------------------------------------------
bool ORBoxVector::CheckFrameDataExistInXml ( BSTR  lXmlFileName, BSTR  lFrameNumber)
{
	bool Result=0;
    if (xmlDoc->load(_T(lXmlFileName)) != VARIANT_TRUE)
    {
		return Result;
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
 	} 
	 	return Result;
}
//-----------------------------------------------------------------------------------------------------------------
bool ORBoxVector::RecordFrameDataStructure (BSTR  lXmlFileName, BSTR  lFrameNumber)
{
		bool Result =0;
       if (xmlDoc->load(_T(lXmlFileName)) != VARIANT_TRUE)
       {
		return Result;
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

		   MSXML::IXMLDOMElementPtr element02;		 
		   MSXML::IXMLDOMAttributePtr attribute02_1;  
		   element02 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("CharRootRot"), _T(""));
		   attribute02_1 = xmlDoc->createAttribute (_T("y"));
		   attribute02_1->value = _T(lCharRootRotY);
		   element02->setAttributeNode (attribute02_1);

		   FrameNode->appendChild (element01);
		   FrameNode->appendChild (element02);
           xmlDoc->documentElement->appendChild(FrameNode);
		   hr = xmlDoc->save(_T(lXmlFileName));
		   Result=1;
	   }
	   return Result;
}
//-----------------------------------------------------------------------------------------------------------------
GamepadInputFrameData ORBoxVector::ReadFrameData (BSTR  lXmlFileName, BSTR  lFrameNumber)
{
	GamepadInputFrameData lGamepadInputFrameData;
	double ArrayReadBuff [256]; 	for (int i=0;i<256;i++) ArrayReadBuff[i]=0;  //i was young, i had no time
	if (xmlDoc->load(_T(lXmlFileName)) != VARIANT_TRUE)
    {
		printf("Unable to load input.xml\n");
		lRecordToggle = 0;
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
	lGamepadInputFrameData.lTimeFrame = stod(FrameNumberDigit);
	int tmpCounter = 0;
	lGamepadInputFrameData.lCharRootPos	[0]	= ArrayReadBuff[tmpCounter++];
	lGamepadInputFrameData.lCharRootPos	[1]	= ArrayReadBuff[tmpCounter++];
	lGamepadInputFrameData.lCharRootPos	[2]	= ArrayReadBuff[tmpCounter++];
	lGamepadInputFrameData.lCharRootRotY	= ArrayReadBuff[tmpCounter++];

	lGamepadInputFrameData.lCharRootVelocityVec_F0[0]   = ArrayReadBuff[tmpCounter++];
	lGamepadInputFrameData.lCharRootVelocityVec_F0[1]   = ArrayReadBuff[tmpCounter++];
	lGamepadInputFrameData.lCharRootVelocityVec_F0[2]   = ArrayReadBuff[tmpCounter++];
	lGamepadInputFrameData.lCharRootVelocityVec_F15[0]  = ArrayReadBuff[tmpCounter++];
	lGamepadInputFrameData.lCharRootVelocityVec_F15[1]  = ArrayReadBuff[tmpCounter++];
	lGamepadInputFrameData.lCharRootVelocityVec_F15[2]  = ArrayReadBuff[tmpCounter++];
	lGamepadInputFrameData.lCharRootVelocityVec_F60[0]  = ArrayReadBuff[tmpCounter++];
	lGamepadInputFrameData.lCharRootVelocityVec_F60[1]  = ArrayReadBuff[tmpCounter++];
	lGamepadInputFrameData.lCharRootVelocityVec_F60[2]  = ArrayReadBuff[tmpCounter++];

	lGamepadInputFrameData.lCharRootRotationDelta = ArrayReadBuff[tmpCounter++];
	lGamepadInputFrameData.lCharRootSpeed = ArrayReadBuff[tmpCounter++];


	return lGamepadInputFrameData;
}
//-----------------------------------------------------------------------------------------------------------------
double ORBoxVector::CalculateSpeedInterpolated (BSTR  lXmlFileName, BSTR  lFrameNumber)
{
	double Interpolated1FrameDistance =0 ;
	GamepadInputFrameData F0, F1, F2, Fn1;
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


		double distance [3];
		double f0x,f0y,f0z,f1x,f1y,f1z,f2x,f2y,f2z,fn1x,fn1y,fn1z;
		f0x=F0.lCharRootPos[0];f0y=F0.lCharRootPos[1];f0z=F0.lCharRootPos[2];f1x=F1.lCharRootPos[0];f1y=F1.lCharRootPos[1];f1z=F1.lCharRootPos[2];
		f2x=F2.lCharRootPos[0];f2y=F2.lCharRootPos[1];f2z=F2.lCharRootPos[2];fn1x=Fn1.lCharRootPos[0];fn1y=Fn1.lCharRootPos[1];fn1z=Fn1.lCharRootPos[2];
		distance [0] = sqrt (pow((f0x-f1x),2.0)+pow((f0y-f1y),2.0)+pow((f0z-f1z),2.0));
		distance [1] = sqrt (pow((f1x-f2x),2.0)+pow((f1y-f2y),2.0)+pow((f1z-f2z),2.0));
		distance [2] = sqrt (pow((fn1x-f0x),2.0)+pow((fn1y-f0y),2.0)+pow((fn1z-f0z),2.0));
		Interpolated1FrameDistance = (distance[0]+distance[1]+distance[2])/3.0;

	return Interpolated1FrameDistance;
}
//-----------------------------------------------------------------------------------------------------------------
double ORBoxVector::CalculateAngleChangeInterpolated (BSTR  lXmlFileName, BSTR  lFrameNumber)
{
	double Interpolated1FrameAngleDelta =0 ;
	GamepadInputFrameData F0, F1, F2, Fn1;
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

		double angleDelta [3];
		double f0_angle = F0.lCharRootRotY, f1_angle = F1.lCharRootRotY, f2_angle = F2.lCharRootRotY, fn1_angle = Fn1.lCharRootRotY;
		//TODO: add Gimbal unbinding for clear values
		angleDelta [0] = f1_angle-f0_angle;
	 	angleDelta [1] = f2_angle-f1_angle;
	 	angleDelta [2] = f0_angle-fn1_angle;
		Interpolated1FrameAngleDelta = (angleDelta[0]+angleDelta[1]+angleDelta[2])/3.0;

	return Interpolated1FrameAngleDelta;
}
//-----------------------------------------------------------------------------------------------------------------
GamepadInputFrameData ORBoxVector::CalculateRootVelocityAt3Points (BSTR  lXmlFileName, BSTR  lFrameNumber)
{
	GamepadInputFrameData CurrentFrame = ReadFrameData (lXmlFileName, lFrameNumber);
	GamepadInputFrameData F0, F1, F15, F60;
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

	for (int i=0; i<3;i++) {CurrentFrame.lCharRootVelocityVec_F0[i] = F1.lCharRootPos[i] - F0.lCharRootPos[i];};
	for (int i=0; i<3;i++) {CurrentFrame.lCharRootVelocityVec_F15[i] = F15.lCharRootPos[i] - F0.lCharRootPos[i];};
	for (int i=0; i<3;i++) {CurrentFrame.lCharRootVelocityVec_F60[i] = F60.lCharRootPos[i] - F0.lCharRootPos[i];};

	return CurrentFrame;
}
//--------------------------------------------------------------------------------------------------------------------
bool ORBoxVector::Append2ndIttDataFrame (BSTR  lXmlFileName, BSTR  lFrameNumber)
{
	   if (xmlDoc->load(_T(lXmlFileName)) != VARIANT_TRUE)
       {
			printf("Unable to load input.xml\n");
			lRecordToggle = 0;
			return lRecordToggle;
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
					GamepadInputFrameData FrameData2ndItt = CalculateRootVelocityAt3Points (lXmlFileName, lFrameNumber);
					FrameData2ndItt.lCharRootRotationDelta = CalculateAngleChangeInterpolated (lXmlFileName, lFrameNumber);					
					FrameData2ndItt.lCharRootSpeed = CalculateSpeedInterpolated (lXmlFileName, lFrameNumber);
					 MSXML::IXMLDOMElementPtr element01;		 
					 MSXML::IXMLDOMAttributePtr attribute01_1;  
					 MSXML::IXMLDOMAttributePtr attribute01_2; 
					 MSXML::IXMLDOMAttributePtr attribute01_3; 
					 element01 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("CharRootVelocityVec_F0"), _T(""));
					 attribute01_1 = xmlDoc->createAttribute (_T("x"));
					 attribute01_2 = xmlDoc->createAttribute (_T("y"));
					 attribute01_3 = xmlDoc->createAttribute (_T("z"));
					 attribute01_1->value = _T(FrameData2ndItt.lCharRootVelocityVec_F0[0]);
					 attribute01_2->value = _T(FrameData2ndItt.lCharRootVelocityVec_F0[1]);
					 attribute01_3->value = _T(FrameData2ndItt.lCharRootVelocityVec_F0[2]);
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
					 attribute02_1->value = _T(FrameData2ndItt.lCharRootVelocityVec_F15[0]);
					 attribute02_2->value = _T(FrameData2ndItt.lCharRootVelocityVec_F15[1]);
					 attribute02_3->value = _T(FrameData2ndItt.lCharRootVelocityVec_F15[2]);
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
					 attribute03_1->value = _T(FrameData2ndItt.lCharRootVelocityVec_F60[0]);
					 attribute03_2->value = _T(FrameData2ndItt.lCharRootVelocityVec_F60[1]);
					 attribute03_3->value = _T(FrameData2ndItt.lCharRootVelocityVec_F60[2]);
					 element03->setAttributeNode (attribute03_1);
					 element03->setAttributeNode (attribute03_2);
					 element03->setAttributeNode (attribute03_3);

					 MSXML::IXMLDOMElementPtr element04;		 
					 MSXML::IXMLDOMAttributePtr attribute04;  
					 element04 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("CharRootRotationDelta"), _T(""));
					 attribute04 = xmlDoc->createAttribute (_T("delta"));
					 attribute04->value = _T(FrameData2ndItt.lCharRootRotationDelta);
					 element04->setAttributeNode (attribute04);

					 MSXML::IXMLDOMElementPtr element06;		 
					 MSXML::IXMLDOMAttributePtr attribute06;  
					 element06 = xmlDoc->createNode (MSXML::NODE_ELEMENT, _T("CharRootSpeed"), _T(""));
					 attribute06 = xmlDoc->createAttribute (_T("speed"));
					 attribute06->value = _T(FrameData2ndItt.lCharRootSpeed);
					 element06->setAttributeNode (attribute06);



					 //---------compose frame node-----------------
					 lFrameIterator->appendChild (element01);
					 lFrameIterator->appendChild (element02);
					 lFrameIterator->appendChild (element03);
					 lFrameIterator->appendChild (element04);
					 lFrameIterator->appendChild (element06);
					// xmlDoc->documentElement->replaceChild(lFrameIterator,lOldFrame);
					 xmlDoc->documentElement->appendChild(lFrameIterator);					 
					 hr = xmlDoc->save(_T(lXmlFileName));

					 lRecordToggle = 1;
					 return lRecordToggle;
			}
		}
	}
	return lRecordToggle;
}
//--------------------------------------------------------------------------------------------------------------------
bool ORBoxVector::RecordData_Pass2 (std::wstring lFileName, bool RecordingToggle)
{
	if (!Itteration2Done)
	{
		BSTR bstrFileName = SysAllocStringLen(lFileName.data(), lFileName.size());
		std::wstring wstrFrame = L"frame ";
		if (xmlDoc->load(_T(bstrFileName)) != VARIANT_TRUE)
		{
			printf("Unable to load input.xml\n");
			lRecordToggle = 0;
			return lRecordToggle;
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
			for (int i=0; i<=(NodeCount-1); i++)  
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
}
//--------------------------------------------------------------------------------------------------------------------
bool ORBoxVector::DebugPoseDataOutput (FBEvaluateInfo* pEvaluateInfo)
{
	if ((TimesliderUpdate!= lTimeFrame)&&(lDebugToggle))
	{
		TimesliderUpdate = lTimeFrame;

		std::wstring lFileName = L"D:/GamepadInputCache.xml";
		std::wstring wstrFrame = L"frame " + std::to_wstring((int)lTimeFrame);
		BSTR bstrFileName = SysAllocStringLen(lFileName.data(), lFileName.size());
		BSTR bstrFrame = SysAllocStringLen(wstrFrame.data(), wstrFrame.size());
		GamepadInputFrameData CurrentFrame = ReadFrameData (bstrFileName, bstrFrame);

		//mY->WriteData( &lVector[1], pEvaluateInfo);
		mDebugCharRootPos	->WriteData( CurrentFrame.lCharRootPos, pEvaluateInfo); 
		mDebugCharRootRotY	->WriteData(&CurrentFrame.lCharRootRotY, pEvaluateInfo);
		mCharRootRotationDelta 	  ->WriteData(&CurrentFrame.lCharRootRotationDelta, pEvaluateInfo);
		mCharRootVelocityVec_F0   ->WriteData( CurrentFrame.lCharRootVelocityVec_F0, pEvaluateInfo);
		mCharRootVelocityVec_F15  ->WriteData( CurrentFrame.lCharRootVelocityVec_F15, pEvaluateInfo);
		mCharRootVelocityVec_F60  ->WriteData( CurrentFrame.lCharRootVelocityVec_F60, pEvaluateInfo);
		mCharRootSpeed			  ->WriteData(&CurrentFrame.lCharRootSpeed, pEvaluateInfo);
		//mCharRootPosDebug			   ->WriteData(  CurrentFrame.lCharRootPos,		pEvaluateInfo);
		//mCharRootRotYDebug			   ->WriteData( &CurrentFrame.lCharRootRotY,	pEvaluateInfo);
		//mHipsPosDebug				   ->WriteData(  CurrentFrame.lHipsPos,			pEvaluateInfo);
		//mHipsRotYDebug				   ->WriteData( &CurrentFrame.lHipsRotY,		pEvaluateInfo);
		//mFootLPosDebug				   ->WriteData(  CurrentFrame.lFootLPos,		pEvaluateInfo);
		//mFootRPosDebug				   ->WriteData(  CurrentFrame.lFootRPos,		pEvaluateInfo);
		//mElbowLPosDebug				   ->WriteData(  CurrentFrame.lElbowLPos,		pEvaluateInfo);
		//mElbowRPosDebug				   ->WriteData(  CurrentFrame.lElbowRPos,		pEvaluateInfo);
		//mRootRotDeltaDebug			   ->WriteData( &CurrentFrame.l2RootRotDelta,	pEvaluateInfo);
		//mHipRotDeltaDebug			   ->WriteData( &CurrentFrame.l2HipRotDelta,	pEvaluateInfo);
		//mCharRootSpeedDebug			   ->WriteData( &CurrentFrame.l2CharRootSpeed,	pEvaluateInfo);
		//mHipsSpeedDebug				   ->WriteData( &CurrentFrame.l2HipsSpeed,		pEvaluateInfo);
		//mFootLSpeedDebug			   ->WriteData( &CurrentFrame.l2FootLSpeed,		pEvaluateInfo);
		//mFootRSpeedDebug			   ->WriteData( &CurrentFrame.l2FootRSpeed,		pEvaluateInfo);
		//mElbowLSpeedDebug			   ->WriteData( &CurrentFrame.l2ElbowLSpeed,	pEvaluateInfo);
		//mElbowRSpeedDebug			   ->WriteData( &CurrentFrame.l2ElbowRSpeed,	pEvaluateInfo);
		//mCharRootVelocityVec_F0Debug   ->WriteData(  CurrentFrame.l2CharRootVelocityVec_F0,		pEvaluateInfo);
		//mCharRootVelocityVec_F15Debug  ->WriteData(  CurrentFrame.l2CharRootVelocityVec_F15,	pEvaluateInfo);
		//mCharRootVelocityVec_F60Debug  ->WriteData(  CurrentFrame.l2CharRootVelocityVec_F60,	pEvaluateInfo);
	    
	};

	return true;
}
//--------------------------------------------------------------------------------------------------------------------




















GamepadInputFrameData::GamepadInputFrameData()
{
lTimeFrame		= 0;
lCharRootRotY	= 0;
for (int i=0; i<3; i++) 
	{
		lCharRootPos[i]=0;
	};
for (int i=0; i<3; i++) 
	{
		lCharRootVelocityVec_F0[i]=0;
	};
for (int i=0; i<3; i++) 
	{
		lCharRootVelocityVec_F15[i]=0;
	};
for (int i=0; i<3; i++) 
	{
		lCharRootVelocityVec_F60[i]=0;
	};
lCharRootRotationDelta=0; 
lCharRootSpeed=0;
}

GamepadInputFrameData::GamepadInputFrameData(double TimeFrame, double CharRootPos[3], double CharRootRotY, double CharRootRotationDelta, double CharRootVelocityVec_F0[3], double CharRootVelocityVec_F15[3], double CharRootVelocityVec_F60[3], double CharRootSpeed)
{
lTimeFrame		= TimeFrame;
lCharRootRotY	= CharRootRotY;
for (int i=0; i<3; i++) 
	{
		lCharRootPos[i]=CharRootPos[i];
	};			
for (int i=0; i<3; i++) 
	{
		lCharRootVelocityVec_F0[i]=CharRootVelocityVec_F0[i];
	};
for (int i=0; i<3; i++) 
	{
		lCharRootVelocityVec_F15[i]=CharRootVelocityVec_F15[i];
	};
for (int i=0; i<3; i++) 
	{
		lCharRootVelocityVec_F60[i]=CharRootVelocityVec_F60[i];
	};
lCharRootRotationDelta  	= CharRootRotationDelta; 
lCharRootSpeed				= CharRootSpeed;
}