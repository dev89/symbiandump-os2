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
// IP SubConnection Provider implementation
// 
//

/**
 @file
 @internalComponent
*/

#include <etelqos.h>
#include "IPSCPRStates.h"
#include "IPSCPR.h"
#include "ipdeftbasescpr.h"
#include "../../iptransportlayer/src/ipscprlog.h"
#include "pfqos_stream.h"
#include "qos_msg.h"
#include <networking/qoserr.h>
#include <networking/sblpextn.h>
#include <comms-infras/ss_log.h>
#include "IPMessages.h"
#include <comms-infras/ss_msgintercept.h>
#include <elements/nm_messages_base.h>
#include <elements/nm_messages_child.h>
#include <elements/nm_messages_peer.h>
#include <comms-infras/ss_nodemessages_factory.h>


//TODO remove this ECFActivityDataClientJoin activity
#include "../../../../commsfw/datacommsserver/esockserver/inc/ss_internal_activities.h"

#if defined __FLOG_ACTIVE || defined SYMBIAN_TRACE_ENABLE
	#define KIPSCprTag KESockSubConnectionTag
	//_LIT8(KIPSCprSubTag, "ipscpr");
#endif


using namespace Messages;
using namespace MeshMachine;
using namespace ESock;
using namespace NetStateMachine;

namespace QoSIPSCprJoinActivity
{ //This activity needs the activity object (& it can fail on AddClientL, so no point converting)
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityDataClientJoin, QoSIpSCprJoin, TCFPeer::TJoinRequest, MeshMachine::CNodeRetryActivity::NewL)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingDataClientJoinRequest, QoSIpSCprStates::TNoTagBlockedByOpenInternalSocket)
	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TAddDataClientAndRespond)
NODEACTIVITY_END()
}

namespace QoSIPSCprClientLeaveActivity
{ //This activity does not need the activity object and cannot fail
DECLARE_DEFINE_NODEACTIVITY(ECFActivityClientLeave, QoSIpSCprLeave, Messages::TNodeSignal::TNullMessageId)
	NODEACTIVITY_ENTRY(KNoTag, QoSIpSCprStates::TRemoveLeavingClientFromQoSChannel, CoreStates::TAwaitingClientLeave, MeshMachine::TNoTag)
NODEACTIVITY_END()
}

/**
   This activity opens the PfQos internal socket asynchronously.
   Join activity is synchronised on this, so no clients can join until the internal socket has been opened or
   the internal socket opening has failed.
*/
namespace QoSIPSCprOpenInternalSocket
{
DECLARE_DEFINE_NODEACTIVITY(QoSIpSCprActivities::EQoSIpSCprOpenInternalSocket, qosIpScprOpenInternalSocket, TQoSIpSCprMessages::TOpenInternalSocket)
    FIRST_NODEACTIVITY_ENTRY(QoSIpSCprStates::TAwaitingOpenInternalSocket, MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(KNoTag, QoSIpSCprStates::TOpenInternalSocket, QoSIpSCprStates::TAwaitingInternalSocketOpened, MeshMachine::TNoTag)
    LAST_NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing)
NODEACTIVITY_END()
}

namespace QoSIPSCprSetParameters
{
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityParamRequest, qosIpScprParamRequest, TCFScpr::TSetParamsRequest, MeshMachine::CNodeRetryActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(PRStates::TAwaitingParamRequest, QoSIpSCprStates::TNoTagBlockedByOpenInternalSocket)
#else
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityParamRequest, qosIpScprParamRequest, TCFScpr::TParamsRequest, MeshMachine::CNodeRetryActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(SCprStates::TAwaitingParamRequest, QoSIpSCprStates::TNoTagBlockedByOpenInternalSocket)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    LAST_NODEACTIVITY_ENTRY(KNoTag, QoSIpSCprStates::TStoreAndSetParameters)
NODEACTIVITY_END()
}

namespace QoSIPSCprApplyRequest
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityApplyChanges, SCprApplyReq, TCFScpr::TApplyRequest, MeshMachine::CNodeRetryActivity::NewL)
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	FIRST_NODEACTIVITY_ENTRY(PRStates::TAwaitingApplyRequest, QoSIpSCprStates::TNoTagOrSendApplyResponseBlockedByOpenInternalSocketAndAddressUpdate)
#else
	FIRST_NODEACTIVITY_ENTRY(SCprStates::TAwaitingApplyRequest, QoSIpSCprStates::TNoTagOrSendApplyResponseBlockedByOpenInternalSocketAndAddressUpdate)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	NODEACTIVITY_ENTRY(SCprStates::KClientsJoining, QoSIpSCprStates::TAddClientToQoSChannel,    QoSIpSCprStates::TAwaitingJoinComplete,  MeshMachine::TNoTag)
	NODEACTIVITY_ENTRY(SCprStates::KClientsLeaving, QoSIpSCprStates::TRemoveClientToQoSChannel, QoSIpSCprStates::TAwaitingLeaveComplete, MeshMachine::TNoTag)
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	LAST_NODEACTIVITY_ENTRY(KNoTag, PRStates::TSendApplyResponse)
#else
	LAST_NODEACTIVITY_ENTRY(KNoTag, SCprStates::TSendApplyResponse)
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
NODEACTIVITY_END()
}

namespace QoSIPSCprAddressUpdate
{
DECLARE_DEFINE_CUSTOM_NODEACTIVITY(IPDeftSCprBaseActivities::ECFActivityAddressUpdate, QoSIPSCprAddressUpdate, TCFIPMessages::TDataClientRouted, MeshMachine::CNodeRetryActivity::NewL)
    FIRST_NODEACTIVITY_ENTRY(IPBaseSCprStates::TAwaitingAddressUpdate, QoSIpSCprStates::TNoTagBlockedByOpenInternalSocket)
    LAST_NODEACTIVITY_ENTRY(KNoTag, QoSIpSCprStates::TStoreAddressUpdateAndAddClientToQoSChannel)
NODEACTIVITY_END()
}

namespace QoSIPSCprDestroy
{
//This destroy activity is special, it waits for the TBindToRequest to complete should one be started from the
//CIpSubConnectionProvider::ReceivedL()
//Problem is as follows:
//When the requesting ControlClient fails to join this SCpr and responds with TBindToComplete(error), the Cpr
//sends TDestroy to this SCpr. This SCpr will destruct itself not waiting for the TBindToRequest to complete
//which results in (for example) the TDataClientJoined hitting void.
//This is not a proper fix, same as sending TBindToRequest from CIpSubConnectionProvider::ReceivedL() is a hack.
//There should be EAP diagrams present, where are they?
typedef MeshMachine::TActivitiesIdMutex<ESock::ECFActivityStop, ESock::ECFActivityStopDataClient, ESock::ECFActivityDataClientGoneDown, ESock::ECFActivityBindTo> TQosIpSCprActivityMutex;
DECLARE_SERIALIZABLE_STATE(
	TNoTagBlockedByQosIpSCprActivities,
    TQosIpSCprActivityMutex,
    MeshMachine::TNoTag
    )

DECLARE_DEFINE_CUSTOM_NODEACTIVITY(ECFActivityDestroy, QoSIPSCprDestroy, TEChild::TDestroy, CoreActivities::CDestroyActivity::New)
	FIRST_NODEACTIVITY_ENTRY(MeshMachine::TAwaitingDestroy, TNoTagBlockedByQosIpSCprActivities)

    //Stop self first
    NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TStopSelf, CoreNetStates::TAwaitingDataClientStopped, CoreStates::TNoTagOrNoClients)

    //The node mustn't go out of scope with clients present. The node must get rid of them first.
	NODEACTIVITY_ENTRY(KNoTag, CoreActivities::CDestroyActivity::TMakeClientsLeaveOrProcessClientLeave, CoreStates::TAwaitingClientLeave, CoreActivities::CDestroyActivity::TNoTagOrNoTagBackwards)
	THROUGH_NODEACTIVITY_ENTRY(KNoTag, CoreActivities::CDestroyActivity::TProcessClientLeave, TTag<CoreNetStates::KNoClients>)

 	THROUGH_NODEACTIVITY_ENTRY(CoreNetStates::KNoClients, PRStates::TProcessDestroy, MeshMachine::TNoTag)
 	NODEACTIVITY_ENTRY(KNoTag, MeshMachine::TDoNothing, MeshMachine::TAwaitingLeaveComplete, CoreActivities::CDestroyActivity::TNoTagOrNoTagBackwards)
 	LAST_NODEACTIVITY_ENTRY(KNoTag, CoreNetStates::TSendClientLeavingAndRemoveControlProvider)
NODEACTIVITY_END()
}

namespace QoSIPSCprActivities
{
DECLARE_DEFINE_ACTIVITY_MAP(activityMap)
	ACTIVITY_MAP_ENTRY(QoSIPSCprJoinActivity, QoSIpSCprJoin)
	ACTIVITY_MAP_ENTRY(QoSIPSCprClientLeaveActivity, QoSIpSCprLeave)
	ACTIVITY_MAP_ENTRY(QoSIPSCprSetParameters, qosIpScprParamRequest)
    ACTIVITY_MAP_ENTRY(QoSIPSCprApplyRequest, SCprApplyReq)
    ACTIVITY_MAP_ENTRY(QoSIPSCprAddressUpdate, QoSIPSCprAddressUpdate)
    ACTIVITY_MAP_ENTRY(QoSIPSCprDestroy, QoSIPSCprDestroy)
    ACTIVITY_MAP_ENTRY(QoSIPSCprOpenInternalSocket, qosIpScprOpenInternalSocket)
ACTIVITY_MAP_END_BASE(IPDeftBaseSCprActivities, ipdeftbasescprActivityMap)
}


