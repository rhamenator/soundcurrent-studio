// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "studio_model.h"
#include "studio_name_text.h"
#include "localization.h"
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QToolTip>
#include <QElapsedTimer>
#include <algorithm>
#include <vector>
#include <array>
#include <optional>
#include <QHelpEvent>
#include <functional>
#include <cmath>
namespace soundcurrent::studio {
// Logical positions, never inferred from a channel count alone. LFE is kept
// outside the room field: bass management is not a spatial balance dimension.
inline std::optional<QPointF> speakerPosition(const QString &role) {
    if(role=="left" || role=="front-left")return QPointF(.15,.15);
    if(role=="right" || role=="front-right")return QPointF(.85,.15);
    if(role=="mono" || role=="center")return QPointF(.5,.15);
    if(role=="rear-left")return QPointF(.15,.8);
    if(role=="rear-right")return QPointF(.85,.8);
    if(role=="side-left")return QPointF(.1,.48);
    if(role=="side-right")return QPointF(.9,.48);
    if(role=="lfe")return QPointF(.5,.92);
    return {};
}
class ChannelMeterView : public QWidget {
public:
    explicit ChannelMeterView(QWidget *parent=nullptr):QWidget(parent) {
        setObjectName("channelMeterView");setAccessibleName(SC_TR("Studio channel output levels"));clock_.start();
    }
    void setSession(const Session &s) {
        bank_=0;names_.clear();positions_.clear();spatial_=s.hasRoomLayout();
        for(int c=0;c<s.names.size();++c){names_<<channelNameText(s,c);const auto pos=speakerPosition(s.defaultNameRole(c));
            positions_.push_back(pos.value_or(QPointF()));spatial_&=pos.has_value();}
        levels_.assign(names_.size(),0);held_.assign(names_.size(),0);expires_.assign(names_.size(),0);
        setMinimumHeight(spatial_?350:std::max(90,((displayedCount()+columns()-1)/columns())*64));update();
    }
    void setBank(int bank) {bank_=std::clamp(bank,0,std::max(0,(int(names_.size())-1)/64));setMinimumHeight(spatial_?350:std::max(90,((displayedCount()+columns()-1)/columns())*64));update();}
    void setLevels(std::span<const float> levels) {
        const auto now=clock_.elapsed();
        for(std::size_t c=0;c<levels_.size();++c){levels_[c]=c<levels.size() && std::isfinite(levels[c])?std::max(0.f,levels[c]):0;
            if(levels_[c]>=held_[c] || now>=expires_[c]){held_[c]=levels_[c];expires_[c]=now+1000;}}
        update();
    }
    float levelForChannel(std::size_t channel) const {return channel<levels_.size()?levels_[channel]:0;}
    bool spatial() const {return spatial_;}
    int channelCount() const {return names_.size();}
    QRectF channelRect(int c) const {
        if(spatial_){const auto p=positions_[std::size_t(c)];const double w=std::min(135.,width()*.25);
            return {p.x()*(width()-w),p.y()*(height()-58),w,54};}
        const int index=c-bank_*64;
        if(index<0 || index>=displayedCount())return {};
        const int cols=columns();const double w=double(width())/cols;
        return {(index%cols)*w,(index/cols)*64.,w-6,58};
    }
protected:
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);
        if(!spatial_)setMinimumHeight(std::max(90,((displayedCount()+columns()-1)/columns())*64));
    }
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);p.setRenderHint(QPainter::Antialiasing);
        if(spatial_){p.setPen(QColor("#43536a"));p.drawRoundedRect(rect().adjusted(width()*3/10,100,-width()*3/10,-100),8,8);}
        for(int c=0;c<names_.size();++c){const auto r=channelRect(c);if(r.isEmpty())continue;p.setPen(palette().text().color());
            p.drawText(r.adjusted(2,0,-2,-34),Qt::AlignCenter,fontMetrics().elidedText(names_[c],Qt::ElideRight,int(r.width()-4)));
            const QRectF bar(r.left()+4,r.top()+23,r.width()-8,12);p.fillRect(bar,QColor("#273345"));
            const auto fraction=[](float a){return std::clamp((20.*std::log10(std::max(1e-6,double(a)))+60)/60,0.,1.);};
            const double f=fraction(levels_[std::size_t(c)]);const auto color=levels_[std::size_t(c)]>=.89125f?QColor("#f16b76"):levels_[std::size_t(c)]>=.5f?QColor("#e6b450"):QColor("#50d1ba");
            p.fillRect(QRectF(bar.left(),bar.top(),bar.width()*f,bar.height()),color);
            const auto hold=held_[std::size_t(c)];p.setPen(hold>=1?QColor("#f16b76"):palette().text().color());
            const double x=bar.left()+bar.width()*fraction(hold);p.drawLine(QPointF(x,bar.top()),QPointF(x,bar.bottom()));
            const QString text=hold>1e-6f?QLocale().toString(20*std::log10(hold),'f',1)+SC_TR(" dBFS"):SC_TR("−∞ dBFS");
            p.drawText(r.adjusted(2,36,-2,0),Qt::AlignRight|Qt::AlignVCenter,text);
        }
    }
    bool event(QEvent *e) override {
        if(e->type()==QEvent::ToolTip){auto *h=static_cast<QHelpEvent *>(e);for(int c=0;c<names_.size();++c)if(channelRect(c).contains(h->pos())){QToolTip::showText(h->globalPos(),names_[c],this);return true;}}
        return QWidget::event(e);
    }
