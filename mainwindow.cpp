#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    std::srand(std::time(NULL));

    setWindowTitle(tr("VKR"));
    QSettings settings("ASh", "VKR");

    resize(settings.value("mainWindow/size", QSize(600, 600)).toSize());
    move(settings.value("mainWindow/position", QPoint(200, 200)).toPoint());


    last_directory_used = settings.value("name/last_directory_used",QDir().homePath()).toString();


    createActions();
    createMenus();

    //группа для первого алгоритма
    Sigma_text = new QLabel(this);
    Sigma_text->setText("Сигма =");
    threshold_text = new QLabel(this);
    threshold_text->setText("Предел =");
    Min_text = new QLabel(this);
    Min_text->setText("Минимальный элемент =");

    QGroupBox* alg1_group = new QGroupBox(tr("Эффективная сегментаци на графах"));
    QVBoxLayout* egbis_text = new QVBoxLayout;
    egbis_text->addWidget(Sigma_text);
    egbis_text->addWidget(threshold_text);
    egbis_text->addWidget(Min_text);
    alg1_group->setLayout(egbis_text);

    stackedWidget = new QStackedWidget;
    stackedWidget->addWidget(alg1_group);

    //группа для второго алгоритма

    MeanNumberK_text = new QLabel(this);
    MeanNumberK_text->setText("Количество кластеров =");
    QGroupBox* alg2_group = new QGroupBox(tr("К-средних"));
    QVBoxLayout* kmean_text = new QVBoxLayout;
    kmean_text->addWidget(MeanNumberK_text);

    alg2_group->setLayout(kmean_text);
    stackedWidget->addWidget(alg2_group);

    //группа для третьего алгоритма

    MeanNumberGMM_text = new QLabel(this);
    MeanNumberGMM_text->setText("Количество кластеров =");
    QGroupBox* alg3_group = new QGroupBox(tr("GMM"));
    QVBoxLayout* gmm_text = new QVBoxLayout;
    gmm_text->addWidget(MeanNumberGMM_text);

    alg3_group->setLayout(gmm_text);
    stackedWidget->addWidget(alg3_group);

    // виджет для отображения изображения
    scrollArea = new QScrollArea;
    imageLabel = new PixmapWidget(scrollArea);
    imageLabel->setBackgroundRole(QPalette::Dark);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->setFocusPolicy(Qt::StrongFocus);

    imageLabel->setMouseTracking(true);
    imageLabel->installEventFilter(this);
    imageLabel->setAcceptDrops(true);
    imageLabel->resize(280, 480);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->set_text(tr("<перетащите_изображение>"));

    // виджет
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->installEventFilter(this);


    scale_spin0 = new QSpinBox;
    scale_spin0->setSingleStep(25);
    scale_spin0->setMinimum(1);
    scale_spin0->setMaximum(5000);
    scale_spin0->setSuffix(" %");

    scale_slider0 = new QSlider(Qt::Horizontal, this);
    scale_slider0->setTickPosition(QSlider::TicksAbove);
    scale_slider0->setMinimum(1);
    scale_slider0->setMaximum(1000);
    scale_slider0->setTickInterval(100);
    scale_slider0->setSingleStep(25);
    scale_slider0->setValue(100);

    QFormLayout* scale_spin_layout = new QFormLayout;
    scale_spin_layout->addRow(tr("масштаб :"), scale_spin0);
    scale_spin_layout->setFormAlignment(Qt::AlignRight);

    QVBoxLayout* scale_layout = new QVBoxLayout;
    scale_layout->addLayout(scale_spin_layout);
    scale_layout->addWidget(scale_slider0);

    QGroupBox* scale_group = new QGroupBox(tr("Изображение"));
    scale_group->setLayout(scale_layout);
    scale_spin0->installEventFilter(this);
    scale_slider0->installEventFilter(this);
    scale_spin0->setMouseTracking(true);
    scale_slider0->setMouseTracking(true);

    connect(scale_spin0,SIGNAL(valueChanged(int)),this,SLOT(do_scale0(int)));
    connect(scale_slider0,SIGNAL(valueChanged(int)),scale_spin0,SLOT(setValue(int)));
    connect(scale_spin0,SIGNAL(valueChanged(int)),scale_slider0,SLOT(setValue(int)));

    //
    QVBoxLayout* info_layout = new QVBoxLayout;
    info_layout->addWidget(stackedWidget);
    info_layout->addWidget(scale_group);
    info_layout->addStretch(1);



    QGridLayout* main_layout = new QGridLayout;

    main_layout->addWidget(scrollArea, 0, 0);
    main_layout->addLayout(info_layout, 1, 0);



    main_layout->setColumnStretch ( 0, 1 );

    main_layout->setSizeConstraint(QLayout::SetMinimumSize);

    QLabel* main_widget = new QLabel(this);
    main_widget->setLayout(main_layout);

    setCentralWidget(main_widget);


    settings_window = new QDialog(this);

    settings_window->setWindowTitle(tr("Настройки"));

    dial_buttons = new QDialogButtonBox(settings_window);

    dial_buttons->addButton(QDialogButtonBox::Ok);
    dial_buttons->addButton(QDialogButtonBox::Cancel);
    dial_buttons->addButton(QDialogButtonBox::Reset);

    connect( dial_buttons, SIGNAL(accepted()), settings_window, SLOT(accept()) );
    connect( dial_buttons, SIGNAL(rejected()), settings_window, SLOT(reject()) );
    connect( dial_buttons->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(default_settings()) );

    QWidget* alg = new QWidget;

    alg->setLayout(algorithm());

    QGridLayout *settings_grid = new QGridLayout;
    settings_grid->addWidget(alg, 0, 0);
    settings_grid->addWidget(dial_buttons,1,1);

    settings_grid->setColumnStretch ( 1, 1 );
    //
    settings_window->setLayout(settings_grid);

    sigma = settings.value("Settings/Algorithm/sigma", 0.5).toFloat();
    threshold = settings.value("Settings/Algorithm/threshold", 1500).toFloat();
    min_component_size = settings.value("Settings/Algorithm/min_component_size", 20).toInt();
    mean_k = settings.value("Settings/Algorithm/mean_k", 4).toInt();
    mean_gmm = settings.value("Settings/Algorithm/mean_gmm", 4).toInt();
    model = settings.value("Settings/Algorithm/model", 1).toInt();



    cancel_settings();


    /////////// окно вычисления отклонения /////////////////////
    // создаем окно
    evaluation_window = new QDialog(this);
    evaluation_window->setWindowTitle(tr("Отклонение"));

    evaluation_window->move(settings.value("evaluationWindow/position", QPoint(200, 200)).toPoint());
    evaluation_window->resize(settings.value("evaluationWindow/size", QSize(665,542)).toSize());

    // изображение А
    scrollAreaA = new QScrollArea;
    imageLabelA = new PixmapWidget(scrollAreaA);
    imageLabelA->setBackgroundRole(QPalette::Dark);
    imageLabelA->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabelA->setScaledContents(true);
    imageLabelA->setMouseTracking(true);
    imageLabelA->installEventFilter(this);
    imageLabelA->setAcceptDrops(true);
    imageLabelA->resize(200, 200);
    imageLabelA->setAlignment(Qt::AlignCenter);
    imageLabelA->set_text(tr("<перетащите изображение>"));

    scrollAreaA->setBackgroundRole(QPalette::Dark);
    scrollAreaA->setWidget(imageLabelA);
    scrollAreaA->setAlignment(Qt::AlignCenter);
    scrollAreaA->setWidgetResizable(true);


    //изображение Б (ручная сегментация)
    scrollAreaB = new QScrollArea;
    imageLabelB = new PixmapWidget(scrollAreaB);
    imageLabelB->setBackgroundRole(QPalette::Dark);
    imageLabelB->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabelB->setScaledContents(true);
    imageLabelB->setMouseTracking(true);
    imageLabelB->installEventFilter(this);
    imageLabelB->setAcceptDrops(true);
    imageLabelB->resize(200, 200);
    imageLabelB->setAlignment(Qt::AlignCenter);
    imageLabelB->set_text(tr("<перетащите изображение>"));

    scrollAreaB->setBackgroundRole(QPalette::Dark);
    scrollAreaB->setWidget(imageLabelB);
    scrollAreaB->setAlignment(Qt::AlignCenter);
    scrollAreaB->setWidgetResizable(true);

    QPushButton* open1_button = new QPushButton(tr("Открыть изображение"));
    connect(open1_button,SIGNAL(clicked()),this,SLOT(openFileNameA()));

    QPushButton* open2_button = new QPushButton(tr("Открыть изображение"));
    connect(open2_button,SIGNAL(clicked()),this,SLOT(openFileNameB()));


    scale_spinA = new QSpinBox;
    scale_spinA->setSingleStep(25);
    scale_spinA->setMinimum(1);
    scale_spinA->setMaximum(5000);
    scale_spinA->setSuffix(" %");
    scale_spinA->setValue(100);
    connect(scale_spinA,SIGNAL(valueChanged(int)),this,SLOT(scaleA(int)));

    scale_spinB = new QSpinBox;
    scale_spinB->setSingleStep(25);
    scale_spinB->setMinimum(1);
    scale_spinB->setMaximum(5000);
    scale_spinB->setSuffix(" %");
    scale_spinB->setValue(100);
    connect(scale_spinB,SIGNAL(valueChanged(int)),this,SLOT(scaleB(int)));

    scale_spinA->installEventFilter(this);
    scale_spinB->installEventFilter(this);
    scale_spinA->setMouseTracking(true);
    scale_spinB->setMouseTracking(true);

    QFormLayout* scaleA = new QFormLayout;
    scaleA->addRow(tr("Масштаб :"),scale_spinA);

    QFormLayout* scaleB = new QFormLayout;
    scaleB->addRow(tr("Масштаб :"),scale_spinB);

    label_name1 = new QLabel(this);
    label_name2 = new QLabel(this);

    label_name1->setText(tr("Имя - размер"));
    label_name2->setText(tr("Имя - размер"));
    label_name1->setAlignment(Qt::AlignCenter);
    label_name2->setAlignment(Qt::AlignCenter);

    QVBoxLayout* imageA_layout = new QVBoxLayout;
    imageA_layout->addWidget(label_name1);
    imageA_layout->addWidget(scrollAreaA);
    imageA_layout->addLayout(scaleA);
    imageA_layout->addWidget(open1_button);

    QVBoxLayout* imageB_layout = new QVBoxLayout;
    imageB_layout->addWidget(label_name2);
    imageB_layout->addWidget(scrollAreaB);
    imageB_layout->addLayout(scaleB);
    imageB_layout->addWidget(open2_button);

    QGroupBox* imageA_group = new QGroupBox(tr("Изображение A"));
    imageA_group->setLayout(imageA_layout);

    QGroupBox* imageB_group = new QGroupBox(tr("Изображение B"));
    imageB_group->setLayout(imageB_layout);

    text_A = new QLabel(this);
    text_B = new QLabel(this);
    text_A->setAlignment(Qt::AlignCenter);
    text_B->setAlignment(Qt::AlignCenter);

    text_A->setText("<font color=red>"+tr("Изображение для автоматической сегментации"));
    text_B->setText("<font color=red>"+tr("Изображение с выполненной ручной сегментацией"));


    QVBoxLayout* left_layout = new QVBoxLayout;
    left_layout->addWidget(text_A);
    left_layout->addWidget(imageA_group);


    QVBoxLayout* right_layout = new QVBoxLayout;
    right_layout->addWidget(text_B);
    right_layout->addWidget(imageB_group);

    egbs_radio_button = new QRadioButton(tr("EGBS"));
    kmean_radio_button = new QRadioButton(tr("K-Mean"));
    gmm_radio_button = new QRadioButton(tr("GMM"));

    sample_spin = new QSpinBox;
    sample_spin->setSingleStep(1);
    sample_spin->setMinimum(1);
    sample_spin->setMaximum(100000);

    QHBoxLayout* alg1_layout = new QHBoxLayout;
    alg1_layout->addWidget(egbs_radio_button);
    alg1_layout->addWidget(kmean_radio_button);
    alg1_layout->addWidget(gmm_radio_button);

    QVBoxLayout* alg2_layout = new QVBoxLayout;
    alg2_layout->addLayout(alg1_layout);
    alg2_layout->addWidget(sample_spin);


    QGroupBox* alg_group = new QGroupBox(tr("Сравнение"));
    alg_group->setLayout(alg2_layout);

    QLabel *text_C = new QLabel(this);
    text_C->setAlignment(Qt::AlignCenter);
    text_C->setText("<font color=red>"+tr("Выбор алгоритма и выборки"));

    QPushButton* evaluate_button = new QPushButton(tr("Начать"));
    connect(evaluate_button,SIGNAL(clicked()),this,SLOT(evaluate()));

    eval = new QLabel(this);

    QVBoxLayout* bottom_layout = new QVBoxLayout;
    bottom_layout->addWidget(text_C);
    bottom_layout->addWidget(alg_group);
    bottom_layout->addWidget(evaluate_button);
    bottom_layout->addWidget(eval);


    QHBoxLayout* lists_select_layout = new QHBoxLayout;
    lists_select_layout->addLayout(left_layout);
    lists_select_layout->addLayout(right_layout);
  //  lists_select_layout->addLayout(bottom_layout);

    QVBoxLayout* lists_select_layout2 = new QVBoxLayout;
    lists_select_layout2->addLayout(lists_select_layout);
    lists_select_layout2->addLayout(bottom_layout);

    evaluation_window->setLayout(lists_select_layout2);

    scale_spinA->setValue(settings.value("Evaluation/spinA", 100).toInt());
    scale_spinB->setValue(settings.value("Evaluation/spinB", 100).toInt());



}

