
/**	\file	orboxvector.cxx
*	Library declarations.
*	Contains the basic routines to declare the DLL as a loadable
*	library.
*/

//--- SDK include
#include <fbsdk/fbsdk.h>



#ifdef KARCH_ENV_WIN
	#include <windows.h>
#endif

//--- Library declaration
FBLibraryDeclare( mm_anim_posedatalist_gen )
{
	FBLibraryRegister( ORBoxVector );
}
FBLibraryDeclareEnd;

/************************************************
 *	Library functions.
 ************************************************/
bool FBLibrary::LibInit()		{ return true; }
bool FBLibrary::LibOpen()		{ return true; }
bool FBLibrary::LibReady()		{ return true; }
bool FBLibrary::LibClose()		{ return true; }
bool FBLibrary::LibRelease()	{ return true; }

/**
*	\mainpage	Simple Box
*	\section	intro	Introduction
*	This simple example will create a vector to number
*	converter. That is to say it will accept a vector on the input side,
*	and via the real-time engine, it will write the individual
*	values of the vector onto three output connectors.
*	\section	walk	Walkthrough
*	\subsection	func1	FBCreate()
*	<ol>
*		<li>Create the input node (type: VECTOR);
*		<li>Create the output nodes (type: 3 x NUMBER);
*		<li>Return true (successfully created).
*	</ol>
*	\subsection	func2	AnimationNodeNotify()
*	<ol>
*		<li>Read the data from the input nodes;
*		<li>If the input node is alive (live or recorded), write the data out to three individual output nodes;
*		<li>Return whether or not the node is alive (true/false).
*	</ol>
*/