//-=========================================================
//
// CIpSubConnectionProvider methods
//
//-=========================================================
CIpSubConnectionProvider* CIpSubConnectionProvider::NewL(CIpDefaultSubConnectionProviderFactory& aFactory)
/**
Construct a new IP SubConnection Provider Object

@param aFactory factory that create this object
@param aConnProvider Connection Provider associated with this object
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CQoSIpSubConnectionProvider::NewL")));
	CIpSubConnectionProvider* self = new (ELeave) CIpSubConnectionProvider(aFactory);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
	return self;
	}


CIpSubConnectionProvider::~CIpSubConnectionProvider()
    {
	__IPCPRLOG(IpCprLog::Printf(_L("CQoSIpSubConnectionProvider::~CQoSIpSubConnectionProvider [%08x]"), this));
    LOG_NODE_DESTROY(KIPSCprTag, CIpSubConnectionProvider);
	if( iChannelId >= 0 ) // Only send a Close if have opened a channel
		{
		TRAPD(ret,SendCloseL());
		if( ret != KErrNone )
			{
			__IPCPRLOG(IpCprLog::Printf(_L("SendCloseL left with error: %d"), ret));
			}
		}

	if( iWriter )
		{
		iWriter->Cancel();
		delete iWriter;
		iWriter = NULL;
		}

	if( iReader )
		{
		iReader->Cancel();
		delete iReader;
		iReader = NULL;
		}

	if (iAsyncWriter)
		{
		iAsyncWriter->Cancel();
		delete iAsyncWriter;
		iAsyncWriter = NULL;
		}

	delete iPrtParameters;
	ResetPrtExtensions();

	iSocket.Close();
    }


CIpSubConnectionProvider::CIpSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory)
:CIpSubConnectionProviderBase(aFactory, QoSIPSCprActivities::activityMap::Self()),
 iSocketError(KErrNotReady),
 iWriter(NULL), iReader(NULL),
 iChannelId(-1),
 iPrtExtensions(_FOFF(CExtensionBase,iLink)),
 iParametersSet(EFalse), iAsyncWriter(NULL)
    {
	__IPCPRLOG(IpCprLog::Printf(_L("CQoSIpSubConnectionProvider::CQoSIpSubConnectionProvider [%08x]"), this));
    LOG_NODE_CREATE(KIPSCprTag, CIpSubConnectionProvider);
    }



void CIpSubConnectionProvider::ConstructL()
/**
IP SubConnection Provider Second Phase Constructor
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CQoSIpSubConnectionProvider::Construct [%08x]"), this));
	__IPCPRLOG(IpCprLog::Printf(_L("CIpSubConnectionProvider::Construct [%08x]"), this));

    CIpSubConnectionProviderBase::ConstructL();

    iUid = RProcess().Type();

	/** Create the CQoSParameters to hold the Qos
	* and Extension Parameters. The values will initially
	be set to default
	*/
	iPrtParameters = new (ELeave) CQoSParameters;
	// Set Default Uplink Parameters
	iPrtParameters->SetUpLinkMaximumBurstSize(3000);  // for TokenBucketSizeUplink
	iPrtParameters->SetUpLinkMaximumPacketSize(1500);	// for MaxPacketSizeUplink
	iPrtParameters->SetUplinkBandwidth(1500);	// for TokenRateUplink
	iPrtParameters->SetUpLinkAveragePacketSize(1500); // for MinimumPolicedUnitUplink
	iPrtParameters->SetUpLinkPriority(KQoSLowestPriority); // for PriorityUplink
	iPrtParameters->SetUpLinkDelay(0); // for DelayUplink
	// Set Default Downlink parameters
	iPrtParameters->SetDownLinkMaximumBurstSize(3000); // for TokenBucketSizeDownlink
	iPrtParameters->SetDownLinkMaximumPacketSize(1500); // for MaxPacketSizeDownlink
	iPrtParameters->SetDownlinkBandwidth(1500); // for TokenRateDownlink
	iPrtParameters->SetDownLinkAveragePacketSize(1500); // for MinimumPolicedUnitDownlink
	iPrtParameters->SetDownLinkPriority(KQoSLowestPriority); // for PriorityDownlonk
	iPrtParameters->SetDownLinkDelay(0); // for DelayDownlink

	iPrtParameters->SetAdaptMode(EFalse);
//	iPrtParameters->SetHeaderMode(???);
//	User::LeaveIfError(iPrtParameters->SetName(name));

	// kick off creation of internal socket
	TNodeCtxId originator(MeshMachine::KActivityNull, Id());
	Messages::RClientInterface::OpenPostMessageClose(originator, Id(), TQoSIpSCprMessages::TOpenInternalSocket().CRef());
	}

RInternalSocket& CIpSubConnectionProvider::InternalSocket()
	{
	return iSocket;
	}

void CIpSubConnectionProvider::InternalSocketOpened(TInt aErr)
	{
	iSocketError = aErr;
	if (iSocketError == KErrNone)
		{
		iReader = CQoSMsgReader::NewL(this,	 iSocket);
		iWriter = CQoSMsgWriter::NewL(this, iSocket);
		iAsyncWriter = CAsyncWriter::NewL(iWriter);
		}
	}

RNodeInterface* CIpSubConnectionProvider::NewClientInterfaceL(const TClientType& aClientType, TAny* /*aClientInfo*/)
    {
	__IPCPRLOG(IpCprLog::Printf(_L("CQoSIpSubConnectionProvider::NewClientInterfaceL [%08x]"), this));
    if (aClientType.Type() & TCFClientType::EData)
        {
        return new (ELeave) RIPDataClientNodeInterface();
        }
    return CCoreSubConnectionProvider::NewClientInterfaceL(aClientType);
    }


void CIpSubConnectionProvider::Received(TNodeContextBase& aContext)
    {
	__IPCPRLOG(IpCprLog::Printf(_L("CQoSIpSubConnectionProvider::ReceivedL [%08x]"), this));
    Messages::TNodeSignal::TMessageId noPeerIds[] = {
        TCFFactory::TPeerFoundOrCreated::Id(),
        TCFPeer::TJoinRequest::Id(),
        Messages::TNodeSignal::TMessageId()
        };
    MeshMachine::AMMNodeBase::Received(noPeerIds, aContext);
	MeshMachine::AMMNodeBase::PostReceived(aContext);
	}

void CIpSubConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
	__IPCPRLOG(IpCprLog::Printf(_L("CQoSIpSubConnectionProvider::ReceivedL [%08x]"), this));
	ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aMessage, aRecipient);

	if (aMessage.IsMessage<TCFDataClient::TProvisionConfig>())
    	{
        ASSERT(ServiceProvider() == NULL && ControlProvider() != NULL);

        TDefaultSCPRFactoryQuery deftScprQuery(ControlProvider()->RecipientId(), TSubConnOpen::EAttachToDefault);
        ESock::CSubConnectionProviderBase* deftSCPR = static_cast<ESock::CSubConnectionProviderBase*>
        		(static_cast<CSubConnectionProviderFactoryBase&>(Factory()).Find(deftScprQuery));
        ASSERT(deftSCPR && deftSCPR->ServiceProvider());

        RClientInterface::OpenPostMessageClose(Id(), Id(),
        	TCFDataClient::TBindTo(deftSCPR->ServiceProvider()->RecipientId()).CRef());
    	}

	QoSIpSCprStates::TContext ctx(*this, aMessage, aSender, aRecipient);
    CIpSubConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}

void CIpSubConnectionProvider::DataClientJoiningL(RIPDataClientNodeInterface& aDataClient)
/**
Function called by Connection Provider when a socket is to be added to a QoS Flow

@param aDataClient Data Client to add to the QoS Channel
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CQoSIpSubConnectionProvider::DataClientJoiningL [%08x]"), this));
	if( aDataClient.iCliSrcAddr.Family() == KAFUnspec )
		{
		__IPCPRLOG(IpCprLog::Printf(_L("Source Address not defined")));
		User::Leave( KErrNotReady );
		}

	if( aDataClient.iCliDstAddr.Family() == KAFUnspec )
		{
		__IPCPRLOG(IpCprLog::Printf(_L("Destination Address not defined")));
		User::Leave( KErrNotReady );
		}

	if( iChannelId >= 0 )
		{
		SendJoinL(aDataClient.iCliSrcAddr, aDataClient.iCliDstAddr, iIapId, aDataClient.iProtocolId);
		}
	else
		{
		SendCreateL(aDataClient.iCliSrcAddr, aDataClient.iCliDstAddr, iIapId, aDataClient.iProtocolId);
		}
	}

void CIpSubConnectionProvider::DataClientLeaving(RIPDataClientNodeInterface& aDataClient)
/**
Function called by Connection Provider when a socket is to be removed from a QoS Flow

@param aDataClient Data Client to remove from the QoS Channel
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CQoSIpSubConnectionProvider::DataClientLeaving [%08x]"), this));
	if( aDataClient.iCliSrcAddr.Family() == KAFUnspec )
		{
		__IPCPRLOG(IpCprLog::Printf(_L("Source Address not defined")));
		return;
		}

	if( aDataClient.iCliDstAddr.Family() == KAFUnspec )
		{
		__IPCPRLOG(IpCprLog::Printf(_L("Destination Address not defined")));
		return;
		}


	if( iChannelId >= 0 )
    	{
		TRAPD(ret, SendLeaveL(aDataClient.iCliSrcAddr, aDataClient.iCliDstAddr, iIapId, aDataClient.iProtocolId));
		if (ret != KErrNone)
			{
			__IPCPRLOG(IpCprLog::Printf(_L("SendLeaveL left with err=%d"), ret));
			}
		}
	else
		{
		ASSERT(EFalse);
		__IPCPRLOG(IpCprLog::Printf(_L("Attempted to leave on an unconnected channel")));
		}
	}


void CIpSubConnectionProvider::SetQoSParametersL()
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CIpSubConnectionProvider::SetParameters [%08x]"), this));

	iParameterRelease = KParameterRelInvalid;
	ConvertParametersFromESockL( iParameterBundle );
	iParametersSet = ETrue;

  	if(iChannelId >= 0)
		{
		SendSetQoSL();
		}
	}

void CIpSubConnectionProvider::SendCreateL(const TInetAddr &aSrcAddr, const TInetAddr &aDstAddr, TUint32 aIapId, TUint32 aProtocolId)
/**
Sends a Message to QoS PRT to create a QoS Channel

@param aSrcAddr Source Address
@param aDstAddr Destination Address
@param aIapId IAP Id
@param aProtocolId Protocol Id
*/
    {
	__IPCPRLOG(IpCprLog::Printf(_L("Sending PRT Msg: EPfqosCreateChannel")));

	if (iAsyncWriter == NULL)
		User::Leave(iSocketError);
	
	// coverity[SYMBIAN.CLEANUP_STACK] coverity [alloc_fn]
    // coverity[SYMBIAN.CLEANUP_STACK] coverity [assign]

    CQoSMsg* msg = CQoSMsg::NewL(EPfqosCreateChannel);
    msg->AddConnInfo(aProtocolId, iUid, aIapId);
    msg->AddSrcAddr(aSrcAddr);
    msg->AddDstAddr(aDstAddr);
    msg->AddChannel(0);

    TQoSParameters qosParams;
    // coverity[SYMBIAN.CLEANUP_STACK] coverity [leave_without_push]
    ConvertCQoSIntoTQoSParamsL(qosParams);
	msg->AddQoSParameters(qosParams);
    msg->AddExtensionPolicy(iPrtExtensions);
	//Now we need to write to the qos.prt asynchronously. This becuase the TCP/IP stack in CIp6Flow::Connect() calls Bearer() before RefreshFlow()
    iAsyncWriter->Send(msg);
    }


