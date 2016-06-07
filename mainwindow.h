#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QLabel>
#include <QMenu>
#include <QScrollArea>
#include <QScrollBar>
#include <QtWidgets>
#include "pixmapwidget.h"
#include "EGBS.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "kmeans.h"
#include "gmm.h"
#include <ctime>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void openFileName();
    void openFileNameA();
    void openFileNameB();
    void deleteList();
    void openRecentFile();
    void saveImage();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void start();
    void evaluate();
    void do_scale0(int value);
    void settings();
    void evaluation();
    void default_settings();
    void scaleA(int value);
    void scaleB(int value);



private:
    PixmapWidget* imageLabel;
    QLabel* iL;
    QScrollArea* scrollArea;
    QSpinBox* scale_spin0;
    QSlider* scale_slider0;





    QLabel* Sigma_text;
    QLabel* Min_text;
    QLabel* threshold_text;
    QLabel* MeanNumberK_text;
    QLabel* MeanNumberGMM_text;

    QStackedWidget* stackedWidget;





    // основные функции
    void open();
    void createActions();
    void createMenus();
    void scaleImage(double factor);
 //   void adjustScrollBar(QScrollBar* scrollBar, double factor);
    double scaleFactor;



    bool eventFilter(QObject* object, QEvent* event);



    QString fileName;


    int img_height;
    int img_width;
    int img_size;


    QImage img;
    QImage image_result;
    cv::Mat mat;
    EGBS egbs;


    QStringList nameFilters;
    QString last_directory_used;
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);
    void setCurrentFile(const QString &fileName1);
    QString curFile;




    QDialog* settings_window;
    QDialogButtonBox* dial_buttons;


    QVBoxLayout* algorithm();
    QRadioButton* egbs_radio;
    QRadioButton* kmean_radio;
    QRadioButton* gmm_radio;
    QDoubleSpinBox* sigma_spin;
    QSpinBox* threshold_spin;
    QSpinBox* min_component_size_spin;
    QSpinBox* kmean_spin;
    QSpinBox* gmm_spin;


    float sigma;
    float threshold;
    int min_component_size;
    int mean_k;
    int mean_gmm;
    int model;

    // Ok
    void apply_settings();
    // отмена
    void cancel_settings();


    //отклонение
    QDialog* evaluation_window;

    void open_imageA();
    void open_imageB();

    QString fileNameA;
    QString fileNameB;

    QScrollArea* scrollAreaA;
    PixmapWidget* imageLabelA;
    QScrollArea* scrollAreaB;
    PixmapWidget* imageLabelB;
    QSpinBox* scale_spinA;
    QSpinBox* scale_spinB;
    QSpinBox *sample_spin;
    QLabel* label_name1;
    QLabel* label_name2;
    QLabel* text_A;
    QLabel* text_B;
    QLabel* eval;
    QImage image1;
    QImage image2;
    int imgA_width;
    int imgA_height;
    int imgA_size;
    int imgB_width;
    int imgB_height;
    int imgB_size;

    QRadioButton *egbs_radio_button;
    QRadioButton *kmean_radio_button;
    QRadioButton *gmm_radio_button;

    int model_evaluation;

protected:

    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);

    void dragEnterEventA(QDragEnterEvent* event);
    void dragMoveEventA(QDragMoveEvent* event);
    void dropEventA(QDropEvent* event);
    void dragLeaveEventA(QDragLeaveEvent* event);

    void dragEnterEventB(QDragEnterEvent* event);
    void dragMoveEventB(QDragMoveEvent* event);
    void dropEventB(QDropEvent* event);
    void dragLeaveEventB(QDragLeaveEvent* event);



    QAction* openAct;
    QAction* separatorAct;
    enum { MaxRecentFiles = 5 };
    QAction* recentFileActs[MaxRecentFiles];
    QAction* deleteAct;
    QAction* saveAct;
    QAction* exitAct;
    QAction* zoomInAct;
    QAction* zoomOutAct;
    QAction* normalSizeAct;
    QAction* startAct;
    QAction* settingsAct;
    QAction* evaluateAct;

    QMenu* fileMenu;
    QMenu* viewMenu;
    QMenu* segmentationMenu;



signals :

    void changed(const QMimeData* mimeData = 0);
};

#endif // MAINWINDOW_H
