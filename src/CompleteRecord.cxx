/*
 * Copyright 2013  Maciej Poleski
 */

#include "CompleteRecord.hxx"

#include <QtCore/QDateTime>
#include <QtCore/QVariant>

CompleteRecord::CompleteRecord(
    const boost::gregorian::date& date,
    const std::tuple< uint_fast16_t, uint_fast16_t, uint_fast16_t,
    uint_fast16_t, uint_fast16_t >& data,
    const boost::local_time::time_zone_ptr& tzPtr):
    Record(std::get<2>(data),std::get<3>(data),std::get<4>(data)),
    _localDatetime(
        date,
        boost::posix_time::hours(std::get<0>(data))+
        boost::posix_time::minutes(std::get<1>(data)),
        tzPtr,
        boost::local_time::local_date_time::EXCEPTION_ON_ERROR
    )
{

}

const boost::local_time::local_date_time CompleteRecord::localDatetime() const
{
    return _localDatetime;
}

void CompleteRecord::setLocalDatetime(const boost::local_time::local_date_time& ldt)
{
    _localDatetime=ldt;
}

const QVariant CompleteRecord::datetimeForGui() const
{
    return QDateTime(
               QDate(_localDatetime.local_time().date().year(),_localDatetime.local_time().date().month(),
                     _localDatetime.local_time().date().day()),
               QTime(_localDatetime.local_time().time_of_day().hours(),
                     _localDatetime.local_time().time_of_day().minutes())
           );
}
