#ifndef ONTOLOGUI_H
#define ONTOLOGUI_H

#include <QMainWindow>
#include "include/ontoloGenius/graphical/ontoloGUI/QCheckBoxExtended.h"

#include "ros/ros.h"
#include <vector>
#include <string>

namespace Ui {
class ontoloGUI;
}

class ontoloGUI : public QMainWindow
{
    Q_OBJECT

public:
  explicit ontoloGUI(QWidget *parent = 0);
  ~ontoloGUI();

  void init(ros::NodeHandle* n) {n_ = n; }
  void wait();
  void start();
  void loadReasoners();

private:
  Ui::ontoloGUI *ui;
  ros::NodeHandle* n_;
  std::vector<std::string> reasoners_names_;
  std::vector<std::string> reasoners_description_;

  void displayUnClosed();
  void constructReasonersCheckBoxs();
  size_t getReasonerIndex(QCheckBoxExtended* box);
  std::string getReasonerDescription(std::string box);
  void displayOntologiesList();
  void displayErrorInfo(std::string text);

  std::string vector2string(std::vector<std::string> vect);
  std::string vector2html(std::vector<std::string> vect);

public slots:
  void ClasshoverEnterSlot();
  void ClasshoverLeaveSlot();
  void objectPropertyhoverEnterSlot();
  void objectPropertyhoverLeaveSlot();
  void dataPropertyhoverEnterSlot();
  void dataPropertyhoverLeaveSlot();
  void IndividualCheckBoxhoverEnterSlot();
  void IndividualhoverEnterSlot();
  void IndividualhoverLeaveSlot();

  void classClickedSlot();
  void objectPropertyClickedSlot();
  void dataPropertyClickedSlot();
  void individualClickedSlot();
  void closeOntologySlot();
  void nameEditingFinishedSlot();
  void ReasonerClickedSlot(int state);
  void ReasonerhoverEnterSlot();
  void ReasonerhoverLeaveSlot();
  void currentTabChangedSlot(int index);

  void displayOntologiesListSlot();
  void addOntologySlot();
  void deleteOntologySlot();
  void differenceOntologySlot();
  void OntologyNameAddDelChangedSlot(const QString&);
  void OntologyNameChangedSlot(const QString&);
};

#endif // ONTOLOGUI_H
