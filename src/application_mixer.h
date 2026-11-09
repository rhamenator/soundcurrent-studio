// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <QWidget>
#include <memory>
class QTimer;
class QVBoxLayout;
class QLabel;
namespace soundcurrent::studio {
class ApplicationMixer final : public QWidget {
public:
    explicit ApplicationMixer(bool testMode=false,QWidget *parent=nullptr);
    ~ApplicationMixer() override;
    void refresh();
    int sliderCount() const;
protected:
    void showEvent(QShowEvent *) override;
    void hideEvent(QHideEvent *) override;
private:
    struct Private;
    std::unique_ptr<Private> d;
};
}
