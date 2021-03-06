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


#include "symmetriccipherimpl.h"

#include <e32def.h>
#include <cryptostrength.h>
#include <cryptospi/cryptospidef.h>
#include "keys.h"
#include <cryptopanic.h>
#include <cryptospi/plugincharacteristics.h>
#include "pluginconfig.h"
#include <securityerr.h>
#include "common/inlines.h"

using namespace SoftwareCrypto;

//
// Implementation of Symmetric Cipher class
//
CSymmetricCipherImpl::CSymmetricCipherImpl() 
	{
	}

void CSymmetricCipherImpl::ConstructL(const CKey& aKey) 
	{
	DoSetKeyL(aKey);		
	}
	
void CSymmetricCipherImpl::SecureDelete(HBufC8*& aBuffer)
	{
	if (aBuffer)
		{
		aBuffer->Des().FillZ();
		}
	delete aBuffer;
	aBuffer = 0;	
	}

CSymmetricCipherImpl::~CSymmetricCipherImpl()
	{			
	SecureDelete(iKey);	
	}
		
void CSymmetricCipherImpl::Close()
	{
	delete this;
	}
	
TAny* CSymmetricCipherImpl::GetExtension(TUid /*aExtensionId*/) 
	{
	return 0;
	}
	
void CSymmetricCipherImpl::GetCharacteristicsL(const TAny*& aPluginCharacteristics)
	{
	TInt numCiphers = sizeof(KSymmetricCipherCharacteristics)/sizeof(TSymmetricCipherCharacteristics*);
	TInt32 implUid = ImplementationUid().iUid;
	for (TInt i = 0; i < numCiphers; ++i)
		{
		if (KSymmetricCipherCharacteristics[i]->cmn.iImplementationUID == implUid)
			{
			aPluginCharacteristics = KSymmetricCipherCharacteristics[i];
			break;
			}
		}	
	}

TInt CSymmetricCipherImpl::GetKeyStrength() const
	{
	return BytesToBits(iKey->Length());
	}
	
HBufC8* CSymmetricCipherImpl::ExtractKeyDataLC(const CKey& aKey) const
	{
	const TDesC8& keyContent = aKey.GetTDesC8L(KSymmetricKeyParameterUid);
	return keyContent.AllocLC();
	}

TInt CSymmetricCipherImpl::KeySize() const
	{
	// return key size in BITS
	return BytesToBits(iKeyBytes);
	}

void CSymmetricCipherImpl::DoSetKeyL(const CKey& aKey)
	{
	HBufC8* key = ExtractKeyDataLC(aKey);
	TInt keyLength(key->Length());
	
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(keyLength));
	if (! IsValidKeyLength(keyLength))
		{
		CleanupStack::PopAndDestroy(key);
		User::Leave(KErrNotSupported);
		}
	
	SecureDelete(iKey);	
	CleanupStack::Pop(key);
	iKey = key;
	iKeyBytes = keyLength;
	}	

//
// Implementation of Symmetric Stream Cipher
//
CSymmetricStreamCipherImpl::CSymmetricStreamCipherImpl()
	{
	}

CSymmetricStreamCipherImpl::~CSymmetricStreamCipherImpl()
	{
	}

void CSymmetricStreamCipherImpl::SetKeyL(const CKey& aKey)
	{
	DoSetKeyL(aKey);
	TCrypto::IsSymmetricWeakEnoughL(GetKeyStrength());
	Reset();
	}	

void CSymmetricStreamCipherImpl::ConstructL(const CKey& aKey) 
	{
	CSymmetricCipherImpl::ConstructL(aKey);
	}

TInt CSymmetricStreamCipherImpl::BlockSize() const
	{
	// return block size in BITS
	return BYTE_BITS;
	}

void CSymmetricStreamCipherImpl::SetCryptoModeL(TUid /*aCryptoMode*/)
	{
	// Call the reset method.
	Reset();
	}
	
TInt CSymmetricStreamCipherImpl::MaxOutputLength(TInt aInputLength) const
	{
	return aInputLength;	
	}
	
TInt CSymmetricStreamCipherImpl::MaxFinalOutputLength(TInt aInputLength) const
	{
	return aInputLength;	
	}
	
