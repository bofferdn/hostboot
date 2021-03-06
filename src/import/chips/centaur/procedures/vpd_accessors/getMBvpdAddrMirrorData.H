/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/centaur/procedures/vpd_accessors/getMBvpdAddrMirrorData.H $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2017                             */
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
///  @file getMBvpdAddrMirror.H
///  @brief Prototype for getMBvpdAddrMirror()-get Address Mirroring Data from MBvpd
///
/// *HWP HWP Owner: Luke Mulkey <lwmulkey@us.ibm.com>
/// *HWP HWP Backup: Andre Marin <aamarin@us.ibm.com>
/// *HWP Team: Memory
/// *HWP Level: 2
/// *HWP Consumed by: HB
#ifndef    _HWP_MBVPDADDRMIRRORDATA_
#define    _HWP_MBVPDADDRMIRRORDATA_

#include   <fapi2.H>

// function pointer typedef definition for HWP call support
typedef fapi2::ReturnCode ( *getMBvpdAddrMirrorData_FP_t)
(const fapi2::Target<fapi2::TARGET_TYPE_MBA>&, uint8_t (&) [2][2]);

extern "C"
{
///
/// @brief get Address Mirroring Data from cvpd record VSPD keyword AM
///
/// The Address Mirroring Data attributes are retrieved from cvpd record VSPD
/// keyword AM.
/// There are two mba per memory buffer: position 0 and position 1.
/// There are two ports for each mba and 2 DIMMs per port.
///
/// cpvd record VSPD keyword AM - 4 bytes
/// -----------------------------------
/// | mba position 0                  |
/// |  -----------------------------  |
/// |  | port 0  (Port A)          |  |
/// |  |  ------------------------ |  |
/// |  |  | DIMM 0 :  4 bits     | |  |
/// |  |  ------------------------ |  |
/// |  |  | DIMM 1 :  4 bits     | |  |
/// |  |  ------------------------ |  |
/// |  |---------------------------|  |
/// |  | port 1  (Port B)          |  |
/// |  |  ------------------------ |  |
/// |  |  | DIMM 0 :  4 bits     | |  |
/// |  |  ------------------------ |  |
/// |  |  | DIMM 1 :  4 bits     | |  |
/// |  |  ------------------------ |  |
/// |  -----------------------------  |
/// |---------------------------------|
/// | mba postion 1                   |
/// |  -----------------------------  |
/// |  | port 0  (Port C)          |  |
/// |  |  ------------------------ |  |
/// |  |  | DIMM 0 :  4 bits     | |  |
/// |  |  ------------------------ |  |
/// |  |  | DIMM 1 :  4 bits     | |  |
/// |  |  ------------------------ |  |
/// |  |---------------------------|  |
/// |  | port 1  (Port D)          |  |
/// |  |  ------------------------ |  |
/// |  |  | DIMM 0 :  4 bits     | |  |
/// |  |  ------------------------ |  |
/// |  |  | DIMM 1 :  4 bits     | |  |
/// |  |  ------------------------ |  |
/// |  -----------------------------  |
/// -----------------------------------
///
/// Bit definition
///
///        RANK0_MIRRORED = 0x08,
///        RANK1_MIRRORED = 0x04,
///        RANK2_MIRRORED = 0x02,
///        RANK3_MIRRORED = 0x01,
///
/// @param[in]  i_mbaTarget  -   mba target
/// @param[out] o_val        -   2 x 2 array of bytes ([num ports] [num dimms])
///                              Address Mirroring 4 bits per dimm returned
///                              in the lower nibble of the byte for the mba
///
/// @return fapi::ReturnCode -   FAPI_RC_SUCCESS if success,relevant error code for failure.
///
///
    fapi2::ReturnCode getMBvpdAddrMirrorData( const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_mbaTarget,
            uint8_t (&o_val) [2][2]);

}

#endif
