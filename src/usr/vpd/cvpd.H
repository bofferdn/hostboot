/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/vpd/cvpd.H $                                          */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2013,2018                        */
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
#ifndef __CVPD_H
#define __CVPD_H


/**
 * @file cvpd.H
 *
 * @brief Provides the interfaces for the CVPD device driver
 *
 */

// ----------------------------------------------
// Includes
// ----------------------------------------------
#include <errl/errlentry.H>
#include <vpd/cvpdenums.H>

#include "ipvpd.H"
#include <config.h>

namespace CVPD
{

    /**
     * @brief Conversion of CVPD Records to corresponding character
     *        representation.
     */
    const IpVpdFacade::recordInfo cvpdRecords[] =
    {
        // -------------------------------------------------------------------
        // NOTE: This list must remain an ordered list!  There will be a
        //      testcase that checks this.  When adding new entries to the
        //      list, be sure that the keyword in each entry (value 0)
        //      are in ascending order.
        // -------------------------------------------------------------------
        { VEIR, "VEIR" },
        { VER0, "VER0" },  //Not currently used, but keep support
        { MER0, "MER0" },
        { VSPD, "VSPD" },
        { VINI, "VINI" },
        { OPFR, "OPFR" },
        { VNDR, "VNDR" },
        { SPDX, "SPDX" },
        // -------------------------------------------------------------------
        // DO NOT USE!!  This is for test purposes ONLY!
        { CVPD_TEST_RECORD, "TEST" },
        // -------------------------------------------------------------------
    };

    /**
     * @brief Conversion of CVPD Keywords to corresponding character
     *        representation.
     */
    const IpVpdFacade::keywordInfo cvpdKeywords[] =
    {
        // -------------------------------------------------------------------
        // NOTE: This list must remain an ordered list!  There will be a
        //      testcase that checks this.  When adding new entries to
        //      the list, be sure that the keyword in each entry (value 0)
        //      are in ascending order.
        // -------------------------------------------------------------------
        { pdI, "#I" },
        { PF,  "PF" },
        { MT,  "MT" },
        { MR,  "MR" },
        { pdA, "#A" },
        { EL,  "EL" },
        { LM,  "LM" },
        { MW,  "MW" },
        { MV,  "MV" },
        { AM,  "AM" },
        { VZ,  "VZ" },
        { pdD, "#D" },
        { MX,  "MX" },
        { DW,  "DW" },
        { PN,  "PN" },
        { SN,  "SN" },
        { DR,  "DR" },
        { CE,  "CE" },
        { FN,  "FN" },
        { CC,  "CC" },
        { HE,  "HE" },
        { CT,  "CT" },
        { HW,  "HW" },
        { VD,  "VD" },
        { VN,  "VN" },
        { VP,  "VP" },
        { VS,  "VS" },
        { M0,  "M0" },
        { M1,  "M1" },
        { M2,  "M2" },
        { M3,  "M3" },
        { M4,  "M4" },
        { M5,  "M5" },
        { M6,  "M6" },
        { M7,  "M7" },
        { M8,  "M8" },
        { T1,  "T1" },
        { T2,  "T2" },
        { T4,  "T4" },
        { T5,  "T5" },
        { T6,  "T6" },
        { T8,  "T8" },
        { Q0,  "Q0" },
        { Q1,  "Q1" },
        { Q2,  "Q2" },
        { Q3,  "Q3" },
        { Q4,  "Q4" },
        { Q5,  "Q5" },
        { Q6,  "Q6" },
        { Q7,  "Q7" },
        { Q8,  "Q8" },
        { K0,  "K0" },
        { K1,  "K1" },
        { K2,  "K2" },
        { K3,  "K3" },
        { K4,  "K4" },
        { K5,  "K5" },
        { K6,  "K6" },
        { K7,  "K7" },
        { K8,  "K8" },
        { MM,  "MM" },
        { SS,  "SS" },
        { ET,  "ET" },
        { VM,  "VM" },
        { pd1, "#1" },
        { pdZ, "#Z" },
        { pd4, "#4" },
        { pd5, "#5" },
        { pd6, "#6" },
        { pd8, "#8" },
        { pdY, "#Y" },
        { MB,  "MB" },

        // -------------------------------------------------------------------
        // DO NOT USE!!  This is for test purposes ONLY!
        { CVPD_TEST_KEYWORD, "XX" },
        // -------------------------------------------------------------------

        //Common ipvpd
        { FULL_RECORD, "FL"},
    };

}; // end CVPD namespace

class CvpdFacade: public IpVpdFacade
{
  public:

    /**
     * @brief Constructor
     */
    CvpdFacade( );

#ifdef CONFIG_SECUREBOOT
    /**
     * @brief Load/unload the appropriate secure section for
     *        an overriden PNOR section
     * @param[in] i_args    Record/keyword
     * @param[in] i_target  Target pointer
     * @param[in] i_load    true=load, false=unload
     * @param[out] o_loaded true=section loaded, false=section not loaded
     * @return Error log
     */
    virtual errlHndl_t loadUnloadSecureSection( input_args_t i_args,
                                                TARGETING::Target* i_target,
                                                bool i_load,
                                                bool& o_loaded );
#endif

  private:

    /**
     * @brief This function returns a primary and an alternate list of records
     *       that should be copied to pnor. The Alternate list is optional.
     *
     * @param[out] o_primaryVpdRecords - Pointer to array of VPD Records to use
     *
     * @param[out] o_primaryRecSize -    Size of o_primaryVpdRecords array
     *
     * @param[out] o_altVpdRecords -     Pointer to array of VPD Records to use
     *
     * @param[out] o_altRecSize -        Size of o_altVpdRecords array
     *
     */
    void getRecordLists(
                const  recordInfo* & o_primaryVpdRecords,
                uint64_t           & o_primaryRecSize,
                const  recordInfo* & o_altVpdRecords,
                uint64_t           & o_altRecSize);

  protected:
    /**
     * @brief Callback function to check for a record override and
     *        set iv_overridePtr appropriately
     * @param[in] i_record  Record name
     * @param[in] i_target  Target pointer
     * @param[out] o_ptr    Pointer to location of record in PNOR,
     *                      ==nullptr if there is no override
     * @return Error log
     */
    virtual errlHndl_t checkForRecordOverride( const char* i_record,
                                               TARGETING::Target* i_target,
                                               uint8_t*& o_ptr );

};
#endif  // __CVPD_H
