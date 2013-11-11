// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __TE_HEADERSTESTSERVER_H__)
#define __TE_HEADERSTESTSERVER_H__
#include <TestExecuteServerBase.h>
		
class CXmlDomHeadersTestServer : public CTestServer
	{
public:
	static CXmlDomHeadersTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};
#endif