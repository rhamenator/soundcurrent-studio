// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "studio_model.h"
#include "enhancement_controls.h"
#include <QWidget>
#include <QTimer>
#include <atomic>
#include <functional>
#include <future>
class QComboBox;
class QCheckBox;
class QSpinBox;
class QDoubleSpinBox;
class QSlider;
class QTableWidget;
class QLineEdit;
class QLabel;
class QPushButton;
class QProgressBar;

namespace soundcurrent::studio {
class StudioPanel : public QWidget {
public:
    explicit StudioPanel(bool persist, QWidget *parent = nullptr);
    ~StudioPanel() override;
    Session session() const { return session_; }
    void setShared(std::span<const EqBand>, double gain, int balance);
    void setLocked(bool);
    void undo();
    void setLiveLevels(std::span<const float>);
    void liveStatus(const QString &, bool rejected = false);
    void selfTest();
    void selfTestFormatting();
    void selfTestRenderErrors();
    void selfTestChannelNames();
    std::function<void()> onChanged;
private:
    void rebuild();
    void loadChannel();
    void commit(const Session &before);
    void change(const std::function<void(Session &)> &);
    void effectPreset(int);
    void saveProfile();
    void openProfile();
    void render();
    void renderFiles(const QString &input, const QString &output);
    void tick();
    void startPreview();
    soundcurrent::EnhancementControls *enhancements_;
    Session session_;
    std::vector<Session> history_;
    std::vector<EqBand> shared_;
    double sharedGain_ = 0;
    int balance_ = 0;
    bool persist_, rebuilding_ = false, locked_ = false;
    QSpinBox *count_, *routeInput_;
    QComboBox *layout_, *channel_, *preset_, *filterType_;
    QCheckBox *offline_, *mute_, *solo_, *delay_, *reverb_, *bypass_, *headroom_, *preview_;
    QLineEdit *name_;
    QSlider *trim_, *wetDelay_, *wetReverb_;
    QDoubleSpinBox *delayMs_, *feedback_, *decay_, *damping_, *filterHz_, *filterDb_, *filterQ_, *routeGain_, *tail_;
    QTableWidget *filters_, *routes_, *meters_;
    QLabel *status_;
    QPushButton *undo_, *render_, *cancel_;
    QProgressBar *progress_;
    QWidget *editing_;
    QTimer timer_;
    std::unique_ptr<AudioEngine> previewEngine_;
    std::unique_ptr<ChannelRouter> previewRouter_;
    std::vector<float> raw_, processed_;
    std::uint64_t previewFrame_ = 0;
    std::future<QString> renderJob_;
    std::atomic<bool> cancelJob_{false};
    std::atomic<int> jobProgress_{0};
};
}
