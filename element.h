#ifndef ELEMENT_H
#define ELEMENT_H

#include <set>
#include <functional>

using namespace std;

class Element
{
public:

    struct MinCostCompare
    {
        bool operator()(const Element* lhs, const Element* rhs) const
        {
            /*  Assim, o heap da std::priority_queue fica com o menor valor. */
            return lhs->m_cost > rhs->m_cost;
        }
    };

    struct MaxCostCompare
    {
        bool operator()(const Element* lhs, const Element* rhs) const
        {
            /*  Assim, o heap da std::priority_queue fica com o maior valor. */
            return lhs->m_cost < rhs->m_cost;
        }
    };

    Element();
    ~Element();


    void addStock( int stock ) { m_stocks.insert( stock ); m_hash+=(stock+1)*(stock+2); }
    set<int>& getStocks() { return m_stocks; }
    uint getStocksSize() { return m_stocks.size(); }
    string getStocksString();

    ulong hash() { return m_hash; }

    double getCost() { return m_cost; }
    void setCost( set<int>& stocks, const double **pweight, int k, double maxVal ) { m_cost = calcCost( stocks, pweight, k, maxVal ); }

private:
    set<int> m_stocks;
    double m_cost;
    ulong m_hash;

    double calcCost( set<int>& stocks, const double **pweight, int k, double maxVal );
    double calcG( set<int>& stocks, const double **pweight );
};
#endif // ELEMENT_H
