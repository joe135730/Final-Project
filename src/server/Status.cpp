#include "server/Status.h"

nlohmann::json StatusResponse::toJson() const {
    nlohmann::json j;
    j["term"] = term;
    j["last_index"] = last_index;
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& snap : summary) {
        arr.push_back(snap.toJson());
    }
    j["summary"] = arr;
    return j;
}


