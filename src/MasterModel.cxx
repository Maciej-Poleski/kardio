/*
 * Copyright 2013  Maciej Poleski
 */

#include "MasterModel.hxx"

#include <fstream>
#include <string>
#include <regex>
#include <tuple>
#include <functional>

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/lexical_cast.hpp>

#include <QtCore/QDateTime>
#include <QtGui/QColor>
#include <QtGui/QImage>
#include <QtGui/QPainter>

#include "CompleteRecord.hxx"
#include "DeprecatedRecord.hxx"
#include "BuiltinNameToTimeTranslator.hxx"

MasterModel::MasterModel(const std::string& timezoneInfoFile, QObject* parent):
    QAbstractTableModel(parent)
{
    _tzDatabase.load_from_file(timezoneInfoFile);
    /* TODO:
     * Support selection of time zone
     */
    _timeZonePtr=_tzDatabase.time_zone_from_region("Europe/Warsaw");
}

int MasterModel::columnCount(const QModelIndex& parent) const
{
    return 4; // size of Record
}

int MasterModel::rowCount(const QModelIndex& parent) const
{
    return _records.size();
}

static int_fast32_t rankFromMaxRate(std::uint_fast16_t rate)
{
    if(rate<120)
        return 0;
    else if(rate<130)
        return 1;
    else if(rate<140)
        return 2;
    else if(rate<160)
        return 3;
    else
        return 4;
}

static int_fast32_t rankFromMinRate(std::uint_fast16_t rate)
{
    if(rate<80)
        return 0;
    else if(rate<85)
        return 1;
    else if(rate<90)
        return 2;
    else if(rate<100)
        return 3;
    else
        return 4;
}

static QColor colorFromRank(std::int_fast32_t rank)
{
    switch(rank)
    {
    case 0:
        return QColor::fromRgb(127,255,127);
    case 1:
        return QColor::fromRgb(0,255,0);
    case 2:
        return QColor::fromRgb(255,255,0);
    case 3:
        return QColor::fromRgb(255,0,0);
    default:
        return QColor::fromRgb(128,0,0);
    }
}

static QColor colorFromMaxRate(std::uint_fast16_t rate)
{
    return colorFromRank(rankFromMaxRate(rate));
}

static QColor colorFromMinRate(std::uint_fast16_t rate)
{
    return colorFromRank(rankFromMinRate(rate));
}

QVariant MasterModel::data(const QModelIndex& index, int role) const
{
    if(role==Qt::DisplayRole)
    {
        auto row=index.row();
        auto column=index.column();
        if(column==0)
        {
            return _records[row]->datetimeForGui();
        }
        else if(column==1)
        {
            return _records[row]->maxRate();
        }
        else if(column==2)
        {
            return _records[row]->minRate();
        }
        else if(column==3)
        {
            return _records[row]->pulse();
        }
        else
        {
            return "If you see it report bug";
        }
    }
    else if(role==Qt::DecorationRole)
    {
        auto row=index.row();
        auto column=index.column();
        if(column==0)
            return colorFromRank(std::max(
                                     rankFromMaxRate(_records[row]->maxRate()),
                                     rankFromMinRate(_records[row]->minRate())
                                 ));
        else if(column==1)
            return colorFromMaxRate(_records[row]->maxRate());
        else if(column==2)
            return colorFromMinRate(_records[row]->minRate());
        else if(column==3);
    }
    return QVariant();
}

