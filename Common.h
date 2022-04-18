#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <cstring>
#include <regex>
#include <map>

typedef std::map<std::string, std::string>  mapParametrs;
typedef std::pair<std::string, std::string> pairParametr;

enum SummationMethods
{
    SummationMethodsUnknown,
    SummationMethodsRows,
    SummationMethodsColumns,
    SummationMethodsAdvanced
};

enum MultithreadingMethods
{
    MultithreadingMethodsOpenMP,
    MultithreadingMethodsThreads
};

struct Parametrs
{
    unsigned int    m_unNumberThreads;
    unsigned int    m_unNumberColumns;
    unsigned int    m_unNumberRows;
    SummationMethods  m_enSummationMethod;

    bool Initialization(const mapParametrs& mParametrs)
    {
        bool bFail;

        m_unNumberThreads = m_unNumberColumns = m_unNumberRows = 0;

        for (const pairParametr& oParametr : mParametrs)
        {
            bFail = false;
            if ("threads" == oParametr.first)
            {
                std::stringstream oConverter(oParametr.second);
                if (!(oConverter >> m_unNumberThreads))
                    bFail = true;
            }
            else if ("columns" == oParametr.first)
            {
                std::stringstream oConverter(oParametr.second);
                if (!(oConverter >> m_unNumberColumns))
                    bFail = true;
            }
            else if ("rows" == oParametr.first)
            {
                std::stringstream oConverter(oParametr.second);
                if (!(oConverter >> m_unNumberRows))
                    bFail = true;
            }
            else if ("method" == oParametr.first)
            {
                if ("rows" == oParametr.second)         m_enSummationMethod = SummationMethodsRows;
                else if ("columns" == oParametr.second) m_enSummationMethod = SummationMethodsColumns;
                else if ("advanced" == oParametr.second)m_enSummationMethod = SummationMethodsAdvanced;
                else                                    m_enSummationMethod = SummationMethodsUnknown;
            }

            if (bFail)
            {
                printf("It is not possible to convert values from the \"'%c'=%s\" faild\n", oParametr.first[0], oParametr.second.c_str());
                return false;
            }
        }
        return true;
    }
};

static bool CheckParametrs(const mapParametrs& mParametrs)
{
    bool bFail = false;
    for (const pairParametr& oPair : mParametrs)
    {
        if (oPair.second.empty())
        {
            std::cout << "Parameter \"" << oPair.first << "\" not defined" << std::endl;
            bFail = true;
        }
    }

    return !bFail;
}

static bool ParseCommandLine(mapParametrs& mParametrs, int argc, char* argv[])
{
    if (mParametrs.empty() ||  argc < 4)
        return false;

    for (unsigned int unIndex = 1; unIndex < (unsigned int)argc; ++unIndex)
    {
        if (strlen(argv[unIndex]) < 2)
            continue;

        std::string sParametr(argv[unIndex]);

        std::regex oRegex("^(-+)([a-zA-Z=0-9]+)");

        if (std::regex_match(sParametr, oRegex) != true)
            continue;

        size_t unFirstPosition = sParametr.find_first_not_of('-');

        if (std::string::npos == unFirstPosition)
            continue;

        sParametr = sParametr.substr(unFirstPosition, sParametr.length() - 1);

        size_t unSecondPosition = sParametr.find('=');

        if (std::string::npos != unSecondPosition)
        {
            std::string sName  = sParametr.substr(0, unSecondPosition);
            std::string sValue = sParametr.substr(unSecondPosition + 1, sParametr.length() - 1);

            mapParametrs::iterator oPosition = mParametrs.find(sName);

            if (mParametrs.end() == oPosition)
            {
                std::cout << "Unknown parameter: \"" << sName << "\"" << std::endl;
                continue;
            }

            (*oPosition).second = sValue;

            continue;
        }

        mapParametrs::iterator oPosition = mParametrs.find(sParametr);

        if (mParametrs.end() == oPosition)
        {
            std::cout << "Unknown parameter: \"" << sParametr << "\"" << std::endl;
            continue;
        }

        (*oPosition).second = argv[++unIndex];
    }

    return CheckParametrs(mParametrs);
}


#endif // COMMON_H
