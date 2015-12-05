#ifndef MEDIATOR_EVENT_H
#define MEDIATOR_EVENT_H

#include <list>
#include <functional>

template<typename retval, typename... params>
class event
{
private:
    std::list<std::function<retval(params...)>> listeners;
public:
    void connect(std::function<retval(params...)> listener)
    {
        listeners.push_back(listener);
    }

    std::list<retval> raise(params... parameters)
    {
        std::list<retval> result_data;
        for (std::function<retval(params...)> &listener : listeners)
        {
            result_data.push_back(listener(parameters...));
        }
        return result_data;
    }
};


#endif //MEDIATOR_EVENT_H
