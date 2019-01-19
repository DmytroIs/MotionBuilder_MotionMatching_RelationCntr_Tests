
#include "mm_posematch_framelist_gen_box.h"
#include <math.h>

//--- Registration defines
#define ORBOXVECTOR__CLASS		ORBOXVECTOR__CLASSNAME
#define ORBOXVECTOR__NAME		ORBOXVECTOR__CLASSSTR
#define	ORBOXVECTOR__LOCATION	"MotionMatching_Test1"
#define ORBOXVECTOR__LABEL		"mm_posematch_framelist_gen"
#define	ORBOXVECTOR__DESC		"mm_posematch_framelist_gen"

//--- implementation and registration
FBBoxImplementation	(	ORBOXVECTOR__CLASS		);		// Box class name
FBRegisterBox		(	ORBOXVECTOR__NAME,				// Unique name to register box.
						ORBOXVECTOR__CLASS,				// Box class name
						ORBOXVECTOR__LOCATION,			// Box location ('plugins')
						ORBOXVECTOR__LABEL,				// Box label (name of box to display)
						ORBOXVECTOR__DESC,			// Box long description.
						FB_DEFAULT_SDK_ICON		);	// Icon filename (default=Open Reality icon)


bool ORBoxVector::FBCreate()
{
	// Input Node 
	mTriggerLoadPoseDatabase	= AnimationNodeInCreate  (0,"[0] Load Pose Database" , ANIMATIONNODE_TYPE_BOOL);
	mTriggerLoadInputData		= AnimationNodeInCreate  (0,"[1] Load Input Data" , ANIMATIONNODE_TYPE_BOOL);
	mTriggerCalculate			= AnimationNodeInCreate  (0,"[2] Calculate sequence to D: ResultFrames.xml" , ANIMATIONNODE_TYPE_BOOL);
	mTriggerPastePoses			= AnimationNodeInCreate  (0,"[3] Generate character poses" , ANIMATIONNODE_TYPE_BOOL);


	// Output Nodes
	mOutputCalculateSucceded	 = AnimationNodeOutCreate ( 0, "Calculation Succeded",	ANIMATIONNODE_TYPE_NUMBER );
	mOutputPastePosesSucceded	 = AnimationNodeOutCreate ( 0, "Poses generating Succeded",	ANIMATIONNODE_TYPE_NUMBER );
  //  mY = AnimationNodeOutCreate ( 0, "Y", ANIMATIONNODE_TYPE_NUMBER );
  //  mZ = AnimationNodeOutCreate ( 0, "Z", ANIMATIONNODE_TYPE_NUMBER );
	isLoadedToMemory =0; isInputDBLoadedToMemory =0;
	for (int i =0; i<20000; i++) ResultFrameSequence[i]=0;
	InitMatchingSettings ();

	hr = CoInitialize(NULL);
    if (SUCCEEDED(hr))
    {
        try
        {
            hr = xmlDoc.CreateInstance(__uuidof(MSXML::DOMDocument60), NULL, CLSCTX_INPROC_SERVER);			
			//correct initialization flag
		}
        catch (_com_error &e)
        {
           printf("ERROR: %ws\n", e.ErrorMessage());
        }
	}
	else
	{
		//init error flag
	}
	return true;
}

void ORBoxVector::FBDestroy()
{
}


