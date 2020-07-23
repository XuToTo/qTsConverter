#include "ConversionModel.hpp"

#include "ConverterFactory.hpp"

ConversionModel::ConversionModel(QObject *parent) :
    QAbstractListModel(parent), m_conversions{ "TS => CSV", "CSV => TS",
                                               "TS => XLSX", "XLSX => TS",
                                               "Error" }
{
}

int ConversionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_conversions.size();
}

QVariant ConversionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    if (role == Roles::String) {
        return m_conversions.at(index.row());
    }

    return {};
}

QHash<int, QByteArray> ConversionModel::roleNames() const
{
    return { { Roles::String, "stringRole" } };
}

void ConversionModel::setInput(const QString &value)
{
    m_input = value;
    deduceInputOutput();
}

void ConversionModel::setOutput(const QString &value)
{
    m_output = value;
    deduceInputOutput();
}

QStringList ConversionModel::getSaveFT()
{
    if (currentIndex == ConverterFactory::Csv2Ts ||
        currentIndex == ConverterFactory::Xlsx2Ts) {
        return QStringList({ "Translation files (*.ts)" });
    }

    if (currentIndex == ConverterFactory::Ts2Csv) {
        return QStringList({ "CSV files (*.csv)" });
    }

    if (currentIndex == ConverterFactory::Ts2Xlsx) {
        return QStringList({ "Excel files (*.xls, *.xlsx)" });
    }

    return QStringList({ "All files (*)" });
}

QStringList ConversionModel::getLoadFT()
{
    if (currentIndex == ConverterFactory::Ts2Csv ||
        currentIndex == ConverterFactory::Ts2Xlsx) {
        return QStringList({ "Translation files (*.ts)" });
    }

    if (currentIndex == ConverterFactory::Csv2Ts) {
        return QStringList({ "CSV files (*.csv)" });
    }

    if (currentIndex == ConverterFactory::Xlsx2Ts) {
        return QStringList({ "Excel files (*.xls, *.xlsx)" });
    }

    return QStringList({ "All files (*)" });
}

void ConversionModel::deduceInputOutput() noexcept
{
    if (m_input.isEmpty() || m_output.isEmpty()) {
        return;
    }

    if (m_input.endsWith(QStringLiteral(".ts"))) {
        if (m_output.endsWith(QStringLiteral(".csv"))) {
            currentIndex = ConverterFactory::Ts2Csv;
        }

        if (m_output.endsWith(QStringLiteral(".xls")) ||
            m_output.endsWith(QStringLiteral(".xlsx"))) {
            currentIndex = ConverterFactory::Ts2Xlsx;
        }
    }

    if (m_input.endsWith(QStringLiteral(".csv"))) {
        if (m_output.endsWith(QStringLiteral(".ts"))) {
            currentIndex = ConverterFactory::Csv2Ts;
        }
    }

    if (m_input.endsWith(QStringLiteral(".xls")) ||
        m_input.endsWith(QStringLiteral(".xlsx"))) {
        if (m_output.endsWith(QStringLiteral(".ts"))) {
            currentIndex = ConverterFactory::Xlsx2Ts;
        }
    }

    Q_EMIT setComboBoxIndex(currentIndex);
}

void ConversionModel::setIndex(const int &newIndex)
{
    currentIndex = newIndex;
}
