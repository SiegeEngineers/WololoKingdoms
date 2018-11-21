#ifndef WKGUI_H
#define WKGUI_H

#include <string>

class WKGui
{

public:
    virtual void log(std::string logMessage) = 0;
    virtual void setInfo(std::string info) = 0;
    virtual void createDialog(std::string info) = 0;
    virtual void createDialog(std::string info, std::string title) = 0;
    virtual void setProgress(int i) = 0;
    virtual void increaseProgress(int i) = 0;
    virtual std::string translate(std::string line) = 0;

};

#endif // WKGUI_H
