/* 
 * File:   ToolDataOut.cpp
 * Author: Bob
 * 
 * Created on September 30, 2020, 12:22 PM
 */

extern "C" 
{
#include "definitions.h"
}
#include <vector>
#include <string>
#include <algorithm>
#include "ToolDataOut.h"
#include "Menu.h"
#include "PPS.h"
#include "Display.h"
#include "DataOutPane.h"
#include "FileSystem.h"

#define ASCII_CHARS_NAME "ASCII characters"

extern const Menu uartOutBaud1;

static const MenuItem selectionItems[5] = {
    MenuItem(UTF8_UPARROW, MenuType::NoChange, nullptr, CB(&ToolDataOut::SelectionUp)), 
    MenuItem(UTF8_DOWNARROW, MenuType::NoChange, nullptr, CB(&ToolDataOut::SelectionDown)),
    MenuItem(),
    MenuItem("Cancel", MenuType::ParentMenu, nullptr, CB(&ToolDataOut::SelectionCancel)), 
    MenuItem("OK", MenuType::ParentMenu, nullptr, CB(&ToolDataOut::SelectionOK))};

static const Menu selectionMenu(selectionItems);

extern const Menu dataOutRunningMenu;

static const MenuItem resumeMenuItems[5] = {
    MenuItem("Resume", MenuType::SiblingMenu, &dataOutRunningMenu, CB(&ToolDataOut::Resume)),
    MenuItem("Stop", MenuType::ParentMenu, nullptr, CB(&ToolDataOut::Stop)),
    MenuItem(),
    MenuItem(),
    MenuItem()};

const Menu resumeMenu(resumeMenuItems);

static const MenuItem runningMenuItems[5] = {
    MenuItem("Pause", MenuType::SiblingMenu, &resumeMenu, CB(&ToolDataOut::Pause)),
    MenuItem("Stop", MenuType::ParentMenu, nullptr, CB(&ToolDataOut::Stop)),
    MenuItem(),
    MenuItem(),
    MenuItem()};

const Menu dataOutRunningMenu(runningMenuItems);

ToolDataOut::ToolDataOut(const char *title, const Menu &menu, const Help &help) :
    Tool(title, new DataOutPane, menu, help), _asciiChar(' '), _transmitting(Stopped),
    _file(SYS_FS_HANDLE_INVALID), _firstIdle(true), _mount(nullptr)
{
}

ToolDataOut::~ToolDataOut() 
{
    laWidget_SetVisible((laWidget *) ListBox, LA_FALSE);
    if (_file != SYS_FS_HANDLE_INVALID)
        SYS_FS_FileClose(_file);
    delete _mount;
}

void ToolDataOut::SelectData()
{
    laListWidget_RemoveAllItems(ListBox);
    uint32_t item = laListWidget_AppendItem(ListBox);
    uint32_t selectItem = item;
    laString str = laString_CreateFromCharBuffer(ASCII_CHARS_NAME, &MonoFont);
    laListWidget_SetItemText(ListBox, item, str);
    laString_Destroy(&str);
    
    MountDrive mount;
    SYS_FS_HANDLE dir = SYS_FS_DirOpen(mount.Name());
    SYS_FS_FSTAT stat;
    char dummy[10];
    stat.lfname = dummy;
    
    _fileNames.clear();
    while (SYS_FS_DirRead(dir, &stat) == SYS_FS_RES_SUCCESS && stat.fname[0]) 
    {
        if ((stat.fattrib & (SYS_FS_ATTR_HID | SYS_FS_ATTR_SYS | SYS_FS_ATTR_VOL | SYS_FS_ATTR_DIR)) == 0)
        {
            _fileNames.push_back(stat.fname);
        }
    }
    SYS_FS_DirClose(dir);
    std::sort(_fileNames.begin(), _fileNames.end(),
        [](const std::string &s1, const std::string &s2) 
        { 
           return strcasecmp(s1.c_str(), s2.c_str()) < 0;
        });
        
#ifdef __DEBUG
    if (_fileNames.empty())
    {
        char fakeName[] = "FakeFileName1";
        for (int i = 0; i < 10; ++i) 
        {
            _fileNames.push_back(fakeName);
            ++fakeName[sizeof(fakeName) - 2];
        }
    }
#endif
    
    for (auto &fileName : _fileNames)
    {
        item = laListWidget_AppendItem(ListBox);
        if (fileName == FileName())
            selectItem = item;
        str = laString_CreateFromCharBuffer(fileName.c_str(), &MonoFont);
        laListWidget_SetItemText(ListBox, item, str);
        laString_Destroy(&str);
    }
    laListWidget_SetItemSelected(ListBox, selectItem, LA_TRUE);
    laListWidget_SetItemVisible(ListBox, selectItem);
    
    laWidget_SetVisible((laWidget *) ListBox, LA_TRUE);
}

