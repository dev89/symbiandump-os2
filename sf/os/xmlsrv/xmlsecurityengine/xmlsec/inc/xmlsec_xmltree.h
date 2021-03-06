/** 
 * XML Security Library (http://www.aleksey.com/xmlsec).
 *
 * Common XML utility functions
 *
 * This is free software; see Copyright file in the source
 * distribution for preciese wording.
 * 
 * Copyright (C) 2002-2003 Aleksey Sanin <aleksey@aleksey.com>
 * Portion Copyright � 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#ifndef __XMLSEC_TREE_H__
#define __XMLSEC_TREE_H__    

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 
#include "xmlsec_config.h"
#include <stdapis/libxml2/libxml2_tree.h>
#include <stdapis/libxml2/libxml2_xpath.h>
#include "xmlsec_xmlsec.h"

/**
 * xmlSecNodeGetName:
 * @node:		the pointer to node.
 *
 * Macro. Returns node's name.
 */
#define xmlSecNodeGetName(node) \
    (((node)) ? ((const char*)((node)->name)) : NULL)

XMLSEC_EXPORT const xmlChar* 	xmlSecGetNodeNsHref	(const xmlNodePtr cur);
XMLSEC_EXPORT int		xmlSecCheckNodeName	(const xmlNodePtr cur, 
							 const xmlChar *name, 
							 const xmlChar *ns);
XMLSEC_EXPORT xmlNodePtr	xmlSecGetNextElementNode(xmlNodePtr cur);
XMLSEC_EXPORT xmlNodePtr	xmlSecFindChild		(const xmlNodePtr parent,
							 const xmlChar *name,
							 const xmlChar *ns);
XMLSEC_EXPORT xmlNodePtr	xmlSecFindParent	(const xmlNodePtr cur, 
							 const xmlChar *name, 
							 const xmlChar *ns);
XMLSEC_EXPORT xmlNodePtr	xmlSecFindNode		(const xmlNodePtr parent,
							 const xmlChar *name,
							 const xmlChar *ns);
XMLSEC_EXPORT xmlNodePtr	xmlSecAddChild		(xmlNodePtr parent, 
							 const xmlChar *name,
							 const xmlChar *ns);
XMLSEC_EXPORT xmlNodePtr	xmlSecAddChildNode	(xmlNodePtr parent, 
							 xmlNodePtr child);
XMLSEC_EXPORT xmlNodePtr	xmlSecAddNextSibling	(xmlNodePtr node, 
							 const xmlChar *name,
							 const xmlChar *ns);
XMLSEC_EXPORT xmlNodePtr	xmlSecAddPrevSibling	(xmlNodePtr node, 
							 const xmlChar *name,
							 const xmlChar *ns);

XMLSEC_EXPORT int		xmlSecReplaceNode	(xmlNodePtr node,
						         xmlNodePtr newNode);
XMLSEC_EXPORT int		xmlSecReplaceContent	(xmlNodePtr node,
							 xmlNodePtr newNode);
XMLSEC_EXPORT int		xmlSecReplaceNodeBuffer	(xmlNodePtr node,
							 const xmlSecByte *buffer, 
							 xmlSecSize size);

XMLSEC_EXPORT void		xmlSecAddIDs		(xmlDocPtr doc,
							 xmlNodePtr cur,
							 const xmlChar** ids);
XMLSEC_EXPORT int		xmlSecGenerateAndAddID	(xmlNodePtr node,
							 const xmlChar* attrName,
							 const xmlChar* prefix,
							 xmlSecSize len);
XMLSEC_EXPORT xmlChar*		xmlSecGenerateID	(const xmlChar* prefix,
							 xmlSecSize len);

XMLSEC_EXPORT xmlDocPtr		xmlSecCreateTree	(const xmlChar* rootNodeName,
							 const xmlChar* rootNodeNs);
XMLSEC_EXPORT int		xmlSecIsEmptyNode	(xmlNodePtr node);
XMLSEC_EXPORT int		xmlSecIsEmptyString	(const xmlChar* str);
XMLSEC_EXPORT xmlChar*		xmlSecGetQName		(xmlNodePtr node,
							 const xmlChar* href,
							 const xmlChar* local);

/**
 * xmlSecIsHex:
 * @c: 			the character.
 * 
 * Macro. Returns 1 if @c is a hex digit or 0 other wise.
 */
#define xmlSecIsHex(c) \
    (( (('0' <= (c)) && ((c) <= '9')) || \
       (('a' <= (c)) && ((c) <= 'f')) || \
       (('A' <= (c)) && ((c) <= 'F')) ) ? 1 : 0)

/**
 * xmlSecGetHex:
 * @c: 			the character,
 *
 * Macro. Returns the hex value of the @c.
 */
#define xmlSecGetHex(c) \
    ( (('0' <= (c)) && ((c) <= '9')) ? (c) - '0' : \
    ( (('a' <= (c)) && ((c) <= 'f')) ? (c) - 'a' + 10 :  \
    ( (('A' <= (c)) && ((c) <= 'F')) ? (c) - 'A' + 10 : 0 )))

/*************************************************************************
 *
 * QName <-> Integer mapping
 *
 ************************************************************************/
typedef struct _xmlSecQName2IntegerInfo			xmlSecQName2IntegerInfo,
							*xmlSecQName2IntegerInfoPtr;