QVariant MasterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Horizontal)
    {
        if(role==Qt::DisplayRole)
        {
            if(section==0)
            {
                return tr("date and time");
            }
            else if(section==1)
            {
                return tr("max rate");
            }
            else if(section==2)
            {
                return tr("min rate");
            }
            else if(section==3)
            {
                return tr("pulse");
            }
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

static bool isDate(const std::string &string)
{
    // DD.MM.YYYY:
    return string.length()==11 && string[2]=='.' && string[5]=='.'
           && string[10]==':';
}

static boost::gregorian::date parseDate(const std::string & date)
{
    using boost::lexical_cast;
    auto day=lexical_cast<uint_fast16_t>(date.substr(0,2));
    if(date[2]!='.')
        throw std::runtime_error("Bad date format: "+date);
    auto month=lexical_cast<uint_fast16_t>(date.substr(3,2));
    if(date[5]!='.')
        throw std::runtime_error("Bad date format: "+date);
    auto year=lexical_cast<uint_fast16_t>(date.substr(6,4));
    if(date[10]!=':')
        throw std::runtime_error("Bad date format: "+date);
    return boost::gregorian::date(year,month,day);
}

static bool isBlankLine(const std::string & line)
{
    return line.empty() || line=="\n";
}

static bool isMeasure(const std::string &string)
{
    static const std::regex measureRegex("^[0-9]{2}:[0-9]{2} [0-9]{1,3}/[0-9]{1,3} [0-9]{1,3}$",std::regex_constants::extended);
    std::smatch match;
    return std::regex_match(string,match,measureRegex);
}

static std::tuple<uint_fast16_t,uint_fast16_t,uint_fast16_t,uint_fast16_t,uint_fast16_t>
parseMeasure(const std::string &string)
{
    if(!isMeasure(string))
        throw std::runtime_error(string+" is not measure");
    static const std::regex measureRegex("^([0-9]{2}):([0-9]{2}) ([0-9]{1,3})/([0-9]{1,3}) ([0-9]{1,3})$",
                                         std::regex_constants::extended);
    std::smatch match;
    std::regex_match(string,match,measureRegex);
    using boost::lexical_cast;
    return std::make_tuple(
               lexical_cast<uint_fast16_t>(match[1].str()),
               lexical_cast<uint_fast16_t>(match[2].str()),
               lexical_cast<uint_fast16_t>(match[3].str()),
               lexical_cast<uint_fast16_t>(match[4].str()),
               lexical_cast<uint_fast16_t>(match[5].str())
           );
}

static bool isDeprecatedMeasure(const std::string &line)
{
    static const std::regex measureRegex("^[0-9]{1,3}/[0-9]{1,3} [0-9]{1,3} [0-9]{2}\\.[0-9]{2}\\.[0-9]{4} [^ ]+$",std::regex_constants::extended);
    std::smatch match;
    return std::regex_match(line,match,measureRegex);
}

static std::tuple<uint_fast16_t,uint_fast16_t,uint_fast16_t,uint_fast16_t,uint_fast16_t,uint_fast16_t,std::string>
parseDeprecatedMeasure(const std::string &line)
{
    if(!isDeprecatedMeasure(line))
        throw std::runtime_error(line+" is not measure");
    static const std::regex measureRegex("^([0-9]{1,3})/([0-9]{1,3}) ([0-9]{1,3}) ([0-9]{2})\\.([0-9]{2})\\.([0-9]{4}) ([^ ]+)$",
                                         std::regex_constants::extended);
    std::smatch match;
    std::regex_match(line,match,measureRegex);
    using boost::lexical_cast;
    return std::make_tuple(
               lexical_cast<uint_fast16_t>(match[1].str()),
               lexical_cast<uint_fast16_t>(match[2].str()),
               lexical_cast<uint_fast16_t>(match[3].str()),
               lexical_cast<uint_fast16_t>(match[4].str()),
               lexical_cast<uint_fast16_t>(match[5].str()),
               lexical_cast<uint_fast16_t>(match[6].str()),
               match[7].str()
           );
}

void MasterModel::loadFromFile(const std::string & filename)
{
    using namespace boost::local_time;
    beginResetModel();
    _records.clear();
    std::ifstream in(filename);
    std::string line;
    uint_fast32_t lineNumber=0;
    boost::gregorian::date currentDate(boost::date_time::not_a_date_time);
    std::shared_ptr<INameToTimeTranslator> nttTranslator(new BuiltinNameToTimeTranslator); // TODO: custom (GUI) time translator
    do
    {
        ++lineNumber;
        std::getline(in,line);
        if(isBlankLine(line))
        {
            currentDate=boost::gregorian::date(boost::date_time::not_a_date_time);
        }
        else if(isDeprecatedMeasure(line))
        {
            auto measure=parseDeprecatedMeasure(line);
            _records.emplace_back(new DeprecatedRecord(
                                      std::get<0>(measure),
                                      std::get<1>(measure),
                                      std::get<2>(measure),
                                      boost::gregorian::date(
                                          std::get<5>(measure),
                                          std::get<4>(measure),
                                          std::get<3>(measure)
                                      ),
                                      std::get<6>(measure),
                                      nttTranslator,
                                      _timeZonePtr
                                  ));
            currentDate=boost::gregorian::date(boost::date_time::not_a_date_time);
        }
        else if(isDate(line))
        {
            auto newDate=parseDate(line);
            if(!currentDate.is_not_a_date() && currentDate>newDate)
                throw std::runtime_error("Current date ("+
                                         boost::gregorian::to_iso_string(currentDate)+
                                         ") is after new date ("+
                                         boost::gregorian::to_iso_string(newDate)+
                                         ")."
                                         " It is not supported (now)");
            currentDate=parseDate(line);
        }
        else if(isMeasure(line))
        {
            if(currentDate.is_not_a_date())
                throw std::runtime_error("Current date is not set, but measure"
                                         " is provided at line "+std::to_string(lineNumber));
            auto measure=parseMeasure(line);
            _records.emplace_back(new CompleteRecord(currentDate,measure,_timeZonePtr));
        }
        else
        {
            std::clog<<"Unexpected line: >>>"<<line<<"<<<\n";
        }
    } while(!in.eof());
    endResetModel();
}

static std::size_t getTimeSpan(
    const std::vector<std::shared_ptr<Record>> &records,
    const std::size_t minutesPerPixel)
{
    boost::local_time::local_date_time startTime=records.front()->localDatetime();
    auto startDate=startTime.local_time().date();

    boost::local_time::local_date_time endTime=records.back()->localDatetime();
    auto endDate=endTime.local_time().date()+boost::gregorian::days(2); // TODO: improve aproximation

    auto duration=endTime-startTime+boost::posix_time::hours(48);
    return duration.total_seconds()/60/minutesPerPixel;
}

static void drawHorizontalLines(QImage &image,const std::size_t spacing,
                                const std::size_t limit,
                                std::function<std::size_t(std::size_t)> heightMapper)
{
    QPainter painter(&image);
    painter.setPen(Qt::lightGray);
    for(std::size_t h=spacing; h<limit; h+=spacing)
    {
        painter.drawLine(0,heightMapper(h),image.width(),heightMapper(h));
    }
}

QImage MasterModel::getChartAsImage() const
{
    const std::size_t minutesPerPixel=15;
    const std::size_t width=getTimeSpan(_records,minutesPerPixel);
    const std::size_t height=400;
    const std::size_t heightScale=2;
    const auto mapHeight=[height,heightScale] (const std::size_t h) {
        return height-1-h*heightScale;
    };
    QImage result(width,400,QImage::Format_ARGB32_Premultiplied);
    result.fill(Qt::white);
    drawHorizontalLines(result,10,200,mapHeight);
    QPainter painter(&result);
    boost::local_time::local_date_time currentTimeBarrier(
        _records[0]->localDatetime().local_time().date(),
        boost::posix_time::minutes(minutesPerPixel),
        _timeZonePtr,
        boost::local_time::local_date_time::EXCEPTION_ON_ERROR);
    std::size_t currentWidth=0;
    QPainterPath pulsePath;
    bool started=false;
    std::size_t density=0;
    for(auto i=_records.begin(),e=_records.end(); i!=e;)
    {
        if((*i)->localDatetime()<currentTimeBarrier)
        {
            // Include this record in this point
            if(started)
                pulsePath.lineTo(currentWidth,mapHeight((*i)->pulse()));
            else
            {
                started=true;
                pulsePath.moveTo(currentWidth,mapHeight((*i)->pulse()));
            }
            ++i;
            ++density;
            if(density>2)
                std::cerr<<"Pulse chart is dense at "<<currentWidth<<" time: "<<(*i)->localDatetime()<<'\n';
        }
        else
        {
            // shift one pixel right and try again
            currentTimeBarrier+=boost::posix_time::minutes(minutesPerPixel);
            ++currentWidth;
            density=0;
        }
    }
    painter.drawPath(pulsePath.subtracted(QPainterPath()));
    return result;
}


#include "MasterModel.moc"
