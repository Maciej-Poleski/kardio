/*
 * Copyright 2013  Maciej Poleski
 */

#ifndef MASTERMODEL_H
#define MASTERMODEL_H

#include <cstdint>
#include <vector>
#include <memory>

#include <boost/date_time/local_time/local_date_time.hpp>
#include <boost/date_time/local_time/tz_database.hpp>
#include <boost/date_time/local_time/local_time_types.hpp>

#include <QtCore/QAbstractTableModel>

#include "Record.hxx"

class QImage;

class MasterModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MasterModel(const std::string & timezoneInfoFile, QObject* parent = 0);
    MasterModel(const MasterModel& ) = delete;
    MasterModel(MasterModel &&) = delete;

    void loadFromFile(const std::string& filename);

    QImage getChartAsImage() const;

    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    std::vector<std::shared_ptr<Record>> _records;
    boost::local_time::tz_database _tzDatabase;
    boost::local_time::time_zone_ptr _timeZonePtr;
};

#endif // MASTERMODEL_H
