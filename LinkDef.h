//
// Created by Lukas Kreis on 12.06.17.
//
#ifdef __MAKECINT__

#include <vector>
#include <TAxis.h>

//#include <QnCorrectionsQnVector.h>
//#pragma link C++ class vector<QnCorrectionsQnVector>+;
//#pragma link C++ class QnAnalysisVector;
//#pragma link C++ class QnCorrelationMatrix;
#pragma link C++ class QnDataContainer<std::unique_ptr<QnCorrectionsQnVector> >+;
#pragma link C++ typedef QnDataContainerQn;

#endif