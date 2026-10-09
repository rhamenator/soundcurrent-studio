<?xml version='1.0' encoding='utf-8'?>
<TS version="2.1" language="da" sourcelanguage="en_US">
  <context>
    <name>SoundCurrent</name>
    <message>
      <source> (currently selected)</source>
      <translation> (valgt nu)</translation>
    </message>
    <message>
      <source> (original; not SS-CS5M2)</source>
      <translation> (oprindelig model; ikke SS-CS5M2)</translation>
      <extracomment>Display suffix distinguishing the original Sony SS-CS5 from SS-CS5M2. Preserve model identifier literally; it is not a measured response equivalence.</extracomment>
    </message>
    <message>
      <source> (restored selection)</source>
      <translation> (gendannet valg)</translation>
    </message>
    <message>
      <source> [custom]</source>
      <translation> [egen]</translation>
    </message>
    <message>
      <source> and </source>
      <translation> og </translation>
    </message>
    <message>
      <source> dB</source>
      <translation> dB</translation>
    </message>
    <message>
      <source> dBFS</source>
      <translation> dBFS</translation>
    </message>
    <message>
      <source> · mono</source>
      <translation> · mono</translation>
    </message>
    <message>
      <source> · no USB microphone detected</source>
      <translation> · ingen USB-mikrofon registreret</translation>
    </message>
    <message>
      <source> · stereo</source>
      <translation> · stereo</translation>
    </message>
    <message>
      <source>%1

Technical details:
%2</source>
      <translation>%1

Tekniske detaljer:
%2</translation>
    </message>
    <message>
      <source>%1
The app remains open; your settings have been kept.</source>
      <translation>%1
Appen forbliver åben; dine indstillinger er bevaret.</translation>
    </message>
    <message>
      <source>%1 %2%3 dB</source>
      <translation>%1 %2%3 dB</translation>
    </message>
    <message>
      <source>%1 / %2
%3
Apply this correction to the %4 route?</source>
      <translation>%1 / %2
%3
Anvend denne korrektion på signalvejen af typen %4?</translation>
    </message>
    <message>
      <source>%1 / %2
%3
Import into your library?</source>
      <translation>%1 / %2
