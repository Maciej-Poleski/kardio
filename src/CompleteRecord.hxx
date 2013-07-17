/*
 * Copyright 2013  Maciej Poleski
 */

#ifndef COMPLETERECORD_H
#define COMPLETERECORD_H

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

#include "Record.hxx"

class CompleteRecord : public Record
{
    friend boost::serialization::access;
public:
    CompleteRecord(const boost::gregorian::date& date,
                   const std::tuple< uint_fast16_t, uint_fast16_t,
                   uint_fast16_t, uint_fast16_t, uint_fast16_t >& data,
                   const boost::local_time::time_zone_ptr& tzPtr);

    virtual const boost::local_time::local_date_time localDatetime() const override;
    virtual const QVariant datetimeForGui() const override;

    void setLocalDatetime(const boost::local_time::local_date_time &ldt);

private:
    template<class Action>
    void serialize(Action &ar, unsigned int const version)
    {
        ar & boost::serialization::base_object<Record>(*this) & _localDatetime;
    }

private:
    /// Local time of measure
    boost::local_time::local_date_time _localDatetime;
};

#endif // COMPLETERECORD_H
