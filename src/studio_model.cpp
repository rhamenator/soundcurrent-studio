// SPDX-License-Identifier: GPL-3.0-only
#include "studio_model.h"
#include <QJsonArray>
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace soundcurrent::studio {
namespace {
QString canonicalName(const QString &role, int channel) {
    if (role == "channel") return QString("Channel %1").arg(channel + 1);
    if (role == "mono") return "Mono";
    if (role == "left") return "Left";
    if (role == "right") return "Right";
    if (role == "front-left") return "Front left";
    if (role == "front-right") return "Front right";
    if (role == "center") return "Center";
    if (role == "lfe") return "LFE";
    if (role == "rear-left") return "Rear left";
    if (role == "rear-right") return "Rear right";
    if (role == "side-left") return "Side left";
    if (role == "side-right") return "Side right";
    return {};
}
void require(bool value, const char *message) { if (!value) throw std::runtime_error(message); }
double numeric(const QJsonObject &o, const char *key, double low, double high) {
    const auto v = o.value(key);
    require(v.isDouble() && std::isfinite(v.toDouble()) && v.toDouble() >= low && v.toDouble() <= high,
            "Studio profile has an invalid numeric field");
    return v.toDouble();
}
bool boolean(const QJsonObject &o, const char *key) {
    require(o.value(key).isBool(), "Studio profile has an invalid boolean field");
    return o.value(key).toBool();
}
}
Session::Session(std::size_t count) {
    require(count > 0 && count <= maxChannels, "Invalid Studio channel count");
    engine.channels.resize(count); routing.resize(count * count); solo.resize(count);
    nameProvenance.resize(count);
    QStringList roles;
    for (std::size_t c = 0; c < count; ++c) {
        routing[c * count + c] = 1;
        roles << "channel";
    }
    if (count == 1) roles = {"mono"};
    if (count == 2) roles = {"left", "right"};
    if (count == 6) roles = {"front-left", "front-right", "center", "lfe", "rear-left", "rear-right"};
    if (count == 8) roles = {"front-left", "front-right", "center", "lfe", "rear-left", "rear-right", "side-left", "side-right"};
    for (int c = 0; c < roles.size(); ++c) {
        names << canonicalName(roles[c], c);
        nameProvenance[std::size_t(c)] = QJsonObject{{"version", 1}, {"role", roles[c]}};
    }
}
QString Session::defaultNameRole(int channel) const {
    if (channel < 0 || channel >= names.size() || std::size_t(channel) >= nameProvenance.size()) return {};
    const auto metadata = nameProvenance[std::size_t(channel)].toObject();
    if (!metadata.value("version").isDouble() || metadata.value("version").toDouble() != 1) return {};
    const auto role = metadata.value("role").toString();
    const auto canonical = canonicalName(role, channel);
    return !canonical.isEmpty() && names[channel] == canonical ? role : QString();
}
void Session::setCustomName(int channel, const QString &name) {
    names[channel] = name;
    nameProvenance[std::size_t(channel)] = QJsonValue(QJsonValue::Undefined);
}

EngineSettings Session::effective(std::span<const EqBand> sharedBands, double gainDb, int balance) const {
    auto result = engine;
    const bool anySolo = std::find(solo.begin(), solo.end(), true) != solo.end();
    // Include the shared -60 dB fader and the saved Studio post gain (down to -24 dB).
    result.postGainDb = std::clamp(result.postGainDb + gainDb, kMinPostGainDb - 24.0, 24.0);
    for (std::size_t c = 0; c < result.channels.size(); ++c) {
        auto &channel = result.channels[c];
        require(channel.bands.size() + sharedBands.size() <= kMaxProcessingBands,
                "Shared and channel EQ exceed 64 filters; remove some channel filters");
        channel.bands.insert(channel.bands.begin(), sharedBands.begin(), sharedBands.end());
        channel.muted = channel.muted || (anySolo && !solo[c]);
        if (result.channels.size() >= 2 && ((c == 0 && balance > 0) || (c == 1 && balance < 0))) {
            const auto factor = 1.0 - std::abs(balance) / 100.0;
            if (factor <= 0) channel.muted = true;
            else channel.gainDb = std::max(-60.0, channel.gainDb + 20 * std::log10(factor));
        }
    }
    return result;
}
QJsonObject Session::json() const {
    QJsonArray channels, routes;
    for (std::size_t c = 0; c < engine.channels.size(); ++c) {
        QJsonArray bands;
        for (const auto &b : engine.channels[c].bands)
            bands.append(QJsonObject{{"frequency", b.frequency}, {"gain", b.gainDb}, {"q", b.q}, {"type", int(b.type)}});
        QJsonObject row{{"name", names[int(c)]}, {"gain", engine.channels[c].gainDb},
                        {"mute", engine.channels[c].muted}, {"solo", solo[c]}, {"bands", bands}};
        if (c < nameProvenance.size() && !nameProvenance[c].isUndefined())
            row.insert("nameProvenance", nameProvenance[c]);
        channels.append(row);
        for (std::size_t in = 0; in < engine.channels.size(); ++in)
            if (routing[c * engine.channels.size() + in] != 0)
                routes.append(QJsonArray{int(c), int(in), routing[c * engine.channels.size() + in]});
    }
    const auto &d = engine.delay; const auto &r = engine.reverb;
    QJsonArray enhancements;for(double v:engine.enhancements.values)enhancements.append(v);
    return {{"enhancements",enhancements},{"schema", 1}, {"channels", channels}, {"routing", routes}, {"offline", offline},
            {"postGain", engine.postGainDb}, {"headroom", engine.automaticHeadroom}, {"bypass", engine.bypass},
            {"delay", QJsonObject{{"enabled", d.enabled}, {"milliseconds", d.milliseconds}, {"feedback", d.feedback}, {"mix", d.mix}}},
            {"reverb", QJsonObject{{"enabled", r.enabled}, {"decay", r.decaySeconds}, {"damping", r.damping}, {"mix", r.mix}}}};
}
Session Session::parse(const QJsonObject &o) {
    require(o.value("schema").toInt() == 1 && o.value("channels").isArray() && o.value("routing").isArray(),
            "Unsupported Studio profile schema");
    const auto channels = o.value("channels").toArray();
    require(!channels.isEmpty() && channels.size() <= int(maxChannels), "Invalid Studio profile channel count");
    Session s(channels.size());
    s.offline = boolean(o, "offline"); s.engine.automaticHeadroom = boolean(o, "headroom");
    s.engine.bypass = boolean(o, "bypass"); s.engine.postGainDb = numeric(o, "postGain", -24, 24);
    for (int c = 0; c < channels.size(); ++c) {
        const auto row = channels[c].toObject();
        require(row.value("name").isString() && row.value("name").toString().size() <= 80 &&
                !row.value("name").toString().trimmed().isEmpty() && row.value("bands").isArray(), "Invalid Studio channel name or filters");
        s.nameProvenance[std::size_t(c)] = row.value("nameProvenance");
        s.names[c] = row.value("name").toString(); auto &channel = s.engine.channels[c];
        channel.gainDb = numeric(row, "gain", -60, 24); channel.muted = boolean(row, "mute"); s.solo[c] = boolean(row, "solo");
        const auto bands = row.value("bands").toArray();
        require(bands.size() <= int(kMaxProcessingBands), "Too many Studio channel filters");
        for (const auto &v : bands) {
            const auto b = v.toObject(); const double type = numeric(b, "type", 0, 4);
            require(type == std::floor(type), "Invalid filter type");
            channel.bands.push_back({numeric(b, "frequency", 20, 20000), numeric(b, "gain", -24, 24),
                                     numeric(b, "q", .1, 20), static_cast<FilterType>(int(type))});
        }
    }
    std::fill(s.routing.begin(), s.routing.end(), 0);
    const auto routes = o.value("routing").toArray();
    require(routes.size() <= channels.size() * channels.size(), "Too many Studio routes");
    std::vector<bool> seen(s.routing.size());
    for (const auto &v : routes) {
        const auto edge = v.toArray(); require(edge.size() == 3, "Invalid Studio route");
        for (const auto &n : edge) require(n.isDouble() && std::isfinite(n.toDouble()), "Invalid route number");
        const double out = edge[0].toDouble(), in = edge[1].toDouble(), weight = edge[2].toDouble();
        require(out >= 0 && out < channels.size() && in >= 0 && in < channels.size() &&
                out == std::floor(out) && in == std::floor(in) && std::abs(weight) <= 4, "Invalid route indexes or weight");
        const auto index = std::size_t(out) * channels.size() + std::size_t(in);
        require(!seen[index], "Duplicate Studio route"); seen[index] = true; s.routing[index] = weight;
    }
    const auto d = o.value("delay").toObject(), r = o.value("reverb").toObject();
    s.engine.delay = {boolean(d, "enabled"), numeric(d, "milliseconds", 1, 2000), numeric(d, "feedback", 0, .9), numeric(d, "mix", 0, 1)};
    s.engine.reverb = {boolean(r, "enabled"), numeric(r, "decay", .1, 10), numeric(r, "damping", 0, .95), numeric(r, "mix", 0, 1)};
    if(o.contains("enhancements")) {
        const auto values=o.value("enhancements").toArray();require(values.size()==EffectParameterCount,"Invalid enhancement parameter count");
        for(int i=0;i<values.size();++i) {require(values[i].isDouble(),"Invalid enhancement parameter type");s.engine.enhancements.values[std::size_t(i)]=values[i].toDouble();}
        require(s.engine.enhancements.valid(),"Enhancements outside supported ranges");
    }
    AudioEngine validator(48000, channels.size()); std::string error;
    require(validator.configure(s.engine, &error), error.c_str());
    return s;
}
}
