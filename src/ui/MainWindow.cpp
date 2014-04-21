/*! ---------------------------------------------------------------
 * \file   MainWindow.cpp
 * \brief  MainWindow class implementation
 * \author Edward Ryabikov
 *
 * PROJ: bestalloc
 * ---------------------------------------------------------------- */

#include "MainWindow.h"
#include "Constants.h"
using namespace bestalloc;

#include <QMenuBar>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>

#include "ConfigReader.h"

#include <iostream>
using namespace std;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_dataProvider(BestAllocAlgorithm()), m_graphWidget()
{
    initMenuBar();

    setCentralWidget(&m_graphWidget);
    setWindowTitle(BESTALLOC_TITLE);

    QDesktopWidget desktopWidget;

    int windowWidth = 850;
    int windowHeight = 600;

    int screenWidth = desktopWidget.screen()->width();
    int screenHeight = desktopWidget.screen()->height();

    setGeometry(screenWidth / 2 - windowWidth / 2,
                screenHeight / 2 - windowHeight / 2,
                windowWidth, windowHeight);
}

QMenu *MainWindow::createFileMenu(QWidget *parent, QWidget *target)
{
    QAction* saveAction = new QAction(NULL);
    saveAction->setText(SAVE_ACTION_MENU_LABEL);
    parent->connect(saveAction, SIGNAL(triggered()),target, SLOT(saveState()));

    QAction* loadAction = new QAction(NULL);
    loadAction->setText(LOAD_ACTION_MENU_LABEL);
    parent->connect(loadAction, SIGNAL(triggered()),target, SLOT(loadState()));

    QMenu* menuFile = new QMenu(FILE_MENU_LABEL);
    menuFile->addAction(saveAction);
    menuFile->addAction(loadAction);
    return menuFile;
}

QMenu *MainWindow::createEditMenu(QWidget *parent, QWidget *target)
{
    QAction* addDataAction = new QAction(NULL);
    addDataAction->setText(ADD_DATA_MENU_LABEL);
    parent->connect(addDataAction, SIGNAL(triggered()), target, SLOT(addNewNode()));

    QAction* changeObjectAction = new QAction(NULL);
    changeObjectAction->setText(CHANGE_OBJECT_MENU_LABEL);
    parent->connect(changeObjectAction, SIGNAL(triggered()), target, SLOT(changeObject()));

    QAction* deleteObjectAction = new QAction(NULL);
    deleteObjectAction->setText(DELETE_OBJECT_MENU_LABEL);
    parent->connect(deleteObjectAction, SIGNAL(triggered()), target, SLOT(deleteObject()));

    QMenu* menuEdit = new QMenu(EDIT_MENU_LABEL);
    menuEdit->addAction(addDataAction);
    menuEdit->addAction(changeObjectAction);
    menuEdit->addAction(deleteObjectAction);
    return menuEdit;
}

QMenu *MainWindow::createToolsMenu(QWidget *parent, QWidget *graph, QWidget *mainWindow)
{
    QAction* generateReportAction = new QAction(NULL);
    generateReportAction->setText(GENERATE_REPORT_ACTION_MENU_LABEL);
    parent->connect(generateReportAction, SIGNAL(triggered()), mainWindow, SLOT(generateReport()));

    QAction* computeAction = new QAction(NULL);
    computeAction->setText(COMPUTE_LABEL);
    parent->connect(computeAction, SIGNAL(triggered()), mainWindow, SLOT(compute()));

    QAction* clearSceneAction = new QAction(NULL);
    clearSceneAction->setText(CLEAR_SCENE_LABEL);
    parent->connect(clearSceneAction, SIGNAL(triggered()), graph, SLOT(clear()));

    QMenu* menuTools = new QMenu(TOOLS_MENU_LABEL);
    menuTools->addAction(computeAction);
    menuTools->addAction(clearSceneAction);
    menuTools->addAction(generateReportAction);

    return menuTools;
}

bool MainWindow::checkTaskInputData()
{
    foreach (EmployeeNode* node, m_graphWidget.getEmployeeNodes()) {
        if(node->getEdges().size()<=0){
            return false;
        }
    }

    foreach (SkillNode* node, m_graphWidget.getSkillNodes()) {
        if(node->getEdges().size()<=0){
            return false;
        }
    }
    return m_graphWidget.getEmployeeNodes().size()==m_graphWidget.getSkillNodes().size();
}

