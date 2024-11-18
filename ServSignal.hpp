#ifndef  SERVSIGNAL_HPP
# define  SERVSIGNAL_HPP

#include <iostream>
#include <string>

class ServSignal
{
    protected:

    private:
        static bool Signal;

    public:
        ~ServSignal(void);
        ServSignal(void);
        ServSignal(const ServSignal &cp);
		ServSignal& operator=(const ServSignal &other);
        static void SignalHandler(int signum);
        void setSignal(bool Signal);
        bool getSignal();
};

#endif
