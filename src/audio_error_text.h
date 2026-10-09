// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "localization_text.h"
#include <QRegularExpression>

namespace soundcurrent::i18n {
// Desktop boundary only. Backend diagnostic strings and processing identifiers
// remain invariant; unknown/external messages retain their original detail.
inline QString audioErrorText(const QString &diagnostic) {
    if (diagnostic == QStringLiteral("Missing, duplicate or oversized WAVE format"))
        return SC_TR("Missing, duplicate or oversized WAVE format");
    if (diagnostic == QStringLiteral("Unsupported extensible WAVE subtype"))
        return SC_TR("Unsupported extensible WAVE subtype");
    if (diagnostic == QStringLiteral("Invalid WAVE frame alignment or byte rate"))
        return SC_TR("Invalid WAVE frame alignment or byte rate");
    if (diagnostic == QStringLiteral("Unsupported WAVE rate or channel count"))
        return SC_TR("Unsupported WAVE rate or channel count");
    if (diagnostic == QStringLiteral("Only little-endian RIFF/WAVE is supported"))
        return SC_TR("Only little-endian RIFF/WAVE is supported");
    if (diagnostic == QStringLiteral("Only PCM16/24/32 or float32 WAVE is supported"))
        return SC_TR("Only PCM16/24/32 or float32 WAVE is supported");
    if (diagnostic == QStringLiteral("Missing or incomplete WAVE audio"))
        return SC_TR("Missing or incomplete WAVE audio");
    if (diagnostic == QStringLiteral("Multiple WAVE data chunks are unsupported"))
        return SC_TR("Multiple WAVE data chunks are unsupported");
    if (diagnostic == QStringLiteral("Input is too short for RIFF/WAVE"))
        return SC_TR("Input is too short for RIFF/WAVE");
    if (diagnostic == QStringLiteral("Truncated extensible WAVE format"))
        return SC_TR("Truncated extensible WAVE format");
    if (diagnostic == QStringLiteral("Output exceeds the RIFF/WAVE 4 GiB limit"))
        return SC_TR("Output exceeds the RIFF/WAVE 4 GiB limit");
    if (diagnostic == QStringLiteral("WAVE output exceeds its declared length"))
        return SC_TR("WAVE output exceeds its declared length");
    if (diagnostic == QStringLiteral("Missing RIFF padding byte"))
        return SC_TR("Missing RIFF padding byte");
    if (diagnostic == QStringLiteral("Excessive number of RIFF chunks"))
        return SC_TR("Excessive number of RIFF chunks");
    if (diagnostic == QStringLiteral("Speaker mask does not match channel count"))
        return SC_TR("Speaker mask does not match channel count");
    if (diagnostic == QStringLiteral("Invalid output speaker mask"))
        return SC_TR("Invalid output speaker mask");
    if (diagnostic == QStringLiteral("Invalid valid-bit count"))
        return SC_TR("Invalid valid-bit count");
    if (diagnostic == QStringLiteral("Invalid float WAVE format"))
        return SC_TR("Invalid float WAVE format");
    if (diagnostic == QStringLiteral("Incomplete WAVE output"))
        return SC_TR("Incomplete WAVE output");
    if (diagnostic == QStringLiteral("Truncated chunk header"))
        return SC_TR("Truncated chunk header");
    if (diagnostic == QStringLiteral("Could not write WAVE header"))
        return SC_TR("Could not write WAVE header");
    if (diagnostic == QStringLiteral("Could not write WAVE audio"))
        return SC_TR("Could not write WAVE audio");
    if (diagnostic == QStringLiteral("Could not flush WAVE output"))
        return SC_TR("Could not flush WAVE output");
    if (diagnostic == QStringLiteral("Could not close WAVE output"))
        return SC_TR("Could not close WAVE output");
    if (diagnostic == QStringLiteral("Invalid WAVE read buffer"))
        return SC_TR("Invalid WAVE read buffer");
    if (diagnostic == QStringLiteral("Invalid output WAVE format"))
        return SC_TR("Invalid output WAVE format");
    if (diagnostic == QStringLiteral("Invalid RIFF size"))
        return SC_TR("Invalid RIFF size");
    if (diagnostic == QStringLiteral("Chunk extends beyond RIFF bounds"))
        return SC_TR("Chunk extends beyond RIFF bounds");
    if (diagnostic == QStringLiteral("Cannot seek to WAVE audio"))
        return SC_TR("Cannot seek to WAVE audio");
    if (diagnostic == QStringLiteral("Cannot create output WAVE file"))
        return SC_TR("Cannot create output WAVE file");
    if (diagnostic == QStringLiteral("Truncated WAVE file"))
        return SC_TR("Truncated WAVE file");
    if (diagnostic == QStringLiteral("Cannot open input WAVE file"))
        return SC_TR("Cannot open input WAVE file");
    if (diagnostic == QStringLiteral("Invalid audio route: loopback requires a separate render source"))
        return SC_TR("Invalid audio route: loopback requires a separate render source");
    if (diagnostic == QStringLiteral("Invalid Studio channel count"))
        return SC_TR("Invalid Studio channel count");
    if (diagnostic == QStringLiteral("Invalid Studio profile channel count"))
        return SC_TR("Invalid Studio profile channel count");
    if (diagnostic == QStringLiteral("Invalid Studio channel name or filters"))
        return SC_TR("Invalid Studio channel name or filters");
    if (diagnostic == QStringLiteral("Too many Studio channel filters"))
        return SC_TR("Too many Studio channel filters");
    if (diagnostic == QStringLiteral("Invalid Studio route"))
        return SC_TR("Invalid Studio route");
    if (diagnostic == QStringLiteral("Duplicate Studio route"))
        return SC_TR("Duplicate Studio route");
    if (diagnostic == QStringLiteral("Invalid route indexes or weight"))
        return SC_TR("Invalid route indexes or weight");
    if (diagnostic == QStringLiteral("Invalid route number"))
        return SC_TR("Invalid route number");
    if (diagnostic == QStringLiteral("Too many Studio routes"))
        return SC_TR("Too many Studio routes");
    if (diagnostic == QStringLiteral("Invalid filter type"))
        return SC_TR("Invalid filter type");
    if (diagnostic == QStringLiteral("Shared and channel EQ exceed 64 filters; remove some channel filters"))
        return SC_TR("Shared and channel EQ exceed 64 filters; remove some channel filters");
    if (diagnostic == QStringLiteral("Invalid enhancement parameter count"))
        return SC_TR("Invalid enhancement parameter count");
    if (diagnostic == QStringLiteral("Invalid enhancement parameter type"))
        return SC_TR("Invalid enhancement parameter type");
    if (diagnostic == QStringLiteral("Enhancements outside supported ranges"))
        return SC_TR("Enhancements outside supported ranges");
    if (diagnostic == QStringLiteral("Unsupported Studio profile schema"))
        return SC_TR("Unsupported Studio profile schema");
    if (diagnostic == QStringLiteral("Studio profile has an invalid numeric field"))
        return SC_TR("Studio profile has an invalid numeric field");
    if (diagnostic == QStringLiteral("Studio profile has an invalid boolean field"))
        return SC_TR("Studio profile has an invalid boolean field");
    if (diagnostic == QStringLiteral("The selected EQ settings are invalid"))
        return SC_TR("Invalid equalizer settings");
    if (diagnostic == QStringLiteral("Cable packet exceeds its capture buffer"))
        return SC_TR("Cable packet exceeds its capture buffer");
    if (diagnostic == QStringLiteral("Could not initialize Windows audio COM"))
        return SC_TR("Could not initialize Windows audio COM");
    if (diagnostic == QStringLiteral("Invalid calibration audio"))
        return SC_TR("Invalid calibration audio");
    if (diagnostic == QStringLiteral("Invalid speaker mix format"))
        return SC_TR("Invalid speaker mix format");
    if (diagnostic == QStringLiteral("Microphone recording consumer stalled"))
        return SC_TR("Microphone recording consumer stalled");
    if (diagnostic == QStringLiteral("Unsupported cable channel count"))
        return SC_TR("Unsupported cable channel count");
    if (diagnostic == QStringLiteral("Unsupported recording format"))
        return SC_TR("Unsupported recording format");
    if (diagnostic == QStringLiteral("Unsupported speaker channel layout or sample rate"))
        return SC_TR("Unsupported speaker channel layout or sample rate");
    if (diagnostic == QStringLiteral("Unsupported speaker mix sample format"))
        return SC_TR("Unsupported speaker mix sample format");
    if (diagnostic == QStringLiteral("Virtual output requires a supported 48 kHz float channel layout"))
        return SC_TR("Virtual output requires a supported 48 kHz float channel layout");
    if (diagnostic == QStringLiteral("Windows audio COM unavailable"))
        return SC_TR("Windows audio COM unavailable");
    if (diagnostic == QStringLiteral("Microphone start timed out"))
        return SC_TR("Microphone start timed out");
    if (diagnostic == QStringLiteral("Cable recording endpoint does not support shared 48 kHz stereo float audio"))
        return SC_TR("Cable recording endpoint does not support shared 48 kHz stereo float audio");
    if (diagnostic == QStringLiteral("Audio route recovery helper could not start. Repair or reinstall SoundCurrent."))
        return SC_TR("Audio route recovery helper could not start. Repair or reinstall SoundCurrent.");
    if (diagnostic == QStringLiteral("PipeWire live streams support at most 64 channels; use offline rendering for larger layouts"))
        return SC_TR("PipeWire live streams support at most 64 channels; use offline rendering for larger layouts");
    if (diagnostic == QStringLiteral("Invalid Studio routing matrix"))
        return SC_TR("Invalid Studio routing matrix");
    if (diagnostic == QStringLiteral("Cannot create PipeWire loop"))
        return SC_TR("Cannot create PipeWire loop");
    if (diagnostic == QStringLiteral("Cannot create PipeWire streams"))
        return SC_TR("Cannot create PipeWire streams");
    if (diagnostic == QStringLiteral("Cannot connect PipeWire streams"))
        return SC_TR("Cannot connect PipeWire streams");
    if (diagnostic == QStringLiteral("Turn playback off before applying a new live channel layout"))
        return SC_TR("Turn playback off before applying a new live channel layout");
    if (diagnostic == QStringLiteral("Channel configuration count does not match engine"))
        return SC_TR("Channel configuration count does not match engine");
    if (diagnostic == QStringLiteral("Post gain must be finite and within -84 to +24 dB"))
        return SC_TR("Post gain must be finite and within -84 to +24 dB");
    if (diagnostic == QStringLiteral("Invalid enhancement settings"))
        return SC_TR("Invalid enhancement settings");
    if (diagnostic == QStringLiteral("Delay settings are outside the supported range"))
        return SC_TR("Delay settings are outside the supported range");
    if (diagnostic == QStringLiteral("Reverb settings are outside the supported range"))
        return SC_TR("Reverb settings are outside the supported range");
    if (diagnostic == QStringLiteral("Invalid channel gain or too many EQ bands"))
        return SC_TR("Invalid channel gain or too many EQ bands");
    if (diagnostic == QStringLiteral("Invalid EQ band"))
        return SC_TR("Invalid EQ band");
    if (diagnostic == QStringLiteral("Effects exceed the preview's 128 MiB state budget"))
        return SC_TR("Effects exceed the preview's 128 MiB state budget");
    if (diagnostic == QStringLiteral("Could not allocate effect state"))
        return SC_TR("Could not allocate effect state");
    // Match only this backend's invariant HRESULT message format. Unknown
    // operations are not guessed, and the hexadecimal error code is preserved.
    static const QRegularExpression failure(QStringLiteral("^(.+) failed \\(0x([0-9A-Fa-f]{1,8})\\)$"));
    const auto match = failure.match(diagnostic);
    if (match.hasMatch()) {
        const auto action = match.captured(1);
        QString translatedAction;
        if (action == QStringLiteral("Automatic audio routing unavailable")) translatedAction = SC_TR("Automatic audio routing unavailable");
        if (action == QStringLiteral("Change default audio endpoint")) translatedAction = SC_TR("Change default audio endpoint");
        if (action == QStringLiteral("Count audio endpoints")) translatedAction = SC_TR("Count audio endpoints");
        if (action == QStringLiteral("Drain test playback")) translatedAction = SC_TR("Drain test playback");
        if (action == QStringLiteral("Enumerate audio devices")) translatedAction = SC_TR("Enumerate audio devices");
        if (action == QStringLiteral("Enumerate endpoints")) translatedAction = SC_TR("Enumerate endpoints");
        if (action == QStringLiteral("Initialize audio capture")) translatedAction = SC_TR("Initialize audio capture");
        if (action == QStringLiteral("Initialize microphone recording")) translatedAction = SC_TR("Initialize microphone recording");
        if (action == QStringLiteral("Initialize speaker output")) translatedAction = SC_TR("Initialize speaker output");
        if (action == QStringLiteral("Initialize test playback")) translatedAction = SC_TR("Initialize test playback");
        if (action == QStringLiteral("List audio endpoints")) translatedAction = SC_TR("List audio endpoints");
        if (action == QStringLiteral("Open audio stream")) translatedAction = SC_TR("Open audio stream");
        if (action == QStringLiteral("Open cable capture stream")) translatedAction = SC_TR("Open cable capture stream");
        if (action == QStringLiteral("Open cable recording endpoint")) translatedAction = SC_TR("Open cable recording endpoint");
        if (action == QStringLiteral("Open endpoint")) translatedAction = SC_TR("Open endpoint");
        if (action == QStringLiteral("Open endpoint volume")) translatedAction = SC_TR("Open endpoint volume");
        if (action == QStringLiteral("Open microphone reader")) translatedAction = SC_TR("Open microphone reader");
        if (action == QStringLiteral("Open speaker endpoint")) translatedAction = SC_TR("Open speaker endpoint");
        if (action == QStringLiteral("Open speaker render stream")) translatedAction = SC_TR("Open speaker render stream");
        if (action == QStringLiteral("Open test playback writer")) translatedAction = SC_TR("Open test playback writer");
        if (action == QStringLiteral("Read audio endpoint")) translatedAction = SC_TR("Read audio endpoint");
        if (action == QStringLiteral("Read audio endpoint ID")) translatedAction = SC_TR("Read audio endpoint ID");
        if (action == QStringLiteral("Read audio endpoint name")) translatedAction = SC_TR("Read audio endpoint name");
        if (action == QStringLiteral("Read audio endpoint properties")) translatedAction = SC_TR("Read audio endpoint properties");
        if (action == QStringLiteral("Read cable audio")) translatedAction = SC_TR("Read cable audio");
        if (action == QStringLiteral("Read cable capture interface")) translatedAction = SC_TR("Read cable capture interface");
        if (action == QStringLiteral("Read cable channel layout")) translatedAction = SC_TR("Read cable channel layout");
        if (action == QStringLiteral("Read cable packet size")) translatedAction = SC_TR("Read cable packet size");
        if (action == QStringLiteral("Read cable speaker mask")) translatedAction = SC_TR("Read cable speaker mask");
        if (action == QStringLiteral("Read default output ID")) translatedAction = SC_TR("Read default output ID");
        if (action == QStringLiteral("Read default output endpoint")) translatedAction = SC_TR("Read default output endpoint");
        if (action == QStringLiteral("Read microphone mix format")) translatedAction = SC_TR("Read microphone mix format");
        if (action == QStringLiteral("Read microphone packet size")) translatedAction = SC_TR("Read microphone packet size");
        if (action == QStringLiteral("Read microphone samples")) translatedAction = SC_TR("Read microphone samples");
        if (action == QStringLiteral("Read next cable packet size")) translatedAction = SC_TR("Read next cable packet size");
        if (action == QStringLiteral("Read next microphone packet")) translatedAction = SC_TR("Read next microphone packet");
        if (action == QStringLiteral("Read output buffer level")) translatedAction = SC_TR("Read output buffer level");
        if (action == QStringLiteral("Read output level")) translatedAction = SC_TR("Read output level");
        if (action == QStringLiteral("Read output mute")) translatedAction = SC_TR("Read output mute");
        if (action == QStringLiteral("Read speaker level")) translatedAction = SC_TR("Read speaker level");
        if (action == QStringLiteral("Read speaker mix format")) translatedAction = SC_TR("Read speaker mix format");
        if (action == QStringLiteral("Read speaker mute")) translatedAction = SC_TR("Read speaker mute");
        if (action == QStringLiteral("Read speaker render interface")) translatedAction = SC_TR("Read speaker render interface");
        if (action == QStringLiteral("Read speaker volume")) translatedAction = SC_TR("Read speaker volume");
        if (action == QStringLiteral("Read test playback padding")) translatedAction = SC_TR("Read test playback padding");
        if (action == QStringLiteral("Read virtual output mix format")) translatedAction = SC_TR("Read virtual output mix format");
        if (action == QStringLiteral("Release cable audio")) translatedAction = SC_TR("Release cable audio");
        if (action == QStringLiteral("Release microphone packet")) translatedAction = SC_TR("Release microphone packet");
        if (action == QStringLiteral("Release speaker buffer")) translatedAction = SC_TR("Release speaker buffer");
        if (action == QStringLiteral("Release test playback")) translatedAction = SC_TR("Release test playback");
        if (action == QStringLiteral("Set full speaker level for EQ")) translatedAction = SC_TR("Set full speaker level for EQ");
        if (action == QStringLiteral("Set output level")) translatedAction = SC_TR("Set output level");
        if (action == QStringLiteral("Set output mute")) translatedAction = SC_TR("Set output mute");
        if (action == QStringLiteral("Size capture buffer")) translatedAction = SC_TR("Size capture buffer");
        if (action == QStringLiteral("Size output buffer")) translatedAction = SC_TR("Size output buffer");
        if (action == QStringLiteral("Size test playback buffer")) translatedAction = SC_TR("Size test playback buffer");
        if (action == QStringLiteral("Start cable capture")) translatedAction = SC_TR("Start cable capture");
        if (action == QStringLiteral("Start microphone recording")) translatedAction = SC_TR("Start microphone recording");
        if (action == QStringLiteral("Start speaker output")) translatedAction = SC_TR("Start speaker output");
        if (action == QStringLiteral("Start test playback")) translatedAction = SC_TR("Start test playback");
        if (action == QStringLiteral("Unmute speaker for EQ")) translatedAction = SC_TR("Unmute speaker for EQ");
        if (action == QStringLiteral("Write speaker buffer")) translatedAction = SC_TR("Write speaker buffer");
        if (action == QStringLiteral("Write test playback")) translatedAction = SC_TR("Write test playback");
        if (!translatedAction.isEmpty())
            return SC_TR("%1 failed (0x%2)").arg(translatedAction, match.captured(2));
    }
    return diagnostic;
}
}
