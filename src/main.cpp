#include <fstream>
#include <iostream>
#include <vector>
#include <QApplication>
#include "MainWindow.h"
#include "Log.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    // setting stylesheet
    std::vector<char> mBuffer;
    std::ifstream ifs;
    ifs.open("default.css", std::ifstream::ate);
    if (!ifs.good()) {
        logError("Unable to open default.css");
        return 1;
    }
    auto fileSize = ifs.tellg();
    mBuffer.resize(fileSize);
    ifs.seekg(0);
    ifs.read(mBuffer.data(), fileSize);
    app.setStyleSheet(mBuffer.data());

    MainWindow wnd;
    wnd.show();
    return app.exec();
}