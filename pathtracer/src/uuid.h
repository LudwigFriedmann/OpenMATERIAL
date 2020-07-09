//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      uuid.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-06-25
/// @brief     Support for Universally Unique IDentifiers (UUIDs)

#ifndef UUID_H
#define UUID_H

#include <cstdint>
#include <random>
#include <string>

#include "exceptions.h"
#include "random.h"

#if defined(_WIN32)
	#include <cctype>
#endif

/// @brief Support for UUIDs according to RCC 4122
///
/// This class provides support for universally unique identifiers
/// according to RFC 4122.
///
/// A uuid is represented by a 128bit number. The 128bit number can be
/// converted to a string. The standard format of the string is to display the
/// hexadecimal characters in the form 8-4-4-4-12:
///
///     fd6fa828-d4aa-448a-910f-c497086d5492
///
/// When creating a new random uuid, the uuid will be created according to
/// version 4, variant 1 of RFC 4122.
///
/// With a 128bit uuid, to have a chance of p=1e-6 of finding a collission, you
/// have to generate 2.6e16 uuids. See Ref. [1] for more examples.
///
/// References:
/// - [1] https://en.wikipedia.org/w/index.php?title=Birthday_problem&oldid=912594355#Probability_table
class Uuid
{
private:
    /// Lower 64bit of the uuid
    uint64_t m_u64UuidLo;
    /// Upper 64bit of the uuid
    uint64_t m_u64UuidHi;

    /// @brief Convert hexadecimal character c to integer
    ///
    /// This function takes a single-digit hexadecimal number ('0'-'9', 'A'-'F') and
    /// returns the corresponding number. If c is not a valid hexadecimal character
    /// -1 is returned.
    ///
    /// Examples:
    ///     hex2int('2') => 2
    ///     hex2int('A') => 10
    ///     hex2int('X') => -1
    ///
    /// @param [in] c hexadecimal digit
    /// @retval number corresponding number (0-15) if c is a valid character
    /// @retval -1 otherwise
    static int hex2int(int c)
    {
        c = std::toupper(c);
        if (std::isdigit(c))
            return c - '0';
        else if (c >= 'A' && c <= 'F')
            return c - 'A' + 10;

        return -1;
    }