void CIpSubConnectionProvider::SendCloseL()
/**
Sends a Message to QoS PRT to close a QoS Channel
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("Sending PRT Msg: EPfqosDeleteChannel")));

	if (iWriter == NULL)
		User::Leave(iSocketError);
	

    CQoSMsg* msg = CQoSMsg::NewL(EPfqosDeleteChannel);
    msg->AddChannel(iChannelId);

    iWriter->Send(msg);
	}


void CIpSubConnectionProvider::SendJoinL(const TInetAddr &aSrcAddr, const TInetAddr &aDstAddr, TUint32 aIapId, TUint32 aProtocolId)
/**
Sends a Message to QoS PRT to add a socket to a QoS Channel

@param aSrcAddr Source Address
@param aDstAddr Destination Address
@param aIapId IAP Id
@param aProtocolId Protocol Id
*/
    {
	__IPCPRLOG(IpCprLog::Printf(_L("Sending PRT Msg: EPfqosJoin")));

	if (iWriter == NULL)
		User::Leave(iSocketError);

    CQoSMsg* msg = CQoSMsg::NewL(EPfqosJoin);
    msg->AddConnInfo(aProtocolId, iUid, aIapId);
    msg->AddSrcAddr(aSrcAddr);
    msg->AddDstAddr(aDstAddr);
    msg->AddChannel(iChannelId);

    iWriter->Send(msg);
    }

void CIpSubConnectionProvider::SendLeaveL(const TInetAddr &aSrcAddr, const TInetAddr &aDstAddr, TUint32 aIapId, TUint32 aProtocolId)
/**
Sends a Message to QoS PRT to remove a socket from a QoS Channel

@param aSrcAddr Source Address
@param aDstAddr Destination Address
@param aIapId IAP Id
@param aProtocolId Protocol Id
*/
    {
	__IPCPRLOG(IpCprLog::Printf(_L("Sending PRT Msg: EPfqosLeave")));

	if (iWriter == NULL)
		User::Leave(iSocketError);

    CQoSMsg* msg = CQoSMsg::NewL(EPfqosLeave);
    msg->AddConnInfo(aProtocolId, iUid, aIapId);
    msg->AddSrcAddr(aSrcAddr);
    msg->AddDstAddr(aDstAddr);
    msg->AddChannel(iChannelId);

    iWriter->Send(msg);
    }

void CIpSubConnectionProvider::SendSetQoSL()
/**
Sends Message to QoS PRT to update the parameters for a QoS Channel
*/
    {
	__IPCPRLOG(IpCprLog::Printf(_L("Sending PRT Msg: EPfqosConfigChannel")));

	if (iWriter == NULL)
		User::Leave(iSocketError);
    // coverity[SYMBIAN.CLEANUP_STACK] coverity [alloc_fn]
    // coverity[SYMBIAN.CLEANUP_STACK] coverity [assign]
    CQoSMsg* msg = CQoSMsg::NewL(EPfqosConfigChannel);
    msg->AddChannel(iChannelId);

    TQoSParameters qosParams;
    // coverity[SYMBIAN.CLEANUP_STACK] coverity [leave_without_push]
    ConvertCQoSIntoTQoSParamsL(qosParams);
    msg->AddQoSParameters(qosParams);
    msg->AddExtensionPolicy(iPrtExtensions);

    iWriter->Send(msg);
    }



void CIpSubConnectionProvider::ProcessPRTMsg(TPfqosMessage& aMsg)
/**
Process Messages sent from the PRT to the SubConnection Provider
Messages are either replies or events

@param aMsg the message from the PRT
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CIpSubConnectionProvider::ProcessPRTMsg [%08x]"), this));

    if( aMsg.iBase.iMsg == NULL )
    	{
		__IPCPRLOG(IpCprLog::Printf(_L("Received malformed message from PRT")));
    	}
    else
    	{
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Msg: %d"), aMsg.iBase.iMsg->pfqos_msg_type));
	    switch(aMsg.iBase.iMsg->pfqos_msg_type)
			{
		case EPfqosEvent:
			{
			TRAPD(ret, ProcessPRTEventL(aMsg) );
			if( ret != KErrNone )
				{
				__IPCPRLOG(IpCprLog::Printf(_L("ProcessPRTEventL left with error: %d"), ret));
				}
			}
			break;

		case EPfqosUpdate:
		case EPfqosDelete:
		case EPfqosAdd:
		case EPfqosGet:
		case EPfqosReject:
		case EPfqosDump:
		case EPfqosConfigure:
		case EPfqosJoin:
		case EPfqosLeave:
		case EPfqosCreateChannel:
		case EPfqosOpenExistingChannel:
		case EPfqosDeleteChannel:
		case EPfqosConfigChannel:
		case EPfqosLoadFile:
		case EPfqosUnloadFile:
			{
			TRAPD(ret, ProcessPRTReplyL(aMsg) );
			if( ret != KErrNone )
				{
				__IPCPRLOG(IpCprLog::Printf(_L("ProcessPRTReplyL left with error: %d"), ret));
				}
			}
		    break;

		default:
			__IPCPRLOG(IpCprLog::Printf(_L("Received Unknown PRT Msg: %d"), aMsg.iBase.iMsg->pfqos_msg_type));
		    break;
			}
    	}
	}


void CIpSubConnectionProvider::ProcessPRTEventL(TPfqosMessage& aMsg)
/**
Process Events sent from the PRT to the SubConnection Provider

@param aMsg the message from the PRT
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CIpSubConnectionProvider::ProcessPRTEventL [%08x]"), this));

	// Only interested in Channel Events
    if (aMsg.iEvent.iExt == NULL || aMsg.iFlowSpec.iExt == NULL || aMsg.iChannel.iExt == NULL)
    	{
		__IPCPRLOG(IpCprLog::Printf(_L("Received malformed event message from PRT")));
    	return;
    	}

	RIPDataClientNodeInterface* client = DetermineClient(aMsg);
	switch (aMsg.iEvent.iExt->event_type)
		{
    	case KPfqosEventFailure:
		    {
/*
		EQoSOk,
    EQoSPolicyExists = -5119,       //< -5119 Policy exists in database
    EQoSNoModules,					//< -5118 No QoS modules available
    EQoSInterface,			        //< -5117 Flows are using different interfaces
	EQoSModules,					//< -5116 Flows use different QoS modules
	EQoSModuleLoadFailed,			//< -5115 Loading of QoS module failed
	EQoSMessageCorrupt,				//< -5114 Pfqos message corrupted
	EQoSJoinFailure,				//< -5113 Join to QoS channel failed
	EQoSLeaveFailure,				//< -5112 Leave from QoS channel failed
	EQoSNoInterface,				//< -5111 Network interface deleted
	EQoSChannelDeleted,				//< -5110 QoS channel deleted
	EQoSDowngradeForced				//< -5109 QoS parameters downgraded by administrative policy
*/


