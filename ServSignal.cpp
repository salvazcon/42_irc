#include "ServSignal.hpp"

ServSignal::~ServSignal()
{
    std::cout << "ServSignal destructor called" << std::endl;
}

ServSignal::ServSignal()
{
    std::cout << "ServSignal constructor called" << std::endl;
}

bool ServSignal::Signal = false;

ServSignal& ServSignal::operator=(const ServSignal &other)
{
    this->Signal = other.Signal;
	std::cout << "ServSignal copy assignment operator called" << std::endl;
	return *this;
}

ServSignal::ServSignal(const ServSignal &cp)
{
	std::cout << "ServSignal copy constructor called" << std::endl;
	*this = cp;
}

void ServSignal::SignalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Signal = true;
}

bool ServSignal::getSignal()
{
	return Signal;
}

void ServSignal::setSignal(bool Signal)
{
	this->Signal = Signal;
}