void CSymmetricStreamCipherImpl::ProcessL(const TDesC8& aInput, TDes8& aOutput)
	{
	TInt outputIndex = aOutput.Size();

	// aOutput may already have outputIndex bytes of data in it
	// check there will still be enough space to process the result
	__ASSERT_DEBUG(aOutput.MaxLength() - outputIndex >= MaxOutputLength(aInput.Length()), User::Panic(KCryptoPanic, ECryptoPanicOutputDescriptorOverflow));

	aOutput.Append(aInput);

	TPtr8 transformBuf((TUint8*)(aOutput.Ptr()) + outputIndex, aInput.Size(),
		aInput.Size());
	DoProcess(transformBuf);
	}

void CSymmetricStreamCipherImpl::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput)
	{
	ProcessL(aInput, aOutput);	
	}

//
// Implementation of Symmetric Block Cipher
//
CSymmetricBlockCipherImpl::CSymmetricBlockCipherImpl(
	TUint8 aBlockBytes,
	TUid aCryptoMode,
	TUid aOperationMode,
	TUid aPaddingMode) :
	iBlockBytes(aBlockBytes),
	iCryptoMode(aCryptoMode),
	iOperationMode(aOperationMode),
	iPaddingMode(aPaddingMode),
	iBufferedPlaintextPtr(0,0,0),
	iCtrUnusedKeystreamPtr(0,0,0)
	{
	}

CSymmetricBlockCipherImpl::~CSymmetricBlockCipherImpl()
	{			
	delete iPadding;
	delete [] iRegister;
	delete [] iCurrentCipherText;
	delete iBufferedPlaintext;
	delete iCtrUnusedKeystream;
	iIv.Close();
	iInputStore.Close();
	iPaddingBlock.Close();	
	}


void CSymmetricBlockCipherImpl::ConstructL(const CKey& aKey) 
	{
	CSymmetricCipherImpl::ConstructL(aKey);
	DoSetOperationModeL(iOperationMode);
	DoSetCryptoModeL(iCryptoMode);	
	DoSetPaddingModeL(iPaddingMode);
	
	iInputStore.ReAllocL(iBlockBytes);
	iPaddingBlock.ReAllocL(iBlockBytes);

	iRegister = new(ELeave) TUint32[iBlockBytes/4];	
	iRegisterPtr = reinterpret_cast<TUint8*>(iRegister);

	iCurrentCipherText = new(ELeave) TUint32[iBlockBytes/4];	
	iCurrentCipherTextPtr = reinterpret_cast<TUint8*>(iCurrentCipherText);
	
	iBufferedPlaintext = HBufC8::NewL(iBlockBytes);
	iBufferedPlaintextPtr.Set(iBufferedPlaintext->Des());
	
	iCtrUnusedKeystream = HBufC8::NewL(iBlockBytes);
	iCtrUnusedKeystreamPtr.Set(iCtrUnusedKeystream->Des());
	}

void CSymmetricBlockCipherImpl::Reset()
	{
	iInputStore.Zero();
	iPaddingBlock.Zero();
	iCtrUnusedKeystreamPtr.Zero();
	
	if (iOperationMode.iUid == KOperationModeCBC)
		{
		// only copy the IV if it is already set
		if (iIv.MaxLength() > 0)
			{
			Mem::Copy(iRegisterPtr, &iIv[0], iBlockBytes);
			}
		}
	}	

void CSymmetricBlockCipherImpl::SetKeyL(const CKey& aKey)
	{
	DoSetKeyL(aKey);
	TCrypto::IsSymmetricWeakEnoughL(GetKeyStrength());
	SetKeySchedule();
	Reset();
	}

void CSymmetricBlockCipherImpl::SetOperationModeL(TUid aOperationMode)
	{
	DoSetOperationModeL(aOperationMode);
	Reset();
	}
	
void CSymmetricBlockCipherImpl::SetCryptoModeL(TUid aCryptoMode)
	{
	DoSetCryptoModeL(aCryptoMode);
	SetKeySchedule();
	Reset();
	}
	
void CSymmetricBlockCipherImpl::SetPaddingModeL(TUid aPaddingMode)
	{
	DoSetPaddingModeL(aPaddingMode);
	Reset();
	}
	
