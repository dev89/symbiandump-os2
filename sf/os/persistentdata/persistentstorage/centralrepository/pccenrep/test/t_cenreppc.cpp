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

#include <x86tool/centralrepository.h>
#include <e32test.h>
#include <f32file.h>

#ifdef __TOOLS2__
#define CENREP_PC_TEST
#endif

enum TestMode{
	ETxt,
	ECre,
	EAutoCre,
	EAutoTxt
};

RTest TheTest(_L("Central Repository Tests"));
RFs TheFs;

//uid for txt testing purpose
const TUid uid={0x000001FF};
//uid for cre testing purpose
const TUid uidcre={0x00001FFF};
const TUid uidcretest = {0x000002FF};
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
const TUid uidcrev1= {0x00004FFF};
#endif

const TUid uidmetatest = {0x00022222};

#ifndef __TOOLS2__
/** FILES FOR MAN NEWL TESTING */
	_LIT(KTemplateTxtFile,"c:\\000001ff.txt");
	_LIT(KTemplateTxtOutFile,"c:\\000001ff.cre");
	//cre equivalent is generated through centrepconv of the txt version
	_LIT(KTemplateCreFile,"c:\\00001fff.cre");
	_LIT(KCopyTemplateCreFile,"c:\\copy00001fff.cre");	
	//the target output is set to a different name
	_LIT(KTemplateCreOutFile,"c:\\00001eee.cre");
	
	_LIT(KMetaTxtTestInFile,"c:\\00022222.txt");  // Txt test input
	_LIT(KMetaTestCreFile,"c:\\00022222.cre"); // Txt test output, Cre test in/output
	_LIT(KMetaTestCreCopy,"c:\\copy00022222.cre");
	_LIT(KMetaTxtTestOgnFile, "c:\\00022222.ogn");

/** FILES FOR AUTO NEWL TESTING */
	_LIT(KPrivateTemplateCreFile,"c:\\private\\00000000\\00001fff.cre");
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	_LIT(KPrivateTemplateCreV1File,"c:\\private\\00000000\\00004fff.cre");
	_LIT(KCopyPrivateTemplateCreV1File,"c:\\private\\00000000\\copy00004fff.cre");	
#endif	
	_LIT(KCopyPrivateTemplateCreFile,"c:\\private\\00000000\\copy00001fff.cre");
	_LIT(KPrivateTemplateTxtFile,"c:\\private\\0000000\\000001ff.txt");
	_LIT(KPrivateTemplateTxtOutFile,"c:\\private\\00000000\\000001ff.cre");

	//for comparison test
	_LIT(KPrivateCreFileForCreTest,"c:\\private\\00000000\\000002ff.cre");
	_LIT(KCopyCreFileForCreTest,"c:\\private\\00000000\\copy000002ff.cre");
	_LIT(KTxtTestRefFile,"c:\\private\\00000000\\ref_winscwtxt.cre");
	_LIT(KCreTestRefFile,"c:\\private\\00000000\\ref_winscwcre.cre");
	_LIT(KTxtTestGenFile,"c:\\private\\00000000\\winscwtxt.cre");
	_LIT(KCreTestGenFile,"c:\\private\\00000000\\winscwcre.cre");

	// Make sure the contents of the following 2 files is the same as crc.txt and
	// ref_00022222.cre, which are generated by the WINSCW t_cenrepcs.exe test.
	_LIT(KCrcRecordFile,"c:\\common_crc.txt");
	_LIT(KMetaTestRefFile,"c:\\private\\00000000\\common_ref_00022222.cre");

	_LIT(KOgnFile1FF, "c:\\private\\00000000\\000001ff.ogn");
	_LIT(KOgnFile1FFF, "c:\\private\\00000000\\00001fff.ogn");
	_LIT(KOgnFile2FF, "c:\\private\\00000000\\000002ff.ogn");
	_LIT(KOgnFile22222, "c:\\private\\00000000\\00022222.ogn");
	
	_LIT(KMetaTestFile,"c:\\private\\00000000\\00022222.cre");
	_LIT(KMetaTestCopy,"c:\\private\\00000000\\copy00022222.cre");

// __TOOLS2__	
#else
/** FILES FOR MAN NEWL TESTING */	
	//in TOOLS2 this is relative
	_LIT(KTemplateTxtFile,"000001ff.txt");
	_LIT(KTemplateTxtOutFile,"000001ff.cre");	
 	//cre equivalent is generated through centrepconv of the txt version	
	_LIT(KTemplateCreFile,"00001fff.cre");
	_LIT(KCopyTemplateCreFile,"copy00001fff.cre");
	_LIT(KTemplateCreOutFile,"00001eee.cre");
	
	_LIT(KMetaTxtTestInFile,"00022222.txt");  // Txt test input
	_LIT(KMetaTestCreFile,"00022222.cre"); // Txt test output, Cre test in/output
	_LIT(KMetaTestCreCopy,"copy00022222.cre");
	_LIT(KMetaTxtTestOgnFile, "00022222.ogn");
	
