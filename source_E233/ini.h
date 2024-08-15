//
// Generated on 2023/04/23 by inimoni 
// Line-16�ɂ�钍�F���̃t�@�C����inimoni�ɂ�萧�삵�܂����B

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

#ifndef TIMS_SIERRAINI_H
#define TIMS_SIERRAINI_H

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
    TIMS_Sierra.ini input output class.
*/
class TIMS_SierraIni
{
public:

    TIMS_SierraIni(string_t fname = _T(""))
    {
        initFileName = _T("./TIMS_Sierra.ini"); //Default file.    
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

    // Spec
    struct _Spec
    {
        int      PilotLampIndex;          //�p�C���b�g�����v�C���f�b�N�X
    } Spec;

    // Disp
    struct _Disp
    {
        int      useLegacyDisp;           //�\������
        int      DispType;                //�^�]����ʂ�E/M
        int      EnableMeterDisp;         //���[�^�[�\���̗L����
        int      EnableTIMSDisp;          //TIMS�\���̗L����
        int      EnableVmeter;            //�d���v�\���̗L����
        int      UnitDispEnable;          //���j�b�g�\�����̗L����
        int      UseDistance;             //���s�����\��
        int      D01ABnum;                //�\������w��
        int      Row6;                    //6�i�ڂ̈����i�d�ԗp�̂݁j
        int      LVMeterindex;            //����d���v
        int      HVMeterindex;            //�ː��d���v
        int      NextStopType;            //��ԉw�_�Ŏd�l
        int      BCMRType;                //BC/MR���v�d�l
        int      AMType;                  //�d���v�d�l
        int      LineUpdate;              //1�s�X�V�̎���
        int      TIMSLag;                 //TIMS�\����̃��O
        int      SoundIdx;                //�����̃C���f�b�N�X
    } Disp;

    // Emulate
    struct _Emulate
    {
        int      ebCutPressure;           //�u���[�L��������
        int      lbInit;                  //����N�����͍s�x��
        int      KeyEvent;                //�}�X�R���L�[
    } Emulate;

protected:

    string_t initFileName;
    string_t loadFileName;

    bool iniRW(string_t f, int r)
    {
        string_t s;

        s = _T("Spec");
        inimoni::inirw(r, f, s, _T("PilotLampIndex   "), Spec.PilotLampIndex);

        s = _T("Disp");
        inimoni::inirw(r, f, s, _T("useLegacyDisp    "), Disp.useLegacyDisp);
        inimoni::inirw(r, f, s, _T("DispType         "), Disp.DispType);
        inimoni::inirw(r, f, s, _T("EnableMeterDisp  "), Disp.EnableMeterDisp);
        inimoni::inirw(r, f, s, _T("EnableTIMSDisp   "), Disp.EnableTIMSDisp);
        inimoni::inirw(r, f, s, _T("EnableVmeter     "), Disp.EnableVmeter);
        inimoni::inirw(r, f, s, _T("UnitDispEnable   "), Disp.UnitDispEnable);
        inimoni::inirw(r, f, s, _T("UseDistance      "), Disp.UseDistance);
        inimoni::inirw(r, f, s, _T("D01ABnum         "), Disp.D01ABnum);
        inimoni::inirw(r, f, s, _T("Row6             "), Disp.Row6);
        inimoni::inirw(r, f, s, _T("LVMeterindex     "), Disp.LVMeterindex);
        inimoni::inirw(r, f, s, _T("HVMeterindex     "), Disp.HVMeterindex);
        inimoni::inirw(r, f, s, _T("NextStopType     "), Disp.NextStopType);
        inimoni::inirw(r, f, s, _T("BCMRType         "), Disp.BCMRType);
        inimoni::inirw(r, f, s, _T("AMType           "), Disp.AMType);
        inimoni::inirw(r, f, s, _T("LineUpdate       "), Disp.LineUpdate);
        inimoni::inirw(r, f, s, _T("TIMSLag          "), Disp.TIMSLag);
        inimoni::inirw(r, f, s, _T("SoundIndex       "), Disp.SoundIdx);

        s = _T("Emulate");
        inimoni::inirw(r, f, s, _T("ebCutPressure    "), Emulate.ebCutPressure);
        inimoni::inirw(r, f, s, _T("lbInit           "), Emulate.lbInit);
        inimoni::inirw(r, f, s, _T("KeyEvent         "), Emulate.KeyEvent);
        return true;
    }

    void init()
    {
        Spec.PilotLampIndex = -1;
        Disp.useLegacyDisp = 0;
        Disp.DispType = 9;
        Disp.EnableMeterDisp = 1;
        Disp.EnableTIMSDisp = 1;
        Disp.EnableVmeter = 1;
        Disp.UnitDispEnable = 1;
        Disp.UseDistance = 1;
        Disp.D01ABnum = 5;
        Disp.Row6 = 0;
        Disp.LVMeterindex = 73;
        Disp.HVMeterindex = 75;
        Disp.NextStopType = 0;
        Disp.BCMRType = 0;
        Disp.AMType = 0;
        Disp.LineUpdate = 90;
        Disp.TIMSLag = 1200;
        Disp.SoundIdx = 63;
        Emulate.ebCutPressure = 1000;
        Emulate.lbInit = 0;
        Emulate.KeyEvent = 0;
    }
};
typedef TIMS_SierraIni TIMS_SierraFile; //�V���݊�

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

