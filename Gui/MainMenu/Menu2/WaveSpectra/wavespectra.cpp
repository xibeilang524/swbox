#include "wavespectra.h"

WaveSpectra::WaveSpectra(QWidget *parent) : QFrame(parent)
{
    key_val = NULL;

    this->resize(455, 185);
    this->move(2, 31);
    this->setStyleSheet("WaveSpectra {border-image: url(:/widgetphoto/mainmenu/bk2.png);}");

    /* set para */
    set_lab = new QLabel(this);
    set_lab->resize(161, 33);
    set_lab->move(1, 1);
    set_lab->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/para0.png);}");

    /* set para text */
    set_txt_lab = new QLabel(set_lab);
    set_txt_lab->resize(88, 19);
    set_txt_lab->move(10, 0);
    set_txt_lab->setAttribute(Qt::WA_TranslucentBackground, true);
    set_txt_lab->setStyleSheet("QLabel {color:gray;}");
    set_txt_lab->setAlignment(Qt::AlignCenter);
    set_txt_lab->setText(tr("参  数  设  置"));

    /* set para menu list */
    set_child0_lab = new QLabel(this);
    set_child0_lab->resize(147, 25);
    set_child0_lab->move(set_lab->x(), set_lab->y() + 21);
    set_child0_lab->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/para_child.png);color:gray}");
    set_child0_lab->setAlignment(Qt::AlignVCenter);
    set_child0_lab->setText(tr("  触发幅值"));

    child0_val_lab = new QLabel(set_child0_lab);
    child0_val_lab->resize(47, 25);
    child0_val_lab->move(100, 0);
    child0_val_lab->setAttribute(Qt::WA_TranslucentBackground, true);
    child0_val_lab->setStyleSheet("QLabel {color:gray;}");
    child0_val_lab->setAlignment(Qt::AlignVCenter);
    child0_val_lab->setText(tr("[  ]"));

    set_child1_lab = new QLabel(this);
    set_child1_lab->resize(147, 25);
    set_child1_lab->move(set_lab->x(), set_child0_lab->y() + set_child0_lab->height());
    set_child1_lab->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/para_child.png);color:gray}");
    set_child1_lab->setAlignment(Qt::AlignVCenter);
    set_child1_lab->setText(tr("  增益调节"));

    child1_val_lab = new QLabel(set_child1_lab);
    child1_val_lab->resize(47, 25);
    child1_val_lab->move(100, 0);
    child1_val_lab->setAttribute(Qt::WA_TranslucentBackground, true);
    child1_val_lab->setStyleSheet("QLabel {color:gray;}");
    child1_val_lab->setAlignment(Qt::AlignVCenter);
    child1_val_lab->setText(tr("[44]dB"));

    set_child2_lab = new QLabel(this);
    set_child2_lab->resize(147, 25);
    set_child2_lab->move(set_lab->x(), set_child1_lab->y() + set_child1_lab->height());
    set_child2_lab->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/para_child.png);color:gray}");
    set_child2_lab->setAlignment(Qt::AlignVCenter);
    set_child2_lab->setText(tr("  相位偏移"));
    child2_val_lab = new QLabel(set_child2_lab);
    child2_val_lab->resize(47, 25);
    child2_val_lab->move(100, 0);
    child2_val_lab->setAttribute(Qt::WA_TranslucentBackground, true);
    child2_val_lab->setStyleSheet("QLabel {color:gray;}");
    child2_val_lab->setAlignment(Qt::AlignVCenter);
    child2_val_lab->setText(tr("[15]"));

    set_child3_lab = new QLabel(this);
    set_child3_lab->resize(147, 25);
    set_child3_lab->move(set_lab->x(), set_child2_lab->y() + set_child2_lab->height());
    set_child3_lab->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/para_child.png);color:gray}");
    set_child3_lab->setAlignment(Qt::AlignVCenter);
    set_child3_lab->setText(tr("  同步方式"));

    child3_val_lab = new QLabel(set_child3_lab);
    child3_val_lab->resize(47, 25);
    child3_val_lab->move(100, 0);
    child3_val_lab->setAttribute(Qt::WA_TranslucentBackground, true);
    child3_val_lab->setStyleSheet("QLabel {color:gray;}");
    child3_val_lab->setAlignment(Qt::AlignVCenter);
    child3_val_lab->setText(tr("[15]S"));

    set_child4_lab = new QLabel(this);
    set_child4_lab->resize(147, 25);
    set_child4_lab->move(set_lab->x(), set_child3_lab->y() + set_child3_lab->height());
    set_child4_lab->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/para_child.png);color:gray}");
    set_child4_lab->setAlignment(Qt::AlignVCenter);
    set_child4_lab->setText(tr("  保存波形"));

    child4_val_lab = new QLabel(set_child4_lab);
    child4_val_lab->resize(47, 25);
    child4_val_lab->move(100, 0);
    child4_val_lab->setAttribute(Qt::WA_TranslucentBackground, true);
    child4_val_lab->setStyleSheet("QLabel {color:gray;}");
    child4_val_lab->setAlignment(Qt::AlignVCenter);
    child4_val_lab->setText(tr("[15]S"));

    set_child5_lab = new QLabel(this);
    set_child5_lab->resize(147, 25);
    set_child5_lab->move(set_lab->x(), set_child4_lab->y() + set_child4_lab->height());
    set_child5_lab->setStyleSheet("QLabel {border-image: url(:/widgetphoto/mainmenu/para_child.png);color:gray}");
    set_child5_lab->setAlignment(Qt::AlignVCenter);
    set_child5_lab->setText(tr("  检测模式"));

    child5_val_lab = new QLabel(set_child5_lab);
    child5_val_lab->resize(47, 25);
    child5_val_lab->move(100, 0);
    child5_val_lab->setAttribute(Qt::WA_TranslucentBackground, true);
    child5_val_lab->setStyleSheet("QLabel {color:gray;}");
    child5_val_lab->setAlignment(Qt::AlignVCenter);
    child5_val_lab->setText(tr("[单次]"));
#if 1
    set_child0_lab->hide();
    set_child1_lab->hide();
    set_child2_lab->hide();
    set_child3_lab->hide();
    set_child4_lab->hide();
    set_child5_lab->hide();
#endif
}

void WaveSpectra::working(CURRENT_KEY_VALUE *val)
{
    if (val == NULL) {
        return;
    }
    key_val = val;

    this->show();
}

void WaveSpectra::trans_key(quint8 key_code)
{
    if (key_val == NULL) {
        return;
    }

    if (key_val->grade.val1 != 3) {
        return;
    }

    switch (key_code) {
    case KEY_OK:
        break;
    case KEY_CANCEL:
        if (!key_val->grade.val3) {
            key_val->grade.val2 = 0;
            fresh_parent();
        }
        break;
    case KEY_UP:
        break;
    case KEY_DOWN:
        break;
    case KEY_LEFT:

        break;

    case KEY_RIGHT:

        break;
    default:
        break;
    }
}