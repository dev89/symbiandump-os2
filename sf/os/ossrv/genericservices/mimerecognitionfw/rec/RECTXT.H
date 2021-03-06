// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#if !defined(__RECTXT_H__)
#define __RECTXT_H__

#if !defined(__APMREC_H__)
#include <apmrec.h>
#endif

class CApaTextRecognizer : public CApaDataRecognizerType
	{
public: // from CApaDataRecognizerType
	CApaTextRecognizer();
	TUint PreferredBufSize();
	TDataType SupportedDataTypeL(TInt aIndex) const;
	//for ECOM
	static CApaDataRecognizerType* CreateRecognizerL();
private: // from CApaDataRecognizerType
	void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	};

#endif