bool ORBoxVector::AnimationNodeNotify( FBAnimationNode* pAnimationNode, FBEvaluateInfo* pEvaluateInfo )
{
	bool	lStatus[4];	// Transferring vector
	bool lStatusTotal=1;// Status of input node
	lStatus[0]  = mTriggerLoadPoseDatabase  ->ReadData( &lTriggerLoadPoseDatabase, pEvaluateInfo );	
	lStatus[1]  = mTriggerLoadInputData	    ->ReadData( &lTriggerLoadInputData	 , pEvaluateInfo );
	lStatus[2]  = mTriggerCalculate		    ->ReadData( &lTriggerCalculate		 , pEvaluateInfo );
	lStatus[3]  = mTriggerPastePoses		->ReadData( &lTriggerPastePoses		 , pEvaluateInfo );
	for (int i=0; i<4; i++) {lStatusTotal*=lStatus[i]; } // total=true when all []=1  

    if( lStatusTotal )
	{

		if (lTriggerLoadPoseDatabase && !lTriggerLoadInputData && !lTriggerCalculate && !lTriggerPastePoses)
		{
			if (!lTriggerLoadPoseDatabase && isLoadedToMemory) {isLoadedToMemory=0;};
			ReadFromFileToMemory_PoseDatabase (L"D:/AnimDatabaseCache.xml");
		}	

		if (!lTriggerLoadPoseDatabase && lTriggerLoadInputData && !lTriggerCalculate && !lTriggerPastePoses)
		{
			if (!lTriggerLoadInputData && isInputDBLoadedToMemory) {isInputDBLoadedToMemory=0;};
			ReadFromFileToMemory_InputDatabase (L"D:/GamepadInputCache.xml");
		}	

		if (!lTriggerLoadPoseDatabase && !lTriggerLoadInputData && lTriggerCalculate && !lTriggerPastePoses)
		{
			if (isLoadedToMemory && isInputDBLoadedToMemory)
			{
				CalculatePoseMatchSequence ();
			};
		}	

		if (!lTriggerLoadPoseDatabase && !lTriggerLoadInputData && !lTriggerCalculate && lTriggerPastePoses)
		{
			// Using   docs.autodesk.com/MOBPRO/2016/ENU/MotionBuilder-Developer-Help/index.html#!/url=./cpp_ref/class_o_r_s_d_k2015_1_1_f_b_character_pose.html#a7707cc6b5604c65def8dce7a3368c9da
		}	
		

	// Write the data to the out node
	//	mIsRecorded->WriteData( &Buff, pEvaluateInfo);
	//	mY->WriteData( &lVector[1], pEvaluateInfo);
	//	mZ->WriteData( &lVector[2], pEvaluateInfo);
	    return true;
	}

    return false;
}


bool ORBoxVector::ReadFromFileToMemory_PoseDatabase (BSTR  lXmlFileName)
{
	if  (lTriggerLoadPoseDatabase && !isLoadedToMemory)
	{
		if (xmlDoc->load(_T(lXmlFileName)) != VARIANT_TRUE)
		{
			return false;
		}
		else
		{	
			xmlDoc->setProperty("SelectionLanguage", "XPath");
			MSXML::IXMLDOMNodeListPtr lFrameNodeList;
			lFrameNodeList = xmlDoc->selectNodes("//frame");
			MSXML::IXMLDOMNodePtr lFrameIterator;
			long NodeCount = lFrameNodeList->Getlength ();
			for (int i=0; i<=20000 && i<NodeCount; i++)
			{														
				lFrameIterator = lFrameNodeList->Getitem (i);
				BSTR FrameNodeTextCurrent = lFrameIterator->Gettext();
				std::wstring FrameNodeTextCheck = L"frame " + std::to_wstring(i);//=std::wstring(lFrameNumber, SysStringLen(lFrameNumber));
				std::wstring wsFrameNodeTextCurrent = std::wstring(FrameNodeTextCurrent, SysStringLen(FrameNodeTextCurrent));
				if (wsFrameNodeTextCurrent ==FrameNodeTextCheck) 
				{	
					PoseDatabase[i].lTimeFrame=i;
					MSXML::IXMLDOMElementPtr element_buff;
					MSXML::IXMLDOMNodeListPtr lChildNodeList = lFrameIterator->GetchildNodes();
					long ChildNodesCount = lChildNodeList->Getlength();
					_variant_t DataReadBuff;
					for (long j=1,k=0;j<ChildNodesCount;j++)
					{
						element_buff = lChildNodeList->Getitem (j);
						BSTR ChildNodeName = element_buff->GetnodeName();

						std::wstring wsChildNodeName = std::wstring(ChildNodeName, SysStringLen(ChildNodeName));
						if (wsChildNodeName == L"CharRootPos") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].lCharRootPos[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].lCharRootPos[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].lCharRootPos[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"CharRootRot") 
						{
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].lCharRootRotY=double(DataReadBuff);
						};
						if (wsChildNodeName == L"lHipsPos") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].lHipsPos[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].lHipsPos[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].lHipsPos[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"lHipsRot") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].lHipsRot[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].lHipsRot[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].lHipsRot[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"lFootLPos") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].lFootLPos[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].lFootLPos[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].lFootLPos[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"lFootRPos")
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].lFootRPos[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].lFootRPos[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].lFootRPos[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"lElbowLPos") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].lElbowLPos[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].lElbowLPos[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].lElbowLPos[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"lElbowRPos") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].lElbowRPos[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].lElbowRPos[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].lElbowRPos[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2RootRotDelta")
						{
							DataReadBuff = element_buff->getAttribute ("delta");
							PoseDatabase[i].l2RootRotDelta=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2HipRotDelta") 
						{
							DataReadBuff = element_buff->getAttribute ("delta");
							PoseDatabase[i].l2HipRotDelta=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2CharRootSpeed") 
						{
							DataReadBuff = element_buff->getAttribute ("speed");
							PoseDatabase[i].l2CharRootSpeed=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2HipsSpeed")
						{
							DataReadBuff = element_buff->getAttribute ("speed");
							PoseDatabase[i].l2HipsSpeed=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2FootLSpeed")
						{
							DataReadBuff = element_buff->getAttribute ("speed");
							PoseDatabase[i].l2FootLSpeed=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2FootRSpeed") 
						{
							DataReadBuff = element_buff->getAttribute ("speed");
							PoseDatabase[i].l2FootRSpeed=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2ElbowLSpeed")
						{
							DataReadBuff = element_buff->getAttribute ("speed");
							PoseDatabase[i].l2ElbowLSpeed=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2ElbowRSpeed") 
						{
							DataReadBuff = element_buff->getAttribute ("speed");
							PoseDatabase[i].l2ElbowRSpeed=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2CharRootVelocityVec_F0") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].l2CharRootVelocityVec_F0[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].l2CharRootVelocityVec_F0[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].l2CharRootVelocityVec_F0[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2CharRootVelocityVec_F15") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].l2CharRootVelocityVec_F15[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].l2CharRootVelocityVec_F15[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].l2CharRootVelocityVec_F15[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2CharRootVelocityVec_F60") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].l2CharRootVelocityVec_F60[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].l2CharRootVelocityVec_F60[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].l2CharRootVelocityVec_F60[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2HipsPos_F15") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].l2HipsPos_F15[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].l2HipsPos_F15[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].l2HipsPos_F15[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2FootLPos_F15")
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].l2FootLPos_F15[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].l2FootLPos_F15[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].l2FootLPos_F15[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2FootRPos_F15") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].l2FootRPos_F15[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].l2FootRPos_F15[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].l2FootRPos_F15[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2ElbowLPos_F15") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].l2ElbowLPos_F15[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].l2ElbowLPos_F15[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].l2ElbowLPos_F15[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"l2ElbowRPos_F15") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							PoseDatabase[i].l2ElbowRPos_F15[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							PoseDatabase[i].l2ElbowRPos_F15[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							PoseDatabase[i].l2ElbowRPos_F15[2]=double(DataReadBuff);
						};
					};
				};
			};
			isLoadedToMemory =1;
			return true;
		};
	};
	return false;
}


