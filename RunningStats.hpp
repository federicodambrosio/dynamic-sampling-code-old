#include <cmath>
#ifndef RUNNINGSTATS_HPP
#define RUNNINGSTATS_HPP
//
// This class is an implementation of the Welford Method: 
// Welford B, 1962, Note on a Method for Calculating Corrected Sums of Squares and Products, Technometrics 4 419–20
//
class RunningStats
{
public:
    RunningStats() : m_n(0), m_newM(0),m_newS(0),m_oldM(0),m_oldS(0) {

    }
    
    void Clear()
    {
        m_n = 0;
    }
    
    void Push(double x)
    {
        m_n++;
        
        // See Knuth TAOCP vol 2, 3rd edition, page 232
        if (m_n == 1)
        {
            m_oldM = m_newM = x;
            m_oldS = 0.0;
        }
        else
        {
            m_newM = m_oldM + (x - m_oldM)/m_n;
            m_newS = m_oldS + (x - m_oldM)*(x - m_newM);
            
            // set up for next iteration
            m_oldM = m_newM;
            m_oldS = m_newS;
        }
    }
    
    int NumDataValues() const
    {
        return m_n;
    }
    
    double Mean() const
    {
        return (m_n > 0) ? m_newM : 0.0;
    }
    
    double Variance() const
    {
        return ( (m_n > 1) ? m_newS/(m_n - 1) : 0.0 );
    }
    
    double StandardDeviation() const
    {
        return sqrt( Variance() );
    }
    
private:
    int m_n;
    double m_oldM, m_newM, m_oldS, m_newS;
};

#endif