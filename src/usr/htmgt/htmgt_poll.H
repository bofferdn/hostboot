/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/htmgt/htmgt_poll.H $                                  */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2014,2017                        */
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

#ifndef HTMGT_POLL_H
#define HTMGT_POLL_H

#include <stdint.h>
#include <htmgt/htmgt.H>
#include "htmgt_activate.H"
#include "htmgt_occ.H"

namespace HTMGT
{

    // OCC Status bits
    const uint8_t OCC_STATUS_MASTER         = 0x80;
    const uint8_t OCC_STATUS_FIR_MASTER     = 0x40;
    const uint8_t OCC_STATUS_ATTN_ENABLE    = 0x08;
    const uint8_t OCC_STATUS_OBS_READY      = 0x02;
    const uint8_t OCC_STATUS_ACTIVE_READY   = 0x01;

    // OCC Extended Status bits
    const uint8_t OCC_XSTATUS_DVFS_OT       = 0x80;
    const uint8_t OCC_XSTATUS_DVFS_PWR      = 0x40;
    const uint8_t OCC_XSTATUS_MEM_THROT_OT  = 0x20;
    const uint8_t OCC_XSTATUS_N_POWER       = 0x10;

    // GPU Config bits
    enum gpuConfig_e
    {
        GPUCFG_GPU2_PRESENT = 0x04,
        GPUCFG_GPU1_PRESENT = 0x02,
        GPUCFG_GPU0_PRESENT = 0x01
    };

    struct occPollRspStruct_t
    {
        uint8_t   status;
        uint8_t   extStatus;
        uint8_t   occsPresent;
        uint8_t   requestedCfg;
        uint8_t   state;
        uint8_t   mode;
        uint8_t   IPSStatus;
        uint8_t   errorId;
        uint32_t  errorAddress;
        uint16_t  errorLength;
        uint8_t   reserved;
        uint8_t   gpuCfg;
        uint8_t   codeLevel[16];
        uint8_t   sensor[6];
        uint8_t   numBlocks;
        uint8_t   version;
        uint8_t   sensorData[4049];
    }  __attribute__((packed));

    const uint32_t HTMGT_OCC_POLL_DATA_SIZE = sizeof(occPollRspStruct_t);


} // end namespace
#endif
