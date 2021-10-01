#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QtCore>

#include "medium4modelsdatatypes.h"

///[!] qml-geo I need just roles
#include "qml-geo-v2/basemapmarkersmodel.h"
#include "qml-geo-v2/mygeoconverthelper.h"

struct LastPageSettIcoList
{

    int icoCol;// = h.value("ico", -1).toInt();
    QStringList icoList;// = h.value("icos").toStringList();
    QStringList colData;

    bool enableCaching;
    LastPageSettIcoList() : icoCol(-1), enableCaching(true) {}
};


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
//    void setNewDeviceModel(QVariantList vl);
    void setTableData(MPrintTableOut table, int keycol);//first line is a header
    void setTableDataExt(const MPrintTableOut &table, const QStringList &header, const int &keycol);
    void setCoordinatorPosition(qreal x, qreal y, QString sn);


//    void showThisDeviceNI(QString ni); //deprecated

//    void showThisDeviceNIQuite(QString ni);



    void showThisDeviceKeyValue(QString keyvalue);

    void showThisDeviceKeyValueQuite(QString keyvalue);


    void centerMapHere(QVariant pos);

    ///from map
    void mapIsReady();//send devices positions

    void addDevice(QString pos);

    void showThisDev(QString ni);
    void showContextMenu4thisDev(QString ni);

    void showThisDevInSource(QString ni);

    void setThisFontSize(int pixelSize);

    void niActivated(QString ni);




    void moveDevice(QVariantHash h);

    void removeDevice(QString ni);

    void addDeviceStreet(QVariantHash h);

    void addDeviceStreetLine(QString line);

    void killMapSignal();



    void setModelHeaderDataRoles(QString columnroles);// list joined with '\n'


    //to basemarkermodel
    void setPredefinedDataImageFilteringSettings(QVariantMap predefinedfiltersettings);

    void setDefaultDataImageFilteringSettings(QVariantMap maponeprofile, QString profilename);


public slots:
    void showMap(QString lastLang);


    void addDeviceStreetSlot(QVariantHash h);


    void addDeviceStreetStr(QString simpletxt, QString pos);

    void moveDeviceStr(QString keyval, QString pos);

    void setNewDeviceModel(QVariantList vl);


//from basemarkermodel
    void gimmeDefaultDataImageFilteringSettings();



    //from parent
    //to basemarkermodel
    void setDefaultDataFilterSettings(QVariantMap maponeprofile, QString profilename);

    void setPredefinedDataFilterSettings(QVariantMap predefinedfiltersettings);

    void realoadFromTheDataSourceForced();


    void showThisCoordinate(QString c);






private slots:




    void on_pushButton_clicked();

private:
    Ui::MapWidget *ui;


    int calcFontPixelSize();

    struct ThisClassState
    {
        bool isQmlReady;


        bool isReadOnly;

        QString lastLang;

//data filter group
        QVariantMap mapdefaultgroupmethod;//group to image color
        QVariantMap predefinedgroupmethod;//group to image paths
        QString dataFilterSettingsName;

        ThisClassState() : isQmlReady(false), isReadOnly(true) {}
    } mstate;


};

#endif // MAPWIDGET_H
