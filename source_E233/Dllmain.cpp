// Ats.cpp : DLL アプリケーション用のエントリ ポイントを定義します。
// (c) Line-16 2023
// Umicorn様、kikuike様のソースコードを参考にさせていただいています。

#include "stdafx.h"
#include <stdlib.h>
#include "atsplugin.h"
#include "ini.h"
#include "main.h"
#include "tims9n.h"

tims9N g_9n; //うさプラ関連

#include "tims.h"
#include "Meter.h"
#include "dead.h"
#include "spp.h"
#include "sub.h"

TIMS g_tims; //TIMS表示器
Meter g_meter; //メーター表示器
SPP g_spp; //誤通過防止装置
Sub g_sub; //その他
DEAD g_dead; //電圧関係

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	//ファイルパス格納
	char filePath[_MAX_PATH + 1] = _T("");
	//検索文字列へのポインタ
	char* posIni;
	//INIファイルのロードに成功したかどうか
	bool loadCheck;
	//Ats.dllのファイルパスを取得
	::GetModuleFileName((HMODULE)hModule, filePath, _MAX_PATH);
	//パスから.dllの位置を検索
	posIni = strstr(filePath, ".dll");
	//.dllを.iniに置換
	memmove(posIni, ".ini", 4);
	// INIファイルをロードして結果を取得
	loadCheck = ini.load(filePath);

    return TRUE;
}

ATS_API void WINAPI Load(void) 
{
	g_tims.Load();
	g_meter.Load();
	g_sub.load();
	g_dead.load();
	snp2Beacon = 0;
}

ATS_API int WINAPI GetPluginVersion(void)
{
	return ATS_VERSION;
}

ATS_API void WINAPI SetVehicleSpec(ATS_VEHICLESPEC vehicleSpec)
{
	g_svcBrake = vehicleSpec.BrakeNotches;
	g_emgBrake = g_svcBrake + 1;

	g_spp.ServiceNotch = vehicleSpec.AtsNotch;

	//INI関連
	snp2Output = ini.Disp.useLegacyDisp;
	BCMRType = ini.Disp.BCMRType;
	AMType = ini.Disp.AMType;
	DispType = ini.Disp.DispType;
	D01ABnum = ini.Disp.D01ABnum >= 1 && ini.Disp.D01ABnum <= 6 ? ini.Disp.D01ABnum : 5;
	Row6 = ini.Disp.Row6;
	EMeter = ini.Disp.EnableMeterDisp;
	ETIMS = ini.Disp.EnableTIMSDisp;
	EVmeter = ini.Disp.EnableVmeter;
	EUD = ini.Disp.UnitDispEnable;
	Dist = ini.Disp.UseDistance;
	TIMSLag = abs(ini.Disp.TIMSLag);
	LineUpdate = ini.Disp.LineUpdate >= 0 && ini.Disp.LineUpdate < 250 ? ini.Disp.LineUpdate : 90;
	EbCut = ini.Emulate.ebCutPressure;
	g_sub.event_lbInit = ini.Emulate.lbInit;
}

ATS_API void WINAPI Initialize(int brake)
{
	g_speed = 0;
	g_spp.Initialize();
	g_9n.Init();
	g_tims.RefreshDir();
}

