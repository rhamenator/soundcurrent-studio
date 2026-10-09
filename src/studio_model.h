// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "engine.h"
#include <QJsonObject>
#include <QStringList>

namespace soundcurrent::studio {
struct Session {
    EngineSettings engine;
    std::vector<double> routing;
    QStringList names;
    // Optional display provenance; opaque future metadata is retained verbatim.
    // No provenance in an old profile means the saved name is user-owned.
    std::vector<QJsonValue> nameProvenance;
    QString defaultNameRole(int channel) const;
    void setCustomName(int channel, const QString &name);
    std::vector<bool> solo;
    bool offline = false;
    explicit Session(std::size_t channels = 2);
    EngineSettings effective(std::span<const EqBand> sharedBands = {}, double gainDb = 0,
                             int balance = 0) const;
    QJsonObject json() const;
    static Session parse(const QJsonObject &);
};
}
