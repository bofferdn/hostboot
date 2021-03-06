/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/pm/p9_quad_power_off.H $   */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2017                        */
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
///
/// @file p9_quad_power_off.H
/// @brief Power off the EQ including the functional cores associatated with it.
///
//----------------------------------------------------------------------------
// *HWP HWP Owner       : Greg Still <stillgs@us.ibm.com>
// *HWP FW Owner        : Prem S Jha <premjha2@in.ibm.com>
// *HWP Team            : PM
// *HWP Level           : 3
// *HWP Consumed by     : OCC:CME:FSP
//----------------------------------------------------------------------------

#ifndef _P9_QUADPOWEROFF_H_
#define _P9_QUADPOWEROFF_H_

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <fapi2.H>

//------------------------------------------------------------------------------
// Constant definitions
//------------------------------------------------------------------------------

// function pointer typedef definition for HWP call support
typedef fapi2::ReturnCode (*p9_quad_power_off_FP_t) (
    const fapi2::Target<fapi2::TARGET_TYPE_EQ>& i_target,
    uint64_t* o_ring_save_data);

extern "C"
{

//------------------------------------------------------------------------------
// Function prototype
//------------------------------------------------------------------------------
    /**
     ** @brief Power off the EQ including the functional cores associatated with it.
     **
     ** @param[in]  i_target         Targe type EQ
     ** @param[out] o_ring_save_data The ring data that tells the PB how to find the
     **                              EQ will need to be saved away for when we p-on
     **
     ** @return FAPI2_RC_SUCCESS if success, error otherwise
     **/
    fapi2::ReturnCode p9_quad_power_off (
        const fapi2::Target<fapi2::TARGET_TYPE_EQ>& i_target,
        uint64_t* o_ring_save_data);

} // extern "C"

#endif // _P9_QUADPOWEROFF_H_
