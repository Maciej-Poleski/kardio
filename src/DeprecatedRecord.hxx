/*
 * Copyright 2013  Maciej Poleski
 */

#ifndef DEPRECATEDRECORD_H
#define DEPRECATEDRECORD_H

#include <string>
#include <memory>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/local_time/local_time.hpp>

#include "Record.hxx"
#include "INameToTimeTranslator.hxx"

class CompleteRecord;

class DeprecatedRecord : public Record
{
    friend boost::serialization::access;
public:
    DeprecatedRecord(uint_fast16_t maxRate, uint_fast16_t minRate,
                     uint_fast16_t pulse,const boost::gregorian::date& date,
                     const std::string &nameOfTime,
                     std::shared_ptr<INameToTimeTranslator const> nameToTimeTranslator,
                     const boost::local_time::time_zone_ptr& tzPtr
                    );
    
    virtual const boost::local_time::local_date_time localDatetime() const override;
    virtual const QVariant datetimeForGui() const override;
    
private:
    const CompleteRecord toCompleteRecord() const;
    explicit operator CompleteRecord() const;
    
    template<class Action>
    void serialize(Action &ar, unsigned int const version)
    {
        ar & boost::serialization::base_object<Record>(*this) & _date &
        _nameOfTime & _nameToTimeTranslator;
    }

private:
    boost::gregorian::date _date;
    std::string _nameOfTime;
    std::shared_ptr<INameToTimeTranslator const> _nameToTimeTranslator;
    boost::local_time::time_zone_ptr _tzPtr;
};

#endif // DEPRECATEDRECORD_H