ATS_API ATS_HANDLES WINAPI Elapse(ATS_VEHICLESTATE vehicleState, int *panel, int *sound)
{
	g_deltaT = vehicleState.Time - g_time;
	g_time = vehicleState.Time;
	g_speed = vehicleState.Speed;

	g_tims.Location = vehicleState.Location;
	g_tims.BcPressure = vehicleState.BcPressure;
	g_tims.MrPressure = vehicleState.MrPressure;
	g_tims.Current = vehicleState.Current;
	g_meter.Location = vehicleState.Location;
	g_meter.BcPressure = vehicleState.BcPressure;
	g_meter.MrPressure = vehicleState.MrPressure;
	g_meter.Current = vehicleState.Current;
	g_sub.BcPressure = vehicleState.BcPressure;

	g_9n.McKey = panel[160] == 5 ? 6 : panel[160];
	g_9n.TrainType = panel[152];
	g_9n.SetArrivalSta(panel[172]);
	g_9n.SetDepartSta();

	g_tims.Execute(); //TIMS表示器
	g_9n.Execute();
	g_meter.Execute(); //メーター表示器
	g_dead.execute(); //電圧関連
	g_spp.Execute(); //誤通過防止装置
	g_sub.Execute(); //他機能


	// ハンドル出力
	if(g_sub.BcPressCut == 1)
		g_output.Brake = 1; //EB緩解時にブレーキを減圧する
	else
		g_output.Brake = g_brakeNotch;

	g_output.Reverser = g_reverser;

	if (g_time > g_sub.AccelCutting && g_dead.VCB_ON == 1)
		g_output.Power = g_powerNotch;
	else
		g_output.Power = 0;

	g_output.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;

	// パネル出力
	//メーター表示器
	if (snp2Output == 0) //9号千代田線系統・5号東西線系統
	{
		if (EMeter != 0) //メーター表示器を出力
		{
			// 電流計
			panel[186] = g_meter.AMMeterD[4]; //電流計[符号]
			if (AMType <= 1)
			{
				panel[187] = g_meter.AMMeterD[0]; //電流計[1000位]
				panel[188] = g_meter.AMMeterD[1]; //電流計[100位]
				panel[189] = g_meter.AMMeterD[2]; //電流計[10位]
				panel[190] = g_meter.AMMeterD[3]; //電流計[1位]
			}
			else if (AMType >= 2 && g_meter.AMMeterA[0] >= 0)
			{
				panel[187] = g_meter.AMMeterD[0] == 10 ? 20 : g_meter.AMMeterD[0]; //電流計[1000位]
				panel[188] = g_meter.AMMeterD[1] == 10 ? 20 : g_meter.AMMeterD[1]; //電流計[100位]
				panel[189] = g_meter.AMMeterD[2] == 10 ? 20 : g_meter.AMMeterD[2]; //電流計[10位]
				panel[190] = g_meter.AMMeterD[3] == 10 ? 20 : g_meter.AMMeterD[3]; //電流計[1位]
				if (g_meter.AMMeterA[1] < 1) { panel[190] = 21; }
			}
			else if(g_meter.AMMeterA[0] < 0)
			{
				panel[187] = g_meter.AMMeterD[0] + 10; //電流計[1000位]
				panel[188] = g_meter.AMMeterD[1] + 10; //電流計[100位]
				panel[189] = g_meter.AMMeterD[2] + 10; //電流計[10位]
				panel[190] = g_meter.AMMeterD[3] + 10; //電流計[1位]
				if (g_meter.AMMeterA[1] < 1) { panel[190] = 21; }
			}
			panel[199] = AMType == 1 || AMType == 3 ? g_meter.AMMeterA[1] : g_meter.AMMeterA[0]; //電流計[指針]
			panel[149] = g_meter.AMMeter[0]; //電流グラフ（+）
			panel[150] = g_meter.AMMeter[1]; //電流グラフ（-）
			//ブレーキシリンダ・元空気ダメ
			if (BCMRType == 0)
			{
				panel[85] = g_meter.BcCaution ? ((g_time % 1000) / 500) : 0; //200kPa警告
				panel[87] = g_meter.BCMeter[0]; //BCグラフ(0～180kPa）
				panel[88] = g_meter.BCMeter[1]; //BCグラフ(200～380kPa）
				panel[89] = g_meter.BCMeter[2]; //BCグラフ(400～580kPa）
				panel[90] = g_meter.BCMeter[3]; //BCグラフ(600～780kPa）
				panel[97] = g_meter.MRMeter[0]; //MRグラフ(750～795kPa）
				panel[98] = g_meter.MRMeter[1]; //MRグラフ(800～845kPa）
				panel[99] = g_meter.MRMeter[2]; //MRグラフ(850～895kPa）
			}
			else if (BCMRType == 2)
			{
				panel[85] = g_meter.BcCaution ? ((g_time % 1000) / 500) : 0; //200kPa警告
				panel[87] = g_meter.BCMeter[0]; //BCグラフ(0～180kPa）
				panel[88] = g_meter.BCMeter[1]; //BCグラフ(200～380kPa）
				panel[89] = g_meter.BCMeter[2]; //BCグラフ(400～580kPa）
				panel[90] = g_meter.BCMeter[3]; //BCグラフ(600～780kPa）
				panel[97] = g_meter.MRMeter[3]; //MRグラフ(700～790kPa）
				panel[98] = g_meter.MRMeter[4]; //MRグラフ(800～890kPa）
				panel[99] = g_meter.MRMeter[5]; //MRグラフ(900～990kPa）
			}
			// ブレーキシリンダ・元空気ダメ（デジタル表示）
			/*
			else*/
			{
				panel[183] = g_meter.BCPressD[0]; //ブレーキシリンダ[100位]
				panel[184] = g_meter.BCPressD[1]; //ブレーキシリンダ[10位]
				panel[185] = g_meter.BCPressD[2]; //ブレーキシリンダ[1位]
				panel[194] = g_meter.BCPress; //ブレーキシリンダ[指針]
				panel[196] = g_meter.MRPressD[0]; //元空気ダメ[100位]
				panel[197] = g_meter.MRPressD[1]; //元空気ダメ[10位]
				panel[198] = g_meter.MRPressD[2]; //元空気ダメ[1位]
				panel[195] = g_meter.MRPress; //元空気ダメ[指針]
			}
			// 速度計
			panel[207] = g_meter.SpeedD[0]; //速度計[100位]
			panel[208] = g_meter.SpeedD[1]; //速度計[10位]
			panel[209] = g_meter.SpeedD[2]; //速度計[1位]
			panel[200] = g_meter.Speed; //速度計[指針]
			// ブレーキ指令計
			panel[216] = g_meter.AccelDelay; //力行指令
			panel[215] = g_meter.BrakeDelay; //ブレーキ指令
			panel[80] = g_meter.BrakeDelay == g_emgBrake ? 1 : 0; //非常ブレーキ
		}
		if (EVmeter != 0) //電圧を出力
		{
			//電圧類
			//panel[310] = g_dead.AC; //交流
			panel[311] = g_dead.DC; //直流
			panel[312] = g_dead.CVacc; //制御電圧異常
			panel[313] = g_dead.CVacc10; //制御電圧[10位]
			panel[314] = g_dead.CVacc1; //制御電圧[1位]
			/*
			panel[315] = g_dead.ACacc; //交流電圧異常
			panel[316] = g_dead.ACacc10000; //交流電圧[10000位]
			panel[317] = g_dead.ACacc1000; //交流電圧[1000位]
			panel[318] = g_dead.ACacc100; //交流電圧[100位]
			*/
			panel[319] = g_dead.DCacc; //直流電圧異常
			panel[320] = g_dead.DCacc1000; //直流電圧[1000位]
			panel[321] = g_dead.DCacc100; //直流電圧[100位]
			panel[322] = g_dead.DCacc10; //直流電圧[10位]
			panel[323] = g_dead.Cvmeter; //制御指針
			//panel[324] = g_dead.Acmeter; //交流指針
			panel[325] = g_dead.Dcmeter; //直流指針
			panel[326] = g_dead.Accident; //事故
			panel[327] = g_dead.Tp; //三相
			panel[329] = g_dead.VCB; //VCB
			//panel[328] = g_dead.alert_ACDC > 0 ? g_dead.alert_ACDC + ((g_time % 800) / 400) : 0; //交直切換
		}

		//TIMS上部表示
			/*
			// 時計
			panel[37] = (g_time / 3600000) % 24; //デジタル時
			panel[38] = g_time / 60000 % 60; //デジタル分
			panel[39] = g_time / 1000 % 60; //デジタル秒
			*/
		// TIMS速度計
		panel[93] = g_tims.TimsSpeed[0]; //TIMS速度計[100位]
		panel[94] = g_tims.TimsSpeed[1]; //TIMS速度計[10位]
		panel[95] = g_tims.TimsSpeed[2]; //TIMS速度計[1位]
		// 走行距離
		if (Dist == 1)
		{
			panel[13] = g_tims.Distance[0]; //走行距離[km]
			panel[14] = g_tims.Distance[1]; //走行距離[100m]
			panel[15] = g_tims.Distance[2]; //走行距離[10m]
		}
		// 矢印
		panel[84] = g_tims.ArrowDirection; //進行方向矢印
		// ユニット表示灯
		if (EUD == 2)
		{
			panel[142] = g_tims.UnitState[0]; //ユニット表示灯1
			panel[143] = g_tims.UnitState[1]; //ユニット表示灯2
			panel[144] = g_tims.UnitState[2]; //ユニット表示灯3
			panel[145] = g_tims.UnitState[3]; //ユニット表示灯4			
		}
		else if (EUD == 1)
		{
			panel[142] = g_tims.UnitTims[0]; //TIMSユニット表示1
			panel[143] = g_tims.UnitTims[1]; //TIMSユニット表示2
			panel[144] = g_tims.UnitTims[2]; //TIMSユニット表示3
			panel[145] = g_tims.UnitTims[3]; //TIMSユニット表示4
		}

		if (ETIMS != 0)
		{
			//TIMS全般表示
			panel[301] = g_9n.Array; //行路表矢印
			panel[302] = g_9n.McKey != 6 ? 0 : g_tims.Kind; //列車種別
			//panel[303] = g_9n.McKey != 6 ? 0 : g_tims.Number[0]; //列車番号[1000位]
			//panel[304] = g_9n.McKey != 6 ? 0 : g_tims.Number[1]; //列車番号[100位]
			//panel[305] = g_9n.McKey != 6 ? 0 : g_tims.Number[2]; //列車番号[10位]
			//panel[306] = g_9n.McKey != 6 ? 0 : g_tims.Number[3]; //列車番号[1位]
			//
			panel[303] = g_9n.Location / 1000; //列車番号[1000位]
			panel[304] = (g_9n.Location % 1000) / 100; //列車番号[100位]
			panel[305] = (g_9n.Location % 100) / 10; //列車番号[10位]
			panel[306] = g_9n.Location % 10; //列車番号[1位]
			//
			panel[307] = g_9n.McKey != 6 ? 0 : g_tims.Charactor; //列車番号[記号]

			panel[308] = g_9n.McKey == 6 && g_tims.Number[3] != 0 ? 1 : 0; //設定完了
			panel[309] = g_9n.McKey != 6 ? 0 : g_tims.PassMode; //通過設定
			//うさプラpanel[119] = g_tims.NextBlinkLamp; //次駅停車表示灯
			panel[397] = g_9n.McKey == 6 ? 0 : g_9n.DepartSta; //運行パターン始発
			panel[398] = g_9n.McKey == 6 ? 0 : g_9n.ArrivalSta; //運行パターン行先
			panel[399] = g_9n.McKey != 6 ? 0 : g_tims.For; //列車行先
			//panel[209] = g_tims.This; //自駅（TIS用）
			//panel[210] = g_tims.Next; //次駅

			// スタフテーブル
			//電列共通
			panel[330] = g_tims.HiddenLine[0] || g_9n.HiddenLine[0] ? 0 : g_9n.McKey != 6 ? g_9n.DispSESta[0] : g_tims.Station[0]; //駅名表示1
			panel[331] = g_tims.HiddenLine[1] || g_9n.HiddenLine[1] ? 0 : g_9n.McKey != 6 ? g_9n.DispSESta[1] : g_tims.Station[1]; //駅名表示2
			panel[332] = g_tims.HiddenLine[2] || g_9n.HiddenLine[2] ? 0 : g_9n.McKey != 6 ? g_9n.DispSESta[2] : g_tims.Station[2]; //駅名表示3
			panel[333] = g_tims.HiddenLine[3] || g_9n.HiddenLine[3] ? 0 : g_9n.McKey != 6 ? g_9n.DispSESta[3] : g_tims.Station[3]; //駅名表示4
			panel[334] = g_tims.HiddenLine[4] || g_9n.HiddenLine[4] ? 0 : g_9n.McKey != 6 ? g_9n.DispSESta[4] : g_tims.Station[4]; //駅名表示5

			panel[335] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Arrive[0][0]; //到着時刻1H
			panel[336] = g_tims.HiddenLine[0]? 0 : g_9n.McKey == 6 ? g_tims.Arrive[0][1] : DispType != 5 ? 0 : g_9n.SetTrainPass(g_9n.DispSESta[0]); //到着時刻1M
			panel[337] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Arrive[0][2]; //到着時刻1S
			panel[338] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Arrive[1][0]; //到着時刻2H
			panel[339] = g_tims.HiddenLine[1]? 0 : g_9n.McKey == 6 ? g_tims.Arrive[1][1] : DispType != 5 ? 0 : g_9n.SetTrainPass(g_9n.DispSESta[1]); //到着時刻2M
			panel[340] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Arrive[1][2]; //到着時刻2S
			panel[341] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Arrive[2][0]; //到着時刻3H
			panel[342] = g_tims.HiddenLine[2]? 0 : g_9n.McKey == 6 ? g_tims.Arrive[2][1] : DispType != 5 ? 0 : g_9n.SetTrainPass(g_9n.DispSESta[2]); //到着時刻3M
			panel[343] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Arrive[2][2]; //到着時刻3S
			panel[344] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Arrive[3][0]; //到着時刻4H
			panel[345] = g_tims.HiddenLine[3]? 0 : g_9n.McKey == 6 ? g_tims.Arrive[3][1] : DispType != 5 ? 0 : g_9n.SetTrainPass(g_9n.DispSESta[3]); //到着時刻4M
			panel[346] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Arrive[3][2]; //到着時刻4S
			panel[347] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Arrive[4][0]; //到着時刻5H
			panel[348] = g_tims.HiddenLine[4]? 0 : g_9n.McKey == 6 ? g_tims.Arrive[4][1] : DispType != 5 ? 0 : g_9n.SetTrainPass(g_9n.DispSESta[4]); //到着時刻5M
			panel[349] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Arrive[4][2]; //到着時刻5S

			//panel[150] = g_tims.HiddenLine[0] ? 0 : g_tims.Leave[0][0]; //発車時刻1H
			panel[350] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Leave[0][1]; //発車時刻1M
			panel[351] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Leave[0][2]; //発車時刻1S
			//panel[153] = g_tims.HiddenLine[1] ? 0 : g_tims.Leave[1][0]; //発車時刻2H
			panel[352] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Leave[1][1]; //発車時刻2M
			panel[353] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Leave[1][2]; //発車時刻2S
			//panel[156] = g_tims.HiddenLine[2] ? 0 : g_tims.Leave[2][0]; //発車時刻3H
			panel[354] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Leave[2][1]; //発車時刻3M
			panel[355] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Leave[2][2]; //発車時刻3S
			//panel[159] = g_tims.HiddenLine[3] ? 0 : g_tims.Leave[3][0]; //発車時刻4H
			panel[356] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Leave[3][1]; //発車時刻4M
			panel[357] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Leave[3][2]; //発車時刻4S
			//panel[162] = g_tims.HiddenLine[4] ? 0 : g_tims.Leave[4][0]; //発車時刻5H
			panel[358] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Leave[4][1]; //発車時刻5M
			panel[359] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Leave[4][2]; //発車時刻5S

			panel[360] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Track[0]; //次駅番線1
			panel[361] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Track[1]; //次駅番線2
			panel[362] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Track[2]; //次駅番線3
			panel[363] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Track[3]; //次駅番線4
			panel[364] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Track[4]; //次駅番線5

			if (DispType == 1)
			{
				/*
				//列車スタフ
				panel[170] = g_tims.HiddenLine[5] ? 0 : g_tims.Station[5]; //駅名表示6
				panel[171] = g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][0]; //到着時刻6H
				panel[172] = g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][1]; //到着時刻6M
				panel[173] = g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][2]; //到着時刻6S
				panel[174] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][0]; //発車時刻6H
				panel[175] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][1]; //発車時刻6M
				panel[176] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][2]; //発車時刻6S
				panel[177] = g_tims.HiddenLine[5] ? 0 : g_tims.Track[5]; //次駅番線6

				panel[178] = g_tims.HiddenLine[0] ? 0 : g_tims.Span[0][0]; //駅間走行時間1M
				panel[179] = g_tims.HiddenLine[0] ? 0 : g_tims.Span[0][1]; //駅間走行時間1S
				panel[180] = g_tims.HiddenLine[1] ? 0 : g_tims.Span[1][0]; //駅間走行時間2M
				panel[181] = g_tims.HiddenLine[1] ? 0 : g_tims.Span[1][1]; //駅間走行時間2S
				panel[182] = g_tims.HiddenLine[2] ? 0 : g_tims.Span[2][0]; //駅間走行時間3M
				panel[183] = g_tims.HiddenLine[2] ? 0 : g_tims.Span[2][1]; //駅間走行時間3S
				panel[184] = g_tims.HiddenLine[3] ? 0 : g_tims.Span[3][0]; //駅間走行時間4M
				panel[185] = g_tims.HiddenLine[3] ? 0 : g_tims.Span[3][1]; //駅間走行時間4S
				panel[186] = g_tims.HiddenLine[4] ? 0 : g_tims.Span[4][0]; //駅間走行時間5M
				panel[187] = g_tims.HiddenLine[4] ? 0 : g_tims.Span[4][1]; //駅間走行時間5S

				panel[188] = g_tims.HiddenLine[0] ? 0 : g_tims.LimitA[0]; //制限速度1IN
				panel[189] = g_tims.HiddenLine[0] ? 0 : g_tims.LimitL[0]; //制限速度1OUT
				panel[190] = g_tims.HiddenLine[1] ? 0 : g_tims.LimitA[1]; //制限速度2IN
				panel[191] = g_tims.HiddenLine[1] ? 0 : g_tims.LimitL[1]; //制限速度2OUT
				panel[192] = g_tims.HiddenLine[2] ? 0 : g_tims.LimitA[2]; //制限速度3IN
				panel[193] = g_tims.HiddenLine[2] ? 0 : g_tims.LimitL[2]; //制限速度3OUT
				panel[194] = g_tims.HiddenLine[3] ? 0 : g_tims.LimitA[3]; //制限速度4IN
				panel[195] = g_tims.HiddenLine[3] ? 0 : g_tims.LimitL[3]; //制限速度4OUT
				panel[196] = g_tims.HiddenLine[4] ? 0 : g_tims.LimitA[4]; //制限速度5IN
				panel[197] = g_tims.HiddenLine[4] ? 0 : g_tims.LimitL[4]; //制限速度5OUT
				panel[198] = g_tims.HiddenLine[5] ? 0 : g_tims.LimitA[5]; //制限速度6IN
				panel[199] = g_tims.HiddenLine[5] ? 0 : g_tims.LimitL[5]; //制限速度6OUT
				*/
			}
			else
			{
				//電車スタフ
				if (Row6 != 1)
				{
					panel[365] = g_9n.McKey != 6 ? 0 : g_tims.After; //次採時駅
					panel[366] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeA[0]; //次採時駅着時刻H
					panel[367] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeA[1]; //次採時駅着時刻M
					panel[368] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeA[2]; //次採時駅着時刻S
					//panel[174] = g_tims.AfterTimeL[0]; //次採時駅発時刻H
					panel[369] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeL[1]; //次採時駅発時刻M
					panel[370] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeL[2]; //次採時駅発時刻S
					panel[371] = g_9n.McKey != 6 ? 0 : g_tims.AfterTrack; //次採時駅番線
				}

				else
				{
					panel[365] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Station[5]; //駅名表示6
					panel[366] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][0]; //到着時刻6H
					panel[367] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][1]; //到着時刻6M
					panel[368] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][2]; //到着時刻6S
					//panel[174] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][0]; //発車時刻6H
					panel[369] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][1]; //発車時刻6M
					panel[370] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][2]; //発車時刻6S
					panel[371] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Track[5]; //次駅番線6
				}

				panel[372] = g_9n.McKey != 6 ? 0 : g_tims.Before; //直前採時駅
				panel[373] = g_9n.McKey != 6 ? 0 : g_tims.BeforeTime[0]; //直前採時駅着時刻H
				panel[374] = g_9n.McKey != 6 ? 0 : g_tims.BeforeTime[1]; //直前採時駅着時刻M
				panel[375] = g_9n.McKey != 6 ? 0 : g_tims.BeforeTime[2]; //直前採時駅着時刻S
				panel[376] = g_9n.McKey != 6 ? 0 : g_tims.BeforeTrack; //直前採時駅番線

				panel[377] = g_9n.McKey != 6 ? 0 : g_tims.Last; //降車駅
				panel[378] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeA[0]; //降車駅着時刻H
				panel[379] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeA[1]; //降車駅着時刻M
				panel[380] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeA[2]; //降車駅着時刻S
				panel[381] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeL[0]; //降車駅発時刻H
				panel[382] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeL[1]; //降車駅発時刻M
				panel[383] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeL[2]; //降車駅発時刻S
				panel[384] = g_9n.McKey != 6 ? 0 : g_tims.LastTrack; //降車駅番線

				panel[385] = g_9n.McKey != 6 ? 0 : g_tims.AfterKind; //列車種別
				panel[386] = g_9n.McKey != 6 ? 0 : g_tims.AfterNumber[0]; //列車番号[1000位]
				panel[387] = g_9n.McKey != 6 ? 0 : g_tims.AfterNumber[1]; //列車番号[100位]
				panel[388] = g_9n.McKey != 6 ? 0 : g_tims.AfterNumber[2]; //列車番号[10位]
				panel[389] = g_9n.McKey != 6 ? 0 : g_tims.AfterNumber[3]; //列車番号[1位]
				panel[390] = g_9n.McKey != 6 ? 0 : g_tims.AfterChara; //列車番号[記号]

				panel[391] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[0][0]; //次行路着時刻H
				panel[392] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[0][1]; //次行路着時刻M
				panel[393] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[0][2]; //次行路着時刻S
				panel[394] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[1][0]; //次行路発時刻H
				panel[395] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[1][1]; //次行路発時刻M
				panel[396] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[1][2]; //次行路発時刻S
			}

			//次駅表示
			/*うさプラ
			panel[203] = g_tims.HiddenLine[D01ABnum] ? 0 : (g_tims.Next * g_tims.NextBlink); //駅名表示（次駅、点滅する）
			panel[204] = g_tims.HiddenLine[D01ABnum] ? 0 : g_tims.NextTime[0]; //次駅着時刻H
			panel[205] = g_tims.HiddenLine[D01ABnum] ? 0 : g_tims.NextTime[1]; //次駅着時刻M
			panel[206] = g_tims.HiddenLine[D01ABnum] ? 0 : g_tims.NextTime[2]; //次駅着時刻S
			panel[207] = g_tims.HiddenLine[D01ABnum] ? 0 : g_tims.NextTrack; //次駅着番線
			*/
		}
	}
	else if (snp2Output == 2) //TIMS_new_E互換機能
	{
		if (EMeter != 0) //メーター表示器を出力
		{
			// 電流計

			panel[239] = g_meter.AMMeterD[4]; //電流計[符号]
			panel[240] = g_meter.AMMeterD[0]; //電流計[1000位]
			panel[241] = g_meter.AMMeterD[1]; //電流計[100位]
			panel[242] = g_meter.AMMeterD[2]; //電流計[10位]
			panel[243] = g_meter.AMMeterD[3]; //電流計[1位]

			panel[244] = AMType == 1 ? g_meter.AMMeterA[1] : g_meter.AMMeterA[0]; //電流計[指針]
			panel[173] = g_meter.AMMeter[0]; //電流グラフ（+）
			panel[174] = g_meter.AMMeter[1]; //電流グラフ（-）
			//ブレーキシリンダ・元空気ダメ
			if (BCMRType == 0)
			{
				panel[78] = g_meter.BcCaution ? ((g_time % 1000) / 500) : 0; //200kPa警告
				panel[71] = g_meter.BCMeter[0]; //BCグラフ(0～180kPa）
				panel[72] = g_meter.BCMeter[1]; //BCグラフ(200～380kPa）
				panel[73] = g_meter.BCMeter[2]; //BCグラフ(400～580kPa）
				panel[74] = g_meter.BCMeter[3]; //BCグラフ(600～780kPa）
				panel[75] = g_meter.MRMeter[0]; //MRグラフ(750～795kPa）
				panel[76] = g_meter.MRMeter[1]; //MRグラフ(800～845kPa）
				panel[77] = g_meter.MRMeter[2]; //MRグラフ(850～895kPa）
				/*
			}
			else if (BCMRType == 2)
			{
				panel[122] = g_meter.BcCaution ? ((g_time % 1000) / 500) : 0; //200kPa警告
				panel[123] = g_meter.BCMeter[0]; //BCグラフ(0～180kPa）
				panel[124] = g_meter.BCMeter[1]; //BCグラフ(200～380kPa）
				panel[125] = g_meter.BCMeter[2]; //BCグラフ(400～580kPa）
				panel[126] = g_meter.BCMeter[3]; //BCグラフ(600～780kPa）
				panel[127] = g_meter.MRMeter[3]; //MRグラフ(700～790kPa）
				panel[128] = g_meter.MRMeter[4]; //MRグラフ(800～890kPa）
				panel[129] = g_meter.MRMeter[5]; //MRグラフ(900～990kPa）
				*/
			}
			// ブレーキシリンダ・元空気ダメ（デジタル表示）
			else
			{
				panel[71] = g_meter.BCPressD[0]; //ブレーキシリンダ[100位]
				panel[72] = g_meter.BCPressD[1]; //ブレーキシリンダ[10位]
				panel[73] = g_meter.BCPressD[2]; //ブレーキシリンダ[1位]
				panel[74] = g_meter.BCPress; //ブレーキシリンダ[指針]
				panel[75] = g_meter.MRPressD[0]; //元空気ダメ[100位]
				panel[76] = g_meter.MRPressD[1]; //元空気ダメ[10位]
				panel[77] = g_meter.MRPressD[2]; //元空気ダメ[1位]
				panel[78] = g_meter.MRPress; //元空気ダメ[指針]
			}
			// 速度計
			panel[67] = g_meter.SpeedD[0]; //速度計[100位]
			panel[68] = g_meter.SpeedD[1]; //速度計[10位]
			panel[69] = g_meter.SpeedD[2]; //速度計[1位]
			panel[70] = g_meter.Speed; //速度計[指針]
			// ブレーキ指令計
			panel[44] = g_meter.AccelDelay; //力行指令
			panel[26] = g_meter.BrakeDelay; //ブレーキ指令
			panel[27] = g_meter.BrakeDelay == g_emgBrake ? 1 : 0; //非常ブレーキ
		}
		if (EVmeter != 0) //電圧を出力
		{
			//電圧類
			//panel[217] = g_dead.AC; //交流
			panel[226] = g_dead.DC; //直流
			panel[227] = g_dead.CVacc; //制御電圧異常
			panel[228] = g_dead.CVacc10; //制御電圧[10位]
			panel[229] = g_dead.CVacc1; //制御電圧[1位]
			/*
			panel[222] = g_dead.ACacc; //交流電圧異常
			panel[223] = g_dead.ACacc10000; //交流電圧[10000位]
			panel[224] = g_dead.ACacc1000; //交流電圧[1000位]
			panel[225] = g_dead.ACacc100; //交流電圧[100位]
			*/
			panel[230] = g_dead.DCacc; //直流電圧異常
			panel[231] = g_dead.DCacc1000; //直流電圧[1000位]
			panel[232] = g_dead.DCacc100; //直流電圧[100位]
			panel[233] = g_dead.DCacc10; //直流電圧[10位]
			panel[234] = g_dead.Cvmeter; //制御指針
			//panel[231] = g_dead.Acmeter; //交流指針
			panel[235] = g_dead.Dcmeter; //直流指針
			panel[236] = g_dead.Accident; //事故
			panel[237] = g_dead.Tp; //三相
			panel[238] = g_dead.VCB; //VCB
			//panel[235] = g_dead.alert_ACDC > 0 ? g_dead.alert_ACDC + ((g_time % 800) / 400) : 0; //交直切換
		}

		//TIMS上部表示
			// 時計
			panel[47] = (g_time / 3600000) % 24; //デジタル時
			panel[48] = g_time / 60000 % 60; //デジタル分
			panel[49] = g_time / 1000 % 60; //デジタル秒
			// TIMS速度計
		panel[194] = g_tims.TimsSpeed[0]; //TIMS速度計[100位]
		panel[195] = g_tims.TimsSpeed[1]; //TIMS速度計[10位]
		panel[196] = g_tims.TimsSpeed[2]; //TIMS速度計[1位]
		// 走行距離
		panel[96] = g_tims.Distance[0]; //走行距離[km]
		panel[97] = g_tims.Distance[1]; //走行距離[100m]
		panel[98] = g_tims.Distance[2]; //走行距離[10m]
		// 矢印
		panel[45] = g_tims.ArrowDirection; //進行方向矢印
		// ユニット表示灯
		if (EUD == 2)
		{
			panel[146] = g_tims.UnitState[0]; //ユニット表示灯1
			panel[147] = g_tims.UnitState[1]; //ユニット表示灯2
			panel[148] = g_tims.UnitState[2]; //ユニット表示灯3
			panel[149] = g_tims.UnitState[3]; //ユニット表示灯4			
		}
		else if (EUD == 1)
		{
			panel[146] = g_tims.UnitTims[0]; //TIMSユニット表示1
			panel[147] = g_tims.UnitTims[1]; //TIMSユニット表示2
			panel[148] = g_tims.UnitTims[2]; //TIMSユニット表示3
			panel[149] = g_tims.UnitTims[3]; //TIMSユニット表示4
		}
		if (ETIMS != 0)
		{
			//TIMS全般表示
			panel[46] = g_tims.TrainArrow; //行路表矢印
			panel[124] = g_tims.Kind; //列車種別
			panel[125] = g_tims.Number[0]; //列車番号[1000位]
			panel[126] = g_tims.Number[1]; //列車番号[100位]
			panel[127] = g_tims.Number[2]; //列車番号[10位]
			panel[128] = g_tims.Number[3]; //列車番号[1位]
			panel[129] = g_tims.Charactor; //列車番号[記号]

			panel[136] = g_tims.Number[3] != 0 ? 1 : 0; //設定完了
			panel[137] = g_tims.PassMode; //通過設定
			//うさプラpanel[119] = g_tims.NextBlinkLamp; //次駅停車表示灯
			panel[138] = g_tims.From; //運行パターン始発
			panel[139] = g_tims.Destination; //運行パターン行先
			panel[82] = g_tims.For; //列車行先
			//panel[209] = g_tims.This; //自駅（TIS用）
			//panel[210] = g_tims.Next; //次駅

			// スタフテーブル
			//電列共通
			panel[60] = g_tims.HiddenLine[0] ? 0 : g_tims.Station[0]; //駅名表示1
			panel[61] = g_tims.HiddenLine[1] ? 0 : g_tims.Station[1]; //駅名表示2
			panel[62] = g_tims.HiddenLine[2] ? 0 : g_tims.Station[2]; //駅名表示3
			panel[63] = g_tims.HiddenLine[3] ? 0 : g_tims.Station[3]; //駅名表示4
			panel[64] = g_tims.HiddenLine[4] ? 0 : g_tims.Station[4]; //駅名表示5
			/*
			panel[147] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Arrive[0][0]; //到着時刻1H
			panel[4] = DispType != 5 && g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Arrive[0][1]; //到着時刻1M
			panel[98] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Arrive[0][2]; //到着時刻1S
			panel[148] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Arrive[1][0]; //到着時刻2H
			panel[10] = DispType != 5 && g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Arrive[1][1]; //到着時刻2M
			panel[99] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Arrive[1][2]; //到着時刻2S
			panel[127] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Arrive[2][0]; //到着時刻3H
			panel[12] = DispType != 5 && g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Arrive[2][1]; //到着時刻3M
			panel[53] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Arrive[2][2]; //到着時刻3S
			panel[128] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Arrive[3][0]; //到着時刻4H
			panel[13] = DispType != 5 && g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Arrive[3][1]; //到着時刻4M
			panel[54] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Arrive[3][2]; //到着時刻4S
			panel[162] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Arrive[4][0]; //到着時刻5H
			panel[16] = DispType != 5 && g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Arrive[4][1]; //到着時刻5M
			panel[57] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Arrive[4][2]; //到着時刻5S

			//panel[150] = g_tims.HiddenLine[0] ? 0 : g_tims.Leave[0][0]; //発車時刻1H
			panel[230] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Leave[0][1]; //発車時刻1M
			panel[231] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Leave[0][2]; //発車時刻1S
			//panel[153] = g_tims.HiddenLine[1] ? 0 : g_tims.Leave[1][0]; //発車時刻2H
			panel[233] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Leave[1][1]; //発車時刻2M
			panel[234] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Leave[1][2]; //発車時刻2S
			//panel[156] = g_tims.HiddenLine[2] ? 0 : g_tims.Leave[2][0]; //発車時刻3H
			panel[198] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Leave[2][1]; //発車時刻3M
			panel[199] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Leave[2][2]; //発車時刻3S
			//panel[159] = g_tims.HiddenLine[3] ? 0 : g_tims.Leave[3][0]; //発車時刻4H
			panel[200] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Leave[3][1]; //発車時刻4M
			panel[201] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Leave[3][2]; //発車時刻4S
			//panel[162] = g_tims.HiddenLine[4] ? 0 : g_tims.Leave[4][0]; //発車時刻5H
			panel[202] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Leave[4][1]; //発車時刻5M
			panel[203] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Leave[4][2]; //発車時刻5S

			panel[204] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Track[0]; //次駅番線1
			panel[205] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Track[1]; //次駅番線2
			panel[206] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Track[2]; //次駅番線3
			panel[208] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Track[3]; //次駅番線4
			panel[209] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Track[4]; //次駅番線5

			if (DispType == 1)
			{
				/*
				//列車スタフ
				panel[170] = g_tims.HiddenLine[5] ? 0 : g_tims.Station[5]; //駅名表示6
				panel[171] = g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][0]; //到着時刻6H
				panel[172] = g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][1]; //到着時刻6M
				panel[173] = g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][2]; //到着時刻6S
				panel[174] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][0]; //発車時刻6H
				panel[175] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][1]; //発車時刻6M
				panel[176] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][2]; //発車時刻6S
				panel[177] = g_tims.HiddenLine[5] ? 0 : g_tims.Track[5]; //次駅番線6

				panel[178] = g_tims.HiddenLine[0] ? 0 : g_tims.Span[0][0]; //駅間走行時間1M
				panel[179] = g_tims.HiddenLine[0] ? 0 : g_tims.Span[0][1]; //駅間走行時間1S
				panel[180] = g_tims.HiddenLine[1] ? 0 : g_tims.Span[1][0]; //駅間走行時間2M
				panel[181] = g_tims.HiddenLine[1] ? 0 : g_tims.Span[1][1]; //駅間走行時間2S
				panel[182] = g_tims.HiddenLine[2] ? 0 : g_tims.Span[2][0]; //駅間走行時間3M
				panel[183] = g_tims.HiddenLine[2] ? 0 : g_tims.Span[2][1]; //駅間走行時間3S
				panel[184] = g_tims.HiddenLine[3] ? 0 : g_tims.Span[3][0]; //駅間走行時間4M
				panel[185] = g_tims.HiddenLine[3] ? 0 : g_tims.Span[3][1]; //駅間走行時間4S
				panel[186] = g_tims.HiddenLine[4] ? 0 : g_tims.Span[4][0]; //駅間走行時間5M
				panel[187] = g_tims.HiddenLine[4] ? 0 : g_tims.Span[4][1]; //駅間走行時間5S

				panel[188] = g_tims.HiddenLine[0] ? 0 : g_tims.LimitA[0]; //制限速度1IN
				panel[189] = g_tims.HiddenLine[0] ? 0 : g_tims.LimitL[0]; //制限速度1OUT
				panel[190] = g_tims.HiddenLine[1] ? 0 : g_tims.LimitA[1]; //制限速度2IN
				panel[191] = g_tims.HiddenLine[1] ? 0 : g_tims.LimitL[1]; //制限速度2OUT
				panel[192] = g_tims.HiddenLine[2] ? 0 : g_tims.LimitA[2]; //制限速度3IN
				panel[193] = g_tims.HiddenLine[2] ? 0 : g_tims.LimitL[2]; //制限速度3OUT
				panel[194] = g_tims.HiddenLine[3] ? 0 : g_tims.LimitA[3]; //制限速度4IN
				panel[195] = g_tims.HiddenLine[3] ? 0 : g_tims.LimitL[3]; //制限速度4OUT
				panel[196] = g_tims.HiddenLine[4] ? 0 : g_tims.LimitA[4]; //制限速度5IN
				panel[197] = g_tims.HiddenLine[4] ? 0 : g_tims.LimitL[4]; //制限速度5OUT
				panel[198] = g_tims.HiddenLine[5] ? 0 : g_tims.LimitA[5]; //制限速度6IN
				panel[199] = g_tims.HiddenLine[5] ? 0 : g_tims.LimitL[5]; //制限速度6OUT
				
			}
			else
			{
				//電車スタフ
				if (Row6 != 1)
				{
					panel[163] = g_9n.McKey != 6 ? 0 : g_tims.After; //次採時駅
					panel[164] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeA[0]; //次採時駅着時刻H
					panel[167] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeA[1]; //次採時駅着時刻M
					panel[168] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeA[2]; //次採時駅着時刻S
					//panel[174] = g_tims.AfterTimeL[0]; //次採時駅発時刻H
					panel[169] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeL[1]; //次採時駅発時刻M
					panel[170] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeL[2]; //次採時駅発時刻S
					panel[171] = g_9n.McKey != 6 ? 0 : g_tims.AfterTrack; //次採時駅番線
				}

				else
				{
					panel[163] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Station[5]; //駅名表示6
					panel[164] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][0]; //到着時刻6H
					panel[167] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][1]; //到着時刻6M
					panel[168] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][2]; //到着時刻6S
					//panel[174] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][0]; //発車時刻6H
					panel[169] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][1]; //発車時刻6M
					panel[170] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][2]; //発車時刻6S
					panel[171] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Track[5]; //次駅番線6
				}

				panel[177] = g_9n.McKey != 6 ? 0 : g_tims.Before; //直前採時駅
				panel[146] = g_9n.McKey != 6 ? 0 : g_tims.BeforeTime[0]; //直前採時駅着時刻H
				panel[174] = g_9n.McKey != 6 ? 0 : g_tims.BeforeTime[1]; //直前採時駅着時刻M
				panel[175] = g_9n.McKey != 6 ? 0 : g_tims.BeforeTime[2]; //直前採時駅着時刻S
				panel[180] = g_9n.McKey != 6 ? 0 : g_tims.BeforeTrack; //直前採時駅番線

				panel[210] = g_9n.McKey != 6 ? 0 : g_tims.Last; //降車駅
				//panel[234] = g_9n.McKey == 6 ? 0 : g_tims.Last; //降車駅
				panel[211] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeA[0]; //降車駅着時刻H
				panel[212] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeA[1]; //降車駅着時刻M
				panel[213] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeA[2]; //降車駅着時刻S
				panel[214] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeL[0]; //降車駅発時刻H
				panel[215] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeL[1]; //降車駅発時刻M
				panel[216] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeL[2]; //降車駅発時刻S
				panel[217] = g_9n.McKey != 6 ? 0 : g_tims.LastTrack; //降車駅番線

				panel[218] = g_9n.McKey != 6 ? 0 : g_tims.AfterKind; //列車種別
				panel[219] = g_9n.McKey != 6 ? 0 : g_tims.AfterNumber[0]; //列車番号[1000位]
				panel[220] = g_9n.McKey != 6 ? 0 : g_tims.AfterNumber[1]; //列車番号[100位]
				panel[221] = g_9n.McKey != 6 ? 0 : g_tims.AfterNumber[2]; //列車番号[10位]
				panel[222] = g_9n.McKey != 6 ? 0 : g_tims.AfterNumber[3]; //列車番号[1位]
				panel[223] = g_9n.McKey != 6 ? 0 : g_tims.AfterChara; //列車番号[記号]

				panel[224] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[0][0]; //次行路着時刻H
				panel[225] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[0][1]; //次行路着時刻M
				panel[226] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[0][2]; //次行路着時刻S
				panel[227] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[1][0]; //次行路発時刻H
				panel[228] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[1][1]; //次行路発時刻M
				panel[229] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[1][2]; //次行路発時刻S
			}
			*/
			//次駅表示
			//うさプラ
			panel[245] = g_tims.HiddenLine[D01ABnum] ? 0 : (g_tims.Next * g_tims.NextBlink); //駅名表示（次駅、点滅する）
			panel[246] = g_tims.HiddenLine[D01ABnum] ? 0 : g_tims.NextTime[0]; //次駅着時刻H
			panel[247] = g_tims.HiddenLine[D01ABnum] ? 0 : g_tims.NextTime[1]; //次駅着時刻M
			panel[248] = g_tims.HiddenLine[D01ABnum] ? 0 : g_tims.NextTime[2]; //次駅着時刻S
			panel[249] = g_tims.HiddenLine[D01ABnum] ? 0 : g_tims.NextTrack; //次駅着番線
			
		}
	}
	else //TIMS_new_9互換機能
	{
		if (EMeter != 0) //メーター表示器を出力
		{
			// 電流計

			panel[226] = g_meter.AMMeterD[4]; //電流計[符号]
			panel[227] = g_meter.AMMeterD[0]; //電流計[1000位]
			panel[228] = g_meter.AMMeterD[1]; //電流計[100位]
			panel[229] = g_meter.AMMeterD[2]; //電流計[10位]
			panel[230] = g_meter.AMMeterD[3]; //電流計[1位]

			panel[231] = AMType == 1 ? g_meter.AMMeterA[1] : g_meter.AMMeterA[0]; //電流計[指針]
			panel[149] = g_meter.AMMeter[0]; //電流グラフ（+）
			panel[150] = g_meter.AMMeter[1]; //電流グラフ（-）
			//ブレーキシリンダ・元空気ダメ
			if (BCMRType == 0)
			{
				panel[100] = g_meter.BcCaution ? ((g_time % 1000) / 500) : 0; //200kPa警告
				panel[87] = g_meter.BCMeter[0]; //BCグラフ(0～180kPa）
				panel[88] = g_meter.BCMeter[1]; //BCグラフ(200～380kPa）
				panel[89] = g_meter.BCMeter[2]; //BCグラフ(400～580kPa）
				panel[90] = g_meter.BCMeter[3]; //BCグラフ(600～780kPa）
				panel[97] = g_meter.MRMeter[0]; //MRグラフ(750～795kPa）
				panel[98] = g_meter.MRMeter[1]; //MRグラフ(800～845kPa）
				panel[99] = g_meter.MRMeter[2]; //MRグラフ(850～895kPa）
				/*
			}
			else if (BCMRType == 2)
			{
				panel[122] = g_meter.BcCaution ? ((g_time % 1000) / 500) : 0; //200kPa警告
				panel[123] = g_meter.BCMeter[0]; //BCグラフ(0～180kPa）
				panel[124] = g_meter.BCMeter[1]; //BCグラフ(200～380kPa）
				panel[125] = g_meter.BCMeter[2]; //BCグラフ(400～580kPa）
				panel[126] = g_meter.BCMeter[3]; //BCグラフ(600～780kPa）
				panel[127] = g_meter.MRMeter[3]; //MRグラフ(700～790kPa）
				panel[128] = g_meter.MRMeter[4]; //MRグラフ(800～890kPa）
				panel[129] = g_meter.MRMeter[5]; //MRグラフ(900～990kPa）
				*/
			}
			// ブレーキシリンダ・元空気ダメ（デジタル表示）
			else
			{
				panel[87] = g_meter.BCPressD[0]; //ブレーキシリンダ[100位]
				panel[88] = g_meter.BCPressD[1]; //ブレーキシリンダ[10位]
				panel[89] = g_meter.BCPressD[2]; //ブレーキシリンダ[1位]
				panel[90] = g_meter.BCPress; //ブレーキシリンダ[指針]
				panel[97] = g_meter.MRPressD[0]; //元空気ダメ[100位]
				panel[98] = g_meter.MRPressD[1]; //元空気ダメ[10位]
				panel[99] = g_meter.MRPressD[2]; //元空気ダメ[1位]
				panel[100] = g_meter.MRPress; //元空気ダメ[指針]
			}
			// 速度計
			panel[197] = g_meter.SpeedD[0]; //速度計[100位]
			panel[198] = g_meter.SpeedD[1]; //速度計[10位]
			panel[199] = g_meter.SpeedD[2]; //速度計[1位]
			panel[200] = g_meter.Speed; //速度計[指針]
			// ブレーキ指令計
			panel[80] = g_meter.BrakeDelay == g_emgBrake ? 1 : 0; //非常ブレーキ
			panel[67] = g_meter.BrakeDelay; //ブレーキ指令
			panel[71] = g_meter.AccelDelay; //力行指令
		}
		if (EVmeter != 0) //電圧を出力
		{
			//電圧類
			//panel[217] = g_dead.AC; //交流
			panel[221] = g_dead.DC; //直流
			panel[207] = g_dead.CVacc; //制御電圧異常
			panel[208] = g_dead.CVacc10; //制御電圧[10位]
			panel[209] = g_dead.CVacc1; //制御電圧[1位]
			/*
			panel[222] = g_dead.ACacc; //交流電圧異常
			panel[223] = g_dead.ACacc10000; //交流電圧[10000位]
			panel[224] = g_dead.ACacc1000; //交流電圧[1000位]
			panel[225] = g_dead.ACacc100; //交流電圧[100位]
			*/
			panel[222] = g_dead.DCacc; //直流電圧異常
			panel[223] = g_dead.DCacc1000; //直流電圧[1000位]
			panel[224] = g_dead.DCacc100; //直流電圧[100位]
			panel[225] = g_dead.DCacc10; //直流電圧[10位]
			panel[233] = g_dead.Cvmeter; //制御指針
			//panel[231] = g_dead.Acmeter; //交流指針
			panel[234] = g_dead.Dcmeter; //直流指針
			panel[242] = g_dead.Accident; //事故
			panel[243] = g_dead.Tp; //三相
			panel[244] = g_dead.VCB; //VCB
			//panel[235] = g_dead.alert_ACDC > 0 ? g_dead.alert_ACDC + ((g_time % 800) / 400) : 0; //交直切換
		}

		//TIMS上部表示
			/*
			// 時計
			panel[37] = (g_time / 3600000) % 24; //デジタル時
			panel[38] = g_time / 60000 % 60; //デジタル分
			panel[39] = g_time / 1000 % 60; //デジタル秒
			*/
			// TIMS速度計
		panel[194] = g_tims.TimsSpeed[0]; //TIMS速度計[100位]
		panel[195] = g_tims.TimsSpeed[1]; //TIMS速度計[10位]
		panel[196] = g_tims.TimsSpeed[2]; //TIMS速度計[1位]
		// 走行距離
		panel[13] = g_tims.Distance[0]; //走行距離[km]
		panel[14] = g_tims.Distance[1]; //走行距離[100m]
		panel[15] = g_tims.Distance[2]; //走行距離[10m]
		// 矢印
		panel[84] = g_tims.ArrowDirection; //進行方向矢印
		// ユニット表示灯
		if (EUD == 2)
		{
			panel[142] = g_tims.UnitState[0]; //ユニット表示灯1
			panel[143] = g_tims.UnitState[1]; //ユニット表示灯2
			panel[144] = g_tims.UnitState[2]; //ユニット表示灯3
			panel[145] = g_tims.UnitState[3]; //ユニット表示灯4			
		}
		else if (EUD == 1)
		{
			panel[142] = g_tims.UnitTims[0]; //TIMSユニット表示1
			panel[143] = g_tims.UnitTims[1]; //TIMSユニット表示2
			panel[144] = g_tims.UnitTims[2]; //TIMSユニット表示3
			panel[145] = g_tims.UnitTims[3]; //TIMSユニット表示4
		}
		if (ETIMS != 0)
		{
			//TIMS全般表示
			panel[95] = g_tims.TrainArrow; //行路表矢印
			panel[186] = g_tims.Kind; //列車種別
			panel[187] = g_tims.Number[0]; //列車番号[1000位]
			panel[188] = g_tims.Number[1]; //列車番号[100位]
			panel[189] = g_tims.Number[2]; //列車番号[10位]
			panel[190] = g_tims.Number[3]; //列車番号[1位]
			panel[191] = g_tims.Charactor; //列車番号[記号]

			panel[93] = g_tims.Number[3] != 0 ? 1 : 0; //設定完了
			panel[94] = g_tims.PassMode; //通過設定
			//うさプラpanel[119] = g_tims.NextBlinkLamp; //次駅停車表示灯
			panel[178] = g_tims.From; //運行パターン始発
			panel[179] = g_tims.Destination; //運行パターン行先
			panel[180] = g_tims.For; //列車行先
			//panel[209] = g_tims.This; //自駅（TIS用）
			//panel[210] = g_tims.Next; //次駅

			// スタフテーブル
			//電列共通
			panel[126] = g_tims.HiddenLine[0] ? 0 : g_tims.Station[0]; //駅名表示1
			panel[127] = g_tims.HiddenLine[1] ? 0 : g_tims.Station[1]; //駅名表示2
			panel[128] = g_tims.HiddenLine[2] ? 0 : g_tims.Station[2]; //駅名表示3
			panel[129] = g_tims.HiddenLine[3] ? 0 : g_tims.Station[3]; //駅名表示4
			panel[130] = g_tims.HiddenLine[4] ? 0 : g_tims.Station[4]; //駅名表示5

			//panel[147] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Arrive[0][0]; //到着時刻1H
			panel[215] = g_tims.HiddenLine[0] ? 0 : g_tims.Arrive[0][1]; //到着時刻1M
			//panel[98] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Arrive[0][2]; //到着時刻1S
			//panel[148] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Arrive[1][0]; //到着時刻2H
			panel[216] = g_tims.HiddenLine[1] ? 0 : g_tims.Arrive[1][1]; //到着時刻2M
			//panel[99] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Arrive[1][2]; //到着時刻2S
			//panel[127] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Arrive[2][0]; //到着時刻3H
			panel[217] = g_tims.HiddenLine[2] ? 0 : g_tims.Arrive[2][1]; //到着時刻3M
			//panel[53] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Arrive[2][2]; //到着時刻3S
			//panel[128] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Arrive[3][0]; //到着時刻4H
			panel[218] = g_tims.HiddenLine[3] ? 0 : g_tims.Arrive[3][1]; //到着時刻4M
			//panel[54] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Arrive[3][2]; //到着時刻4S
			//panel[162] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Arrive[4][0]; //到着時刻5H
			panel[219] = g_tims.HiddenLine[4] ? 0 : g_tims.Arrive[4][1]; //到着時刻5M
			//panel[57] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Arrive[4][2]; //到着時刻5S
			/*
			//panel[150] = g_tims.HiddenLine[0] ? 0 : g_tims.Leave[0][0]; //発車時刻1H
			panel[230] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Leave[0][1]; //発車時刻1M
			panel[231] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Leave[0][2]; //発車時刻1S
			//panel[153] = g_tims.HiddenLine[1] ? 0 : g_tims.Leave[1][0]; //発車時刻2H
			panel[233] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Leave[1][1]; //発車時刻2M
			panel[234] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Leave[1][2]; //発車時刻2S
			//panel[156] = g_tims.HiddenLine[2] ? 0 : g_tims.Leave[2][0]; //発車時刻3H
			panel[198] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Leave[2][1]; //発車時刻3M
			panel[199] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Leave[2][2]; //発車時刻3S
			//panel[159] = g_tims.HiddenLine[3] ? 0 : g_tims.Leave[3][0]; //発車時刻4H
			panel[200] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Leave[3][1]; //発車時刻4M
			panel[201] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Leave[3][2]; //発車時刻4S
			//panel[162] = g_tims.HiddenLine[4] ? 0 : g_tims.Leave[4][0]; //発車時刻5H
			panel[202] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Leave[4][1]; //発車時刻5M
			panel[203] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Leave[4][2]; //発車時刻5S

			panel[204] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Track[0]; //次駅番線1
			panel[205] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Track[1]; //次駅番線2
			panel[206] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Track[2]; //次駅番線3
			panel[208] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Track[3]; //次駅番線4
			panel[209] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Track[4]; //次駅番線5

			if (DispType == 1)
			{
				/*
				//列車スタフ
				panel[170] = g_tims.HiddenLine[5] ? 0 : g_tims.Station[5]; //駅名表示6
				panel[171] = g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][0]; //到着時刻6H
				panel[172] = g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][1]; //到着時刻6M
				panel[173] = g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][2]; //到着時刻6S
				panel[174] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][0]; //発車時刻6H
				panel[175] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][1]; //発車時刻6M
				panel[176] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][2]; //発車時刻6S
				panel[177] = g_tims.HiddenLine[5] ? 0 : g_tims.Track[5]; //次駅番線6

				panel[178] = g_tims.HiddenLine[0] ? 0 : g_tims.Span[0][0]; //駅間走行時間1M
				panel[179] = g_tims.HiddenLine[0] ? 0 : g_tims.Span[0][1]; //駅間走行時間1S
				panel[180] = g_tims.HiddenLine[1] ? 0 : g_tims.Span[1][0]; //駅間走行時間2M
				panel[181] = g_tims.HiddenLine[1] ? 0 : g_tims.Span[1][1]; //駅間走行時間2S
				panel[182] = g_tims.HiddenLine[2] ? 0 : g_tims.Span[2][0]; //駅間走行時間3M
				panel[183] = g_tims.HiddenLine[2] ? 0 : g_tims.Span[2][1]; //駅間走行時間3S
				panel[184] = g_tims.HiddenLine[3] ? 0 : g_tims.Span[3][0]; //駅間走行時間4M
				panel[185] = g_tims.HiddenLine[3] ? 0 : g_tims.Span[3][1]; //駅間走行時間4S
				panel[186] = g_tims.HiddenLine[4] ? 0 : g_tims.Span[4][0]; //駅間走行時間5M
				panel[187] = g_tims.HiddenLine[4] ? 0 : g_tims.Span[4][1]; //駅間走行時間5S

				panel[188] = g_tims.HiddenLine[0] ? 0 : g_tims.LimitA[0]; //制限速度1IN
				panel[189] = g_tims.HiddenLine[0] ? 0 : g_tims.LimitL[0]; //制限速度1OUT
				panel[190] = g_tims.HiddenLine[1] ? 0 : g_tims.LimitA[1]; //制限速度2IN
				panel[191] = g_tims.HiddenLine[1] ? 0 : g_tims.LimitL[1]; //制限速度2OUT
				panel[192] = g_tims.HiddenLine[2] ? 0 : g_tims.LimitA[2]; //制限速度3IN
				panel[193] = g_tims.HiddenLine[2] ? 0 : g_tims.LimitL[2]; //制限速度3OUT
				panel[194] = g_tims.HiddenLine[3] ? 0 : g_tims.LimitA[3]; //制限速度4IN
				panel[195] = g_tims.HiddenLine[3] ? 0 : g_tims.LimitL[3]; //制限速度4OUT
				panel[196] = g_tims.HiddenLine[4] ? 0 : g_tims.LimitA[4]; //制限速度5IN
				panel[197] = g_tims.HiddenLine[4] ? 0 : g_tims.LimitL[4]; //制限速度5OUT
				panel[198] = g_tims.HiddenLine[5] ? 0 : g_tims.LimitA[5]; //制限速度6IN
				panel[199] = g_tims.HiddenLine[5] ? 0 : g_tims.LimitL[5]; //制限速度6OUT
				
			}
			else
			{
				//電車スタフ
				if (Row6 != 1)
				{
					panel[163] = g_9n.McKey != 6 ? 0 : g_tims.After; //次採時駅
					panel[164] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeA[0]; //次採時駅着時刻H
					panel[167] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeA[1]; //次採時駅着時刻M
					panel[168] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeA[2]; //次採時駅着時刻S
					//panel[174] = g_tims.AfterTimeL[0]; //次採時駅発時刻H
					panel[169] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeL[1]; //次採時駅発時刻M
					panel[170] = g_9n.McKey != 6 ? 0 : g_tims.AfterTimeL[2]; //次採時駅発時刻S
					panel[171] = g_9n.McKey != 6 ? 0 : g_tims.AfterTrack; //次採時駅番線
				}

				else
				{
					panel[163] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Station[5]; //駅名表示6
					panel[164] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][0]; //到着時刻6H
					panel[167] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][1]; //到着時刻6M
					panel[168] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][2]; //到着時刻6S
					//panel[174] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][0]; //発車時刻6H
					panel[169] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][1]; //発車時刻6M
					panel[170] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][2]; //発車時刻6S
					panel[171] = g_9n.McKey != 6 ? 0 : g_tims.HiddenLine[5] ? 0 : g_tims.Track[5]; //次駅番線6
				}

				panel[177] = g_9n.McKey != 6 ? 0 : g_tims.Before; //直前採時駅
				panel[146] = g_9n.McKey != 6 ? 0 : g_tims.BeforeTime[0]; //直前採時駅着時刻H
				panel[174] = g_9n.McKey != 6 ? 0 : g_tims.BeforeTime[1]; //直前採時駅着時刻M
				panel[175] = g_9n.McKey != 6 ? 0 : g_tims.BeforeTime[2]; //直前採時駅着時刻S
				panel[180] = g_9n.McKey != 6 ? 0 : g_tims.BeforeTrack; //直前採時駅番線

				panel[210] = g_9n.McKey != 6 ? 0 : g_tims.Last; //降車駅
				//panel[234] = g_9n.McKey == 6 ? 0 : g_tims.Last; //降車駅
				panel[211] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeA[0]; //降車駅着時刻H
				panel[212] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeA[1]; //降車駅着時刻M
				panel[213] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeA[2]; //降車駅着時刻S
				panel[214] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeL[0]; //降車駅発時刻H
				panel[215] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeL[1]; //降車駅発時刻M
				panel[216] = g_9n.McKey != 6 ? 0 : g_tims.LastTimeL[2]; //降車駅発時刻S
				panel[217] = g_9n.McKey != 6 ? 0 : g_tims.LastTrack; //降車駅番線

				panel[218] = g_9n.McKey != 6 ? 0 : g_tims.AfterKind; //列車種別
				panel[219] = g_9n.McKey != 6 ? 0 : g_tims.AfterNumber[0]; //列車番号[1000位]
				panel[220] = g_9n.McKey != 6 ? 0 : g_tims.AfterNumber[1]; //列車番号[100位]
				panel[221] = g_9n.McKey != 6 ? 0 : g_tims.AfterNumber[2]; //列車番号[10位]
				panel[222] = g_9n.McKey != 6 ? 0 : g_tims.AfterNumber[3]; //列車番号[1位]
				panel[223] = g_9n.McKey != 6 ? 0 : g_tims.AfterChara; //列車番号[記号]

				panel[224] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[0][0]; //次行路着時刻H
				panel[225] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[0][1]; //次行路着時刻M
				panel[226] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[0][2]; //次行路着時刻S
				panel[227] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[1][0]; //次行路発時刻H
				panel[228] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[1][1]; //次行路発時刻M
				panel[229] = g_9n.McKey != 6 ? 0 : g_tims.AfterTime[1][2]; //次行路発時刻S
			}

			//次駅表示
			/*うさプラ
			panel[203] = g_tims.HiddenLine[D01ABnum] ? 0 : (g_tims.Next * g_tims.NextBlink); //駅名表示（次駅、点滅する）
			panel[204] = g_tims.HiddenLine[D01ABnum] ? 0 : g_tims.NextTime[0]; //次駅着時刻H
			panel[205] = g_tims.HiddenLine[D01ABnum] ? 0 : g_tims.NextTime[1]; //次駅着時刻M
			panel[206] = g_tims.HiddenLine[D01ABnum] ? 0 : g_tims.NextTime[2]; //次駅着時刻S
			panel[207] = g_tims.HiddenLine[D01ABnum] ? 0 : g_tims.NextTrack; //次駅着番線
			*/
		}
	}


	// サウンド出力
	sound[225] = g_sub.LbInit; //
	sound[100] = g_sub.AirApply; //ブレーキ昇圧音
	sound[101] = g_sub.AirApplyEmg; //ブレーキ昇圧音（非常）
	sound[103] = g_sub.AirHigh; //非常ブレーキ緩解音
	sound[105] = g_sub.EmgAnnounce; //非常ブレーキ放送
	sound[106] = g_sub.UpdateInfo; //運行情報更新

	sound[ini.Disp.SoundIdx + 1] = (DispType == 5 || DispType == 9) && snp2Output == 0 && g_9n.McKey != 6 ? ATS_SOUND_STOP : g_spp.HaltChime3; //停車チャイムループ（減少しない）
	sound[ini.Disp.SoundIdx + 2] = (DispType == 5 || DispType == 9) && snp2Output == 0 && g_9n.McKey != 6 ? ATS_SOUND_STOP : g_spp.HaltChime; //停車チャイム
	sound[ini.Disp.SoundIdx + 3] = (DispType == 5 || DispType == 9) && snp2Output == 0 && g_9n.McKey != 6 ? ATS_SOUND_STOP : g_spp.PassAlarm; //通過チャイム
	sound[ini.Disp.SoundIdx] = (DispType == 5 || DispType == 9) && snp2Output == 0 && g_9n.McKey != 6 ? ATS_SOUND_STOP : g_spp.HaltChime2; //停車チャイムループ

    return g_output;
}

