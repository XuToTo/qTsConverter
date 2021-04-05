#include "ConversionModel.hpp"

#include "ConverterFactory.hpp"

#include <QDesktopServices>
#include <QFileInfo>
#include <QUrl>
#include <algorithm>

ConversionModel::ConversionModel(QObject *parent) :
    QAbstractListModel(parent), m_conversions{ "TS => CSV", "CSV => TS",
                                               "TS => XLSX", "XLSX => TS",
                                               "Error" }
{
}

auto ConversionModel::rowCount(const QModelIndex &parent) const -> int
{
    Q_UNUSED(parent)
    return m_conversions.size();
}

auto ConversionModel::data(const QModelIndex &index, int role) const -> QVariant
{
    if (!index.isValid()) {
        return {};
    }

    if (role == Roles::String) {
        return m_conversions.at(index.row());
    }

    return {};
}

auto ConversionModel::roleNames() const -> QHash<int, QByteArray>
{
    return { { Roles::String, "stringRole" } };
}

void ConversionModel::clearInput()
{
    m_input.clear();
}

void ConversionModel::addInput(const QString &value)
{
    m_input.push_back(value);

    if (m_input.size() == 1) {
        m_sourceMsg = m_input.front();
        emit sourceMsgChanged();
        return;
    }

    if (m_input.size() > 1 && !inputHaveSameExtension()) {
        m_sourceMsg = "source files should have same extension";
        emit sourceMsgChanged();
        return;
    }

    m_sourceMsg = QString::number(m_input.size()) + " files selected";
    emit sourceMsgChanged();
}

void ConversionModel::setOutput(const QString &value)
{
    m_output = value;

    const auto uselessCharsForFolder = 8; // file:///
    m_output = m_output.remove(0, uselessCharsForFolder);
    m_output.push_front("/");
    if (QFileInfo(m_output).isDir()) {
        return;
    }

    m_output = value;

    QString temp =
        m_output.right(m_output.length() - 1 - m_output.lastIndexOf("/"));

    if (!temp.contains(QRegExp(R"(\S+\.\S+)"))) {
        if (currentIndex == ConverterFactory::Ts2Xlsx) {
            m_output += ".xlsx";
        } else if (currentIndex == ConverterFactory::Ts2Csv) {
            m_output += ".csv";
        } else if (currentIndex != ConverterFactory::Dummy) {
            m_output += ".ts";
        }
    }
}

auto ConversionModel::input() noexcept -> QStringList
{
    return m_input;
}

auto ConversionModel::inputHaveSameExtension() noexcept -> bool
{
    const auto extension = m_input.first().split(".")[1];
    return std::all_of(m_input.cbegin(), m_input.cend(), [&](const auto &s) {
        return s.split(".")[1] == extension;
    });
}

void ConversionModel::setIndex(const int &newIndex)
{
    currentIndex = newIndex;
}

void ConversionModel::openOutput()
{
    QString replaced = m_output;
    replaced.replace(0, 7, "");
    QDesktopServices::openUrl(QUrl::fromLocalFile(replaced));
}

void ConversionModel::openOutputFolder()
{
    if (QFileInfo(m_output).isDir()) {
        QDesktopServices::openUrl(QUrl(m_output));
        return;
    }

    QString replaced = m_output;
    replaced.replace(0, 7, "");
    int pos = replaced.lastIndexOf(QRegExp("/.*"));
    replaced.replace(pos, replaced.length() - pos, "");
    QDesktopServices::openUrl(QUrl::fromLocalFile(replaced));
}

auto ConversionModel::sourceMsg() const -> QString
{
    return m_sourceMsg;
}
