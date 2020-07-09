//
// Copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//

/// @file      utils.h
/// @author    Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @copyright (C) 2019, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
/// @date      2019-04-24
/// @brief     Utility and helper functions

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

namespace utils
{
    namespace path
    {
        char getFileSeparator();
        std::string basename(const std::string &pathname);
        std::string dirname(const std::string &rPathname);
        std::string fileextension(const std::string &pathname);
        bool fileExists(const std::string &crsFileName);
    }

    namespace string
    {
        std::vector<std::string> split(const std::string &s, const std::string delimiter, int maxsplit=-1);

        bool strcaseequal(const std::string& a, const std::string& b);
        std::string rstrip(const std::string &s);
        std::string lstrip(const std::string &s);
        std::string strip(const std::string &s);
    }
}

#endif // UTILS_H