void ToolDataOut::SelectionUp()
{
    int32_t sel = (int32_t) laListWidget_GetFirstSelectedItem(ListBox);
    if (sel > 0)
    {
        laListWidget_SetItemSelected(ListBox, sel, LA_FALSE);
        laListWidget_SetItemSelected(ListBox, --sel, LA_TRUE);
        laListWidget_SetItemVisible(ListBox, sel);
    }
}

void ToolDataOut::SelectionDown()
{
    int32_t sel = (int32_t) laListWidget_GetFirstSelectedItem(ListBox);
    if (sel < _fileNames.size())
    {
        laListWidget_SetItemSelected(ListBox, sel, LA_FALSE);
        laListWidget_SetItemSelected(ListBox, ++sel, LA_TRUE);
        laListWidget_SetItemVisible(ListBox, sel);
    }
}

void ToolDataOut::SelectionCancel()
{
    laScrollBarWidget_SetScrollValue(ListBox->scrollbar, 0);
    laWidget_SetVisible((laWidget *) ListBox, LA_FALSE);
}

void ToolDataOut::SelectionOK()
{
    int32_t sel = (int32_t) laListWidget_GetFirstSelectedItem(ListBox);
    if (sel > 0)
        SetFileName(_fileNames[sel - 1].c_str());
    else
        SetFileName("");
    if (_file != SYS_FS_HANDLE_INVALID)
    {
        SYS_FS_FileClose(_file);
        _file = SYS_FS_HANDLE_INVALID;
        delete _mount;
        _mount = nullptr;
    }
    SelectionCancel();
    SetLabels();
}

void ToolDataOut::OnIdle()
{
    if (_firstIdle)
    {
        SetLabels();
        _firstIdle = false;
    }
    
    if (_transmitting == Sending)
    {
        // If we're transmitting a file
        if (FileName()[0])
        {
            // Try to open the file
            if (_file == SYS_FS_HANDLE_INVALID)
            {
                _mount = new MountDrive();
                _file = SYS_FS_FileOpen(FileName(), SYS_FS_FILE_OPEN_READ);
                // If we can't open it
                if (_file == SYS_FS_HANDLE_INVALID)
                {
                    // Clear the file setting and stop transmitting
                    SetFileName("");
                    _transmitting = Stopped;
                    return;
                }
            }
            
            // Send characters from the file
            while (_transmitting && TXReady())
            {
                char c;
                // If we successfully read a character
                if (SYS_FS_FileRead(_file, &c, 1) == 1)
                {
                    // Transmit it
                    TXData(c);
                    // Display it
                    GetPane()->AddText(&c, 1);
                }
                // Else (file error; most likely EOF)
                else
                {
                    SYS_FS_FileClose(_file);
                    _file = SYS_FS_HANDLE_INVALID;
                    delete _mount;
                    _mount = nullptr;
                    if (!_loop)
                        _transmitting = Stopped;
                }
            }
        }
        
        // Else (we're transmitting ASCII characters)
        else
        {
            while (_transmitting == Sending && TXReady())
            {
                TXData(_asciiChar);
                // Display it
                GetPane()->AddText(&_asciiChar, 1);
                
                if (++_asciiChar == 0x7f)
                {
                    _asciiChar = ' ';
                    if (!_loop)
                        _transmitting = Stopped;
                }
            }
        }
        
        // If we're no longer transmitting
        if (_transmitting == Stopped)
        {
            // Return from the "Pause/Stop" menu
            PopMenu();
            SetLabels();
        }
        
        GetPane()->Update();
    }
}

void ToolDataOut::ResetData()
{
    _asciiChar = ' ';
    if (_file != SYS_FS_HANDLE_INVALID)
    {
        SYS_FS_FileClose(_file);
        _file = SYS_FS_HANDLE_INVALID;
        delete _mount;
        _mount = nullptr;
    }
}

void ToolDataOut::SetLabels()
{
    GetPane()->SetDataName(*FileName() ? FileName() : ASCII_CHARS_NAME);
    char buf[30];
    switch (_transmitting)
    {
        case Stopped :
            strcpy(buf, "Stopped"); break;
        case Paused :
            strcpy(buf, "Paused"); break;
        case Sending :
            strcpy(buf, "Sending"); break;
    }
    if (_loop)
        strcat(buf, "; Looping");
    GetPane()->SetStatus(buf);
}
