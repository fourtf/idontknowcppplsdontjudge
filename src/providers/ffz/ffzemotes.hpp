#pragma once

#include "signalvector.hpp"
#include "util/concurrentmap.hpp"
#include "util/emotemap.hpp"

#include <map>

namespace chatterino {
namespace providers {
namespace ffz {

class FFZEmotes
{
public:
    util::EmoteMap globalEmotes;
    SignalVector<QString> globalEmoteCodes;

    util::EmoteMap channelEmotes;
    std::map<QString, SignalVector<QString>> channelEmoteCodes;

    void loadGlobalEmotes();
    void loadChannelEmotes(const QString &channelName,
                           std::weak_ptr<util::EmoteMap> channelEmoteMap);

private:
    util::ConcurrentMap<int, util::EmoteData> channelEmoteCache;
};

}  // namespace ffz
}  // namespace providers
}  // namespace chatterino