bool ORBoxVector::ReadFromFileToMemory_InputDatabase (BSTR  lXmlFileName)
{
	if  (lTriggerLoadInputData && !isInputDBLoadedToMemory)
	{
		if (xmlDoc->load(_T(lXmlFileName)) != VARIANT_TRUE)
		{
			return false;
		}
		else
		{	
			xmlDoc->setProperty("SelectionLanguage", "XPath");
			MSXML::IXMLDOMNodeListPtr lFrameNodeList;
			lFrameNodeList = xmlDoc->selectNodes("//frame");
			MSXML::IXMLDOMNodePtr lFrameIterator;
			long NodeCount = lFrameNodeList->Getlength ();
			for (int i=0; i<=20000 && i<NodeCount; i++)
			{														
				lFrameIterator = lFrameNodeList->Getitem (i);
				BSTR FrameNodeTextCurrent = lFrameIterator->Gettext();
				std::wstring FrameNodeTextCheck = L"frame " + std::to_wstring(i);
				std::wstring wsFrameNodeTextCurrent = std::wstring(FrameNodeTextCurrent, SysStringLen(FrameNodeTextCurrent));
				if (wsFrameNodeTextCurrent ==FrameNodeTextCheck) 
				{	
					InputDatabase[i].lTimeFrame=i;
					MSXML::IXMLDOMElementPtr element_buff;
					MSXML::IXMLDOMNodeListPtr lChildNodeList = lFrameIterator->GetchildNodes();
					long ChildNodesCount = lChildNodeList->Getlength();
					_variant_t DataReadBuff;
					for (long j=1,k=0;j<ChildNodesCount;j++)
					{
						element_buff = lChildNodeList->Getitem (j);
						BSTR ChildNodeName = element_buff->GetnodeName();
						std::wstring wsChildNodeName = std::wstring(ChildNodeName, SysStringLen(ChildNodeName));

						if (wsChildNodeName == L"CharRootPos") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							InputDatabase[i].lCharRootPos[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							InputDatabase[i].lCharRootPos[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							InputDatabase[i].lCharRootPos[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"CharRootRot") 
						{
							DataReadBuff = element_buff->getAttribute ("y");
							InputDatabase[i].lCharRootRotY=double(DataReadBuff);
						};
						if (wsChildNodeName == L"CharRootVelocityVec_F0") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							InputDatabase[i].lCharRootVelocityVec_F0[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							InputDatabase[i].lCharRootVelocityVec_F0[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							InputDatabase[i].lCharRootVelocityVec_F0[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"CharRootVelocityVec_F15") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							InputDatabase[i].lCharRootVelocityVec_F15[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							InputDatabase[i].lCharRootVelocityVec_F15[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							InputDatabase[i].lCharRootVelocityVec_F15[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"CharRootVelocityVec_F60") 
						{
							DataReadBuff = element_buff->getAttribute ("x");
							InputDatabase[i].lCharRootVelocityVec_F60[0]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("y");
							InputDatabase[i].lCharRootVelocityVec_F60[1]=double(DataReadBuff);
							DataReadBuff = element_buff->getAttribute ("z");
							InputDatabase[i].lCharRootVelocityVec_F60[2]=double(DataReadBuff);
						};
						if (wsChildNodeName == L"CharRootRotationDelta")
						{
							DataReadBuff = element_buff->getAttribute ("delta");
							InputDatabase[i].lCharRootRotationDelta=double(DataReadBuff);
						};
						if (wsChildNodeName == L"CharRootSpeed")
						{
							DataReadBuff = element_buff->getAttribute ("speed");
							InputDatabase[i].lCharRootSpeed=double(DataReadBuff);
						};	
					};
				};
			};
			isInputDBLoadedToMemory =1;
			return true;
		};
	};
	return false;
}


bool ORBoxVector::CalculatePoseMatchSequence ()
{
	for (int iFrame = 1; iFrame<20000; iFrame++)
	{
		if (InputDatabase[iFrame].lTimeFrame)  //check input frame data existence
		{
			ResultFrameSequence[iFrame]= ReturnPoseMatchFrame (InputDatabase[iFrame]);
			if (MMSettings.FrameRate >1) 
			{
				for (int i=1; i<MMSettings.FrameRate; i++) 
				{
					iFrame++;
					ResultFrameSequence[iFrame]=ResultFrameSequence[iFrame-1]+1;					
				};
			};
		};
	}
	return true;
}


long ORBoxVector::ReturnPoseMatchFrame (GamepadInputFrameData InputData)
{
	long PoseScoreMatrix [20000]; for (int i=0;i<20000;i++) {PoseScoreMatrix[i]=0;};
	for (int i=1; i<20000; i++)
	{
		if ((PoseDatabase[i].l2CharRootSpeed >= (InputData.lCharRootSpeed - MMSettings.Deviation_RootSpeed)) && (PoseDatabase[i].l2CharRootSpeed <= (InputData.lCharRootSpeed + MMSettings.Deviation_RootSpeed)) )
		{
			PoseScoreMatrix[i]+=(MMSettings.MatchingWeight_RootSpeed * (1 - ((std::abs(InputData.lCharRootSpeed - PoseDatabase[i].l2CharRootSpeed)) / MMSettings.Deviation_RootSpeed)));
			if ((PoseDatabase[i].l2RootRotDelta >= (InputData.lCharRootRotationDelta - MMSettings.Deviation_RootRotDelta)) && (PoseDatabase[i].l2RootRotDelta <= (InputData.lCharRootRotationDelta + MMSettings.Deviation_RootRotDelta)) )
			{
				PoseScoreMatrix[i]+= (MMSettings.MatchingWeight_RootRotDelta * (1 - ((std::abs(InputData.lCharRootRotationDelta - PoseDatabase[i].l2RootRotDelta)) / MMSettings.Deviation_RootRotDelta)));
				/* TODO: pose scores compare withing this cycle regarding active curr pose */
			}
		}
		
	};


	return 1;
}


void ORBoxVector::InitMatchingSettings ()
{
	MMSettings.FrameRate					=3;

	MMSettings.Deviation_RootSpeed			=1;
	MMSettings.Deviation_RootRotDelta		=3;
	MMSettings.Deviation_Hips				=2;

	MMSettings.MatchingWeight_RootSpeed		=50;
	MMSettings.MatchingWeight_RootRotDelta	=40;
	MMSettings.MatchingWeight_Hips			=4;


	return;
}




//-----------------------------------------------------------------------------------------------------
FramePoseData::FramePoseData ()
{
	lTimeFrame = lCharRootRotY = 0 ;
	for (int i=0; i<3; i++)
	{
		lCharRootPos[i]	= 0;
		lHipsPos[i] = 0;
		lFootLPos[i] = 0;
		lFootRPos[i] = 0;
		lElbowLPos[i] = 0;
		lElbowRPos[i] = 0;
		lHipsRot[i]= 0;
		l2HipsPos_F15[i]			 = 0;
		l2FootLPos_F15[i]			 = 0;
		l2FootRPos_F15[i]			 = 0;
		l2ElbowLPos_F15[i]			 = 0;
		l2ElbowRPos_F15[i]			 = 0;
		l2CharRootVelocityVec_F0[i]	 = 0;
		l2CharRootVelocityVec_F15[i] = 0;
		l2CharRootVelocityVec_F60[i] = 0;	
			
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

FramePoseData::FramePoseData (double TimeFrame, double CharRootPos[3], double CharRootRotY, double HipsPos[3], double HipsRot[3], double FootLPos[3], double FootRPos[3], double ElbowLPos[3], double ElbowRPos[3] , double HipsPos_F15[3],	double FootLPos_F15[3],	double FootRPos_F15[3],	double ElbowLPos_F15[3], double ElbowRPos_F15[3], double CharRootVelocityVec_F0[3],	double CharRootVelocityVec_F15[3],	double CharRootVelocityVec_F60[3], double RootRotDelta, double HipRotDelta, double CharRootSpeed, double HipsSpeed, double FootLSpeed, double FootRSpeed, double ElbowLSpeed, double ElbowRSpeed)
{
	lTimeFrame = TimeFrame;
	lCharRootRotY = CharRootRotY;	
	for (int i=0; i<3; i++)
	{
		lCharRootPos[i]	= CharRootPos[i] ;
		lHipsPos[i]		= HipsPos[i]	 ;
		lHipsRot[i] = HipsRot[i];
		lFootLPos[i]	= FootLPos[i]	 ;
		lFootRPos[i]	= FootRPos[i]	 ;
		lElbowLPos[i]	= ElbowLPos[i]	 ;
		lElbowRPos[i]	= ElbowRPos[i]	 ;
		l2HipsPos_F15[i]			 = HipsPos_F15[i]			 ;
		l2FootLPos_F15[i]			 = FootLPos_F15[i]			 ;
		l2FootRPos_F15[i]			 = FootRPos_F15[i]			 ;
		l2ElbowLPos_F15[i]			 = ElbowLPos_F15[i]			 ;
		l2ElbowRPos_F15[i]			 = ElbowRPos_F15[i]			 ;
		l2CharRootVelocityVec_F0[i]	 = CharRootVelocityVec_F0[i] ;	
		l2CharRootVelocityVec_F15[i] = CharRootVelocityVec_F15[i];
		l2CharRootVelocityVec_F60[i] = CharRootVelocityVec_F60[i];
	};
	l2RootRotDelta 		=   RootRotDelta 	;
	l2HipRotDelta		=   HipRotDelta		;
	l2CharRootSpeed 	=   CharRootSpeed	;
	l2HipsSpeed 		=   HipsSpeed 		;
	l2FootLSpeed 		=   FootLSpeed 		;
	l2FootRSpeed 		=   FootRSpeed 		;
	l2ElbowLSpeed 		=   ElbowLSpeed 	;
	l2ElbowRSpeed		=   ElbowRSpeed		;

	
}
//-------------------------------------------------------------------------------------------------------



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
//-------------------------------------------------------------------------------------------------------


MotionMatchingSettings::MotionMatchingSettings ()
{
		MMSettings.FrameRate					=0; //3 frames
		MMSettings.Deviation_RootSpeed			=0;
		MMSettings.Deviation_RootRotDelta		=0;
		MMSettings.Deviation_Hips				=0;
		MMSettings.MatchingWeight_RootSpeed		=0;
		MMSettings.MatchingWeight_RootRotDelta	=0;
		MMSettings.MatchingWeight_Hips			=0;



}