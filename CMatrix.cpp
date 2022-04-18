#include "CMatrix.h"

#include <iostream>
#include <chrono>
#include <cmath>

#include <thread>
#include <omp.h>

typedef std::chrono::time_point<std::chrono::steady_clock> Time;

#define TIMENOW std::chrono::steady_clock::now()

CMatrix::CMatrix() : m_pValues(nullptr), m_unNumberColumns(0), m_unNumberRows(0) {};

CMatrix::CMatrix(unsigned int unColumns, unsigned int unRows, bool bZeroize)
{
    if (0 == unColumns || 0 == unRows) return;

    m_unNumberColumns = unColumns;
    m_unNumberRows    = unRows;

    m_pValues = new int*[unRows];

    for (unsigned int unRowIndex = 0; unRowIndex < unRows; ++unRowIndex)
    {
        m_pValues[unRowIndex] = new int[unColumns];

        for (unsigned int unColumntIndex = 0; unColumntIndex < unColumns; ++unColumntIndex)
            m_pValues[unRowIndex][unColumntIndex] = (bZeroize) ? 0 : rand() % 100 - 50;
    }

}

CMatrix::~CMatrix()
{
    Clear();
}

unsigned int CMatrix::GetNumberColumns() const
{
    return m_unNumberColumns;
}

unsigned int CMatrix::GetNumberRows() const
{
    return  m_unNumberRows;
}

bool CMatrix::Empty() const
{
    return  (nullptr == m_pValues || 0 == m_unNumberColumns || 0 == m_unNumberRows);
}

void CMatrix::Print() const
{
    if (Empty()) return;

    for (unsigned int unRowIndex = 0; unRowIndex < m_unNumberRows; ++unRowIndex)
    {
        for (unsigned int unColumnIndex = 0; unColumnIndex < m_unNumberColumns; ++unColumnIndex)
            std::cout << m_pValues[unRowIndex][unColumnIndex] << " ";

        std::cout << std::endl;
    }
}

bool CMatrix::SizeEquivalent(const CMatrix &oMatrix) const
{
    return (m_unNumberRows == oMatrix.m_unNumberRows && m_unNumberColumns == oMatrix.m_unNumberColumns);
}

int *CMatrix::operator[](const unsigned int unIndex) const
{
    if (Empty()) return nullptr;

    return m_pValues[unIndex];
}

CMatrix CMatrix::MatrixSummation(const CMatrix &oFirstMatrix, const CMatrix &oSecondMatrix, unsigned int unNumberThreads, SummationMethods enSummationMethod)
{
    if (oFirstMatrix.Empty() || oSecondMatrix.Empty() || !oFirstMatrix.SizeEquivalent(oSecondMatrix) || 0 == unNumberThreads)
        return CMatrix();

    switch (enSummationMethod)
    {
        case SummationMethodsRows:      MatrixSummationByRows(oFirstMatrix, oSecondMatrix, unNumberThreads, MultithreadingMethodsOpenMP); return MatrixSummationByRows(oFirstMatrix, oSecondMatrix, unNumberThreads, MultithreadingMethodsThreads);
        case SummationMethodsColumns:   MatrixSummationByColumns(oFirstMatrix, oSecondMatrix, unNumberThreads, MultithreadingMethodsOpenMP); return MatrixSummationByColumns(oFirstMatrix, oSecondMatrix, unNumberThreads, MultithreadingMethodsThreads);
        case SummationMethodsAdvanced:  return MatrixSummationAdvanced  (oFirstMatrix, oSecondMatrix, unNumberThreads);
        case SummationMethodsUnknown:
        default: break;
    }

    return CMatrix();
}

CMatrix CMatrix::MatrixSummationByRows(const CMatrix &oFirstMatrix, const CMatrix &oSecondMatrix, unsigned int unNumberThreads, MultithreadingMethods enMultithreadingMethod)
{
    if (MultithreadingMethodsOpenMP == enMultithreadingMethod)
        return MatrixSummationByRowsOMP(oFirstMatrix, oSecondMatrix, unNumberThreads);
    if (MultithreadingMethodsThreads == enMultithreadingMethod)
        return MatrixSummationByRowsT(oFirstMatrix, oSecondMatrix, unNumberThreads);

    return CMatrix();
}