/** FILES FOR AUTO NEWL TESTING */	
	_LIT(KPrivateTemplateCreFile,"00001fff.cre");
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	_LIT(KPrivateTemplateCreV1File,"00004fff.cre");
	_LIT(KCopyPrivateTemplateCreV1File,"copy00004fff.cre");	
#endif	
	_LIT(KCopyPrivateTemplateCreFile,"copy00001fff.cre");	
	_LIT(KPrivateTemplateTxtFile,"000001ff.txt");
	_LIT(KPrivateTemplateTxtOutFile,"000001ff.cre");	
		
	//for comparison test
	_LIT(KPrivateCreFileForCreTest,"000002ff.cre");
	_LIT(KCopyCreFileForCreTest,"copy000002ff.cre");
	_LIT(KTxtTestRefFile,"ref_winscwtxt.cre");
	_LIT(KCreTestRefFile,"ref_winscwcre.cre");
	_LIT(KTxtTestGenFile,"winscwtxt.cre");
	_LIT(KCreTestGenFile,"winscwcre.cre");
	// if start with \epoc32, symfile resolves the address related to EPOCROOT

    // Make sure the contents of the following 2 files is the same as crc.txt and
	// ref_00022222.cre, which are generated by the WINSCW t_cenrepcs.exe test.
	_LIT(KCrcRecordFile,"\\epoc32\\winscw\\c\\common_crc.txt"); 
	_LIT(KMetaTestRefFile,"\\epoc32\\winscw\\c\\private\\00000000\\common_ref_00022222.cre");

	_LIT(KOgnFile1FF, "000001ff.ogn");
	_LIT(KOgnFile1FFF, "00001fff.ogn");
	_LIT(KOgnFile2FF, "000002ff.ogn");
	_LIT(KOgnFile22222, "00022222.ogn");
	

	_LIT(KMetaTestFile,"00022222.cre");
	_LIT(KMetaTestCopy,"copy00022222.cre");
#endif	

///////////////////////////////////////////////////////////////////////////////////////

LOCAL_C void Check(TInt aValue, TInt aExpected, TInt aLine)
    {
    if(aValue != aExpected)
        {
        RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
        TheTest(EFalse, aLine);
        }
    }
	
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

///////////////////////////////////////////////////////////////////////////////////////

//DEFINED IN THE COMMON CPP FILE
extern TInt CopyFile(const TDesC& aSource, const TDesC& aTarget);
extern void ObjectCreateDeleteOOM(TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode);
extern void OomTest(void (*testFuncL)(CRepository* aRepository),TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode);
extern void BasicFunctionL(TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode);
extern void OomBasicFunction(TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode);
extern void DoFileCompL(const TDesC& aGenerated, const TDesC& aReference, TUint32& aCrcValue);
extern void DoCrcCompL(const TUint32& aCrcValue, const TDesC& aCrcRecord, TBool aCrcOfTxt);
extern void DEF130394L(TUid aUid);

void InitialiseLC(CRepository*& aRepository,TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode)
	{
#ifdef CENREP_PC_TEST	
	if (aTestMode==ETxt || aTestMode==ECre)
		aRepository=CRepository::NewLC(aInFilePath,aOutFilePath);
	else
#endif	
		aRepository=CRepository::NewLC(aUid);
	}

void ClearOgns()
	{
	TheFs.Delete(KMetaTxtTestOgnFile);
	TheFs.Delete(KOgnFile1FF);
	TheFs.Delete(KOgnFile1FFF);
	TheFs.Delete(KOgnFile2FF);
	TheFs.Delete(KOgnFile22222);
	}

void SetupEnv(const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode)
	{
	ClearOgns();
	if (aTestMode==ETxt || aTestMode==EAutoTxt)
		TheFs.Delete(aOutFilePath);
	if (aTestMode==ECre)
		{
		//always copy so we always start the OOM with fresh original cre file
		CopyFile(KCopyTemplateCreFile,aInFilePath);
		TheFs.Delete(aOutFilePath);
		}
	if (aTestMode==EAutoCre)
		{
		TheFs.Delete(aOutFilePath);
#ifndef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
		CopyFile(KCopyPrivateTemplateCreFile,aInFilePath);
#else
		if (aInFilePath.Compare(KPrivateTemplateCreFile)==0)
			CopyFile(KCopyPrivateTemplateCreFile,aInFilePath);
		else
			CopyFile(KCopyPrivateTemplateCreV1File,aInFilePath);
#endif		
		}
	}

