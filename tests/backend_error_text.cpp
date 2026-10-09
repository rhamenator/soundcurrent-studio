// SPDX-License-Identifier: GPL-3.0-only
#include "audio_error_text.h"
#include "audio_setup_arguments.h"
#include <QCoreApplication>
#include <QProcess>
#include <QTranslator>
#include <QTextStream>
#include <stdexcept>
void require(bool pass) {if(!pass)throw std::runtime_error("Backend diagnostic translation invariant failed");}
class Fixture : public QTranslator {
 public:
 bool isEmpty() const override {return false;}
 QString translate(const char *context,const char *source,const char *,int) const override {
  if(QByteArray(context)!="SoundCurrent")return {};
  if(QByteArray(source).startsWith("Invalid ") || QByteArray(source).startsWith("Too many ") || QByteArray(source)=="Duplicate Studio route" || QByteArray(source)=="Enhancements outside supported ranges" || QByteArray(source).startsWith("Shared and channel EQ"))return "OWNED_DIAGNOSTIC";
  if(QByteArray(source)=="Chunk extends beyond RIFF bounds")return "RIFF_BOUNDS";
  if(QByteArray(source)=="Could not write WAVE header")return "WAVE_HEADER";
  if(QByteArray(source)=="Could not write WAVE audio")return "WAVE_AUDIO";
  if(QByteArray(source)=="Incomplete WAVE output")return "WAVE_INCOMPLETE";
  if(QByteArray(source)=="Truncated chunk header")return "CHUNK_HEADER";
  if(QByteArray(source)=="Speaker mask does not match channel count")return "SPEAKER_MASK";
  if(QByteArray(source)=="Missing RIFF padding byte")return "RIFF_PADDING";
  if(QByteArray(source)=="Excessive number of RIFF chunks")return "RIFF_CHUNKS";
  if(QByteArray(source)=="Output exceeds the RIFF/WAVE 4 GiB limit")return "RIFF_LIMIT";
  if(QByteArray(source)=="WAVE output exceeds its declared length")return "WAVE_LENGTH";
  if(QByteArray(source)=="Input is too short for RIFF/WAVE")return "WAVE_SHORT";
  if(QByteArray(source)=="Truncated extensible WAVE format")return "WAVE_EXTENSIBLE";
  if(QByteArray(source)=="Missing or incomplete WAVE audio")return "WAVE_DATA";
  if(QByteArray(source)=="Multiple WAVE data chunks are unsupported")return "WAVE_MULTI_DATA";
  if(QByteArray(source)=="Only little-endian RIFF/WAVE is supported")return "WAVE_ENDIAN";
  if(QByteArray(source)=="Only PCM16/24/32 or float32 WAVE is supported")return "WAVE_SAMPLES";
  if(QByteArray(source)=="Unsupported WAVE rate or channel count")return "WAVE_RATE";
  if(QByteArray(source)=="Could not flush WAVE output")return "WAVE_FLUSH";
  if(QByteArray(source)=="Could not close WAVE output")return "WAVE_CLOSE";
  if(QByteArray(source)=="Cannot seek to WAVE audio")return "WAVE_SEEK";
  if(QByteArray(source)=="Cannot create output WAVE file")return "WAVE_CREATE";
  if(QByteArray(source)=="Truncated WAVE file")return "WAVE_TRUNCATED";
  if(QByteArray(source)=="Cannot open input WAVE file")return "WAVE_OPEN";
  if(QByteArray(source)=="Read speaker level")return "LEVEL";
  if(QByteArray(source)=="Unsupported Studio profile schema")return "PROFILE_SCHEMA";
  if(QByteArray(source)=="Studio profile has an invalid numeric field")return "PROFILE_NUMBER";
  if(QByteArray(source)=="Studio profile has an invalid boolean field")return "PROFILE_BOOLEAN";
  if(QByteArray(source)=="%1 failed (0x%2)")return "%1 | %2";
  return {};
 }
};
int main(int argc,char **argv) try {
 QCoreApplication app(argc,argv);Fixture fixture;app.installTranslator(&fixture);
 using soundcurrent::i18n::audioErrorText;
 require(audioErrorText("Invalid WAVE frame alignment or byte rate")=="OWNED_DIAGNOSTIC");
 require(audioErrorText("Unsupported WAVE rate or channel count")=="WAVE_RATE");
 require(audioErrorText("Only little-endian RIFF/WAVE is supported")=="WAVE_ENDIAN");
 require(audioErrorText("Only PCM16/24/32 or float32 WAVE is supported")=="WAVE_SAMPLES");
 require(audioErrorText("Missing or incomplete WAVE audio")=="WAVE_DATA");
 require(audioErrorText("Multiple WAVE data chunks are unsupported")=="WAVE_MULTI_DATA");
 require(audioErrorText("Input is too short for RIFF/WAVE")=="WAVE_SHORT");
 require(audioErrorText("Truncated extensible WAVE format")=="WAVE_EXTENSIBLE");
 require(audioErrorText("Output exceeds the RIFF/WAVE 4 GiB limit")=="RIFF_LIMIT");
 require(audioErrorText("WAVE output exceeds its declared length")=="WAVE_LENGTH");
 require(audioErrorText("Missing RIFF padding byte")=="RIFF_PADDING");
 require(audioErrorText("Excessive number of RIFF chunks")=="RIFF_CHUNKS");
 require(audioErrorText("Speaker mask does not match channel count")=="SPEAKER_MASK");
 require(audioErrorText("Invalid output speaker mask")=="OWNED_DIAGNOSTIC");
 require(audioErrorText("Invalid valid-bit count")=="OWNED_DIAGNOSTIC");
 require(audioErrorText("Invalid float WAVE format")=="OWNED_DIAGNOSTIC");
 require(audioErrorText("Incomplete WAVE output")=="WAVE_INCOMPLETE");
 require(audioErrorText("Truncated chunk header")=="CHUNK_HEADER");
 require(audioErrorText("Could not write WAVE header")=="WAVE_HEADER");
 require(audioErrorText("Could not write WAVE audio")=="WAVE_AUDIO");
 require(audioErrorText("Could not flush WAVE output")=="WAVE_FLUSH");
 require(audioErrorText("Could not close WAVE output")=="WAVE_CLOSE");
 require(audioErrorText("Invalid WAVE read buffer")=="OWNED_DIAGNOSTIC");
 require(audioErrorText("Invalid output WAVE format")=="OWNED_DIAGNOSTIC");
 require(audioErrorText("Invalid RIFF size")=="OWNED_DIAGNOSTIC");
 require(audioErrorText("Chunk extends beyond RIFF bounds")=="RIFF_BOUNDS");
 require(audioErrorText("Cannot open input WAVE file")=="WAVE_OPEN");
 require(audioErrorText("Truncated WAVE file")=="WAVE_TRUNCATED");
 require(audioErrorText("Cannot create output WAVE file")=="WAVE_CREATE");
 require(audioErrorText("Cannot seek to WAVE audio")=="WAVE_SEEK");
 const auto unicodeOutput=QString::fromUtf8("Réparation / 日本語 / العربية / %1 / 音声");
 const auto outputBytes=unicodeOutput.toUtf8()+"\r\n";
 QByteArray accumulated;
 for(const char byte:outputBytes)accumulated.append(byte);
 require(soundcurrent::i18n::audioSetupOutput(accumulated)==unicodeOutput);
 const auto fixtureIndex=app.arguments().indexOf("--powershell-output-fixture");
 if(fixtureIndex>=0) {
  require(fixtureIndex+2<app.arguments().size());
  QProcess child;child.setProgram(app.arguments()[fixtureIndex+1]);
  child.setArguments({"-NoProfile","-NonInteractive","-ExecutionPolicy","RemoteSigned","-File",app.arguments()[fixtureIndex+2]});
  child.start();
  const bool finished=child.waitForFinished(15000);
  const auto standardOutput=child.readAllStandardOutput();
  const auto standardError=child.readAllStandardError();
  const bool successful=finished && child.exitStatus()==QProcess::NormalExit && child.exitCode()==0;
  const bool correctOutput=soundcurrent::i18n::audioSetupOutput(standardOutput)==unicodeOutput;
  if(!successful || !correctOutput) {
   // Inert fixture only. Bound diagnostics; bytes expose encoding differences
   // without relying on the runner's console code page to render them.
   QTextStream diagnostic(stderr);
   diagnostic << "PowerShell output fixture failed: finished=" << finished
              << " processError=" << static_cast<int>(child.error())
              << " exitStatus=" << static_cast<int>(child.exitStatus())
              << " exitCode=" << child.exitCode() << '\n'
              << "processErrorText=" << child.errorString() << '\n'
              << "stdoutHex=" << standardOutput.left(4096).toHex() << '\n'
              << "stderrHex=" << standardError.left(4096).toHex() << '\n'
              << "expectedHex=" << unicodeOutput.toUtf8().toHex() << '\n';
   diagnostic.flush();
   if(!finished) {child.kill();child.waitForFinished(5000);}
   return 1;
  }
 }
 const auto setupScript=QString::fromUtf8("C:/Program Files/SoundCurrent/音声 setup.ps1");
 for(const auto &language:QStringList{"fr","nn","zh-Hant","en","qps-rtl"}) {
  for(bool install:{false,true}) {
   const auto args=soundcurrent::i18n::audioSetupArguments(setupScript,install,1234,language);
   require(args==QStringList{"-NoProfile","-ExecutionPolicy","RemoteSigned","-File",setupScript,
                            install?"-Install":"-Settings","-Quiet","-RequestingProcessId","1234","-Language",language});
  }
 }
 require(audioErrorText("Unsupported Studio profile schema")=="PROFILE_SCHEMA");
 require(audioErrorText("Studio profile has an invalid numeric field")=="PROFILE_NUMBER");
 require(audioErrorText("Studio profile has an invalid boolean field")=="PROFILE_BOOLEAN");
 require(audioErrorText("Invalid audio route: loopback requires a separate render source") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Invalid Studio channel count") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Invalid Studio profile channel count") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Invalid Studio channel name or filters") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Too many Studio channel filters") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Invalid Studio route") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Duplicate Studio route") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Invalid route indexes or weight") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Invalid route number") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Too many Studio routes") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Invalid filter type") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Shared and channel EQ exceed 64 filters; remove some channel filters") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Invalid enhancement parameter count") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Invalid enhancement parameter type") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Enhancements outside supported ranges") == "OWNED_DIAGNOSTIC");
 require(audioErrorText("Invalid external route %1")=="Invalid external route %1");
 require(audioErrorText("External profile %1 — unknown")=="External profile %1 — unknown");
 require(audioErrorText("Read speaker level failed (0xAbCd1234)")=="LEVEL | AbCd1234");
 require(audioErrorText("External driver failed (0x80070005)")=="External driver failed (0x80070005)");
 require(audioErrorText("Read speaker level failed (0xZZ)")=="Read speaker level failed (0xZZ)");
} catch(const std::exception &error) {
 QTextStream diagnostic(stderr);
 diagnostic << error.what() << '\n';
 diagnostic.flush();
 return 1;
}