CMatrix CMatrix::MatrixSummationByRowsOMP(const CMatrix &oFirstMatrix, const CMatrix &oSecondMatrix, unsigned int unNumberThreads)
{
    CMatrix oNewMatrix(oFirstMatrix.GetNumberColumns(), oFirstMatrix.GetNumberRows(), true);

    double dFirstTime = omp_get_wtime();

    unsigned int unRowIndex, unColumnIndex;
    #pragma omp parallel for private(unRowIndex, unColumnIndex) shared(oFirstMatrix, oSecondMatrix) num_threads(unNumberThreads)
    for (unRowIndex = 0; unRowIndex < oFirstMatrix.GetNumberRows(); ++unRowIndex)
        for (unColumnIndex = 0; unColumnIndex < oFirstMatrix.GetNumberColumns(); ++unColumnIndex)
            oNewMatrix[unRowIndex][unColumnIndex] = oFirstMatrix[unRowIndex][unColumnIndex] + oSecondMatrix[unRowIndex][unColumnIndex];

    double dFinishTime = omp_get_wtime();

    printf("Execution time of matrix addition by rows using OpenMP = %f\n", (dFinishTime - dFirstTime));

    return oNewMatrix;
}

CMatrix CMatrix::MatrixSummationByRowsT(const CMatrix &oFirstMatrix, const CMatrix &oSecondMatrix, unsigned int unNumberThreads)
{
    CMatrix oNewMatrix(oFirstMatrix.GetNumberColumns(), oFirstMatrix.GetNumberRows(), true);

    unsigned int unExactNumberThreads       = std::min(unNumberThreads, oFirstMatrix.GetNumberRows());
    unsigned int unNumberSelectedElements   = std::ceil((double)oFirstMatrix.GetNumberRows() / (double)unExactNumberThreads);

    std::thread arThreads[unExactNumberThreads];

    for (unsigned int unThreadIndex = 0; unThreadIndex < unExactNumberThreads; ++unThreadIndex)
    {
        arThreads[unThreadIndex] = std::thread([&oFirstMatrix, &oSecondMatrix, &oNewMatrix, unThreadIndex, &unNumberSelectedElements]()
                                                {
                                                    for (unsigned int unRowIndex = unThreadIndex * unNumberSelectedElements; unRowIndex < std::min((unThreadIndex + 1) * unNumberSelectedElements, oFirstMatrix.GetNumberRows()); ++unRowIndex)
                                                        for (unsigned int unColumnIndex = 0; unColumnIndex < oFirstMatrix.GetNumberColumns(); ++unColumnIndex)
                                                            oNewMatrix[unRowIndex][unColumnIndex] = oFirstMatrix[unRowIndex][unColumnIndex] + oSecondMatrix[unRowIndex][unColumnIndex];
                                                });
    }

    Time oTimeStart = TIMENOW;

    for (std::thread& oThread : arThreads)
        oThread.join();

    Time oTimeEnd = TIMENOW;

    std::chrono::nanoseconds oDifference = std::chrono::duration_cast<std::chrono::nanoseconds>(oTimeEnd - oTimeStart);

    printf("Execution time of matrix addition by rows using std::thread = %f\n", (double)(oDifference.count() / 1000000000.0f));

    return oNewMatrix;
}

CMatrix CMatrix::MatrixSummationByColumns(const CMatrix &oFirstMatrix, const CMatrix &oSecondMatrix, unsigned int unNumberThreads, MultithreadingMethods enMultithreadingMethod)
{
    if (MultithreadingMethodsOpenMP == enMultithreadingMethod)
        return MatrixSummationByColumnsOMP(oFirstMatrix, oSecondMatrix, unNumberThreads);
    if (MultithreadingMethodsThreads == enMultithreadingMethod)
        return MatrixSummationByColumnsT(oFirstMatrix, oSecondMatrix, unNumberThreads);

    return CMatrix();
}

CMatrix CMatrix::MatrixSummationByColumnsOMP(const CMatrix &oFirstMatrix, const CMatrix &oSecondMatrix, unsigned int unNumberThreads)
{
    CMatrix oNewMatrix(oFirstMatrix.GetNumberColumns(), oFirstMatrix.GetNumberRows(), true);

    double dFirstTime = omp_get_wtime();

    unsigned int unRowIndex, unColumnIndex;
    #pragma omp parallel for private(unRowIndex, unColumnIndex) shared(oFirstMatrix, oSecondMatrix) num_threads(unNumberThreads)
    for (unColumnIndex = 0; unColumnIndex < oFirstMatrix.GetNumberColumns(); ++unColumnIndex)
        for (unRowIndex = 0; unRowIndex < oFirstMatrix.GetNumberRows(); ++unRowIndex)
            oNewMatrix[unRowIndex][unColumnIndex] = oFirstMatrix[unRowIndex][unColumnIndex] + oSecondMatrix[unRowIndex][unColumnIndex];

    double dFinishTime = omp_get_wtime();

    printf("Execution time of matrix addition by columns using OpenMP = %f\n", (dFinishTime - dFirstTime));

    return oNewMatrix;
}

