#ifndef H_D4AFACD5_55EE_49C4_8AF5_B839F87EA452
#define H_D4AFACD5_55EE_49C4_8AF5_B839F87EA452

template<typename Container, typename ValueType>
class RecentValues
{
public:
    RecentValues(Container& container, int maxSize)
        : m_container{container}
        , m_maxSize{maxSize}
    {}

    void addValue(const ValueType& newValue)
    {
        ValueType prevValue = m_container.get(0);
        for (int i = 1; i < m_maxSize; i++) {
            if (!m_container.isValid(prevValue)) {
                break;
            }
            if (newValue == prevValue) {
                continue;
            }
            const auto& tempValue = m_container.get(i);
            m_container.set(i, prevValue);
            prevValue = tempValue;
        }
        m_container.set(0, newValue);
    }

private:
    Container& m_container;
    int m_maxSize = 0;
};

#endif // CIRCULARBUFFER_H
