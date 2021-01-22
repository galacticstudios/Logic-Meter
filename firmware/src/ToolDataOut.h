/* 
 * File:   ToolDataOut.h
 * Author: Bob
 *
 * Created on September 30, 2020, 12:22 PM
 */

#ifndef TOOLDATAOUT_H
#define	TOOLDATAOUT_H

#include "Tool.h"
#include "Utility.h"

class DataOutPane;
class MountDrive;

class ToolDataOut : public Tool
{
protected:
    ToolDataOut(const char *title, const Menu &menu, const Help &help);

public:
    virtual ~ToolDataOut();
    
    void OnIdle();
    
    void Start() {ResetData(); _loop = false; _transmitting = Sending; SetLabels();}
    void Loop() {ResetData(); _loop = true; _transmitting = Sending; SetLabels();}
    void Pause() {_transmitting = Paused; SetLabels();}
    void Resume() {_transmitting = Sending; SetLabels();}
    void Stop() {_transmitting = Stopped; _loop = false; ResetData(); SetLabels();}
    void SelectData();
    
    void SelectionUp();
    void SelectionDown();
    void SelectionCancel();
    void SelectionOK();
    
protected:
    virtual bool TXReady() const = 0;
    virtual void TXData(uint32_t data) = 0;
    virtual char *FileName() = 0;
    virtual void SetFileName(const char *fileName) = 0;

private:
    ToolDataOut(const ToolDataOut& orig);
    
    DataOutPane *GetPane() const {return (DataOutPane *) Tool::GetPane();}
    
    void SetLabels();
    
    void ResetData();
    
    enum {Stopped, Paused, Sending} _transmitting;
    bool _loop;
    char _asciiChar;
    bool _firstIdle;
    
    std::vector<std::string> _fileNames;
    SYS_FS_HANDLE _file;
    MountDrive *_mount;
};

extern const Menu dataOutRunningMenu;


#endif	/* TOOLDataOUT_H */

