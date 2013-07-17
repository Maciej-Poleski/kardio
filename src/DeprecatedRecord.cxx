/*
 * Copyright 2013  Maciej Poleski
 */

#include "DeprecatedRecord.hxx"

#include <QtCore/QVariant>
#include <QtCore/QDate>

#include "CompleteRecord.hxx"

DeprecatedRecord::DeprecatedRecord(
    uint_fast16_t maxRate, uint_fast16_t minRate, uint_fast16_t pulse,
    const boost::gregorian::date& date, const std::string& nameOfTime,
    std::shared_ptr<INameToTimeTranslator const> nameToTimeTranslator,
    const boost::local_time::time_zone_ptr& tzPtr
):
    Record(maxRate, minRate, pulse), _date(date), _nameOfTime(nameOfTime),
    _nameToTimeTranslator(nameToTimeTranslator), _tzPtr(tzPtr)
{

}

const CompleteRecord DeprecatedRecord::toCompleteRecord() const
{
    auto time=_nameToTimeTranslator->timeFromName(_nameOfTime);
    return CompleteRecord(_date,
                          std::make_tuple(
                              std::get<0>(time),
                              std::get<1>(time),
                              maxRate(),
                              minRate(),
                              pulse()
                          ),
                          _tzPtr);
}

DeprecatedRecord::operator CompleteRecord() const
{
    return toCompleteRecord();
}

const boost::local_time::local_date_time DeprecatedRecord::localDatetime() const
{
    CompleteRecord record(toCompleteRecord());
    return record.localDatetime();
}

const QVariant DeprecatedRecord::datetimeForGui() const
{
    return QDate(_date.year(),_date.month(),_date.day()).toString(Qt::SystemLocaleDate) +" "+
           QString::fromStdString(_nameOfTime);
}
