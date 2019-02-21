#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QtCore>


namespace Ui {
class MapWidget;
}

class MapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapWidget(bool isReadOnly = false, const QString mapTitle = "", QWidget *parent = 0);
    ~MapWidget();

signals:

    /// to map
    void setNewDeviceModel(QVariantList vl);

    void showThisDeviceNI(QString ni);

    void showThisDeviceNIQuite(QString ni);

    void showThisCoordinates(QString c);

    ///from map
    void addDevice(QString pos);

    void showThisDev(QString ni);
    void showThisDevInSource(QString ni);

    void setThisFontSize(int pixelSize);

    void niActivated(QString ni);


    void updateModel4ls();


    void moveDevice(QVariantHash h);

    void removeDevice(QString ni);

    void addDeviceStreet(QVariantHash h);

    void addDeviceStreet(QString line);

    void killMapSignal();

public slots:
    void showMap(QString lastLang);


    void addDeviceStreetSlot(QVariantHash h);


private slots:




    void on_pushButton_clicked();

private:
    Ui::MapWidget *ui;


    int calcFontPixelSize();

    bool isQmlReady;


    bool isReadOnly;

    QString lastLang;
    QTemporaryDir tmpDir;
};

#endif // MAPWIDGET_H
