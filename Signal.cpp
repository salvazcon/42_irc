#include "Signal.hpp"

Signal::~Signal()
{
    //std::cout << "Signal destructor called" << std::endl;
}

Signal::Signal()
{
    //std::cout << "Signal constructor called" << std::endl;
}

bool Signal::signal = false;

Signal& Signal::operator=(const Signal &other)
{
	//std::cout << "Signal copy assignment operator called" << std::endl;
    this->signal = other.signal;
	return *this;
}

Signal::Signal(const Signal &cp)
{
	//std::cout << "Signal copy constructor called" << std::endl;
	*this = cp;
}

void Signal::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	signal = true;
}

bool Signal::getSignal()
{
	return signal;
}

void Signal::setSignal(bool signal)
{
	this->signal = signal;
}