#if defined(QOS_ERROR_REPORTING) //Awaiting code delivery for qos and guqos to support this functionality
#pramga message ("QOS_ERROR_REPORTING hasn't been found switched on yet - any point in converting this code?")
ASSERT(EFalse);
/*
		switch(aMsg.iBase.iMsg->pfqos_msg_errno)
			{
				case EQoSJoinFailure:

					if( client )
						{
						__IPCPRLOG(IpCprLog::Printf(_L("Join failed Event")));
						client->JoinFailed(*this,aMsg.iBase.iMsg->pfqos_msg_errno);
						}
					break;

				case EQoSLeaveFailure:
					if( client )
						{
						__IPCPRLOG(IpCprLog::Printf(_L("Leave failed Event")));
						//We don't particularly care that it failed - just pretend that it was successful
						client->LeaveComplete(*this);
						}
					break;
				case EQoSNoInterface:
				case EQoSChannelDeleted:
					{
					CSubConGenEventSubConDown* scde = CSubConGenEventSubConDown::NewL();
					scde->SetError(aMsg.iBase.iMsg->pfqos_msg_errno);
					event = scde;
					iChannelId = -1;
					break;
					}
				case EQoSDowngradeForced:
					{
					event = CSubConGenEventParamsChanged::NewL();
					break;
					}

				case EQoSChannelFailed:
					if( client )
						{
						__IPCPRLOG(IpCprLog::Printf(_L("Join failed Event")));
						client->JoinFailed(*this,aMsg.iBase.iMsg->pfqos_msg_errno);
						}
					// no break here we want to continue into the next case statement;
				case EQoSParamsRejected:
					{
					CSubConGenEventParamsRejected* scde = CSubConGenEventParamsRejected::NewL();
					scde->SetError(aMsg.iBase.iMsg->pfqos_msg_errno);
					scde->SetFamilyId(KSubConQoSFamily);
					event = scde;
					break;
					}

				default:
					__IPCPRLOG(IpCprLog::Printf(_L("Unknown event sent ")));
			};
*/
#else
		switch(aMsg.iBase.iMsg->pfqos_msg_errno)
			{
			case EQoSChannelDeleted:
				iChannelId = -1;
				break;
			case EQoSNoInterface:
				iChannelId = -1;
      			/*phoney activity id, so nobody accidentally picks up the response*/
      			TCFDataClient::TStop dcStop(KErrAbort);
      			TInt errValue = (TInt)EQoSNoInterface;
      			ReceivedL(TNodeId(Id()), TNodeCtxId(errValue, Id()), dcStop);
                return;
			}

			if( client )
				{
				__IPCPRLOG(IpCprLog::Printf(_L("Join failed Event")));
                ProcessPRTError( aMsg, aMsg.iBase.iMsg->pfqos_msg_errno );
				}
#endif

			CSubConGenEventParamsRejected* scde = CSubConGenEventParamsRejected::NewL();
			/**
			There can be two set of parameters that are sent to QOS either as KSubConQoSFamily
			or as KSubConAuthorisationFamily. Here we should differentiate, the QOS is rejected
			because of which parameter family.
			At present there is no indication comes from the lower layer why the QOS has failed, and in
			all the case the member *aMsg.iBase.iMsg->pfqos_msg_type* will return *EPfQoSReject*.

			At the time of writing this code any differentiation method was not available to differentiate
			between events. i.e whether the event is SBLP or UMTS events. So this has not been done.
			This needs to be done when the TPfqosMessage will have the differentiation
			*/
			scde->SetError(aMsg.iBase.iMsg->pfqos_msg_errno);
			scde->SetFamilyId(KSubConQoSFamily);
			NotifyClientsL(*scde);
		    }
	    break;

	case KPfqosEventConfirm:
		{
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Event: KPfqosEventConfirm")));

		// Setting QoS Parameters OK
		CSubConGenEventParamsGranted* event = CSubConGenEventParamsGranted::NewL();
		ConvertParametersFromQOSL(aMsg, event);

		NotifyClientsL(*event);
  		if( client != NULL && client->iActivityAwaitingResponse != KActivityNull)
  			{
  			TCFPeer::TJoinComplete joinComplete;
  			ReceivedL(TNodeId(Id()), TNodeCtxId(client->iActivityAwaitingResponse, Id()), joinComplete);
            client->iActivityAwaitingResponse = KActivityNull;
  			}
  		}
	    break;

	case KPfqosEventAdapt:
		{
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Event: KPfqosEventAdapt")));

		// Available QoS Changed
		CSubConGenEventParamsChanged* event = CSubConGenEventParamsChanged::NewL();
		event->SetError(aMsg.iBase.iMsg->pfqos_msg_errno);
		ConvertParametersFromQOSL(aMsg, event);

		NotifyClientsL(*event);
		}
	    break;

	case KPfqosEventJoin:
		{
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Event: KPfqosEventJoin")));
		if (aMsg.iBase.iMsg->pfqos_msg_errno)
		   {
		   ProcessPRTError( aMsg, aMsg.iBase.iMsg->pfqos_msg_errno );
		   return;
		   }


		CSubConGenEventDataClientJoined* event = CSubConGenEventDataClientJoined::NewL();

		if( client )
			{
			event->SetSourceAddress( client->iCliSrcAddr );
			event->SetDestAddress( client->iCliDstAddr );
			event->SetIap( iIapId );

            if (client->iActivityAwaitingResponse != KActivityNull)
                {
                TCFPeer::TJoinComplete joinComplete;
      			ReceivedL(TNodeId(Id()), TNodeCtxId(client->iActivityAwaitingResponse, Id()), joinComplete);
                client->iActivityAwaitingResponse = KActivityNull;
                }
			}

		NotifyClientsL(*event);
		}
	    break;

	case KPfqosEventLeave:
		{
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Event: KPfqosEventLeave")));

		CSubConGenEventDataClientLeft* event = CSubConGenEventDataClientLeft::NewL();

		if( client )
			{
			event->SetSourceAddress( client->iCliSrcAddr );
			event->SetDestAddress( client->iCliDstAddr );
			event->SetIap( iIapId );

			if ( client->iActivityAwaitingResponse != KActivityNull)
    			{
    			TEPeer::TLeaveComplete leaveComplete;
    			ReceivedL(TNodeId(Id()), TNodeCtxId(client->iActivityAwaitingResponse, Id()), leaveComplete);
    			client = NULL;
    			}
    	    /*else
        	    {
        	    client->PostMessage(TCFMessage::TRejoin(defaultSubCon).CRef());
        	    }
    	    */
			}
		NotifyClientsL(*event);

		}
	    break;

	default:
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Event: Unknown - %d"), aMsg.iEvent.iExt->event_type));
	    break;
		}
	}

void CIpSubConnectionProvider::ProcessPRTReplyL(TPfqosMessage& aMsg)
/**
Process Replies sent from the PRT to the SubConnection Provider

@param aMsg the message from the PRT
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CIpSubConnectionProvider::ProcessPRTReplyL [%08x]"), this));

	if( aMsg.iChannel.iExt == NULL )
		{
		__IPCPRLOG(IpCprLog::Printf(_L("Received malformed reply message from PRT")));
		}
	else if( aMsg.iBase.iMsg->pfqos_msg_errno != KErrNone )
    	{
    	ProcessPRTError( aMsg, aMsg.iBase.iMsg->pfqos_msg_errno );
    	}
	else
    	{
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Reply: %d"), aMsg.iBase.iMsg->pfqos_msg_type));

	    RIPDataClientNodeInterface* client = DetermineClient(aMsg);
		if( client == NULL )
			{
			__IPCPRLOG(IpCprLog::Printf(_L("Could not determine DataClient from message.")));
			}

		switch (aMsg.iBase.iMsg->pfqos_msg_type)
			{
		case EPfqosOpenExistingChannel:
			iChannelId = aMsg.iChannel.iExt->channel_id;
			__IPCPRLOG(IpCprLog::Printf(_L("Processing Reply for message: EPfqosOpenExistingChannel")));
			break;

		case EPfqosCreateChannel:
			iChannelId = aMsg.iChannel.iExt->channel_id;
			__IPCPRLOG(IpCprLog::Printf(_L("Processing Reply for message: EPfqosCreateChannel")));
			//Now you will expect to complete the join here. This will not work becuase
			//qos.prt send a reply before finishing the join down to the nif level
			//we will have to delay the response until the event is received
			break;

		case EPfqosJoin:
			__IPCPRLOG(IpCprLog::Printf(_L("Processing Reply for message: EPfqosJoin")));
			//Now you will expect to complete the join here. This will not work becuase
			//qos.prt send a reply before finishing the join down to the nif level
			//we will have to delay the response until the event is received
			break;

		case EPfqosLeave:
			__IPCPRLOG(IpCprLog::Printf(_L("Processing Reply for message: EPfqosLeave")));
			break;

		case EPfqosConfigChannel:
			{
			__IPCPRLOG(IpCprLog::Printf(_L("Processing Reply for message: EPfqosConfigChannel")));
			}
			break;

		case EPfqosDeleteChannel:
			iChannelId = -1;
			__IPCPRLOG(IpCprLog::Printf(_L("Processing Reply for message: EPfqosDeleteChannel")));
			break;

		default:
			__IPCPRLOG(IpCprLog::Printf(_L("Ignoring Reply for unknown message: %d"), aMsg.iBase.iMsg->pfqos_msg_type));
			break;
			}
    	}

	__IPCPRLOG(IpCprLog::Printf(_L("Provider=0x%x Channel=%d"), this, iChannelId));
	}


#ifdef _DEBUG
void CIpSubConnectionProvider::ProcessPRTError(TInt aMsgType, TInt __IPCPRLOG(aError))
/**
Process Errors that occur in communicating between the PRT and the
SubConnection Provider

@param aMsgType the type of message that encountered the error
@param aError the error ththas occurred
*/
	{
	switch (aMsgType)
		{
	case EPfqosOpenExistingChannel:
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Error %d on EPfqosOpenExistingChannel"), aError));
		break;

	case EPfqosCreateChannel:
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Error %d on EPfqosCreateChannel"), aError));
		break;

	case EPfqosDeleteChannel:
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Error %d on EPfqosDeleteChannel"), aError));
		break;

	case EPfqosJoin:
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Error %d on EPfqosJoin"), aError));
		break;

	case EPfqosLeave:
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Error %d on EPfqosLeave"), aError));
		break;

	case EPfqosConfigChannel:
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Error %d on EPfqosConfigChannel"), aError));
		break;

   case  EPfqosEvent:
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Error %d on EPfqosEvent"), aError));
		break;

	default:
		__IPCPRLOG(IpCprLog::Printf(_L("Received PRT Error %d on Unknown Message"), aError));
		break;
		}
	}
