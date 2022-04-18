#include "CMatrix.h"

int main(int argc, char* argv[])
{
    srand(time(NULL));

    mapParametrs mParametrs({{"threads",""},
                             {"columns",""},
                             {"rows",   ""},
                             {"method", ""}});

    if (!ParseCommandLine(mParametrs, argc, argv))
        return -1;

    Parametrs oParametrs;

    if (!oParametrs.Initialization(mParametrs))
        return -1;

    CMatrix oFirstMatrix    (oParametrs.m_unNumberColumns, oParametrs.m_unNumberRows);
    CMatrix oSecondMatrix   (oParametrs.m_unNumberColumns, oParametrs.m_unNumberRows);

    CMatrix oNewMatrix = CMatrix::MatrixSummation(oFirstMatrix, oSecondMatrix, oParametrs.m_unNumberThreads, oParametrs.m_enSummationMethod);

    if ((SummationMethodsRows == oParametrs.m_enSummationMethod && oNewMatrix.GetNumberRows() < 10) ||
         (SummationMethodsColumns == oParametrs.m_enSummationMethod && oNewMatrix.GetNumberColumns() < 10))
    {
        oFirstMatrix.Print();
        std::cout << std::endl;
        oSecondMatrix.Print();
        std::cout << std::endl;
        oNewMatrix.Print();
    }

    return 0;
}
