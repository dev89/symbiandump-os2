/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @internalComponent	 
 @test
*/

#ifndef TUPSDIALOGCREATOR_H
#define TUPSDIALOGCREATOR_H

#include <e32base.h>
#include <e32cmn.h>
#include <ups/dialogcreator.h>
#include <ups/fingerprint.h>
#include <ups/upsnotifierutil.h>

using namespace UserPromptService;

/**
 Test/Reference implementation of a User Prompt Service dialog creator ECOM plug-in.
 */
class CTestDialogCreator : public CDialogCreator
	{
public:
	typedef enum
		{
		EIdle,
		EPrepareDialog,
		EDisplayDialog,
		EProcessResult		
		} TState;
	static CDialogCreator* CreateDialogCreatorL();				
	~CTestDialogCreator();
	
protected:
	// From CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
	// From CDialogCreator
	void PrepareDialog(
		const CPromptRequest& aRequest, const CPolicy& aPolicy,			
		const RPointerArray<CFingerprint>& aFingerprints, const CClientEntity* aClientEntity, 
		const TAny* aDialogCreatorParams, TRequestStatus& aStatus);
	void DisplayDialog(CPolicy::TOptions& aOptionSelcted, const CFingerprint*& aFingerprint,
			TUint& aEvaluatorInfo, TRequestStatus& aStatus);	
		
private:	
	CTestDialogCreator();
	void ConstructL();
	void ResolveClientNameL(const TSecureId& aSid);
	void ResolveVendorName(const TVendorId& aVid);
	void DoPrepareDialogL();
	void DoDisplayDialogL();
	void DoProcessResultL();	
	
	// Const data from UPS & policy evaluator
	const CPromptRequest* iRequest;						///< Request data from system server
	const CPolicy* iPolicy;								///< The policy that matches iRequest
	const RPointerArray<CFingerprint>* iFingerprints;	///< Fingerprints generated by policy evaluator from iRequest
	const TAny* iEvalPrivateData;						///< Extra data generated by policy evaluator
	
	TRequestStatus* iClientStatus;				///< UPS's request object 
	
	// Return data for DisplayDialog
	CPolicy::TOptions* iOptionSelected;			///< The option selected by the user
	const CFingerprint** iFingerprint;			///< The fingerprint for the new decision if Always or Never selected
	TUint* iEvaluatorInfo;						///< Plug-in specific field in decision record

	CPromptData* iPromptData;					///< Data used by notifier to render dialog
	RBuf8 iPromptDataDes;						///< Externalized prompt data
	TPromptResult iPromptResult;				///< Response buffer for notifier
	TPckg<TPromptResult> iPromptResultPckg;		///< Packages iPromptResult
	RNotifier iNotifier;						///< Connection to notifier framework 
	TState iState;								///< Current state 
	};
#endif // TUPSDIALOGCREATOR_H