#endif


void CIpSubConnectionProvider::ProcessPRTError(TPfqosMessage& aMsg, TInt aError)
/**
Process Errors that occur in communicating between the PRT and the
SubConnection Provider

@param aMsg the message from the PRT
@param aError the error ththas occurred
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CIpSubConnectionProvider::ProcessPRTError [%08x]"), this));

	if( aError != KErrNone)
		{
		TInt msgType = aMsg.iBase.iMsg->pfqos_msg_type;

#ifdef _DEBUG
		ProcessPRTError(msgType, aError);
#endif

    	RIPDataClientNodeInterface* client = DetermineClient(aMsg);
		if( client == NULL )
			{
			__IPCPRLOG(IpCprLog::Printf(_L("Could not determine DataClient from message.")));
			}

		if( msgType == EPfqosJoin ||
		    msgType == EPfqosOpenExistingChannel ||
		    msgType == EPfqosCreateChannel ||
		    (msgType == EPfqosEvent && aMsg.iBase.iMsg->pfqos_msg_errno == EQoSJoinFailure))
			{
			if( client && client->iActivityAwaitingResponse != KActivityNull)
				{
				TEBase::TError error(TCFPeer::TJoinRequest::Id(), aError);
                ReceivedL(TNodeId(Id()), TNodeCtxId(client->iActivityAwaitingResponse, Id()), error);
                client->iActivityAwaitingResponse = KActivityNull;
				}
			}
		else if( msgType == EPfqosConfigChannel )
			{
 			//This will send an error only if ECom successfuly constructs the event object
 			CSubConGenEventParamsRejected* event = NULL;
 			TRAP_IGNORE(event = CSubConGenEventParamsRejected::NewL());
 			if (event)
 				{
 				/**
 				comments give in case of *KPfqosEventFailure* in function *ProcessPRTEventL*
 				will also applicable here
 				*/
 				if (aMsg.iBase.iMsg->pfqos_msg_errno == RPacketContext::EEtelPcktPolicyControlRejectionCode)
 					{
 					event->SetFamilyId(KSubConAuthorisationFamily);
 					}
 				else
 					{
 					event->SetFamilyId(KSubConQoSFamily);
 					}

 				event->SetError(aError);

 				NotifyClientsL(*event);
 				}
   			}
		else if( msgType == EPfqosLeave ||
		   (msgType == EPfqosEvent && aMsg.iBase.iMsg->pfqos_msg_errno == EQoSLeaveFailure))
			{
			// Not Interested in whether the leave was successful.  Inform client regardless
			if( client != NULL  && client->iActivityAwaitingResponse != KActivityNull)
				{
				TEBase::TError error(TEPeer::TLeaveRequest::Id(), aError);
				ReceivedL(TNodeId(Id()), TNodeCtxId(client->iActivityAwaitingResponse, Id()), error);
                client->iActivityAwaitingResponse = KActivityNull;
				}
			}
		else
    		{
			if( client != NULL  && client->iActivityAwaitingResponse != KActivityNull)
				{
				TEBase::TError error(TEPeer::TLeaveRequest::Id(), aError);
				ReceivedL(TNodeId(Id()), TNodeCtxId(client->iActivityAwaitingResponse, Id()), error);
                client->iActivityAwaitingResponse = KActivityNull;
				}
    		}
		}
	}



RIPDataClientNodeInterface* CIpSubConnectionProvider::DetermineClient(const TPfqosMessage& aMsg)
/**
Determines which Data Client a message from the QoS PRT is for based upon source and destination
address; and Protocol Id.

@param aMsg QoS PRT Response Message
@param aDataClient Output Variable as Data Client that matches reponse.

@return KErrNone on finding the required client.  Client passed back as argument.
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CIpSubConnectionProvider::DetermineClient [%08x]"), this));

	RIPDataClientNodeInterface* dataClient = NULL;

	const TInetAddr* msgSrcAddr = aMsg.iSrcAddr.iAddr;
	const TInetAddr* msgDstAddr = aMsg.iDstAddr.iAddr;

	if( msgSrcAddr == NULL || msgDstAddr == NULL || aMsg.iSelector.iExt == NULL )
		{
		__IPCPRLOG(IpCprLog::Printf(_L("Received malformed message from PRT")););
    	}
    else
		{
		TUint32 msgProtocol = aMsg.iSelector.iExt->protocol;

		__IPCPRLOG(
			THostName srcName;
			msgSrcAddr->OutputWithScope(srcName);
			THostName dstName;
			msgDstAddr->OutputWithScope(dstName);
		    IpCprLog::Printf(_L("Msg SAddr[%S][%d] DAddr[%S][%d] Prot[%d]"), &srcName, msgSrcAddr->Port(), &dstName, msgDstAddr->Port(), msgProtocol);
	    );

    	RIPDataClientNodeInterface* client = NULL;
    	TClientIter<TDefaultClientMatchPolicy> iter = GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::EData));
    	while (NULL != (client = static_cast<RIPDataClientNodeInterface*>(iter++)))
			{
			if( client->iCliSrcAddr.Family() != KAFUnspec && client->iCliDstAddr.Family() != KAFUnspec /*&& connInfo != NULL*/ )
				{
				TUint32 cliProtocol = client->iProtocolId;

				TInetAddr srcInetAddr(client->iCliSrcAddr);
				TInetAddr dstInetAddr(client->iCliDstAddr);

				if (dstInetAddr.Family() == KAfInet)
					{
					dstInetAddr.ConvertToV4Mapped();
					}

				__IPCPRLOG(
					srcInetAddr.OutputWithScope(srcName);
					dstInetAddr.OutputWithScope(dstName);
		    		IpCprLog::Printf(_L("Cli SAddr[%S][%d] DAddr[%S][%d] Prot[%d]"), &srcName, srcInetAddr.Port(), &dstName, dstInetAddr.Port(), cliProtocol);
		    	);

				if( (msgSrcAddr->Port() == srcInetAddr.Port() &&
				    (msgDstAddr->CmpAddr(dstInetAddr) && msgDstAddr->Port() == dstInetAddr.Port())) &&
					msgProtocol == cliProtocol )
					{
					// Have our required client
					__IPCPRLOG(IpCprLog::Printf(_L("Client Matched [%08x]"), client));
					dataClient = client;
					break;
					}
				}
			}
    	}

	if( dataClient != NULL )
		{
		__IPCPRLOG(IpCprLog::Printf(_L("Client Found")));
		}
	else
		{
		__IPCPRLOG(IpCprLog::Printf(_L("Client Not Found!")));
		}

	return dataClient;
	}





//-=================================================================
//
//
//
// Parameter mapping methods
//
//
//
//-=================================================================
void CIpSubConnectionProvider::MapGenericParamsFromESockToPRTL(const CSubConQosGenericParamSet& generic) const
/**
Mapping function to map the generic parameters from the ESock to QoS PRT equivalent
@param Generic Input structure contains the generic parameters
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CIpSubConnectionProvider::MapGenericParamsFromESockToPRTL")));
	/**
	Get and Set Uplink Parameters
	*/
	iPrtParameters->SetUpLinkMaximumBurstSize(generic.GetUpLinkMaximumBurstSize());
	iPrtParameters->SetUpLinkMaximumPacketSize(generic.GetUpLinkMaximumPacketSize());
	iPrtParameters->SetUplinkBandwidth(generic.GetUplinkBandwidth());
	iPrtParameters->SetUpLinkAveragePacketSize(generic.GetUpLinkAveragePacketSize());
	iPrtParameters->SetUpLinkPriority(generic.GetUpLinkPriority());
	iPrtParameters->SetUpLinkDelay(generic.GetUpLinkDelay());
	/**
	Get and Set Downlink Parameters
	*/
	iPrtParameters->SetDownLinkMaximumBurstSize(generic.GetDownLinkMaximumBurstSize());
	iPrtParameters->SetDownLinkMaximumPacketSize(generic.GetDownLinkMaximumPacketSize());
	iPrtParameters->SetDownlinkBandwidth(generic.GetDownlinkBandwidth());
	iPrtParameters->SetDownLinkAveragePacketSize(generic.GetDownLinkAveragePacketSize());
	iPrtParameters->SetDownLinkPriority(generic.GetDownLinkPriority());
	iPrtParameters->SetDownLinkDelay(generic.GetDownLinkDelay());
	/**
	Get and Set rest of the Parameters
	*/
	iPrtParameters->SetHeaderMode(generic.GetHeaderMode());
	TName name = generic.GetName();
	iPrtParameters->SetName(name);
	}