// If testsymcenrep or t_cenreppc fails, common_crc.txt and common_ref_00022222.cre might need
// to be updated (with the versions generated by the WINSCW t_cenrepcs.exe test,
// crc.txt and ref_00022222.cre), if the format of the repository files has changed.
void CrcComparisonTestL()
	{
	// Auto Txt Test
	TUint32 txtTestCrcVal;
	BasicFunctionL(uid,KPrivateTemplateTxtFile,KPrivateTemplateTxtOutFile,EAutoTxt);
	CopyFile(KPrivateTemplateTxtOutFile, KTxtTestGenFile);
	DoFileCompL(KTxtTestGenFile, KTxtTestRefFile, txtTestCrcVal);
	DoCrcCompL(txtTestCrcVal, KCrcRecordFile, EFalse);

	// Auto Cre Test
	TUint32 creTestCrcVal;
	TheFs.Delete(KPrivateCreFileForCreTest);
	CopyFile(KCopyCreFileForCreTest, KPrivateCreFileForCreTest);
	BasicFunctionL(uidcretest,KNullDesC,KNullDesC,EAutoTxt);
	CopyFile(KPrivateCreFileForCreTest, KCreTestGenFile);
	DoFileCompL(KCreTestGenFile, KCreTestRefFile, creTestCrcVal);
	DoCrcCompL(creTestCrcVal, KCrcRecordFile, ETrue);
	}

void FlushFunctionalityTestingL(CRepository* aRepository)
	{
	TInt r= aRepository->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
	TheTest(r==KErrNone);
	aRepository->CleanupFailTransactionPushL();	
	
	User::LeaveIfError(aRepository->Set(1,9));
	
	CleanupStack::Pop();
	aRepository->FailTransaction();
	
	User::LeaveIfError(aRepository->Flush());
	TInt value=0;
	User::LeaveIfError(aRepository->Get(1,value));
	TheTest(value==9);
	}