void CSymmetricBlockCipherImpl::SetIvL(const TDesC8& aIv)
	{
	if ((iOperationMode.iUid != KOperationModeCBC) && (iOperationMode.iUid != KOperationModeCTR))
		{
		User::Leave(KErrNotSupported);
		}
	DoSetIvL(aIv);
	Reset();
	}

void CSymmetricBlockCipherImpl::DoSetOperationModeL(TUid aOperationMode)
	{
	switch (aOperationMode.iUid)
		{
		case KOperationModeNone:
		case KOperationModeECB:
		case KOperationModeCBC:
			break;
		case KOperationModeCTR:
			SetCryptoModeL(KCryptoModeEncryptUid);
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	iOperationMode = aOperationMode;		
	}

void CSymmetricBlockCipherImpl::DoSetCryptoModeL(TUid aCryptoMode)
	{
	switch (aCryptoMode.iUid)
		{
		case KCryptoModeEncrypt:
			break;
		case KCryptoModeDecrypt:
			if (iOperationMode.iUid == KOperationModeCTR)
				{
				return;
				}
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	iCryptoMode = aCryptoMode;		
	}

void CSymmetricBlockCipherImpl::DoSetPaddingModeL(TUid aPaddingMode)
	{
	CPadding* padding(0);
	switch (aPaddingMode.iUid)
		{
		case KPaddingModeNone:
			padding = CPaddingNone::NewL(iBlockBytes);
		break;
		case KPaddingModeSSLv3:
			padding = CPaddingSSLv3::NewL(iBlockBytes);
		break;
		case KPaddingModePKCS7:
			padding = CPaddingPKCS7::NewL(iBlockBytes);
		break;
		default:
			User::Leave(KErrNotSupported);
		}
	delete iPadding;
	iPadding = padding;
	iPaddingMode = aPaddingMode;
	}	

void CSymmetricBlockCipherImpl::DoSetIvL(const TDesC8& aIv)
	{
	iIv.ReAllocL(iBlockBytes);
	iIv.SetLength(iBlockBytes);

	iIv.Zero();
	if (aIv.Length() != iBlockBytes) 
		{
		User::Leave(KErrArgument);
		}
	iIv = aIv;
	Mem::Copy(iRegisterPtr, &iIv[0], iBlockBytes);	//for CTR mode

	}	

TInt CSymmetricBlockCipherImpl::BlockSize() const
	{
	// return block size in BITS
	if (iOperationMode.iUid == KOperationModeCTR)
		{
		return 8;
		}
	else
		{
		return BytesToBits(iBlockBytes);
		}
	}

TInt CSymmetricBlockCipherImpl::MaxOutputLength(TInt aInputLength) const
	{	
	if (iOperationMode.iUid == KOperationModeCTR)
		{
		return aInputLength;
		}
	else
		{
		// The maximum output length required for Process is equal to the
		// size of the number of whole input blocks available.
		//
		// The block bytes is a power of two so we can use this to avoid
		// doing a real mod operation
		TUint inputStoreLength(iInputStore.Length());
		TInt rem = (aInputLength + inputStoreLength) & (iBlockBytes - 1);
		return (aInputLength + inputStoreLength - rem);
		}
	}	

TInt CSymmetricBlockCipherImpl::MaxFinalOutputLength(TInt aInputLength) const
	{
	if (iOperationMode.iUid == KOperationModeCTR)
		{
		return aInputLength;
		}
	else if (iCryptoMode.iUid == KCryptoModeEncrypt)
		{
		return iPadding->MaxPaddedLength(iInputStore.Length() + aInputLength);
		}
	else
		{
		return iPadding->MaxUnPaddedLength(aInputLength + iInputStore.Size());
		}
	}

void CSymmetricBlockCipherImpl::ProcessL(const TDesC8& aInput, TDes8& aOutput)
	{
	// if we're running in CBC or CTR mode then we must have an IV set before we can 
	// do any processing ie call SetIvL() before this method
	if ((iOperationMode.iUid == KOperationModeCBC) || (iOperationMode.iUid == KOperationModeCTR))
		{
		if (iIv.MaxLength() == 0)
			{
			User::Leave(KErrNotSupported);
			}
		}

	TInt inputLength(aInput.Length());	
	TInt inputStoreLength(iInputStore.Length());
	
	if (MaxOutputLength(inputLength) > aOutput.MaxLength())
		{
		User::Leave(KErrOverflow);
		}	

	if (iOperationMode.iUid == KOperationModeCTR)
		{
		ProcessCtrL(aInput, aOutput);
		}	
	else
		{
		TUint8 blockSizeLog = CryptoLog2(iBlockBytes);
		TInt wholeBlocks = (inputLength + inputStoreLength) >> blockSizeLog; 
		TInt wholeBlocksSize = wholeBlocks << blockSizeLog;
	
		if (wholeBlocks)
			{
			TInt outputLength(aOutput.Length());

			if (inputStoreLength > 0)
				{
				aOutput.Append(iInputStore);
				iInputStore.Zero();
				}
			aOutput.Append(aInput.Left(wholeBlocksSize - inputStoreLength));
			Transform(const_cast<TUint8*>(aOutput.Ptr()) + outputLength, wholeBlocks);
			}
		
		TInt remainingBytes = inputLength + inputStoreLength - wholeBlocksSize;
		if (remainingBytes > 0)
			{		
			iInputStore.Append(aInput.Right(remainingBytes));
			}
		}
	}
		
void CSymmetricBlockCipherImpl::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput)
	{
	if (iOperationMode.iUid == KOperationModeCTR)
		{
		ProcessL(aInput, aOutput);
		}
	else
		{
		// if we're running in CBC mode then we must have an IV set before we can 
		// do any processing ie call SetIvL() before this method
		if (iOperationMode.iUid == KOperationModeCBC)
			{
			if (iIv.MaxLength() == 0)
				{
				User::Leave(KErrNotSupported);
				}
			}

		if (iCryptoMode.iUid == KCryptoModeEncrypt)
			{
			return DoProcessFinalEncryptL(aInput, aOutput);
			}
		else
			{
			return DoProcessFinalDecryptL(aInput, aOutput);
			}
		}
	}

void CSymmetricBlockCipherImpl::DoProcessFinalEncryptL(const TDesC8& aInput, TDes8& aOutput)
	{	
	if (MaxFinalOutputLength(aInput.Length()) > aOutput.MaxLength() - aOutput.Length())
		{
		User::Leave(KErrOverflow);
		}
		
	// process everything up to the last (possibly empty block)
	TInt outputStartIndex = aOutput.Length();
	ProcessL(aInput, aOutput);

	// pad the plaintext
	iPadding->PadL(iInputStore, iPaddingBlock);
	
	// if padding required
	if (iPaddingBlock.Length() > 0)
		{
		iInputStore.Zero();

		// make sure the output is a multiple of the block size
		User::LeaveIfError(((aOutput.Length() - outputStartIndex + iPaddingBlock.Length()) % iBlockBytes) == 0 ? KErrNone : KErrInvalidPadding);

		outputStartIndex = aOutput.Length();
		aOutput.Append(iPaddingBlock);
		iPaddingBlock.Zero();
		TransformEncrypt(const_cast<TUint8*>(aOutput.Ptr()) + outputStartIndex, 1);		
		}
	}

void CSymmetricBlockCipherImpl::DoProcessFinalDecryptL(const TDesC8& aInput, TDes8& aOutput)
	{
	if (MaxFinalOutputLength(aInput.Length()) > aOutput.MaxLength() - aOutput.Length())
		{
		User::Leave(KErrOverflow);
		}

	// Input length (including inputstore) must be a multiple of the 
	// block size in length
	if ((aInput.Length() + iInputStore.Length()) & (iBlockBytes - 1)) 
		{
		User::Leave(KErrArgument);
		}

	if(aInput.Length() > iBlockBytes)
		{
		HBufC8* processBuf = HBufC8::NewLC(MaxFinalOutputLength(aInput.Length()));
		TPtr8 processPtr = processBuf->Des(); 
	
		ProcessL(aInput, processPtr);

		ASSERT(iInputStore.Length()==0); // all the blocks should have been decrypted
		
		// Unpad processPtr into aOutput
		iPadding->UnPadL(processPtr, aOutput);

		CleanupStack::PopAndDestroy(processBuf);
		}
	else 
		{
		// now contains the final ciphertext block
		iInputStore.Append(aInput);

		// Decrypt the last _padding_ blocksize into a new buffer
		TransformDecrypt(const_cast<TUint8*>(iInputStore.Ptr()), 1);
		
		// Unpad the last block and append to output
		iPadding->UnPadL(iInputStore, aOutput);
		}
			
	iPaddingBlock.Zero();
	iInputStore.Zero();
	}

	
/**
CTR mode behaves like a stream cipher, accepting input of any arbitrary length. This results 
in a significant body of code that behaves fundamentally differently to the ECB and CBC modes. 
ProcessCtrL() is called by ProcessL() when operating in CTR mode, wrapping up all this 
functionality into a separate method for clarity.

Encrypting zero-filled bytes will return the keystream since the output of Transformation is simply 
the input XORed with the keystream.
	
See: http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf
*/
void CSymmetricBlockCipherImpl::ProcessCtrL(const TDesC8& aInput, TDes8& aOutput)
	{
	TInt inputLength(aInput.Length());	

	TInt outputLength(aOutput.Length());
	TInt amountToXor = Min(iCtrUnusedKeystreamPtr.Length(), inputLength);

	// Try applying previously unused key stream bytes.
	if (amountToXor > 0)
		{
		aOutput.Append(aInput.Left(amountToXor));
		for (TInt i = 0; i < amountToXor; ++i)
			{
			aOutput[outputLength + i] ^= iCtrUnusedKeystreamPtr[i];
			}
		iCtrUnusedKeystreamPtr = iCtrUnusedKeystreamPtr.RightTPtr((iCtrUnusedKeystreamPtr.Length() - amountToXor));	
		}
		
	TInt amountToEncode = inputLength - amountToXor;
	
	if ((iCtrUnusedKeystreamPtr.Length() == 0) && (amountToEncode > 0))
		{
		// For each whole block's worth of input, transform it.
		TInt wholeBlocks = (amountToEncode) / iBlockBytes; 
		TInt wholeBlocksSize = wholeBlocks * iBlockBytes;		
		outputLength = aOutput.Length();
		
		if (wholeBlocks)
			{
			aOutput.Append(aInput.Mid(amountToXor, wholeBlocksSize));
			Transform(const_cast<TUint8*>(aOutput.Ptr()) + outputLength, wholeBlocks);
			}
			
		// CTR mode can handle arbitrary sized inputs. Here any remaining input data of less than the block size
		// is padded with zeros and then transformed. On return this padded section of the block will contain the next
		// sequence of keystream, which is copied to iCtrUnusedKeystream for use next time ProcessCtrL() is called.
		TInt remainingBytes = amountToEncode - wholeBlocksSize;
		iCtrUnusedKeystreamPtr = iCtrUnusedKeystream->Des();
		iCtrUnusedKeystreamPtr.SetMax();
		iCtrUnusedKeystreamPtr.FillZ();
		iCtrUnusedKeystreamPtr.Copy(aInput.Right(remainingBytes));
		iCtrUnusedKeystreamPtr.SetLength(iBlockBytes);	
	
		Transform(const_cast<TUint8*>(iCtrUnusedKeystreamPtr.Ptr()), 1);
	
		aOutput.Append(iCtrUnusedKeystreamPtr.Left(remainingBytes));
			
		iCtrUnusedKeystreamPtr = iCtrUnusedKeystreamPtr.RightTPtr((iCtrUnusedKeystreamPtr.Length() - remainingBytes));	
		}
	}



// Methods implemented in subclass. No coverage here.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif
void CSymmetricStreamCipherImpl::SetOperationModeL(TUid /*aOperationMode*/)
	{
	// Override in subclass
	User::Leave(KErrNotSupported);
	}
	
void CSymmetricStreamCipherImpl::SetPaddingModeL(TUid /*aPaddingMode*/)
	{
	// Override in subclass
	User::Leave(KErrNotSupported);
	}
	
void CSymmetricStreamCipherImpl::SetIvL(const TDesC8& /*aIv*/)
	{
	// Override in subclass
	User::Leave(KErrNotSupported);
	}