void CIpSubConnectionProvider::MapGenericParamsFromPRTToESockL(CSubConQosGenericParamSet& generic) const
/**
Map generic parameters from QoS PRT to the ESock Equivalents

@param params input structure that contains the QoS PRT generic parameters
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CIpSubConnectionProvider::MapGenericParamsFromPRTToESockL")));
	/**
	Get and Set Uplink Parameters
	*/
	generic.SetUpLinkMaximumBurstSize(iPrtParameters->GetUpLinkMaximumBurstSize());
	generic.SetUpLinkMaximumPacketSize(iPrtParameters->GetUpLinkMaximumPacketSize());
	generic.SetUplinkBandwidth(iPrtParameters->GetUplinkBandwidth());
	generic.SetUpLinkAveragePacketSize(iPrtParameters->GetUpLinkAveragePacketSize());
	generic.SetUpLinkPriority(iPrtParameters->GetUpLinkPriority());
	generic.SetUpLinkDelay(iPrtParameters->GetUpLinkDelay());
	/**
	Get and Set Downlink Parameters
	*/
	generic.SetDownLinkMaximumBurstSize(iPrtParameters->GetDownLinkMaximumBurstSize());
	generic.SetDownLinkMaximumPacketSize(iPrtParameters->GetDownLinkMaximumPacketSize());
	generic.SetDownlinkBandwidth(iPrtParameters->GetDownlinkBandwidth());
	generic.SetDownLinkAveragePacketSize(iPrtParameters->GetDownLinkAveragePacketSize());
	generic.SetDownLinkPriority(iPrtParameters->GetDownLinkPriority());
	generic.SetDownLinkDelay(iPrtParameters->GetDownLinkDelay());
	/**
	Get and Set rest of the Parameters
	*/
	generic.SetHeaderMode(iPrtParameters->GetHeaderMode());
	TName name = iPrtParameters->GetName();
	generic.SetName(name);
	}

void CIpSubConnectionProvider::ConvertTQoSIntoCQoSParamsL(const TQoSParameters& aParameters)
/**
This function will copy the aParameters into the class CQoSParameters
using member access functions
@param aParameters input parameters that needs to be copied
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CIpSubConnectionProvider::ConvertTQoSIntoCQoSParamsL")));
	/**
	Get and Set Uplink Parameters
	*/
	iPrtParameters->SetUplinkBandwidth(aParameters.GetUplinkBandwidth());
	iPrtParameters->SetUpLinkMaximumBurstSize(aParameters.GetUpLinkMaximumBurstSize());
	iPrtParameters->SetUpLinkMaximumPacketSize(aParameters.GetUpLinkMaximumPacketSize());
	iPrtParameters->SetUpLinkAveragePacketSize(aParameters.GetUpLinkAveragePacketSize());
	iPrtParameters->SetUpLinkDelay(aParameters.GetUpLinkDelay());
	iPrtParameters->SetUpLinkPriority(aParameters.GetUpLinkPriority());
	/**
	Get and Set Downlink Parameters
	*/
	iPrtParameters->SetDownlinkBandwidth(aParameters.GetDownlinkBandwidth());
	iPrtParameters->SetDownLinkMaximumBurstSize(aParameters.GetDownLinkMaximumBurstSize());
	iPrtParameters->SetDownLinkMaximumPacketSize(aParameters.GetDownLinkMaximumPacketSize());
	iPrtParameters->SetDownLinkAveragePacketSize(aParameters.GetDownLinkAveragePacketSize());
	iPrtParameters->SetDownLinkDelay(aParameters.GetDownLinkDelay());
	iPrtParameters->SetDownLinkPriority(aParameters.GetDownLinkPriority());

	iPrtParameters->SetAdaptMode(aParameters.AdaptMode());
	iPrtParameters->SetHeaderMode(aParameters.GetHeaderMode());
	const TName name = aParameters.GetName();
	iPrtParameters->SetName(name);
	}

void CIpSubConnectionProvider::ConvertCQoSIntoTQoSParamsL(TQoSParameters& aParameters) const
/**
This function will copy the iParameters (CQoSParameters) into aParameters
using member access functions
@param aParameters output parameters that iParameters will be copied into
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CIpSubConnectionProvider::ConvertCQoSIntoTQoSParamsL")));
	/**
	Get and Set Uplink Parameters
	*/
	aParameters.SetUplinkBandwidth(iPrtParameters->GetUplinkBandwidth());
	aParameters.SetUpLinkMaximumBurstSize(iPrtParameters->GetUpLinkMaximumBurstSize());
	aParameters.SetUpLinkMaximumPacketSize(iPrtParameters->GetUpLinkMaximumPacketSize());
	aParameters.SetUpLinkAveragePacketSize(iPrtParameters->GetUpLinkAveragePacketSize());
	aParameters.SetUpLinkDelay(iPrtParameters->GetUpLinkDelay());
	aParameters.SetUpLinkPriority(iPrtParameters->GetUpLinkPriority());

	/**
	Get and Set Downlink Parameters
	*/
	aParameters.SetDownlinkBandwidth(iPrtParameters->GetDownlinkBandwidth());
	aParameters.SetDownLinkMaximumBurstSize(iPrtParameters->GetDownLinkMaximumBurstSize());
	aParameters.SetDownLinkMaximumPacketSize(iPrtParameters->GetDownLinkMaximumPacketSize());
	aParameters.SetDownLinkAveragePacketSize(iPrtParameters->GetDownLinkAveragePacketSize());
	aParameters.SetDownLinkDelay(iPrtParameters->GetDownLinkDelay());
	aParameters.SetDownLinkPriority(iPrtParameters->GetDownLinkPriority());

	aParameters.SetAdaptMode(iPrtParameters->AdaptMode());
	aParameters.SetHeaderMode(iPrtParameters->GetHeaderMode());
	const TName name = iPrtParameters->GetName();
	aParameters.SetName(name);
	}


void CIpSubConnectionProvider::MapExtensionParamsFromESockToPRTL(const CSubConQosIPLinkR99ParamSet& extension, TUmtsQoSParameters& params)
/**
Map extension parameters from QoS PRT to the ESock Equivalents

@param extension input structure that contains the ESock extension parameters
@param params structure updated to contains the equivalent QoS PRT extension parameters
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("MapExtensionParamsFromESockToPRTL")));

	RPacketQoS::TTrafficClass trafficClass = extension.GetTrafficClass();
	User::LeaveIfError(params.SetTrafficClass((TUmtsTrafficClass)trafficClass));

	RPacketQoS::TDeliveryOrder deliveryOrder = extension.GetDeliveryOrder();
	User::LeaveIfError(params.SetDeliveryOrder((TUmtsDeliveryOrder)deliveryOrder));

	RPacketQoS::TErroneousSDUDelivery sduDelivery = extension.GetErroneousSDUDelivery();
	User::LeaveIfError(params.SetDeliveryOfErroneusSdu((TUmtsErroneousSDUDelivery)sduDelivery));

	RPacketQoS::TBitErrorRatio bitErrorRate = extension.GetResidualBitErrorRatio();
	User::LeaveIfError(params.SetResidualBer((TUmtsBitErrorRatio)bitErrorRate));

	RPacketQoS::TSDUErrorRatio sduErrorRatio = extension.GetSDUErrorRatio();
	User::LeaveIfError(params.SetErrorRatio((TUmtsSDUErrorRatio)sduErrorRatio));

	RPacketQoS::TTrafficHandlingPriority trafficHandlingPriority = extension.GetTrafficHandlingPriority();
	User::LeaveIfError(params.SetPriority((TUmtsTrafficHandlingPriority)trafficHandlingPriority));

	TInt transferDelay = extension.GetTransferDelay();
	User::LeaveIfError(params.SetTransferDelay(transferDelay));

	TInt maxSduSize = extension.GetMaxSduSize();
	User::LeaveIfError(params.SetMaxSduSize(maxSduSize));

	TInt maxBitRateUp = extension.GetMaxBitrateUplink();
	User::LeaveIfError(params.SetMaxBitrateUplink(maxBitRateUp));

	TInt maxBitRateDown = extension.GetMaxBitrateDownlink();
	User::LeaveIfError(params.SetMaxBitrateDownlink(maxBitRateDown));

	TInt guaBitRateUp = extension.GetGuaBitrateUplink();
	User::LeaveIfError(params.SetGuaranteedBitrateUplink(guaBitRateUp));

	TInt guaBitRateDown = extension.GetGuaBitrateDownlink();
	User::LeaveIfError(params.SetGuaranteedBitrateDownlink(guaBitRateDown));

    iParameterRelease = KParameterRel4Rel99;
	}

void CIpSubConnectionProvider::MapExtensionParamsFromESockToPRTL(const CSubConSBLPR5ExtensionParamSet& extension, CSblpParameters& params) const
	{
	TAuthToken authToken = extension.GetMAT();
	params.SetMAT(authToken);

	/**
	Get Flow Identifires And store that into the RArray of Flow Ids
	of SBLP Paramters
	*/
	RArray<CSblpParameters::TFlowIdentifier> arrFlowIds;
	CleanupClosePushL(arrFlowIds);

	TInt nrOfFlows = extension.GetNumberOfFlowIds();
	for ( TInt i = 0; i < nrOfFlows; i ++ )
	{
	    const TFlowId & Fid = extension.GetFlowIdAt(i);
		CSblpParameters::TFlowIdentifier FlowId;
		FlowId.iMediaComponentNumber = Fid.GetMediaComponentNumber();
		FlowId.iIPFlowNumber = Fid.GetIPFlowNumber();
		arrFlowIds.AppendL(FlowId);
	}
	params.SetFlowIds(arrFlowIds);
 	CleanupStack::Pop(&arrFlowIds);
 	arrFlowIds.Close();
	}