MainWindow::~MainWindow()
{

}


// Создание действий в строке меню
void MainWindow::createActions()
{
    openAct = new QAction(tr("Открыть..."), this);
    openAct->setStatusTip(tr("Открыть файл изображения (*.png, *.bmp, *.jpg, *.jpeg, *.tiff, *.tif, *.gif, *.pbm, *.pgm, *.ppm, *.svg, *.svgz, *.mng, *.xbm, *.xpm)."));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFileName()));

    for( int i = 0; i < MaxRecentFiles; ++i )
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    deleteAct = new QAction(tr("×   Очистить список"), this);
    deleteAct->setStatusTip(tr("Очистить список файлов"));
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteList()));

    saveAct = new QAction(tr("Сохранить..."), this);
    saveAct->setEnabled(false);
    saveAct->setStatusTip(tr("Сохранить изображение"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(saveImage()));

    exitAct = new QAction(tr("Выход"), this);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    zoomInAct = new QAction(tr("Увеличить (25%)"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Уменьшить (25%)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("Нормальный размер"), this);
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    startAct = new QAction(tr("Старт"), this);
    startAct->setEnabled(false);
    startAct->setStatusTip(tr("Начать сегментацию"));
    connect(startAct, SIGNAL(triggered()), this, SLOT(start()));

    evaluateAct = new QAction(tr("Отклонение"), this);
    evaluateAct->setStatusTip(tr("Отклонение ручной и автоматической сегментации"));
    evaluateAct->setEnabled(true);
    connect(evaluateAct, SIGNAL(triggered()), this, SLOT(evaluation()));



    settingsAct = new QAction(tr("Настройки"), this);
    settingsAct->setEnabled(true);
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(settings()));

    menuBar()->addSeparator();
}

