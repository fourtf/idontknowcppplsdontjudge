#include "SearchPopup.hpp"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "common/Channel.hpp"
#include "messages/Message.hpp"
#include "messages/predicates/AuthorPredicate.hpp"
#include "messages/predicates/LinkPredicate.hpp"
#include "messages/predicates/SubstringPredicate.hpp"
#include "widgets/helper/ChannelView.hpp"

namespace chatterino {

SearchPopup::SearchPopup()
{
    this->initLayout();
    this->resize(400, 600);
}

void SearchPopup::setChannel(ChannelPtr channel)
{
    this->channelName_ = channel->getName();
    this->snapshot_ = channel->getMessageSnapshot();
    this->performSearch();

    this->setWindowTitle("Searching in " + channel->getName() + "s history");
}

void SearchPopup::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
    {
        this->close();
        return;
    }

    BaseWidget::keyPressEvent(e);
}

void SearchPopup::initLayout()
{
    // VBOX
    {
        QVBoxLayout *layout1 = new QVBoxLayout(this);
        layout1->setMargin(0);

        // HBOX
        {
            QHBoxLayout *layout2 = new QHBoxLayout(this);
            layout2->setMargin(6);

            // SEARCH INPUT
            {
                this->searchInput_ = new QLineEdit(this);
                layout2->addWidget(this->searchInput_);
                QObject::connect(this->searchInput_, &QLineEdit::returnPressed,
                                 [this] { this->performSearch(); });
            }

            // SEARCH BUTTON
            {
                QPushButton *searchButton = new QPushButton(this);
                searchButton->setText("Search");
                layout2->addWidget(searchButton);
                QObject::connect(searchButton, &QPushButton::clicked,
                                 [this] { this->performSearch(); });
            }

            layout1->addLayout(layout2);
        }

        // CHANNELVIEW
        {
            this->channelView_ = new ChannelView(this);

            layout1->addWidget(this->channelView_);
        }

        this->setLayout(layout1);
    }
}

void SearchPopup::performSearch()
{
    QString text = searchInput_->text();
    ChannelPtr channel(new Channel(this->channelName_, Channel::Type::None));

    // Parse predicates from tags in "text" and add them to "predicates_"
    parsePredicates(text);

    // Check for every message whether it fulfills all predicates that have
    // been registered
    for (size_t i = 0; i < this->snapshot_.size(); ++i)
    {
        MessagePtr message = this->snapshot_[i];

        bool accept = true;
        for (MessagePredicatePtr &pred : this->predicates_)
        {
            // Discard the message as soon as one predicate fails
            if (!pred->appliesTo(message))
            {
                accept = false;
                break;
            }
        }

        // If all predicates match, add the message to the channel
        if (accept)
            channel->addMessage(message);
    }

    this->channelView_->setChannel(channel);
}

void SearchPopup::parsePredicates(const QString &input)
{
    this->predicates_.clear();

    // Get a working copy we can modify
    QString text = input;

    // Check for "from:" tags
    QStringList searchedUsers = parseSearchedUsers(text);
    if (searchedUsers.size() > 0)
    {
        this->predicates_.push_back(
            std::make_shared<AuthorPredicate>(searchedUsers));
        removeTagFromText("from:", text);
    }

    // Check for "contains:link" tags
    if (text.contains("contains:link", Qt::CaseInsensitive))
    {
        this->predicates_.push_back(std::make_shared<LinkPredicate>());
        removeTagFromText("contains:link", text);
    }

    // The rest of the input is treated as a substring search.
    // If "text" is empty, every message will be matched.
    if (text.size() > 0)
    {
        this->predicates_.push_back(std::make_shared<SubstringPredicate>(text));
    }
}

void SearchPopup::removeTagFromText(const QString &tag, QString &text)
{
    for (QString &word : text.split(' ', QString::SkipEmptyParts))
    {
        if (word.startsWith(tag, Qt::CaseInsensitive))
            text.remove(word);
    }

    // Remove whitespace introduced by spaces between tags
    text = text.trimmed();
}

QStringList SearchPopup::parseSearchedUsers(const QString &input)
{
    QStringList parsedUserNames;

    for (QString &word : input.split(' ', QString::SkipEmptyParts))
    {
        // Users can be searched for by specifying them like so:
        // "from:user1,user2,user3" or "from:user1 from:user2 from:user3"

        if (!word.startsWith("from:"))
            // Ignore this word
            continue;

        // Get a working copy so we can manipulate the string
        QString fromTag = word;

        // Delete the "from:" part so we can parse the user names more easily
        fromTag.remove(0, 5);

        // Parse comma-seperated user names
        for (QString &user : fromTag.split(',', QString::SkipEmptyParts))
        {
            parsedUserNames << user;
        }
    }

    return parsedUserNames;
}

}  // namespace chatterino