void MapExtensionParamsFromPRTToESockL(TUmtsQoSParameters& params, CSubConQosIPLinkR99ParamSet& extension)
/**
Map extension parameters from ESock to the QoS PRT Equivalents

@param params input structure that contains the QoS PRT extension parameters
@param extension structure updated to contain the ESock equivalent extension parameters
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("MapExtensionParamsFromPRTToESockL")));

	TUmtsTrafficClass trafficClass = params.TrafficClass();
	extension.SetTrafficClass((RPacketQoS::TTrafficClass) trafficClass);

	TUmtsDeliveryOrder deliveryOrder = params.DeliveryOrder();
	extension.SetDeliveryOrder((RPacketQoS::TDeliveryOrder) deliveryOrder);

	TUmtsErroneousSDUDelivery deliveryOfErroneusSdu = params.DeliveryOfErroneusSdu();
	extension.SetErroneousSDUDelivery((RPacketQoS::TErroneousSDUDelivery) deliveryOfErroneusSdu);

	TUmtsBitErrorRatio residualBer = params.ResidualBer();
	extension.SetResidualBitErrorRatio((RPacketQoS::TBitErrorRatio) residualBer);

	TUmtsSDUErrorRatio errorRatio = params.ErrorRatio();
	extension.SetSDUErrorRatio((RPacketQoS::TSDUErrorRatio) errorRatio);

	TUmtsTrafficHandlingPriority priority = params.Priority();
	extension.SetTrafficHandlingPriority((RPacketQoS::TTrafficHandlingPriority) priority);

	TInt transferDelay = params.TransferDelay();
	extension.SetTransferDelay(transferDelay);

	TInt maxSduSize = params.MaxSduSize();
	extension.SetMaxSduSize(maxSduSize);

	TInt maxBitrateUplink = params.MaxBitrateUplink();
	extension.SetMaxBitrateUplink(maxBitrateUplink);

	TInt maxBitrateDownlink = params.MaxBitrateDownlink();
	extension.SetMaxBitrateDownlink(maxBitrateDownlink);

	TInt guaBitrateUplink = params.GuaranteedBitrateUplink();
	extension.SetGuaBitrateUplink(guaBitrateUplink);

	TInt guaBitrateDownlink = params.GuaranteedBitrateDownlink();
	extension.SetGuaBitrateDownlink(guaBitrateDownlink);
	}


void CIpSubConnectionProvider::MapExtensionParamsFromESockToPRTL(const CSubConQosR5ParamSet& aExtension, TUmtsR5QoSParameters& aParams)
/**
Map extension parameters from QoS PRT to the ESock Equivalents

@param extension input structure that contains the ESock extension parameters
@param params structure updated to contains the equivalent QoS PRT extension parameters
*/
	{

	__IPCPRLOG(IpCprLog::Printf(_L("MapExtensionParamsFromESockToPRTL")));

	MapExtensionParamsFromESockToPRTL(static_cast<const CSubConQosIPLinkR99ParamSet&>(aExtension), static_cast<TUmtsQoSParameters&>(aParams));

	TBool signallingIndicator = aExtension.GetSignallingIndicator();
	User::LeaveIfError(aParams.SetSignallingIndicator(signallingIndicator));

	RPacketQoS::TSourceStatisticsDescriptor sourceStatisticsDescriptor = aExtension.GetSourceStatisticsDescriptor();
	User::LeaveIfError(aParams.SetSourceStatisticsDescriptor(reinterpret_cast<TUmtsSourceStatisticsDescriptor&>(sourceStatisticsDescriptor)));

    iParameterRelease = KParameterRel5;
	}

void MapExtensionParamsFromPRTToESockL(TUmtsR5QoSParameters& aParams, CSubConQosR5ParamSet& aExtension)
/**
Map extension parameters from ESock to the QoS PRT Equivalents

@param params input structure that contains the QoS PRT extension parameters
@param extension structure updated to contain the ESock equivalent extension parameters
*/
	{

	__IPCPRLOG(IpCprLog::Printf(_L("MapExtensionParamsFromPRTToESockL")));

	MapExtensionParamsFromPRTToESockL(static_cast<TUmtsQoSParameters&>(aParams),static_cast<CSubConQosIPLinkR99ParamSet&>(aExtension));

	TBool signallingprtIndicator = aParams.SignallingIndicator();
	aExtension.SetSignallingIndicator(signallingprtIndicator);

	TUmtsSourceStatisticsDescriptor sourceStatisticsprtDescriptor = aParams.SourceStatisticsDescriptor();
	aExtension.SetSourceStatisticsDescriptor(reinterpret_cast<RPacketQoS::TSourceStatisticsDescriptor&>(sourceStatisticsprtDescriptor));

	}

void CIpSubConnectionProvider::MapExtensionParamsFromESockToPRTL(const CSubConImsExtParamSet& aExtension, TImsParameter& aParams)
/**
Map extension parameters from QoS PRT to the ESock Equivalents

@param extension input structure that contains the ESock extension parameters
@param params structure updated to contains the equivalent QoS PRT extension parameters
*/
	{


	__IPCPRLOG(IpCprLog::Printf(_L("MapExtensionParamsFromESockToPRTL")));

	TBool IMSSignallingIndicator = aExtension.GetImsSignallingIndicator();
	aParams.SetIMSSigallingIndicator(IMSSignallingIndicator);

	}

void MapExtensionParamsFromPRTToESockL(TImsParameter& aParams, CSubConImsExtParamSet& aExtension)
/**
Map extension parameters from ESock to the QoS PRT Equivalents

@param params input structure that contains the QoS PRT extension parameters
@param extension structure updated to contain the ESock equivalent extension parameters
*/
	{

	__IPCPRLOG(IpCprLog::Printf(_L("MapExtensionParamsFromPRTToESockL")));

	TBool IMSSignallingIndicator = aParams.GetIMSSigallingIndicator();
	aExtension.SetImsSignallingIndicator(IMSSignallingIndicator);

	}


void CIpSubConnectionProvider::ConvertParametersFromESockL(const RCFParameterFamilyBundleC& aParameterBundle)
/**
Covert QoS Parameters sent from ESock to QoS PRT values and cache the values

@param aParameterBundle the bundle that contains all of ESock's QoS Variables
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CIpSubConnectionProvider::ConvertParametersFromESockL [%08x]"), this));

    ResetPrtExtensions();

	RParameterFamily family = aParameterBundle.FindFamily(KSubConQoSFamily);
	if(!family.IsNull())
		{
		CUmtsQoSPolicy* policy = CUmtsQoSPolicy::NewL();
		CleanupStack::PushL(policy);
		TBool policySet = EFalse;

		// Map Requested Generic Parameters from ESock to PRT Values
		CSubConQosGenericParamSet* generic =
			static_cast<CSubConQosGenericParamSet*>
				(family.FindParameterSet(
						STypeId::CreateSTypeId(CSubConQosGenericParamSet::EUid,CSubConQosGenericParamSet::EType),
					RParameterFamily::ERequested));
		if(generic)
			{
			TRAPD(err, MapGenericParamsFromESockToPRTL(*generic));
			if( err != KErrNone )
				{
				__IPCPRLOG(IpCprLog::Printf(_L("Failed to convert from ESock to PRT Generic Parameters (Requested) with Error: %d"),err));
				}
			}

		// Map Requested UMTS Parameters from ESock to PRT values
		CSubConQosIPLinkR99ParamSet* extension = (CSubConQosIPLinkR99ParamSet*)family.FindParameterSet(
				STypeId::CreateSTypeId(KSubConIPParamsUid,KSubConQosIPLinkR99ParamsType), RParameterFamily::ERequested);
		if(extension)
			{
			TUmtsQoSParameters params;

			TRAPD(err, MapExtensionParamsFromESockToPRTL(*extension, params));
			if( err != KErrNone )
				{
				__IPCPRLOG(IpCprLog::Printf(_L("Failed to convert from ESock to PRT Extension Parameters (Requested) with Error: %d"),err));
				}

			policy->SetQoSRequested(params);
			policySet = ETrue;
			}

		// Map Minimum UMTS Parameters from ESock to PRT values
		extension = (CSubConQosIPLinkR99ParamSet*)family.FindParameterSet(
				STypeId::CreateSTypeId(KSubConIPParamsUid,KSubConQosIPLinkR99ParamsType), RParameterFamily::EAcceptable);
		if(extension)
			{
			TUmtsQoSParameters params;
			TRAPD(err, MapExtensionParamsFromESockToPRTL(*extension,params));
			if( err != KErrNone )
				{
				__IPCPRLOG(IpCprLog::Printf(_L("Failed to convert from ESock to PRT Extension Parameters (Minimum) with Error: %d"),err));
				}

			policy->SetQoSMinimum(params);
			policySet = ETrue;
			}

		if(policySet)
			{
			iPrtExtensions.AddFirst(*policy);
			CleanupStack::Pop();
			}
		else
			{
		    CleanupStack::PopAndDestroy(policy);
			}

		CUmtsR5QoSPolicy* policyR5 = CUmtsR5QoSPolicy::NewL();
		CleanupStack::PushL(policyR5);
		TBool policySetR5 = EFalse;
		// Map Requested UMTS Parameters from ESock to PRT values
		CSubConQosR5ParamSet* extensionR5 = static_cast<CSubConQosR5ParamSet*>(family.FindParameterSet(STypeId::CreateSTypeId(KSubConIPParamsUid,KSubConQosR5ParamsType), RParameterFamily::ERequested));
		if(extensionR5)
			{
			 TUmtsR5QoSParameters paramsR5;
	         TRAPD(err, MapExtensionParamsFromESockToPRTL(*extensionR5, paramsR5));
			 if( err != KErrNone )
				{
				__IPCPRLOG(IpCprLog::Printf(_L("Failed to convert from ESock to PRT Extension Parameters (Requested) with Error: %d"),err));
				}

			policyR5->SetQoSRequested(paramsR5);
			policySetR5 = ETrue;
			}
		// Map Minimum UMTS Parameters from ESock to PRT values
		extensionR5 = static_cast<CSubConQosR5ParamSet*>(family.FindParameterSet(STypeId::CreateSTypeId(KSubConIPParamsUid,KSubConQosR5ParamsType), RParameterFamily::EAcceptable));
		if(extensionR5)
			{
			TUmtsR5QoSParameters paramsR5;
			TRAPD(err, MapExtensionParamsFromESockToPRTL(*extensionR5,paramsR5));
			if( err != KErrNone )
				{
				__IPCPRLOG(IpCprLog::Printf(_L("Failed to convert from ESock to PRT Extension Parameters (Minimum) with Error: %d"),err));
				}

			policyR5->SetQoSMinimum(paramsR5);
			policySetR5 = ETrue;
			}

		if(policySetR5)
			{
			iPrtExtensions.AddFirst(*policyR5);
			CleanupStack::Pop();
			}
		else
			{
		    CleanupStack::PopAndDestroy(policyR5);
			}


			CImsPolicy* policyIms = CImsPolicy ::NewL();
			CleanupStack::PushL(policyIms);
			TBool policySetIms = EFalse;
			// Map Requested UMTS IMS Parameters from ESock to PRT values
			CSubConImsExtParamSet* extensionIms = static_cast<CSubConImsExtParamSet*>(family.FindParameterSet(STypeId::CreateSTypeId(KSubConIPParamsUid,KSubConImsExtParamsType), RParameterFamily::ERequested));
		if(extensionIms)
			{
			TImsParameter paramsIms;

			TRAPD(err, MapExtensionParamsFromESockToPRTL(*extensionIms, paramsIms));
			if( err != KErrNone )
				{
				__IPCPRLOG(IpCprLog::Printf(_L("Failed to convert from ESock to PRT Extension Parameters (Requested) with Error: %d"),err));
				}

			policyIms->SetImsParameter(paramsIms);
			policySetIms = ETrue;
			}

		if(policySetIms)
			{
			iPrtExtensions.AddFirst(*policyIms);
			CleanupStack::Pop();
			}
		else
			{
		    CleanupStack::PopAndDestroy(policyIms);
			}
	}

	// Convert SBLP set if present
	// Check for the presence of the family in the bundle
	RParameterFamily sblpFamily = aParameterBundle.FindFamily(KSubConAuthorisationFamily);
	if ( ! sblpFamily.IsNull() )
		{
		CSblpPolicy* policy = CSblpPolicy::NewL();
		CleanupStack::PushL(policy);
		TBool policySet = EFalse;

		/**
		Extract the generic and extension Parmaters of the Family. At present the generic
		parameters will just contains a dummy , and will not be used. The code is commented
		and can be enabled sometimes when any parameter varification at the code is required
		*/
		// CSubConAuthorisationGenericParamSet* generic = (CSubConAuthorisationGenericParamSet*)SblpFamily->GetGenericSet(CSubConParameterFamily::ERequested);
		/**
		There can be more than one extension set containing multiple MATs, as a requirement for the R6.
		currently the APIs in CSubConParameterFamily is not able to extract multiple parameters. once this
		is done, the code below must be changed to run on a loop on number of extensions
		*/

		// Map Requested UMTS Parameters from ESock to PRT values
		CSubConSBLPR5ExtensionParamSet* extension = (CSubConSBLPR5ExtensionParamSet*)sblpFamily.FindParameterSet(
				STypeId::CreateSTypeId(KSubConIPParamsUid,KSubConnSBLPR5ExtensionParamsType), RParameterFamily::ERequested);
		if(extension)
			{
			CSblpParameters* params = new (ELeave)CSblpParameters;
			CleanupStack::PushL(params);
			TRAPD(err, MapExtensionParamsFromESockToPRTL(*extension,*params));
			if( err != KErrNone )
				{
				__IPCPRLOG(IpCprLog::Printf(_L("Failed to convert from ESock to PRT Extension Parameters (Requested) with Error: %d"),err));
				}

			policy->SetSblpParameters(*params); //does a bitwise copy therefore we must still delete params afterwards
			policySet = ETrue;
			CleanupStack::Pop(params);
			delete params;
			}

		if( policySet )
			{
			iPrtExtensions.AddFirst(*policy);
			CleanupStack::Pop();
			}
		else
    		{
		    CleanupStack::PopAndDestroy(policy);
	    	}
		}
	}


