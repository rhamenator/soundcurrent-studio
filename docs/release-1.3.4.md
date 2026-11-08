# SoundCurrent Studio 1.3.4

- Make the Equalizer, Studio channels and effects, configuration, and analysis pages adapt cleanly to small screens, larger display scaling, long translations, and right-to-left layouts.
- Keep the equalizer band strip horizontally scrollable without widening the whole page. Band columns now use deterministic sizing, including a tested 15-to-16-band transition that does not resize or overlap existing sliders.
- Clarify that offline rendering and silent meter tests support all channels, and remove the redundant “Studio settings applied to live playback” status message.
- Make listening-preset, selected-band, device, language, update, Studio routing/effects, and analyzer controls wrap responsively when space is limited.
- Add automated appearance and scaling coverage for 800×600, 200% scaling, pseudolocalized text, and all 34 localization catalogs.

## Upgrade

Use **Quit**, then install 1.3.4 over the existing application. Settings and custom profiles are retained. Audio processing and driver behavior are unchanged.