ATS_API void WINAPI SetPower(int notch)
{
	g_powerNotch = notch;
}

ATS_API void WINAPI SetBrake(int notch)
{
	g_sub.PlaySoundAirHigh(g_brakeNotch, notch);
	g_sub.PlaySoundEmgAnnounce(g_brakeNotch, notch);
	g_sub.PlaySoundAirApplyEmg(g_brakeNotch, notch);
	g_spp.NotchChanged();
	g_brakeNotch = notch;
}

ATS_API void WINAPI SetReverser(int pos)
{
	g_reverser = pos;
	g_sub.SetLbInit(pos, g_sub.event_lbInit);
}

ATS_API void WINAPI KeyDown(int atsKeyCode)
{
}

ATS_API void WINAPI KeyUp(int hornType)
{
}

ATS_API void WINAPI HornBlow(int atsHornBlowIndex)
{
}

ATS_API void WINAPI DoorOpen()
{
	g_pilotLamp = false;
	g_tims.DoorOpen();
	g_spp.StopChime();
}

ATS_API void WINAPI DoorClose()
{
	g_pilotLamp = true;
	g_tims.DoorClose();
}

ATS_API void WINAPI SetSignal(int signal)
{
}

ATS_API void WINAPI SetBeaconData(ATS_BEACONDATA beaconData)
{
	if (beaconData.Type == 615)//互換モードを判定する
	{
		snp2Beacon = beaconData.Optional == 0 ? 0 : 1;
	}
	switch (beaconData.Type)//メトロ互換は615番地上子によらない
	{
	case 10: //メトロTIS駅名読み込み
		g_9n.SetSESta(g_9n.ConvUsao2TIMS(beaconData.Optional));
		break;
	case 53: //走行距離
		g_tims.SetPositionDef(beaconData.Optional / 10, beaconData.Optional % 10 == 0 ? -1 : 1);
		break;
	case 70: //小田急TIS駅名割り込み
		g_9n.SetSESta(beaconData.Optional / 100);
		break;
	case 89: //方向判定・系統設定（下り31～江ノ島線、51～多摩線、上り51～新宿行き）
		g_9n.SetSEDirection(beaconData.Optional % 2, beaconData.Optional);
		break;
	case 604: //駅名読み込み
		g_9n.SetSESta(beaconData.Optional);
		break;
	case 621: //距離程設定
		g_9n.SetLocation(beaconData.Optional);
		break;
	case 622:
		g_9n.RecieveSE(beaconData.Optional % 10000000, beaconData.Optional / 10000000);
		break;

	}
	if (snp2Beacon != 1)
	{
		switch (beaconData.Type)
		{
		case 30://次駅接近
			if (g_speed != 0)//駅ジャンプを除外する
				g_spp.Recieve(beaconData.Optional % 100000);
			break;
		case 150://列番
			g_tims.SetNumber(beaconData.Optional / 100, beaconData.Optional % 100);
			break;
		case 101://種別
			g_tims.SetKind(beaconData.Optional);
			break;
		case 102: //進行方向
			g_tims.SetDirection(beaconData.Optional);
			break;
		case 103: //走行距離
			g_tims.SetDistance(beaconData.Distance, beaconData.Optional);
			break;
		case 104://運行パターン
			g_tims.SetLeg(beaconData.Optional);
			break;
		case 105://次駅接近
			if (g_speed != 0)//駅ジャンプを除外する
				g_spp.Recieve(beaconData.Optional % 10000);
			g_tims.Recieve(beaconData.Optional % 10000000, beaconData.Optional / 10000000); //駅ジャンプを除外しない
			break;
		case 106://自駅・次駅の設定
			g_tims.SetNext(beaconData.Optional);
			break;
		case 107://次駅の時刻設定
			g_tims.SetNextTime(beaconData.Optional);
			break;
		case 108://次駅到着番線の設定
			g_tims.SetNextTrack(beaconData.Optional);
			break;
		case 109://行先の設定
			g_tims.SetFor(beaconData.Optional);
			break;
		case 110://駅名
			g_tims.InputLine(1, (beaconData.Optional / 1000) - 1, beaconData.Optional % 1000);
			break;
		case 111://到着時刻
			g_tims.InputLine(2, (beaconData.Optional / 1000000) - 1, beaconData.Optional % 1000000);
			break;
		case 112://出発時刻
			g_tims.InputLine(3, (beaconData.Optional / 1000000) - 1, beaconData.Optional % 1000000);
			break;
		case 113://番線
			g_tims.InputLine(4, (beaconData.Optional / 100) - 1, beaconData.Optional % 100);
			break;
		case 114://到着制限
			g_tims.InputLine(5, (beaconData.Optional / 100) - 1, beaconData.Optional % 100);
			break;
		case 115://出発制限
			g_tims.InputLine(6, (beaconData.Optional / 100) - 1, beaconData.Optional % 100);
			break;
		case 116://駅間時間
			g_tims.InputLine(0, (beaconData.Optional / 10000) - 1, beaconData.Optional % 10000);
			break;
		case 117://行路表矢印
			g_tims.SetArrowState(beaconData.Optional);
			break;
		case 118:
			g_tims.SetAfteruent(1, beaconData.Optional, 0);
			break;
		case 119:
			g_tims.SetAfteruent(2, beaconData.Optional, 0);
			break;
		case 120:
			g_dead.AlartACDC(beaconData.Optional);
			break;
		case 121:
			g_dead.SetACDC(beaconData.Optional);
			break;
		case 122:
			g_tims.SetAfteruent(0, beaconData.Optional / 100, beaconData.Optional % 100);
			break;
		case 123:
			g_tims.SetAfteruent(3, beaconData.Optional, 0);
			break;
		case 124:
			g_tims.SetLastStop(0, beaconData.Optional);
			break;
		case 125:
			g_tims.SetLastStop(1, beaconData.Optional);
			break;
		case 126:
			g_tims.SetLastStop(2, beaconData.Optional);
			break;
		case 127:
			g_tims.SetLastStation(beaconData.Optional);
			break;
		case 128:
			g_tims.SetTimeStationTime(0, beaconData.Optional);
			break;
		case 129:
			g_tims.SetTimeStationTime(1, beaconData.Optional);
			break;
		case 130:
			g_tims.SetTimeStationTime(2, beaconData.Optional);
			break;
		case 131:
			g_tims.SetTimeStationTime(3, beaconData.Optional);
			break;
		case 132:
			g_tims.SetTimeStationTime(4, beaconData.Optional);
			break;
		case 133:
			g_tims.SetTimeStationName(0, beaconData.Optional);
			break;
		case 134:
			g_tims.SetTimeStationName(1, beaconData.Optional);
			break;
		}
	}
	else
	{
		switch (beaconData.Type)
		{
		case 8:
			if (g_speed != 0)//駅ジャンプを除外する
				g_spp.Recieve(beaconData.Optional % 100000);
			break;
		case 100://次駅接近
			if (g_speed != 0)//駅ジャンプを除外する
				g_spp.Recieve(beaconData.Optional % 10000);
			g_tims.Recieve(beaconData.Optional % 10000000, beaconData.Optional / 10000000); //駅ジャンプを除外しない
			break;
		case 102://自駅・次駅の設定・次駅時刻の設定
			if (beaconData.Optional >= 10000)
			{
				g_tims.SetNextTime(beaconData.Optional);
			}
			else
			{
				int from = beaconData.Optional / 100;
				int next = beaconData.Optional % 100;
				g_tims.SetNext(from * 1000 + next);
			}
			break;
		case 105://駅名
			g_tims.InputLine(1, (beaconData.Optional / 100) - 1, beaconData.Optional % 100);
			break;
		case 106://到着時刻
			g_tims.InputLine(7, (beaconData.Optional / 1000000) - 1, beaconData.Optional % 1000000);
			break;
		case 107://出発時刻
			g_tims.InputLine(8, (beaconData.Optional / 1000000) - 1, beaconData.Optional % 1000000);
			break;
		case 108://番線・制限
			g_tims.InputLine(4, (beaconData.Optional / 10000) - 1, ((beaconData.Optional % 10000) - (beaconData.Optional % 100)) / 100); //到着番線
			g_tims.InputLine(5, (beaconData.Optional / 10000) - 1, beaconData.Optional % 100); //制限速度
			break;
		case 109://駅間時間
			g_tims.InputLine(9, (beaconData.Optional / 10000) - 1, beaconData.Optional % 10000);
			break;
		case 110://種別
			g_tims.SetKind(beaconData.Optional);
			break;
		case 111://列番
			g_tims.SetNumber(beaconData.Optional % 10000, beaconData.Optional / 10000);
			break;
		case 112://運行パターン
		{
			int from = beaconData.Optional / 100;
			int destination = beaconData.Optional % 100;
			g_tims.SetLeg(from * 1000 + destination);
		}
			break;
		case 113: //走行距離
		{
			int direction = beaconData.Optional / 10000;
			int data = beaconData.Optional % 10000;
			g_tims.SetDistance(beaconData.Distance, direction * 1000000 + data);
		}
			break;
		case 114: //走行距離
			g_tims.SetPositionDef(beaconData.Optional, 0);
			break;
		case 115:
			g_tims.SetAfteruent(0, beaconData.Optional / 100, beaconData.Optional % 100);
			break;
		case 116:
			g_tims.SetAfteruent(1, beaconData.Optional, 0);
			break;
		case 117:
			g_tims.SetAfteruent(2, beaconData.Optional, 0);
			break;
		case 118:
			g_tims.SetAfteruent(3, beaconData.Optional, 0);
			break;
		case 119: //進行方向
			g_tims.SetDirection(beaconData.Optional);
			break;
		case 122:
			g_tims.SetLastStop(0, beaconData.Optional);
			break;
		case 123:
			g_tims.SetLastStop(2, beaconData.Optional);
			break;
		case 124: //降車駅設定
			g_tims.SetLastStation(beaconData.Optional);
			break;
		case 125: //行路表矢印
			g_tims.SetArrowState(beaconData.Optional);
			break;
		case 201:
			g_dead.AlartACDC(beaconData.Optional);
			break;
		case 200:
			g_dead.SetACDC(beaconData.Optional);
			break;
		}
	}
}

ATS_API void WINAPI Dispose(void) 
{
}