%3
Importér til dit bibliotek?</translation>
    </message>
    <message>
      <source>%1 Hz: measured %2%3 dB; suggested %4%5 dB</source>
      <translation>%1 Hz: målt %2%3 dB; foreslået %4%5 dB</translation>
    </message>
    <message>
      <source>%1 Hz: signal %2, background %3</source>
      <translation>%1 Hz: signal %2, baggrund %3</translation>
      <extracomment>Debug calibration tone amplitude and background noise amplitude. %1 is frequency, %2 signal amplitude, %3 background amplitude. Display only; no change to numerical analysis.</extracomment>
    </message>
    <message>
      <source>%1 Hz: too quiet to measure</source>
      <translation>%1 Hz: for lavt niveau til måling</translation>
    </message>
    <message>
      <source>%1 disconnected. </source>
      <translation>%1 frakoblet. </translation>
    </message>
    <message>
      <source>%1 failed (0x%2)</source>
      <translation>Handlingen mislykkedes: %1 (0x%2)</translation>
    </message>
    <message>
      <source>%1 setup did not finish. %2 itself is installed. Use %3 in the Start menu to retry; see setup details for the reason.</source>
      <translation>Installationen af %1 blev ikke fuldført. Selve %2 er installeret. Brug %3 i Start-menuen for at prøve igen; se installationsoplysningerne for årsagen.</translation>
      <extracomment>Setup failure dialog after app files/shortcuts copied. %1 = stable driver name; %2 = stable app name; %3 = actual currently English Start-menu shortcut name Audio driver setup (not localized Qt button). Setup failure does not prove existing driver absent. Preserve app installed, Start-menu retry and details for reason. Shortcut display-name localization and upgrade cleanup remain open. AI review only; native review unverified.</extracomment>
    </message>
    <message>
      <source>%1 setup did not finish. Retry using the Start menu shortcut.</source>
      <translation>Installationen af %1 blev ikke fuldført. Prøv igen via genvejen i Start-menuen.</translation>
      <extracomment>Nonzero setup exit progress notice, excluding restart-required code 3010. %1 is driver name (SoundCurrent Audio or VB-CABLE). Start-menu shortcut is Audio driver setup. Failure may be installation or update failure; do not imply driver absent. AI review only; native review unverified.</extracomment>
    </message>
    <message>
      <source>%1%2 dB</source>
      <translation>%1%2 dB</translation>
    </message>
    <message>
      <source>.1-10 seconds (default 1.5)</source>
      <extracomment>Reverb decay parameter in seconds inclusive .1–10, default 1.5; used in feedback decay calculation. Numeric examples keep CLI decimal dots.</extracomment>
      <translation>.1-10 sekunder (standard: 1.5)</translation>
    </message>
    <message>
      <source>0-.95 (default .4)</source>
      <extracomment>Reverb damping coefficient inclusive 0–.95, default .4; larger value damps high-frequency recirculation more. Not damping in dB or delay feedback.</extracomment>
      <translation>0-.95 (standard: .4)</translation>
    </message>
    <message>
      <source>0-0.9 (default .35)</source>
      <extracomment>Delay feedback fraction inclusive 0–0.9, default .35. Numeric examples retain decimal dot accepted by from_chars, independent of regional decimal comma.</extracomment>
      <translation>0-0.9 (standard: .35)</translation>
    </message>
    <message>
      <source>1-2000 ms (default 250)</source>
      <extracomment>Delay duration in milliseconds, inclusive 1–2000, default 250. Preserve numeric CLI syntax and ms.</extracomment>
      <translation>1-2000 ms (standard: 250)</translation>
    </message>
    <message>
      <source>1-256 output channels (default: input count)</source>
      <extracomment>CLI output channel count is inclusive 1–256, default equal to input WAVE channel count. Preserve the literal numeric range 1-256. Not input device selection.</extracomment>
      <translation>1-256 outputkanaler (standard: antal inputkanaler)</translation>
    </message>
    <message>
      <source>16 channels</source>
      <translation>16 kanaler</translation>
    </message>
    <message>
      <source>Abort</source>
      <translation>Afbryd</translation>
    </message>
    <message>
      <source>Acoustic</source>
      <translation>Akustisk</translation>
    </message>
    <message>
      <source>Active / passive / unknown</source>
      <translation>Aktiv / passiv / ukendt</translation>
    </message>
    <message>
      <source>Add filter</source>
      <translation>Tilføj filter</translation>
    </message>
    <message>
      <source>Adjust the output from -60 to +12 dB after the EQ. Higher gain can cause clipping.</source>
      <translation>Justér udgangen fra −60 til +12 dB efter EQ'en. Højere forstærkning kan medføre klipning.</translation>
    </message>
    <message>
      <source>Adjust this tone band around the natural voice profile</source>
      <translation>Justér dette frekvensbånd i forhold til profilen for naturlig stemme</translation>
    </message>
    <message>
      <source>Advanced enhancement controls</source>
      <translation>Avancerede kontroller til lydeffekter</translation>
    </message>
    <message>
      <source>Air</source>
      <translation>Luftighed</translation>
    </message>
    <message>
      <source>All brands</source>
      <translation>Alle mærker</translation>
    </message>
    <message>
      <source>All equipment</source>
      <translation>Alt udstyr</translation>
    </message>
    <message>
      <source>All families</source>
      <translation>Alle serier</translation>
    </message>
    <message>
      <source>All manufacturers</source>
      <translation>Alle producenter</translation>
    </message>
    <message>
      <source>All speaker types</source>
      <translation>Alle højttalertyper</translation>
    </message>
    <message>
      <source>All subtypes</source>
      <translation>Alle undertyper</translation>
    </message>
    <message>
      <source>Ambience</source>
      <translation>Rumfornemmelse</translation>
    </message>
    <message>
      <source>Ambience damping</source>
      <translation>Dæmpning af rumrefleksionernes høje frekvenser</translation>
    </message>
    <message>
      <source>Ambience decay</source>
      <translation>Rumrefleksionernes henfaldstid</translation>
    </message>
    <message>
      <source>Amp details</source>
      <translation>Forstærkerdetaljer</translation>
    </message>
    <message>
      <source>Amplifier</source>
      <translation>Forstærker</translation>
    </message>
    <message>
      <source>Amplifier / receiver</source>
      <translation>Forstærker / receiver</translation>
    </message>
    <message>
      <source>Amplifier model profile</source>
      <translation>Profil for forstærkermodel</translation>
    </message>
    <message>
      <source>Amplifier profile details</source>
      <translation>Detaljer om forstærkerprofil</translation>
    </message>
    <message>
      <source>Amplifier profiles require electrical measurements with known speaker load, input, and tone settings. Import a measured correction file; no amplifier curves are assumed from marketing specifications.</source>
      <translation>Forstærkerprofiler kræver elektriske målinger med kendt højttalerbelastning, indgang og toneindstillinger. Importér en målt korrektionsfil; forstærkerkurver udledes ikke af markedsføringsspecifikationer.</translation>
    </message>
    <message>
      <source>An application update was installed. Use Quit and reopen to load it; closing this window keeps the old version running.</source>
      <translation>En appopdatering er installeret. Brug Afslut og åbn appen igen for at indlæse den; hvis du lukker dette vindue, fortsætter den gamle version med at køre.</translation>
    </message>
    <message>
      <source>Another SoundCurrent Studio sink is already running</source>
      <translation>En anden SoundCurrent Studio-udgang kører allerede</translation>
    </message>
    <message>
      <source>Another SoundCurrent app or audio driver setup is running. Quit it before opening this app.</source>
      <translation>En anden SoundCurrent-app eller installation af lyddriver kører. Afslut den, før du åbner denne app.</translation>
    </message>
    <message>
      <source>Another SoundCurrent equalizer is running. Quit EQ or Studio before opening the other app.</source>
      <translation>En anden SoundCurrent-equalizer kører. Afslut EQ eller Studio, før du åbner den anden app.</translation>
    </message>
    <message>
      <source>Another SoundCurrent microphone filter is running</source>
      <translation>Et andet SoundCurrent-mikrofonfilter kører</translation>
    </message>
    <message>
      <source>Another equalizer route is present: %1. Quit it before using SoundCurrent.</source>
      <translation>En anden equalizers signalvej findes: %1. Afslut den, før du bruger SoundCurrent.</translation>
    </message>
    <message>
      <source>Application update</source>
      <translation>Appopdatering</translation>
    </message>
    <message>
      <source>Application updates</source>
      <translation>Appopdateringer</translation>
    </message>
    <message>
      <source>Apply</source>
      <translation>Anvend</translation>
    </message>
    <message>
      <source>Apply amplifier correction?</source>
      <translation>Anvend forstærkerkorrektion?</translation>
      <extracomment>Confirmation title before applying a measured amplifier frequency-response correction. Correction changes EQ, not hardware gain or firmware.</extracomment>
    </message>
    <message>
      <source>Apply correction?</source>
      <translation>Anvend korrektion?</translation>
    </message>
    <message>
      <source>Apply only if these conditions match your system.</source>
      <translation>Anvend kun, hvis disse forhold svarer til dit system.</translation>
      <extracomment>Only apply measured amplifier EQ correction if the measurement setup matches the user’s actual equipment. This prevents using a load-dependent curve indiscriminately.</extracomment>
    </message>
    <message>
      <source>Apply profile</source>
      <translation>Anvend profil</translation>
    </message>
    <message>
      <source>Apply suggested EQ</source>
      <translation>Anvend foreslået EQ</translation>
    </message>
    <message>
      <source>Audio bridge did not start</source>
      <translation>Lydbroen startede ikke</translation>
    </message>
    <message>
      <source>Audio driver setup</source>
      <translation>Installation af lyddriver</translation>
    </message>
    <message>
      <source>Audio driver setup completed. Restart Windows before using SoundCurrent.</source>
      <translation>Opsætningen af lyddriveren er fuldført. Genstart Windows, før du bruger SoundCurrent.</translation>
    </message>
    <message>
      <source>Audio driver setup did not finish: %1</source>
      <translation>Opsætningen af lyddriveren blev ikke fuldført: %1</translation>
    </message>
    <message>
      <source>Audio error: %1</source>
      <translation>Lydfejl: %1</translation>
    </message>
    <message>
      <source>Audio recovery helper</source>
      <translation>Lydgendannelseshjælper</translation>
    </message>
    <message>
      <source>Audio route recovery helper could not start. Repair or reinstall SoundCurrent.</source>
      <translation>Hjælpeprogrammet til gendannelse af lydruten kunne ikke starte. Reparer eller geninstaller SoundCurrent.</translation>
    </message>
    <message>
      <source>Audio setup</source>
      <translation>Lydopsætning</translation>
    </message>
    <message>
      <source>Audio setup could not finish</source>
      <translation>Lydopsætningen kunne ikke fuldføres</translation>
    </message>
    <message>
      <source>Audio setup failed. Restart Windows if VB-CABLE was just installed, then try again.</source>
      <translation>Lydopsætningen mislykkedes. Genstart Windows, hvis VB-CABLE lige er installeret, og prøv derefter igen.</translation>
    </message>
    <message>
      <source>Audio setup is missing. Repair or reinstall SoundCurrent.</source>
      <translation>Værktøjet til lydopsætning mangler. Reparér eller geninstallér SoundCurrent.</translation>
    </message>
    <message>
      <source>Audio setup is running. Processing is paused; the app remains open.</source>
      <translation>Lydopsætning er i gang. Behandlingen er sat på pause; appen forbliver åben.</translation>
    </message>
    <message>
      <source>Auto headroom %1 dB</source>
      <translation>Automatisk niveaumargin %1 dB</translation>
    </message>
    <message>
      <source>Automatic (SoundCurrent Microphone)</source>
      <translation>Automatisk (SoundCurrent Microphone)</translation>
    </message>
    <message>
      <source>Automatic (follow connected devices)</source>
      <translation>Automatisk (følg tilsluttede enheder)</translation>
    </message>
    <message>
      <source>Automatic (follow connected microphones)</source>
      <translation>Automatisk (følg tilsluttede mikrofoner)</translation>
    </message>
    <message>
      <source>Automatic EQ headroom</source>
      <translation>Automatisk EQ-niveaumargin</translation>
    </message>
    <message>
      <source>Automatic audio routing unavailable</source>
      <translation>Automatisk lydrouting er ikke tilgængelig</translation>
    </message>
    <message>
      <source>Automatically shape a connected microphone; click to bypass the microphone EQ</source>
      <translation>Form automatisk lyden fra en tilsluttet mikrofon; klik for at omgå mikrofonens EQ</translation>
    </message>
    <message>
      <source>Back</source>
      <translation>Tilbage</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Balance</source>
      <extracomment>Left/right audio channel balance. Not bank balance or physical equilibrium.</extracomment>
      <translation>Balance</translation>
    </message>
    <message>
      <source>Balance position</source>
      <translation>Balanceposition</translation>
    </message>
    <message>
      <source>Balanced</source>
      <translation>Balanceret</translation>
    </message>
    <message>
      <source>Band %1 gain</source>
      <translation>Forstærkning for bånd %1</translation>
    </message>
    <message>
      <source>Bands</source>
      <extracomment>Frequency bands in an audio equalizer. Not music groups, belts or radio stations.</extracomment>
      <translation>Bånd</translation>
    </message>
    <message>
      <source>Bars beside the sliders show estimated post-EQ levels. Red peak text warns of possible clipping.</source>
      <translation>Indikatorerne ved skyderne viser anslåede niveauer efter EQ'en. Rød tekst ved spidsværdier advarer om mulig klipning.</translation>
    </message>
    <message>
      <source>Bass Boost</source>
      <translation>Basforstærkning</translation>
    </message>
    <message>
      <source>Bass Cut</source>
      <translation>Basreduktion</translation>
    </message>
    <message>
      <source>Bass adds low-frequency weight; Clarity adds high-frequency detail; Ambience adds room reflections; Surround widens stereo; Dynamic Boost compresses and raises quieter material with a peak ceiling. Boosting can increase output level.</source>
      <translation>Bas giver vægt til lave frekvenser; Klarhed giver detaljer i høje frekvenser; Rumfornemmelse giver rumrefleksioner; Surround udvider stereoen; Dynamisk forstærkning komprimerer og hæver svagere materiale med en grænse for spidsniveau. Forstærkning kan hæve udgangsniveauet.</translation>
    </message>
    <message>
      <source>Bass frequency</source>
      <translation>Basfrekvens</translation>
    </message>
    <message>
      <source>Bookshelf speaker</source>
      <translation>Reolhøjttaler</translation>
      <extracomment>Speaker enclosure/installation category in equipment taxonomy. Display label only; original equipmentType key is preserved. Center means center-channel speaker, not a location control.</extracomment>
    </message>
    <message>
      <source>Boxiness</source>
      <translation>Kasselyd</translation>
    </message>
    <message>
      <source>Brand</source>
      <translation>Mærke</translation>
    </message>
    <message>
      <source>Brand, family and model are required (maximum 120 characters each).</source>
      <translation>Mærke, serie og model er påkrævet (højst 120 tegn hver).</translation>
    </message>
    <message>
      <source>Bright</source>
      <translation>Lys</translation>
    </message>
    <message>
      <source>Browse all equipment profiles / editor</source>
      <translation>Gennemse alle udstyrsprofiler / redigér</translation>
    </message>
    <message>
      <source>Bypass Studio processing</source>
      <translation>Omgå Studio-behandling</translation>
    </message>
    <message>
      <source>Cable packet exceeds its capture buffer</source>
      <translation>Kabelpakken overskrider optagelsesbufferens kapacitet</translation>
    </message>
    <message>
      <source>Cable recording endpoint does not support shared 48 kHz stereo float audio</source>
      <translation>Det virtuelle kabels optagelsesslutpunkt understøtter ikke 48 kHz stereolyd i flydende komma-format i delt tilstand</translation>
    </message>
    <message>
      <source>Calibration test signal</source>
      <translation>Testsignal til kalibrering</translation>
    </message>
    <message>
      <source>Calibration tone level</source>
      <translation>Kalibreringstonens niveau</translation>
    </message>
    <message>
      <source>Cancel</source>
      <translation>Annullér</translation>
    </message>
    <message>
      <source>Cancel render</source>
      <translation>Annullér rendering</translation>
    </message>
    <message>
      <source>Cannot acquire the shared SoundCurrent session guard.</source>
      <translation>Kan ikke låse den fælles SoundCurrent-session.</translation>
    </message>
    <message>
      <source>Cannot connect PipeWire streams</source>
      <translation>Kan ikke forbinde PipeWire-streams</translation>
    </message>
    <message>
      <source>Cannot create PipeWire loop</source>
      <translation>Kan ikke oprette PipeWire-løkke</translation>
    </message>
    <message>
      <source>Cannot create PipeWire streams</source>
      <translation>Kan ikke oprette PipeWire-streams</translation>
    </message>
    <message>
      <source>Cannot create amplifier profile folder.</source>
      <translation>Kan ikke oprette mappe til forstærkerprofiler.</translation>
    </message>
    <message>
      <source>Cannot create output WAVE file</source>
      <translation>Kan ikke oprette WAVE-outputfilen</translation>
      <extracomment>Owned offline WAVE writer file-creation failure, including staging output. Does not assert missing disk space or permission denial. WAVE denotes the file format. Contextual AI translation; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Cannot create output staging directory</source>
      <translation>Kan ikke oprette midlertidig outputmappe</translation>
    </message>
    <message>
      <source>Cannot create profile folder.</source>
      <translation>Kan ikke oprette profilmappe.</translation>
    </message>
    <message>
      <source>Cannot create the shared SoundCurrent session guard.</source>
      <translation>Kan ikke oprette låsen til den fælles SoundCurrent-session.</translation>
    </message>
    <message>
      <source>Cannot finish inspecting running equalizers; SoundCurrent will not enable processing.</source>
      <translation>Kan ikke fuldføre kontrollen af aktive equalizere; SoundCurrent aktiverer ikke behandlingen.</translation>
    </message>
    <message>
      <source>Cannot finish saving amplifier profile.</source>
      <translation>Kan ikke fuldføre lagringen af forstærkerprofilen.</translation>
    </message>
    <message>
      <source>Cannot finish saving profile library.</source>
      <translation>Kan ikke fuldføre lagringen af profilbiblioteket.</translation>
    </message>
    <message>
      <source>Cannot finish saving setup.</source>
      <translation>Kan ikke fuldføre lagringen af indstillingerne.</translation>
    </message>
    <message>
      <source>Cannot inspect running equalizers; SoundCurrent will not enable processing.</source>
      <translation>Kan ikke kontrollere aktive equalizere; SoundCurrent aktiverer ikke behandlingen.</translation>
    </message>
    <message>
      <source>Cannot open input WAVE file</source>
      <translation>Kan ikke åbne WAVE-inputfilen</translation>
      <extracomment>Owned offline-render input-file opening failure. WAVE is the file format, not an acoustic wave. Does not assert the cause is missing media or permissions. Preserve WAVE literally. Contextual AI translation; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Cannot protect output staging directory</source>
      <extracomment>POSIX permissions could not be restricted to owner-only on the renderer staging directory. Local temporary files, not encryption or network security. Windows branch does not emit this diagnostic.</extracomment>
      <translation>Den midlertidige outputmappe kan ikke beskyttes</translation>
    </message>
    <message>
      <source>Cannot publish output: %1; choose a new name on a filesystem supporting hard links</source>
      <extracomment>Local atomic no-overwrite hard-link publication failed. %1 is the filesystem error detail and must be preserved verbatim. Publication means moving the completed render into its requested local filename, not Internet sharing. Hard links are filesystem links, not symbolic links.</extracomment>
      <translation>Output kan ikke offentliggøres: %1; vælg et nyt navn på et filsystem, der understøtter hårde links</translation>
    </message>
    <message>
      <source>Cannot read profile library.</source>
      <translation>Kan ikke læse profilbiblioteket.</translation>
    </message>
    <message>
      <source>Cannot read profile or file exceeds 1 MiB.</source>
      <translation>Kan ikke læse profilen, eller filen overstiger 1 MiB.</translation>
    </message>
    <message>
      <source>Cannot read response or file exceeds 1 MiB.</source>
      <translation>Kan ikke læse frekvensgangen, eller filen overstiger 1 MiB.</translation>
    </message>
    <message>
      <source>Cannot save amplifier profile.</source>
      <translation>Kan ikke gemme forstærkerprofilen.</translation>
    </message>
    <message>
      <source>Cannot save profile library.</source>
      <translation>Kan ikke gemme profilbiblioteket.</translation>
    </message>
    <message>
      <source>Cannot save profile.</source>
      <translation>Kan ikke gemme profilen.</translation>
    </message>
    <message>
      <source>Cannot save setup</source>
      <translation>Kan ikke gemme indstillingerne</translation>
    </message>
    <message>
      <source>Cannot seek to WAVE audio</source>
      <translation>Kan ikke gå til positionen for WAVE-lyddata</translation>
      <extracomment>Owned WAVE file-stream seek failure when positioning the read cursor at the audio-data offset. Not device discovery or searching for a song. Preserve WAVE file-format identifier. Contextual AI translation; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Cannot start measurement: %1</source>
      <translation>Kan ikke starte målingen: %1</translation>
    </message>
    <message>
      <source>Capture bytes: %1, noise bytes: %2</source>
      <translation>Optagede byte: %1, støjbyte: %2</translation>
      <extracomment>Debug calibration counts: %1 captured audio bytes, %2 background-noise audio bytes. Counts are byte lengths, not loudness, frequency or monetary amounts.</extracomment>
    </message>
    <message>
      <source>Center</source>
      <translation>Midten</translation>
      <extracomment>Generated audio channel display name only, backed by explicit saved role provenance. Direction is the loudspeaker/channel position from the listener perspective. Center is the center audio channel, not a UI alignment or political meaning. Mono is single-channel audio. Generic channel placeholder is a regional one-based index. Never translate arbitrary saved/custom names.</extracomment>
    </message>
    <message>
      <source>Center channel</source>
      <translation>Center</translation>
      <extracomment>Generated audio channel display name only, backed by explicit saved role provenance. Direction is the loudspeaker/channel position from the listener perspective. Center is the center audio channel, not a UI alignment or political meaning. Mono is single-channel audio. Generic channel placeholder is a regional one-based index. Never translate arbitrary saved/custom names.</extracomment>
    </message>
    <message>
      <source>Center speaker</source>
      <translation>Centerhøjttaler</translation>
      <extracomment>Speaker enclosure/installation category in equipment taxonomy. Display label only; original equipmentType key is preserved. Center means center-channel speaker, not a location control.</extracomment>
    </message>
    <message>
      <source>Change default audio endpoint</source>
      <translation>Ændring af standardlydslutpunkt</translation>
    </message>
    <message>
      <source>Change to detail view mode</source>
      <translation>Skift til detaljeret visningstilstand</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Change to list view mode</source>
      <translation>Skift til listevisningstilstand</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Channel</source>
      <translation>Kanal</translation>
    </message>
    <message>
      <source>Channel %1</source>
      <translation>Kanal %1</translation>
      <extracomment>Generated audio channel display name only, backed by explicit saved role provenance. Direction is the loudspeaker/channel position from the listener perspective. Center is the center audio channel, not a UI alignment or political meaning. Mono is single-channel audio. Generic channel placeholder is a regional one-based index. Never translate arbitrary saved/custom names.</extracomment>
    </message>
    <message>
      <source>Channel configuration count does not match engine</source>
      <translation>Antallet af kanalkonfigurationer svarer ikke til motoren</translation>
    </message>
    <message>
      <source>Channel gain in half dB steps</source>
      <translation>Kanalforstærkning i trin på en halv dB</translation>
    </message>
    <message>
      <source>Channel indexes are one-based and must exist</source>
      <extracomment>Standalone CLI channel numbers start at 1; zero, fractions and numbers beyond the available channel count are rejected. This does not change internal zero-based indexes or routing.</extracomment>
      <translation>Kanalindekser starter ved 1 og skal angive eksisterende kanaler</translation>
    </message>
    <message>
      <source>Channel indexes start at 1. Existing output files are never overwritten.</source>
      <extracomment>CLI channel numbers are one-based. Existing output file protection is unconditional: the renderer refuses overwriting, including races at publication. No option to overwrite is implied.</extracomment>
      <translation>Kanalindekser starter ved 1. Eksisterende outputfiler overskrives aldrig.</translation>
    </message>
    <message>
      <source>Channels and routing</source>
      <translation>Kanaler og routing</translation>
    </message>
    <message>
      <source>Check for updates</source>
      <translation>Søg efter opdateringer</translation>
    </message>
    <message>
      <source>Checking %1 Hz</source>
      <translation>Kontrollerer %1 Hz</translation>
      <extracomment>Calibration worker progress for a single test frequency. %1 is a locale-formatted frequency; Hz is the physical unit.</extracomment>
    </message>
    <message>
      <source>Checking for published updates…</source>
      <translation>Søger efter udgivne opdateringer…</translation>
    </message>
    <message>
      <source>Checks published releases and downloaded installers. No update is installed automatically.</source>
      <translation>Kontrollerer udgivne versioner og hentede installationsprogrammer. Ingen opdatering installeres automatisk.</translation>
    </message>
    <message>
      <source>Choose a name that is not a built-in preset.</source>
      <translation>Vælg et navn, der ikke tilhører en indbygget forudindstilling.</translation>
    </message>
    <message>
      <source>Choose one audio setup action.</source>
      <translation>Vælg præcis én handling til lydopsætning.</translation>
      <extracomment>Exactly one helper action switch must be selected; this is action validation, not an audio-device choice.</extracomment>
    </message>
    <message>
      <source>Choose update folder…</source>
      <translation>Vælg opdateringsmappe…</translation>
    </message>
    <message>
      <source>Chunk extends beyond RIFF bounds</source>
      <translation>Datablokken går ud over RIFF-grænserne</translation>
      <extracomment>Owned file-parser validation: a binary chunk payload length extends beyond the declared RIFF extent. Not an audio clip region or buffer overload. Preserve RIFF identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Cinema speaker</source>
      <translation>Biografhøjttaler</translation>
      <extracomment>Speaker for cinema sound reproduction, not a film file or video player.</extracomment>
    </message>
    <message>
      <source>Clarity</source>
      <translation>Klarhed</translation>
    </message>
    <message>
      <source>Clarity frequency</source>
      <translation>Klarhedsfrekvens</translation>
    </message>
    <message>
      <source>Classical</source>
      <translation>Klassisk musik</translation>
    </message>
    <message>
      <source>Clear Voice</source>
      <translation>Tydelig stemme</translation>
    </message>
    <message>
      <source>Clear imported equipment corrections</source>
      <translation>Ryd importerede udstyrskorrektioner</translation>
    </message>
    <message>
      <source>Click to turn the equalizer on or off</source>
      <translation>Klik for at slå equalizeren til eller fra</translation>
    </message>
    <message>
      <source>Clipping risk · estimated peak %1 dBFS</source>
      <translation>Risiko for klipning · anslået spidsniveau %1 dBFS</translation>
    </message>
    <message>
      <source>Close</source>
      <translation>Luk</translation>
    </message>
    <message>
      <source>Column speaker</source>
      <translation>Søjlehøjttaler</translation>
      <extracomment>Column-format speaker for sound reinforcement, distinct from the floorstanding home speaker category.</extracomment>
    </message>
    <message>
      <source>Conditions</source>
      <translation>Forhold</translation>
    </message>
    <message>
      <source>Connect an output and a microphone before measuring.</source>
      <translation>Tilslut en udgang og en mikrofon før målingen.</translation>
    </message>
    <message>
      <source>Connect your audio</source>
      <translation>Tilslut lyd</translation>
    </message>
    <message>
      <source>Constant-beamwidth speaker</source>
      <translation>Højttaler med konstant strålebredde</translation>
      <extracomment>Constant angular acoustic coverage/beam width across frequency; not constant bandwidth or frequency response. CBT examples verified with official JBL documentation.</extracomment>
    </message>
    <message>
      <source>Copy</source>
      <translation>Kopiér</translation>
      <extracomment>Qt text-editing context-menu action; operates on text selection and clipboard, not files or audio processing.</extracomment>
    </message>
    <message>
      <source>Correction filters:</source>
      <translation>Korrektionsfiltre:</translation>
      <extracomment>Heading for the actual bounded EQ correction filters in the imported profile; JSON identifiers below remain unchanged.</extracomment>
    </message>
    <message>
      <source>Correction profile (*.json)</source>
      <translation>Korrektionsprofil (*.json)</translation>
    </message>
    <message>
      <source>Could not allocate effect state</source>
      <translation>Kunne ikke allokere hukommelse til effekttilstanden</translation>
    </message>
    <message>
      <source>Could not close WAVE output</source>
      <translation>Kunne ikke lukke WAVE-outputfilen</translation>
      <extracomment>Owned WaveWriter finalization failure: closing output file stream reported an error. Not closing the GUI or stopping an audio device. Preserve WAVE identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Could not create a private test folder</source>
      <translation>Kunne ikke oprette en privat testmappe</translation>
    </message>
    <message>
      <source>Could not create microphone configuration folder</source>
      <translation>Kunne ikke oprette mappe til mikrofonkonfiguration</translation>
    </message>
    <message>
      <source>Could not create preset folder.</source>
      <translation>Kunne ikke oprette mappe til forudindstillinger.</translation>
    </message>
    <message>
      <source>Could not create quiet frequency sweep</source>
      <translation>Kunne ikke oprette et svagt frekvenssweep</translation>
    </message>
    <message>
      <source>Could not create test tone</source>
      <translation>Kunne ikke oprette testtone</translation>
    </message>
    <message>
      <source>Could not finish saving preset.</source>
      <translation>Kunne ikke fuldføre lagringen af forudindstillingen.</translation>
    </message>
    <message>
      <source>Could not flush WAVE output</source>
      <translation>Kunne ikke tømme WAVE-outputbufferen</translation>
      <extracomment>Owned WaveWriter finalization failure: flushing buffered file writes failed. Not clearing effects, deleting audio or changing speaker output. Preserve WAVE identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Could not initialize Windows audio COM</source>
      <translation>COM til Windows-lyd kunne ikke initialiseres</translation>
    </message>
    <message>
      <source>Could not open test waveform</source>
      <translation>Kunne ikke åbne testlydfilen</translation>
    </message>
    <message>
      <source>Could not play quiet test audio</source>
      <translation>Kunne ikke afspille svag testlyd</translation>
    </message>
    <message>
      <source>Could not play test audio through the selected output</source>
      <translation>Kunne ikke afspille testlyd gennem den valgte udgang</translation>
    </message>
    <message>
      <source>Could not read output volume</source>
      <translation>Kunne ikke læse udgangslydstyrken</translation>
    </message>
    <message>
      <source>Could not run %1</source>
      <translation>Kunne ikke køre %1</translation>
    </message>
    <message>
      <source>Could not save preset.</source>
      <translation>Kunne ikke gemme forudindstillingen.</translation>
    </message>
    <message>
      <source>Could not start audio setup: %1. The app remains open.</source>
      <translation>Kunne ikke starte lydopsætningen: %1. Appen forbliver åben.</translation>
    </message>
    <message>
      <source>Could not start microphone capture</source>
      <translation>Kunne ikke starte mikrofonoptagelsen</translation>
    </message>
    <message>
      <source>Could not start microphone filter</source>
      <translation>Kunne ikke starte mikrofonfilteret</translation>
    </message>
    <message>
      <source>Could not start output volume safety guard</source>
      <translation>Kunne ikke starte udgangslydstyrkens sikkerhedsbeskyttelse</translation>
    </message>
    <message>
      <source>Could not start the measurement.</source>
      <translation>Kunne ikke starte målingen.</translation>
    </message>
    <message>
      <source>Could not update startup settings.</source>
      <translation>Kunne ikke opdatere startindstillingerne.</translation>
    </message>
    <message>
      <source>Could not write WAVE audio</source>
      <translation>Kunne ikke skrive WAVE-lyddata</translation>
      <extracomment>Owned WaveWriter failure writing sample data into an output file. Not speaker playback or microphone recording. Preserve WAVE identifier. Does not assert a particular disk failure cause. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Could not write WAVE header</source>
      <translation>Kunne ikke skrive WAVE-filheaderen</translation>
      <extracomment>Owned WaveWriter failure writing binary format/header metadata to output file. Header is not a UI title. Preserve WAVE identifier. Does not assert a particular disk failure cause. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Could not write frequency sweep</source>
      <translation>Kunne ikke skrive frekvenssweepet</translation>
    </message>
    <message>
      <source>Could not write microphone configuration</source>
      <translation>Kunne ikke skrive mikrofonkonfigurationen</translation>
    </message>
    <message>
      <source>Could not write test tone</source>
      <translation>Kunne ikke skrive testtonen</translation>
    </message>
    <message>
      <source>Count audio endpoints</source>
      <translation>Optælling af lydslutpunkter</translation>
    </message>
    <message>
      <source>Create a New Folder</source>
      <translation>Opret en ny mappe</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Create new folder</source>
      <translation>Opret ny mappe</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Create profile</source>
      <translation>Opret profil</translation>
    </message>
    <message>
      <source>Current EQ kept.</source>
      <translation>Aktuel EQ bevares.</translation>
    </message>
    <message>
      <source>Custom</source>
      <translation>Egen</translation>
    </message>
    <message>
      <source>Custom copy of %1</source>
      <translation>Tilpasset kopi af %1</translation>
      <extracomment>App-authored editable defaults or provenance for a newly created/imported profile or explicitly saved copy. Existing saved/imported metadata remains verbatim. Never translate filenames, hashes or source IDs substituted for placeholders.</extracomment>
    </message>
    <message>
      <source>Cut</source>
      <translation>Klip</translation>
      <extracomment>Qt text-editing context-menu action; operates on text selection and clipboard, not files or audio processing.</extracomment>
    </message>
    <message>
      <source>Damping</source>
      <translation>Dæmpning</translation>
    </message>
    <message>
      <source>Dance</source>
      <translation>Dansemusik</translation>
    </message>
    <message>
      <source>Date modified</source>
      <translation>Ændringsdato</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Decay</source>
      <translation>Henfaldstid</translation>
    </message>
    <message>
      <source>Deep Bass</source>
      <translation>Dyb bas</translation>
    </message>
    <message>
      <source>Delay / echo</source>
      <translation>Forsinkelse / ekko</translation>
    </message>
    <message>
      <source>Delay settings are outside the supported range</source>
      <translation>Forsinkelsesindstillingerne ligger uden for det understøttede interval</translation>
    </message>
    <message>
      <source>Delay time</source>
      <translation>Forsinkelsestid</translation>
    </message>
    <message>
      <source>Delay wet mix</source>
      <translation>Forsinkelsens effektandel</translation>
    </message>
    <message>
      <source>Delay wet mix percent</source>
      <translation>Forsinkelsens effektandel i procent</translation>
    </message>
    <message>
      <source>Delay wet mix · %1%</source>
      <translation>Forsinkelsens effektandel · %1%</translation>
    </message>
    <message>
      <source>Delete</source>
      <translation>Slet</translation>
      <extracomment>Qt text-editing context-menu action; operates on text selection and clipboard, not files or audio processing.</extracomment>
    </message>
    <message>
      <source>Detail view</source>
      <translation>Detaljeret visning</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Discard</source>
      <translation>Kassér ændringer</translation>
    </message>
    <message>
      <source>Drag curve points or tune the selected band below.</source>
      <translation>Træk kurvens punkter, eller justér det valgte bånd nedenfor.</translation>
    </message>
    <message>
      <source>Drain test playback</source>
      <translation>Afslutning af testafspilning</translation>
    </message>
    <message>
      <source>Drive</source>
      <translation>Drev</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Driver setup failed (code %1). No Windows security settings were changed.</source>
      <translation>Driveropsætningen mislykkedes (kode %1). Ingen Windows-sikkerhedsindstillinger blev ændret.</translation>
    </message>
    <message>
      <source>Dry</source>
      <translation>Uden effekt</translation>
    </message>
    <message>
      <source>Duplicate Studio route</source>
      <translation>Duplikeret Studio-lydforbindelse</translation>
      <extracomment>Same input/output routing edge appears more than once; not duplicated media or road route.</extracomment>
    </message>
    <message>
      <source>Dynamic Boost</source>
      <translation>Dynamisk forstærkning</translation>
    </message>
    <message>
      <source>Dynamics attack</source>
      <translation>Kompressorens attacktid</translation>
    </message>
    <message>
      <source>Dynamics ceiling</source>
      <translation>Kompressorens maksimale spidsniveau</translation>
    </message>
    <message>
      <source>Dynamics makeup</source>
      <translation>Kompressorens kompensationsforstærkning</translation>
    </message>
    <message>
      <source>Dynamics ratio</source>
      <translation>Kompressionsforhold</translation>
    </message>
    <message>
      <source>Dynamics release</source>
      <translation>Kompressorens releasetid</translation>
    </message>
    <message>
      <source>Dynamics threshold</source>
      <translation>Kompressorens tærskel</translation>
    </message>
    <message>
      <source>Echo and space</source>
      <translation>Ekko og rum</translation>
    </message>
    <message>
      <source>Edit / save copy</source>
      <translation>Redigér / gem kopi</translation>
    </message>
    <message>
      <source>Effect preset</source>
      <translation>Effektforudindstilling</translation>
    </message>
    <message>
      <source>Effect tail</source>
      <translation>Effekthale</translation>
    </message>
    <message>
      <source>Effects</source>
      <translation>Effekter</translation>
    </message>
    <message>
      <source>Effects exceed the preview's 128 MiB state budget</source>
      <translation>Effekterne overskrider forhåndslytningens budget på 128 MiB til tilstandshukommelse</translation>
    </message>
    <message>
      <source>Electronic</source>
      <translation>Elektronisk musik</translation>
    </message>
    <message>
      <source>Enhancements outside supported ranges</source>
      <translation>Lydforbedringer uden for de understøttede intervaller</translation>
      <extracomment>Enhancement values fail the supported range validation; not frequency coverage or wireless range.</extracomment>
    </message>
    <message>
      <source>Enumerate audio devices</source>
      <translation>Oplistning af lydenheder</translation>
    </message>
    <message>
      <source>Enumerate endpoints</source>
      <translation>Oplistning af slutpunkter</translation>
    </message>
    <message>
      <source>Equalizer</source>
      <extracomment>Audio frequency-response processor, not social equality.</extracomment>
      <translation>Equalizer</translation>
    </message>
    <message>
      <source>Equalizer and configuration pages</source>
      <translation>Sider til equalizer og indstillinger</translation>
    </message>
    <message>
      <source>Equalizer conflict</source>
      <translation>Konflikt mellem equalizere</translation>
      <extracomment>Warning title when another equalizer or processing owner conflicts with this app. It is a software routing/ownership conflict, not clipping or a bad acoustic measurement.</extracomment>
    </message>
    <message>
      <source>Equalizer curve. Select a point or drag it to adjust frequency and gain.</source>
      <translation>Equalizerkurve. Vælg et punkt, eller træk det for at justere frekvens og forstærkning.</translation>
    </message>
    <message>
      <source>Equalizer is off. Windows selected the physical output directly.</source>
      <translation>Equalizeren er slået fra. Windows valgte den fysiske udgang direkte.</translation>
    </message>
    <message>
      <source>Equalizer is off. Your audio uses its normal output.</source>
      <translation>Equalizeren er slået fra. Lyden bruger sin normale udgang.</translation>
    </message>
    <message>
      <source>Equalizer is still running. Use the tray icon to reopen or quit.</source>
      <translation>Equalizeren kører stadig. Brug ikonet i meddelelsesområdet til at åbne igen eller afslutte.</translation>
    </message>
    <message>
      <source>Equalizer off</source>
      <translation>Equalizer fra</translation>
    </message>
    <message>
      <source>Equalizer on</source>
      <translation>Equalizer til</translation>
    </message>
    <message>
      <source>Equalizer on or off</source>
      <translation>Equalizer til eller fra</translation>
    </message>
    <message>
      <source>Equipment brand</source>
      <translation>Udstyrets mærke</translation>
    </message>
    <message>
      <source>Equipment family</source>
      <translation>Udstyrets serie</translation>
    </message>
    <message>
      <source>Equipment kind must be speaker, microphone or amplifier.</source>
      <translation>Udstyrsarten skal være højttaler, mikrofon eller forstærker.</translation>
    </message>
    <message>
      <source>Equipment profile (*.json)</source>
      <translation>Udstyrsprofil (*.json)</translation>
    </message>
    <message>
      <source>Equipment profile editor</source>
      <translation>Udstyrsprofilredigering</translation>
    </message>
    <message>
      <source>Equipment profiles (*.json)</source>
      <translation>Udstyrsprofiler (*.json)</translation>
    </message>
    <message>
      <source>Equipment profiles by brand family and model</source>
      <translation>Udstyrsprofiler efter mærke, serie og model</translation>
    </message>
    <message>
      <source>Equipment profiles — brand / family / model</source>
      <translation>Udstyrsprofiler — mærke / serie / model</translation>
    </message>
    <message>
      <source>Equipment resource missing.</source>
      <translation>Udstyrsressource mangler.</translation>
    </message>
    <message>
      <source>Equipment subtype</source>
      <translation>Udstyrets undertype</translation>
    </message>
    <message>
      <source>Equipment type</source>
      <translation>Udstyrstype</translation>
    </message>
    <message>
      <source>Estimated output level near band %1</source>
      <translation>Anslået udgangsniveau nær bånd %1</translation>
    </message>
    <message>
      <source>Estimated output near %1: %2 dBFS</source>
      <translation>Anslået udgang nær %1: %2 dBFS</translation>
    </message>
    <message>
      <source>Estimated output peak and clipping risk</source>
      <translation>Anslået udgangsspids og risiko for klipning</translation>
    </message>
    <message>
      <source>Estimated overall output level</source>
      <translation>Anslået samlet udgangsniveau</translation>
    </message>
    <message>
      <source>Estimated overall output peak: %1 dBFS</source>
      <translation>Anslået samlet spidsniveau ved udgangen: %1 dBFS</translation>
    </message>
    <message>
      <source>Estimated peak %1 dBFS</source>
      <translation>Anslået spidsniveau %1 dBFS</translation>
    </message>
    <message>
      <source>Estimated peak: EQ off</source>
      <translation>Anslået spidsniveau: EQ fra</translation>
    </message>
    <message>
      <source>Estimated peak: waiting for audio</source>
      <translation>Anslået spidsniveau: venter på lyd</translation>
    </message>
    <message>
      <source>Estimated post-EQ level near this frequency</source>
      <translation>Anslået niveau efter EQ nær denne frekvens</translation>
    </message>
    <message>
      <source>Estimated post-EQ output peak, including post gain and balance</source>
      <translation>Anslået spidsniveau ved udgangen efter EQ, inklusive udgangsforstærkning og balance</translation>
    </message>
    <message>
      <source>Excessive number of RIFF chunks</source>
      <translation>For mange RIFF-datablokke</translation>
      <extracomment>Owned RIFF parser resource limit: more than 4096 binary chunks. Chunk means container data block, not track, clip or channel. Preserve RIFF identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Exit SoundCurrent Studio and restore normal audio</source>
      <translation>Afslut SoundCurrent Studio og gendan normal lyd</translation>
    </message>
    <message>
      <source>Expanded test language</source>
      <translation>Udvidet testsprog</translation>
    </message>
    <message>
      <source>Expected a JSON equipment profile. Import response text using the response import button.</source>
      <translation>Der forventes en JSON-udstyrsprofil. Importér frekvensgangstekst med knappen til import af frekvensgang.</translation>
    </message>
    <message>
      <source>Expected frequency Hz and relative measured response dB on every data line.</source>
      <translation>Hver datalinje skal indeholde frekvens i Hz og relativ målt frekvensgang i dB.</translation>
    </message>
    <message>
      <source>Export</source>
      <translation>Eksportér</translation>
    </message>
    <message>
      <source>Export JSON</source>
      <translation>Eksportér JSON</translation>
    </message>
    <message>
      <source>Export profile</source>
      <translation>Eksportér profil</translation>
    </message>
    <message>
      <source>FPS Footsteps</source>
      <translation>Fodtrin i FPS-spil</translation>
    </message>
    <message>
      <source>Family</source>
      <translation>Serie</translation>
    </message>
    <message>
      <source>Feedback</source>
      <translation>Tilbagekobling</translation>
    </message>
    <message>
      <source>File</source>
      <translation>Fil</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>File name:</source>
      <translation>Filnavn:</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Files</source>
      <translation>Filer</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Files of type:</source>
      <translation>Filer af type:</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Filter Q</source>
      <translation>Filterets kvalitetsfaktor Q</translation>
      <extracomment>Dimensionless quality factor controlling filter sharpness: higher Q produces a narrower peak. Not a bandwidth in Hz. Stable processing parameter remains q.</extracomment>
    </message>
    <message>
      <source>Filter type</source>
      <translation>Filtertype</translation>
    </message>
    <message>
      <source>Filter values must be numbers.</source>
      <translation>Filterværdier skal være tal.</translation>
    </message>
    <message>
      <source>Filters exceed frequency, gain or Q limits.</source>
      <translation>Filtrene overskrider grænserne for frekvens, forstærkning eller Q.</translation>
    </message>
    <message>
      <source>Flat</source>
      <extracomment>Preset with zero equalizer gain at every frequency. Not an apartment; does not imply muted audio.</extracomment>
      <translation>Flad frekvensgang</translation>
    </message>
    <message>
      <source>Floorstanding speaker</source>
      <translation>Gulvhøjttaler</translation>
      <extracomment>Speaker enclosure/installation category in equipment taxonomy. Display label only; original equipmentType key is preserved. Center means center-channel speaker, not a location control.</extracomment>
    </message>
    <message>
      <source>Folder</source>
      <translation>Mappe</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Forward</source>
      <translation>Frem</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Frequency</source>
      <translation>Frekvens</translation>
    </message>
    <message>
      <source>Frequency Hz</source>
      <translation>Frekvens i Hz</translation>
    </message>
    <message>
      <source>Front L/R enhancements (mono supported); other channels keep their own Studio effects. Zero amounts bypass each enhancement.</source>
      <translation>Effekter til forreste L/R-kanaler (mono understøttes); andre kanaler beholder deres egne Studio-effekter. Nulværdier omgår hver effekt.</translation>
    </message>
    <message>
      <source>Front left</source>
      <translation>Forreste venstre</translation>
      <extracomment>Generated audio channel display name only, backed by explicit saved role provenance. Direction is the loudspeaker/channel position from the listener perspective. Center is the center audio channel, not a UI alignment or political meaning. Mono is single-channel audio. Generic channel placeholder is a regional one-based index. Never translate arbitrary saved/custom names.</extracomment>
    </message>
    <message>
      <source>Front right</source>
      <translation>Forreste højre</translation>
      <extracomment>Generated audio channel display name only, backed by explicit saved role provenance. Direction is the loudspeaker/channel position from the listener perspective. Center is the center audio channel, not a UI alignment or political meaning. Mono is single-channel audio. Generic channel placeholder is a regional one-based index. Never translate arbitrary saved/custom names.</extracomment>
    </message>
    <message>
      <source>Gain</source>
      <extracomment>Audio signal level adjustment in dB, positive or negative. Not financial profit.</extracomment>
      <translation>Forstærkning</translation>
    </message>
    <message>
      <source>Gain / polarity</source>
      <translation>Forstærkning / polaritet</translation>
    </message>
    <message>
      <source>Gain dB</source>
      <translation>Forstærkning i dB</translation>
    </message>
    <message>
      <source>Gaming</source>
      <translation>Spil</translation>
    </message>
    <message>
      <source>Go back</source>
      <translation>Gå tilbage</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Go forward</source>
      <translation>Gå fremad</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Go to the parent directory</source>
      <translation>Gå til den overliggende mappe</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Headphones</source>
      <translation>Hovedtelefoner</translation>
    </message>
    <message>
      <source>Help</source>
      <translation>Hjælp</translation>
    </message>
    <message>
      <source>Hide advanced controls</source>
      <translation>Skjul avancerede kontroller</translation>
    </message>
    <message>
      <source>High pass</source>
      <translation>Højpasfilter</translation>
    </message>
    <message>
      <source>High shelf</source>
      <translation>Højfrekvent shelving-filter</translation>
    </message>
    <message>
      <source>High-shelf filter</source>
      <translation>Shelvingfilter til diskant</translation>
      <extracomment>Shelving EQ: raise/lower the high-frequency region. Do not translate as high-pass; it is not a cutoff filter.</extracomment>
    </message>
    <message>
      <source>Hip-Hop</source>
      <translation>Hip-Hop</translation>
    </message>
    <message>
      <source>Ignore</source>
      <translation>Ignorér</translation>
    </message>
    <message>
      <source>Import</source>
      <translation>Importér</translation>
    </message>
    <message>
      <source>Import JSON</source>
      <translation>Importér JSON</translation>
    </message>
    <message>
      <source>Import create and edit equipment profiles</source>
      <translation>Importér, opret og redigér udstyrsprofiler</translation>
    </message>
    <message>
      <source>Import equipment profile</source>
      <translation>Importér udstyrsprofil</translation>
    </message>
    <message>
      <source>Import measured amplifier correction</source>
      <translation>Importér målt forstærkerkorrektion</translation>
    </message>
    <message>
      <source>Import measured profile</source>
      <translation>Importér målt profil</translation>
    </message>
    <message>
      <source>Import profile?</source>
      <translation>Importér profil?</translation>
    </message>
    <message>
      <source>Import relative measured response</source>
      <translation>Importér relativ målt frekvensgang</translation>
    </message>
    <message>
      <source>Import response text</source>
      <translation>Importér frekvensgangstekst</translation>
    </message>
    <message>
      <source>Imported %1; SHA256 %2</source>
      <translation>Importerede %1; SHA256 %2</translation>
      <extracomment>%1 is an exact imported filename, %2 is its raw hexadecimal SHA256 digest. Preserve SHA256 and both placeholders; no identity or file-content changes.</extracomment>
    </message>
    <message>
      <source>In-wall speaker</source>
      <translation>Vægindbygningshøjttaler</translation>
      <extracomment>Speaker enclosure/installation category in equipment taxonomy. Display label only; original equipmentType key is preserved. Center means center-channel speaker, not a location control.</extracomment>
    </message>
    <message>
      <source>Include preview releases</source>
      <translation>Medtag forhåndsversioner</translation>
    </message>
    <message>
      <source>Incomplete WAVE output</source>
      <translation>Ufuldstændigt WAVE-output</translation>
      <extracomment>Owned WaveWriter finalization validation: written frame count differs from the declared output frame count. Not merely a quiet or short musical passage. Preserve WAVE identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Initialize audio capture</source>
      <translation>Initialisering af lydoptagelse</translation>
    </message>
    <message>
      <source>Initialize microphone recording</source>
      <translation>Initialisering af mikrofonoptagelse</translation>
    </message>
    <message>
      <source>Initialize speaker output</source>
      <translation>Initialisering af højttalerudgang</translation>
    </message>
    <message>
      <source>Initialize test playback</source>
      <translation>Initialisering af testafspilning</translation>
    </message>
    <message>
      <source>Input WAVE file</source>
      <translation>WAVE-inputfil</translation>
    </message>
    <message>
      <source>Input channel</source>
      <translation>Indgangskanal</translation>
    </message>
    <message>
      <source>Input has more channels than the Studio layout; choose a matching or larger layout</source>
      <translation>Input har flere kanaler end Studio-layoutet; vælg et tilsvarende eller større layout</translation>
    </message>
    <message>
      <source>Input is too short for RIFF/WAVE</source>
      <translation>Inputfilen er for kort til RIFF/WAVE</translation>
      <extracomment>Owned parser minimum byte-length check before reading 12-byte RIFF/WAVE header. Not recording duration or speaker response. Preserve RIFF/WAVE identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Input: PCM16/24/32 or float32 RIFF/WAVE. Output: float32 extensible WAVE.</source>
      <extracomment>Input accepts PCM integer 16/24/32 or IEEE float32 in little-endian RIFF/WAVE. Output is float32 WAVE_FORMAT_EXTENSIBLE. Preserve PCM16/24/32, float32 (twice), RIFF/WAVE and WAVE format identifiers.</extracomment>
      <translation>Input: PCM16/24/32 eller float32 RIFF/WAVE. Output: float32 i udvideligt WAVE-format.</translation>
    </message>
    <message>
      <source>Install SoundCurrent Audio using Audio driver setup, then reopen the app to enable the microphone route.</source>
      <translation>Installer SoundCurrent Audio via opsætningen af lyddriveren, og åbn derefter appen igen for at aktivere mikrofonens lydrute.</translation>
    </message>
    <message>
      <source>Install VB-CABLE if missing (administrator approval)</source>
      <translation>Installer VB-CABLE, hvis det mangler (administratorgodkendelse)</translation>
    </message>
    <message>
      <source>Install new packages over this version — no uninstall needed. Presets and profiles are kept. Save your work, use Quit (closing the window keeps it running), install the update, then reopen.</source>
      <translation>Installér nye pakker oven på denne version — afinstallation er ikke nødvendig. Forudindstillinger og profiler bevares. Gem dit arbejde, brug Afslut (hvis du lukker vinduet, fortsætter appen med at køre), installér opdateringen, og åbn igen.</translation>
    </message>
    <message>
      <source>Install or update %1. You do not need to uninstall an older version. Your settings, presets and equipment profiles will be kept.</source>
      <translation>Installer eller opdater %1. Du behøver ikke at afinstallere en ældre version. Dine indstillinger, forudindstillinger og udstyrsprofiler bevares.</translation>
      <extracomment>Installer welcome first paragraph. %1 is stable app name. In-place install/update preserves user settings, listening presets, and equipment response/correction profiles; older app need not be uninstalled first. AI contextual review only; native review unverified.</extracomment>
    </message>
    <message>
      <source>Install or update the shared SoundCurrent Audio driver</source>
      <translation>Installer eller opdater den delte SoundCurrent Audio-driver</translation>
    </message>
    <message>
      <source>Install the Windows audio route using Audio driver setup, then reopen the app.</source>
      <translation>Installer Windows-lydruten via opsætningen af lyddriveren, og åbn derefter appen igen.</translation>
    </message>
    <message>
      <source>Installed version: %1</source>
      <translation>Installeret version: %1</translation>
    </message>
    <message>
      <source>Interface language</source>
      <translation>Grænsefladesprog</translation>
    </message>
    <message>
      <source>Invalid EQ band</source>
      <translation>Ugyldigt EQ-bånd</translation>
    </message>
    <message>
      <source>Invalid RIFF size</source>
      <translation>Ugyldig RIFF-størrelse</translation>
      <extracomment>Owned file-parser validation: declared RIFF extent is too small or exceeds actual file length. Not sample rate or channel count. Preserve RIFF identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Invalid Studio channel count</source>
      <translation>Ugyldigt antal Studio-kanaler</translation>
      <extracomment>Session channel count must be 1..256; audio channels, not stations.</extracomment>
    </message>
    <message>
      <source>Invalid Studio channel name or filters</source>
      <translation>Ugyldigt Studio-kanalnavn eller filterliste</translation>
      <extracomment>Saved channel name must be a nonempty string up to 80 characters, and bands must be an array; filter list, not filter-value validation.</extracomment>
    </message>
    <message>
      <source>Invalid Studio profile channel count</source>
      <translation>Ugyldigt antal kanaler i Studio-profilen</translation>
      <extracomment>Saved profile channels array must be nonempty and contain at most 256 channels.</extracomment>
    </message>
    <message>
      <source>Invalid Studio route</source>
      <translation>Ugyldig Studio-lydforbindelse</translation>
      <extracomment>Saved audio routing edge must contain exactly three entries: output index, input index, mixing coefficient.</extracomment>
    </message>
    <message>
      <source>Invalid Studio routing matrix</source>
      <translation>Ugyldig Studio-routingmatrix</translation>
    </message>
    <message>
      <source>Invalid Studio settings</source>
      <translation>Ugyldige Studio-indstillinger</translation>
    </message>
    <message>
      <source>Invalid WAVE frame alignment or byte rate</source>
      <translation>Ugyldig WAVE-rammejustering eller bytehastighed</translation>
      <extracomment>Owned WAVE file metadata check: block alignment must equal channel count times bytes per sample, and byte rate must equal sample rate times block alignment. Not latency, visual frame alignment or clock sync. Preserve WAVE. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Invalid WAVE read buffer</source>
      <translation>Ugyldig WAVE-læsebuffer</translation>
      <extracomment>Owned WaveReader buffer validation: destination sample count is not a multiple of file channel count. Not a playback device buffer or memory allocation failure. Preserve WAVE identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Invalid audio route: loopback requires a separate render source</source>
      <translation>Ugyldig lydrouting: loopback kræver en separat afspilningskilde</translation>
      <extracomment>Owned Windows routing diagnostic displayed at the desktop boundary. Loopback captures a render source; it must not capture the processed destination, which would feed audio back into itself. No change to routing IDs or backend strings. Contextual AI translation only; native review unverified.</extracomment>
    </message>
    <message>
      <source>Invalid audio setup requester.</source>
      <translation>Ugyldig proces, der anmoder om lydopsætning.</translation>
      <extracomment>The requesting Windows process failed expected executable-name or same-session validation. Requester is a process, not the human user.</extracomment>
    </message>
    <message>
      <source>Invalid calibration audio</source>
      <translation>Ugyldig kalibreringslyd</translation>
    </message>
    <message>
      <source>Invalid channel gain or too many EQ bands</source>
      <translation>Ugyldig kanalforstærkning eller for mange EQ-bånd</translation>
    </message>
    <message>
      <source>Invalid enhancement parameter count</source>
      <translation>Ugyldigt antal parametre til lydforbedring</translation>
      <extracomment>Enhancement array must contain the required number of parameters.</extracomment>
    </message>
    <message>
      <source>Invalid enhancement parameter type</source>
      <translation>Ugyldig datatype for en lydforbedringsparameter</translation>
      <extracomment>Enhancement parameter must be a JSON number; do not reinterpret strings or Boolean values.</extracomment>
    </message>
    <message>
      <source>Invalid enhancement settings</source>
      <translation>Ugyldige indstillinger for lydforbedring</translation>
    </message>
    <message>
      <source>Invalid equalizer settings</source>
      <translation>Ugyldige equalizerindstillinger</translation>
    </message>
    <message>
      <source>Invalid equipment subtype or power type</source>
      <translation>Ugyldig udstyrsundertype eller strømforsyningstype</translation>
    </message>
    <message>
      <source>Invalid filter type</source>
      <translation>Ugyldig filtertype</translation>
      <extracomment>Filter type numeric identifier must be a whole supported enum value; not a file type.</extracomment>
    </message>
    <message>
      <source>Invalid filter.</source>
      <translation>Ugyldigt filter.</translation>
    </message>
    <message>
      <source>Invalid finite numeric argument</source>
      <translation>Ugyldigt endeligt numerisk argument</translation>
      <extracomment>Owned CLI from_chars numeric parser rejects invalid syntax, partial parses, NaN and infinity. Finite means mathematically finite, not final. Numeric option remains locale-independent machine syntax. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Invalid float WAVE format</source>
      <translation>Ugyldigt WAVE-format med flydende komma</translation>
      <extracomment>Owned extensible WAVE floating-point validation: valid-bit field must be 32 for supported float samples. Float means floating-point numbers, not floating playback position. Preserve WAVE identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Invalid measured amplifier profile. Requires model, HTTPS measurement source, conditions, and 1–16 bounded PK/LS/HS filters. See the profile format in the README.</source>
      <translation>Ugyldig målt forstærkerprofil. Model, HTTPS-målekilde, forhold og 1–16 PK/LS/HS-filtre inden for grænserne er påkrævet. Se profilformatet i README.</translation>
    </message>
    <message>
      <source>Invalid microphone tuning</source>
      <translation>Ugyldig mikrofonjustering</translation>
    </message>
    <message>
      <source>Invalid or unordered measured response.</source>
      <translation>Ugyldig eller usorteret målt frekvensgang.</translation>
    </message>
    <message>
      <source>Invalid or unordered response data.</source>
      <translation>Ugyldige eller usorterede frekvensgangsdata.</translation>
    </message>
    <message>
      <source>Invalid output WAVE format</source>
      <translation>Ugyldigt WAVE-outputformat</translation>
      <extracomment>Owned WaveWriter output format validation before file creation. Not an input file parsing error. Preserve WAVE identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Invalid output speaker mask</source>
      <translation>Ugyldig højttalerkanalmaske for output</translation>
      <extracomment>Owned WAVE writer validation of output speaker-position bitmask against output channel count. Metadata error, not disconnected speakers or balance. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Invalid processing buffer</source>
      <extracomment>AudioEngine reported an invalid interleaved sample buffer size relative to its channel count. Internal memory buffer, not an effect preset or playback device.</extracomment>
      <translation>Ugyldig behandlingsbuffer</translation>
    </message>
    <message>
      <source>Invalid profile library.</source>
      <translation>Ugyldigt profilbibliotek.</translation>
    </message>
    <message>
      <source>Invalid response from pactl</source>
      <translation>Ugyldigt svar fra pactl</translation>
    </message>
    <message>
      <source>Invalid response point.</source>
      <translation>Ugyldigt frekvensgangspunkt.</translation>
    </message>
    <message>
      <source>Invalid route indexes or weight</source>
      <translation>Ugyldige kanalindekser eller blandingskoefficient</translation>
      <extracomment>Audio route indices must be whole channel indices in range and mixing coefficient magnitude at most 4; weight means a signed mixing coefficient, not physical mass.</extracomment>
    </message>
    <message>
      <source>Invalid route number</source>
      <translation>Ugyldig numerisk værdi for lydforbindelsen</translation>
      <extracomment>A saved audio routing entry contains a nonnumeric or nonfinite number.</extracomment>
    </message>
    <message>
      <source>Invalid routing buffer</source>
      <extracomment>ChannelRouter rejected interleaved input/output sample spans with incompatible sizes. Internal memory buffer, not physical routing hardware or network buffering.</extracomment>
      <translation>Ugyldig routingbuffer</translation>
    </message>
    <message>
      <source>Invalid routing matrix</source>
      <extracomment>ChannelRouter rejected the supplied matrix dimensions or finite weight values. Mathematical audio mixing/routing matrix, not a visual grid.</extracomment>
      <translation>Ugyldig routingmatrix</translation>
    </message>
    <message>
      <source>Invalid speaker correction filter count</source>
      <translation>Ugyldigt antal højttalerkorrektionsfiltre</translation>
    </message>
    <message>
      <source>Invalid speaker filter type</source>
      <translation>Ugyldig højttalerfiltertype</translation>
    </message>
    <message>
      <source>Invalid speaker identity</source>
      <translation>Ugyldige højttaleroplysninger</translation>
    </message>
    <message>
      <source>Invalid speaker mix format</source>
      <translation>Ugyldigt mixformat for højttalere</translation>
    </message>
    <message>
      <source>Invalid valid-bit count</source>
      <translation>Ugyldigt antal gyldige bits</translation>
      <extracomment>Owned extensible WAVE metadata check: valid bits per sample must be greater than zero and not exceed stored bits per sample. Not file length, bitrate or successful packet count. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Jazz</source>
      <translation>Jazz</translation>
    </message>
    <message>
      <source>Keep current EQ</source>
      <translation>Behold aktuel EQ</translation>
    </message>
    <message>
      <source>L</source>
      <translation>L</translation>
    </message>
    <message>
      <source>Language and regional settings</source>
      <translation>Sprog og regionale indstillinger</translation>
    </message>
    <message>
      <source>Large hall</source>
      <translation>Stor sal</translation>
    </message>
    <message>
      <source>Layout</source>
      <translation>Layout</translation>
    </message>
    <message>
      <source>Left</source>
      <translation>Venstre</translation>
      <extracomment>Generated audio channel display name only, backed by explicit saved role provenance. Direction is the loudspeaker/channel position from the listener perspective. Center is the center audio channel, not a UI alignment or political meaning. Mono is single-channel audio. Generic channel placeholder is a regional one-based index. Never translate arbitrary saved/custom names.</extracomment>
    </message>
    <message>
      <source>Left right balance</source>
      <translation>Venstre/højre-balance</translation>
    </message>
    <message>
      <source>Level indicator refresh interval</source>
      <translation>Opdateringsinterval for niveauindikatorer</translation>
    </message>
    <message>
      <source>Level refresh</source>
      <translation>Niveauopdatering</translation>
    </message>
    <message>
      <source>Library exceeds 16 MiB.</source>
      <translation>Biblioteket overstiger 16 MiB.</translation>
    </message>
    <message>
      <source>Linear route gain (negative = invert)</source>
      <translation>Lineær forstærkning for signalvej (negativ = invertér polaritet)</translation>
    </message>
    <message>
      <source>List audio endpoints</source>
      <translation>Hentning af liste over lydslutpunkter</translation>
    </message>
    <message>
      <source>List of places and bookmarks</source>
      <translation>Liste med steder og bogmærker</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>List view</source>
      <translation>Listevisning</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Listening preset</source>
      <extracomment>Saved equalizer settings for playback. Not a listening device.</extracomment>
      <translation>Lytteforudindstilling</translation>
    </message>
    <message>
      <source>Live</source>
      <translation>Live</translation>
    </message>
    <message>
      <source>Live layouts must fit the selected audio device. Offline rendering and silent meter tests support all 256 channels.</source>
      <translation>Live-layout skal passe til den valgte lydenhed. Offline-rendering og lydløse indikatortest understøtter alle 256 kanaler.</translation>
    </message>
    <message>
      <source>Lo-Fi</source>
      <translation>Lo-Fi</translation>
    </message>
    <message>
      <source>Lock EQ</source>
      <extracomment>Prevent accidental editing of EQ controls; not encryption or a security lock.</extracomment>
      <translation>Lås EQ</translation>
    </message>
    <message>
      <source>Lock equalizer settings</source>
      <translation>Lås equalizerindstillinger</translation>
    </message>
    <message>
      <source>Look in:</source>
      <translation>Kig i:</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Loudness</source>
      <translation>Loudnesskompensation</translation>
    </message>
    <message>
      <source>Low pass</source>
      <translation>Lavpasfilter</translation>
    </message>
    <message>
      <source>Low shelf</source>
      <translation>Lavfrekvent shelving-filter</translation>
    </message>
    <message>
      <source>Low-shelf filter</source>
      <translation>Shelvingfilter til bas</translation>
      <extracomment>Shelving EQ: raise/lower the low-frequency region. Do not translate as low-pass; it is not a cutoff filter.</extracomment>
    </message>
    <message>
      <source>Manufacturer</source>
      <translation>Producent</translation>
    </message>
    <message>
      <source>Maximum of 32 amplifier profiles reached.</source>
      <translation>Grænsen på 32 forstærkerprofiler er nået.</translation>
    </message>
    <message>
      <source>Maximum stereo width</source>
      <translation>Maksimal stereobredde</translation>
    </message>
    <message>
      <source>Measure</source>
      <translation>Mål</translation>
    </message>
    <message>
      <source>Measure speaker room and microphone response</source>
      <translation>Mål højttalernes, rummets og mikrofonens frekvensgang</translation>
    </message>
    <message>
      <source>Measured model correction is added to your listening EQ. You can still add bass or adjust any band. Includes conservative gain limits; room and amplifier effects require a system measurement.</source>
      <translation>Målt modelkorrektion føjes til din lytte-EQ. Du kan stadig tilføje bas eller justere ethvert bånd. Forsigtige forstærkningsgrænser anvendes; rummets og forstærkerens påvirkning kræver en systemmåling.</translation>
    </message>
    <message>
      <source>Measured response</source>
      <translation>Målt respons</translation>
      <extracomment>Editable family default for imported relative frequency-response measurements; not the already-inverted correction EQ.</extracomment>
    </message>
    <message>
      <source>Measurement conditions are required.</source>
      <translation>Måleforhold er påkrævet.</translation>
    </message>
    <message>
      <source>Measurement conditions: %1</source>
      <translation>Måleforhold: %1</translation>
      <extracomment>Label for imported amplifier measurement conditions, including electrical load and tone settings. %1 is verbatim supplied data.</extracomment>
    </message>
    <message>
      <source>Measurement data was incomplete.</source>
      <translation>Måledata var ufuldstændige.</translation>
    </message>
    <message>
      <source>Measurement failed. Try a higher test level or move the mic closer.</source>
      <translation>Målingen mislykkedes. Prøv et højere testniveau, eller flyt mikrofonen tættere på.</translation>
    </message>
    <message>
      <source>Measurement failed: %1</source>
      <translation>Målingen mislykkedes: %1</translation>
      <extracomment>Calibration failure prefix. %1 is a translated owned diagnostic or preserved external technical detail; do not modify device identifiers or paths.</extracomment>
    </message>
    <message>
      <source>Measurement stopped.</source>
      <translation>Målingen blev stoppet.</translation>
    </message>
    <message>
      <source>Measurement: %1</source>
      <translation>Måling: %1</translation>
      <extracomment>Label for verbatim published speaker measurement attribution, not a new calibration run.</extracomment>
    </message>
    <message>
      <source>Metal</source>
      <translation>Metal</translation>
    </message>
    <message>
      <source>Mic gain</source>
      <translation>Mikrofonforstærkning</translation>
    </message>
    <message>
      <source>Microphone</source>
      <translation>Mikrofon</translation>
    </message>
    <message>
      <source>Microphone %1 adjustment</source>
      <translation>Mikrofonjustering %1</translation>
    </message>
    <message>
      <source>Microphone EQ is off.</source>
      <translation>Mikrofonens EQ er slået fra.</translation>
    </message>
    <message>
      <source>Microphone audio bridge did not start</source>
      <translation>Mikrofonens lydbro startede ikke</translation>
    </message>
    <message>
      <source>Microphone capture stopped during playback</source>
      <translation>Mikrofonoptagelsen stoppede under afspilningen</translation>
    </message>
    <message>
      <source>Microphone capture stopped during the test</source>
      <translation>Mikrofonoptagelsen stoppede under testen</translation>
    </message>
    <message>
      <source>Microphone error: %1</source>
      <translation>Mikrofonfejl: %1</translation>
    </message>
    <message>
      <source>Microphone filter did not appear</source>
      <translation>Mikrofonfilteret dukkede ikke op</translation>
    </message>
    <message>
      <source>Microphone filter disappeared</source>
      <translation>Mikrofonfilteret forsvandt</translation>
    </message>
    <message>
      <source>Microphone gain adjustment</source>
      <translation>Justering af mikrofonforstærkning</translation>
    </message>
    <message>
      <source>Microphone input device</source>
      <translation>Mikrofonens inputenhed</translation>
    </message>
    <message>
      <source>Microphone recording consumer stalled</source>
      <translation>Behandlingen af mikrofonoptagelsen er gået i stå</translation>
    </message>
    <message>
      <source>Microphone recording is clipping. Lower microphone gain or boost and repeat the measurement.</source>
      <translation>Mikrofonoptagelsen klipper. Sænk mikrofonforstærkningen eller ekstra forstærkning, og gentag målingen.</translation>
    </message>
    <message>
      <source>Microphone route</source>
      <translation>Mikrofonens signalvej</translation>
    </message>
    <message>
      <source>Microphone start timed out</source>
      <translation>Tidsgrænsen for mikrofonstart blev overskredet</translation>
    </message>
    <message>
      <source>Missing RIFF padding byte</source>
      <translation>Manglende RIFF-udfyldningsbyte</translation>
      <extracomment>Owned RIFF parser validation: the alignment padding byte after an odd-length binary chunk is outside declared extent. Not audio silence, delay or padded samples. Preserve RIFF identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Missing option value</source>
      <translation>Manglende optionsværdi</translation>
      <extracomment>Owned CLI parser error: an option requiring a following argument has no value. Not an unavailable UI choice or lost saved setting. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Missing or incomplete WAVE audio</source>
      <translation>Manglende eller ufuldstændige WAVE-lyddata</translation>
      <extracomment>Owned WaveReader validation: format/data chunk is missing or data length is not a whole number of frames. Not missing microphone, silent samples or absent speaker sound. Preserve WAVE identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Missing, duplicate or oversized WAVE format</source>
      <translation>Manglende, duplikerede eller for store WAVE-formatmetadata</translation>
      <extracomment>Owned WaveReader fmt-chunk validation: no duplicate format chunk and payload size must be 16..4096 bytes. Format means binary metadata, not file extension or project type. Preserve WAVE. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Model</source>
      <translation>Model</translation>
    </message>
    <message>
      <source>Mono</source>
      <translation>Mono</translation>
      <extracomment>Generated audio channel display name only, backed by explicit saved role provenance. Direction is the loudspeaker/channel position from the listener perspective. Center is the center audio channel, not a UI alignment or political meaning. Mono is single-channel audio. Generic channel placeholder is a regional one-based index. Never translate arbitrary saved/custom names.</extracomment>
    </message>
    <message>
      <source>Move toward L or R to reduce the opposite channel; center keeps both at full level</source>
      <translation>Flyt mod L eller R for at sænke den modsatte kanal; midten bevarer begge på fuldt niveau</translation>
    </message>
    <message>
      <source>Movies</source>
      <translation>Film</translation>
    </message>
    <message>
      <source>Multiple WAVE data chunks are unsupported</source>
      <translation>Flere WAVE-datablokke understøttes ikke</translation>
      <extracomment>Owned WaveReader support limitation: a second binary data chunk was encountered. Not multichannel audio, multiple tracks or multiple selected files. Preserve WAVE identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Mute</source>
      <translation>Slå lyden fra</translation>
    </message>
    <message>
      <source>My equipment</source>
      <translation>Mit udstyr</translation>
      <extracomment>App-authored editable defaults or provenance for a newly created/imported profile or explicitly saved copy. Existing saved/imported metadata remains verbatim. Never translate filenames, hashes or source IDs substituted for placeholders.</extracomment>
    </message>
    <message>
      <source>Name</source>
      <translation>Navn</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Natural mic EQ</source>
      <extracomment>Microphone equalization feature intended to produce natural-sounding audio. Not a claim that the microphone has a measured neutral response.</extracomment>
      <translation>EQ til naturlig stemme</translation>
    </message>
    <message>
      <source>Natural mic EQ on · %1</source>
      <translation>EQ til naturlig stemme til · %1</translation>
    </message>
    <message>
      <source>Natural microphone equalizer on or off</source>
      <translation>Equalizer til naturlig mikrofonstemme til eller fra</translation>
    </message>
    <message>
      <source>New folder</source>
      <translation>Ny mappe</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>New profile</source>
      <translation>Ny profil</translation>
      <extracomment>App-authored editable defaults or provenance for a newly created/imported profile or explicitly saved copy. Existing saved/imported metadata remains verbatim. Never translate filenames, hashes or source IDs substituted for placeholders.</extracomment>
    </message>
    <message>
      <source>New rendered WAVE file</source>
      <translation>Ny renderet WAVE-fil</translation>
    </message>
    <message>
      <source>Night Listening</source>
      <translation>Natlytning</translation>
    </message>
    <message>
      <source>No</source>
      <translation>Nej</translation>
    </message>
    <message>
      <source>No imported equipment correction selected.</source>
      <translation>Ingen importeret udstyrskorrektion valgt.</translation>
    </message>
    <message>
      <source>No measured amplifier correction is selected. Marketing frequency-range specifications are insufficient to derive a correction curve.</source>
      <translation>Ingen målt forstærkerkorrektion er valgt. Markedsførte frekvensområder er ikke nok til at udlede en korrektionskurve.</translation>
    </message>
    <message>
      <source>No microphone connected.</source>
      <translation>Ingen mikrofon tilsluttet.</translation>
    </message>
    <message>
      <source>No model correction selected. Your listening EQ works normally.</source>
      <translation>Ingen modelkorrektion valgt. Din lytte-EQ fungerer normalt.</translation>
    </message>
    <message>
      <source>No newer published release found. Downloaded installers are also checked.</source>
      <translation>Ingen nyere udgivet version fundet. Hentede installationsprogrammer kontrolleres også.</translation>
    </message>
    <message>
      <source>No output device is available.</source>
      <translation>Ingen outputenhed er tilgængelig.</translation>
    </message>
    <message>
      <source>No output device is connected.</source>
      <translation>Ingen outputenhed er tilsluttet.</translation>
    </message>
    <message>
      <source>No to All</source>
      <translation>Nej til alle</translation>
    </message>
    <message>
      <source>None — use my own EQ</source>
      <translation>Ingen — brug min egen EQ</translation>
    </message>
    <message>
      <source>Number and date format</source>
      <translation>Tal- og datoformat</translation>
    </message>
    <message>
      <source>Number of equalizer bands</source>
      <translation>Antal equalizerbånd</translation>
    </message>
    <message>
      <source>OK</source>
      <translation>OK</translation>
    </message>
    <message>
      <source>Offline WAVE rendering</source>
      <translation>Offline-rendering af WAVE</translation>
    </message>
    <message>
      <source>Offline editing — keep current playback unchanged</source>
      <translation>Offline-redigering — behold aktuel afspilning uændret</translation>
    </message>
    <message>
      <source>Offline editing. Current playback keeps its last live Studio setup.</source>
      <translation>Offlineredigering. Den aktuelle afspilning beholder den seneste Studio-konfiguration til realtid.</translation>
    </message>
    <message>
      <source>Omnidirectional speaker</source>
      <translation>Rundstrålende højttaler</translation>
      <extracomment>Speaker radiating in all directions; not a microphone pickup pattern.</extracomment>
    </message>
    <message>
      <source>On · Playing through %1</source>
      <translation>Til · Afspiller gennem %1</translation>
    </message>
    <message>
      <source>Only PCM16/24/32 or float32 WAVE is supported</source>
      <translation>Kun PCM16/24/32 eller float32 WAVE understøttes</translation>
      <extracomment>Owned WAVE reader supports signed integer PCM 16/24/32-bit or 32-bit floating-point samples. Preserve PCM16/24/32, float32 and WAVE literally; numbers are bits per sample, not sample rates or channel counts. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Only little-endian RIFF/WAVE is supported</source>
      <translation>Kun RIFF/WAVE med little-endian-byterækkefølge understøttes</translation>
      <extracomment>Owned WAVE reader format support: RIFF/WAVE little-endian byte order only; big-endian RIFX is not supported. Little-endian is byte ordering, not audio phase or low frequencies. Preserve RIFF/WAVE. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Only one SoundCurrent app starts at sign-in. Enabling this replaces the other app's startup setting. It starts in the background when a tray icon is available.</source>
      <translation>Kun én SoundCurrent-app starter ved login. Hvis du aktiverer dette, erstattes den anden apps startindstilling. Den starter i baggrunden, når et ikon i meddelelsesområdet er tilgængeligt.</translation>
    </message>
    <message>
      <source>Open</source>
      <translation>Åbn</translation>
    </message>
    <message>
      <source>Open Studio setup</source>
      <translation>Åbn Studio-indstillinger</translation>
    </message>
    <message>
      <source>Open VB-Audio's control panel for cable latency and internal sample rate. Changing these while audio is running can interrupt playback.</source>
      <translation>Åbn VB-Audios kontrolpanel til kabelforsinkelse og intern samplingsfrekvens. Ændringer, mens lyden kører, kan afbryde afspilningen.</translation>
    </message>
    <message>
      <source>Open VB-CABLE control panel</source>
      <translation>Åbn VB-CABLE-kontrolpanelet</translation>
    </message>
    <message>
      <source>Open audio stream</source>
      <translation>Åbning af lydstrøm</translation>
    </message>
    <message>
      <source>Open cable capture stream</source>
      <translation>Åbning af det virtuelle kabels optagelsesstrøm</translation>
    </message>
    <message>
      <source>Open cable recording endpoint</source>
      <translation>Åbning af det virtuelle kabels optagelsesslutpunkt</translation>
    </message>
    <message>
      <source>Open endpoint</source>
      <translation>Åbning af slutpunkt</translation>
    </message>
    <message>
      <source>Open endpoint volume</source>
      <translation>Åbning af slutpunktets lydstyrkegrænseflade</translation>
    </message>
    <message>
      <source>Open microphone reader</source>
      <translation>Åbning af mikrofonens læsegrænseflade</translation>
    </message>
    <message>
      <source>Open release downloads</source>
      <translation>Åbn versionsdownloads</translation>
    </message>
    <message>
      <source>Open speaker endpoint</source>
      <translation>Åbning af højttalerslutpunkt</translation>
    </message>
    <message>
      <source>Open speaker render stream</source>
      <translation>Åbning af højttalernes afspilningsstrøm</translation>
    </message>
    <message>
      <source>Open test playback writer</source>
      <translation>Åbning af testafspilningens skrivegrænseflade</translation>
    </message>
    <message>
      <source>Open update folder</source>
      <translation>Åbn opdateringsmappe</translation>
    </message>
    <message>
      <source>Opening %1 setup...</source>
      <translation>Åbner installationen af %1...</translation>
      <extracomment>Cable setup launch progress. %1 is stable VB-CABLE name. Opening installer, not claim of successful installation.</extracomment>
    </message>
    <message>
      <source>Orange: measured response where supplied. Teal: correction at 48 kHz. Drag teal control points or edit the table. Saving preserves the reference and creates a custom copy.</source>
      <translation>Orange: målt frekvensgang, når den findes. Turkis: korrektion ved 48 kHz. Træk turkise punkter, eller redigér tabellen. Når du gemmer, bevares referencen, og en egen kopi oprettes.</translation>
    </message>
    <message>
      <source>Outdoor speaker</source>
      <translation>Udendørshøjttaler</translation>
      <extracomment>Speaker designed for outdoor use; not an output device selector.</extracomment>
    </message>
    <message>
      <source>Output already exists; select a new filename</source>
      <translation>Output findes allerede; vælg et nyt filnavn</translation>
    </message>
    <message>
      <source>Output device</source>
      <translation>Outputenhed</translation>
    </message>
    <message>
      <source>Output device is no longer available</source>
      <translation>Outputenheden er ikke længere tilgængelig</translation>
    </message>
    <message>
      <source>Output exceeds the RIFF/WAVE 4 GiB limit</source>
      <translation>Output overskrider RIFF/WAVE-grænsen på 4 GiB</translation>
      <extracomment>Owned WaveWriter size validation: output payload plus RIFF header must fit supported 32-bit RIFF size. Preserve RIFF/WAVE and 4 GiB literally; GiB is binary size, not GB. Does not mean insufficient RAM or free disk space. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Output has no volume channels</source>
      <translation>Udgangen har ingen lydstyrkekanaler</translation>
    </message>
    <message>
      <source>Overall output</source>
      <translation>Samlet udgang</translation>
    </message>
    <message>
      <source>Panel speaker</source>
      <translation>Panelhøjttaler</translation>
      <extracomment>Panel-format speaker category, including planar/electrostatic models; not an application UI panel.</extracomment>
    </message>
    <message>
      <source>Parent directory</source>
      <translation>Ovenliggende mappe</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Paste</source>
      <translation>Indsæt</translation>
      <extracomment>Qt text-editing context-menu action; operates on text selection and clipboard, not files or audio processing.</extracomment>
    </message>
    <message>
      <source>Pause processing and open audio setup. The app stays open and reports the result. Restart Windows after installing the driver.</source>
      <translation>Sæt behandlingen på pause, og åbn lydopsætningen. Appen forbliver åben og viser resultatet. Genstart Windows efter installation af driveren.</translation>
    </message>
    <message>
      <source>Peak</source>
      <translation>Spidsniveau</translation>
    </message>
    <message>
      <source>Peak before clipping: %1; clipped samples: %2; invalid samples: %3</source>
      <extracomment>Successful standalone render statistics. %1 linear absolute peak before hard clipping (not dB); %2 individual clipped samples across channels; %3 invalid/nonfinite input or processing samples. Numbers and processing stay unchanged; labels may avoid plural inflection.</extracomment>
      <translation>Spidsniveau før klipning: %1; klippede samples: %2; ugyldige samples: %3</translation>
    </message>
    <message>
      <source>Peak markers</source>
      <translation>Spidsmarkører</translation>
    </message>
    <message>
      <source>Peaking</source>
      <translation>Klokkefilter</translation>
    </message>
    <message>
      <source>Peaking filter</source>
      <translation>Klokkefilter</translation>
      <extracomment>Bell-shaped parametric EQ filter centered at its frequency; this is not a peak/clipping indicator.</extracomment>
    </message>
    <message>
      <source>Piano</source>
      <translation>Klaver</translation>
    </message>
    <message>
      <source>PipeWire live streams support at most 64 channels; use offline rendering for larger layouts</source>
      <translation>PipeWire-streams i realtid understøtter højst 64 kanaler; brug offlinerendering til større kanallayout</translation>
    </message>
    <message>
      <source>Play quiet test audio and preview suggested playback EQ changes</source>
      <translation>Afspil svag testlyd, og få vist foreslåede ændringer i afspilnings-EQ</translation>
    </message>
    <message>
      <source>Playback</source>
      <translation>Afspilning</translation>
    </message>
    <message>
      <source>Playing a logarithmic sweep from 20 Hz to 25 kHz</source>
      <translation>Afspiller et logaritmisk sweep fra 20 Hz til 25 kHz</translation>
      <extracomment>Calibration worker progress while playing a logarithmic frequency sweep. Preserve the physical 20 Hz and 25 kHz bounds; do not change synthesis or sample rate.</extracomment>
    </message>
    <message>
      <source>Playing quiet test audio. Stop if it is uncomfortable.</source>
      <translation>Afspiller svag testlyd. Stop, hvis det er ubehageligt.</translation>
    </message>
    <message>
      <source>Plug in your microphone to select a microphone profile</source>
      <translation>Tilslut din mikrofon for at vælge en mikrofonprofil</translation>
    </message>
    <message>
      <source>Podcast</source>
      <translation>Podcast</translation>
    </message>
    <message>
      <source>Pop</source>
      <translation>Pop</translation>
    </message>
    <message>
      <source>Portable PA speaker</source>
      <translation>Bærbar PA-højttaler</translation>
      <extracomment>Portable public-address/sound-reinforcement speaker; PA is not a country or personal assistant.</extracomment>
    </message>
    <message>
      <source>Post gain</source>
      <extracomment>Signal level adjustment after EQ processing, in dB; permits attenuation as well as amplification. Not financial profit.</extracomment>
      <translation>Udgangsforstærkning</translation>
    </message>
    <message>
      <source>Post gain after equalization</source>
      <translation>Udgangsforstærkning efter equalizeren</translation>
    </message>
    <message>
      <source>Post gain must be finite and within -84 to +24 dB</source>
      <translation>Udgangsforstærkningen skal være endelig og ligge mellem -84 og +24 dB</translation>
    </message>
    <message>
      <source>Post gain value in decibels</source>
      <translation>Udgangsforstærkningens værdi i decibel</translation>
    </message>
    <message>
      <source>Preset name:</source>
      <translation>Forudindstillingens navn:</translation>
    </message>
    <message>
      <source>Prevent changes to presets, EQ bands, post gain, and balance</source>
      <translation>Forhindr ændringer af forudindstillinger, EQ-bånd, udgangsforstærkning og balance</translation>
    </message>
    <message>
      <source>Profile</source>
      <translation>Profil</translation>
    </message>
    <message>
      <source>Profile details</source>
      <translation>Profildetaljer</translation>
    </message>
    <message>
      <source>Profile exceeds the 1 MiB limit.</source>
      <translation>Profilen overskrider grænsen på 1 MiB.</translation>
    </message>
    <message>
      <source>Profile library exceeds 16 MiB.</source>
      <translation>Profilbiblioteket overstiger 16 MiB.</translation>
    </message>
    <message>
      <source>Profile metadata is too long.</source>
      <translation>Profilens metadata er for lange.</translation>
    </message>
    <message>
      <source>Profile must be readable and smaller than 64 KiB.</source>
      <translation>Profilen skal kunne læses og være mindre end 64 KiB.</translation>
    </message>
    <message>
      <source>Profiles need 1–16 correction filters.</source>
      <translation>Profiler kræver 1–16 korrektionsfiltre.</translation>
    </message>
    <message>
      <source>Published measurement sources: &lt;a href="https://www.spinorama.org/"&gt;Speaker measurements / EQ&lt;/a&gt; · &lt;a href="https://support.daytonaudio.com/microphonecalibrationtool"&gt;Dayton serial calibration&lt;/a&gt; · &lt;a href="https://www.minidsp.com/products/acoustic-measurement/umik-1"&gt;miniDSP serial calibration&lt;/a&gt; · &lt;a href="https://www.neumann.com/de-de/downloads/"&gt;Neumann microphone graphs&lt;/a&gt; · &lt;a href="https://docs.audio-technica.com/us/at2020_english.pdf"&gt;AT2020 response graph&lt;/a&gt; · &lt;a href="https://www.soundstagenetwork.com/index.php?Itemid=154&amp;amp;id=97&amp;amp;option=com_content&amp;amp;view=category"&gt;Amplifier measurements&lt;/a&gt;</source>
      <translation>Udgivne målekilder: &lt;a href="https://www.spinorama.org/"&gt;Højttalermålinger / EQ&lt;/a&gt; · &lt;a href="https://support.daytonaudio.com/microphonecalibrationtool"&gt;Dayton-kalibrering efter serienummer&lt;/a&gt; · &lt;a href="https://www.minidsp.com/products/acoustic-measurement/umik-1"&gt;miniDSP-kalibrering efter serienummer&lt;/a&gt; · &lt;a href="https://www.neumann.com/de-de/downloads/"&gt;Neumann-mikrofongrafer&lt;/a&gt; · &lt;a href="https://docs.audio-technica.com/us/at2020_english.pdf"&gt;AT2020-frekvensgang&lt;/a&gt; · &lt;a href="https://www.soundstagenetwork.com/index.php?Itemid=154&amp;amp;id=97&amp;amp;option=com_content&amp;amp;view=category"&gt;Forstærkermålinger&lt;/a&gt;</translation>
    </message>
    <message>
      <source>Published profiles need an HTTPS measurement source.</source>
      <translation>Udgivne profiler kræver en HTTPS-målekilde.</translation>
    </message>
    <message>
      <source>Published releases could not be checked. Private Studio releases require GitHub access. Use Open release downloads; downloaded installers are still detected locally.</source>
      <translation>Udgivne versioner kunne ikke kontrolleres. Private Studio-versioner kræver GitHub-adgang. Brug Åbn versionsdownloads; hentede installationsprogrammer findes stadig lokalt.</translation>
    </message>
    <message>
      <source>Published response and editable correction curves</source>
      <translation>Udgivet frekvensgang og redigerbare korrektionskurver</translation>
    </message>
    <message>
      <source>Published update %1 is available. Open release downloads, then install over this version and reopen.</source>
      <translation>Udgivet opdatering %1 er tilgængelig. Åbn versionsdownloads, installér oven på denne version, og åbn igen.</translation>
    </message>
    <message>
      <source>Punchy Bass</source>
      <translation>Slagkraftig bas</translation>
    </message>
    <message>
      <source>Quiet logarithmic sweep</source>
      <translation>Svagt logaritmisk frekvenssweep</translation>
    </message>
    <message>
      <source>Quit %1 before uninstalling it.</source>
      <translation>Afslut %1, før du afinstallerer appen.</translation>
      <extracomment>Running application blocks uninstall. %1 is stable product name. Quit means fully exit process, not close/hide window. AI contextual review; native review unverified.</extracomment>
    </message>
    <message>
      <source>Quit %1 before updating. Closing the window keeps it running. No uninstall is needed.</source>
      <translation>Afslut %1 før opdateringen. Appen fortsætter med at køre, når vinduet lukkes. Afinstallation er ikke nødvendig.</translation>
      <extracomment>Running application blocks update. %1 is stable SoundCurrent product name. Quit fully exits process; closing UI leaves it running. In-place updates do not require prior uninstall. AI contextual review; native review unverified.</extracomment>
    </message>
    <message>
      <source>Quit SoundCurrent Studio</source>
      <translation>Afslut SoundCurrent Studio</translation>
    </message>
    <message>
      <source>Quit any running SoundCurrent app before changing the shared driver. Removing one app keeps the driver if the other app still uses it.</source>
      <translation>Afslut alle kørende SoundCurrent-apps, før du ændrer den delte driver. Når én app afinstalleres, bevares driveren, hvis den anden app stadig bruger den.</translation>
    </message>
    <message>
      <source>Quit any running equalizer before driver setup. When removing the last SoundCurrent app, its uninstaller offers VB-CABLE removal. Other software may also need the cable. Extra A/B cables are not bundled.</source>
      <translation>Afslut alle kørende equalizere før driverinstallationen. Når den sidste SoundCurrent-app fjernes, tilbyder dens afinstallationsprogram at fjerne VB-CABLE. Andre programmer kan også have brug for kablet. Ekstra A/B-kabler er ikke inkluderet.</translation>
      <extracomment>Shared virtual cable notice: quit exits the equalizer, not just closes UI. Cable removal is offered when the other SoundCurrent app is absent; user confirmation remains required, silent app removal does not remove cable. A/B refers to separate extra virtual cables, not physical wires. Other software may depend on shared VB-CABLE. AI contextual review only; native review unverified.</extracomment>
    </message>
    <message>
      <source>Quit app</source>
      <extracomment>Exit the process and unload audio processing; closing the window alone keeps the app running.</extracomment>
      <translation>Afslut appen</translation>
    </message>
    <message>
      <source>Quit running SoundCurrent apps and wait for audio recovery to finish before changing the shared audio driver.</source>
      <translation>Luk kørende SoundCurrent-apps, og vent på, at lydgendannelsen er færdig, før du ændrer den delte lyddriver.</translation>
    </message>
    <message>
      <source>Quit the following before changing VB-CABLE: %1.</source>
      <translation>Luk følgende, før du ændrer VB-CABLE: %1.</translation>
    </message>
    <message>
      <source>R</source>
      <translation>R</translation>
    </message>
    <message>
      <source>R&amp;B</source>
      <translation>R&amp;B</translation>
    </message>
    <message>
      <source>Read audio endpoint</source>
      <translation>Læsning af lydslutpunkt</translation>
    </message>
    <message>
      <source>Read audio endpoint ID</source>
      <translation>Læsning af lydslutpunktets ID</translation>
    </message>
    <message>
      <source>Read audio endpoint name</source>
      <translation>Læsning af lydslutpunktets navn</translation>
    </message>
    <message>
      <source>Read audio endpoint properties</source>
      <translation>Læsning af lydslutpunktets egenskaber</translation>
    </message>
    <message>
      <source>Read cable audio</source>
      <translation>Læsning af det virtuelle kabels lyd</translation>
    </message>
    <message>
      <source>Read cable capture interface</source>
      <translation>Hentning af det virtuelle kabels optagelsesgrænseflade</translation>
    </message>
    <message>
      <source>Read cable channel layout</source>
      <translation>Læsning af det virtuelle kabels kanallayout</translation>
    </message>
    <message>
      <source>Read cable packet size</source>
      <translation>Læsning af det virtuelle kabels pakkestørrelse</translation>
    </message>
    <message>
      <source>Read cable speaker mask</source>
      <translation>Læsning af det virtuelle kabels højttalermaske</translation>
    </message>
    <message>
      <source>Read default output ID</source>
      <translation>Læsning af standardudgangens ID</translation>
    </message>
    <message>
      <source>Read default output endpoint</source>
      <translation>Læsning af standardudgangens slutpunkt</translation>
    </message>
    <message>
      <source>Read microphone mix format</source>
      <translation>Læsning af mikrofonens mixformat</translation>
    </message>
    <message>
      <source>Read microphone packet size</source>
      <translation>Læsning af mikrofonens pakkestørrelse</translation>
    </message>
    <message>
      <source>Read microphone samples</source>
      <translation>Læsning af mikrofonens samples</translation>
    </message>
    <message>
      <source>Read next cable packet size</source>
      <translation>Læsning af størrelsen på det virtuelle kabels næste pakke</translation>
    </message>
    <message>
      <source>Read next microphone packet</source>
      <translation>Læsning af næste mikrofonpakke</translation>
    </message>
    <message>
      <source>Read output buffer level</source>
      <translation>Læsning af udgangsbufferens fyldningsniveau</translation>
    </message>
    <message>
      <source>Read output level</source>
      <translation>Læsning af udgangsniveau</translation>
    </message>
    <message>
      <source>Read output mute</source>
      <translation>Læsning af udgangens lydløse status</translation>
    </message>
    <message>
      <source>Read speaker level</source>
      <translation>Læsning af højttalerniveau</translation>
    </message>
    <message>
      <source>Read speaker mix format</source>
      <translation>Læsning af højttalernes mixformat</translation>
    </message>
    <message>
      <source>Read speaker mute</source>
      <translation>Læsning af højttalernes lydløse status</translation>
    </message>
    <message>
      <source>Read speaker render interface</source>
      <translation>Hentning af højttalernes afspilningsgrænseflade</translation>
    </message>
    <message>
      <source>Read speaker volume</source>
      <translation>Læsning af højttalernes lydstyrke</translation>
    </message>
    <message>
      <source>Read test playback padding</source>
      <translation>Læsning af antallet af bufferlagrede lydframes til testafspilning</translation>
    </message>
    <message>
      <source>Read virtual output mix format</source>
      <translation>Læsning af den virtuelle udgangs mixformat</translation>
    </message>
    <message>
      <source>Ready. Effects are dry until enabled.</source>
      <translation>Klar. Effekterne bruges ikke, før de aktiveres.</translation>
    </message>
    <message>
      <source>Rear left</source>
      <translation>Bageste venstre</translation>
      <extracomment>Generated audio channel display name only, backed by explicit saved role provenance. Direction is the loudspeaker/channel position from the listener perspective. Center is the center audio channel, not a UI alignment or political meaning. Mono is single-channel audio. Generic channel placeholder is a regional one-based index. Never translate arbitrary saved/custom names.</extracomment>
    </message>
    <message>
      <source>Rear right</source>
      <translation>Bageste højre</translation>
      <extracomment>Generated audio channel display name only, backed by explicit saved role provenance. Direction is the loudspeaker/channel position from the listener perspective. Center is the center audio channel, not a UI alignment or political meaning. Mono is single-channel audio. Generic channel placeholder is a regional one-based index. Never translate arbitrary saved/custom names.</extracomment>
    </message>
    <message>
      <source>Redo</source>
      <translation>Annuller fortryd</translation>
      <extracomment>Reapply the last undone text edit; does not reset the audio profile.</extracomment>
    </message>
    <message>
      <source>Refresh devices</source>
      <translation>Opdatér enheder</translation>
    </message>
    <message>
      <source>Relative measurements include the speaker, room, and microphone response. The proposed changes are limited to 3 dB per measured frequency.