// Создание меню главного окна
void MainWindow::createMenus()
{
    fileMenu = new QMenu(tr("Файл"), this);
    fileMenu->addAction(openAct);

    separatorAct = fileMenu->addSeparator();
    for( int i = 0; i < MaxRecentFiles; ++i )
    {
        fileMenu->addAction(recentFileActs[i]);
    }

    fileMenu->addAction(deleteAct);

    fileMenu->addSeparator();

    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    updateRecentFileActions();

    viewMenu = new QMenu(tr("Вид"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);

    segmentationMenu = new QMenu(tr("Сегментация"), this);
    segmentationMenu->addAction(startAct);
    segmentationMenu->addAction(evaluateAct);
    segmentationMenu->addAction(settingsAct);


    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(segmentationMenu);

}

void MainWindow::setCurrentFile(const QString &fileName1)
{
    curFile = fileName1;
    //setWindowFilePath(curFile);

    QSettings settings("ASh", "VKR");
    QStringList files = settings.value("name/recentFileList").toStringList();
    files.removeAll(fileName1);
    files.prepend(fileName1);
    while( files.size() > MaxRecentFiles )
    {
        files.removeLast();
    }

    settings.setValue("name/recentFileList", files);

    foreach( QWidget *widget, QApplication::topLevelWidgets() )
    {
        MainWindow *imageViewer = qobject_cast<MainWindow *>(widget);
        if( imageViewer )
        {
            imageViewer->updateRecentFileActions();
        }
    }
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings("ASh", "VKR");
    QStringList files = settings.value("name/recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for( int i = 0; i < numRecentFiles; ++i )
    {
        QString text = tr("&%1   %2").arg(i + 1).arg( strippedName(files[i]) );
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
        recentFileActs[i]->setStatusTip(files[i]);
    }

    for( int j = numRecentFiles; j < MaxRecentFiles; ++j )
    {
        recentFileActs[j]->setVisible(false);
    }

    separatorAct->setVisible(numRecentFiles > 0);

    if( files.isEmpty() )
    {
        deleteAct->setVisible(false);
    }
    else
    {
        deleteAct->setVisible(true);
    }
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::deleteList()
{
    QStringList files;
    files.clear();

    QSettings settings("ASh", "VKR");
    settings.setValue("name/recentFileList", files);

    updateRecentFileActions();
}

void MainWindow::openFileName()
{
    fileName = QFileDialog::getOpenFileName(this,
                                            tr("Открыть файл"),
                                            last_directory_used,
                                            tr("Файл изображения (%1)").arg(nameFilters.join(" ")));


    open();
}

void MainWindow::openRecentFile()
{
    QAction* action = qobject_cast<QAction*>( sender() );
    if( action )
    {
        fileName = action->data().toString().toLatin1();
    }

    open();
}

void MainWindow::open()
{

    if( !fileName.isEmpty() )
    {
        QFileInfo fi(fileName);
            img = QImage(fileName);
            if( img.isNull() )
            {
                QMessageBox::information(this, tr("Ошибка"),
                                         tr("Невозможно загрузить %1.").arg(fileName));
                return;
            }



        imageLabel->set_qimage0(img);
        QApplication::processEvents();

        last_directory_used = fi.absolutePath();
        setCurrentFile(fileName);

        img_width = img.width();
        img_height = img.height();
        img_size = img_width*img_height;


        disconnect(scale_spin0,SIGNAL(valueChanged(int)),this,SLOT(do_scale0(int)));

        scale_spin0->setMaximum(1000000/img_height);
        scale_spin0->setSingleStep(80000/(7*img_height));

        scale_slider0->setMaximum(160000/img_height);
        scale_slider0->setTickInterval(160000/(7*img_height));

        scale_spin0->setValue(int(100.0*double(imageLabel->getPixWidth())/double(img_width)));

        connect(scale_spin0,SIGNAL(valueChanged(int)),this,SLOT(do_scale0(int)));

        Sigma_text->setText(tr("Сигма = ")+QString::number(sigma));
        Min_text->setText(tr("Минимальный элемент = ")+QString::number(min_component_size));
        threshold_text->setText(tr("Предел = ")+QString::number(threshold));
        MeanNumberK_text->setText(tr("Количество кластеров = ")+QString::number(mean_k));
        MeanNumberGMM_text->setText(tr("Количество кластеров = ")+QString::number(mean_gmm));

        zoomInAct->setEnabled(true);
        zoomOutAct->setEnabled(true);
        normalSizeAct->setEnabled(true);
        startAct->setEnabled(true);
        saveAct->setEnabled(true);

    }
}

void MainWindow::saveImage()
{
    QFileInfo fi(fileName);
    QString fileName_save;

    // выбор папки для сохранения

        fileName_save = QFileDialog::getSaveFileName(this,
                                                     tr("Сохранить изображение"),
                                                     last_directory_used + "/" + fi.baseName(),
                                                     "BMP (*.bmp);;JPG (*.jpg);;PNG (*.png);;PPM (*.ppm);;TIFF (*.tiff);;XBM (*.xbm);;XPM (*.xpm)");


    QImage img_displayed = imageLabel->get_qimage();

    if( !fileName_save.isEmpty() )
    {
        QFileInfo fi1(fileName_save);
        last_directory_used = fi1.absolutePath();


        img_displayed.save(fileName_save);
    }
}


void MainWindow::zoomIn()
{
    scale_spin0->setValue( int(100.0f*imageLabel->get_zoomFactor()*1.25f) );
}

void MainWindow::zoomOut()
{
    scale_spin0->setValue( int(100.0f*imageLabel->get_zoomFactor()*0.8f) );
}

void MainWindow::normalSize()
{
    scale_spin0->setValue(100);
}

// Масштабирование
void MainWindow::do_scale0(int value)
{
        double scale_factor = value/100.0;
        imageLabel->setZoomFactor(scale_factor);

}


bool MainWindow::eventFilter(QObject *object, QEvent *event)
{

    if( object == imageLabel && event->type() == QEvent::DragEnter )
    {
        QDragEnterEvent* drag = static_cast<QDragEnterEvent*>(event);
        dragEnterEvent(drag);
    }
    if( object == imageLabel && event->type() == QEvent::DragMove )
    {
        QDragMoveEvent* drag = static_cast<QDragMoveEvent*>(event);
        dragMoveEvent(drag);
    }
    if( object == imageLabel && event->type() == QEvent::Drop )
    {
        QDropEvent* drag = static_cast<QDropEvent*>(event);
        dropEvent(drag);
    }
    if( object == imageLabel && event->type() == QEvent::DragLeave )
    {
        QDragLeaveEvent* drag = static_cast<QDragLeaveEvent*>(event);
        dragLeaveEvent(drag);
    }

    if( object == imageLabelA && event->type() == QEvent::DragEnter )
    {
        QDragEnterEvent* drag = static_cast<QDragEnterEvent*>(event);
        dragEnterEventA(drag);
    }
    if( object == imageLabelA && event->type() == QEvent::DragMove )
    {
        QDragMoveEvent* drag = static_cast<QDragMoveEvent*>(event);
        dragMoveEventA(drag);
    }
    if( object == imageLabelA && event->type() == QEvent::Drop )
    {
        QDropEvent* drag = static_cast<QDropEvent*>(event);
        dropEventA(drag);
    }
    if( object == imageLabelA && event->type() == QEvent::DragLeave )
    {
        QDragLeaveEvent* drag = static_cast<QDragLeaveEvent*>(event);
        dragLeaveEventA(drag);
    }

    if( object == imageLabelB && event->type() == QEvent::DragEnter )
    {
        QDragEnterEvent* drag = static_cast<QDragEnterEvent*>(event);
        dragEnterEventB(drag);
    }
    if( object == imageLabelB && event->type() == QEvent::DragMove )
    {
        QDragMoveEvent* drag = static_cast<QDragMoveEvent*>(event);
        dragMoveEventB(drag);
    }
    if( object == imageLabelB && event->type() == QEvent::Drop )
    {
        QDropEvent* drag = static_cast<QDropEvent*>(event);
        dropEventB(drag);
    }
    if( object == imageLabelB && event->type() == QEvent::DragLeave )
    {
        QDragLeaveEvent* drag = static_cast<QDragLeaveEvent*>(event);
        dragLeaveEventB(drag);
    }





    return false;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{

        QString text(tr("<перетащите_изображение>"));
        imageLabel->set_text(text);
        imageLabel->setBackgroundRole(QPalette::Highlight);
        imageLabel->set_hasText(true);

        event->acceptProposedAction();
        emit changed(event->mimeData());
}

void MainWindow::dragEnterEventA(QDragEnterEvent *event)
{
    QString text(tr("<перетащите_изображение>"));
    imageLabelA->set_text(text);
    imageLabelA->setBackgroundRole(QPalette::Highlight);
    imageLabelA->set_hasText(true);

    event->acceptProposedAction();
    emit changed(event->mimeData());
}

void MainWindow::dragEnterEventB(QDragEnterEvent *event)
{
    QString text(tr("<перетащите_изображение>"));
    imageLabelB->set_text(text);
    imageLabelB->setBackgroundRole(QPalette::Highlight);
    imageLabelB->set_hasText(true);

    event->acceptProposedAction();
    emit changed(event->mimeData());
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{

        event->acceptProposedAction();
}

void MainWindow::dragMoveEventA(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dragMoveEventB(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{

        const QMimeData* mimeData = event->mimeData();

        if( mimeData->hasUrls() )
        {
            QList<QUrl> urlList = mimeData->urls();
            fileName = urlList.first().toLocalFile();
        }
        imageLabel->setBackgroundRole(QPalette::Dark);
        open();
        event->acceptProposedAction();
}

void MainWindow::dropEventA(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    if( mimeData->hasUrls() )
    {
        QList<QUrl> urlList = mimeData->urls();
        fileNameA = urlList.first().toLocalFile();
    }
    imageLabelA->setBackgroundRole(QPalette::Dark);
    open_imageA();
    event->acceptProposedAction();
}

void MainWindow::dropEventB(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    if( mimeData->hasUrls() )
    {
        QList<QUrl> urlList = mimeData->urls();
        fileNameB = urlList.first().toLocalFile();
    }
    imageLabelB->setBackgroundRole(QPalette::Dark);
    open_imageB();
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{

        QString text(tr("<перетащите_изображение>"));
        imageLabel->set_text(text);
        imageLabel->setBackgroundRole(QPalette::Dark);
        imageLabel->set_hasText(true);
        emit changed();
        event->accept();

}

void MainWindow::dragLeaveEventA(QDragLeaveEvent *event)
{
    QString text(tr("<перетащите_изображение>"));
    imageLabelA->set_text(text);
    imageLabelA->setBackgroundRole(QPalette::Dark);
    imageLabelA->set_hasText(true);
    emit changed();
    event->accept();
}

void MainWindow::dragLeaveEventB(QDragLeaveEvent *event)
{
    QString text(tr("<перетащите_изображение>"));
    imageLabelB->set_text(text);
    imageLabelB->setBackgroundRole(QPalette::Dark);
    imageLabelB->set_hasText(true);
    emit changed();
    event->accept();
}

void MainWindow::settings()
{

    int result_settings_window;
    result_settings_window = settings_window->exec();

    if( result_settings_window )
    {
        apply_settings();

    }
    else
    {
        cancel_settings();
    }

}

QVBoxLayout *MainWindow::algorithm()
{
    QGroupBox* gb = new QGroupBox(tr("Выбор алгоритма и его параметров"));

    egbs_radio = new QRadioButton(tr("EGBS"));
    kmean_radio = new QRadioButton(tr("K-Mean"));
    gmm_radio = new QRadioButton(tr("GMM"));

    sigma_spin = new QDoubleSpinBox;
    sigma_spin->setSingleStep(0.1);
    sigma_spin->setMinimum(0.0);
    sigma_spin->setMaximum(100000.0);

    threshold_spin = new QSpinBox;
    threshold_spin->setSingleStep(1);
    threshold_spin->setMinimum(0);
    threshold_spin->setMaximum(100000);

    min_component_size_spin = new QSpinBox;
    min_component_size_spin->setSingleStep(1);
    min_component_size_spin->setMinimum(0);
    min_component_size_spin->setMaximum(100000);

    kmean_spin = new QSpinBox;
    kmean_spin->setSingleStep(1);
    kmean_spin->setMinimum(0);
    kmean_spin->setMaximum(15);

    gmm_spin = new QSpinBox;
    gmm_spin->setSingleStep(1);
    gmm_spin->setMinimum(0);
    gmm_spin->setMaximum(15);


    QFormLayout* layout1 = new QFormLayout;
    layout1->addRow(tr("sigma ="), sigma_spin);
    layout1->addRow(tr("threshold ="), threshold_spin);
    layout1->addRow(tr("min_component_size ="), min_component_size_spin);

    QFormLayout* layout2 = new QFormLayout;
    layout2->addRow(tr("mean ="), kmean_spin);

    QFormLayout* layout3 = new QFormLayout;
    layout3->addRow(tr("mean ="), gmm_spin);

    QVBoxLayout* alg_layout = new QVBoxLayout;
    alg_layout->addWidget(egbs_radio);
    alg_layout->addLayout(layout1);

    QVBoxLayout* alg2_layout = new QVBoxLayout;
    alg2_layout->addWidget(kmean_radio);
    alg2_layout->addLayout(layout2);

    QVBoxLayout* alg3_layout = new QVBoxLayout;
    alg3_layout->addWidget(gmm_radio);
    alg3_layout->addLayout(layout3);



    QVBoxLayout* main_alg_layout = new QVBoxLayout;

    main_alg_layout->addLayout(alg_layout);
    main_alg_layout->addLayout(alg2_layout);
    main_alg_layout->addLayout(alg3_layout);
    gb->setLayout(main_alg_layout);

    return main_alg_layout;
}


void MainWindow::apply_settings()
{
    sigma = sigma_spin->value();
    threshold = threshold_spin->value();
    min_component_size = min_component_size_spin->value();
    mean_k = kmean_spin->value();
    mean_gmm = gmm_spin->value();


    if( egbs_radio->isChecked() )
    {
        model = 1;
    }

    if( kmean_radio->isChecked() )
    {
        model = 2;
    }

    if( gmm_radio->isChecked() )
    {
        model = 3;
    }

    QSettings settings("ASh", "VKR");

    settings.setValue("Settings/Algorithm/sigma", sigma);
    settings.setValue("Settings/Algorithm/threshold", threshold);
    settings.setValue("Settings/Algorithm/min_component_size", min_component_size);
    settings.setValue("Settings/Algorithm/mean_k", mean_k);
    settings.setValue("Settings/Algorithm/mean_gmm", mean_gmm);
    settings.setValue("Settings/Algorithm/model", model);



    Sigma_text->setText(tr("Сигма =")+QString::number(sigma));
    Min_text->setText(tr("Минимальный элемент =")+QString::number(min_component_size));
    threshold_text->setText(tr("Предел =")+QString::number(threshold));
    MeanNumberK_text->setText(tr("Количество кластеров =")+QString::number(mean_k));
    MeanNumberGMM_text->setText(tr("Количество кластеров =")+QString::number(mean_gmm));

    if( model == 1 )
    {
        stackedWidget->setCurrentIndex(0);
    }

    if( model == 2 )
    {
        stackedWidget->setCurrentIndex(1);
    }
    if( model == 3 )
    {
        stackedWidget->setCurrentIndex(2);
    }

}

void  MainWindow::cancel_settings()
{
    sigma_spin->setValue(sigma);
    threshold_spin->setValue(threshold);
    min_component_size_spin->setValue(min_component_size);
    kmean_spin->setValue(mean_k);
    gmm_spin->setValue(mean_gmm);

    if( model == 1 )
    {
        egbs_radio->setChecked(true);
    }

    if( model == 2 )
    {
        kmean_radio->setChecked(true);
    }

    if( model == 2 )
    {
        gmm_radio->setChecked(true);
    }

    QSettings settings("ASh", "VKR");

    settings.setValue("Settings/Algorithm/sigma", sigma);
    settings.setValue("Settings/Algorithm/threshold", threshold);
    settings.setValue("Settings/Algorithm/min_component_size", min_component_size);
    settings.setValue("Settings/Algorithm/mean_k", mean_k);
    settings.setValue("Settings/Algorithm/mean_gmm", mean_gmm);
    settings.setValue("Settings/Algorithm/model", model);
}

void MainWindow::default_settings()
{
    sigma             = 0.5;
    threshold         = 1500;
    min_component_size  = 20;
    mean_k              =4;
    mean_gmm            =4;
    cancel_settings();
}


void MainWindow::start()
{
    if(model==1){

    mat= cv::imread(fileName.toLatin1().data());
    egbs.applySegmentation( mat, sigma, threshold, min_component_size );
    cv::Mat random_color  = egbs.recolor(false);
    cv::cvtColor(random_color,random_color,CV_BGR2RGB);
    image_result= QImage((const unsigned char*)(random_color.data), random_color.cols,random_color.rows,QImage::Format_RGB888);
    image_result.bits();
    QApplication::processEvents();
    imageLabel->set_qimage(image_result);

    }

    if (model==2) {
        mat= cv::imread(fileName.toLatin1().data());
        KMeans kmeans;
        cv::Mat k = kmeans.applySegmentation( mat, mean_k);
        cv::cvtColor(k,k,CV_BGR2RGB);
        image_result= QImage((const unsigned char*)(k.data), k.cols,k.rows,QImage::Format_RGB888);
        image_result.bits();
        QApplication::processEvents();
        imageLabel->set_qimage(image_result);

    }


    if (model==3) {
        mat= cv::imread(fileName.toLatin1().data());
        GMM gmm;
        cv::Mat g = gmm.applySegmentation( mat, mean_gmm);
        cv::cvtColor(g,g,CV_BGR2RGB);
        image_result= QImage((const unsigned char*)(g.data), g.cols,g.rows,QImage::Format_RGB888);
        image_result.bits();
        QApplication::processEvents();
        imageLabel->set_qimage(image_result);

    }

    if( model == 1 )
    {
        stackedWidget->setCurrentIndex(0);
    }

    if( model == 2 )
    {
        stackedWidget->setCurrentIndex(1);
    }
    if( model == 3 )
    {
        stackedWidget->setCurrentIndex(2);
    }
}


// Отображать окно оценки отклонения
void MainWindow::evaluation()
{
    // если не видно
    if( !evaluation_window->isVisible() )
    {
        evaluation_window->show();
    }
}

void MainWindow::openFileNameA()
{
    fileNameA = QFileDialog::getOpenFileName(this,
                                            tr("Открыть файл"),
                                            last_directory_used,
                                            tr("Файл изображения (%1)").arg(nameFilters.join(" ")));


    open_imageA();
}

void MainWindow::openFileNameB()
{
    fileNameB = QFileDialog::getOpenFileName(this,
                                            tr("Открыть файл"),
                                            last_directory_used,
                                            tr("Файл изображения (%1)").arg(nameFilters.join(" ")));


    open_imageB();
}

void MainWindow::open_imageA()
{
    if( !fileNameA.isEmpty() )
    {
        image1 = QImage(fileNameA);
        if( image1.isNull() )
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Невозможно загрузить %1.").arg(fileName));
            return;
        }

        imageLabelA->set_qimage0(image1);
        QApplication::processEvents();

        imgA_height = image1.height();
        imgA_width = image1.width();
        imgA_size = imgA_width*imgA_height;

        QFileInfo fi1(fileNameA);
        QString name1 = fi1.fileName();

        QString string_lists_text;
        string_lists_text=QString::number(imgA_width)+"×"+QString::number(imgA_height);
        label_name1->setText(name1 +" - "+string_lists_text);
    }
}

void MainWindow::open_imageB()
{
    if( !fileNameB.isEmpty() )
    {
        image2 = QImage(fileNameB);
        if( image2.isNull() )
        {
            QMessageBox::information(this, tr("Ошибка"),
                                     tr("Невозможно загрузить %1.").arg(fileName));
            return;
        }

        imageLabelB->set_qimage0(image2);
        QApplication::processEvents();

        imgB_height = image2.height();
        imgB_width = image2.width();
        imgB_size = imgB_width*imgB_height;


        QFileInfo fi2(fileNameB);
        QString name2 = fi2.fileName();

        QString string_lists_text;
        string_lists_text=QString::number(imgB_width)+"×"+QString::number(imgB_height);
        label_name2->setText(name2 +" - "+string_lists_text);
    }
}

void MainWindow::scaleA(int value)
{
    if( !image1.isNull() )
    {
        double scale_factor = value/100.0;
        imageLabelA->setZoomFactor(scale_factor);
    }
}

void MainWindow::scaleB(int value)
{
    if( !image2.isNull() )
    {
        double scale_factor = value/100.0;
        imageLabelB->setZoomFactor(scale_factor);
    }

}

void MainWindow::evaluate()
{
    int sample = sample_spin->value();
    float zxc=0;
    float max=0;

    if (egbs_radio_button->isChecked())
    {
        for (int g=0; g<sample;g++)
        {
            cv::Mat matA= cv::imread(fileNameA.toLatin1().data());
            cv::Mat matB= cv::imread(fileNameB.toLatin1().data());

            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float th = rand()%5000;
            int mcs = rand()%500;

            egbs.applySegmentation( matA, r, th, mcs);
            cv::Mat random_color  = egbs.recolor(false);

            matA = contour(random_color, matA);
            zxc = deviation(matA, matB);
            if (zxc>max)
            {
                max=zxc;
                cv::cvtColor(matA,matA,CV_BGR2RGB);
                image_result= QImage((const unsigned char*)(matA.data), matA.cols,matA.rows,QImage::Format_RGB888);
                image_result.bits();
                QApplication::processEvents();
                imageLabelA->set_qimage(image_result);
            }
        }
        eval->setText(tr("Отклонение = ")+QString::number(100-max));
    }
    if (kmean_radio_button->isChecked())
    {
        for (int g=0; g<sample;g++)
        {
            cv::Mat matA= cv::imread(fileNameA.toLatin1().data());
            cv::Mat matB= cv::imread(fileNameB.toLatin1().data());
            int mn = rand()%6+1;
            KMeans kmeans;
            cv::Mat k = kmeans.applySegmentation( matA, mn);
            matA = contour(k, matA);
            zxc = deviation(matA, matB);
            if (zxc>max)
            {
                max=zxc;
                cv::cvtColor(matA,matA,CV_BGR2RGB);
                image_result= QImage((const unsigned char*)(matA.data), matA.cols,matA.rows,QImage::Format_RGB888);
                image_result.bits();
                QApplication::processEvents();
                imageLabelA->set_qimage(image_result);
            }
        }
        eval->setText(tr("Отклонение = ")+QString::number(100-max));
    }
    if (gmm_radio_button->isChecked())
    {
        for (int g=0; g<sample;g++)
        {
            cv::Mat matA= cv::imread(fileNameA.toLatin1().data());
            cv::Mat matB= cv::imread(fileNameB.toLatin1().data());
            int mn = rand()%3+2;
            GMM GMM;
            cv::Mat gmm = GMM.applySegmentation( matA, mn);
            matA = contour(gmm, matA);
            zxc = deviation(matA, matB);
            if (zxc>max)
            {
                max=zxc;
                cv::cvtColor(matA,matA,CV_BGR2RGB);
                image_result= QImage((const unsigned char*)(matA.data), matA.cols,matA.rows,QImage::Format_RGB888);
                image_result.bits();
                QApplication::processEvents();
                imageLabelA->set_qimage(image_result);
            }
        }
        eval->setText(tr("Отклонение = ")+QString::number(100-max));
    }
}

cv::Mat MainWindow::contour(cv::Mat img1, cv::Mat img2)
{
    cv::Mat gray,src_gray, edge, draw;
    cv::cvtColor(img1, src_gray, CV_BGR2GRAY);
    cv::blur( src_gray, gray, cv::Size(3,3) );
    cv::Canny( gray, edge, 50, 150, 3, true );
    cv::Mat contoursInv;
    cv::threshold(edge,contoursInv,128,255,  cv::THRESH_BINARY_INV);
    contoursInv.convertTo(draw, CV_8U);
    cv::Size imageSize = img2.size();

    for( int y = 0; y < imageSize.height; y++ ) {
        for( int x = 0; x < imageSize.width; x++ ) {

            if (draw.at<uchar>(y,x)==0)
            {
                img2.at<cv::Vec3b>(y, x)[0]=0;
                img2.at<cv::Vec3b>(y, x)[1]=0;
                img2.at<cv::Vec3b>(y, x)[2]=254;
            }
        }
    }
    return img2;
}

float MainWindow::deviation(Mat img1, Mat img2)
{
    cv::Size imageSize = img1.size();
    int v=0;
    int z=0;
    float dev;
    for( int y = 0; y < imageSize.height; y++ ) {
        for( int x = 0; x < imageSize.width; x++ ) {
            cv::Vec3b pixA = img2.at<cv::Vec3b>(y, x);
            if (pixA[0]==0 and pixA[1]==0 and pixA[2]==254)
            {
                cv::Vec3b pixB = img1.at<cv::Vec3b>(y, x);
                if (pixB[0]==0 and pixB[1]==0 and pixB[2]==254)
                {
                    v++;
                }
                z++;
            }
        }
    }
    dev=(v*100)/(double)z;
    return dev;
}
