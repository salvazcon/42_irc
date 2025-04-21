#include "Signal.hpp"

Signal::~Signal(){}

Signal::Signal(){}

bool Signal::signal = false;

Signal& Signal::operator=(const Signal &other)
{
    this->signal = other.signal;
	return *this;
}

Signal::Signal(const Signal &cp)
{
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