%1</source>
      <translation>Relative målinger omfatter højttalernes, rummets og mikrofonens frekvensgang. Foreslåede ændringer begrænses til 3 dB pr. målt frekvens.

%1</translation>
    </message>
    <message>
      <source>Release cable audio</source>
      <translation>Frigivelse af det virtuelle kabels lydpakke</translation>
    </message>
    <message>
      <source>Release microphone packet</source>
      <translation>Frigivelse af mikrofonpakke</translation>
    </message>
    <message>
      <source>Release speaker buffer</source>
      <translation>Frigivelse af højttalerbuffer</translation>
    </message>
    <message>
      <source>Release test playback</source>
      <translation>Frigivelse af testafspilningens buffer</translation>
    </message>
    <message>
      <source>Remind me when updates are available or a restart is needed</source>
      <translation>Mind mig om tilgængelige opdateringer eller behov for genstart</translation>
    </message>
    <message>
      <source>Remove VB-CABLE?</source>
      <translation>Fjern VB-CABLE?</translation>
    </message>
    <message>
      <source>Remove selected</source>
      <translation>Fjern valgte</translation>
    </message>
    <message>
      <source>Remove selected filter</source>
      <translation>Fjern valgt filter</translation>
    </message>
    <message>
      <source>Remove selected route</source>
      <translation>Fjern valgt signalvej</translation>
    </message>
    <message>
      <source>Remove the shared VB-CABLE driver too? Other users, recording apps, or voice tools may need it. Confirm to open the official remover, then click Remove Driver. Decline to keep the cable and uninstall only SoundCurrent.</source>
      <translation>Fjern også den delte VB-CABLE-driver? Andre brugere, optagelsesapps eller stemmeværktøjer kan have brug for den. Bekræft for at åbne det officielle fjernelsesprogram, og klik derefter på Remove Driver. Afvis for at beholde kablet og kun afinstallere SoundCurrent.</translation>
    </message>
    <message>
      <source>Rename</source>
      <translation>Omdøb</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Render audio file…</source>
      <translation>Render lydfil…</translation>
    </message>
    <message>
      <source>Render cancelled; no output file published</source>
      <translation>Rendering annulleret; ingen endelig outputfil oprettet</translation>
    </message>
    <message>
      <source>Render: %1</source>
      <translation>Rendering: %1</translation>
    </message>
    <message>
      <source>Rendered %1 -&gt; %2 channels, %3 frames at %4 Hz.</source>
      <extracomment>Successful standalone offline render. %1 input channels, %2 output channels, %3 audio frame count (not per-channel samples), %4 sample rate. Keep Hz and -&gt; identifiers. Count-label wording is allowed to avoid number-dependent noun inflection.</extracomment>
      <translation>Renderet: kanaler %1 -&gt; %2, rammer %3 ved %4 Hz.</translation>
    </message>
    <message>
      <source>Rendered %1 channels. Clipped samples: %2. %3</source>
      <translation>Renderede kanaler: %1. Klippede samples: %2. %3</translation>
    </message>
    <message>
      <source>Rendering…</source>
      <translation>Renderer…</translation>
    </message>
    <message>
      <source>Repair incomplete VB-CABLE installation</source>
      <translation>Reparer ufuldstændig VB-CABLE-installation</translation>
    </message>
    <message>
      <source>Reset</source>
      <translation>Nulstil</translation>
    </message>
    <message>
      <source>Reset all routing</source>
      <translation>Nulstil al routing</translation>
    </message>
    <message>
      <source>Reset enhancements</source>
      <translation>Nulstil lydeffekter</translation>
    </message>
    <message>
      <source>Reset mic tone</source>
      <translation>Nulstil mikrofontone</translation>
    </message>
    <message>
      <source>Reset to flat</source>
      <extracomment>Restore zero gain in all EQ bands. Does not mute playback.</extracomment>
      <translation>Nulstil til flad frekvensgang</translation>
    </message>
    <message>
      <source>Response data (*.txt *.csv *.frd *.cal)</source>
      <translation>Frekvensgangsdata (*.txt *.csv *.frd *.cal)</translation>
    </message>
    <message>
      <source>Response exceeds 4096 points.</source>
      <translation>Frekvensgangen indeholder mere end 4096 punkter.</translation>
    </message>
    <message>
      <source>Response frequencies must increase, with finite bounded values.</source>
      <translation>Frekvenserne skal være stigende med endelige værdier inden for de tilladte grænser.</translation>
    </message>
    <message>
      <source>Response has no usable audio range.</source>
      <translation>Frekvensgangen har intet brugbart lydfrekvensområde.</translation>
    </message>
    <message>
      <source>Response import</source>
      <translation>Importér frekvensgang</translation>
    </message>
    <message>
      <source>Response needs 2–4096 measured points.</source>
      <translation>Frekvensgangen skal indeholde 2–4096 målte punkter.</translation>
    </message>
    <message>
      <source>Restart Windows before using VB-CABLE. Audio setup has completed, but the driver and its settings require a system restart.</source>
      <translation>Genstart Windows, før du bruger VB-CABLE. Lydopsætningen er fuldført, men driveren og dens indstillinger kræver en genstart af systemet.</translation>
    </message>
    <message>
      <source>Restart Windows before using the equalizer or VB-CABLE settings. Audio driver changes need a system restart.</source>
      <translation>Genstart Windows, før du bruger equalizeren eller VB-CABLE-indstillingerne. Ændringer af lyddrivere kræver en systemgenstart.</translation>
    </message>
    <message>
      <source>Restore Defaults</source>
      <translation>Gendan standardindstillinger</translation>
    </message>
    <message>
      <source>Restore the previous EQ setting (Ctrl+Z)</source>
      <translation>Gendan den forrige EQ-indstilling (Ctrl+Z)</translation>
    </message>
    <message>
      <source>Retry</source>
      <translation>Prøv igen</translation>
    </message>
    <message>
      <source>Reverb</source>
      <translation>Rumklang</translation>
    </message>
    <message>
      <source>Reverb settings are outside the supported range</source>
      <translation>Rumklangsindstillingerne ligger uden for det understøttede interval</translation>
    </message>
    <message>
      <source>Reverb wet mix</source>
      <translation>Rumklangens effektandel</translation>
    </message>
    <message>
      <source>Reverb wet mix percent</source>
      <translation>Rumklangens effektandel i procent</translation>
    </message>
    <message>
      <source>Reverb wet mix · %1%</source>
      <translation>Rumklangens effektandel · %1%</translation>
    </message>
    <message>
      <source>Rhythmic echo</source>
      <translation>Rytmisk ekko</translation>
    </message>
    <message>
      <source>Right</source>
      <translation>Højre</translation>
      <extracomment>Generated audio channel display name only, backed by explicit saved role provenance. Direction is the loudspeaker/channel position from the listener perspective. Center is the center audio channel, not a UI alignment or political meaning. Mono is single-channel audio. Generic channel placeholder is a regional one-based index. Never translate arbitrary saved/custom names.</extracomment>
    </message>
    <message>
      <source>Right-to-left test language</source>
      <translation>Testsprog med højre-til-venstre-retning</translation>
    </message>
    <message>
      <source>Rock</source>
      <translation>Rock</translation>
    </message>
    <message>
      <source>Route gain must be between -120 and +12 dB</source>
      <extracomment>Standalone --route OUT:IN:DB matrix entry gain, inclusive -120 to +12 dB; machine numeric syntax and dB identifier unchanged. Not post gain or channel trim, whose ranges differ.</extracomment>
      <translation>Rutens forstærkning skal være mellem -120 og +12 dB</translation>
    </message>
    <message>
      <source>Routes into selected output channel</source>
      <translation>Routninger til den valgte udgangskanal</translation>
    </message>
    <message>
      <source>Save</source>
      <translation>Gem</translation>
    </message>
    <message>
      <source>Save All</source>
      <translation>Gem alle</translation>
    </message>
    <message>
      <source>Save EQ preset</source>
      <translation>Gem EQ-forudindstilling</translation>
    </message>
    <message>
      <source>Save Studio setup</source>
      <translation>Gem Studio-konfiguration</translation>
    </message>
    <message>
      <source>Save modified profile?</source>
      <translation>Gem ændret profil?</translation>
    </message>
    <message>
      <source>Save preset</source>
      <translation>Gem forudindstilling</translation>
    </message>
    <message>
      <source>Save profile</source>
      <translation>Gem profil</translation>
    </message>
    <message>
      <source>Save system response profile</source>
      <translation>Gem profil for systemets frekvensgang</translation>
    </message>
    <message>
      <source>Save your work and quit the running app before continuing. Closing its window keeps it running in the background.</source>
      <translation>Gem dit arbejde, og afslut den kørende app, før du fortsætter. Når vinduet lukkes, fortsætter appen med at køre i baggrunden.</translation>
      <extracomment>Installer welcome second paragraph. Save work and fully quit running app before install/update; closing window hides UI while audio processing keeps running. Generic exit action, not a guessed untranslated Quit button caption. AI contextual review only; native review unverified.</extracomment>
    </message>
    <message>
      <source>Saved preset “%1”.</source>
      <translation>Forudindstillingen »%1« blev gemt.</translation>
    </message>
    <message>
      <source>Search brand, family, model or measurement conditions</source>
      <translation>Søg efter producent, familie, model eller måleforhold</translation>
    </message>
    <message>
      <source>Second virtual cable for microphone EQ</source>
      <translation>Andet virtuelt kabel til mikrofon-EQ</translation>
    </message>
    <message>
      <source>Select a filter to update, or remove filters before adding more</source>
      <translation>Vælg et filter, der skal opdateres, eller fjern filtre, før du tilføjer flere</translation>
    </message>
    <message>
      <source>Select all</source>
      <translation>Markér alt</translation>
      <extracomment>Qt text-editing context-menu action; operates on text selection and clipboard, not files or audio processing.</extracomment>
    </message>
    <message>
      <source>Select band %1</source>
      <translation>Vælg bånd %1</translation>
    </message>
    <message>
      <source>Select this band to edit frequency, gain, and Q</source>
      <translation>Vælg dette bånd for at redigere frekvens, forstærkning og Q</translation>
    </message>
    <message>
      <source>Selected audio device is unavailable</source>
      <translation>Den valgte lydenhed er ikke tilgængelig</translation>
    </message>
    <message>
      <source>Selected band</source>
      <extracomment>Currently selected frequency band in the equalizer.</extracomment>
      <translation>Valgt bånd</translation>
    </message>
    <message>
      <source>Selected band filter Q</source>
      <translation>Filter-Q for det valgte bånd</translation>
    </message>
    <message>
      <source>Selected band frequency</source>
      <translation>Frekvens for det valgte bånd</translation>
    </message>
    <message>
      <source>Selected band gain</source>
      <translation>Forstærkning for det valgte bånd</translation>
    </message>
    <message>
      <source>Selected channel</source>
      <translation>Valgt kanal</translation>
    </message>
    <message>
      <source>Selected channel EQ filters</source>
      <translation>EQ-filtre for den valgte kanal</translation>
    </message>
    <message>
      <source>Selected output device is no longer available</source>
      <translation>Den valgte udgangsenhed er ikke længere tilgængelig</translation>
    </message>
    <message>
      <source>Selected output was unplugged. Switched to automatic output.</source>
      <translation>Den valgte udgang blev frakoblet. Skiftede til automatisk udgang.</translation>
    </message>
    <message>
      <source>Selected speakers are disconnected</source>
      <translation>De valgte højttalere er frakoblet</translation>
    </message>
    <message>
      <source>Separate quiet tones</source>
      <translation>Separate stille toner</translation>
    </message>
    <message>
      <source>Set full speaker level for EQ</source>
      <translation>Indstilling af fuldt højttalerniveau til equalizeren</translation>
    </message>
    <message>
      <source>Set output level</source>
      <translation>Indstilling af udgangsniveau</translation>
    </message>
    <message>
      <source>Set output mute</source>
      <translation>Indstilling af udgangens lydløse status</translation>
    </message>
    <message>
      <source>Set route</source>
      <translation>Indstil routning</translation>
    </message>
    <message>
      <source>Set up %1 for %2.</source>
      <translation>Konfigurer %1 til %2.</translation>
    </message>
    <message>
      <source>Setting up the shared %1 driver...</source>
      <translation>Konfigurerer den delte %1-driver...</translation>
      <extracomment>Native driver setup progress. %1 is stable SoundCurrent Audio name; shared means EQ and Studio share driver ownership, not network sharing. Not completion.</extracomment>
    </message>
    <message>
      <source>Settings &amp;&amp; calibration</source>
      <translation>Indstillinger &amp;&amp; kalibrering</translation>
    </message>
    <message>
      <source>Setup cannot be read or exceeds 8 MiB</source>
      <translation>Konfigurationen kan ikke læses eller er større end 8 MiB</translation>
    </message>
    <message>
      <source>Setup could not check the driver. You can retry with %1 in the app or Start menu.</source>
      <translation>Installationsprogrammet kunne ikke kontrollere driveren. Du kan prøve igen med %1 i appen eller menuen Start.</translation>
    </message>
    <message>
      <source>Setup opens VB-Audio’s signed installer. Click Install Driver, then restart Windows before using the equalizer or VB-CABLE settings.</source>
      <translation>Installationsprogrammet åbner VB-Audios signerede installationsprogram. Klik på Install Driver, og genstart derefter Windows, før du bruger equalizeren eller VB-CABLE-indstillingerne.</translation>
      <extracomment>Missing-driver installer notice (check exit 10). Signed means digitally signed installer software. Install Driver is the exact external button caption and remains English. Restart Windows before using EQ or cable settings. AI contextual review; native review unverified.</extracomment>
    </message>
    <message>
      <source>Shared and channel EQ exceed 64 filters; remove some channel filters</source>
      <translation>Fælles EQ og kanal-EQ overskrider 64 filtre; fjern nogle kanalfiltre</translation>
      <extracomment>Sum of shared EQ and channel EQ must not exceed 64 filters. Remove channel filters, not speaker profiles. Keep the limit 64.</extracomment>
    </message>
    <message>
      <source>Shared audio driver removal did not finish. This app was kept so you can retry. Quit any running SoundCurrent app, then retry uninstalling.</source>
      <translation>Fjernelsen af den delte lyddriver blev ikke fuldført. Denne app blev beholdt, så du kan prøve igen. Afslut alle kørende SoundCurrent-apps, og prøv derefter at afinstallere igen.</translation>
      <extracomment>Native uninstall nonzero failure (excluding restart code 3010) aborts before app payload deletion so user can retry. Shared audio driver means EQ/Studio ownership, not network. Quit any running SoundCurrent apps, not necessarily both products; fully exit rather than hide UI. SoundCurrent is invariant. AI contextual review only; native review unverified.</extracomment>
    </message>
    <message>
      <source>Shortcut</source>
      <translation>Genvej</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Shorter intervals update levels more often and use more CPU; audio delivery may limit the actual rate</source>
      <translation>Kortere intervaller opdaterer niveauerne oftere og bruger mere CPU; lydleveringen kan begrænse den faktiske opdateringshastighed</translation>
    </message>
    <message>
      <source>Show a falling peak hold line on each frequency level</source>
      <translation>Vis en faldende linje, der fastholder spidsniveauet for hvert frekvensniveau</translation>
    </message>
    <message>
      <source>Show advanced controls</source>
      <translation>Vis avancerede kontroller</translation>
    </message>
    <message>
      <source>Show hidden files</source>
      <translation>Vis skjulte filer</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Show peak markers on frequency levels</source>
      <translation>Vis spidsmarkører på frekvensniveauerne</translation>
    </message>
    <message>
      <source>Side left</source>
      <translation>Venstre side</translation>
      <extracomment>Generated audio channel display name only, backed by explicit saved role provenance. Direction is the loudspeaker/channel position from the listener perspective. Center is the center audio channel, not a UI alignment or political meaning. Mono is single-channel audio. Generic channel placeholder is a regional one-based index. Never translate arbitrary saved/custom names.</extracomment>
    </message>
    <message>
      <source>Side right</source>
      <translation>Højre side</translation>
      <extracomment>Generated audio channel display name only, backed by explicit saved role provenance. Direction is the loudspeaker/channel position from the listener perspective. Center is the center audio channel, not a UI alignment or political meaning. Mono is single-channel audio. Generic channel placeholder is a regional one-based index. Never translate arbitrary saved/custom names.</extracomment>
    </message>
    <message>
      <source>Sidebar</source>
      <translation>Sidebjælke</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Size</source>
      <translation>Størrelse</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Size capture buffer</source>
      <translation>Bestemmelse af optagelsesbufferens størrelse</translation>
    </message>
    <message>
      <source>Size output buffer</source>
      <translation>Bestemmelse af udgangsbufferens størrelse</translation>
    </message>
    <message>
      <source>Size test playback buffer</source>
      <translation>Bestemmelse af testafspilningens bufferstørrelse</translation>
    </message>
    <message>
      <source>Slapback echo</source>
      <translation>Slapback-ekko</translation>
    </message>
    <message>
      <source>Small Speakers</source>
      <translation>Små højttalere</translation>
    </message>
    <message>
      <source>Small room</source>
      <translation>Lille rum</translation>
    </message>
    <message>
      <source>Soft Treble</source>
      <translation>Blød diskant</translation>
    </message>
    <message>
      <source>Solo</source>
      <translation>Solo</translation>
    </message>
    <message>
      <source>Sound enhancements</source>
      <translation>Lydforbedringer</translation>
    </message>
    <message>
      <source>SoundCurrent Audio is already present. With driver setup enabled, setup will register this app and keep the shared driver available for the other SoundCurrent app.</source>
      <translation>SoundCurrent Audio findes allerede. Hvis driveropsætning forbliver aktiveret, registrerer installationsprogrammet denne app og holder den delte driver tilgængelig for den anden SoundCurrent-app.</translation>
    </message>
    <message>
      <source>SoundCurrent Audio is ready. Open the app and choose your speakers or headphones.</source>
      <translation>SoundCurrent Audio er klar. Åbn appen, og vælg dine højttalere eller hovedtelefoner.</translation>
    </message>
    <message>
      <source>SoundCurrent Audio provides its own microphone route when installed. With VB-CABLE, simultaneous microphone and speaker EQ needs a separately installed second cable (A or B). Select that cable in recording apps. Automatic prefers the SoundCurrent route when available.</source>
      <translation>SoundCurrent Audio leverer sin egen mikrofonroutning, når det er installeret. Med VB-CABLE kræver samtidig mikrofon- og højttaler-EQ et separat installeret andet kabel (A eller B). Vælg dette kabel i optageapps. Automatisk tilstand foretrækker SoundCurrent-routningen, når den er tilgængelig.</translation>
    </message>
    <message>
      <source>SoundCurrent Audio routes playback through the app. Choose your physical speakers or headphones inside the app. Their hardware drivers are preserved.</source>
      <translation>SoundCurrent Audio sender afspilningen gennem appen. Vælg dine fysiske højttalere eller hovedtelefoner i appen. Deres hardwaredrivere bevares.</translation>
    </message>
    <message>
      <source>SoundCurrent EQ is already processing playback. Quit it before enabling SoundCurrent Studio.</source>
      <translation>SoundCurrent EQ behandler allerede afspilningen. Afslut det, før du aktiverer SoundCurrent Studio.</translation>
    </message>
    <message>
      <source>SoundCurrent Studio offline renderer (no audio device required)</source>
      <extracomment>Standalone renderer works on files without opening an audio device or live stream. Offline means non-live rendering, not a requirement to disconnect from the Internet. Preserve product name SoundCurrent Studio.</extracomment>
      <translation>SoundCurrent Studio offline-renderer (kræver ingen lydenhed)</translation>
    </message>
    <message>
      <source>Soundbar</source>
      <translation>Lydbar</translation>
      <extracomment>Integrated elongated speaker system commonly used with televisions.</extracomment>
    </message>
    <message>
      <source>Source</source>
      <translation>Kilde</translation>
    </message>
    <message>
      <source>Source: %1</source>
      <translation>Kilde: %1</translation>
      <extracomment>Published measurement source URL. %1 is verbatim source data, not a translated equipment identifier.</extracomment>
    </message>
    <message>
      <source>Speaker</source>
      <translation>Højttaler</translation>
    </message>
    <message>
      <source>Speaker &amp;&amp; room calibration</source>
      <translation>Højttaler- &amp;&amp; rumkalibrering</translation>
    </message>
    <message>
      <source>Speaker + room check</source>
      <translation>Kontrol af højttaler + rum</translation>
    </message>
    <message>
      <source>Speaker and room measurement</source>
      <translation>Højttaler- og rummåling</translation>
    </message>
    <message>
      <source>Speaker filter is outside conservative bounds</source>
      <translation>Højttalerfilteret ligger uden for de forsigtigt fastsatte grænser</translation>
    </message>
    <message>
      <source>Speaker manufacturer</source>
      <translation>Højttalerproducent</translation>
    </message>
    <message>
      <source>Speaker mask does not match channel count</source>
      <translation>Højttalerkanalmasken svarer ikke til antallet af kanaler</translation>
      <extracomment>Owned extensible WAVE metadata validation: nonzero speaker-position bitmask must have one set bit per audio channel. Mask means bitmask, not physical speaker covering or EQ curve. Not a hardware fault. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Speaker model correction</source>
      <translation>Korrektion for højttalermodel</translation>
    </message>
    <message>
      <source>Speaker model profile</source>
      <translation>Profil for højttalermodel</translation>
    </message>
    <message>
      <source>Speaker profile details</source>
      <translation>Oplysninger om højttalerprofil</translation>
    </message>
    <message>
      <source>Speaker profile resource is missing</source>
      <translation>Højttalerprofilens ressource mangler</translation>
    </message>
    <message>
      <source>Speaker type</source>
      <translation>Højttalertype</translation>
    </message>
    <message>
      <source>Spinorama AutoEQ: correction gain is limited to %1 and Q to %2. Boosts below %3 are omitted. Your listening preset is added separately.</source>
      <translation>Spinorama AutoEQ: korrektionsforstærkningen begrænses til %1 og Q til %2. Forstærkninger under %3 udelades. Din lytteforudindstilling tilføjes separat.</translation>
      <extracomment>Speaker correction safety policy. %1 is the signed gain limit including dB, %2 is the dimensionless Q limit, %3 is the minimum boost frequency including Hz. Listening preset EQ is summed separately and can exceed these correction-only bounds. Spinorama AutoEQ is a name.</extracomment>
    </message>
    <message>
      <source>Start cable capture</source>
      <translation>Start af det virtuelle kabels lydoptagelse</translation>
    </message>
    <message>
      <source>Start microphone recording</source>
      <translation>Start af mikrofonoptagelse</translation>
    </message>
    <message>
      <source>Start quiet. Raise only if the microphone cannot hear the tones.</source>
      <translation>Start stille. Skru kun op, hvis mikrofonen ikke kan opfange tonerne.</translation>
    </message>
    <message>
      <source>Start speaker output</source>
      <translation>Start af højttalerudgang</translation>
    </message>
    <message>
      <source>Start test playback</source>
      <translation>Start af testafspilning</translation>
    </message>
    <message>
      <source>Start when I sign in</source>
      <translation>Start, når jeg logger ind</translation>
    </message>
    <message>
      <source>Startup</source>
      <translation>Autostart</translation>
    </message>
    <message>
      <source>Step down</source>
      <translation>Formindsk værdien</translation>
      <extracomment>Decrease the numeric spin-box value by one step; not physical movement.</extracomment>
    </message>
    <message>
      <source>Step up</source>
      <translation>Forøg værdien</translation>
      <extracomment>Increase the numeric spin-box value by one step; not physical movement.</extracomment>
    </message>
    <message>
      <source>Stereo</source>
      <translation>Stereo</translation>
    </message>
    <message>
      <source>Stop the microphone calibration before changing the audio driver.</source>
      <translation>Stop mikrofonkalibreringen, før du skifter lyddriver.</translation>
    </message>
    <message>
      <source>Stop tones</source>
      <translation>Stop toner</translation>
    </message>
    <message>
      <source>Studio channel count</source>
      <translation>Antal Studio-kanaler</translation>
    </message>
    <message>
      <source>Studio channel output levels</source>
      <translation>Udgangsniveauer for Studio-kanaler</translation>
    </message>
    <message>
      <source>Studio channels &amp;&amp; effects</source>
      <translation>Studio-kanaler &amp;&amp; effekter</translation>
    </message>
    <message>
      <source>Studio effect preset</source>
      <translation>Studio-effektforudindstilling</translation>
    </message>
    <message>
      <source>Studio profile has an invalid boolean field</source>
      <translation>Studio-profilen indeholder et ugyldigt boolesk felt</translation>
      <extracomment>Saved Studio setup requires a JSON true/false field. Wrong type or missing value is rejected; do not confuse this with an audio level or textual yes/no preference.</extracomment>
    </message>
    <message>
      <source>Studio profile has an invalid numeric field</source>
      <translation>Studio-profilen indeholder et ugyldigt numerisk felt</translation>
      <extracomment>Saved Studio setup numeric field is wrong type, nonfinite or outside its supported range. JSON numbers use invariant syntax; do not reinterpret them according to the interface locale.</extracomment>
    </message>
    <message>
      <source>Studio selected channel</source>
      <translation>Valgt Studio-kanal</translation>
    </message>
    <message>
      <source>Studio settings applied to live playback.</source>
      <translation>Studio-indstillingerne er anvendt på afspilningen i realtid.</translation>
    </message>
    <message>
      <source>Studio settings ready. Enable playback on the Equalizer tab.</source>
      <translation>Studio-indstillingerne er klar. Aktivér afspilning på fanen Equalizer.</translation>
    </message>
    <message>
      <source>Studio setup (*.scstudio)</source>
      <translation>Studio-konfiguration (*.scstudio)</translation>
    </message>
    <message>
      <source>Studio setup loaded for offline review. Uncheck offline editing to use it live.</source>
      <translation>Studio-konfigurationen er indlæst til offlinegennemgang. Fjern markeringen for offlineredigering for at bruge den i realtid.</translation>
    </message>
    <message>
      <source>Studio setup saved.</source>
      <translation>Studio-konfigurationen blev gemt.</translation>
    </message>
    <message>
      <source>Suggested EQ applied. Use Save preset to keep it.</source>
      <translation>Den foreslåede EQ er anvendt. Brug Gem forudindstilling for at beholde den.</translation>
    </message>
    <message>
      <source>Suggested changes to the playback EQ</source>
      <translation>Foreslåede ændringer af afspilnings-EQ</translation>
    </message>
    <message>
      <source>Surround Sound</source>
      <translation>Surroundlyd</translation>
    </message>
    <message>
      <source>Surround speaker</source>
      <translation>Surroundhøjttaler</translation>
      <extracomment>Speaker used for surround audio channels; not an app surround-mode toggle.</extracomment>
    </message>
    <message>
      <source>System response profile editor opened. Saved profiles are available in the equipment library.</source>
      <translation>Profileditoren for systemets frekvensgang er åbnet. Gemte profiler findes i udstyrsbiblioteket.</translation>
    </message>
    <message>
      <source>TV Dialogue</source>
      <translation>TV-dialog</translation>
    </message>
    <message>
      <source>Tail must be between 0 and 30 seconds</source>
      <extracomment>Standalone CLI --tail appends this many seconds of zero input after the source to render delay/reverb decay. Inclusive range 0–30 seconds; not animal anatomy, input duration or reverb decay parameter. Audio processing and flag syntax stay invariant.</extracomment>
      <translation>Effekternes udklangstid skal være mellem 0 og 30 sekunder</translation>
    </message>
    <message>
      <source>Teal: correction EQ. Orange: measured response, when supplied. Vertical scale is relative dB.</source>
      <translation>Turkis: korrektions-EQ. Orange: målt frekvensgang, når den er tilgængelig. Den lodrette skala viser relative dB.</translation>
    </message>
    <message>
      <source>Test channel meters with a silent generated signal</source>
      <translation>Test kanalmålerne med et stille genereret signal</translation>
    </message>
    <message>
      <source>Test level</source>
      <translation>Testniveau</translation>
    </message>
    <message>
      <source>Test level is outside the allowed range</source>
      <translation>Testniveauet ligger uden for det tilladte område</translation>
    </message>
    <message>
      <source>The VB-CABLE package is missing. Repair the SoundCurrent installation.</source>
      <translation>VB-CABLE-pakken mangler. Reparer SoundCurrent-installationen.</translation>
      <extracomment>The bundled official VB-CABLE ZIP is absent. Repair the SoundCurrent app installation; do not change speakers or cable hardware.</extracomment>
    </message>
    <message>
      <source>The audio processor stopped unexpectedly.</source>
      <translation>Lydprocessoren stoppede uventet.</translation>
    </message>
    <message>
      <source>The audio readiness helper is missing. Repair the SoundCurrent installation.</source>
      <translation>Hjælperen til kontrol af lydberedskab mangler. Reparer SoundCurrent-installationen.</translation>
    </message>
    <message>
      <source>The custom library holds up to 256 profiles.</source>
      <translation>Det brugerdefinerede bibliotek kan rumme op til 256 profiler.</translation>
    </message>
    <message>
      <source>The driver manager is not signed. Install a signed SoundCurrent release.</source>
      <translation>Driveradministratoren er ikke signeret. Installer en signeret version af SoundCurrent.</translation>
    </message>
    <message>
      <source>The driver package is incomplete or Windows cannot verify its signature.</source>
      <translation>Driverpakken er ufuldstændig, eller Windows kan ikke bekræfte dens signatur.</translation>
    </message>
    <message>
      <source>The incomplete VB-CABLE installation was removed. Restart Windows, open %1 again, click Install Driver, then restart once more.</source>
      <translation>Den ufuldstændige VB-CABLE-installation blev fjernet. Genstart Windows, åbn %1 igen, klik på Install Driver, og genstart derefter endnu en gang.</translation>
    </message>
    <message>
      <source>The route-preserving setup helper is missing.</source>
      <translation>Opsætningshjælpen, der bevarer lydroutningen, mangler.</translation>
      <extracomment>The installed executable that preserves prior default audio routing while launching driver setup is missing. Route refers to audio endpoints, not navigation/network routing.</extracomment>
    </message>
    <message>
      <source>The shared driver manager is missing. Repair the app installation.</source>
      <translation>Den delte driveradministrator mangler. Reparer appinstallationen.</translation>
    </message>
    <message>
      <source>The update response was invalid. No installer was opened.</source>
      <translation>Opdateringssvaret var ugyldigt. Intet installationsprogram blev åbnet.</translation>
    </message>
    <message>
      <source>This Studio layout has more channels than the output device. Use offline editing or select a compatible device.</source>
      <translation>Denne Studio-konfiguration har flere kanaler end udgangsenheden. Brug offlineredigering, eller vælg en kompatibel enhed.</translation>
    </message>
    <message>
      <source>This imports measured RESPONSE, not already-inverted EQ gains. Confirm equipment type. Absolute SPL needs normalization before import.</source>
      <translation>Dette importerer målt FREKVENSGANG, ikke allerede inverterede EQ-forstærkninger. Bekræft udstyrstypen. Absolut lydtrykniveau skal normaliseres før import.</translation>
    </message>
    <message>
      <source>This profile has changed. Save a custom copy before leaving?</source>
      <translation>Denne profil er ændret. Gem en brugerdefineret kopi, før du forlader den?</translation>
    </message>
    <message>
      <source>Timed out waiting for the equalizer sink: %1</source>
      <translation>Tidsgrænsen blev overskredet under venten på equalizerens sink: %1</translation>
    </message>
    <message>
      <source>Too little test audio reached the microphone. Move it closer or raise the test level slightly.</source>
      <translation>For lidt testlyd nåede mikrofonen. Flyt den tættere på, eller hæv testniveauet lidt.</translation>
    </message>
    <message>
      <source>Too many Studio channel filters</source>
      <translation>For mange filtre på en Studio-kanal</translation>
      <extracomment>Per-channel EQ filter count exceeds 64; unchanged processing bound.</extracomment>
    </message>
    <message>
      <source>Too many Studio routes</source>
      <translation>For mange Studio-lydforbindelser</translation>
      <extracomment>Saved audio routing edge count exceeds channel-count squared.</extracomment>
    </message>
    <message>
      <source>Touring PA speaker</source>
      <translation>PA-højttaler til turnéer</translation>
      <extracomment>Professional sound-reinforcement speaker for touring/live events, distinct from portable PA.</extracomment>
    </message>
    <message>
      <source>Translation coverage: %1 of %2 messages. Missing translations use English. Language packs are unverified and await native-speaker review. Use Quit and reopen to apply changes.</source>
      <translation>Oversættelsesdækning: %1 af %2 meddelelser. Manglende oversættelser vises på engelsk. Sprogpakkerne er ubekræftede og afventer gennemgang af modersmålstalere. Brug Afslut, og åbn igen for at anvende ændringer.</translation>
    </message>
    <message>
      <source>Treble Detail</source>
      <translation>Diskantdetaljer</translation>
    </message>
    <message>
      <source>Trim</source>
      <translation>Niveaujustering</translation>
    </message>
    <message>
      <source>Trim · %1 dB</source>
      <translation>Niveaujustering · %1 dB</translation>
    </message>
    <message>
      <source>Truncated WAVE file</source>
      <translation>Afkortet WAVE-fil</translation>
      <extracomment>Owned WAVE binary read failure: expected bytes cannot be read completely. Does not mean musical trim/crop or an intentionally shortened clip. WAVE denotes the file format. Contextual AI translation; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Truncated chunk header</source>
      <translation>Afkortet datablokheader</translation>
      <extracomment>Owned RIFF parser validation: fewer than eight bytes remain for a chunk header. Header means binary metadata, not a UI title. Not an intentionally trimmed audio clip. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Truncated extensible WAVE format</source>
      <translation>Afkortet udvidelig WAVE-formatstruktur</translation>
      <extracomment>Owned WAVE_FORMAT_EXTENSIBLE header validation: extension structure lacks declared fields or length. Extensible is the format variant, not ability to lengthen music. Preserve WAVE identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Turn equalizer off</source>
      <translation>Slå equalizeren fra</translation>
    </message>
    <message>
      <source>Turn equalizer on</source>
      <translation>Slå equalizeren til</translation>
    </message>
    <message>
      <source>Turn playback off before applying a different live channel layout</source>
      <translation>Slå afspilningen fra, før du anvender et andet kanallayout til behandling i realtid</translation>
    </message>
    <message>
      <source>Turn playback off before applying a new live channel layout</source>
      <translation>Slå afspilningen fra, før du anvender et nyt kanallayout til behandling i realtid</translation>
    </message>
    <message>
      <source>Type</source>
      <translation>Type</translation>
    </message>
    <message>
      <source>Unclassified equipment</source>
      <translation>Uklassificeret udstyr</translation>
      <extracomment>Equipment taxonomy has no more specific classification; not an error, missing device, or user permission status.</extracomment>
    </message>
    <message>
      <source>Undo</source>
      <extracomment>Reverse the previous editable setting change.</extracomment>
      <translation>Fortryd</translation>
    </message>
    <message>
      <source>Undo Studio change</source>
      <translation>Fortryd Studio-ændring</translation>
    </message>
    <message>
      <source>Undo last equalizer change</source>
      <translation>Fortryd sidste equalizerændring</translation>
    </message>
    <message>
      <source>Uninstall</source>
      <translation>Afinstaller</translation>
      <extracomment>Windows Start-menu shortcut action removing this application. Distinct from Quit or closing the UI. Driver removal remains optional shared-driver policy.</extracomment>
    </message>
    <message>
      <source>Unknown</source>
      <translation>Ukendt</translation>
      <extracomment>Qt fallback file chooser caption. Navigation refers to folders/files, never audio playback or signal routing. User filenames and paths must remain unchanged.</extracomment>
    </message>
    <message>
      <source>Unknown option: %1</source>
      <extracomment>Standalone CLI diagnostic for an unrecognized command-line flag. %1 is the exact option spelling supplied by the caller; preserve it verbatim and do not translate/reparse it. Not a missing option value or unknown equipment model.</extracomment>
      <translation>Ukendt indstilling: %1</translation>
    </message>
    <message>
      <source>Unlock EQ</source>
      <translation>Lås EQ op</translation>
    </message>
    <message>
      <source>Unlock controls and finish measurement before editing profiles.</source>
      <translation>Lås kontrollerne op, og afslut målingen, før du redigerer profiler.</translation>
    </message>
    <message>
      <source>Unmute speaker for EQ</source>
      <translation>Aktivering af højttalerlyd til equalizeren</translation>
    </message>
    <message>
      <source>Unsupported Studio profile schema</source>
      <translation>Studio-profilformatet understøttes ikke</translation>
      <extracomment>Saved Studio setup schema/version or required top-level structure is unsupported. This is a file format, not a visual theme or room calibration profile.</extracomment>
    </message>
    <message>
      <source>Unsupported WAVE rate or channel count</source>
      <translation>WAVE-samplingsfrekvens eller kanalantal understøttes ikke</translation>
      <extracomment>Owned WaveReader file-format support limit: channel count must be 1..maxChannels and sample rate 8000..384000 Hz. Rate means sample rate, not bitrate or playback speed. Not live device capability. Preserve WAVE. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Unsupported cable channel count</source>
      <translation>Antallet af kabelkanaler understøttes ikke</translation>
    </message>
    <message>
      <source>Unsupported equipment profile schema (expected 2).</source>
      <translation>Udstyrsprofilens skema understøttes ikke (forventet: 2).</translation>
    </message>
    <message>
      <source>Unsupported extensible WAVE subtype</source>
      <translation>Undertype af udvideligt WAVE-format understøttes ikke</translation>
      <extracomment>Owned WAVE_FORMAT_EXTENSIBLE subtype identifier validation: GUID tail is unsupported. Not a physical speaker model or plugin type. Preserve WAVE. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Unsupported filter type.</source>
      <translation>Filtertypen understøttes ikke.</translation>
    </message>
    <message>
      <source>Unsupported microphone channel layout</source>
      <translation>Mikrofonens kanallayout understøttes ikke</translation>
    </message>
    <message>
      <source>Unsupported recording format</source>
      <translation>Optagelsesformatet understøttes ikke</translation>
    </message>
    <message>
      <source>Unsupported speaker channel layout or sample rate</source>
      <translation>Højttalernes kanallayout eller samplingsfrekvens understøttes ikke</translation>
    </message>
    <message>
      <source>Unsupported speaker mix sample format</source>
      <translation>Sampleformatet for højttalermixet understøttes ikke</translation>
    </message>
    <message>
      <source>Unsupported speaker profile schema</source>
      <translation>Højttalerprofilens skema understøttes ikke</translation>
    </message>
    <message>
      <source>Update %1 is downloaded: %2. Quit, install over the existing app, then reopen.</source>
      <translation>Opdateringen %1 er hentet: %2. Afslut, installér oven på den eksisterende app, og åbn derefter igen.</translation>
    </message>
    <message>
      <source>Update download folder</source>
      <translation>Mappe til hentede opdateringer</translation>
    </message>
    <message>
      <source>Update selected</source>
      <translation>Opdatér valgte</translation>
    </message>
    <message>
      <source>Usage: %1 [options]</source>
      <extracomment>CLI usage line. %1 is invariant executable name, required flags and example filenames. Translate only the surrounding usage/options words; flags and filenames remain literal.</extracomment>
      <translation>Brug: %1 [indstillinger]</translation>
    </message>
    <message>
      <source>Use a quiet room. Measures speakers, room, and microphone together; results include the mic response.</source>
      <translation>Brug et stille rum. Måler højttalere, rum og mikrofon sammen; resultaterne omfatter mikrofonens frekvensgang.</translation>
    </message>
    <message>
      <source>Use system language</source>
      <translation>Brug systemets sprog</translation>
    </message>
    <message>
      <source>Use system locale</source>
      <extracomment>Use the operating system regional number/date formatting settings; independent of interface language.</extracomment>
      <translation>Brug systemets landestandard</translation>
    </message>
    <message>
      <source>User imported relative frequency response; specify microphone orientation / serial, or speaker measurement conditions before use.</source>
      <translation>Brugerimporteret relativ frekvensrespons; angiv mikrofonens retning / serienummer eller højttalerens måleforhold før brug.</translation>
      <extracomment>App-authored editable defaults or provenance for a newly created/imported profile or explicitly saved copy. Existing saved/imported metadata remains verbatim. Never translate filenames, hashes or source IDs substituted for placeholders.</extracomment>
    </message>
    <message>
      <source>User-created correction; enter equipment and measurement conditions.</source>
      <translation>Brugeroprettet korrektion; indtast udstyr og måleforhold.</translation>
      <extracomment>App-authored editable defaults or provenance for a newly created/imported profile or explicitly saved copy. Existing saved/imported metadata remains verbatim. Never translate filenames, hashes or source IDs substituted for placeholders.</extracomment>
    </message>
    <message>
      <source>User-created profile</source>
      <translation>Brugeroprettet profil</translation>
      <extracomment>App-authored editable defaults or provenance for a newly created/imported profile or explicitly saved copy. Existing saved/imported metadata remains verbatim. Never translate filenames, hashes or source IDs substituted for placeholders.</extracomment>
    </message>
    <message>
      <source>VB-CABLE has a driver record but no usable audio endpoints. Setup offers repair: remove the driver, restart, reinstall, and restart again.</source>
      <translation>VB-CABLE er registreret som driver, men har ingen brugbare lydendepunkter. Installationsprogrammet tilbyder reparation: fjern driveren, genstart, installer den igen, og genstart endnu en gang.</translation>
      <extracomment>Incomplete driver registration notice (check exit 11). Audio endpoints mean Windows playback/recording devices. Preserve two computer restarts and the remove/reinstall order. Not a claim that repair completed. VB-CABLE is invariant. Contextual AI review only; native review unverified.</extracomment>
    </message>
    <message>
      <source>VB-CABLE is already installed. If it was just installed or updated, restart Windows before using the equalizer or VB-CABLE settings. Otherwise, select your speakers in SoundCurrent.</source>
      <translation>VB-CABLE er allerede installeret. Hvis det lige er blevet installeret eller opdateret, skal du genstarte Windows, før du bruger equalizeren eller VB-CABLE-indstillingerne. Ellers skal du vælge dine højttalere i SoundCurrent.</translation>
    </message>
    <message>
      <source>VB-CABLE is already present. It will be reused. SoundCurrent restores your normal output when switched off or when you use %1.</source>
      <translation>VB-CABLE findes allerede og genbruges. SoundCurrent gendanner din normale udgang, når det slås fra, eller når du bruger %1.</translation>
    </message>
    <message>
      <source>VB-CABLE is not installed. Open "%1", then restart Windows before opening the cable settings.</source>
      <translation>VB-CABLE er ikke installeret. Åbn "%1", og genstart Windows, før du åbner kabelindstillingerne.</translation>
    </message>
    <message>
      <source>VB-CABLE is not present. Restart Windows if requested, then retry audio setup.</source>
      <translation>VB-CABLE er ikke til stede. Genstart Windows, hvis du blev bedt om det, og prøv lydopsætningen igen.</translation>
    </message>
    <message>
      <source>VB-CABLE is still present. If removal requested a restart, restart Windows and retry SoundCurrent uninstall; otherwise finish Remove Driver in the official setup.</source>
      <translation>VB-CABLE er stadig til stede. Hvis fjernelsen krævede en genstart, skal du genstarte Windows og prøve at afinstallere SoundCurrent igen; ellers skal du fuldføre Remove Driver i det officielle installationsprogram.</translation>
    </message>
    <message>
      <source>VB-CABLE package checksum mismatch. Repair the installation.</source>
      <translation>Kontrolsummen for VB-CABLE-pakken stemmer ikke. Reparer installationen.</translation>
      <extracomment>The bundled ZIP SHA-256 differs from the pinned official package checksum. It is rejected before extraction/execution. This is file integrity, not audio level or signal quality.</extracomment>
    </message>
    <message>
      <source>VB-CABLE removal did not finish. This app was kept so you can retry.</source>
      <translation>Fjernelsen af VB-CABLE blev ikke fuldført. Denne app blev beholdt, så du kan prøve igen.</translation>
      <extracomment>Cable uninstall nonzero failure excluding restart code 3010 aborts before app payload deletion. App retained for retry. NSIS caller appends newline and actual helper output as $1; never put runtime variables in translations. VB-CABLE invariant. Contextual AI review only; native review unverified.</extracomment>
    </message>
    <message>
      <source>VB-CABLE routes playback through the app. Choose speakers inside SoundCurrent. VB-CABLE is VB-Audio donationware: https://vb-cable.com — donations are welcome.</source>
      <translation>VB-CABLE fører afspilningen gennem appen. Vælg højttalere i SoundCurrent. VB-CABLE er software fra VB-Audio, der støttes af donationer: https://vb-cable.com — donationer er velkomne.</translation>
      <extracomment>Cable audio page routing and donation notice. Software routes system playback through SoundCurrent to physical output selected inside app. Donationware means supported by voluntary donations, not mandatory payment. Preserve VB-CABLE twice, SoundCurrent, VB-Audio and exact donation URL. Contextual AI review only; native review unverified.</extracomment>
    </message>
    <message>
      <source>VB-CABLE settings</source>
      <translation>VB-CABLE-indstillinger</translation>
    </message>
    <message>
      <source>VB-CABLE settings could not open. Restart Windows if the driver was just installed or updated, then try again.</source>
      <translation>VB-CABLE-indstillingerne kunne ikke åbnes. Genstart Windows, hvis driveren lige er blevet installeret eller opdateret, og prøv igen.</translation>
    </message>
    <message>
      <source>VB-CABLE setup finished. Restart Windows now before using the equalizer or VB-CABLE settings. Your prior audio defaults were preserved where still available.</source>
      <translation>VB-CABLE-opsætningen er fuldført. Genstart Windows nu, før du bruger equalizeren eller VB-CABLE-indstillingerne. Dine tidligere standardlydenheder blev bevaret, hvor de stadig var tilgængelige.</translation>
    </message>
    <message>
      <source>VB-CABLE setup requires a Windows restart. Restart before using the equalizer or opening VB-CABLE settings.</source>
      <translation>Opsætning af VB-CABLE kræver en genstart af Windows. Genstart, før du bruger equalizeren eller åbner indstillingerne for VB-CABLE.</translation>
    </message>
    <message>
      <source>VB-CABLE setup was cancelled or did not finish (code %1). SoundCurrent was retained for retry.</source>
      <translation>VB-CABLE-opsætningen blev annulleret eller ikke fuldført (kode %1). SoundCurrent forbliver installeret, så du kan prøve igen.</translation>
    </message>
    <message>
      <source>VB-CABLE still has no usable playback/recording endpoints. Complete Remove Driver in the official setup, restart Windows, then open %1 again to reinstall. Windows Sound settings must have CABLE Input and CABLE Output enabled.</source>
      <translation>VB-CABLE har stadig ingen brugbare afspilnings- eller optageenheder. Fuldfør Remove Driver i det officielle installationsprogram, genstart Windows, og åbn %1 igen for at geninstallere driveren. CABLE Input og CABLE Output skal være aktiveret i Windows' lydindstillinger.</translation>
    </message>
    <message>
      <source>VB-CABLE was kept because the other SoundCurrent app is installed. Remove it with the last app if no other software needs it.</source>
      <translation>VB-CABLE blev bevaret, fordi den anden SoundCurrent-app er installeret. Fjern det sammen med den sidste app, hvis ingen anden software har brug for det.</translation>
    </message>
    <message>
      <source>Virtual output requires a supported 48 kHz float channel layout</source>
      <translation>Den virtuelle udgang kræver et understøttet kanallayout ved 48 kHz i flydende komma-format</translation>
    </message>
    <message>
      <source>Vocal Focus</source>
      <translation>Vokalfokus</translation>
    </message>
    <message>
      <source>WAVE audio (*.wav)</source>
      <translation>WAVE-lyd (*.wav)</translation>
    </message>
    <message>
      <source>WAVE output exceeds its declared length</source>
      <translation>WAVE-output overskrider den angivne længde</translation>
      <extracomment>Owned WaveWriter frame-count validation: attempted sample writes exceed the frame count declared for the output. Not exceeding volume, clipping threshold or speaker capability. Preserve WAVE identifier. Contextual AI review; native verification unverified.</extracomment>
    </message>
    <message>
      <source>Waiting for a microphone.</source>
      <translation>Venter på en mikrofon.</translation>
    </message>
    <message>
      <source>Warm</source>
      <translation>Varm</translation>
    </message>
    <message>
      <source>Warm hall</source>
      <translation>Varm sal</translation>
    </message>
    <message>
      <source>Warmth</source>
      <translation>Varme</translation>
    </message>
    <message>
      <source>Windows audio COM unavailable</source>
      <translation>COM til Windows-lyd er ikke tilgængeligt</translation>
    </message>
    <message>
      <source>Windows could not verify the VB-Audio executable signature.</source>
      <translation>Windows kunne ikke bekræfte signaturen på VB-Audios eksekverbare fil.</translation>
      <extracomment>Windows Authenticode did not report a valid signature for the vendor executable. No claim is made about why verification failed; no instruction to bypass verification.</extracomment>
    </message>
    <message>
      <source>Windows has a VB-CABLE driver record but no usable cable endpoints. First check that CABLE Input and CABLE Output are enabled in Windows Sound settings. To reinstall: click Remove Driver in the official setup that opens next, restart Windows, then open %1 in the app again and click Install Driver. Restart once more before playing audio through SoundCurrent. Removing this shared cable affects other apps that use it.</source>
      <translation>Windows har en driverregistrering for VB-CABLE, men ingen brugbare endepunkter for kablet. Kontrollér først, at CABLE Input og CABLE Output er aktiveret i Windows lydindstillinger. For at geninstallere: klik på Remove Driver i det officielle installationsprogram, der åbner bagefter, genstart Windows, åbn derefter %1 i appen igen, og klik på Install Driver. Genstart endnu en gang, før du afspiller lyd gennem SoundCurrent. Fjernelse af dette delte kabel påvirker andre apps, der bruger det.</translation>
      <extracomment>Pre-repair modal, before official driver installer is opened. Existing driver record but endpoints unavailable; first check Windows endpoint enablement. Remove Driver and Install Driver are exact English external buttons. %1 is actual localized Audio driver setup button inside app, not English Start-menu shortcut. Preserve removal -&gt; Windows restart -&gt; app setup -&gt; reinstall -&gt; second restart, then audio playback; affects other users of shared cable. No claim removal already happened. AI contextual review only; native review unverified.</extracomment>
    </message>
    <message>
      <source>Windows has a VB-CABLE driver record, but its playback or recording endpoint is unavailable. If you have already restarted, open %1 to repair it. Enable CABLE Input and CABLE Output in Windows Sound settings if they are disabled.</source>
      <translation>Windows har en driverregistrering for VB-CABLE, men afspilnings- eller optagelsesslutpunktet er ikke tilgængeligt. Hvis du allerede har genstartet, skal du åbne %1 for at reparere det. Aktivér CABLE Input og CABLE Output i Windows’ lydindstillinger, hvis de er deaktiverede.</translation>
    </message>
    <message>
      <source>Windows will request administrator approval for the signed driver manager. Setup will tell you if a restart is required.</source>
      <translation>Windows vil bede om administratorgodkendelse til det signerede program til administration af drivere. Installationsprogrammet fortæller, om en genstart er nødvendig.</translation>
    </message>
    <message>
      <source>Write speaker buffer</source>
      <translation>Skrivning til højttalerbufferen</translation>
    </message>
    <message>
      <source>Write test playback</source>
      <translation>Skrivning af testlyd til afspilning</translation>
    </message>
    <message>
      <source>Wrong number of colon-separated fields</source>
      <extracomment>Standalone CLI colon-delimited numeric option has an exact required field count (EQ: 4, filters/routes: 3, gain: 2). Colon syntax remains unchanged; this is not a CSV delimiter preference.</extracomment>
      <translation>Forkert antal felter adskilt med kolon</translation>
    </message>
    <message>
      <source>Yes</source>
      <translation>Ja</translation>
    </message>
    <message>
      <source>Yes to All</source>
      <translation>Ja til alle</translation>
    </message>
    <message>
      <source>Zero turns each effect off. These listening effects apply to speaker playback, not microphone correction.</source>
      <translation>Nul slår hver effekt fra. Disse lytteeffekter påvirker højttalerafspilning, ikke mikrofonkorrektion.</translation>
    </message>
    <message>
      <source>append 0-30 seconds to render effect tails</source>
      <extracomment>Append 0–30 seconds of zero input after source audio so delay/reverb tails can decay into the export. Does not extend input media or change reverb decay itself. Preserve 0-30.</extracomment>
      <translation>tilføj 0-30 sekunder til rendering af effekternes udklang</translation>
    </message>
    <message>
      <source>bypass EQ, effects, gains and mute</source>
      <extracomment>Bypass engine EQ, delay/reverb/enhancements, channel/global gain and channel mute. Routing matrix still applies; final clipping and invalid-sample protection still apply. No device-routing bypass is implied.</extracomment>
      <translation>omgå EQ, effekter, forstærkning og lydløs</translation>
    </message>
    <message>
      <source>disable automatic EQ headroom</source>
      <extracomment>Disable automatic per-channel EQ gain compensation/headroom. Does not disable final clipping or invalid-sample protection.</extracomment>
      <translation>deaktiver automatisk EQ-niveaumargin</translation>
    </message>
    <message>
      <source>explicit matrix gain; using any route clears defaults</source>
      <extracomment>CLI --route OUT:IN:DB: when any explicit route exists the matrix starts at zero; only specified routes remain. Clearing defaults does not restore identity or automatic routing.</extracomment>
      <translation>eksplicit matrixforstærkning; enhver rute fjerner standardruterne</translation>
    </message>
    <message>
      <source>interface language; unsupported tags use English</source>
      <extracomment>CLI --language: selects interface catalog, normalizes tag case/separators and uses supported base language where available. Unresolved tags fall back to English. Does not change audio or numeric argument syntax.</extracomment>
      <translation>grænsefladesprog; ikke-understøttede sprogkoder bruger engelsk</translation>
    </message>
    <message>
      <source>optional channel high-pass</source>
      <extracomment>CLI high-pass output-channel filter attenuates low frequencies, passing high frequencies. Optional means absent unless specified. Not treble boost.</extracomment>
      <translation>valgfrit højpasfilter for kanalen</translation>
    </message>
    <message>
      <source>optional channel low-pass (e.g. LFE)</source>
      <extracomment>CLI low-pass output-channel filter attenuates high frequencies, passing low frequencies; LFE is only an example channel use, not an automatic speaker role. Preserve LFE identifier.</extracomment>
      <translation>valgfrit lavpasfilter for kanalen (f.eks. LFE)</translation>
    </message>
    <message>
      <source>output channel trim, -60 to +24 dB</source>
      <extracomment>Per-output-channel gain/trim, inclusive -60 to +24 dB. Preserve signs, bounds and dB; this is not the wider global post-gain range.</extracomment>
      <translation>niveaujustering for outputkanalen, -60 til +24 dB</translation>
    </message>
    <message>
      <source>overall post gain, -84 to +24 dB</source>
      <extracomment>Global post-gain control, inclusive -84 to +24 dB, applied to all channels. Preserve signs, bounds and dB; do not substitute the narrower channel trim range.</extracomment>
      <translation>samlet forstærkning efter behandling, -84 til +24 dB</translation>
    </message>
    <message>
      <source>peaking EQ for one output channel; repeat as needed</source>
      <extracomment>CLI --eq CH:HZ:DB:Q adds one peaking/bell filter to an output channel, repeatable within 64 filters per channel. Not peak detection or a shelf filter. CLI flag and argument tokens remain unchanged.</extracomment>
      <translation>peaking-EQ for én outputkanal; gentag efter behov</translation>
    </message>
    <message>
      <source>wet fraction 0-1 (enables delay)</source>
      <extracomment>Delay wet/processed-signal mix fraction inclusive 0–1; zero dry, one wet. Setting the option marks delay enabled even if zero mix is inaudible. Wet is audio mixing, not humidity.</extracomment>
      <translation>andel behandlet signal 0-1 (aktiverer delay)</translation>
    </message>
    <message>
      <source>wet fraction 0-1 (enables reverb)</source>
      <extracomment>Reverb wet/processed-signal mix fraction inclusive 0–1; zero dry, one wet. Setting the option marks reverb enabled. Wet is audio mixing, not humidity.</extracomment>
      <translation>andel behandlet signal 0-1 (aktiverer rumklang)</translation>
    </message>
    <message>
      <source>−∞ dBFS</source>
      <translation>−∞ dBFS</translation>
    </message>
  </context>
</TS>