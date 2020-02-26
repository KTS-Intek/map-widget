#include "gpsitemspreview.h"
#include "ui_gpsitemspreview.h"
#include <QShortcut>
#include <QMessageBox>


///[!] map-widget
#include "map-pgs/mapwidget.h"


///[!] guisett-shared-core
#include "src/nongui/settloader.h"
#include "src/nongui/showmesshelpercore.h"
#include "src/nongui/tableheaders.h"


#include "moji_defy.h"
#include "matildalimits.h"

GpsItemsPreview::GpsItemsPreview(GuiHelper *gHelper, QWidget *parent) :
    ReferenceWidgetClass(gHelper, parent),
    ui(new Ui::GpsItemsPreview)
{
    ui->setupUi(this);
}

GpsItemsPreview::~GpsItemsPreview()
{
    delete ui;
}

QStringList GpsItemsPreview::getHeader4meterType()
{
    QString s = TableHeaders::getColNamesItemsCoordinateList() ;

    return s.split(",");
}

void GpsItemsPreview::initPage()
{
    mapIsReady = false;
    setupObjects(ui->horizontalLayout_61, ui->tvTable, ui->tbFilter, ui->cbFilterMode, ui->leFilter, SETT_FILTERS_ITEMS_GPSPOS);
    connect(this, SIGNAL(openContextMenu(QPoint)), this, SLOT(on_tvTable_customContextMenuRequested(QPoint)));

    connectMessSignal();
    clearPage();

    connect(ui->pbClose, SIGNAL(clicked(bool)), this, SLOT(deleteLater()) );

    QShortcut *s = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(s, SIGNAL(activated()), this, SLOT(deleteLater())) ;
    connectModelSignalWithGuiHelper(CLBRD_ITMSGPS_LIST);

    if(!addLater.meters.isEmpty())
        addMeters2list(addLater.replaceAll, addLater.meters);
}

void GpsItemsPreview::clearPage()
{
    gHelper->hashUndoHistoryTv.remove(ui->tvTable);
//    ui->cbAddMeterModel->clear();
    ui->tbFilter->setEnabled(true);
    ui->leFilter->clear();

    ui->swMeterList->setCurrentIndex(0);
    ui->tbShowList->setChecked(true);
    ui->tbShowMap->setChecked(false);


    StandardItemModelHelper::setModelHorizontalHeaderItems(model , getHeader4meterType());

}

void GpsItemsPreview::setPageSett(const QVariantHash &h)
{

    StandardItemModelHelper::append2model(h, model);
    setHasDataFromRemoteDevice();
    emit resizeTv2content(ui->tvTable);
}

void GpsItemsPreview::showThisDev(QString ni)
{
    TableViewHelper::selectRowWithThisCell(lastTv, ni, 2);
}

void GpsItemsPreview::showContextMenu4thisDev(QString ni)
{
    showThisDev(ni);
    emit request2showContextMenuAnimated();
}

void GpsItemsPreview::showThisDevInSource(QString ni)
{
    showThisDev(ni);
    ui->tbShowList->animateClick();
}

void GpsItemsPreview::moveDevice(QVariantHash h)
{
    const int row = StandardItemModelHelper::getRowFromNI(2, h.value("ni").toString(), model);

    if(row < 0)
        return;

    model->item(row, 8)->setText(h.value("pos").toString());
    sayModelChanged();
}

void GpsItemsPreview::removeDevice(QString ni)
{
    const int row = StandardItemModelHelper::getRowFromNI(2, ni, model);

    if(row < 0)
        return;
    gHelper->deleteTheseRowsFromTv(QList<int>() << row , ui->tvTable, model);
    sayModelChanged();
}

void GpsItemsPreview::sayModelChanged()
{

}

void GpsItemsPreview::sayModelChangedLater()
{

}

void GpsItemsPreview::addMeters2list(bool replaceAll, QVariantHash meters)
{
    if(!getInitStarted()){
        addLater.replaceAll = replaceAll;
        addLater.meters = meters;
        return;
    }
    hasDataFromDevice = true;
    QMenu *menu = new QMenu(ui->tvTable);
    QAction *a = new QAction(menu);
    if(replaceAll || model->rowCount() < 1)
        StandardItemModelHelper::setModelHorizontalHeaderItems(model, getHeader4meterType());
    gHelper->hashData2table(meters, a);
    menu->deleteLater();
    sayModelChangedLater();

    addLater.meters.clear();
}

void GpsItemsPreview::onActivateMap()
{
    if(!ui->tbShowMap->isChecked())
        ui->tbShowMap->animateClick();
}

void GpsItemsPreview::on_tbShowList_clicked()
{
    ui->tbShowList->setChecked(true);
    ui->swMeterList->setCurrentIndex(0);
    ui->tbShowMap->setChecked(false);
}