void MainWindow::initMenuBar()
{
    QMenu* menuFile = createFileMenu(this,this);

    QMenu* menuEdit = createEditMenu(this,&m_graphWidget);

    QMenu* menuTools = createToolsMenu(this,&m_graphWidget,this);

    menuBar()->addMenu(menuFile);
    menuBar()->addMenu(menuEdit);
    menuBar()->addMenu(menuTools);
}

void MainWindow::compute()
{
    m_dataProvider.clearContainers();

    if(!checkTaskInputData()){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Warning");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Can`t compute");
        msgBox.setInformativeText(
                    "1) The number of peaks in different parts should be the same; \n"
                    "2) There must not be connected vertices, each vertex must be connected with anything;"
                    );
        msgBox.exec();
        return;
    }

    foreach (EmployeeNode* node, m_graphWidget.getEmployeeNodes()) {
        m_dataProvider.addEmployee(*node);
    }

    foreach (SkillNode* node, m_graphWidget.getSkillNodes()) {
        m_dataProvider.addSkill(*node);
    }

    foreach (GraphEdge* edge, m_graphWidget.getEdges()) {
        m_dataProvider.assignSkill(edge->getSourceNode()->getId(), edge->getDestNode()->getId(), edge->getWeight());
    }

    vector< pair<Employee, Skill> > bestAllocMap = m_dataProvider.getBestAllocation();
    m_graphWidget.setBestAllocation(bestAllocMap);
}

void MainWindow::saveState()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save task to file"), "/home", tr("Best files (*.bst)"));
    if(fileName!=""){
        ConfigReader::save(fileName+".bst",m_dataProvider,m_graphWidget);
    }
}

void MainWindow::loadState()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Load task from file"), "/home", tr("Best files (*.bst)"));
    if(fileName!=""){
        ConfigReader::load(fileName,m_dataProvider,m_graphWidget);
    }
}

void MainWindow::generateReport()
{
    const QString xml_ext = "xml";
    const QString csv_ext = "csv";
    const QString text_ext = "txt";
    const QString point = ".";
    DataConverter *dataConverter;
    QString selectedFilter = "";
    QFileDialog filedialog;
    QString f_name = filedialog.getSaveFileName(this,
                                                tr("Generate report"),
                                                QDir::currentPath(),
                                                tr(TEXT_FILE_FILTER";;"CSV_FILE_FILTER";;"XML_FILE_FILTER),
                                                &selectedFilter);
    if(f_name != ""){
        QFileInfo fileInfo(f_name);
        if(fileInfo.suffix().isEmpty() || !((fileInfo.suffix() == text_ext && selectedFilter == TEXT_FILE_FILTER)||
                                            (fileInfo.suffix() == csv_ext && selectedFilter == CSV_FILE_FILTER)||
                                            (fileInfo.suffix() == xml_ext && selectedFilter == XML_FILE_FILTER) ))
        if(selectedFilter == TEXT_FILE_FILTER)
            f_name += point + text_ext;
        else if(selectedFilter == CSV_FILE_FILTER)
            f_name += point + csv_ext;
        else if(selectedFilter == XML_FILE_FILTER)
            f_name += point + xml_ext;
        qDebug() <<f_name;

        QFileInfo fileInfoChecked(f_name);

        if(fileInfoChecked.suffix() == text_ext)
            dataConverter = new TextDataConverter();
        else if(fileInfoChecked.suffix() == csv_ext)
            dataConverter = new CSVDataConverter();
        else if(fileInfoChecked.suffix() == xml_ext)
            dataConverter = new XMLDataConverter();

        ReportGenerator reportGenerator(f_name, dataConverter);
        reportGenerator.setEmployeeNodes(m_graphWidget.getEmployeeNodes());
        reportGenerator.setSkillNodes(m_graphWidget.getSkillNodes());
        reportGenerator.setEdges(m_graphWidget.getEdges());
        reportGenerator.setBestAllocMap(m_dataProvider.getBestAllocation());
        reportGenerator.generateReport();

        delete dataConverter;
    }
}

MainWindow::~MainWindow()
{
}
