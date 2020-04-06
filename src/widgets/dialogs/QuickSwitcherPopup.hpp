#pragma once

#include "common/Channel.hpp"
#include "widgets/BasePopup.hpp"
#include "widgets/splits/Split.hpp"

namespace chatterino {

class QuickSwitcherPopup : public BasePopup
{
public:
    // TODO(leon): Replace this with a custom type in order to be able to
    // overload equality operator
    using ChannelSplits = std::pair<ChannelPtr, Split *>;
    explicit QuickSwitcherPopup(QWidget *parent = nullptr);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
    void updateSuggestions(const QString &text);

private:
    class SwitcherItem : public QListWidgetItem
    {
    public:
        bool matches(const QString &query) const;

    private:
        std::function<void> action_;
        Split *split_;

        // TODO(leon): Check if needed
        friend class QuickSwitcherPopup;
    };

    struct {
        QLineEdit *searchEdit{};
        QListWidget *list{};
    } ui_;

    void initWidgets();
};

inline uint qHash(const ChannelPtr &key)
{
    return qHash(key->getName());
}

}  // namespace chatterino