void GpsItemsPreview::on_tbShowMap_clicked()
{
    ui->tbShowList->setChecked(false);
    ui->swMeterList->setCurrentIndex(1);
    ui->tbShowMap->setChecked(true);

    if(!mapIsReady){
        mapIsReady = true;

        QTimer *tmrModelChanged = new QTimer(this);
        tmrModelChanged->setInterval(555);
        tmrModelChanged->setSingleShot(true);
        proxy_model->enableModelChangedSignal();
        connect(proxy_model, SIGNAL(onModelChanged()), tmrModelChanged, SLOT(start()) );
        connect(tmrModelChanged, SIGNAL(timeout()), this, SLOT(onModelChanged()) );

        MapWidget *w = new MapWidget;
        ui->vlQmlMap->addWidget(w);

        connect(this, SIGNAL(showMap(QString))  , w, SLOT(showMap(QString)) );


        connect(this, &GpsItemsPreview::setCoordinatorPosition, w, &MapWidget::setCoordinatorPosition);
        connect(this, &GpsItemsPreview::setTableDataExt, w, &MapWidget::setTableDataExt);
        connect(this, &GpsItemsPreview::setModelHeaderDataRoles, w, &MapWidget::setModelHeaderDataRoles);


        connect(w, &MapWidget::mapIsReady, this, &GpsItemsPreview::onModelChanged);
        connect(this, &GpsItemsPreview::showThisDeviceKeyValue, w, &MapWidget::showThisDeviceKeyValue);


//        connect(editWdgt, SIGNAL(showThisDev(QString))       , w, SIGNAL(showThisCoordinate(QString))      );

        connect(w, SIGNAL(showThisDev(QString))         , this, SLOT(showThisDev(QString))          );
        connect(w, SIGNAL(showContextMenu4thisDev(QString)), this, SLOT(showContextMenu4thisDev(QString)));
        connect(w, SIGNAL(showThisDevInSource(QString)), this, SLOT(showThisDevInSource(QString)));

        connect(w, SIGNAL(removeDevice(QString))        , this, SLOT(removeDevice(QString))         );
        connect(w, SIGNAL(moveDevice(QVariantHash))     , this, SLOT(moveDevice(QVariantHash)) );
//        connect(w, SIGNAL(addDeviceStreet(QVariantHash)), editWdgt, SLOT(addDeviceStreet(QVariantHash)) );
//        connect(w, SIGNAL(moveDevice(QVariantHash))     , editWdgt, SLOT(moveDevice(QVariantHash)) );
//        connect(w, SIGNAL(addDevice(QString))           , editWdgt, SLOT(addDevice(QString))            );

//        connect(ui->tbShowMap, SIGNAL(toggled(bool)), w, SLOT(isParentWidgetVisible(bool)) );
    }

    emit showMap(gHelper->guiSett->currLang);
}

void GpsItemsPreview::onModelChanged()
{

    const QList<int> hiddenCols = QList<int>();
    const QList<int> rowsList = StandardItemModelHelper::getSourceRows(WidgetsHelper::getRowsListTo(proxy_model->rowCount()), proxy_model);//  ;
    int valCounter;///unused

    const QStringList headerlist = StandardItemModelHelper::modelHeaderListWithRowID(model, hiddenCols);
    QStringList heaaderroles;
    for(int i = 0, imax = headerlist.size(); i < imax; i++)
        heaaderroles.append(0);

    heaaderroles.replace(0, QString::number(BaseMapMarkersModel::itmmarkertxt));

    heaaderroles.replace(3, QString::number(BaseMapMarkersModel::itmkeyvaluetxt));//NI
    heaaderroles.replace(9, QString::number(BaseMapMarkersModel::coordinatevar));

//    const QStringList l4app = QString("ni pos pll img").split(" ");
//    const QList<int> l4appIndx = QList<int>() << 2 << 8 << 5 ; //-1

    emit setModelHeaderDataRoles(heaaderroles.join("\n"));
    const MPrintTableOut out = StandardItemModelHelper::getModelAsVector(model, proxy_model, hiddenCols, rowsList, true, valCounter);



    emit setTableDataExt(out, headerlist, 3);
    if(!lDevInfo->matildaDev.coordinatesIsDefault)//must be sent after table data
        emit setCoordinatorPosition(lDevInfo->matildaDev.coordinates.x(),lDevInfo->matildaDev.coordinates.y(), lDevInfo->matildaDev.lastSerialNumber );


//    if(row < 0)
//        return;
//    emit showThisDeviceKeyValue(model->item(row, 2)->text());
}

void GpsItemsPreview::on_tvTable_customContextMenuRequested(const QPoint &pos)
{
    gHelper->createCustomMenu(pos, ui->tvTable, (GuiHelper::ShowReset|GuiHelper::ShowExport|GuiHelper::ShowOnlyCopy), CLBRD_SMPL_PRXTBL, ShowMessHelperCore::matildaFileName(windowTitle()));

}
