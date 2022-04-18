#ifndef CMATRIX_H
#define CMATRIX_H

#include "Common.h"

class CMatrix
{
public:
    CMatrix();
    CMatrix(unsigned int unColumns, unsigned int unRows, bool bZeroize = false);
    ~CMatrix();

    unsigned int GetNumberColumns() const;
    unsigned int GetNumberRows()    const;

    bool Empty() const;
    void Print() const;

    bool SizeEquivalent(const CMatrix& oMatrix) const;

    int* operator[] (const unsigned int unIndex) const;

    static CMatrix MatrixSummation(const CMatrix& oFirstMatrix, const CMatrix& oSecondMatrix, unsigned int unNumberThreads, SummationMethods enSummationMethod);

    static CMatrix MatrixSummationByRows( const CMatrix& oFirstMatrix, const CMatrix& oSecondMatrix, unsigned int unNumberThreads, MultithreadingMethods enMultithreadingMethod);
    static CMatrix MatrixSummationByRowsOMP(const CMatrix& oFirstMatrix, const CMatrix& oSecondMatrix, unsigned int unNumberThreads);
    static CMatrix MatrixSummationByRowsT(const CMatrix& oFirstMatrix, const CMatrix& oSecondMatrix, unsigned int unNumberThreads);

    static CMatrix MatrixSummationByColumns( const CMatrix& oFirstMatrix, const CMatrix& oSecondMatrix, unsigned int unNumberThreads, MultithreadingMethods enMultithreadingMethod);
    static CMatrix MatrixSummationByColumnsOMP(const CMatrix& oFirstMatrix, const CMatrix& oSecondMatrix, unsigned int unNumberThreads);
    static CMatrix MatrixSummationByColumnsT(const CMatrix& oFirstMatrix, const CMatrix& oSecondMatrix, unsigned int unNumberThreads);

    static CMatrix MatrixSummationAdvanced(const CMatrix& oFirstMatrix, const CMatrix& oSecondMatrix, unsigned int unNumberThreads);

private:
    void Clear();

    int **m_pValues;
    unsigned int m_unNumberColumns;
    unsigned int m_unNumberRows;
};

#endif // CMATRIX_H
