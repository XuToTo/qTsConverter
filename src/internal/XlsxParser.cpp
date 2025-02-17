#include "XlsxParser.hpp"

#include "TitleHeaders.hpp"

#include <QApplication>
#include <QVersionNumber>
#include <xlsx/xlsxdocument.h>

XlsxParser::XlsxParser(InOutParameter &&parameter) :
    Parser{ std::move(parameter) }
{
}

auto XlsxParser::parse() const -> Result
{
    QXlsx::Document xlsx(m_ioParameter.inputFile);

    auto offsetRow{ 0 };
    const auto appVersion       = qApp->applicationVersion();
    const auto currentVersion   = QVersionNumber::fromString(appVersion);
    const auto TsSupportVersion = QVersionNumber(4, 5, 0);
    InOutParameter p{ "", "", m_ioParameter.tsVersion, {} };
    if (QVersionNumber::compare(currentVersion, TsSupportVersion) >= 0) {
        p.tsVersion = xlsx.cellAt(2, 1)->value().toString();
        offsetRow   = 2;
    }

    if (xlsx.read(offsetRow + 1, 1) != TitleHeader::Context ||
        xlsx.read(offsetRow + 1, 2) != TitleHeader::Source ||
        xlsx.read(offsetRow + 1, 3) != TitleHeader::Translation ||
        xlsx.read(offsetRow + 1, 4) != TitleHeader::Location) {
        return Result{ "Invalid XLSX file, check the headers!", {}, {} };
    }

    Translations translations;
    TranslationContext context;
    TranslationMessage msg;

    const auto lastRow    = xlsx.dimension().lastRow();
    const auto lastColumn = xlsx.dimension().lastColumn();

    for (auto row = 2; row <= lastRow; ++row) {
        context.name    = xlsx.cellAt(row, 1)->value().toString();
        msg.source      = xlsx.cellAt(row, 2)->value().toString();
        msg.translation = xlsx.cellAt(row, 3)->value().toString();

        for (auto col = 4; col <= lastColumn; ++col) {
            const auto loc = xlsx.cellAt(row, col);
            if (!loc || loc->value().toString().isEmpty()) {
                break;
            }
            auto list = loc->value().toString().split(QStringLiteral(" - "));
            msg.locations.emplace_back(
                std::make_pair(list.first(), list.last().toInt()));
        }

        auto it =
            std::find_if(translations.begin(), translations.end(),
                         [&](const auto &c) { return c.name == context.name; });
        if (it == translations.end()) {
            context.messages.clear();
            context.messages.emplace_back(msg);
            translations.emplace_back(context);
        } else {
            context.messages.emplace_back(msg);
            translations.at(std::distance(translations.begin(), it)) = context;
        }
        msg.locations.clear();
    }

    return Result{ "", std::move(translations), std::move(p) };
}