void FlushTestingL(TUid aUid,const TDesC& aInFilePath,const TDesC& aOutFilePath,TUint aTestMode)
	{
	__UHEAP_MARK;
	SetupEnv(aInFilePath,aOutFilePath,aTestMode);
	
	//basic functionality test
	CRepository* repository=NULL;
	InitialiseLC(repository,aUid,aInFilePath,aOutFilePath,aTestMode);	
	
	//made some modification
	FlushFunctionalityTestingL(repository);
	CleanupStack::PopAndDestroy();

	//oom functionality test
	OomTest(FlushFunctionalityTestingL,aUid,aInFilePath,aOutFilePath,aTestMode);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID			PDS-CENTRALREPOSITORY-CT-4080
@SYMTestCaseDesc		Test for DEF130394
@SYMTestPriority		Medium
@SYMTestActions			This is Part 2 of the test - Part 1 is in t_cenrrepcs.cpp
						Loading a repository multiple times and then compare 
						(using CRC compare) the created cre file with the reference 
						cre from CS test.
@SYMTestExpectedResults Cre file is identical as the reference cre.
@SYMDEF					DEF130394
*/	
void TestLoadingDEF130394L(const TDesC& aInFilePath, const TDesC& aOutFilePath)
	{
	CRepository* repos = NULL;
	
	repos = CRepository::NewLC(aInFilePath, aOutFilePath);
	TInt err = repos->Create(1,1);
	TEST2(err, KErrNone);
	
	CleanupStack::PopAndDestroy();
	
	// Second time we need to load from output file it self, otherwise it is not loading
	// a repository multiple times because changes of the first time loading is not
	// flushed into input file.
	repos = CRepository::NewLC(aOutFilePath, aOutFilePath);
	err = repos->Create(1,1);
	TEST2(err, KErrAlreadyExists);
	
	CleanupStack::PopAndDestroy();
	}

void TestForDEF130394L()
	{
	TUint32 dummy;
	
	// NewLC Txt=>Cre Loading test
	ClearOgns();
	TheFs.Delete(KMetaTestCreFile);
	TestLoadingDEF130394L(KMetaTxtTestInFile, KMetaTestCreFile);
	DoFileCompL(KMetaTestCreFile, KMetaTestRefFile, dummy);

	// NewLC Cre=>Cre Loading test
	ClearOgns();
	TheFs.Delete(KMetaTestCreFile);
	CopyFile(KMetaTestCreCopy, KMetaTestCreFile);
	TestLoadingDEF130394L(KMetaTestCreFile, KMetaTestCreFile);
	DoFileCompL(KMetaTestCreFile, KMetaTestRefFile, dummy);

	// Auto Loading Cre test
	ClearOgns();
	TheFs.Delete(KMetaTestFile);
	CopyFile(KMetaTestCopy, KMetaTestFile);
	DEF130394L(uidmetatest);
	DoFileCompL(KMetaTestRefFile, KMetaTestFile, dummy);
	
	//Auto Loading Txt test
	ClearOgns();
	TheFs.Delete(KMetaTestFile);
	DEF130394L(uidmetatest);
	DoFileCompL(KMetaTestRefFile, KMetaTestFile, dummy);
	}

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4041
@SYMTestCaseDesc		PC-side CentralRepository functionality test
@SYMTestPriority		High
@SYMTestActions			Wrapper function calling up test functions. Test functions include test for basic
						functions of the component, OOm test and CRC comparison test.
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2111
*/						   
LOCAL_C void MainL()
	{
	//Note these files are equivalent, the cre being generated from txt
	User::LeaveIfError(TheFs.Connect());

	//TXT => CRE
	TheTest.Start(_L("Functional NewLC TXT tests"));
	BasicFunctionL(uid,KTemplateTxtFile(),KTemplateTxtOutFile(),ETxt);
	TheTest.Next(_L("OOM NewLC TXT tests"));
	ObjectCreateDeleteOOM(uid,KTemplateTxtFile(),KTemplateTxtOutFile(),ETxt);
	OomBasicFunction(uid,KTemplateTxtFile(),KTemplateTxtOutFile(),ETxt);

	//CRE => CRE 
	TheTest.Next(_L("Functional NewLC CRE tests"));	
	BasicFunctionL(uidcre,KTemplateCreFile(),KTemplateCreOutFile,ECre);
	TheTest.Next(_L("OOM NewL CRE tests"));
	ObjectCreateDeleteOOM(uidcre,KTemplateCreFile(),KTemplateCreOutFile,ECre);
	OomBasicFunction(uidcre,KTemplateCreFile(),KTemplateCreOutFile,ECre);

	//CRE exists only
	TheTest.Next(_L("Functional NewLC Auto CRE tests"));	
	BasicFunctionL(uidcre,KPrivateTemplateCreFile(),KPrivateTemplateCreFile(),EAutoCre);
	TheTest.Next(_L("OOM NewLC Auto CRE tests"));	
	ObjectCreateDeleteOOM(uidcre,KPrivateTemplateCreFile(),KPrivateTemplateCreFile(),EAutoCre);
	OomBasicFunction(uidcre,KPrivateTemplateCreFile(),KPrivateTemplateCreFile(),EAutoCre);		

	//TXT exists only
	TheTest.Next(_L("Functional NewLC Auto TXT tests"));	
	BasicFunctionL(uid,KPrivateTemplateTxtFile(),KPrivateTemplateTxtOutFile(),EAutoTxt);
	TheTest.Next(_L("OOM NewLC Auto TXT tests"));					
	ObjectCreateDeleteOOM(uid,KPrivateTemplateTxtFile(),KPrivateTemplateTxtOutFile(),EAutoTxt);
	OomBasicFunction(uid,KPrivateTemplateTxtFile(),KPrivateTemplateTxtOutFile(),EAutoTxt);	

	//PC side only testing
	TheTest.Next(_L("Flush Functionality test"));
	FlushTestingL(uidcre,KPrivateTemplateCreFile(),KPrivateTemplateCreFile(),EAutoCre);
	FlushTestingL(uid,KPrivateTemplateTxtFile(),KPrivateTemplateTxtOutFile(),EAutoTxt);	

	TheTest.Next(_L("CRC comparison test"));
	CrcComparisonTestL();
	
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	//perform testing on CREV1 , only CRE exists only
	TheTest.Next(_L("Functional NewLC Auto CREV1 tests"));	
	BasicFunctionL(uidcrev1,KPrivateTemplateCreV1File(),KPrivateTemplateCreV1File(),EAutoCre);
	TheTest.Next(_L("OOM NewLC Auto CREV1 tests"));	
	ObjectCreateDeleteOOM(uidcrev1,KPrivateTemplateCreV1File(),KPrivateTemplateCreV1File(),EAutoCre);
	OomBasicFunction(uidcrev1,KPrivateTemplateCreV1File(),KPrivateTemplateCreV1File(),EAutoCre);

#endif	
	
	TheTest.Next(_L("Test for DEF130394L"));
	TestForDEF130394L();
	
	TheFs.Close();
	TheTest.End();
	TheTest.Close();
	}

TInt E32Main()
	{	
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, MainL());
	if (err != KErrNone)
		User::Panic(_L("Testing failed: "), err);
	
	delete cleanup;
	__UHEAP_MARKEND;

	return 0;
	}