CMatrix CMatrix::MatrixSummationByColumnsT(const CMatrix &oFirstMatrix, const CMatrix &oSecondMatrix, unsigned int unNumberThreads)
{
    CMatrix oNewMatrix(oFirstMatrix.GetNumberColumns(), oFirstMatrix.GetNumberRows(), true);

    unsigned int unExactNumberThreads       = std::min(unNumberThreads, oFirstMatrix.GetNumberColumns());
    unsigned int unNumberSelectedElements   = std::ceil((double)oFirstMatrix.GetNumberColumns() / (double)unExactNumberThreads);

    std::thread arThreads[unExactNumberThreads];

    for (unsigned int unThreadIndex = 0; unThreadIndex < unExactNumberThreads; ++unThreadIndex)
    {
        arThreads[unThreadIndex] = std::thread([&oFirstMatrix, &oSecondMatrix, &oNewMatrix, unThreadIndex, &unNumberSelectedElements]()
                                                {
                                                    for (unsigned int unColumnIndex = unThreadIndex * unNumberSelectedElements; unColumnIndex < std::min((unThreadIndex + 1) * unNumberSelectedElements, oFirstMatrix.GetNumberColumns()); ++unColumnIndex)
                                                        for (unsigned int unRowIndex = 0; unRowIndex < oFirstMatrix.GetNumberRows(); ++unRowIndex)
                                                            oNewMatrix[unRowIndex][unColumnIndex] = oFirstMatrix[unRowIndex][unColumnIndex] + oSecondMatrix[unRowIndex][unColumnIndex];
                                                });
    }

    Time oTimeStart = TIMENOW;

    for (std::thread& oThread : arThreads)
        oThread.join();

    Time oTimeEnd = TIMENOW;

    std::chrono::nanoseconds oDifference = std::chrono::duration_cast<std::chrono::nanoseconds>(oTimeEnd - oTimeStart);

    printf("Execution time of matrix addition by columns using std::thread = %f\n", (double)(oDifference.count() / 1000000000.0f));

    return oNewMatrix;
}

CMatrix CMatrix::MatrixSummationAdvanced(const CMatrix &oFirstMatrix, const CMatrix &oSecondMatrix, unsigned int unNumberThreads)
{
    CMatrix oNewMatrix(oFirstMatrix.GetNumberColumns(), oFirstMatrix.GetNumberRows(), true);

    unsigned int unNumberColumns        = oFirstMatrix.GetNumberColumns();
    unsigned int unNumberRows           = oFirstMatrix.GetNumberRows();
    unsigned int unNumberAllElements    = unNumberColumns * unNumberRows;

    unsigned int unRowIndex, unColumnIndex;

    double dFirstTime = omp_get_wtime();

    #pragma omp parallel for private(unRowIndex, unColumnIndex) shared(oFirstMatrix, oSecondMatrix) num_threads(unNumberThreads)
    for (unsigned int unIndex = 0; unIndex < unNumberAllElements; ++unIndex)
    {
        unRowIndex     = (unsigned int)(unIndex / unNumberColumns);
        unColumnIndex  = (unsigned int)(unIndex % unNumberRows);
        oNewMatrix[unRowIndex][unColumnIndex] = oFirstMatrix[unRowIndex][unColumnIndex] + oSecondMatrix[unRowIndex][unColumnIndex];
    }

    double dFinishTime = omp_get_wtime();

    printf("Execution time of matrix addition with sequential addition = %f\n", (dFinishTime - dFirstTime));

    return oNewMatrix;
}

void CMatrix::Clear()
{
    if (nullptr == m_pValues) return;

    for (unsigned int unIndex = 0; unIndex < m_unNumberRows; ++unIndex)
        delete [] m_pValues[unIndex];

    delete [] m_pValues;
    m_pValues = nullptr;
}
