#include "ChannelTableModel.h"
#include <QDebug>
#include <QList>
#include <QtGui>

/*!
 Constructs an instance of a table model.
 */
ChannelTableModel::ChannelTableModel()
{
    mapSignalName_["1C"] = "L1 C/A"; // GPS L1 C/A 1575.42MHz
    mapSignalName_["1G"] = "L1 C/A"; // Glonass L1 C/A 1602.00MHz
    mapSignalName_["1B"] = "E1"; // Galileo E1 B/C 157542MHz
    mapSignalName_["B1"] = "B1"; // Beidou B1I 1561.098MHz

    mapSignalName_["2S"] = "L2C"; // GPS L2 L2CM 1227.60MHz
    mapSignalName_["2G"] = "L2 C/A"; // Glonass L2 C/A 1246.00MHz
    mapSignalName_["B3"] = "B3I"; // Beidou B3I 1268.520MHz

    mapSignalName_["5X"] = "E5a"; // Galileo E5a 1176.450MHz
    mapSignalName_["7X"] = "E5b"; // Galileo E5b 1207.140MHz
    mapSignalName_["L5"] = "L5";
    mapSignalName_["E6"] = "E6b"; // Galileo E6B 1278.75MHz

    columns_ = 10;
    bufferSize_ = BUFFER_SIZE_FOR_CHANNEL;

    checkTimer_.setInterval(500);
    connect(&checkTimer_,&QTimer::timeout,this,&ChannelTableModel::checkChannels);
    checkTimer_.start();
}

/*!
 Triggers a reset of the table model which causes the views to be repainted.
 */
void ChannelTableModel::update()
{
    beginResetModel();
    endResetModel();
}

///
/// \param parent (override)
/// \return 返回正在跟踪的通道总数/行数
int ChannelTableModel::rowCount(const QModelIndex &parent) const
{
    return (int)channels_.size();
}

///
/// \param parent (override)
/// \return 返回表格的列数
int ChannelTableModel::columnCount(const QModelIndex &parent) const
{
    return columns_;
}

