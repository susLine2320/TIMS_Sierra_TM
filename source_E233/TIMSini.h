//
// Generated on 2023/02/15 by inimoni 
//

/*
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Documentation
  Struct = Section name, Field = Key name.
  The function used is only a declaration, load(), and save().
  The constructor automatically reads initialization file [initFileName].
  Please rewrite [initFileName] in the source freely.
  It becomes Windows folder when there is not path. The relative path can be used.

Example
#include "SampleIni.h"
void CTestDlg::OnButton1()
{
    //initFileName = "./sample.ini";
    SampleIni data1;                   // Open ./sample.ini
    int v = data1.Sample.value;        // [Sample]-value Get
    data1.Sample.value = v+1;          // [Sample]-value Modify
    data1.save();                      // Save ./Sample.ini (Opened file)

    SampleIni data2("sample.ini");     // Open C:/WINDOWS/Sample.ini
    data2.load("./sample.ini");        // Open Specified file.
    data2.save();                      // Save Opened file (Making at not being)
}

Supplementation
  Reading and writing becomes the unit of the file.
  [initFileName] is the same as the argument of GetPrivateProfileString() of SDK.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

#ifndef TIMS_NEW_TMINI_H
#define TIMS_NEW_TMINI_H

#include <string>
using namespace std;

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#ifdef _UNICODE
typedef std::wstring string_t;
#else
typedef std::string string_t;
#endif

namespace inimoni {
    template<class T>
    static bool inirw(int is_read, string_t& fname, string_t sec, string_t key, T& val_t);
};

/**
    TIMS_new_TM.ini input output class.
*/
class TIMS_new_TMIni
{
public:

    TIMS_new_TMIni(string_t fname = _T(""))
    {
        initFileName = _T("./TIMS_new_TM.ini"); //Default file.    
        init();
        load(fname);
    }

    /**
        INI file is read.
        @param fname Filename
        When there is not a file, It becomes initFileName.
        When there is not a pass, It becomes Windows folder.
    */
    bool load(string_t fname = _T(""))
    {
        if (fname.empty()) {
            fname = initFileName;
        }
        loadFileName = fname;
        WIN32_FIND_DATA fd;
        HANDLE h = ::FindFirstFile(fname.c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            iniRW(fname, 1);
        }
        return (h != INVALID_HANDLE_VALUE);
    }

    /**
        It writes it in the INI file.
        @param fname Filename
        When there is not a file, It becomes open file.
        When there is not a pass, It becomes Windows folder.
    */
    bool save(string_t fname = _T(""))
    {
        if (fname.empty()) {
            fname = loadFileName;
        }
        iniRW(fname, 0);
        return true;
    }

public:

    // Emulate
    struct _Emulate
    {
        int      ebCutPressure;           //ブレーキ減圧時間
        int      lbInit;                  //初回起動時力行遅延
    } Emulate;

    // Disp
    struct _Disp
    {
        int      UseJRTIMS;               //JR用TIMSの使用可否（0不使用、1使用、2JR線内のみ使用）
        int      UseUnit;                 //ユニット灯使用（小田急PIあれば不要）
        int      Line;                    //路線系統（9号、5号、13号）
        int      UseDistance;             //走行距離使用可否
    } Disp;

protected:

    string_t initFileName;
    string_t loadFileName;

    bool iniRW(string_t f, int r)
    {
        string_t s;

        s = _T("Emulate");
        inimoni::inirw(r, f, s, _T("ebCutPressure    "), Emulate.ebCutPressure);
        inimoni::inirw(r, f, s, _T("lbInit           "), Emulate.lbInit);

        s = _T("Disp");
        inimoni::inirw(r, f, s, _T("UseJRTIMS        "), Disp.UseJRTIMS);
        inimoni::inirw(r, f, s, _T("UseUnit          "), Disp.UseUnit);
        inimoni::inirw(r, f, s, _T("Line             "), Disp.Line);
        return true;
    }

    void init()
    {
        Emulate.ebCutPressure = 1000;
        Emulate.lbInit = 1;
        Disp.UseJRTIMS = 0;
        Disp.UseUnit = 0;
        Disp.UseDistance = 1;
        Disp.Line = 9;
    }
};
typedef TIMS_new_TMIni TIMS_new_TMFile; //新旧互換

//---------------------------------------------------------------------------
// Common method                                                             
//---------------------------------------------------------------------------
#ifndef INIMONI_INIRW                                                        
#define INIMONI_INIRW                                                        
namespace inimoni
{
    /*
    Read and Write INI file
      int     is_read  1=Read mode, 0=Write mode
      string  fname    Filename (The Windows folder when there is not path)
      string  sec      Section name
      string  key      Key name
      T       val_t    [Read]Init+Output, [Write]Input
    */
    template<class T>
    bool inirw(int is_read, string_t& fname, string_t sec, string_t key, T& val_t)
    {
        if (is_read) {
            inimoni::read(fname.c_str(), sec.c_str(), key.c_str(), val_t);
        }
        else {
            inimoni::write(fname.c_str(), sec.c_str(), key.c_str(), val_t);
        }
        return true;
    }

    bool read(string_t ifn, string_t sec, string_t key, int& dst)
    {
        dst = GetPrivateProfileInt(sec.c_str(), key.c_str(), dst, ifn.c_str());
        return true;
    }

    bool read(string_t ifn, string_t sec, string_t key, basic_string<TCHAR>& dst)
    {
        TCHAR buf[256];
        GetPrivateProfileString(
            sec.c_str(),
            key.c_str(),
            dst.c_str(),
            buf,
            sizeof(buf),
            ifn.c_str());
        dst = buf;
        return true;
    }

    bool read(string_t ifn, string_t sec, string_t key, double& dst)
    {
        string_t s;
        inimoni::read(ifn, sec, key, s);

        TCHAR* e;
        double x = _tcstod(s.c_str(), &e);

        dst = 0.0;
        if (!*e) {
            dst = x;
        }
        return true;
    }

    template<class T>
    bool write(string_t ifn, string_t sec, string_t key, T val_t)
    {
        TCHAR val[1024];
        inimoni::to_string(val, val_t);
        WritePrivateProfileString(sec.c_str(), key.c_str(), val, ifn.c_str());
        return true;
    }

    void to_string(TCHAR* str, int val)
    {
        _stprintf(str, _T("%d"), val);
    }

    void to_string(TCHAR* str, double val)
    {
        _stprintf(str, _T("%f"), val);
    }

    void to_string(TCHAR* str, basic_string<TCHAR> val)
    {
        _stprintf(str, _T("%s"), val.c_str());
    }
};

#endif                                                                  
#endif                                                                  

