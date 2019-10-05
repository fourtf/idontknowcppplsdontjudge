#pragma once

#include "Application.hpp"
#include "debug/Log.hpp"
#include "util/RapidJsonSerializeQString.hpp"
#include "util/RapidjsonHelpers.hpp"

#include <QRegularExpression>
#include <QString>
#include <pajlada/serialize.hpp>

namespace chatterino {

class HighlightPhrase
{
public:
    static const QColor DEFAULT_HIGHLIGHT_COLOR;

    bool operator==(const HighlightPhrase &other) const;

    HighlightPhrase(const QString &pattern, bool hasAlert, bool hasSound,
                    bool isRegex, bool isCaseSensitive, const QString &soundUrl,
                    const QColor &color);

    /**
     * @brief Encode a given color as string.
     *
     * The color will be encoded as the color name, and the alpha value as an int,
     * separated by a ":".
     *
     * For example, "#157368:100" is a valid color encoding.
     *
     * @param color the color to encode
     * @return a string encoding the passed color
     */
    static QString encodeColor(const QColor &color);

    /**
     * @brief Decode the color given by `encodedColor`.
     *
     * This method attempts to parse the color encoded in the passed string.
     * A valid encoding is given by the color name, and the alpha value as an int,
     * separated by a ":".
     * If the encoding does not follow this format, the default color is returned.
     *
     * For example, "#157368:100" is a valid color encoding.
     *
     * @param encodedColor the string to decoded
     * @return the encoded color, or the default color if the format could not be
     *         parsed
     */
    static QColor decodeColor(const QString& encodedColor);

    const QString &getPattern() const;
    bool hasAlert() const;
    bool hasSound() const;
    bool hasCustomSound() const;
    bool isRegex() const;
    bool isValid() const;
    bool isMatch(const QString &subject) const;
    bool isCaseSensitive() const;
    const QUrl &getSoundUrl() const;
    const QColor &getColor() const;

private:
    QString pattern_;
    bool hasAlert_;
    bool hasSound_;
    bool isRegex_;
    bool isCaseSensitive_;
    QUrl soundUrl_;
    QColor color_;
    QRegularExpression regex_;
};

}  // namespace chatterino

namespace pajlada {

template <>
struct Serialize<chatterino::HighlightPhrase> {
    static rapidjson::Value get(const chatterino::HighlightPhrase &value,
                                rapidjson::Document::AllocatorType &a)
    {
        rapidjson::Value ret(rapidjson::kObjectType);

        QString encodedColor = chatterino::HighlightPhrase::encodeColor(value.getColor());

        chatterino::rj::set(ret, "pattern", value.getPattern(), a);
        chatterino::rj::set(ret, "alert", value.hasAlert(), a);
        chatterino::rj::set(ret, "sound", value.hasSound(), a);
        chatterino::rj::set(ret, "regex", value.isRegex(), a);
        chatterino::rj::set(ret, "case", value.isCaseSensitive(), a);
        chatterino::rj::set(ret, "soundUrl", value.getSoundUrl().toString(), a);
        chatterino::rj::set(ret, "color", encodedColor, a);

        return ret;
    }
};

template <>
struct Deserialize<chatterino::HighlightPhrase> {
    static chatterino::HighlightPhrase get(const rapidjson::Value &value)
    {
        if (!value.IsObject())
        {
            return chatterino::HighlightPhrase(
                QString(), true, false, false, false, "",
                chatterino::HighlightPhrase::DEFAULT_HIGHLIGHT_COLOR);
        }

        QString _pattern;
        bool _hasAlert = true;
        bool _hasSound = false;
        bool _isRegex = false;
        bool _isCaseSensitive = false;
        QString _soundUrl;
        QString encodedColor;

        chatterino::rj::getSafe(value, "pattern", _pattern);
        chatterino::rj::getSafe(value, "alert", _hasAlert);
        chatterino::rj::getSafe(value, "sound", _hasSound);
        chatterino::rj::getSafe(value, "regex", _isRegex);
        chatterino::rj::getSafe(value, "case", _isCaseSensitive);
        chatterino::rj::getSafe(value, "soundUrl", _soundUrl);
        chatterino::rj::getSafe(value, "color", encodedColor);

        QColor _color = 
            chatterino::HighlightPhrase::decodeColor(encodedColor);

        return chatterino::HighlightPhrase(_pattern, _hasAlert, _hasSound,
                                           _isRegex, _isCaseSensitive,
                                           _soundUrl, _color);
    }
};

}  // namespace pajlada
