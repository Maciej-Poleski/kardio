#include <string>
#include <regex>
#include <vector>

#include <QtWidgets/QApplication>
#include <QtWidgets/QTableView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtGui/QImage>

#include "src/MasterModel.hxx"

int main(int argc, char **argv) {
    QApplication app(argc,argv);

    MasterModel *model=new MasterModel("/home/evil/projects/kardio/resources/date_time_zonespec.csv");
    model->loadFromFile("/home/evil/projects/kardio/resources/testdata.txt");

    QTableView *view=new QTableView;
    view->setModel(model);
    view->horizontalHeader()->setStretchLastSection(true);
    view->resizeColumnsToContents();
    view->setColumnWidth(0,view->columnWidth(0)+20);
    view->resize(400,400);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->show();
    view->scrollTo(model->index(model->rowCount()-1,0));

    QLabel *chartLabel=new QLabel();
    chartLabel->setPixmap(QPixmap::fromImage(model->getChartAsImage()));

    QScrollArea *chartWidget=new QScrollArea();
    chartWidget->setWidget(chartLabel);
    chartWidget->setMinimumHeight(chartLabel->sizeHint().height()+16); // TODO: do it better
    chartWidget->resize(1000,chartLabel->sizeHint().height()+16);
    chartWidget->show();

    app.exec();
    return 0;
}

