// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "studio_model.h"
#include "localization_text.h"
#include <QLocale>

namespace soundcurrent::studio {
// Only explicit, applicable generated-name provenance permits translation.
// Unknown/legacy/custom names are user data and remain verbatim.
inline QString channelNameText(const Session &session, int channel) {
    const auto role = session.defaultNameRole(channel);
    if (role == "channel") return SC_TR("Channel %1").arg(QLocale().toString(channel + 1));
    if (role == "mono") return SC_TR("Mono");
    if (role == "left") return SC_TR("Left");
    if (role == "right") return SC_TR("Right");
    if (role == "front-left") return SC_TR("Front left");
    if (role == "front-right") return SC_TR("Front right");
    if (role == "center") return SC_TR("Center channel");
    if (role == "lfe") return QStringLiteral("LFE"); // Standard channel abbreviation.
    if (role == "rear-left") return SC_TR("Rear left");
    if (role == "rear-right") return SC_TR("Rear right");
    if (role == "side-left") return SC_TR("Side left");
    if (role == "side-right") return SC_TR("Side right");
    return session.names[channel];
}
}