    /// @brief Convert number to hexadecimal character
    ///
    /// This function takes a number 0 <= c <= 15 and converts it to the
    /// corresponding hexadecimal character. If c<0 or c>15 -1 is returned.
    ///
    /// Examples:
    ///     int2hex(2) => '2'
    ///     int2hex(10) => 'A'
    ///     int2hex(20) => -1
    ///
    /// @param [in] c number (0<=c<=15)
    /// @retval hex hexadecimal character ('0'-'9','A'-'F') if 0<=c<=15
    /// @retval -1 otherwise
    static int int2hex(int c)
    {
        int map[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

        if (c >= 0 && c <= 15)
            return map[c];

        return -1;
    }

public:
    /// Create random UUID object according to RFC 4122 version 4 variant 1
    Uuid()
    {
        m_u64UuidLo = Random::getRandomU64();
        m_u64UuidHi = Random::getRandomU64();

        // The format of a RFC 4122 uuid is
        //
        //      xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
        //      \--m_u64UuidHi---/ \--m_u64UuidLo--/
        //
        // where M denotes the version (M=4) and the two higher bits of N
        // denote the variant (N=0b10xx).

        // Set version to 4:
        // Set the 4 bits corresponding to M to 0
        m_u64UuidHi &= ~((uint64_t(15)) << (3*4));
        // Set the 4 bits corresponding to M to 4
        m_u64UuidHi |= ((uint64_t(4)) << (3*4));

        // Set variant to 1 (this corresponds to 2=0b10):
        m_u64UuidLo |= (uint64_t(1) << 63);
        m_u64UuidLo &= ~(uint64_t(1) << 62);
    }

    /// Create Uuid object from two 64 bit integers
    //
    // The 64 high bits of the uuid are given by uuid_hi, and the 64 low bits
    // are given by uuid_hi.
    Uuid(uint64_t uuid_lo, uint64_t uuid_hi)
        : m_u64UuidLo(uuid_lo)
        , m_u64UuidHi(uuid_hi)
    {
    }

    /// Create a copy of the Uuid object rUuid
    Uuid(const Uuid& rUuid)
        : m_u64UuidLo(rUuid.m_u64UuidLo)
        , m_u64UuidHi(rUuid.m_u64UuidHi)
    {
    }

    /// @brief Create Uuid object from string
    ///
    /// When parsing, all minus signs (the character '-') as well as trailing
    /// characters are ignored. Reading is case insensitive. Therefore, the
    /// following strings yield the same uuid:
    ///
    ///     - fd6fa828-d4aa-448a-910f-c497086d5492: standard format
    ///     - fD6fA828-d4AA-448a-910f-c497086D5492: mixed upper/lower cases
    ///     - fd6fa828d4aa448a910fc497086d5492: without dashes
    ///     - fd6fa828-d4aa-448a-910f-c497086d5492FOOBAR: trailing characters are ignored
    ///
    /// If the string is too short or a non-hexadecimal character is found
    /// (except minus sign) is found, a ValueError is thrown.
    Uuid(const std::string& rcsUuid)
    {
        uint64_t u64UuidLo = 0, u64UuidHi = 0;

        int j = 0;
        for (size_t i = 0; i < rcsUuid.length() && j < 32; i++) {
            const char character = rcsUuid[i];

            // Ignore dashes
            if(character == '-')
                continue;

            const int c = hex2int(character);
            if(c < 0)
                throw ValueError(std::string("Unexpected character '") + character + "' in uuid");

            // c is a valid character...
            if (j < 16)
                u64UuidHi |= (uint64_t)c << 4 * (15 - j++);
            else // j >= 16
                u64UuidLo |= (uint64_t)c << 4 * (31 - j++);
        }

        if (j != 32)
            throw ValueError("Invalid format of uuid: too short");

        m_u64UuidLo = u64UuidLo;
        m_u64UuidHi = u64UuidHi;
    }

    /// Copy the less significant 64 bits of the UUID to uuid_lo, and the most
    /// significant 64 bits to uuid_hi
    void get(uint64_t& uuid_lo, uint64_t& uuid_hi) const
    {
        uuid_hi = m_u64UuidHi;
        uuid_lo = m_u64UuidLo;
    }

    /// Return the lower 64 bits of the uuid
    uint64_t getLo() const { return m_u64UuidLo; }

    /// Return the higher 64 bits of the uuid
    uint64_t getHi() const { return m_u64UuidHi; }

    /// Convert UUID to a hexadecimal string
    std::string toString() const
    {
        // 32 hexadecimal characters + 4 minus signs + final '\0' byte
        char s[37] = { 0 };

        int c, j = 0;
        for(int i = 0; i < 32; i++)
        {
            if (i < 16)
                c = (m_u64UuidHi >> (60 - 4 * i)) & 0xf;
            else
                c = (m_u64UuidLo >> (124 - 4 * i)) & 0xf;
            s[j++] = int2hex(c);

            // Add dashes '-' for readability
            if (j == 8 || j == 13 || j == 18 || j == 23)
                s[j++] = '-';
        }

        return std::string(s);
    }

    /// Overload comparison operator for UUID objects
    bool operator==(const Uuid& rUUID2) const
    {
        return m_u64UuidHi == rUUID2.m_u64UuidHi && m_u64UuidLo == rUUID2.m_u64UuidLo;
    }

    /// Overload comparison operator for UUID objects
    bool operator!=(const Uuid& rUUID2) const
    {
        return !(*this == rUUID2);
    }

    /// Overload addition operator: uuid + string
    std::string operator+(const std::string& crsString) const
    {
        return toString() + crsString;
    }

    /// Overload addition operator: string + uuid
    friend std::string operator+(const std::string& crsString, const Uuid& crUuid)
    {
        return crsString + crUuid.toString();
    }

    /// Overload "<<" operator
    friend std::ostream& operator<<(std::ostream& os, const Uuid& crUuid)
    {
        os << crUuid.toString();
        return os;
    }
};

/// @cond
namespace std
{
  template <>
  struct hash<Uuid>
  {
    std::size_t operator()(const Uuid &rUuid) const
    {
        return static_cast<std::size_t>(rUuid.getLo());
    }
  };
}
/// @endcond 
#endif // UUID_H

