/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/secureboot/node_comm/node_comm.C $                    */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2018                             */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
/**
 * @file node_comm.C
 *
 * @brief Implementation of the Secure Node Communications Functions
 *
 */

// ----------------------------------------------
// Includes
// ----------------------------------------------
#include <string.h>
#include <sys/time.h>
#include <sys/task.h>
#include <errl/errlentry.H>
#include <errl/errlmanager.H>
#include <errl/errludtarget.H>
#include <errl/errludlogregister.H>
#include <targeting/common/targetservice.H>
#include <devicefw/userif.H>
#include <devicefw/driverif.H>
#include <secureboot/secure_reasoncodes.H>
#include <secureboot/nodecommif.H>
#include <targeting/common/commontargeting.H>
#include <targeting/common/utilFilter.H>

#include "node_comm.H"

using   namespace   TARGETING;

namespace SECUREBOOT
{

namespace NODECOMM
{

// ----------------------------------------------
// Defines
// ----------------------------------------------
// If the link(s) are up the operation should complete right away
// so there will only be a short polling window
#define NODE_COMM_POLL_DELAY_NS (10 * NS_PER_MSEC)  // Sleep for 10ms per poll
// FSP is expecting a reply in 30 seconds, so leave some buffer
#define NODE_COMM_POLL_DELAY_TOTAL_NS (25 * NS_PER_SEC) // Total time 25s


/**
 *  @brief This function waits for the processor to receive a message over
 *         ABUS from a processor on another node.
 */
errlHndl_t nodeCommAbusRecvMessage(TARGETING::Target* i_pProc,
                                   uint64_t & o_data,
                                   uint8_t & o_linkId,
                                   uint8_t & o_mboxId)
{
    errlHndl_t err = nullptr;
    bool attn_found = false;

    const uint64_t interval_ns = NODE_COMM_POLL_DELAY_NS;
    uint64_t time_polled_ns = 0;

    TRACFCOMP(g_trac_nc,ENTER_MRK"nodeCommAbusRecvMessage: pProc=0x%.08X",
              get_huid(i_pProc));

    do
    {
        do
        {

        // Look for Attention
        err = nodeCommMapAttn(i_pProc,
                              NCDD_MODE_ABUS,
                              attn_found,
                              o_linkId,
                              o_mboxId);
        if (err)
        {
            TRACFCOMP(g_trac_nc,ERR_MRK"nodeCommAbusRecvMessage: Error Back "
                      "From nodeCommMapAttn: Tgt=0x%.08X: "
                      TRACE_ERR_FMT,
                      get_huid(i_pProc),
                      TRACE_ERR_ARGS(err));
            break;
        }
        if (attn_found == true)
        {
            TRACFCOMP(g_trac_nc,INFO_MRK"nodeCommAbusRecvMessage: "
              "nodeCommMapAttn attn_found (%d) for Tgt=0x%.08X, link=%d, "
              "mbox=%d",
              attn_found, get_huid(i_pProc), o_linkId, o_mboxId);
            break;
        }

        if (time_polled_ns >= NODE_COMM_POLL_DELAY_TOTAL_NS)
        {
            TRACFCOMP(g_trac_nc,EXIT_MRK"nodeCommAbusRecvMessage: "
              "timeout: time_polled_ns-0x%.16llX, MAX=0x%.16llX, "
              "interval=0x%.16llX",
              time_polled_ns, NODE_COMM_POLL_DELAY_TOTAL_NS, interval_ns);

            /*@
             * @errortype
             * @reasoncode       RC_NC_WAITING_TIMEOUT
             * @moduleid         MOD_NC_RECV
             * @userdata1[0:31]  Master Proc Target HUID
             * @userdata1[32:63] Time Polled in ns
             * @userdata2[0:31]  Defined MAX Poll Time in ns
             * @userdata2[32:63] Time Interval Between Polls in ns
             * @devdesc          Timed out waiting to receive message over
             *                   ABUS Link Mailbox
             * @custdesc         Secure Boot failure
             */
            err = new ERRORLOG::ErrlEntry( ERRORLOG::ERRL_SEV_UNRECOVERABLE,
                                           MOD_NC_RECV,
                                           RC_NC_WAITING_TIMEOUT,
                                           TWO_UINT32_TO_UINT64(
                                             get_huid(i_pProc),
                                             time_polled_ns),
                                           TWO_UINT32_TO_UINT64(
                                             NODE_COMM_POLL_DELAY_TOTAL_NS,
                                             interval_ns));

            // Bus Callout will be handled by caller since
            // it should know expected peer target

            // Or HB code failed to do the procedure correctly
            err->addProcedureCallout(HWAS::EPUB_PRC_HB_CODE,
                                     HWAS::SRCI_PRIORITY_LOW);

            // Grab FFDC from the target
            getNodeCommFFDC(NCDD_MODE_ABUS,
                            i_pProc,
                            err);

            break;
        }

        // Sleep before polling again
        nanosleep( 0, interval_ns );
        task_yield(); // wait patiently
        time_polled_ns += interval_ns;

        } while(attn_found == false);

    if (err)
    {
        break;
    }

    if (attn_found == true)
    {
        //  Read message on proc with Link Mailbox found above
        o_data = 0;
        size_t expSize = sizeof(o_data);
        auto reqSize = expSize;
        err = DeviceFW::deviceRead(i_pProc,
                                   &o_data,
                                   reqSize,
                                   DEVICE_NODECOMM_ADDRESS(NCDD_MODE_ABUS,
                                                           o_linkId,
                                                           o_mboxId));

        if (err)
        {
            TRACFCOMP(g_trac_nc,ERR_MRK"nodeCommRecvMessage: Error Back From "
                      "Abus MBox Read: Tgt=0x%.08X, link=%d, mbox=%d: "
                      TRACE_ERR_FMT,
                      get_huid(i_pProc), o_linkId, o_mboxId,
                      TRACE_ERR_ARGS(err));
            break;
        }
        assert(reqSize==expSize,"nodeCommRecvMessage: SCOM deviceRead didn't return expected data size of %d (it was %d)",
               expSize,reqSize);

    }

    } while( 0 );

    TRACFCOMP(g_trac_nc,EXIT_MRK"nodeCommAbusRecvMessage: "
              "Tgt=0x%.08X, link=%d, mbox=%d attn_found=%d: "
              "data=0x%.16llX. "
              TRACE_ERR_FMT,
              get_huid(i_pProc), o_linkId, o_mboxId, attn_found, o_data,
              TRACE_ERR_ARGS(err));

    return err;

} // end of nodeCommAbusRecvMessage


/**
 *  @brief This function sends a message over the ABUS from the processor of
 *         the current node to a processor on another node.
 */
errlHndl_t nodeCommAbusSendMessage(TARGETING::Target* i_pProc,
                                   const uint64_t & i_data,
                                   const uint8_t & i_linkId,
                                   const uint8_t & i_mboxId)
{
    errlHndl_t err = nullptr;

    TRACFCOMP(g_trac_nc,ENTER_MRK"nodeCommAbusSendMessage: iProc=0x%.08X "
              "to send data=0x%.16llX through linkId=%d mboxId=%d",
              get_huid(i_pProc), i_data, i_linkId, i_mboxId);

    do
    {
        // Send Data
        uint64_t data = i_data; // to keep i_data const
        size_t expSize = sizeof(i_data);
        auto reqSize = expSize;
        err = DeviceFW::deviceWrite(i_pProc,
                                    &data,
                                    reqSize,
                                    DEVICE_NODECOMM_ADDRESS(NCDD_MODE_ABUS,
                                                            i_linkId,
                                                            i_mboxId));
        if (err)
        {
            TRACFCOMP(g_trac_nc,ERR_MRK"nodeCommAbusSendMessage: Error Back "
                      "From Abus MBox Send: Tgt=0x%.08X, data=0x%.16llX, "
                      "link=%d, mbox=%d: "
                      TRACE_ERR_FMT,
                      get_huid(i_pProc), i_data, i_linkId, i_mboxId,
                      TRACE_ERR_ARGS(err));
            break;
        }
        assert(reqSize==expSize,"nodeCommAbusSendMessage: SCOM deviceRead didn't return expected data size of %d (it was %d)",
               expSize,reqSize);

    } while( 0 );

    TRACFCOMP(g_trac_nc,EXIT_MRK"nodeCommAbusSendMessage: "
              TRACE_ERR_FMT,
              TRACE_ERR_ARGS(err));

    return err;

} // end of nodeCommAbusSendMessage



/**
 *  @brief Map Attention Bits in FIR Register to specific Link Mailbox
 */
errlHndl_t nodeCommMapAttn(TARGETING::Target* i_pProc,
                           const node_comm_modes_t i_mode,
                           bool & o_attn_found,
                           uint8_t & o_linkId,
                           uint8_t & o_mboxId)
{
    errlHndl_t err = nullptr;
    uint64_t fir_data = 0x0;
    uint64_t fir_data_with_mask = 0x0;
    o_attn_found = false;

    assert(i_mode < NCDD_MODE_INVALID,"nodeCommMapAttn: Invalid mode: %d",
           i_mode);

    const uint64_t fir_mask = (i_mode == NCDD_MODE_ABUS)
                              ? NCDD_ABUS_FIR_ATTN_MASK
                              : NCDD_XBUS_FIR_ATTN_MASK;

    const uint64_t fir_addr = (i_mode == NCDD_MODE_ABUS)
                              ? NCDD_REG_FIR + NCDD_ABUS_REG_OFFSET
                              : NCDD_REG_FIR;

    const size_t expSize = sizeof(fir_data);

    TRACUCOMP(g_trac_nc,ENTER_MRK
              "nodeCommMapAttn: tgt=0x%X, mode=%s, fir_addr=0x%.16llX",
              get_huid(i_pProc),
              (i_mode == NCDD_MODE_ABUS)
                ? NCDD_ABUS_STRING : NCDD_XBUS_STRING,
              fir_addr);


    do
    {
    // Read the FIR reg
    auto reqSize = expSize;
    err = DeviceFW::deviceRead(i_pProc,
                               &fir_data,
                               reqSize,
                               DEVICE_SCOM_ADDRESS(fir_addr));

    if(err)
    {
        TRACFCOMP(g_trac_nc,ERR_MRK"nodeCommMapAttn Read Fail! (%s): "
                  " tgt=0x%X, reg_addr=0x%.16llX, data=0x%.16llX "
                  TRACE_ERR_FMT,
                  (i_mode == NCDD_MODE_ABUS)
                    ? NCDD_ABUS_STRING : NCDD_XBUS_STRING,
                  TARGETING::get_huid(i_pProc),
                  fir_addr, fir_data,
                  TRACE_ERR_ARGS(err));
        break;
    }
    assert(reqSize==expSize,"nodeCommMapAttn: SCOM deviceRead didn't return expected data size of %d (it was %d)",
           expSize,reqSize);

    // Map Attention bits in the FIR
    fir_data_with_mask = fir_data & fir_mask;
    const int bit_count = __builtin_popcount(fir_data_with_mask);
    TRACUCOMP(g_trac_nc,"nodeCommMapAttn: FIR data = 0x%.16llX, "
              "mask=0x%.16llX, data+mask=0x%.16llX, count=%d",
              fir_data, fir_mask, fir_data_with_mask, bit_count);

    if (bit_count == 0)
    {
        TRACUCOMP(g_trac_nc,INFO_MRK"nodeCommMapAttn: no attentions found: "
                  "FIR data = 0x%.16llX, mask=0x%.16llX, data+mask=0x%.16llX",
                  fir_data, fir_mask, fir_data_with_mask);
        break;
    }
    else if (bit_count > 1)
    {
        TRACFCOMP(g_trac_nc,ERR_MRK"nodeCommMapAttn: "
                  "Too many attentions found (%d) in fir: data=0x%.16llX, "
                  "data+mask=0x%.16llX, fir_addr=0x%.16llX",
                  bit_count, fir_data, fir_data_with_mask, fir_addr);

        /*@
         * @errortype
         * @reasoncode       RC_NC_TOO_MANY_ATTNS_FOUND
         * @moduleid         MOD_NC_MAP_ATTN
         * @userdata1        Raw FIR Data
         * @userdata2[0:31]  Number of Attentions found
         * @userdata2[32:63] Target HUID FIR was read from
         * @devdesc          Too many attentions were found in
         *                   the Node Comm FIR Register
         * @custdesc         Secure Boot failure
         */
        err = new ERRORLOG::ErrlEntry( ERRORLOG::ERRL_SEV_UNRECOVERABLE,
                                       MOD_NC_MAP_ATTN,
                                       RC_NC_TOO_MANY_ATTNS_FOUND,
                                       fir_data,
                                       TWO_UINT32_TO_UINT64(
                                         bit_count,
                                         get_huid(i_pProc)));

        // Likely HB code failed to do the procedure correctly
        err->addProcedureCallout(HWAS::EPUB_PRC_HB_CODE,
                                 HWAS::SRCI_PRIORITY_HIGH);

        // Or unlikely an issue with Processor or its bus
        err->addHwCallout( i_pProc,
                           HWAS::SRCI_PRIORITY_LOW,
                           HWAS::NO_DECONFIG,
                           HWAS::GARD_NULL );

        // Collect FFDC
        getNodeCommFFDC(i_mode, i_pProc, err);

        err->collectTrace(SECURE_COMP_NAME);
        err->collectTrace(NODECOMM_TRACE_NAME);

        break;
    }

    int bit = 0;
    const int possible_attn_bits = __builtin_popcount(fir_mask);
    for ( ; bit < possible_attn_bits ; ++bit)
    {
        // Start at first bit and shift right to find an attention
        if ( fir_data & (NCDD_START_OF_ATTN_BITS >> bit))
        {
            o_attn_found = true;
            o_linkId = (bit / 2);
            o_mboxId = (bit % 2);

            TRACFCOMP(g_trac_nc,INFO_MRK"nodeCommMapAttn: tgt=0x%X: "
                      "o_attn_found=%d, o_linkId=%d, mboxId=%d, "
                      TRACE_ERR_FMT,
                      get_huid(i_pProc), o_attn_found, o_linkId, o_mboxId,
                      TRACE_ERR_ARGS(err));
            break;
        }
    }

    } while( 0 );

    TRACUCOMP(g_trac_nc,EXIT_MRK"nodeCommMapAttn: tgt=0x%X: "
              "o_attn_found=%d, o_linkId=%d, mboxId=%d, "
              TRACE_ERR_FMT,
              get_huid(i_pProc), o_attn_found, o_linkId, o_mboxId,
              TRACE_ERR_ARGS(err));

    return err;

} // end of nodeCommMapAttn


/**
 * @brief Add FFDC for the target to an error log
 */
void getNodeCommFFDC( node_comm_modes_t   i_mode,
                      TARGETING::Target*  i_pProc,
                      errlHndl_t          &io_log)
{
    TRACFCOMP(g_trac_nc,ENTER_MRK
              "getNodeCommFFDC: tgt=0x%X, mode=%s, err_plid=0x%X",
              get_huid(i_pProc),
              (i_mode == NCDD_MODE_ABUS)
                ? NCDD_ABUS_STRING : NCDD_XBUS_STRING,
              ERRL_GETPLID_SAFE(io_log));

    do
    {
    if (io_log == nullptr)
    {
        TRACFCOMP(g_trac_nc,INFO_MRK"getNodeCommFFDC: io_log==nullptr, so "
                  "no FFDC has been collected for tgt=0x%X, mode=%s",
                  get_huid(i_pProc),
                  (i_mode == NCDD_MODE_ABUS)
                    ? NCDD_ABUS_STRING : NCDD_XBUS_STRING);
        break;
    }

    // Add Target to log
    ERRORLOG::ErrlUserDetailsTarget(i_pProc,"Proc Target").addToLog(io_log);

    // Add HW regs
    ERRORLOG::ErrlUserDetailsLogRegister ffdc(i_pProc);

    // FIR/Control/Status/Data Registers
    ffdc.addData(DEVICE_SCOM_ADDRESS(getLinkMboxRegAddr(NCDD_REG_FIR,i_mode)));
    ffdc.addData(DEVICE_SCOM_ADDRESS(getLinkMboxRegAddr(NCDD_REG_CTRL,i_mode)));
    ffdc.addData(DEVICE_SCOM_ADDRESS(getLinkMboxRegAddr(NCDD_REG_DATA,i_mode)));

    // Loop Through All of the Mailbox Registers Where the Data Could End Up
    uint64_t l_reg = 0;
    const auto max_linkId = (i_mode==NCDD_MODE_ABUS)
                              ? NCDD_MAX_ABUS_LINK_ID
                              : NCDD_MAX_XBUS_LINK_ID;

    for (size_t linkId=0;  linkId <= max_linkId ; ++linkId)
    {
        for (size_t mboxId=0; mboxId <= NCDD_MAX_MBOX_ID; ++mboxId)
        {
            l_reg = getLinkMboxReg(linkId, mboxId);
            ffdc.addData(DEVICE_SCOM_ADDRESS(getLinkMboxRegAddr(l_reg,i_mode)));
        }
    }

    ffdc.addToLog(io_log);


    } while( 0 );

    TRACFCOMP(g_trac_nc,EXIT_MRK"getNodeCommFFDC");

    return;

} // end of getNodeCommFFDC


/**
 * @brief Add a bus callout to an error log
 */
void addNodeCommBusCallout(node_comm_modes_t i_mode,
                           TARGETING::Target* i_pProc,
                           uint8_t & i_linkId,
                           errlHndl_t & io_log,
                           HWAS::callOutPriority i_priority)
{
    TRACFCOMP(g_trac_nc,ENTER_MRK
              "addNodeCommBusCallout: tgt=0x%X, mode=%s, linkId=%d, "
              "err_plid=0x%X, priority=%d",
              get_huid(i_pProc),
              (i_mode == NCDD_MODE_ABUS)
                ? NCDD_ABUS_STRING : NCDD_XBUS_STRING,
              i_linkId,
              ERRL_GETPLID_SAFE(io_log), i_priority);

    // Bus associated with i_pProc and i_linkId (PHYS_PATH)
    char * l_ep1_path_str = nullptr;

    // PEER_PATH associated with l_ep1
    char * l_ep2_path_str = nullptr;

    bool found_peer_endpoint = false;

    do
    {
    if ((io_log == nullptr) ||
        (i_pProc == nullptr) ||
        ((i_mode != NCDD_MODE_ABUS) &&
         (i_mode != NCDD_MODE_XBUS)))
    {
        TRACFCOMP(g_trac_nc,INFO_MRK"addNodeCommBusCallout: io_log==nullptr or "
                  "tgt=0x%X is nullptr or i_mode=%d is invalid (not ABUS (%d) "
                  "or XBUS (%d)) so no bus callout has beed added",
                  get_huid(i_pProc), i_mode, NCDD_MODE_ABUS, NCDD_MODE_XBUS);
        break;
    }

    // Get Bus Type
    HWAS::busTypeEnum l_bus_type = (i_mode == NCDD_MODE_ABUS)
                                    ? HWAS::O_BUS_TYPE : HWAS::X_BUS_TYPE; // using O_ instead of A_BUS_TYPE

    // Get all Chiplets for this target aligned with the input mode
    TargetHandleList l_busTargetList;
    TYPE l_type = (i_mode == NCDD_MODE_ABUS)
                   ? TYPE_OBUS : TYPE_XBUS;
    getChildChiplets(l_busTargetList, i_pProc, l_type);

    // Get BUS Instance
    // For each OBUS or XBUS instance there are 2 links and 2 mailboxes,
    // so divide the linkId by 2 to get bus instance
    uint8_t l_bus_instance_ep1 = i_linkId / 2;


    // Look through Bus Targets looking for specific Bus Instance
    // to find the right PEER_TARGET
    for (const auto & l_busTgt : l_busTargetList)
    {
        found_peer_endpoint = false;
        EntityPath l_ep1 = l_busTgt->getAttr<ATTR_PHYS_PATH>();
        if (l_ep1_path_str != nullptr)
        {
            free(l_ep1_path_str);
        }
        l_ep1_path_str = l_ep1.toString();


        EntityPath l_ep2 = l_busTgt->getAttr<ATTR_PEER_PATH>();
        if (l_ep2_path_str != nullptr)
        {
            free(l_ep2_path_str);
        }
        l_ep2_path_str = l_ep2.toString();

        TRACUCOMP(g_trac_nc,INFO_MRK"addNodeCommBusCallout: Checking "
                      "i_pProc 0x%.08X BUS HUID 0x%.08X's (%s) PEER_PATH: %s",
                      get_huid(i_pProc), get_huid(l_busTgt),
                      l_ep1_path_str,
                      l_ep2_path_str);

        EntityPath::PathElement l_ep2_peBus =
                                  l_ep2.pathElementOfType(l_type);
        if(l_ep2_peBus.type == TYPE_NA)
        {
            TRACUCOMP(g_trac_nc,INFO_MRK"addNodeCommBusCallout: "
                      "Skipping i_pProc 0x%.08X "
                      "BUS HUID 0x%.08X's (%s) PEER_PATH %s because "
                      "cannot find BUS in PEER_PATH",
                      get_huid(i_pProc), get_huid(l_busTgt),
                      l_ep1_path_str, l_ep2_path_str);
            continue;
        }

        if (l_bus_instance_ep1 == l_busTgt->getAttr<ATTR_REL_POS>())
        {
            TRACFCOMP(g_trac_nc,INFO_MRK"addNodeCommBusCallout: "
                      "Using i_pProc 0x%.08X BUS HUID 0x%.08X (%s) "
                      "PEER_PATH %s as it had right instance %d for ep1",
                      get_huid(i_pProc), get_huid(l_busTgt), l_ep1_path_str,
                      l_ep2_path_str, l_bus_instance_ep1);

            found_peer_endpoint = true;

            // Add Bus Callout
            io_log->addBusCallout(l_ep1,
                                  l_ep2,
                                  l_bus_type,
                                  i_priority);

           // @TODO RTC 184518 - Possibly go to depth of TYPE_SMPGROUP,
           // which is one level below OBUS for ABUS/OBUS callouts

            break;
        }
        else
        {
            TRACUCOMP(g_trac_nc,INFO_MRK"addNodeCommBusCallout: "
                      "Skipping i_pProc 0x%.08X BUS HUID 0x%.08X's "
                      "PEER_PATH %s because ep1 bus instance (%d) does not "
                      "match instance (%d) converted from i_linkId (%d)",
                      get_huid(i_pProc), get_huid(l_busTgt),
                      l_ep2_path_str, l_busTgt->getAttr<ATTR_ORDINAL_ID>(),
                      l_bus_instance_ep1, i_linkId);
        }
    }

    if (found_peer_endpoint == false)
    {
        TRACFCOMP(g_trac_nc,INFO_MRK"addNodeCommBusCallout: Unable to find a "
                  "peer_tgt for tgt=0x%X, mode=%s, linkId=%d, so no bus "
                  "callout has been added",
                  get_huid(i_pProc),
                  (i_mode == NCDD_MODE_ABUS)
                    ? NCDD_ABUS_STRING : NCDD_XBUS_STRING,
                  i_linkId);
        break;
    }

    } while( 0 );

    if (l_ep1_path_str != nullptr)
    {
        free(l_ep1_path_str);
        l_ep1_path_str = nullptr;
    }

    if (l_ep2_path_str != nullptr)
    {
        free(l_ep2_path_str);
        l_ep2_path_str = nullptr;
    }

    TRACFCOMP(g_trac_nc,EXIT_MRK"addNodeCommBusCallout");

    return;

} // end of addNodeCommBusCallout

} // End NODECOMM namespace

} // End SECUREBOOT namespace

