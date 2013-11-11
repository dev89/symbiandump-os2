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


#include <ups/policyevaluator.h>
#include <ups/upserr.h>
#include <ecom/ecom.h>
#include "upslog.h"

using namespace UserPromptService;

/**
Default implementation of a policy evaluator. 
This is returned if no policy evaluator is defined in the policy file OR
the policy evaluator implementation UID is zero.
*/
NONSHARABLE_CLASS(CDefaultPolicyEvaluator) : public CPolicyEvaluator 
	{
public:
	static CDefaultPolicyEvaluator* NewL();
	
	void GenerateFingerprints(
		const CPromptRequest& aRequest, const CPolicy& aPolicy,
		RPointerArray<CFingerprint>& aFingerprints, const CClientEntity*& aClientEntity, 
		const TAny*& aDialogCreatorParams,
		TRequestStatus& aStatus);
	
	// Empty implementations of CActive pure virtual functions
	// RunL is never called because the request is completed immediately.
	void RunL() {};
	void DoCancel() {};
	
	~CDefaultPolicyEvaluator();
	
private:
	CDefaultPolicyEvaluator();
	};

EXPORT_C CPolicyEvaluator::CPolicyEvaluator()
/**
Constructor
*/
: CActive(EPriorityStandard)
	{
	}

EXPORT_C CPolicyEvaluator::~CPolicyEvaluator()
/**
Destructor
*/
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}
	
EXPORT_C CPolicyEvaluator* CPolicyEvaluator::NewL(const TUid& aPolicyEvaluatorImplementationId)
/**
Creates a new policy evaluator.
@param	aPolicyEvaluatorImplementationId	The UID of the policy evaluator implemenation
											as specified in the policy file.

@return A pointer to the new dialog creator implementation.
*/
	{
	if (aPolicyEvaluatorImplementationId.iUid == 0)
		{
		DEBUG_PRINTF(_L8("Instantiating default policy evaluator"));
		return CDefaultPolicyEvaluator::NewL();
		}
	
	TAny* plugin(0);
	DEBUG_PRINTF2(_L8("Instantiating policy evaluator 0x%08x"), aPolicyEvaluatorImplementationId);
	TRAPD(err, plugin = 
	REComSession::CreateImplementationL(aPolicyEvaluatorImplementationId,
		_FOFF(CPolicyEvaluator,iDtor_ID_Key)));

	if (err != KErrNone)
		{
		DEBUG_PRINTF3(_L8("Failed to instantiate policy evaluator 0x%08x, err = %d"),
					  aPolicyEvaluatorImplementationId.iUid, err);
		}

	if (err == KErrNotFound)
		{
		err = KErrUpsMissingPolicyEvaluator;  
		}
	User::LeaveIfError(err);
	return reinterpret_cast<CPolicyEvaluator*>(plugin);		
	}

/**
If a decision record matching one of the fingerprints created by \ref CPolicyEvaluator::GenerateFingerprints
was found then this method will be invoked. This allows the policy evaluator 
to instruct the user prompt service to ignore the result of the decision record and prompt 
the user.\p
This functionality could be used to trigger periodic prompts if an application
makes very high usage of a service e.g. after 100 SMS messages have been sent. 
The usage could be tracked externally or via the evaluatorInfo field in the decision record.\n
N.B. The User Prompt Service does not mandate the contents or usage of the evaluatorInfo field.

- The default implementation CPolicyEvaluator::ForcePromptL always returns EFalse and does
  not modifiy aNewEvaluatorInfo.

@param	aDecision			The first decision record that matched a fingerprint generated by
							\ref CPolicyEvaluator::GenerateFingerprints
@param	aNewEvaluatorInfo	A reference to a copy of the evaluatorInfo field from the decision record.
							If this method modifies this value then this field will be updated in the
							decision record regardless of whether a prompt is required.
@return						ETrue if a prompt should be displayed; otherwise, 
							EFalse if the result of the decision record should be used.
*/
EXPORT_C TBool CPolicyEvaluator::ForcePromptL(const CDecisionRecord& aDecision, TUint& aNewEvaluatorInfo)
	{
	(void) aDecision;
	(void) aNewEvaluatorInfo;
	return EFalse;
	}

EXPORT_C TInt CPolicyEvaluator::GetExtension(TUint aExtensionId, TAny*& a0, TAny* a1)
/**
Allows extension of this interface. Calls Extension_

@param aExtensionId		The UID of the interface to instantiate.
@param a0				A reference to a pointer that should be set to the newly
						instantiated object.
@param a1				Data specific to the instantiate of the specified interface.

@return KErrNone if the extension is supported or KErrNotSupported if the extension is not
		recognised; otherwise, a system wide error may be returned.
*/
	{
	return Extension_(aExtensionId, a0, a1);
	}

EXPORT_C TInt CPolicyEvaluator::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	return CActive::Extension_(aExtensionId, a0, a1);
	}

CDefaultPolicyEvaluator* CDefaultPolicyEvaluator::NewL() 
/**
Factory method that creates a new default policy evaluator object.
@return A pointer to the new default policy evaluator object.
*/
	{
	CDefaultPolicyEvaluator* self = new(ELeave) CDefaultPolicyEvaluator();
	return self;
	}

CDefaultPolicyEvaluator::CDefaultPolicyEvaluator()
/**
Constructor
*/
	: CPolicyEvaluator()
	{
	CActiveScheduler::Add(this);
	}

CDefaultPolicyEvaluator::~CDefaultPolicyEvaluator()
/**
Destructor
*/
	{
	Deque();
	}
	
void CDefaultPolicyEvaluator::GenerateFingerprints(
	const CPromptRequest& /* aRequest */, const CPolicy& /* aPolicy */,
	RPointerArray<CFingerprint>& aFingerprints, const CClientEntity*& /* aClientEntity */, 
	const TAny*& /* aDialogCreatorParams */,
	TRequestStatus& aStatus)
/**
Returns a single, null fingerprint and completes immediately.
*/
	{
	CFingerprint* f(0);
	TRAPD(retVal, f = CFingerprint::NewLC(KNullDesC8, KNullDesC);
			   aFingerprints.AppendL(f); 
			   CleanupStack::Pop(f));
	
	TRequestStatus *status = &aStatus;
	*status = KRequestPending;	
	User::RequestComplete(status, retVal);
	}