void CIpSubConnectionProvider::ConvertParametersFromQOSL(TPfqosMessage& aMsg, CSubConGenEventParamsGranted* aEvent)
/**
Convert parameters from a PRT response to ESock equivalents.  PRT parameters are cached

@param aMsg QoS PRT Response Message
@param aEvent ESock event to contain changed QoS Parameters
*/
	{
	__IPCPRLOG(IpCprLog::Printf(_L("CIpSubConnectionProvider::ConvertParametersFromQOSL [%08x]"), this));

	/** Changed as per usase of CQosParamaters instead of
	TQoSParameter
	*/

    TQoSParameters qosParams;
	aMsg.SetQoSParameters(qosParams);

    // Copy qosParams into the iPrtParameters
	ConvertTQoSIntoCQoSParamsL(qosParams);
	aEvent->SetFamily(KSubConQoSFamily);

	CSubConQosGenericParamSet* generic = CSubConQosGenericParamSet::NewL();
	MapGenericParamsFromPRTToESockL(*generic);
	aEvent->SetGenericSet(generic);



    TSglQueIter<CPfqosPolicyData> iter(aMsg.iExtensions);
	CPfqosPolicyData* data = NULL;

	// for each pfqos extension that exists in aMsg
	while ((data = iter++) != NULL)
		{
	    const TUint8 *p = data->Data().Ptr();
	    TInt length = data->Data().Length();
	    const struct pfqos_configure* pfqosExtConfig = reinterpret_cast<const struct pfqos_configure*>(p);

	    if (length > (TInt)sizeof(pfqos_configure) &&
		    pfqosExtConfig->pfqos_configure_len * 8 == length &&
		    pfqosExtConfig->pfqos_ext_type == EPfqosExtExtension)
	    	{

	    	// pfqos_extension struct is located straight after pfqos_configure struct
		    p += sizeof(struct pfqos_configure);
		    const struct pfqos_extension* pfqosExtension = reinterpret_cast<const struct pfqos_extension*>(p);
			TInt extType = pfqosExtension->pfqos_extension_type;

			CExtensionBase* scPfqosExtension = NULL;
			TQoSExtensionQueueIter iter2(iPrtExtensions);

			// for each pfqos extension that has been configured in this subconnection
			while ((scPfqosExtension = iter2++) != NULL)
				{
				if (scPfqosExtension->Type() == extType)
					{
					// the extension in the message matches an extension that is configured on the
					// subconnection, now we can parse it
					scPfqosExtension->ParseMessage(data->Data());
                    CSubConExtensionParameterSet* extension = NULL;
                    switch (extType)
                        {
                        case KPfqosExtensionUmts:
                            extension = MapFromUmtsR5ExtensionL(static_cast<CUmtsR5QoSPolicy*>(scPfqosExtension));
                            break;

                        case KPfqosExtensionIMS:
                            extension = MapFromImsExtensionL(static_cast<CImsPolicy*>(scPfqosExtension));
                            break;
                        }


                        if (extension)
                            {
                            // Need to add it to the cleanup stack so we don't lose the ptr
                            // in the event that AddExtensionSetL leaves
                            CleanupStack::PushL(extension);
		                    aEvent->AddExtensionSetL(extension);
		                    CleanupStack::Pop(extension);
                            }

					// Next extension from aMsg
					break;
					}
				}
			}
		}
	}


CSubConExtensionParameterSet* CIpSubConnectionProvider::MapFromUmtsR5ExtensionL (const CUmtsR5QoSPolicy* aPolicy)
    {
 	__IPCPRLOG(IpCprLog::Printf(_L("CQoSIpSubConnectionProvider::MapFromUmtsR5ExtensionL [%08x]"), this));
    ASSERT(aPolicy);

	TNegotiatedUmtsR5QoSParameters grantedR5QoS;
	aPolicy->GetQoSNegotiated(grantedR5QoS);

	CSubConExtensionParameterSet* paramSet;

    switch (iParameterRelease)
        {
        case KParameterRel5:
    		paramSet = CSubConQosR5ParamSet::NewL();
    		CleanupStack::PushL(paramSet);
    		MapExtensionParamsFromPRTToESockL(static_cast<TUmtsR5QoSParameters&>(grantedR5QoS),
    		    *static_cast<CSubConQosR5ParamSet*>(paramSet));
    		CleanupStack::Pop(paramSet);
            break;

        case KParameterRel4Rel99:
    		paramSet = CSubConQosR99ParamSet::NewL();
    		CleanupStack::PushL(paramSet);
    		MapExtensionParamsFromPRTToESockL(static_cast<TUmtsQoSParameters&>(grantedR5QoS),
    		    *static_cast<CSubConQosR99ParamSet*>(paramSet));
            CleanupStack::Pop(paramSet);
            break;

        default:
            // Break in debug builds
            ASSERT(EFalse);
            return NULL;
        }

    return paramSet;
    }


CSubConExtensionParameterSet* CIpSubConnectionProvider::MapFromImsExtensionL (const CImsPolicy* aPolicy)
    {
 	__IPCPRLOG(IpCprLog::Printf(_L("CQoSIpSubConnectionProvider::MapFromImsExtensionL [%08x]"), this));
    ASSERT(aPolicy);

	TImsParameter grantedImsQoS;
	aPolicy->GetImsParameter(grantedImsQoS);

	CSubConImsExtParamSet* extensionIms = CSubConImsExtParamSet::NewL();
	CleanupStack::PushL(extensionIms);
	MapExtensionParamsFromPRTToESockL(grantedImsQoS, *extensionIms);
	CleanupStack::Pop(extensionIms);

	return extensionIms;
    }

 void CIpSubConnectionProvider::ResetPrtExtensions()
 	{
 	TQoSExtensionQueueIter iter(iPrtExtensions);
 	CExtensionBase *ext;
 	ext = iter++;
 	while (ext)
 		{
 		delete ext;
 		ext = iter++;
 		}
 	iPrtExtensions.Reset();
 	}


