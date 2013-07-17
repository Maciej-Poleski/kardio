/*
 * Copyright 2013  Maciej Poleski
 */

#include "Record.hxx"

Record::Record(uint_fast16_t maxRate, uint_fast16_t minRate, uint_fast16_t pulse) :
    _maxRate(maxRate), _minRate(minRate), _pulse(pulse)
{

}

const std::string Record::datetimeString() const
{
    return localDatetime().to_string();
}

const uint16_t Record::maxRate() const
{
    return _maxRate;
}

const uint16_t Record::minRate() const
{
    return _minRate;
}

const uint16_t Record::pulse() const
{
    return _pulse;
}

void Record::setMaxRate(uint16_t maxRate)
{
    _maxRate=maxRate;
}

void Record::setMinRate(uint16_t minRate)
{
    _minRate=minRate;
}

void Record::setPulse(uint16_t pulse)
{
    _pulse=pulse;
}