typedef const struct _xmlSecQName2IntegerInfo*		xmlSecQName2IntegerInfoConstPtr;

struct _xmlSecQName2IntegerInfo {
    const xmlChar*      qnameHref;
    const xmlChar*      qnameLocalPart;
    int       		intValue;
};
XMLSEC_EXPORT xmlSecQName2IntegerInfoConstPtr xmlSecQName2IntegerGetInfo	
								(xmlSecQName2IntegerInfoConstPtr info,
								 int intValue);
XMLSEC_EXPORT int		xmlSecQName2IntegerGetInteger	(xmlSecQName2IntegerInfoConstPtr info,
								 const xmlChar* qnameHref,
								 const xmlChar* qnameLocalPart,
								 int* intValue);
XMLSEC_EXPORT int		xmlSecQName2IntegerGetIntegerFromString	
								(xmlSecQName2IntegerInfoConstPtr info,
								 xmlNodePtr node,
								 const xmlChar* qname,
								 int* intValue);
XMLSEC_EXPORT xmlChar* 		xmlSecQName2IntegerGetStringFromInteger
								(xmlSecQName2IntegerInfoConstPtr info,
								 xmlNodePtr node,
								 int intValue);
XMLSEC_EXPORT int		xmlSecQName2IntegerNodeRead	(xmlSecQName2IntegerInfoConstPtr info,
								 xmlNodePtr node,
								 int* intValue);
XMLSEC_EXPORT int		xmlSecQName2IntegerNodeWrite	(xmlSecQName2IntegerInfoConstPtr info,
								 xmlNodePtr node,
								 const xmlChar* nodeName,
								 const xmlChar* nodeNs,
								 int intValue);
XMLSEC_EXPORT int		xmlSecQName2IntegerAttributeRead(xmlSecQName2IntegerInfoConstPtr info,
								 xmlNodePtr node,
								 const xmlChar* attrName,
								 int* intValue);
XMLSEC_EXPORT int		xmlSecQName2IntegerAttributeWrite(xmlSecQName2IntegerInfoConstPtr info,
								 xmlNodePtr node,
								 const xmlChar* attrName,
								 int intValue);
XMLSEC_EXPORT void		xmlSecQName2IntegerDebugDump	(xmlSecQName2IntegerInfoConstPtr info,
								 int intValue,
								 const xmlChar* name,
								 FILE* output);
XMLSEC_EXPORT void		xmlSecQName2IntegerDebugXmlDump(xmlSecQName2IntegerInfoConstPtr info,
								 int intValue,
								 const xmlChar* name,
								 FILE* output);

/*************************************************************************
 *
 * QName <-> Bits mask mapping
 *
 ************************************************************************/
typedef unsigned int                            	xmlSecBitMask;
typedef struct _xmlSecQName2BitMaskInfo			xmlSecQName2BitMaskInfo,
							*xmlSecQName2BitMaskInfoPtr;
typedef const struct _xmlSecQName2BitMaskInfo*		xmlSecQName2BitMaskInfoConstPtr;

struct _xmlSecQName2BitMaskInfo {
    const xmlChar*      qnameHref;
    const xmlChar*      qnameLocalPart;
    xmlSecBitMask       mask;
};

XMLSEC_EXPORT xmlSecQName2BitMaskInfoConstPtr xmlSecQName2BitMaskGetInfo	
								(xmlSecQName2BitMaskInfoConstPtr info,
								 xmlSecBitMask mask);
XMLSEC_EXPORT int		xmlSecQName2BitMaskGetBitMask	(xmlSecQName2BitMaskInfoConstPtr info,
								 const xmlChar* qnameLocalPart,
								 const xmlChar* qnameHref,
								 xmlSecBitMask* mask);
XMLSEC_EXPORT int		xmlSecQName2BitMaskNodesRead	(xmlSecQName2BitMaskInfoConstPtr info,
								 xmlNodePtr* node,
								 const xmlChar* nodeName,
								 const xmlChar* nodeNs,
								 int stopOnUnknown,
								 xmlSecBitMask* mask);
XMLSEC_EXPORT int		xmlSecQName2BitMaskGetBitMaskFromString	
								(xmlSecQName2BitMaskInfoConstPtr info,
								 xmlNodePtr node,
								 const xmlChar* qname,
								 xmlSecBitMask* mask);
XMLSEC_EXPORT xmlChar* 		xmlSecQName2BitMaskGetStringFromBitMask
								(xmlSecQName2BitMaskInfoConstPtr info,
								 xmlNodePtr node,
								 xmlSecBitMask mask);
XMLSEC_EXPORT int		xmlSecQName2BitMaskNodesWrite	(xmlSecQName2BitMaskInfoConstPtr info,
								 xmlNodePtr node,
								 const xmlChar* nodeName,
								 const xmlChar* nodeNs,
								 xmlSecBitMask mask);
XMLSEC_EXPORT void		xmlSecQName2BitMaskDebugDump	(xmlSecQName2BitMaskInfoConstPtr info,
								 xmlSecBitMask mask,
								 const xmlChar* name,
								 FILE* output);
XMLSEC_EXPORT void		xmlSecQName2BitMaskDebugXmlDump(xmlSecQName2BitMaskInfoConstPtr info,
								 xmlSecBitMask mask,
								 const xmlChar* name,
								 FILE* output);

								 


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XMLSEC_TREE_H__ */