private:
    int bank_=0;
    int displayedCount() const {return std::max(0,std::min(64,int(names_.size())-bank_*64));}
    int columns() const {return std::max(1,std::min({8,width()/145,displayedCount()}));}
    QStringList names_;
    std::vector<QPointF> positions_;
    std::vector<float> levels_,held_;
    std::vector<qint64> expires_;
    bool spatial_=false;
    QElapsedTimer clock_;
};
class RoomBalancePad : public QWidget {
public:
    explicit RoomBalancePad(QWidget *parent=nullptr):QWidget(parent){setMinimumSize(200,180);setMaximumHeight(220);setFocusPolicy(Qt::StrongFocus);setAccessibleName(SC_TR("Balance"));setObjectName("roomBalancePad");}
    void setPosition(double x,double y){position_={x,y};update();}
    QPointF position() const {return position_;}
    std::function<void(double,double)> onEdited;
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);p.setRenderHint(QPainter::Antialiasing);const auto r=field();p.setPen(QColor("#8fa2bb"));p.drawRect(r);p.drawLine(QPointF(r.center().x(),r.top()),QPointF(r.center().x(),r.bottom()));p.drawLine(QPointF(r.left(),r.center().y()),QPointF(r.right(),r.center().y()));
        p.setPen(palette().text().color());p.drawText(QRectF(0,0,width(),25),Qt::AlignCenter,SC_TR("Front left")+QStringLiteral(" / ")+SC_TR("Front right"));
        p.drawText(QRectF(0,height()-25,width(),25),Qt::AlignCenter,SC_TR("Rear left")+QStringLiteral(" / ")+SC_TR("Rear right"));
        p.setBrush(QColor("#50d1ba"));p.drawEllipse(QPointF(r.center().x()+position_.x()*r.width()/2,r.center().y()+position_.y()*r.height()/2),7,7);
    }
    void mousePressEvent(QMouseEvent *e) override {if(e->button()==Qt::LeftButton){setFocus();moveTo(e->position());}}
    void mouseMoveEvent(QMouseEvent *e) override {if(e->buttons()&Qt::LeftButton)moveTo(e->position());}
    void mouseDoubleClickEvent(QMouseEvent *e) override {if(e->button()==Qt::LeftButton)edit(0,0);}
    void keyPressEvent(QKeyEvent *e) override {
        const double step=e->modifiers()&Qt::ShiftModifier?.01:.05;
        switch(e->key()) {case Qt::Key_Left:edit(position_.x()-step,position_.y());break;case Qt::Key_Right:edit(position_.x()+step,position_.y());break;case Qt::Key_Up:edit(position_.x(),position_.y()-step);break;case Qt::Key_Down:edit(position_.x(),position_.y()+step);break;case Qt::Key_Home:edit(0,0);break;default:QWidget::keyPressEvent(e);}
    }
private:
    QRectF field() const {return QRectF(25,30,width()-50,height()-60);}
    void moveTo(QPointF p){const auto r=field();edit((p.x()-r.center().x())*2/r.width(),(p.y()-r.center().y())*2/r.height());}
    void edit(double x,double y){if(!isEnabled())return;position_={std::clamp(x,-1.,1.),std::clamp(y,-1.,1.)};update();if(onEdited)onEdited(position_.x(),position_.y());}
    QPointF position_;
};
}