///
/// \param index (override) 提供被查询的位置
/// \param role itemDataRole 对应不同类型的数据呈现模式
/// \return 返回对应的数据
QVariant ChannelTableModel::data(const QModelIndex &index, int role) const
{
    // 显示、悬停、图标
    if (role == Qt::DisplayRole ||
        role == Qt::ToolTipRole ||
        role == Qt::DecorationRole)
    {
        try{
            int channel_id = channelsId_.at(index.row());
            auto channel = channels_.at(channel_id);
            // 通道所属的卫星系统：GPS/BDS/Galileo
            QString channel_signal = channelsSignal_.at(channel_id);

            auto time_cbuf = channelsTime_.at(channel_id);
            auto i_cbuf = channelsI_.at(channel_id);
            auto q_cbuf = channelsQ_.at(channel_id);
            auto cn0_cbuf = channelsCn0_.at(channel_id);
            auto doppler_cbuf = channelsDoppler_.at(channel_id);

            // 为了下一步画图做准备
            QList<QVariant> channel_prompt_iq,channel_cn0,channel_doppler;
            // 将绘画点存入list中，方便绘图调用
            for (int i = 0; i < cn0_cbuf.size(); i++)
            {
                channel_prompt_iq << QPointF(i_cbuf.at(i),
                    q_cbuf.at(i));
                channel_cn0 << QPointF(time_cbuf.at(i), cn0_cbuf.at(i));
                channel_doppler << QPointF(time_cbuf.at(i),
                    doppler_cbuf.at(i));
            }

            if (role == Qt::DisplayRole)
            {
                switch (index.column())
                {
                case CHANNEL_ID:
                    return channel.channel_id;
                case CHANNEL_SYSTEM:
                    return channel_signal;
                case CHANNEL_PRN:
                    return channel.prn;
                case CHANNEL_ACQ_DOPPLER:
                    return channel.acq_doppler_hz;
                case CHANNEL_CONSTELLATION:
                    return channel_prompt_iq;
                case CHANNEL_CN0:
                    return channel_cn0;
                case CHANNEL_DOPPLER:
                    return channel_doppler;
                case CHANNEL_TOW:
                    return channel.tow_at_current_symbol_ms;
                case CHANNEL_TLM:
                    return channel.flag_valid_word;
                case CHANNEL_PSEUDORANGE:
                    return QString::number(channel.pseudorange_m,'f',3);
                }
            }
            else if (role == Qt::ToolTipRole)
            {
                switch (index.column())
                {
                case CHANNEL_CN0:
                    return cn0_cbuf.back();

                case CHANNEL_DOPPLER:
                    return doppler_cbuf.back();

                default:
                    return QVariant::Invalid;
                }
            }
            else if (index.column() == CHANNEL_SYSTEM)
            {
                if (channel.system == "G")
                    return QIcon(":/images/flag-us.png");
                else if (channel.system == "R")
                    return QIcon(":/images/flag-ru.png");
                else if (channel.system == "E")
                    return QIcon(":/images/flag-eu.png");
                else if (channel.system == "C")
                    return QIcon(":/images/flag-cn.png");
            }
        }
        catch (const std::exception &ex)
        {
            qDebug() << ex.what();
            return QVariant::Invalid;
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }
    return QVariant::Invalid;
}

///
/// \param section 第几列
/// \param orientation 标题朝向
/// \param role 显示模式
/// \return 返回表格第一行标题
QVariant ChannelTableModel::headerData(int section,Qt::Orientation orientation,
    int role) const
{
    if (role == Qt::DisplayRole){
        if (orientation == Qt::Horizontal){
            switch (section)
            {
            case CHANNEL_ID:
                return "CH";
            case CHANNEL_SYSTEM:
                return "Signal";
            case CHANNEL_PRN:
                return "PRN";
            case CHANNEL_ACQ_DOPPLER:
                return "ACQ Doppler [Hz]";
            case CHANNEL_CONSTELLATION:
                return "Constellation";
            case CHANNEL_CN0:
                return "C/N0 [dB-Hz]";
            case CHANNEL_DOPPLER:
                return "Doppler [Hz]";
            case CHANNEL_TOW:
                return "TOW [ms]";
            case CHANNEL_TLM:
                return "TLM";
            case CHANNEL_PSEUDORANGE:
                return "Pseudorange [m]";
            default:
                return QVariant::Invalid;
            }
        }
    }
    if (role == Qt::FontRole)
        return QFont("Calibri", 16, QFont::Medium);
    return QVariant::Invalid;
}

/*!
 Populates the internal data structures of the table model with the data of the \a stocks collection of GnssSynchro objects.
 Internally, this function calls populateChannel() on each individual GnssSynchro object in the collection.
 */
void ChannelTableModel::populateChannels(const std::vector<ChannelStruct>& vector)
{
    // 这里存在一个问题就是一次只发送一个channel的数据，并不是预想的所有通道
    // 通过observable全部发送过来
    if (!vector.empty()){
        for (const auto & i : vector){
            populateChannel(i);
        }
    }
}

/*!
 Populates the internal data structures of the table model with the data of the \a ch GnssSynchro object.
 本函数仅处理一个通道的数据
 */
void ChannelTableModel::populateChannel(const ChannelStruct& ch)
{
        int id =  ch.channel_id;
        // Channel is valid, now check if it exists in the map of channels.
        if (channels_.find(id) != channels_.end())
        {
            // Channel exists, now check if its PRN is the same.
            if (channels_.at(id).prn != ch.prn){
                // PRN has changed so reset the channel.
                clearChannel(id);
            }
        }

        // Check the size of the map of GnssSynchro objects before adding new data.
        size_t map_size = channels_.size();

        // update/add the new GnssSynchro object to the local channel.
        channels_[id] = ch;

        // Time.
        if (channelsTime_.find(id) == channelsTime_.end()){
            // Channel does not exist so make room for it.
            channelsTime_[id].resize(bufferSize_);
            channelsTime_[id].clear();
        }
        channelsTime_[id].push_back(ch.rx_time);

        // In-phase prompt component.
        if (channelsI_.find(id) == channelsI_.end()){
            channelsI_[id].resize(bufferSize_);
            channelsI_[id].clear();
        }
        channelsI_[id].push_back(ch.prompt_i);

        // Quadrature prompt component.
        if (channelsQ_.find(id) == channelsQ_.end()){
            channelsQ_[id].resize(bufferSize_);
            channelsQ_[id].clear();
        }
        channelsQ_[id].push_back(ch.prompt_q);

        // CN0.
        if (channelsCn0_.find(id) == channelsCn0_.end()){
            channelsCn0_[id].resize(bufferSize_);
            channelsCn0_[id].clear();
        }
        channelsCn0_[id].push_back(ch.cn0_db_hz);

        // Doppler.
        if (channelsDoppler_.find(id) == channelsDoppler_.end()){
            channelsDoppler_[id].resize(bufferSize_);
            channelsDoppler_[id].clear();
        }
        channelsDoppler_[id].push_back(ch.carrier_doppler_hz);

        // Signal name.
        channelsSignal_[id] = getSignalPrettyName(ch);

        // check if new channel(s) is added to local channels
        if (channels_.size() != map_size){
            // Map size has changed
            // so record the new channel number in the vector of channel IDs.
            channelsId_.push_back(id);
            emit addChannel(getName());
        }
}

/*!
 Clears the data of a single channel specified by \a ch_id from the table model.
 */
void ChannelTableModel::clearChannel(int ch_id)
{
    channelsId_.erase(std::remove(channelsId_.begin(), channelsId_.end(), ch_id),
        channelsId_.end());
    channels_.erase(ch_id);
    channelsSignal_.erase(ch_id);
    channelsTime_.erase(ch_id);
    channelsI_.erase(ch_id);
    channelsQ_.erase(ch_id);
    channelsCn0_.erase(ch_id);
    channelsDoppler_.erase(ch_id);
}

/*!
 Clears the data of all channels from the table model.
 */
void ChannelTableModel::clearChannels()
{
    channelsId_.clear();
    channels_.clear();
    channelsSignal_.clear();
    channelsTime_.clear();
    channelsI_.clear();
    channelsQ_.clear();
    channelsCn0_.clear();
    channelsDoppler_.clear();
}

/*!
 Gets the descriptive string formed by the combination of the GNSS system and signal name for a given \a ch GnssSynchro object.
 */
QString ChannelTableModel::getSignalPrettyName(const ChannelStruct& ch)
{
    QString system_name;
    QString name = QString::fromStdString(ch.system);
    if (!name.isEmpty())
    {
        if (name == "G")
            system_name = QStringLiteral("GPS");
        else if (name == "E")
            system_name = QStringLiteral("Galileo");
        else if (name == "C")
            system_name = QStringLiteral("BeiDou");
        else if (name == "R")
            system_name = QStringLiteral("Glonass");
        else
            system_name = QVariant::Invalid;

        if (mapSignalName_.find(ch.signal) != mapSignalName_.end())
            system_name.append(" ").append(mapSignalName_.at(ch.signal));
    }
    return system_name;
}

/*!
 Gets the number of columns of the table model.
 */
int ChannelTableModel::getColumns() const
{
    return columns_;
}

/*!
 Sets the size of the internal circular buffers that store the data of the table model.
 */
void ChannelTableModel::setBufferSize()
{
    QSettings settings;
    settings.beginGroup("Preferences_Dialog");
    int size = settings.value("buffer_size", BUFFER_SIZE_FOR_CHANNEL).toInt();
    settings.endGroup();

    bufferSize_ = size;
    clearChannels();
}

/*!
 Gets the id number of the channel occupying the queried \a row of the table model.
 */
int ChannelTableModel::getChannelId(int row)
{
    return channelsId_.at(row);
}

// 本函数实现了对失去跟踪锁定通道在显示界面的删除
void ChannelTableModel::checkChannels()
{
    if (channelsTime_.empty()) return;
    if (checkNum_.empty()){
        for (const auto& i : channelsTime_){
            checkNum_.insert(std::make_pair(i.first,i.second.back()));
        }
        return;
    }
    for (auto j : checkNum_){
        if (channelsTime_.find(j.first)==channelsTime_.end()){
            checkNum_.clear();
            return;
        }
        else{
            if (channelsTime_.find(j.first)->second.back()==j.second){
                clearChannel(j.first);
                break;
            }
        }
    }
    checkNum_.clear();
    for (const auto& i : channelsTime_){
        checkNum_.insert(std::make_pair(i.first,i.second.back()));
    }
}

std::map<int, QVector<QPointF>> ChannelTableModel::getCN0()
{
    std::map<int, QVector<QPointF>> cn0_map;
    QVector<QPointF> cn0;

    if (channelsCn0_.empty() || channelsTime_.empty()) return cn0_map;
    for (const auto& i : channelsCn0_){
        if (channelsTime_.find(i.first) == channelsTime_.end()) continue;
        for (int j = 0; j < i.second.size(); j++){
            QPointF point(channelsTime_.at(i.first).at(j),i.second.at(j));
            cn0.push_back(point);
        }
        cn0_map.insert(std::make_pair(i.first,cn0));
        cn0.clear();
    }
    return cn0_map;
}

std::map<int, QString> ChannelTableModel::getName()
{
    std::map<int, QString> name_map;
    if (channelsSignal_.empty() || channelsId_.empty()) return name_map;
    for (const auto& i : channelsSignal_){
        if (channels_.find(i.first) == channels_.end()) continue;
        name_map.insert(std::make_pair(i.first,i.second + " " + QString::number(channels_.at(i.first).prn)));
    }
    return name_map;
}
