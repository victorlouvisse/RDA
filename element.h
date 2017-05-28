#ifndef ELEMENT_H
#define ELEMENT_H

#include <set>
#include <functional>

using namespace std;

class Element
{
public:
    bool operator()(const Element* lhs, const Element* rhs) const
    {
        /*  Assim, o heap da std::priority_queue fica com o menor valor. */
        return lhs->m_cost > rhs->m_cost;
    }

    Element();

    ~Element();

    void addStock( int stock ) { m_stocks.insert( stock ); }
    set<int>& getStocks() { return m_stocks; }
    uint getStocksSize() { return m_stocks.size(); }
    string getStocksString();


    double getCost() { return m_cost; }
    void setCost( set<int>& stocks, const double **pweight, int k, double minVal ) { m_cost = calcCost( stocks, pweight, k, minVal ); }

private:
    set<int> m_stocks;
    double m_cost;

    double calcCost( set<int>& stocks, const double **pweight, int k, double minVal );
    double calcG( set<int>& stocks, const double **pweight );
};
#endif // ELEMENT_H
