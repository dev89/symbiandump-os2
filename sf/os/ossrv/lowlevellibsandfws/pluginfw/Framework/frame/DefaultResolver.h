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


#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_RESOLVER_3A1D097B0305_INCLUDED
#define _INC_RESOLVER_3A1D097B0305_INCLUDED

#include <ecom/resolver.h>
#include "RegistryData.h"

class TEComResolverParams;

/**
	@internalComponent
	@since 7.0
	Controls the identification, (resolution), of which implementation 
	will be used to satisfy an interface implementation instantiation.
 */

class CDefaultResolver : public CResolver
{
public:
/**
	@fn				NewL(CRegistryData& aRegistry)
	Intended Usage	: Standardized safe construction which leaves nothing 
					on the cleanup stack.	
	Error Condition	: Cannot fully construct because of memory limitations.	
	@leave  		KErrNoMemory
	@since			7.0
	@return			A pointer to the new class
	@post			CDefaultResolver is fully constructed, 
					and initialized.
 */
	
	static CDefaultResolver* NewL(MPublicRegistry& aRegistry);

/**
	@fn				~CDefaultResolver()
	Intended Usage	: Standard default d'tor	
	Error Condition	: None	
	@since			7.0
 */
	
	~CDefaultResolver();

/**
	@fn				IdentifyImplementationL(TUid aInterfaceUid, 
											const TEComResolverParams& aAdditionalParameters) const
	Intended Usage	:	Request that the resolver identify the most appropriate 
						interface implementation.
	Error Condition	:	
	@since			7.0
	@param			aInterfaceUid The interface for which implementations are requested
	@param			aAdditionalParameters Data to be used to refine the search further
	@return			The Uid of the best fit interface implementation - KNullUid if no match is found
	@pre 			Object is fully constructed and initialized
	@post			Registry contents are not modified but registry keys may be updated
 */
	
	TUid IdentifyImplementationL(TUid aInterfaceUid, const TEComResolverParams& aAdditionalParameters)const ;

/**
	@fn				ListAllL(TUid aInterfaceUid, const TEComResolverParams& aAdditionalParameters) const
	Intended Usage	:	List all the implementations which satisfy the specified interface.
	Error Condition	:	
	@since			7.0
	@param			aInterfaceUid The interface for which implementations are requested
	@param			aAdditionalParameters Data to be used to refine the search further
	@return			Pointer to an array of suitable implementations. Ownership of this array
	is passed to the calling function.
	@pre 			Object is fully constructed and initialized
	@post			Registry contents are not modified but registry keys may be updated
 */
	
	RImplInfoArray* ListAllL(TUid aInterfaceUid, 
							 const TEComResolverParams& aAdditionalParameters)const;

protected:
/**
	@internalComponent
	@fn				CDefaultResolver(CRegistryData& aRegistry)
	Intended Usage	: Standardized default c'tor	
	Error Condition	: None	
	@since			7.0
	@post			CDefaultResolver is fully constructed
 */
	
	explicit CDefaultResolver(MPublicRegistry& aRegistry);

/**
	@fn				Resolve(const RImplInfoArray& aImplementationsInfo, 
							const TEComResolverParams& aAdditionalParameters) const
	Intended Usage	:	Called by IdentifyImplementationL to select an appropriate 
					implementation from a list of possibles
	@since			7.0
	@param			aImplementationsInfo Information on the potential implementations
	@param			aAdditionalParameters The data to match against to detemine the 
					implementation
	@return			The Uid of the selected implementation - KNullUid if no match is found
	@pre 			This object is fully constructed
 */
	
	TUid Resolve(const RImplInfoArray& aImplementationsInfo, 
				 const TEComResolverParams& aAdditionalParameters) const;

private:
/**
	@fn				Match(const TDesC8& aImplementationType, 
						  const TDesC8& aMatchType, 
						  TBool aUseWildcards) const
	Intended Usage	:	Searches for a match of a data type on an implementation type.
	Match returns ETrue if aMatchType is found within aImplementationType according to 
	the following rules:
	1) aImplementationType is treated as a series of descriptors separated by double 
	bars (||). ETrue is returned if aMatchType matches exactly any of the short 
	descriptors.  If no double bar is present then aImplementationType is treated as a
	single descriptor.
	2) If aUseWildcards == ETrue then a '?' in aMatchType will be matched to any single
	character and '*' will be matched to any series of characters.
	@leave  		KErrNoMemory
	@since			7.0
	@param			aImplementationType The implementation data type to search for a match
	@param			aMatchType The data to search for
	@param			aUseWildcards ETrue if wildcard matching should be allowed
	@return			ETrue if a match was found, EFalse otherwise
	@pre 			This object is fully constructed
 */
	
	TBool Match(const TDesC8& aImplementationType, 
				const TDesC8& aMatchType, 
				TBool aUseWildcards) const;

protected:
	
	

	/** Required for test code to have access to internal state */
	friend class TDefaultResolver_StateAccessor;
};

#endif /* _INC_RESOLVER_3A1D097B0305_INCLUDED */

