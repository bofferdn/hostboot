/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/errl/plugins/errluserdetails.H $                      */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2011,2018                        */
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
#ifndef ERRL_ERRLUSERDETAILS_H
#define ERRL_ERRLUSERDETAILS_H

/**
 *  @file errluserdetails.H
 *
 *  Defines the ErrlUserDetailsParser base class
 *
 *  For each class derived from ErrlUserDetails used to create user detail data,
 *  derive a class from ErrlUserDetailsParser to parse user detail data
 */
#include <stdint.h>

namespace ERRORLOG
{

/**
 *  @brief Returns the uint64_t at the pointed to location in host byte order
 *
 *  @param[in] i_pUint64 Pointer to a uint64_t in network byte order
 *
 *  @return uint64_t The uint64_t at the pointed to location in host byte order
 */
inline uint64_t NTH_UINT64(const void* i_pUint64)
{
    return (ntohll(*(reinterpret_cast<const uint64_t*>(i_pUint64))));
}

/**
 *  @brief Returns the uint32_t at the pointed to location in host byte order
 *
 *  @param[in] i_pUint32 Pointer to a uint32_t in network byte order
 *
 *  @return uint32_t The uint32_t at the pointed to location in host byte order
 */
inline uint32_t NTH_UINT32(const void* i_pUint32)
{
    return (ntohl(*(reinterpret_cast<const uint32_t*>(i_pUint32))));
}

/**
 *  @brief Returns the uint16_t at the pointed to location in host byte order
 *
 *  @param[in] i_pUint16 Pointer to a uint16_t in network byte order
 *
 *  @return uint16_t The uint16_t at the pointed to location in host byte order
 */
inline uint16_t NTH_UINT16(const void* i_pUint16)
{
    return (ntohs(*(reinterpret_cast<const uint16_t*>(i_pUint16))));
}

/**
 *  @brief Returns the uint8_t at the pointed to location in host byte order
 *
 *  @param[in] i_pUint8 Pointer to a uint8_t in network byte order
 *
 *  @return uint8_t The uint8_t at the pointed to location in host byte order
 */
inline uint8_t NTH_UINT8(const void* i_pUint8)
{
    return (*(reinterpret_cast<const uint8_t*>(i_pUint8)));
}

/**
 *
 * @class ErrlUserDetailsParser
 *
 * This is an abstract base class that defines how user detail error log data
 * should be parsed. For every different format of user detail data, a
 * component should create a class that derives from this class.
 */
class ErrlUserDetailsParser
{
public:

    /**
     *  @brief Constructor
     *
     */
    ErrlUserDetailsParser()
    {
    }

    /**
     *  @brief Destructor
     *
     */
    virtual ~ErrlUserDetailsParser()
    {
    }

    /**
     *  @brief Parses user detail data from an error log
     *
     *  Parses the supplied user detail data and outputs info to i_parser. This
     *  is a pure virtual function that must be overridden by a concrete derived
     *  class that is specific to a particular component, version and subsection
     *
     *  @param  i_version
     *      Version of the data
     *
     *  @param  i_parse
     *      ErrlUsrParser object for outputting information
     *
     *  @param  i_pBuffer
     *      Pointer to buffer containing detail data
     *
     *  @param  i_buflen
     *      Length of the buffer
     *
     *  @return None
     *
     */
    virtual void parse(errlver_t i_version,
                       ErrlUsrParser & i_parser,
                       void * i_pBuffer,
                       const uint32_t i_buflen) const = 0;

private:

    // Disabled
    ErrlUserDetailsParser(const ErrlUserDetailsParser &);
    ErrlUserDetailsParser & operator=(const ErrlUserDetailsParser &);
};

}
#endif

