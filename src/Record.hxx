/*
 * Copyright 2013  Maciej Poleski
 */

#ifndef RECORD_H
#define RECORD_H

#include <boost/date_time/local_time/local_date_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/serialization/access.hpp>

class QVariant;

class Record
{
    friend boost::serialization::access;
public:
    Record(uint_fast16_t maxRate,uint_fast16_t minRate, uint_fast16_t pulse);

    const std::string datetimeString() const;

    const std::uint16_t maxRate() const;
    const std::uint16_t minRate() const;
    const std::uint16_t pulse() const;
    virtual const boost::local_time::local_date_time localDatetime() const=0;
    virtual const QVariant datetimeForGui() const=0;

    void setMaxRate(std::uint16_t maxRate);
    void setMinRate(std::uint16_t minRate);
    void setPulse(std::uint16_t pulse);

private:

    template<class Action>
    void serialize(Action & ar, const unsigned int version)
    {
        ar & _maxRate & _minRate & _pulse;
    }

private:
    std::uint16_t _maxRate; // 2^16 should be enough for everybody
    std::uint16_t _minRate;
    std::uint16_t _pulse;
};

#endif // RECORD_H
