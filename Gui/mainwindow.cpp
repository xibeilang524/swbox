#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QQuickItem>

#ifdef PRINTSCREEN
#include <QPixmap>
#include <QApplication>
#include <QScreen>
#endif

#define LOW_POWER_TIMES     3


MainWindow::MainWindow(QSplashScreen *sp, QWidget *parent ) :
    QFrame(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setGeometry (0, 0, RESOLUTION_X, RESOLUTION_Y);

    QPalette Pal(this->palette());
    Pal.setColor(QPalette::Background, QColor(36,36,36));
    this->setPalette(Pal);

//    sqlcfg->sql_save(sqlcfg->default_config());       //用于程序崩溃时重置数据

    key_val.val = 0;
    data = new G_PARA;
    memset(data, 0, sizeof(G_PARA));

    sp->showMessage(tr("正在初始化实时数据库..."),Qt::AlignBottom|Qt::AlignLeft);
    init_rdb();

    sp->showMessage(tr("正在初始化按键..."),Qt::AlignBottom|Qt::AlignLeft);
    keydetect = new KeyDetect(this);

    sp->showMessage(tr("正在设置FPGA..."),Qt::AlignBottom|Qt::AlignLeft);
    fifodata = new FifoData(data);

    sp->showMessage(tr("正在初始化通信..."),Qt::AlignBottom|Qt::AlignLeft);
    modbus = new Modbus(this,data);

    //注册两个自定义类型
    qRegisterMetaType<VectorList>("VectorList");
    qRegisterMetaType<MODE>("MODE");

    connect(keydetect, &KeyDetect::sendkey, this, &MainWindow::trans_key);
    connect(modbus,SIGNAL(closeTimeChanged(int)),this,SLOT(set_reboot_time()) );

#ifdef PRINTSCREEN
    connect(timer_time,SIGNAL(timeout()),this,SLOT(printSc()));  //截屏
#endif

    sp->showMessage(tr("正在初始化主菜单..."),Qt::AlignBottom|Qt::AlignLeft);
    menu_init();
    qml_init();
    statusbar_init();
    function_init(sp);
    options_init();

    for (int i = 0; i < mode_list.count(); ++i) {
        if(mode_list.at(i) != Disable){
            key_val.grade.val0 = i;
            ui->tabWidget->setCurrentIndex(i);
            fresh_menu_icon();
            break;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::menu_init()
{
    ui->tabWidget->setStyleSheet("QTabBar::tab {border: 0px solid white; min-width: 0ex;padding: 5px 1px 0px 1px; }"
                                 "QTabBar::tab:selected{ background:rgb(46, 52, 54);  }"
                                 "QTabBar::tab:!selected{ background:transparent;   }"
                                 "QTabWidget::pane{border-width:0px;}"
                                 );
    ui->tabWidget->setFocusPolicy(Qt::NoFocus);
    menu_icon0 = new QLabel(this);
    menu_icon0->resize(41, 24);
    menu_icon1 = new QLabel(this);
    menu_icon1->resize(41, 24);
    menu_icon2 = new QLabel(this);
    menu_icon2->resize(41, 24);
    menu_icon3 = new QLabel(this);
    menu_icon3->resize(41, 24);
    menu_icon4 = new QLabel(this);
    menu_icon4->resize(41, 24);
    menu_icon5 = new QLabel(this);
    menu_icon5->resize(41, 24);

    ui->tabWidget->tabBar()->setTabButton(0,QTabBar::LeftSide,menu_icon0);
    ui->tabWidget->tabBar()->setTabButton(1,QTabBar::LeftSide,menu_icon1);
    ui->tabWidget->tabBar()->setTabButton(2,QTabBar::LeftSide,menu_icon2);
    ui->tabWidget->tabBar()->setTabButton(3,QTabBar::LeftSide,menu_icon3);
    ui->tabWidget->tabBar()->setTabButton(4,QTabBar::LeftSide,menu_icon4);
    ui->tabWidget->tabBar()->setTabButton(5,QTabBar::LeftSide,menu_icon5);

    //设置每个通道界面的背景，由于使用样式表，会造成子部件背景色混乱，改用调色板设置
    QPalette Pal(this->palette());
    Pal.setColor(QPalette::Background, QColor(46, 52, 54));
    for (int i = 0; i < ui->tabWidget->count(); ++i) {
        ui->tabWidget->widget(i)->setPalette(Pal);
    }
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(fresh_menu_icon()) );
}

void MainWindow::statusbar_init()
{
    battery = new Battery;
    low_power = LOW_POWER_TIMES;

    timer_time = new QTimer();
    timer_time->setInterval(1000);   //1秒1跳
    timer_time->start();

    timer_batt = new QTimer();
    timer_batt->setInterval(10000);   //10秒1跳
    timer_batt->start();

    timer_reboot =  new QTimer();
    timer_reboot->setSingleShot(true);
    set_reboot_time();

    connect(timer_time, SIGNAL(timeout()), this, SLOT(fresh_status()) );
    connect(timer_batt, SIGNAL(timeout()), this, SLOT(fresh_batt()) );
    connect(timer_reboot, SIGNAL(timeout()), this, SLOT(system_reboot()) );

    fresh_batt();       //立刻显示一次电量


#ifdef OHV
    ui->lab_logo->setPixmap(QPixmap(":/widgetphoto/bk/ohv_gary.png").scaled(ui->lab_logo->size()));     //logo
#elif AMG
    ui->lab_logo->hide();
#else
    ui->lab_logo->hide();
#endif

}

void MainWindow::function_init(QSplashScreen *sp)
{
    tev1_widget = NULL;
    tev2_widget = NULL;
    hfct1_widget = NULL;
    hfct2_widget = NULL;
    double_widget = NULL;
    aa_widget = NULL;
    ae_widget = NULL;

    sp->showMessage(tr("正在初始化高频通道..."),Qt::AlignBottom|Qt::AlignLeft);
    channel_init((MODE)sqlcfg->get_para()->menu_h1,0);
    channel_init((MODE)sqlcfg->get_para()->menu_h2,1);
    channel_init((MODE)sqlcfg->get_para()->menu_double,2);
    sp->showMessage(tr("正在初始化低频通道..."),Qt::AlignBottom|Qt::AlignLeft);
    channel_init((MODE)sqlcfg->get_para()->menu_aa,3);
    channel_init((MODE)sqlcfg->get_para()->menu_ae,4);
    mode_list.append(Options_Mode);

    if(tev1_widget != NULL){
        connect(this, SIGNAL(send_key(quint8)), tev1_widget, SLOT(trans_key(quint8)) );
        connect(tev1_widget,SIGNAL(fresh_parent()), this, SLOT(fresh_menu_icon()) );
        //录波
        connect(tev1_widget,SIGNAL(startRecWave(MODE,int)),fifodata,SIGNAL(startRecWave(MODE,int)) );
        connect(fifodata,SIGNAL(waveData(VectorList,MODE)),tev1_widget,SLOT(showWaveData(VectorList,MODE)) );
        //重载数据
        connect(ui->tabWidget,SIGNAL(currentChanged(int)), tev1_widget, SLOT(reload(int)) );
    }
    if(tev2_widget != NULL){
        connect(this, SIGNAL(send_key(quint8)), tev2_widget, SLOT(trans_key(quint8)) );
        connect(tev2_widget,SIGNAL(fresh_parent()), this, SLOT(fresh_menu_icon()) );
        //录波
        connect(tev2_widget,SIGNAL(startRecWave(MODE,int)),fifodata,SIGNAL(startRecWave(MODE,int)) );
        connect(fifodata,SIGNAL(waveData(VectorList,MODE)),tev2_widget,SLOT(showWaveData(VectorList,MODE)) );
        //重载数据
        connect(ui->tabWidget,SIGNAL(currentChanged(int)), tev2_widget, SLOT(reload(int)) );
    }
    if(hfct1_widget != NULL){
        connect(this, SIGNAL(send_key(quint8)), hfct1_widget, SLOT(trans_key(quint8)) );
        connect(hfct1_widget,SIGNAL(fresh_parent()), this, SLOT(fresh_menu_icon()) );
        //录波
        connect(hfct1_widget,SIGNAL(startRecWave(MODE,int)),fifodata,SIGNAL(startRecWave(MODE,int)) );
        connect(fifodata,SIGNAL(waveData(VectorList,MODE)),hfct1_widget,SLOT(showWaveData(VectorList,MODE)) );
        //重载数据
        connect(ui->tabWidget,SIGNAL(currentChanged(int)), hfct1_widget, SLOT(reload(int)) );
        //菊花
        connect(hfct1_widget,SIGNAL(show_indicator(bool)), this, SLOT(show_busy(bool)) );
    }
    if(hfct2_widget != NULL){
        connect(this, SIGNAL(send_key(quint8)), hfct2_widget, SLOT(trans_key(quint8)) );
        connect(hfct2_widget,SIGNAL(fresh_parent()), this, SLOT(fresh_menu_icon()) );
        //录波
        connect(hfct2_widget,SIGNAL(startRecWave(MODE,int)),fifodata,SIGNAL(startRecWave(MODE,int)) );
        connect(fifodata,SIGNAL(waveData(VectorList,MODE)),hfct2_widget,SLOT(showWaveData(VectorList,MODE)) );
        //重载数据
        connect(ui->tabWidget,SIGNAL(currentChanged(int)), hfct2_widget, SLOT(reload(int)) );
        //菊花
        connect(hfct2_widget,SIGNAL(show_indicator(bool)), this, SLOT(show_busy(bool)) );
    }
    if(double_widget != NULL){
        connect(this, SIGNAL(send_key(quint8)), double_widget, SLOT(trans_key(quint8)) );
        connect(double_widget,SIGNAL(fresh_parent()), this, SLOT(fresh_menu_icon()) );
        //录波
        connect(fifodata,SIGNAL(waveData(VectorList,MODE)),double_widget,SLOT(showWaveData(VectorList,MODE)) );
        //重载数据
        connect(ui->tabWidget,SIGNAL(currentChanged(int)), double_widget, SLOT(reload(int)) );
    }
    if(aa_widget != NULL){
        connect(this, SIGNAL(send_key(quint8)), aa_widget, SLOT(trans_key(quint8)) );
        connect(aa_widget,SIGNAL(fresh_parent()), this, SLOT(fresh_menu_icon()) );
        //录波
        connect(aa_widget,SIGNAL(startRecWave(MODE,int)),fifodata,SIGNAL(startRecWave(MODE,int)) );
        connect(fifodata,SIGNAL(waveData(VectorList,MODE)),aa_widget,SLOT(showWaveData(VectorList,MODE)) );
        //重载数据
        connect(ui->tabWidget,SIGNAL(currentChanged(int)), aa_widget, SLOT(reload(int)) );
        //菊花
        connect(aa_widget,SIGNAL(show_indicator(bool)), this, SLOT(show_busy(bool)) );
    }
    if(ae_widget != NULL){
        connect(this, SIGNAL(send_key(quint8)), ae_widget, SLOT(trans_key(quint8)) );
        connect(ae_widget,SIGNAL(fresh_parent()), this, SLOT(fresh_menu_icon()) );
    }
}

void MainWindow::channel_init(MODE mode, int index)
{
    switch (index) {
    case 0:
        switch (mode) {
        case TEV1:
            tev1_widget = new TEVWidget(data,&key_val,mode,0,ui->H_Channel1);
            break;
        case TEV2:
            tev2_widget = new TEVWidget(data,&key_val,mode,0,ui->H_Channel1);
            break;
        case HFCT1:
            hfct1_widget = new HFCTWidget(data,&key_val,mode,0,ui->H_Channel1);
            break;
        case HFCT2:
            hfct2_widget = new HFCTWidget(data,&key_val,mode,0,ui->H_Channel1);
            break;
        default:
            mode = Disable;
            break;
        }
        break;
    case 1:
        switch (mode) {
        case TEV1:
            tev1_widget = new TEVWidget(data,&key_val,mode,1,ui->H_Channel2);
            break;
        case TEV2:
            tev2_widget = new TEVWidget(data,&key_val,mode,1,ui->H_Channel2);
            break;
        case HFCT1:
            hfct1_widget = new HFCTWidget(data,&key_val,mode,1,ui->H_Channel2);
            break;
        case HFCT2:
            hfct2_widget = new HFCTWidget(data,&key_val,mode,1,ui->H_Channel2);
            break;
        default:
            mode = Disable;
            break;
        }
        break;
    case 2:
        if(mode == Double_Channel){
            double_widget = new FaultLocation(data,&key_val,mode_list,2,ui->Double_Channel);
        }
        else{
            mode = Disable;
        }
        break;
    case 3:
        if(mode == AA_Ultrasonic){
            aa_widget = new AAWidget(data,&key_val,3,ui->L_Channel1);
        }
        else{
            mode = Disable;
        }
        break;
    case 4:
        if(mode == AE_Ultrasonic){
            ae_widget = new AEWidget(data,&key_val,4,ui->L_Channel2);
        }
        else{
            mode = Disable;
        }
        break;
    default:
        break;
    }

    mode_list.append(mode);
}

void MainWindow::options_init()
{
    options = new Options(ui->Options,data);
    debugset = new DebugSet(data,ui->Options);
    systeminfo = new SystemInfo(ui->Options);
    factoryreset = new FactoryReset(ui->Options);
    recwavemanage = new RecWaveManage(ui->Options);

    connect(this, SIGNAL(send_key(quint8)), options, SLOT(trans_key(quint8)) );
    connect(this, SIGNAL(send_key(quint8)), debugset, SLOT(trans_key(quint8)) );
    connect(this, SIGNAL(send_key(quint8)), systeminfo, SLOT(trans_key(quint8)) );
    connect(this, SIGNAL(send_key(quint8)), factoryreset, SLOT(trans_key(quint8)) );
    connect(this, SIGNAL(send_key(quint8)), recwavemanage, SLOT(trans_key(quint8)) );
    connect(options,SIGNAL(fresh_parent()), this, SLOT(fresh_menu_icon()) );
    connect(debugset,SIGNAL(fresh_parent()), this, SLOT(fresh_menu_icon()) );
    connect(systeminfo,SIGNAL(fresh_parent()), this, SLOT(fresh_menu_icon()) );
    connect(factoryreset,SIGNAL(fresh_parent()), this, SLOT(fresh_menu_icon()) );
    connect(recwavemanage,SIGNAL(fresh_parent()), this, SLOT(fresh_menu_icon()) );
    //显示信息
    connect(options,SIGNAL(update_statusBar(QString)), this, SLOT(show_message(QString)) );
    connect(options, SIGNAL(closeTimeChanged(int)), this, SLOT(set_reboot_time()) );
    connect(options,SIGNAL(fregChanged(int)),this,SLOT(fresh_status()) );
    connect(debugset,SIGNAL(update_statusBar(QString)), this, SLOT(show_message(QString)) );
    //播放声音
    connect(recwavemanage,SIGNAL(play_voice(VectorList)),fifodata,SIGNAL(playVoiceData(VectorList)));
    connect(recwavemanage,SIGNAL(stop_play_voice()),fifodata,SIGNAL(stop_play_voice()));
    connect(fifodata,SIGNAL(playVoiceProgress(int,int,bool)),recwavemanage,SLOT(playVoiceProgress(int,int,bool)));
    //键盘
    connect(options,SIGNAL(show_input(QString)),this,SIGNAL(show_input(QString)));
    connect(options,SIGNAL(send_input_key(quint8)),this,SIGNAL(send_input_key(quint8)) );
    connect(this, SIGNAL(input_str(QString)), options, SLOT(input_finished(QString)) );
    //菊花
    connect(options,SIGNAL(show_indicator(bool)), this, SLOT(show_busy(bool)) );
    connect(recwavemanage,SIGNAL(show_indicator(bool)), this, SLOT(show_busy(bool)) );
    //状态栏
    connect(options,SIGNAL(show_wifi_icon(int)), this, SLOT(set_wifi_icon(int)) );
}

void MainWindow::qml_init()
{
    busyIndicator = new QQuickWidget(this);
//    busyIndicator->setAttribute(Qt::WA_AlwaysStackOnTop);
//    busyIndicator->setAttribute(Qt::WA_TranslucentBackground,true);
//    busyIndicator->setStyleSheet("background: transparent;");
//    busyIndicator->setAttribute(Qt::WA_OpaquePaintEvent);

    busyIndicator->setClearColor(QColor(Qt::transparent));
    busyIndicator->setSource(QUrl(QStringLiteral("qrc:/Busy.qml")));


//    busyIndicator->hide();
}

void MainWindow::trans_key(quint8 key_code)
{
    //    qDebug()<<"val0 = "<<key_val.grade.val0 <<"\nval1 = "<<key_val.grade.val1 <<"\nval2 = "<<key_val.grade.val2 ;

    set_reboot_time();          //接到任何按键，重置重启计时器

    if(key_val.grade.val0 == 5 && key_val.grade.val2 !=0 ){
        emit send_key(key_code);
        return;
    }


    switch (key_code) {
    case KEY_LEFT:
        if(key_val.grade.val1 == 0 ){
            do{
                Common::change_index(key_val.grade.val0, -1, 5, 0);
            }
            while(mode_list.at(key_val.grade.val0) == Disable);
            ui->tabWidget->setCurrentIndex(key_val.grade.val0);
        }
        break;
    case KEY_RIGHT:
        if(key_val.grade.val1 == 0 ){
            do{
                Common::change_index(key_val.grade.val0, 1, 5, 0);
            }
            while(mode_list.at(key_val.grade.val0) == Disable);
            ui->tabWidget->setCurrentIndex(key_val.grade.val0);
        }
        break;
    case KEY_OK:
        if(key_val.grade.val0 == 5){
            switch (key_val.grade.val1) {
            case 1:
                key_val.grade.val2 = 1;
                options->working(&key_val);
                break;
            case 2:
                key_val.grade.val2 = 1;
                debugset->working(&key_val);
                break;
            case 3:
                key_val.grade.val2 = 1;
                recwavemanage->working(&key_val);
                break;
            case 4:
                key_val.grade.val2 = 1;
                systeminfo->working(&key_val);
                break;
            case 5:
                key_val.grade.val2 = 1;
                factoryreset->working(&key_val);
                break;
            default:
                break;
            }
            fresh_menu_icon();
        }
        break;
    case KEY_UP:
        if (key_val.grade.val0 == 5) {
            Common::change_index(key_val.grade.val1,-1,5,1);
        }
//        else{
//            key_val.grade.val1 = 1;
//            emit send_key(key_code);
//        }
        break;
    case KEY_DOWN:
        if (key_val.grade.val0 == 5) {
            Common::change_index(key_val.grade.val1,1,5,1);
        }
//        else{
//            key_val.grade.val1 = 1;
//            emit send_key(key_code);
//        }
        break;
    case KEY_CANCEL:
        if (key_val.grade.val1 > 0){
            key_val.grade.val1 = 0;
            emit send_key(key_code);
        }
        break;
    default:
        break;
    }

    fresh_grade1();
    fresh_menu_icon();
    if(key_val.grade.val0 != 5 /*&& key_val.grade.val1 !=0*/ ){
        emit send_key(key_code);
        return;
    }
}

void MainWindow::fresh_menu_icon()
{
    //先根据图标的刷新模式，绘制非当前图标
    if(key_val.grade.val1 == 0){
        set_non_current_menu_icon();
    }
    else{
        set_disable_menu_icon();
    }

    //再刷新当前图标
    switch (key_val.grade.val0) {
    case 0:
        switch (mode_list.at(0)) {
        case TEV1:
            menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/TEV1_1.png"));
            ui->lab_imformation->setText(tr("地电波检测(通道1)"));
            break;
        case TEV2:
            menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/TEV2_1.png"));
            ui->lab_imformation->setText(tr("地电波检测(通道2)"));
            break;
        case HFCT1:
            menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/HFCT1_1.png"));
            ui->lab_imformation->setText(tr("电缆局放检测(通道1)"));
            break;
        case HFCT2:
            menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/HFCT2_1.png"));
            ui->lab_imformation->setText(tr("电缆局放检测(通道2)"));
            break;
        default:
            break;
        }
        break;
    case 1:
        switch (mode_list.at(1)) {
        case TEV1:
            menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/TEV1_1.png"));
            ui->lab_imformation->setText(tr("地电波检测(通道1)"));
            break;
        case TEV2:
            menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/TEV2_1.png"));
            ui->lab_imformation->setText(tr("地电波检测(通道2)"));
            break;
        case HFCT1:
            menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/HFCT1_1.png"));
            ui->lab_imformation->setText(tr("电缆局放检测(通道1)"));
            break;
        case HFCT2:
            menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/HFCT2_1.png"));
            ui->lab_imformation->setText(tr("电缆局放检测(通道2)"));
            break;
        default:
            break;
        }
        break;
    case 2:
        menu_icon2->setPixmap(QPixmap(":/widgetphoto/menu/Double_1.png"));
        ui->lab_imformation->setText(tr("双通道检测"));
        break;
    case 3:
        menu_icon3->setPixmap(QPixmap(":/widgetphoto/menu/AA_1.png"));
        ui->lab_imformation->setText(tr("AA超声波检测"));
        break;
    case 4:
        menu_icon4->setPixmap(QPixmap(":/widgetphoto/menu/AE_1.png"));
        ui->lab_imformation->setText(tr("AE超声波检测"));
        break;
    case 5:
        menu_icon5->setPixmap(QPixmap(":/widgetphoto/menu/Option_1.png"));
        ui->lab_imformation->setText(tr("系统设置"));
        if (!key_val.grade.val1) {
            ui->lab_imformation->setText(tr("系统设置"));
        } else if (key_val.grade.val1 == 1){
            ui->lab_imformation->setText(tr("系统设置-参数设置"));
        } else if (key_val.grade.val1 == 2) {
            ui->lab_imformation->setText(tr("系统设置-调试模式"));
        } else if (key_val.grade.val1 == 3){
            ui->lab_imformation->setText(tr("系统设置-录波管理"));
        } else if (key_val.grade.val1 == 4) {
            ui->lab_imformation->setText(tr("系统设置-系统信息"));
        } else if (key_val.grade.val1 == 5) {
            ui->lab_imformation->setText(tr("系统设置-恢复出厂"));
        }
        break;
    default:
        break;
    }
}

//非活动菜单
//存在于光标位于顶层菜单，可用左右键切换时，非当前菜单的图标
//资源文件后缀为2
void MainWindow::set_non_current_menu_icon()
{
    switch (mode_list.at(0)) {
    case TEV1:
        menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/TEV1_2.png"));
        break;
    case TEV2:
        menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/TEV2_2.png"));
        break;
    case HFCT1:
        menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/HFCT1_2.png"));
        break;
    case HFCT2:
        menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/HFCT2_2.png"));
        break;
    default:                     //禁用
        if(mode_list.at(1) == TEV1){
            menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/TEV2_0.png"));
        }
        else{
            menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/TEV1_0.png"));
        }
        break;
    }

    switch (mode_list.at(1)) {
    case TEV1:
        menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/TEV1_2.png"));
        break;
    case TEV2:
        menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/TEV2_2.png"));
        break;
    case HFCT1:
        menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/HFCT1_2.png"));
        break;
    case HFCT2:
        menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/HFCT2_2.png"));
        break;
    default:                     //禁用
        if(mode_list.at(0) == TEV2){
            menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/TEV1_0.png"));
        }
        else{
            menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/TEV2_0.png"));
        }
        break;
    }

    switch (mode_list.at(2)) {
    case Double_Channel:
        menu_icon2->setPixmap(QPixmap(":/widgetphoto/menu/Double_2.png"));
        break;
    default:                     //禁用
        menu_icon2->setPixmap(QPixmap(":/widgetphoto/menu/Double_0.png"));
        break;
    }

    switch (mode_list.at(3)) {
    case AA_Ultrasonic:
        menu_icon3->setPixmap(QPixmap(":/widgetphoto/menu/AA_2.png"));
        break;
    default:                     //禁用
        menu_icon3->setPixmap(QPixmap(":/widgetphoto/menu/AA_0.png"));
        break;
    }

    switch (mode_list.at(4)) {
    case AE_Ultrasonic:
        menu_icon4->setPixmap(QPixmap(":/widgetphoto/menu/AE_2.png"));
        break;
    default:                     //禁用
        menu_icon4->setPixmap(QPixmap(":/widgetphoto/menu/AE_0.png"));
        break;
    }

    menu_icon5->setPixmap(QPixmap(":/widgetphoto/menu/Option_2.png"));
}

//禁用菜单
//存在于光标操作子项目，其他全禁用的时候
//资源文件后缀为0
void MainWindow::set_disable_menu_icon()
{
    switch (mode_list.at(0)) {
    case TEV1:
        menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/TEV1_0.png"));
        break;
    case TEV2:
        menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/TEV2_0.png"));
        break;
    case HFCT1:
        menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/HFCT1_0.png"));
        break;
    case HFCT2:
        menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/HFCT2_0.png"));
        break;
    default:                     //禁用
        if(mode_list.at(1) == TEV1){
            menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/TEV2_0.png"));
        }
        else{
            menu_icon0->setPixmap(QPixmap(":/widgetphoto/menu/TEV1_0.png"));
        }
        break;
    }

    switch (mode_list.at(1)) {
    case TEV1:
        menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/TEV1_0.png"));
        break;
    case TEV2:
        menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/TEV2_0.png"));
        break;
    case HFCT1:
        menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/HFCT1_0.png"));
        break;
    case HFCT2:
        menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/HFCT2_0.png"));
        break;
    default:                     //禁用
        if(mode_list.at(0) == TEV2){
            menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/TEV1_0.png"));
        }
        else{
            menu_icon1->setPixmap(QPixmap(":/widgetphoto/menu/TEV2_0.png"));
        }
        break;
    }

    menu_icon2->setPixmap(QPixmap(":/widgetphoto/menu/Double_0.png"));
    menu_icon3->setPixmap(QPixmap(":/widgetphoto/menu/AA_0.png"));
    menu_icon4->setPixmap(QPixmap(":/widgetphoto/menu/AE_0.png"));
    menu_icon5->setPixmap(QPixmap(":/widgetphoto/menu/Option_0.png"));
}

//刷新系统设置页面的二级菜单
void MainWindow::fresh_grade1()
{
    ui->lab_Options->setStyleSheet("QLabel {border-image: url(:/widgetphoto/bk/grade2_bk.png);color:gray;}");
    ui->lab_Debug->setStyleSheet("QLabel {border-image: url(:/widgetphoto/bk/grade2_bk.png);color:gray;}");
    ui->lab_RecWave->setStyleSheet("QLabel {border-image: url(:/widgetphoto/bk/grade2_bk.png);color:gray;}");
    ui->lab_SysInfo->setStyleSheet("QLabel {border-image: url(:/widgetphoto/bk/grade2_bk.png);color:gray;}");
    ui->lab_factory->setStyleSheet("QLabel {border-image: url(:/widgetphoto/bk/grade2_bk.png);color:gray;}");

    switch (key_val.grade.val1) {
    case 1:
        ui->lab_Options->setStyleSheet("QLabel {border-image: url(:/widgetphoto/bk/grade2_bk.png);color:white;}");
        break;
    case 2:
        ui->lab_Debug->setStyleSheet("QLabel {border-image: url(:/widgetphoto/bk/grade2_bk.png);color:white;}");
        break;
    case 3:
        ui->lab_RecWave->setStyleSheet("QLabel {border-image: url(:/widgetphoto/bk/grade2_bk.png);color:white;}");
        break;
    case 4:
        ui->lab_SysInfo->setStyleSheet("QLabel {border-image: url(:/widgetphoto/bk/grade2_bk.png);color:white;}");
        break;
    case 5:
        ui->lab_factory->setStyleSheet("QLabel {border-image: url(:/widgetphoto/bk/grade2_bk.png);color:white;}");
        break;
    default:
        break;
    }
}

void MainWindow::set_reboot_time()
{
    int m = sqlcfg->get_para()->close_time;
    if(m != 0){
        timer_reboot->setInterval(m*60 *1000);
        timer_reboot->start();
//        qDebug()<<"reboot timer started!  interval is :"<<m*60<<"sec";
    }
    else if(timer_reboot->isActive()){
        timer_reboot->stop();
        qDebug()<<"reboot timer stoped!";
    }
}

void MainWindow::fresh_status()
{
    ui->lab_time->setText(QDate::currentDate().toString("yyyy年M月d日")
                          + " "
                          + QTime::currentTime().toString("h:mm:ss"));

    int s = timer_reboot->remainingTime() / 1000;    //自动关机秒数
    if(timer_reboot->isActive() && s < 60){
        ui->lab_imformation->setText(tr("再过%1秒将自动关机，按任意键取消").arg(s));
    }
    ui->lab_freq->setText(QString("%1Hz").arg(sqlcfg->get_para()->freq_val));

}

void MainWindow::fresh_batt()
{
    int batt_val;

    batt_val = battery->battValue();

    //自动关机
    if(battery->is_low_power()){
        low_power--;
        if(low_power == 0){
            system_reboot();
        }
    }
    else{
        low_power = LOW_POWER_TIMES;      //检测错误,重置
    }

    //UI
    ui->lab_pwr_num->setText(QString("%1%").arg(batt_val));
    if(batt_val>25){
        ui->lab_pwr_num->setStyleSheet("QLabel {color:white;}");
    }
    else{
        ui->lab_pwr_num->setStyleSheet("QLabel {color:red;}");
    }

    switch (batt_val / 10) {
    case 0:
        ui->lab_pwr->setStyleSheet("QLabel {border-image: url(:/widgetphoto/pwr/pwr0.png);}");
        break;
    case 1:
        ui->lab_pwr->setStyleSheet("QLabel {border-image: url(:/widgetphoto/pwr/pwr10.png);}");
        break;
    case 2:
        ui->lab_pwr->setStyleSheet("QLabel {border-image: url(:/widgetphoto/pwr/pwr20.png);}");
        break;
    case 3:
        ui->lab_pwr->setStyleSheet("QLabel {border-image: url(:/widgetphoto/pwr/pwr30.png);}");
        break;
    case 4:
        ui->lab_pwr->setStyleSheet("QLabel {border-image: url(:/widgetphoto/pwr/pwr40.png);}");
        break;
    case 5:
        ui->lab_pwr->setStyleSheet("QLabel {border-image: url(:/widgetphoto/pwr/pwr50.png);}");
        break;
    case 6:
        ui->lab_pwr->setStyleSheet("QLabel {border-image: url(:/widgetphoto/pwr/pwr60.png);}");
        break;
    case 7:
        ui->lab_pwr->setStyleSheet("QLabel {border-image: url(:/widgetphoto/pwr/pwr70.png);}");
        break;
    case 8:
        ui->lab_pwr->setStyleSheet("QLabel {border-image: url(:/widgetphoto/pwr/pwr80.png);}");
        break;
    case 9:
        ui->lab_pwr->setStyleSheet("QLabel {border-image: url(:/widgetphoto/pwr/pwr90.png);}");
        break;
    case 10:
        ui->lab_pwr->setStyleSheet("QLabel {border-image: url(:/widgetphoto/pwr/pwr100.png);}");
        break;
    default:
        break;
    }
}

void MainWindow::system_reboot()
{
    qDebug()<<"system will reboot immediately!";
    system("reboot");
}

void MainWindow::show_message(QString str)
{
    ui->lab_imformation->setText(str);
}

void MainWindow::show_busy(bool f)
{
    QObject *busy = busyIndicator->rootObject()->findChild<QObject*>("busy");
    busy->setProperty("running",f);
}

void MainWindow::set_wifi_icon(int w)
{
    switch (w) {
    case WIFI_AP:
        ui->lab_wifi->setPixmap(QPixmap(":/widgetphoto/wifi/wifi3.png").scaled(ui->lab_wifi->size()));
        break;
    case WIFI_HOTPOT:
        ui->lab_wifi->setPixmap(QPixmap(":/widgetphoto/wifi/wifi_hot.png").scaled(ui->lab_wifi->size()));
//        ui->lab_wifi->setPixmap(QPixmap(":/widgetphoto/wifi/wifi_hot.png") );
        break;
    case WIFI_SYNC:

        break;
    default:
        ui->lab_wifi->setPixmap(QPixmap(":/widgetphoto/wifi/wifi0.png").scaled(ui->lab_wifi->size()));
        break;
    }
}

#ifdef PRINTSCREEN
void MainWindow::printSc()
{
    //    QPixmap fullScreenPixmap = this->grab(this->rect());                      //老的截屏方式，只能截取指定Wdiget及其子类
    QPixmap fullScreenPixmap = QGuiApplication::primaryScreen()->grabWindow(0);     //新截屏方式更加完美
    bool flag = fullScreenPixmap.save(QString("./ScreenShots/ScreenShots-%1.png").arg(QTime::currentTime().toString("hh-mm-ss")),"PNG");
    if(flag)
        qDebug()<<"fullScreen saved!";
    else
        qDebug()<<"fullScreen failed!";
}
#endif
