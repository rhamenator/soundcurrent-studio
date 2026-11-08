// Actual Windows default-route change while the live UI is processing.
if (app.arguments().contains("--windows-live-manual-route-test")) {
    try {
        soundcurrent::ProcessingGuard guard;
        if (!guard.acquire(soundcurrent::processingGuardDirectory())) throw std::runtime_error("Audio session busy");
        const auto owned=standardCable(false).toStdWString();
        const auto outputs=devices();
        if (outputs.isEmpty()) throw std::runtime_error("No physical fixture");
        auto physical=outputs.front().name.toStdWString();
        const auto current=soundcurrent::windowsDefaultOutputId();
        for(const auto &device:outputs) if(device.name.toStdWString()==current) physical=current;
        const bool otherOutput=app.arguments().contains("--other-physical-output");
        QString selected;
        if(otherOutput) {
            for(const auto &device:outputs) if(device.name.toStdWString()!=physical) {selected=device.name;break;}
            if(selected.isEmpty()) throw std::runtime_error("Need two physical outputs");
        }
        int result=0;
        {
            MainWindow window(true);
            for(auto *button:window.findChildren<QPushButton *>())
                if(button->text()=="Refresh devices") {button->click();break;}
            auto *power=window.findChild<QCheckBox *>("powerToggle");
            if (!power) throw std::runtime_error("Power control missing");
            if(otherOutput) {
                QComboBox *output=nullptr;
                for(auto *box:window.findChildren<QComboBox *>())
                    if(box->accessibleName()=="Output device") {output=box;break;}
                if(!output || output->findData(selected)<0) throw std::runtime_error("Output control missing");
                output->setCurrentIndex(output->findData(selected));
            }
            QTimer::singleShot(500,&window,[&] {
                try {
                    if (!power->isChecked()) throw std::runtime_error("Live audio did not start");
                    for(int role=0;role<3;++role)
                        if(soundcurrent::windowsDefaultEndpointId(false,role)!=owned) throw std::runtime_error("Route not acquired");
                    // Change actual endpoint defaults, as an external Windows
                    // device choice would; avoid changing volume in this call.
                    if(!soundcurrent::windowsRestoreOwnedRoute(false,owned,physical,{physical,physical,physical},false))
                        throw std::runtime_error("Cannot select direct physical output");
                } catch(const std::exception &e){qCritical("Manual selection: %s",e.what());result=1;app.quit();}
            });
            QTimer::singleShot(4000,&window,[&] {
                try {
                    if(power->isChecked()) throw std::runtime_error("UI still claims EQ on after Windows selects direct physical output");
                    for(int role=0;role<3;++role)
                        if(soundcurrent::windowsDefaultEndpointId(false,role)!=physical) throw std::runtime_error("Manual output choice overwritten");
                } catch(const std::exception &e){qCritical("Manual selection: %s",e.what());result=1;}
                app.quit();
            });
            app.exec();
        }
        if(result==0) QTextStream(stdout)<<"PASS: direct Windows output choice disables processing and preserves all roles"<<Qt::endl;
        return result;
    } catch(const std::exception &e){qCritical("Manual route test: %s",e.what());return 1;}
}
// Opt-in test coordinated by a host-side unplug/replug fixture.
if (app.arguments().contains("--windows-live-hotplug-test")) {
    try {
        if (app.arguments().size()!=3) throw std::runtime_error("Supply a test status file");
        soundcurrent::ProcessingGuard guard;
        if (!guard.acquire(soundcurrent::processingGuardDirectory())) throw std::runtime_error("Audio session busy");
        const auto owned=standardCable(false).toStdWString();
        auto report=[&](const QString &value) {
            QSaveFile file(app.arguments()[2]);
            if (!file.open(QIODevice::WriteOnly) || file.write(value.toUtf8())<0 || !file.commit())
                throw std::runtime_error("Cannot report hotplug test state");
        };
        int result=0;
        {
            // Exercise the production live monitor, not offline preview mode.
            MainWindow window(true);
            for (auto *button:window.findChildren<QPushButton *>())
                if (button->text()=="Refresh devices") {button->click();break;}
            QComboBox *output=nullptr;
            for (auto *box:window.findChildren<QComboBox *>())
                if (box->accessibleName()=="Output device") {output=box;break;}
            auto *power=window.findChild<QCheckBox *>("powerToggle");
            auto *status=window.findChild<QLabel *>("status");
            if (!output || !power || !status) throw std::runtime_error("Live controls unavailable");
            const QString usb="Speakers (Audio Device)", onboard="Speakers (High Definition Audio Device)";
            const int usbIndex=output->findText(usb);
            if (usbIndex<0 || output->findText(onboard)<0) throw std::runtime_error("Need clone USB and onboard fixtures");
            output->setCurrentIndex(usbIndex);
            power->setChecked(true);
            if (!power->isChecked() || !status->text().contains(usb)) throw std::runtime_error("USB processing did not start");
            report("ready");
            int stage=0;QElapsedTimer elapsed;elapsed.start();QTimer poll;poll.setInterval(100);
            QObject::connect(&poll,&QTimer::timeout,&window,[&] {
                try {
                    if (elapsed.elapsed()>60000) {
                        QStringList names;
                        for (int i=0;i<output->count();++i) names.append(output->itemText(i));
                        throw std::runtime_error((QString("Hotplug test timed out; stage=%1; status=%2; outputs=%3")
                            .arg(stage).arg(status->text(),names.join(" | "))).toStdString());
                    }
                    if (!power->isChecked()) throw std::runtime_error("Processing disabled during hotplug");
                    if (stage==0 && output->findText(usb)<0 && status->text().contains(onboard)) {
                        if (!output->currentData().toString().isEmpty()) throw std::runtime_error("Unplug did not switch to automatic");
                        for (int role=0;role<3;++role)
                            if (soundcurrent::windowsDefaultEndpointId(false,role)!=owned)
                                throw std::runtime_error("Owned output role missing after completed switch");
                        report("fallback");stage=1;
                    } else if (stage==1 && output->findText(usb)>=0 && status->text().contains(usb)) {
                        for (int role=0;role<3;++role)
                            if (soundcurrent::windowsDefaultEndpointId(false,role)!=owned)
                                throw std::runtime_error("Owned output role missing after completed switch");
                        report("reconnected");poll.stop();power->setChecked(false);app.quit();
                    }
                } catch (const std::exception &error) {
                    result=1;poll.stop();report(QString("FAIL: ")+error.what());app.quit();
                }
            });
            poll.start();app.exec();
        }
        for (int role=0;role<3;++role)
            if (soundcurrent::windowsDefaultEndpointId(false,role)==owned) throw std::runtime_error("Hotplug test left owned default");
        return result;
    } catch (const std::exception &error) { qCritical("Hotplug test: %s",error.what());return 1; }
}
// SPDX-License-Identifier: GPL-3.0-only
// Included inside main(): opt-in isolated Windows clone only.
if (app.arguments().contains("--windows-live-ui-test")) {
    try {
        soundcurrent::ProcessingGuard guard;
        if (!guard.acquire(soundcurrent::processingGuardDirectory()))
            throw std::runtime_error("Another SoundCurrent session is active");
        const auto physical = devices();
        if (physical.isEmpty()) throw std::runtime_error("No physical output fixture");
        const auto owned = standardCable(false).toStdWString();
        QTextStream(stdout)<<"Managed output endpoint: "<<QString::fromStdWString(owned)<<Qt::endl;
        std::array<std::wstring,3> before;
        for (int role=0; role<3; ++role) {
            before[role]=soundcurrent::windowsDefaultEndpointId(false,role);
            if (before[role]==owned) throw std::runtime_error("Fixture already owns default output; preserve it");
        }
        auto verify = [&](bool enabled) {
            for (int role=0; role<3; ++role)
                if (soundcurrent::windowsDefaultEndpointId(false,role)!=(enabled?owned:before[role]))
                    throw std::runtime_error("UI output route does not match enabled state");
        };
        {
            MainWindow window(false);
            for (auto *button:window.findChildren<QPushButton *>())
                if (button->text()=="Refresh devices") {button->click();break;}
            auto *power=window.findChild<QCheckBox *>("powerToggle");
            if (!power || !power->isEnabled()) throw std::runtime_error("Output control unavailable");
            power->setChecked(true);
            if (!power->isChecked()) throw std::runtime_error("UI failed to start audio");
            verify(true);
            // End-to-end regression: real Studio widgets -> callback -> WASAPI
            // processing -> physical-output loopback, with isolated settings.
            soundcurrent::studio::StudioPanel *panel=nullptr;
            for(auto *widget:window.findChildren<QWidget *>())
                if(auto *candidate=dynamic_cast<soundcurrent::studio::StudioPanel *>(widget))panel=candidate;
            auto *input=window.findChild<QSlider *>("inputGain");
            auto *mixer=window.findChild<QTableWidget *>("channelMixer");
            auto *apply=window.findChild<QPushButton *>("applyStudioSettings");
            if(!panel||!input||!mixer||!apply)throw std::runtime_error("Studio controls unavailable");
            auto *offline=panel->findChild<QPushButton *>("studioOfflineEditing");
            if(!offline||panel->session().offline)throw std::runtime_error("Normal Studio session is offline");
            auto measure=[&] {
                std::vector<std::int16_t> tone(48000*3*2);
                for(std::size_t f=0;f<tone.size()/2;++f)
                    tone[2*f]=tone[2*f+1]=std::int16_t(655*std::sin(2*std::numbers::pi*997*f/48000));
                soundcurrent::WindowsRecorder recorder;
                const auto destination=before[1];
                recorder.start(destination,48000,2,true);
                auto play=std::async(std::launch::async,[&]{soundcurrent::windowsPlayPcm(owned,tone,48000,2);});
                QElapsedTimer elapsed;elapsed.start();double left=0,right=0;std::size_t frames=0;
                while(play.wait_for(std::chrono::milliseconds(0))!=std::future_status::ready) {
                    QCoreApplication::processEvents();QThread::msleep(10);
                    const auto pcm=recorder.take();
                    if(elapsed.elapsed()<1400)continue;
                    for(std::size_t i=0;i+1<pcm.size();i+=2){left+=double(pcm[i])*pcm[i];right+=double(pcm[i+1])*pcm[i+1];++frames;}
                }
                play.get();recorder.stop();
                if(!frames)throw std::runtime_error("No Studio UI loopback frames");
                return std::array<double,2>{std::sqrt(left/frames)/32768,std::sqrt(right/frames)/32768};
            };
            auto db=[](double value,double reference){return 20*std::log10(value/reference);};
            const auto flat=measure();
            if(flat[0]<.0001||flat[1]<.0001)throw std::runtime_error("Studio UI route is silent");
            offline->setChecked(true);input->setValue(-600);
            const auto staged=measure();
            if(std::abs(db(staged[0],flat[0]))>.6)throw std::runtime_error("Offline UI edits changed playback");
            apply->click();const auto applied=measure();
            if(panel->session().offline||std::abs(db(applied[0],flat[0])+6)>.6)
                throw std::runtime_error("Apply settings did not deliver live input gain");
            input->setValue(0);
            auto *channelMute=qobject_cast<QCheckBox *>(mixer->cellWidget(0,2));
            channelMute->setChecked(true);const auto muted=measure();
            if(muted[0]>flat[0]*.01||std::abs(db(muted[1],flat[1]))>.6)
                throw std::runtime_error("Live mixer channel mute failed");
            channelMute->setChecked(false);
            auto *trim=qobject_cast<QDoubleSpinBox *>(mixer->cellWidget(0,1));
            trim->setValue(-9);const auto trimmed=measure();
            if(std::abs(db(trimmed[0],flat[0])+9)>.6||std::abs(db(trimmed[1],flat[1]))>.6)
                throw std::runtime_error("Live mixer channel trim failed");
            trim->setValue(0);
            auto *solo=qobject_cast<QCheckBox *>(mixer->cellWidget(0,3));
            solo->setChecked(true);const auto soloed=measure();
            if(soloed[1]>flat[1]*.01||std::abs(db(soloed[0],flat[0]))>.6)
                throw std::runtime_error("Live mixer solo failed");
            solo->setChecked(false);
            QComboBox *preset=nullptr;
            for(auto *box:panel->findChildren<QComboBox *>())
                if(box->accessibleName()=="Studio effect preset")preset=box;
            if(!preset)throw std::runtime_error("Effect preset control missing");
            QMetaObject::invokeMethod(preset,"activated",Qt::DirectConnection,Q_ARG(int,1));
            const auto effect=measure();
            if(!panel->session().engine.delay.enabled||std::abs(db(effect[0],flat[0]))<.1)
                throw std::runtime_error("Live effect preset did not change audio");
            QMetaObject::invokeMethod(preset,"activated",Qt::DirectConnection,Q_ARG(int,0));
            QTextStream(stdout)<<"PASS: Studio UI offline staging preserved playback; Apply input gain "
                <<db(applied[0],flat[0])<<" dB; live channel mute RMS "<<muted[0]<<" / "<<muted[1]
                <<"; trim "<<db(trimmed[0],flat[0])<<" dB; solo RMS "<<soloed[0]<<" / "<<soloed[1]
                <<"; effect change "<<db(effect[0],flat[0])<<" dB"<<Qt::endl;
            power->setChecked(false);
            verify(false);
            power->setChecked(true);
            verify(true);
            auto *quit=window.findChild<QPushButton *>("persistentQuit");
            if (!quit) throw std::runtime_error("Quit control unavailable");
            QTimer::singleShot(0,quit,&QPushButton::click);
            if (app.exec()!=0) throw std::runtime_error("Quit event loop failed");
        }
        verify(false);
        QTextStream(stdout)<<"PASS: live UI enable, disable, re-enable and Quit restored all output roles"<<Qt::endl;
        return 0;
    } catch (const std::exception &error) {
        qCritical("Live UI route test failed: %s",error.what());
        return 1;
    }
}
