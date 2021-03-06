#ifndef GPSITEMSPREVIEW_H
#define GPSITEMSPREVIEW_H


///[!] widgets-shared
#include "gui-src/referencewidgetclass.h"


namespace Ui {
class GpsItemsPreview;
}

class GpsItemsPreview : public ReferenceWidgetClass
{
    Q_OBJECT

public:
    explicit GpsItemsPreview(GuiHelper *gHelper, QWidget *parent = nullptr);
    ~GpsItemsPreview();

    QStringList getHeader4meterType();

signals:

    ///map 4 meter list
    void showMap(QString lastLang);


    /// to map
    void setTableData(const MPrintTableOut &table, const int &keycol);//first line is a header
    void setTableDataExt(const MPrintTableOut &table, const QStringList &header, const int &keycol);
    void setCoordinatorPosition(qreal x, qreal y, QString sn);


    void setModelHeaderDataRoles(QString columnroles);// list joined with '\n'

    void showThisDeviceKeyValue(QString keyvalue);


    void onImportMetersFromFile();

public slots:
    void initPage();

    void clearPage();

    void setPageSett(const QVariantHash &h);

    ///from map

    void showThisDev(QString ni);

    void showContextMenu4thisDev(QString ni);

    void showThisDevInSource(QString ni);

    void moveDevice(QVariantHash h);

    void removeDevice(QString ni);

    void sayModelChanged();
    void sayModelChangedLater();

    void addMeters2list(bool replaceAll, QVariantHash meters);

    void onActivateMap();
private slots:

    void on_tbShowList_clicked();

    void on_tbShowMap_clicked();

    void onModelChanged();

    void on_tvTable_customContextMenuRequested(const QPoint &pos);



private:
    Ui::GpsItemsPreview *ui;
    bool mapIsReady;


    struct AddDataLater
    {
        bool replaceAll;
        QVariantHash meters;

        AddDataLater() {}
    } addLater;
};

#endif // GPSITEMSPREVIEW_H
