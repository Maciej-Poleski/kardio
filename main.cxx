#include <string>
#include <regex>
#include <vector>

#include <QtWidgets/QApplication>
#include <QtWidgets/QTableView>
#include <QtWidgets/QHeaderView>

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

    app.exec();
    return 0;
}

