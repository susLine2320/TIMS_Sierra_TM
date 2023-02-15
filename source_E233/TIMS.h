int g_emgBrake; // 非常ノッチ
int g_svcBrake; // 常用最大ノッチ
int g_brakeNotch; // ブレーキノッチ
int g_powerNotch; // 力行ノッチ
int g_reverser; // レバーサ
bool g_pilotlamp; // パイロットランプ
int g_time; // 現在時刻
float g_speed; // 速度計の速度[km/h]
int g_deltaT; // フレーム時間[ms/frame]
	//うさプラ対応
int McKey; //マスコンキー
int ini_UseJRTIMS; //JRTIMS使用可否
int ini_UseUnit; //ユニット灯使用可否
int ini_UseDistance; //走行距離使用可否
int ini_Line; //系統（5：東西、7：南北、9：千代田、13：副都心）

TIMS_new_TMIni ini;

ATS_HANDLES g_output; // 出力

#define ATS_BEACON_SPP 30 // 停車駅通過防止装置
#define ATS_BEACON_NUM 150 // TIMS列車番号
#define ATS_BEACON_KIND 101 // TIMS列車種別
#define ATS_BEACON_DIR 102 // TIMS進行方向設定
#define ATS_BEACON_DIST 103 // TIMS距離加算設定
#define ATS_BEACON_APP 105 // TIMS次駅接近
#define ATS_BEACON_NEXT 106 // TIMS次駅設定
#define ATS_BEACON_NEXTTIME 107 // TIMS次駅到着時刻設定
#define ATS_BEACON_NEXTTRL 108 // TIMS次駅到着番線設定
#define ATS_BEACON_STA 110 // TIMS駅名表示
#define ATS_BEACON_ARV 111 // TIMS到着時刻
#define ATS_BEACON_LEV 112 // TIMS出発時刻
#define ATS_BEACON_TRL 113 // TIMS到着番線
#define ATS_BEACON_LIM1 114 // TIMS制限速度IN
#define ATS_BEACON_LIM2 115 // TIMS制限速度OUT
#define ATS_BEACON_SPN 116 // TIMS駅間時間

#define ATS_BEACON_LEG 104 // TIMS運行パターン
#define ATS_BEACON_FOR 109 // TIMS列車行先
#define ATS_BEACON_ARW 117 // TIMS行路表矢印

#define ATS_BEACON_SUBA 118 //TIMS次行路着時刻
#define ATS_BEACON_SUBL 119 //TIMS次行路発時刻
#define ATS_BEACON_SUBN 122 //TIMS次行路列番設定
#define ATS_BEACON_SUBK 123 //TIMS次行路種別設定

#define ATS_BEACON_LSA 124 //TIMS終点到着時刻
#define ATS_BEACON_LSL 125 //TIMS終点出発時刻
#define ATS_BEACON_LST 126 //TIMS終点到着番線
#define ATS_BEACON_LSN 127 //TIMS行路終点駅名

#define ATS_BEACON_BTSL 128 //TIMS直前採時駅時刻
#define ATS_BEACON_ATSA 129 //TIMS次採時駅着時刻
#define ATS_BEACON_ATSL 130 //TIMS次採時駅発時刻
#define ATS_BEACON_BTST 131 //TIMS直前採時駅番線
#define ATS_BEACON_ATST 132 //TIMS次採時駅番線
#define ATS_BEACON_BTSN 133 //TIMS直前採時駅駅名
#define ATS_BEACON_ATSN 134 //TIMS次採時駅駅名


#define TIMS_DECELERATION 19.5F // 減速定数 (減速度[km/h/s] x 7.2)
// 19.5=55^2/(180-25)
#define TIMS_OFFSET 0.5F // 車上子オフセット[m]

#define RANDOMIZE_UNITTIMS 30 // TIMSユニット表示の更新確率
#define RANDOMIZE_UNITLAMP 10 // ユニット表示灯の更新確率
#define UPDATE_SPAN 180000 // 運行情報更新の最低間隔
#define RELAY_REQUIREMENT 7.5f // 走行検知リレー動作する速度

#define SPP_DECELERATION 19.5F // 減速定数 (減速度[km/h/s] x 7.2)
// 19.5=55^2/(180-25)
#define SPP_OFFSET 0.5F // 車上子オフセット[m]
#define SPP_ALARMTYPE 1 // 0: 繰り返し発音, 1; 1回発音
#pragma warning(disable: 4996)

class CTims
{
public:
	int EmergencyNotch; // 非常ノッチ
	int *BrakeNotch; // ブレーキノッチ
	int *PowerNotch; // 力行ノッチ
	int *Reverser; // レバーサー
	int *Time; // 現在時刻
	float *TrainSpeed; // 速度計の速度[km/h]
	int *DeltaT; // フレーム時間[ms/frame]
	float BcPressure; // ブレーキシリンダ圧力[kPa]
	float MrPressure; // 元空気ダメ圧力[kPa]
	float Current; // 電流[A]
	double Location; // 列車位置[m]

	int Kind; // 列車種別
	int Number[4]; // 列車番号(1000 - 100 - 10 - 1)
	int Charactor; // 列車番号(記号)
	int From; // 駅名表示(始発)
	int For; // 駅名表示(列車行先)
	int Destination; // 駅名表示(行先)
	int This; // 駅名表示(自駅)
	int Next; // 駅名表示(次駅)
	int NextBlink; // 駅名表示の点滅
	int	NextBlinkLamp; // 次駅停車灯 20190207追加
	int NextTime[3]; // 到着時刻(次駅、時 - 分 - 秒)
	int NextTrack; // 次駅到着番線
	int Crawl; // 徐行区間
	int CrawlLimit; // 徐行速度
	int AfterNumber[4]; // 次運用列車番号(1000 - 100 - 10 - 1)
	int AfterChara; // 次運用記号
	int AfterTime[2][3]; // 次運用着発時刻
	int AfterKind; //次運用種別
	int TrainArrow; // 行路表矢印
	int Station[10]; // 駅名表示
	int SESta[10]; //社線駅名
	int SEDirection; //社線進行方向
	int SEArea; //社線用走行方面
	int PassSta[10]; // 通過駅名表示
	int Arrive[10][3]; // 到着時刻(時 - 分 - 秒)
	int Leave[10][3]; // 出発時刻(時 - 分 - 秒)
	int Track[10]; // 次駅到着番線
	int Limit[10]; // 最高速度IN
	int Limit2[10]; // 最高速度OUT
	int Span[10][2]; // 駅間走行時間
	int HiddenLine[10]; // 更新時に1行ずつ非表示にする
	int TrackPath[10]; // 開通情報(100mごと)
	int TrackPath2[120]; // 開通情報(25mごと)
	float StartingPoint; // 距離計算の起点
	int Distance1000; // 走行距離(kmの桁)
	int Distance100; // 走行距離(100mの桁)
	int Distance10; // 走行距離(10mの桁)
	int Location1000; // 列車位置(kmの桁)
	int Location100; // 列車位置(100mの桁)
	int Location10; // 列車位置(10mの桁)
	int TimsSpeed100; // TIMS速度(100の桁)
	int TimsSpeed10; // TIMS速度(10の桁)
	int TimsSpeed1; // TIMS速度(1の桁)
	int UnitTims[10]; // TIMSユニット表示(0-9)
	int UnitState[10]; // ユニット表示灯(0-9)
	int BrakeDelay; // ブレーキ指令
	int AccelDelay; // 力行指令
	int BcCaution; // 200kPa警告
	int BcPressCut; // ブレーキ減圧する
	int RelayState; // 走行検知リレー
	int Direction; // 進行方向
	int ArrowDirection; // 進行方向矢印
	int PassMode; // 通過設定

	//電車スタフ用追加
	int	Before; //直前採時駅
	int	After; //次採時駅
	int	BeforeStationTime[3]; // 直前採時駅到着時刻(時 - 分 - 秒)
	int	AfterStationTimeA[3]; // 次採時駅到着時刻(時 - 分 - 秒)
	int	AfterStationTimeL[3]; // 次採時駅発車時刻(時 - 分 - 秒)
	int	BeforeTrack; // 直前採時駅番線
	int	AfterTrack; // 次採時駅番線
	int LastStopTime[3]; // 終点到着時刻(時 - 分 - 秒)
	int LastStopTimeL[3]; // 終点出発時刻(時 - 分 - 秒)
	int LastStopTrack; // 終点到着番線
	int RelayName; // 行路終点駅名


	int Speed; // 速度計
	int Speed100; // 速度(100の桁)
	int Speed10; // 速度(10の桁)
	int Speed1; // 速度(1の桁)
	int BcPress0; // ブレーキシリンダ指針(0-180kPa)
	int BcPress1; // ブレーキシリンダ指針(200-380kPa)
	int BcPress2; // ブレーキシリンダ指針(400-580kPa)
	int BcPress3; // ブレーキシリンダ指針(600-780kPa)

	int BcPress4; // ブレーキシリンダ数値(100位)
	int BcPress5; // ブレーキシリンダ数値(10位)
	int BcPress6; // ブレーキシリンダ数値(1位)
	int BcPress7; // ブレーキシリンダ指針(int型)

	int MrPress0; // 元空気ダメ指針(750-795kPa)
	int MrPress1; // 元空気ダメ指針(800-845kPa)
	int MrPress2; // 元空気ダメ指針(850-895kPa)

	int MrPress4; // 元空気ダメ数値(100位)
	int MrPress5; // 元空気ダメ数値(10位)
	int MrPress6; // 元空気ダメ数値(1位)
	int MrPress7; // 元空気ダメ指針(int型)

	int Ammeter0; // 電流計
	int Ammeter1; // 電流計1000位
	int Ammeter2; // 電流計100位
	int Ammeter3; // 電流計10位
	int Ammeter4; // 電流計1位
	int AmmeterC; // 電流計符号

	int AccelCutting; // 力行遅延(この時刻まで反映しない)

	int RelayD; // 走行検知リレー
	int LbInit; // 初回起動時断流器音
	int AirHigh; // 非常ブレーキ緩解音
	int AirApply; // ブレーキ昇圧音
	int EmgAnnounce; // 非常ブレーキ放送
	int AirApplyEmg; // ブレーキ昇圧音(非常)
	int UpdateInfo; // 運行情報更新


	// 交直切替
	int AC; // 交流
	int DC; // 直流
	int CVacc; // 制御電圧異常
	int CVacc10; // 制御電圧(10位)
	int CVacc1; // 制御電圧(1位)
	int ACacc; // 交流電圧異常
	int ACacc10000; // 交流電圧(10000位)
	int ACacc1000; // 交流電圧(1000位)
	int ACacc100; // 交流電圧(100位)
	int DCacc; // 直流電圧異常
	int DCacc1000; // 直流電圧(1000位)
	int DCacc100; // 直流電圧(100位)
	int DCacc10; // 直流電圧(10位)
	int Cvmeter; // 制御指針
	int Acmeter; // 交流指針
	int Dcmeter; // 直流指針
	int Accident; // 事故
	int Tp; // 三相
	int VCB_ON; // VCB全入
	int VCB_OFF; // VCB全切
	int VCB; // VCB
	int tmr_ACDC;
	int step_ACDC;
	int alert_ACDC;

	// INIファイルから設定
	int ini_cutPressTime; // ブレーキ圧を減圧する時間
	int event_lbInit; // 初回起動時力行遅延

	void load(void)
	{
		Kind = 0; // 列車種別
		Charactor = 0; // 列車番号(記号)
		From = 0; // 駅名表示(始発)
		For = 0; // 駅名表示(列車行先)
		Destination = 0; // 駅名表示(行先)
		This = 0; // 駅名表示(自駅)
		Next = 0; // 駅名表示(次駅)
		NextBlink = 0; // 駅名表示の点滅
		NextBlinkLamp = 0; // 次駅停車灯 20190207追加
		NextTrack = 0; // 次駅到着番線
		Crawl = 0; // 徐行区間
		CrawlLimit = 0; // 徐行速度
		AfterChara = 0; // 次運用記号
		LastStopTrack = 0; // 終点到着番線
		RelayName = 0; // 行路終点駅名
		TrainArrow = 0; // 行路表矢印

		m_dist = 0; // 停止予定点距離

		m_pushUpFlag = -1; // 表示更新のフラグ
		m_pushUpBeacon = 0; // 表示更新の地上子
		m_pushUpCount = 0; // 表示更新の繰り返し数
		m_tmrVisible = *Time; // モニタのステップ更新

		m_thisName = 0; // 自駅名
		m_nextName = 0; // 次駅名
		m_nextTrack = 0; // 次駅到着番線

		m_pathStaDist = 0; // 開通情報の駅位置情報

		m_distance = 0; // 走行距離
		m_direction = 1; // 距離減算の判定
		m_distDef = 0; // 列車位置の補正

		ResetArray(); // 配列の初期化
		// PushNext(); // 次駅を最新にする
		// Receive(0); // テーブルを更新する

		StartingPoint = 0.0f; // 距離計算の起点
		Distance1000 = 0; // 走行距離(kmの桁)
		Distance100 = 10; // 走行距離(100mの桁)
		Distance10 = 10; // 走行距離(10mの桁)
		Location1000 = 0; // 列車位置(kmの桁)
		Location100 = 10; // 列車位置(100mの桁)
		Location10 = 10; // 列車位置(10mの桁)

		TimsSpeed100 = 0; // TIMS速度(100の桁)
		TimsSpeed10 = 0; // TIMS速度(10の桁)
		TimsSpeed1 = 10; // TIMS速度(1の桁)

		BrakeDelay = 0; // ブレーキ指令
		AccelDelay = 0; // 力行指令
		BcCaution = 0; // 200kPa警告
		BcPressCut = 0; // ブレーキ減圧する
		RelayState = 0; // 走行検知リレー
		Direction = 0; // 進行方向
		ArrowDirection = 0; // 進行方向矢印
		PassMode = 0; // 通過設定

		Before = 0; //直前採時駅
		After = 0; //次採時駅
		BeforeTrack = 0; // 直前採時駅番線
		AfterTrack = 0; // 次採時駅番線

		SEDirection = 1; //社線用進行方向
		SEArea = 0; //社線用走行方面

		Speed = 0; // 速度計
		Speed100 = 10; // 速度(100の桁)
		Speed10 = 10; // 速度(10の桁)
		Speed1 = 0; // 速度(1の桁)
		BcPress0 = 10; // ブレーキシリンダ指針(0-180kPa)
		BcPress1 = 10; // ブレーキシリンダ指針(200-380kPa)
		BcPress2 = 10; // ブレーキシリンダ指針(400-580kPa)
		BcPress3 = 10; // ブレーキシリンダ指針(600-780kPa)
		BcPress4 = 10; // ブレーキシリンダ指針(100位)
		BcPress5 = 10; // ブレーキシリンダ指針(10位)
		BcPress6 = 0; // ブレーキシリンダ指針(1位)
		BcPress7 = 0; // ブレーキシリンダ指針(int)

		MrPress0 = 10; // 元空気ダメ指針(750-795kPa)
		MrPress1 = 10; // 元空気ダメ指針(800-845kPa)
		MrPress2 = 10; // 元空気ダメ指針(850-895kPa)
		MrPress4 = 10; // 元空気ダメ指針(100位)
		MrPress5 = 10; // 元空気ダメ指針(10位)
		MrPress6 = 0; // 元空気ダメ指針(1位)
		MrPress7 = 0; // 元空気ダメ指針(int)

		Ammeter0 = 0; // 電流計

		Ammeter1 = 0; // 電流計1000位
		Ammeter2 = 0; // 電流計100位
		Ammeter3 = 0; // 電流計10位
		Ammeter4 = 0; // 電流計1位
		AmmeterC = 10; // 電流計符号

		AccelCutting = 0; // 力行遅延

		m_tmrBcPressCut = 0; // ブレーキ減圧のタイマー

		m_relayD = ATS_SOUND_STOP; // 走行検知リレー
		m_lbInit = ATS_SOUND_STOP; // 初回起動時断流器音
		m_airHigh = ATS_SOUND_STOP; // 非常ブレーキ緩解音
		AirApply = ATS_SOUND_STOP; // ブレーキ昇圧音
		m_bcPrsDelta = 0.0F; // ブレーキシリンダ圧力[kPa]
		m_emgAnnounce = ATS_SOUND_STOP; // 非常ブレーキ放送
		m_airApplyEmg = ATS_SOUND_STOP; // ブレーキ昇圧音(非常)
		m_updateInfo = ATS_SOUND_STOP; // 運行情報更新
		m_deltaUpdate = *Time - UPDATE_SPAN; // 前回運行情報更新

		// 交直切替
		AC = 0; // 交流
		DC = 1; // 直流
		CVacc = 0; // 制御電圧異常
		CVacc10 = 10; // 制御電圧(10位)
		CVacc1 = 10; // 制御電圧(1位)
		ACacc = 0; // 交流電圧異常
		ACacc10000 = 10; // 交流電圧(10000位)
		ACacc1000 = 10; // 交流電圧(1000位)
		ACacc100 = 10; // 交流電圧(100位)
		DCacc = 0; // 直流電圧異常
		DCacc1000 = 10; // 直流電圧(1000位)
		DCacc100 = 10; // 直流電圧(100位)
		DCacc10 = 10; // 直流電圧(10位)
		Cvmeter = 100; // 制御指針
		Acmeter = 0; // 交流指針
		Dcmeter = 1600; // 直流指針
		Accident = 0; // 事故
		Tp = 0; // 三相
		VCB_ON = 1; // VCB全入
		VCB_OFF = 0; // VCB全切
		VCB = 1; // VCB
		tmr_ACDC = *Time;
		step_ACDC = 3;
		alert_ACDC = 0;
	}

	void initialize(void)
	{
	}

	void execute(void)
	{
		// 毎フレーム
		float speed = fabsf(*TrainSpeed); // 速度の絶対値[km/h]
		float def = speed / 3600 * *DeltaT; // 1フレームで動いた距離(絶対値)[m]
		float def2 = *TrainSpeed / 3600 * *DeltaT; // 1フレームで動いた距離(相対値)[m]
		m_dist -= def; // 残り距離を減算する

		// 交直切替
		int temp_time = *Time - tmr_ACDC;
		switch(step_ACDC)
		{
		case 0: // 直流→VCB切
			VCB_OFF = 1;
			VCB_ON = 0;
			VCB = 2;
			
			if(temp_time > 250)
			{
				Accident = 1; // 事故
			}
			if(temp_time > 500)
			{
				DCacc = 1; // 直流電圧異常
				DCacc1000 = 10; // 直流電圧(1000位)
				DCacc100 = 10; // 直流電圧(100位)
				DCacc10 = 10; // 直流電圧(10位)
				Dcmeter = 0; // 直流指針
			}
			if(temp_time > 750)
			{
				CVacc = 1; // 制御電圧異常
				Cvmeter = 83; // 制御指針
				CVacc10 = 8; // 制御電圧(10位)
				CVacc1 = 3; // 制御電圧(1位)
			}
			if(temp_time > 1000)
			{
				Tp = 1; // 三相
			}
			break;
		case 1: // 無電区間
			VCB_OFF = 0;
			VCB_ON = 0;
			VCB = 0;

			AC = 0;
			DC = 0;
			ACacc = 0; // 交流電圧異常
			ACacc10000 = 10; // 交流電圧(10000位)
			ACacc1000 = 10; // 交流電圧(1000位)
			ACacc100 = 10; // 交流電圧(100位)
			DCacc = 0; // 直流電圧異常
			DCacc1000 = 10; // 直流電圧(1000位)
			DCacc100 = 10; // 直流電圧(100位)
			DCacc10 = 10; // 直流電圧(10位)
			Acmeter = 0; // 交流指針
			Dcmeter = 0; // 直流指針
			break;
		case 2: // AC有電区間
			VCB_OFF = 0;
			VCB_ON = 1;
			VCB = 1;

			DC = 0;
			DCacc = 0; // 直流電圧異常
			DCacc1000 = 10; // 直流電圧(1000位)
			DCacc100 = 10; // 直流電圧(100位)
			DCacc10 = 10; // 直流電圧(10位)
			Dcmeter = 0; // 直流指針

			AC = 1;
			if(temp_time > 500)
			{
				ACacc = 1; // 交流電圧異常
				ACacc10000 = 10; // 交流電圧(10000位)
				ACacc1000 = 2; // 交流電圧(1000位)
				ACacc100 = 2; // 交流電圧(100位)
				Acmeter = 220; // 交流指針
			}
			if(temp_time > 1000)
			{
				ACacc = 1; // 交流電圧異常
				ACacc10000 = 1; // 交流電圧(10000位)
				ACacc1000 = 1; // 交流電圧(1000位)
				ACacc100 = 2; // 交流電圧(100位)
				Acmeter = 11200; // 交流指針
			}
			if(temp_time > 2000)
			{
				ACacc = 0; // 交流電圧異常
				ACacc10000 = 10; // 交流電圧(10000位)
				ACacc1000 = 10; // 交流電圧(1000位)
				ACacc100 = 10; // 交流電圧(100位)
				Acmeter = 15500; // 交流指針
			}
			if(temp_time > 3000)
			{
				ACacc = 0; // 交流電圧異常
				ACacc10000 = 10; // 交流電圧(10000位)
				ACacc1000 = 10; // 交流電圧(1000位)
				ACacc100 = 10; // 交流電圧(100位)
				Acmeter = 18000; // 交流指針
			}
			if(temp_time > 4000)
			{
				ACacc = 0; // 交流電圧異常
				ACacc10000 = 10; // 交流電圧(10000位)
				ACacc1000 = 10; // 交流電圧(1000位)
				ACacc100 = 10; // 交流電圧(100位)
				Acmeter = 20000; // 交流指針
			}
			if(temp_time > 5000)
			{
				ACacc = 0; // 交流電圧異常
				ACacc10000 = 10; // 交流電圧(10000位)
				ACacc1000 = 10; // 交流電圧(1000位)
				ACacc100 = 10; // 交流電圧(100位)
				Acmeter = 22000; // 交流指針
			}
			if(temp_time > 8000)
			{
				Accident = 0; // 事故
			}
			if(temp_time > 12000)
			{
				Tp = 0; // 三相
			}
			if(temp_time > 13000)
			{
				CVacc = 0; // 制御電圧異常
				CVacc10 = 10; // 制御電圧(10位)
				CVacc1 = 10; // 制御電圧(1位)
				Cvmeter = 100; // 制御指針
			}
			break;
		case 3: // DC有電区間
			VCB_OFF = 0;
			VCB_ON = 1;
			VCB = 1;
			
			AC = 0;
			ACacc = 0; // 交流電圧異常
			ACacc10000 = 10; // 交流電圧(10000位)
			ACacc1000 = 10; // 交流電圧(1000位)
			ACacc100 = 10; // 交流電圧(100位)
			Acmeter = 0; // 交流指針
			
			DC = 1;
			if(temp_time > 500)
			{
				DCacc = 1; // 直流電圧異常
				DCacc1000 = 10; // 直流電圧(1000位)
				DCacc100 = 1; // 直流電圧(100位)
				DCacc10 = 8; // 直流電圧(10位)
				Dcmeter = 180; // 直流指針
			}
			if(temp_time > 1000)
			{
				DCacc = 1; // 直流電圧異常
				DCacc1000 = 10; // 直流電圧(1000位)
				DCacc100 = 7; // 直流電圧(100位)
				DCacc10 = 4; // 直流電圧(10位)
				Dcmeter = 740; // 直流指針
			}
			if(temp_time > 2000)
			{
				DCacc = 0; // 直流電圧異常
				DCacc1000 = 10; // 直流電圧(1000位)
				DCacc100 = 10; // 直流電圧(100位)
				DCacc10 = 10; // 直流電圧(10位)
				Dcmeter = 1380; // 直流指針
			}
			if(temp_time > 3000)
			{
				DCacc = 0; // 直流電圧異常
				DCacc1000 = 10; // 直流電圧(1000位)
				DCacc100 = 10; // 直流電圧(100位)
				DCacc10 = 10; // 直流電圧(10位)
				Dcmeter = 1500; // 直流指針
			}
			if(temp_time > 4000)
			{
				DCacc = 0; // 交流電圧異常
				DCacc1000 = 10; // 直流電圧(1000位)
				DCacc100 = 10; // 直流電圧(100位)
				DCacc10 = 10; // 直流電圧(10位)
				Dcmeter = 1600; // 直流指針
			}
			if(temp_time > 7000)
			{
				Accident = 0; // 事故
			}
			if(temp_time > 11000)
			{
				Tp = 0; // 三相
			}
			if(temp_time > 12000)
			{
				CVacc = 0; // 制御電圧異常
				CVacc10 = 10; // 制御電圧(10位)
				CVacc1 = 10; // 制御電圧(1位)
				Cvmeter = 100; // 制御指針
			}
			break;
		case 4: // 交流→VCB切
			VCB_OFF = 1;
			VCB_ON = 0;
			VCB = 2;
			
			if(temp_time > 250)
			{
				Accident = 1; // 事故
			}
			if(temp_time > 500)
			{
				ACacc = 1; // 交流電圧異常
				ACacc10000 = 10; // 交流電圧(10000位)
				ACacc1000 = 10; // 交流電圧(1000位)
				ACacc100 = 10; // 交流電圧(100位)
				Acmeter = 0; // 交流指針
			}
			if(temp_time > 750)
			{
				CVacc = 1; // 制御電圧異常
				Cvmeter = 83; // 制御指針
				CVacc10 = 8; // 制御電圧(10位)
				CVacc1 = 3; // 制御電圧(1位)
			}
			if(temp_time > 1000)
			{
				Tp = 1; // 三相
			}
			break;
		}

		// 初回起動時力行遅延
		if(event_lbInit > 0 && *PowerNotch > 0)
		{
			event_lbInit = 0;
			AccelCutting = *Time + 500;
			m_lbInit = ATS_SOUND_PLAY;
		}

		// 初回起動時断流器音
		LbInit = m_lbInit;
		m_lbInit = ATS_SOUND_CONTINUE;

		// ブレーキ減圧
		if(m_tmrBcPressCut < *Time)
		{
			BcPressCut = 0;
			m_tmrBcPressCut = 0;
		}
		else
		{
			BcPressCut = 1;
		}

		//緩解遅延タイマーの防護
		if(speed > 3.0f)
		{
			m_tmrBcPressCut = 0;
			BcPressCut = 0;
		}

		// 走行検知リレー
		if(RelayState == 0 && speed >= RELAY_REQUIREMENT)
		{
			RelayState = 1;
			m_relayD = ATS_SOUND_PLAY;
		}
		else if(RelayState == 1 && speed <= RELAY_REQUIREMENT)
		{
			RelayState = 0;
			m_relayD = ATS_SOUND_PLAY;
		}

		RelayD = m_relayD;
		m_relayD = ATS_SOUND_CONTINUE;

		// 非常ブレーキ緩解音
		AirHigh = m_airHigh;
		m_airHigh = ATS_SOUND_CONTINUE;

		// ブレーキ昇圧音
		if(*BrakeNotch > 0 && BcPressure > 40.0F && BcPressure > m_bcPrsDelta)
		{
			AirApply = ATS_SOUND_PLAYLOOPING;
		}
		else
		{
			AirApply = ATS_SOUND_STOP;
		}

		m_bcPrsDelta = BcPressure; // ブレーキシリンダ圧力[kPa]

		// 非常ブレーキ放送
		EmgAnnounce = m_emgAnnounce;
		m_emgAnnounce = ATS_SOUND_CONTINUE;

		// ブレーキ昇圧音(非常)
		AirApplyEmg = m_airApplyEmg;
		m_airApplyEmg = ATS_SOUND_CONTINUE;

		// 運行情報更新
		if(*TrainSpeed > 10.0f && *Time > m_deltaUpdate + UPDATE_SPAN)
		{
			m_deltaUpdate = *Time;

			if(rand()%200 == 0)
			{
				m_updateInfo = ATS_SOUND_PLAY;
			}
		}

		UpdateInfo = m_updateInfo; // 運行情報更新
		m_updateInfo = ATS_SOUND_CONTINUE;

		//駅名の挿入
		for (int i = 0; i < 7; i++)
		{
			if (SEDirection == 1) //営団A線、小田急下り
			{
				switch (SESta[i])
				{
				case 56: //北綾瀬
					SESta[i + 1] = 101;
					break;
				case 101:
					SESta[i + 1] = 57;
					break;
				case 58: //北千住
					SESta[i + 1] = 102;
					break;
				case 102:
					SESta[i + 1] = 59;
					break;
				case 62: //根津
					SESta[i + 1] = 103;
					break;
				case 103:
					SESta[i + 1] = 63;
					break;
				case 64: //新御茶ノ水
					SESta[i + 1] = 104;
					break;
				case 104:
					SESta[i + 1] = 65;
					break;
				case 67: //日比谷
					SESta[i + 1] = 105;
					break;
				case 105:
					SESta[i + 1] = 68;
					break;
				case 68: //霞ケ関
					SESta[i + 1] = 106;
					break;
				case 106:
					SESta[i + 1] = 69;
					break;
				case 71: //乃木坂
					SESta[i + 1] = 107;
					break;
				case 107:
					SESta[i + 1] = 72;
					break;
				case 73: //明治神宮
					SESta[i + 1] = 108;
					break;
				case 108: //公園側線
					SESta[i + 1] = 74;
					break;
				case 74: //代々木公園
					SESta[i + 1] = 31;
					break;
				case 49: //新百合ヶ丘
					SESta[i + 1] = SEArea == 1 ? 93 : 50;
					break;
				case 53: //町田
					SESta[i + 1] = 13;
					break;
				case 13: //相模大野
					SESta[i + 1] = SEArea == 2 ? 12 : 75;
					break;
				case 12:
				case 11:
				case 10:
				case 9:
				case 8:
				case 7:
				case 6:
				case 5:
				case 4:
				case 3:
				case 2:
					SESta[i + 1] = SESta[i] - 1;//江ノ島線通常
					break;
				case 1: //藤沢本町
					SESta[i + 1] = 91;
					break;
				default:
					SESta[i + 1] = SESta[i] + 1;//通常時
					break;
				}
			}
			else //営団B線、小田急上り
			{
				switch (SESta[i])
				{
				case 57: //綾瀬
					SESta[i + 1] = 101;
					break;
				case 101:
					SESta[i + 1] = 56;
					break;
				case 59: //町屋
					SESta[i + 1] = 102;
					break;
				case 102:
					SESta[i + 1] = 58;
					break;
				case 63: //湯島
					SESta[i + 1] = 103;
					break;
				case 103:
					SESta[i + 1] = 62;
					break;
				case 65: //大手町
					SESta[i + 1] = 104;
					break;
				case 104:
					SESta[i + 1] = 64;
					break;
				case 68: //霞ケ関
					SESta[i + 1] = 105;
					break;
				case 105:
					SESta[i + 1] = 67;
					break;
				case 69: //国会議事堂前
					SESta[i + 1] = 106;
					break;
				case 106:
					SESta[i + 1] = 68;
					break;
				case 72: //表参道
					SESta[i + 1] = 107;
					break;
				case 107:
					SESta[i + 1] = 71;
					break;
				case 74: //代々木公園
					SESta[i + 1] = 108;
					break;
				case 108: //公園側線
					SESta[i + 1] = 73;
					break;
				case 31: //代々木上原
					SESta[i + 1] = SEArea == 0 ? 74 : 30;
					break;
				case 93: //五月台
					SESta[i + 1] = 49;
					break;
				case 75: //小田急相模原
					SESta[i + 1] = 13;
					break;
				case 13: //相模大野
					SESta[i + 1] = 53;
					break;
				case 12:
				case 11:
				case 10:
				case 9:
				case 8:
				case 7:
				case 6:
				case 5:
				case 4:
				case 3:
				case 2:
				case 1:
					SESta[i + 1] = SESta[i] + 1;//江ノ島線通常
					break;
				case 91: //藤沢
					SESta[i + 1] = 1;
					break;
				default:
					SESta[i + 1] = SESta[i] - 1;//通常時
					break;
				}

			}
		}

		// 次駅を点滅させる
		if(speed * speed / TIMS_DECELERATION >= m_dist - 50 && m_dist > 0 && !m_blinking) // 速度照査 (パターン)
		{
			m_blinking = true;

			if(m_pushUpFlag == 1) // 停車駅のとき
			{
				m_pushUpFlag = 2;
			}
		}

		// 駅名表示の点滅
		NextBlink = m_pushUpFlag == 2 ? ((*Time % 750) / 375) : 1;
		//20190207暫定追加
		NextBlinkLamp = m_pushUpFlag == 2 ? 1 : 0;

		// 開通情報25mごと(0無表示、1未開通区間、2停止境界1、3停止境界2、4停止境界3、5停止境界4、6開通区間1、7開通区間2、8駅区間1、9駅区間2)
		for(int initPath2=0;initPath2<120;initPath2++)
		{
			// 開通区間の表示(青線)
			TrackPath2[initPath2] = 6;
		}

		// 開通情報の駅位置情報
		m_pathStaDist -= def2; // 残り距離を減算する

		if(m_pathStaDist > 0)
		{
			int pathStaDistfabsf25 = (int)fabsf(m_pathStaDist) / 25;

			// 駅区間の表示(太い青線)
			if(TrackPath2[pathStaDistfabsf25] >= 6){TrackPath2[pathStaDistfabsf25] = 8;}
			if(TrackPath2[pathStaDistfabsf25 - 1] >= 6){TrackPath2[pathStaDistfabsf25 - 1] = 8;}
			if(TrackPath2[pathStaDistfabsf25 - 2] >= 6){TrackPath2[pathStaDistfabsf25 - 2] = 9;}
			if(TrackPath2[pathStaDistfabsf25 - 3] >= 6){TrackPath2[pathStaDistfabsf25 - 3] = 8;}
			if(TrackPath2[pathStaDistfabsf25 - 4] >= 6){TrackPath2[pathStaDistfabsf25 - 4] = 8;}
			if(TrackPath2[pathStaDistfabsf25 - 5] >= 6){TrackPath2[pathStaDistfabsf25 - 5] = 9;}
			if(TrackPath2[pathStaDistfabsf25 - 6] >= 6){TrackPath2[pathStaDistfabsf25 - 6] = 8;}
			if(TrackPath2[pathStaDistfabsf25 - 7] >= 6){TrackPath2[pathStaDistfabsf25 - 7] = 8;}
			if(TrackPath2[pathStaDistfabsf25 - 8] >= 6){TrackPath2[pathStaDistfabsf25 - 8] = 8;}
		}

		// 開通情報の閉そく情報
		for(int i=0;i<10;i++)
		{
			// 残り距離を減算する
			m_pathSecDist[i] -= def2;
			m_passSecLine[i] -= def2;

			// 閉そく区切り線の表示
			if(m_passSecLine[i] > 0)
			{
				int passSecLinefabsf25 = (int)fabsf(m_passSecLine[i]) / 25; // 区切り線を入れたい地点

				if(TrackPath2[passSecLinefabsf25] == 6){TrackPath2[passSecLinefabsf25] = 7;} // 開通区間1なら区切り線を入れる
			}

			// 走行限界の表示
			if(m_pathSecDist[i] > 0)
			{
				int pathSecDistfabsf25 = (int)fabsf(m_pathSecDist[i]) / 25; // 走行できる限界の地点
				TrackPath2[pathSecDistfabsf25] = 5; // 停止限界の表示(赤い三角形)
				TrackPath2[pathSecDistfabsf25 + 1] = 4; // 停止限界の表示(赤い三角形)
				TrackPath2[pathSecDistfabsf25 + 2] = 3; // 停止限界の表示(赤い三角形)
				TrackPath2[pathSecDistfabsf25 + 3] = 2; // 停止限界の表示(赤い三角形)

				for(int pathNone2=pathSecDistfabsf25+4;pathNone2<120;pathNone2++)
				{
					TrackPath2[pathNone2] = 1; // 未開通区間の表示(白線)
				}
			}
		}

		// グラスコックピット
		m_tmrUpdate -= abs(*DeltaT); // 毎フレーム減算
		if(m_tmrUpdate < 0) // || *TrainSpeed == 0) // 更新する
		{
			Speed = fabs(*TrainSpeed);
			Speed100 = fabs(*TrainSpeed) / 100;
			Speed10 = (fabs(*TrainSpeed) / 10) - (Speed100 * 10);
			Speed1 = fabs(*TrainSpeed) - (Speed100 * 100 + Speed10 * 10);

			if(*TrainSpeed < 1000 && Speed100 == 0){Speed100 = 10;}
			if(*TrainSpeed < 100 && Speed10 == 0){Speed10 = 10;}

			BrakeDelay = *BrakeNotch; // ブレーキ指令
			AccelDelay = *PowerNotch; // 力行指令
			BcCaution = BcPressure < 200 && !m_door; // 200kPa警告

			BcPress0 = 10; // ブレーキシリンダ指針(0-180kPa)
			BcPress1 = 10; // ブレーキシリンダ指針(200-380kPa)
			BcPress2 = 10; // ブレーキシリンダ指針(400-580kPa)
			BcPress3 = 10; // ブレーキシリンダ指針(600-780kPa)
			MrPress0 = 10; // 元空気ダメ指針(700-790kPa)
			MrPress1 = 10; // 元空気ダメ指針(800-890kPa)
			MrPress2 = 10; // 元空気ダメ指針(900-990kPa)

			if(BcPressure < 200){BcPress0 = BcPressure / 20;} // ブレーキシリンダ指針(0-180kPa)
			else if(BcPressure < 400){BcPress1 = (BcPressure - 200) / 20;} // ブレーキシリンダ指針(200-380kPa)
			else if(BcPressure < 600){BcPress2 = (BcPressure - 400) / 20;} // ブレーキシリンダ指針(400-580kPa)
			else if(BcPressure < 800){BcPress3 = (BcPressure - 600) / 20;} // ブレーキシリンダ指針(600-780kPa)

			if(MrPressure <= 750){MrPress0 = 10;} // 750kPa以下
			else if(MrPressure > 750 && MrPressure < 795){MrPress0 = (MrPressure - 750) / 5;} // 元空気ダメ指針(750-795kPa)
			else if(MrPressure < 845){MrPress1 = (MrPressure - 800) / 5;} // 元空気ダメ指針(800-845kPa)
			else if(MrPressure < 895){MrPress2 = (MrPressure - 850) / 5;} // 元空気ダメ指針(850-895kPa)

			//桁表示//
			
			BcPress4 = fabs(BcPressure) / 100;
			BcPress5 = (fabs(BcPressure) / 10) - (BcPress4 * 10);
			BcPress6 = fabs(BcPressure) - (BcPress4 * 100 + BcPress5 * 10);

			BcPress7= fabs(BcPressure);

			if(BcPressure < 1000 && BcPress4 == 0){BcPress4 = 10;}
			if(BcPressure < 100 && BcPress5 == 0){BcPress5 = 10;}

			MrPress4 = fabs(MrPressure) / 100;
			MrPress5 = (fabs(MrPressure) / 10) - (MrPress4 * 10);
			MrPress6 = fabs(MrPressure) - (MrPress4 * 100 + MrPress5 * 10);

			MrPress7 = fabs(MrPressure);

			if(BcPressure < 1000 && BcPress4 == 0){BcPress4 = 10;}
			if(BcPressure < 100 && BcPress5 == 0){BcPress5 = 10;}
			//桁表示ここまで

			Ammeter0 = fabs(Current); // 電流計

			Ammeter1 = fabs(Current) / 1000;
			Ammeter2 = (fabs(Current) / 100) - (Ammeter1 * 10);
			Ammeter3 = (fabs(Current) - (Ammeter1 * 1000 + Ammeter2 * 100)) / 10;
			Ammeter4 = (fabs(Current) - (Ammeter1 * 1000 + Ammeter2 * 100 + Ammeter3 * 10));

			if(Current < 1000 && Ammeter2 == 0){Ammeter2 = 10;}
			if(Current < 100 && Ammeter3 == 0){Ammeter3 = 10;}
			if(Current < 10 && Ammeter4 == 0){Ammeter4 = 10;}

			if(Current >= 0){AmmeterC = 10;}
			if(Current < 0){AmmeterC = 12;}

			m_tmrUpdate = 200 + (*Time % 50) * 5;
		}

		// 右モニター速度計
		// m_distance += (def2 / 10) * m_direction; // 毎フレーム減算
		m_tmrTims -= abs(*DeltaT); // 毎フレーム減算
		if(m_tmrTims < 0) // || *TrainSpeed == 0) // 更新する
		{
			// double temp_distance = m_distance + (fabs(StartingPoint - Location) * m_direction / 10);
			double temp_distance = ((Location / 10) - (StartingPoint / 10) * m_direction) + m_distance;
			
			Distance1000 = temp_distance / 100; // 走行距離(kmの桁)
			Distance100 = (temp_distance / 10) - (Distance1000 * 10); // 走行距離(100mの桁)
			Distance10 = temp_distance - (Distance1000 * 100 + Distance100 * 10); // 走行距離(10mの桁)
/*
			Distance1000 = fabs(m_distance) / 100; // 走行距離(kmの桁)
			Distance100 = (fabs(m_distance) / 10) - (Distance1000 * 10); // 走行距離(100mの桁)
			Distance10 = fabs(m_distance) - (Distance1000 * 100 + Distance100 * 10); // 走行距離(10mの桁)

			double tmp_location = Location + m_distDef; // 列車位置補正を適用する
			Location1000 = tmp_location / 1000; // 列車位置(kmの桁)
			Location100 = (tmp_location / 100) - (Location1000 * 10); // 列車位置(100mの桁)
			Location10 = (tmp_location / 10) - (Location1000 * 100 + Location100 * 10); // 列車位置(10mの桁)
*/
			if(temp_distance >= 100 && Distance100 == 0){Distance100 = 10;}
			if(temp_distance >= 10 && Distance10 == 0){Distance10 = 10;}

			TimsSpeed100 = fabs(*TrainSpeed) / 100;
			TimsSpeed10 = (fabs(*TrainSpeed) / 10) - (TimsSpeed100 * 10);
			TimsSpeed1 = fabs(*TrainSpeed) - (TimsSpeed100 * 100 + TimsSpeed10 * 10);

			if(*TrainSpeed >= 1000 && TimsSpeed100 == 0){TimsSpeed100 = 10;}
			if(*TrainSpeed >= 100 && TimsSpeed10 == 0){TimsSpeed10 = 10;}
			if(TimsSpeed1 == 0){TimsSpeed1 = 10;}

			m_tmrTims = 1200;
		}

		// ユニット表示
		for(int i=0;i<10;i++) // 状態の更新、ユニット状態を10回ループ
		{
			// TIMS用
			if(Current > 0 && UnitTims[i] != 1 && rand() % RANDOMIZE_UNITTIMS == 0){UnitTims[i] = 1;}
			else if(Current < 0 && UnitTims[i] != 2 && rand() % RANDOMIZE_UNITTIMS == 0){UnitTims[i] = 2;}
			else if(Current == 0 && UnitTims[i] != 0 && rand() % RANDOMIZE_UNITTIMS == 0){UnitTims[i] = 0;}

			// 表示灯用
			if(Current != 0 && UnitState[i] != 1 && rand() % RANDOMIZE_UNITLAMP == 0){UnitState[i] = 1;}
			else if(Current == 0 && UnitState[i] != 0 && rand() % RANDOMIZE_UNITLAMP == 0){UnitState[i] = 0;}
		}

		// 進行方向矢印
		switch(Direction)
		{
		case 0: //方向を0にした場合
			ArrowDirection = 0;
			break;
		case 1: //方向を1にした場合
			switch(*Reverser)
			{
			case 1: //レバーサが前進なら、1を返す
				ArrowDirection = 1;
				break;
			case -1: //レバーサが後退なら、2を返す
				ArrowDirection = 2;
				break;
			case 0: //レバーサが切なら、0を返す
			default:
				ArrowDirection = 0;
				break;
			}
			break;
		case 2:
			switch(*Reverser)
			{
			case 1: //レバーサが前進なら、2を返す
				ArrowDirection = 2;
				break;
			case -1: //レバーサが後退なら、1を返す
				ArrowDirection = 1;
				break;
			case 0: //レバーサが切なら、0を返す
			default:
				ArrowDirection = 0;
				break;
			}
			break;
		}

		// プッシュアップイベント
		if(*TrainSpeed != 0) // 駅ジャンプを除外する
		{
			if((m_pushUpFlag == 2 && (m_pushUpBeacon == 1 || *TrainSpeed < 2.0f)) || (m_pushUpFlag == -1 && m_dist <= 0))//通過時に距離程通過or停車時に速度2キロ以下
			{
				m_pushUpFlag = 0;

				// ステップ更新の回数だけループ
				for( ;m_pushUpCount>0;m_pushUpCount--)
				{
					if(m_pushUpCount > 1)
					{
						for(int i=0;i<10;i++)
						{
							PushUp(i);
						}
					}
					else
					{
						m_tmrVisible = *Time;
						for(int i=0;i<10;i++)
						{
							m_update[i] = 1;

							// 3回目の時、次駅を最新にする、採時駅を最新にする、列番を更新する
							if(i == 3)
							{
								PushNext();
								PushTimeStation();
								PushTrainNumber();
								PushSESta();
							}
						}
					}
				}
				//20180824暫定追加
				//ページめくりしない
				if(m_option > 0)
				{
					PushNext(); // 次駅を最新にする
					PushTimeStation(); //採時駅を最新にする
					PushTrainNumber(); //列番を更新する
				}
			}
		}
		else if(m_pushUpFlag == 3 || m_pushUpBeacon == 2) // 駅ジャンプのとき
		{
			m_pushUpFlag = 0;
			PushNext(); // 次駅を最新にする
			PushTimeStation(); //採時駅を最新にする
			PushTrainNumber(); //列番を更新する
			PushSESta();

			for(int i=0;i<10;i++)
			{
				PushUp(i);
			}
		}
		else if(m_pushUpFlag == -1) // 起動時の初期化
		{
			PushNext(); // 次駅を最新にする
			PushTimeStation(); //採時駅を最新にする
			PushTrainNumber(); //列番を更新する
			PushSESta();
		}

		// ステップ更新の処理
		for(int i=0;i<10;i++)
		{
			if(*Time >= m_tmrVisible + (i * (90 * (2 / 3))) && *Time <= m_tmrVisible + 90 * (i + 1) - (90 * (1 / 3)))
			{
				HiddenLine[i] = 1;

				if(m_update[i] == 1)
				{
					m_update[i] = 0;
					PushUp(i);
				}
			}
			else
			{
				HiddenLine[i] = 0;
			}
		}
	}

	// ドアが開いた
	void DoorOpening(void)
	{
		m_door = 0; // ドア状態
		m_tmrVisible = 0; // 表示の更新をさせない
	}

	// ドアが閉まった
	void DoorClosing(void)
	{
		m_door = 1; // ドア状態
	}

	// 次駅接近した
	//void Receive(int data) //20180824暫定撤去
	void Receive(int data,int option) //20180824暫定追加
	{
		m_pushUpFlag = data >= 0 ? 1 : -1;
		m_pushUpBeacon = 0;
		m_pushUpCount = abs(data) > 100000 ? 0 : abs(data) / 10000 > 0 ? abs(data) / 10000 : 1;//100000より多いとプッシュアップしない
		m_option = abs(option) > 0 ? 1 : 0; //20180824暫定追加

		m_dist = abs(data % 10000) - TIMS_OFFSET;
		m_blinking = false;

		m_pathStaDist = abs(data % 10000) + 5.0F;

		if(*TrainSpeed == 0) // 駅ジャンプを除外する
		{
			// m_pushUpFlag = abs(data) < 1000000 ? 3 : 0;
			m_pushUpFlag = abs(data) < 100000 ? 3 : 0;
			m_pushUpCount = 0;
			m_dist = 0;
			m_pathStaDist = 0;
		}
	}

	// 表示を更新する
	void UpdateView(void)
	{
		m_pushUpBeacon = *TrainSpeed != 0 ? 1 : 2; // 表示更新の地上子
	}

	// 自駅、次駅の設定
	void SetNext(int data)
	{
		m_thisName = data / 1000;
		m_nextName = data % 1000;
	}

	// 自駅、次駅の時刻設定
	void SetNextTime(int data)
	{
		m_nextTime[0] = data / 10000;
		m_nextTime[1] = ((data % 10000) - (data % 100)) / 100;
		m_nextTime[2] = (data % 100) / 5;	
	}

	// 次駅到着番線の設定
	void SetNextTrack(int data)
	{
		m_nextTrack = data;
	}

	// 列車種別の設定
	void SetKind(int data)
	{
		m_kind = data;
	}

	// 列車番号の設定
	void SetNumber(int data, int chara)
	{
		m_charactor = chara;//種別は下2桁
		
		if(data >= 1000) //列番が1000以上なら
		{
			//1000以上で1000位が0なら0を表示
			m_number[0] = (data / 1000) % 10 == 0 ? 10 : (data / 1000) % 10; //列番千位は1000位
		}
		else
		{
			m_number[0] = 0; //未満は無表示
		}

		if(data >= 100) //列番が100以上なら
		{
			//100以上で100位が0なら0を表示
			m_number[1] = (data / 100) % 10 == 0 ? 10 : (data / 100) % 10; //列番百位は100位
		}
		else
		{
			m_number[1] = 0; //未満は無表示
		}

		if(data >= 10) //列番が10以上なら
		{
			m_number[2] = (data / 10) % 10 == 0 ? 10 : (data / 10) % 10; //列番十位は10位
		}
		else
		{
			m_number[2] = 0; //未満は無表示
		}

		m_number[3] = data % 10 == 0 ? 10 : data % 10; //代入値が0なら10にする、他は1位

	}

	// 運行区間の設定
	void SetLeg(int data)
	{
		m_from = data / 1000;
		m_destination = data % 1000;
	}

	// 行先の設定
	void SetFor(int data)
	{
		m_for = data;
	}

	// 徐行区間の設定
	void SetCrawl(int data)
	{
		if(data > 0)
		{
			Crawl = 1;
			CrawlLimit = data / 5;
		}
		else
		{
			Crawl = 0;
			CrawlLimit = 0;
		}
	}

	// 次運用の設定
	void SetAfteruent(int type, int data, int chara)
	{
		if(type == 0) // 列車番号
		{
			/*
			AfterChara = data / 10000;
			AfterNumber[0] = ((data % 10000) - (data % 1000)) / 1000;
			AfterNumber[1] = ((data % 1000) - (data % 100)) / 100;
			AfterNumber[2] = ((data % 100) - (data % 10)) / 10;
			AfterNumber[3] = data % 10;
			*/
			m_afterChara = chara;
			
			if(data >= 1000) //列番が1000以上なら
			{
				//1000以上で1000位が0なら0を表示
				m_afterNumber[0] = (data / 1000) % 10 == 0 ? 10 : (data / 1000) % 10; //列番千位は1000位
			}
			else
			{
				m_afterNumber[0] = 0; //未満は無表示
			}

			if(data >= 100) //列番が100以上なら
			{
				//100以上で100位が0なら0を表示
				m_afterNumber[1] = (data / 100) % 10 == 0 ? 10 : (data / 100) % 10; //列番百位は100位
			}
			else
			{
				m_afterNumber[1] = 0; //未満は無表示
			}

			if(data >= 10) //列番が10以上なら
			{
				m_afterNumber[2] = (data / 10) % 10 == 0 ? 10 : (data / 10) % 10; //列番十位は10位
			}
			else
			{
				m_afterNumber[2] = 0; //未満は無表示
			}

			m_afterNumber[3] = data % 10 == 0 ? 10 : data % 10; //代入値が0なら10にする、他は1位
		}
		else if(type == 1) // 到着時刻
		{
			m_afterTime[0][0] = data / 10000;
			m_afterTime[0][1] = ((data % 10000) - (data % 100)) / 100;
			m_afterTime[0][2] = (data % 100) / 5;
		}
		else if(type == 2) // 発車時刻
		{
			m_afterTime[1][0] = data / 10000;
			m_afterTime[1][1] = ((data % 10000) - (data % 100)) / 100;
			m_afterTime[1][2] = (data % 100) / 5;
		}
		else if(type == 3) // 種別
		{
			m_afterKind = data;
		}
		
			
	}

	// 進行方向設定
	void SetDirection(int direction)
	{
		switch(direction)
		{
		case 1: // 左
			Direction = 1;
			break;
		case 2: // 右
			Direction = 2;
			break;
		case 0:
		default: // 例外
			Direction = 0;
			break;
		}
	}

	// 行路終点駅の設定
	void SetLastStop(int type, int data)
	{
		if(type == 0) // 到着時刻
		{
			m_lastStopTime[0] = data / 10000;
			m_lastStopTime[1] = ((data % 10000) - (data % 100)) / 100;
			m_lastStopTime[2] = (data % 100) / 5;
		}
		else if(type == 1) // 出発時刻
		{
			m_lastStopTimeL[0] = data / 10000;
			m_lastStopTimeL[1] = ((data % 10000) - (data % 100)) / 100;
			m_lastStopTimeL[2] = (data % 100) / 5;
		}
		else if(type == 2) // 到着番線
		{
			m_lastStopTrack = data;
		}
	}

	// 行路終点駅名設定
	void SetRelayStation(int data)
	{
		m_relayName = data;
	}

	// 採時駅時刻番線設定
	void SetTimeStationTime(int type, int data)
	{
		if(type == 0) // 直前採時駅時刻
		{
			m_beforeTime[0] = data / 10000;
			m_beforeTime[1] = ((data % 10000) - (data % 100)) / 100;
			m_beforeTime[2] = (data % 100) / 5;
		}
		else if(type == 1) // 次採時駅着時刻
		{
			m_afterTimeA[0] = data / 10000;
			m_afterTimeA[1] = ((data % 10000) - (data % 100)) / 100;
			m_afterTimeA[2] = (data % 100) / 5;
		}
		else if(type == 2) // 次採時駅発時刻
		{
			m_afterTimeL[0] = data / 10000;
			m_afterTimeL[1] = ((data % 10000) - (data % 100)) / 100;
			m_afterTimeL[2] = (data % 100) / 5;
		}
		else if(type == 3) // 直前採時駅到着番線
		{
			m_beforeTrack = data;
		}
		else if(type == 4) // 次採時駅到着番線
		{
			m_afterTrack = data;
		}
	}

	//採時駅駅名の設定
	void SetTimeStationName(int type, int data)
	{
		if(type == 0) // 直前採時駅設定
		{
			m_beforeName = data;
		}
		else if(type == 1) // 次採時駅設定
		{
			m_afterName = data;
		}
	}

	//社線用駅名の設定
	void SetSESta(int sta)
	{
		m_seSta = sta; //駅名データを入れる
	}

	//社線用駅名割込み
	void SetSEStaExtra(int data)
	{
		SESta[0] = data;
	}

	//社線用運転方向
	void SetSEDirection(int data, int area)
	{
		SEDirection = data == 0 ? -1 : 1; //上り電車では減算
		if (area >= 51)
			SEArea = 1;//下り多摩線・上り新宿行き
		if (area >= 31)
			SEArea = 2;//下り江ノ島線
		else
			SEArea = 0;//下り小田原線・上り千代田線
	}

	// 行路表矢印の設定
	void SetArrowState(int data)
	{
		TrainArrow = data;
	}

	// 走行距離の設定
	void SetDistance(float distance, int data)
	{
		StartingPoint = distance; // 距離計算の起点

		if(data >= 100000) // 距離減算するかどうか
		{
			m_direction = -1;
		}
		else 
		{
			m_direction = 1;
		}

		m_distance = data % 100000; // 走行距離
	}

	// 列車位置補正の設定
	void SetPositionDef(int data)
	{
		m_distDef = data; // 列車位置の補正
	}

	// スタフテーブルの書き込み
	void InputLine(int colum, int line, int data)
	{
		switch(colum) // 列の振り分け
		{
		case 0:
			Span[line][0] = data / 100;
			Span[line][1] = (data % 100) / 5;
			break;
		case 1:
			Station[line] = data;
			break;
		case 2:
			Arrive[line][0] = data / 10000;
			Arrive[line][1] = ((data % 10000) - (data % 100)) / 100;
			Arrive[line][2] = (data % 100) / 5;
			break;
		case 3:
			Leave[line][0] = data / 10000;
			Leave[line][1] = ((data % 10000) - (data % 100)) / 100;
			Leave[line][2] = (data % 100) / 5;
			break;
		case 4:
			Track[line] = data;
			break;
		case 5:
			Limit[line] = data / 5;
			break;
		case 6:
			Limit2[line] = data / 5;
			break;
		}
	}

	// 開通情報の停止閉そく情報を更新する
	void CheckPath(int signal, float dist=0, int section=0)
	{
		if((signal == 0 || signal == 9 || signal == 10) && dist > 0)
		{
			m_pathSecDist[section] = dist; // 開通情報の閉そく情報
			m_passSecLine[section] = 0; // 開通情報の閉そく区切り線
		}
		else
		{
			m_pathSecDist[section] = 0;
			m_passSecLine[section] = dist;
		}
	}

	// 非常ブレーキ緩解音を再生する
	void PlaySoundAirHigh(int state, int newState)
	{
		if(state == EmergencyNotch && newState < EmergencyNotch && BcPressure > 340)
		{
			m_airHigh = ATS_SOUND_PLAY;
			m_tmrBcPressCut = *Time + ini_cutPressTime; // ブレーキ減圧のタイマー
		}
	}

	// 非常ブレーキ放送を再生する
	void PlaySoundEmgAnnounce(int state, int newState)
	{
		if(state != EmergencyNotch && newState == EmergencyNotch && *TrainSpeed > 5.0F) //非常ブレーキ放送速度
		{
			m_emgAnnounce = ATS_SOUND_PLAY;
		}
	}

	// 初回起動時断流器音をリセットする
	void SetLbInit(int pos, int enable)
	{
		if(pos == 0 && enable == 1)
		{
			event_lbInit = 1;
		}
	}

	// ブレーキ昇圧音(非常)を再生する
	void PlaySoundAirApplyEmg(int state, int newState)
	{
		if(state != EmergencyNotch && newState == EmergencyNotch && BcPressure < 150.0f)
		{
			m_airApplyEmg = ATS_SOUND_PLAY;
		}
	}

	// 交直切替を行う
	void SetACDC(int state)
	{
		step_ACDC = state;
		tmr_ACDC = *Time;
	}

	// 交直切替のお知らせ
	void AlartACDC(int state)
	{
		alert_ACDC = state;
	}

	// ランダムシードを変更する
	void RandomizeSeed(void)
	{
		srand(*Time);
	}

private: //メモリーとして使うもの
	int m_door; // ドア状態
	float m_dist; // 停止予定点距離
	bool m_blinking; // 次駅点滅かどうか

	int m_pushUpFlag; // 表示更新のフラグ
	int m_pushUpBeacon; // 表示更新の地上子
	int m_pushUpCount; // 表示更新の繰り返し数
	int m_tmrVisible; // モニタのステップ更新
	int m_update[7]; // ステップ更新の状態
	int m_thisName; // 自駅名
	int m_nextName; // 次駅名
	int m_nextTime[3]; // 次駅時刻
	int m_nextTrack; // 次駅到着番線

	//採時駅用追加
	int m_beforeName; // 前採時駅名
	int m_afterName; // 次採時駅名
	int m_beforeTime[3]; //前採時駅時刻
	int m_afterTimeA[3]; //次採時駅着時刻
	int m_afterTimeL[3]; //次採時駅発時刻
	int m_beforeTrack; // 前採時駅番線
	int m_afterTrack; // 次採時駅番線
	int m_lastStopTime[3]; // 終点到着時刻(時 - 分 - 秒)
	int m_lastStopTimeL[3]; // 終点出発時刻(時 - 分 - 秒)
	int m_lastStopTrack; // 終点到着番線
	int m_relayName; // 行路終点駅名

	//20181014暫定追加
	int m_from; //運行パターン発駅
	int m_destination; //運行パターン着駅
	int	m_for; //列車行先

	//20180824暫定追加
	int m_option; //次駅と採時駅のみ更新する

	//20181014暫定追加
	int m_kind; // 列車種別(通過設定含む)
	int m_number[4]; // 列車番号(1000 - 100 - 10 - 1)
	int m_charactor; // 列車番号(記号)
	int m_afterNumber[4]; // 次運用列車番号(1000 - 100 - 10 - 1)
	int m_afterChara; // 次運用記号
	int m_afterTime[2][3]; // 次運用着発時刻
	int m_afterKind; //次運用種別

	float m_pathStaDist; // 開通情報の駅位置情報
	float m_pathSecDist[10]; // 開通情報の閉そく情報
	float m_passSecLine[10]; // 開通情報の閉そく区切り線

	double m_distance; // 走行距離
	int m_direction; // 距離減算するかどうか
	double m_distDef; // 列車位置の補正

	int m_tmrUpdate; // 速度計の更新タイマー
	int m_tmrTims; // 右モニターの更新タイマー
	int m_tmrBcPressCut; // ブレーキ減圧のタイマー

	int m_relayD; // 走行検知リレー
	int m_lbInit; // 初回起動時断流器音
	int m_airHigh; // 非常ブレーキ緩解音
	int m_airApply; // ブレーキ昇圧音
	float m_bcPrsDelta; // ブレーキシリンダ圧力[kPa]
	int m_emgAnnounce; // 非常ブレーキ放送
	int m_airApplyEmg; // ブレーキ昇圧音(非常)
	int m_updateInfo; // 運行情報更新
	int m_deltaUpdate; // 前回運行情報更新

	//社線用
	int m_seSta;

	// スタフテーブルのプッシュアップ
	void PushUp(int line)
	{
		if(line == 9) //10行目の時刻以外リセット
		{
			Station[line] = 0;
			Track[line] = 0;
			Limit[line] = 0;
			Limit2[line] = 0;
			Span[line][0] = 0;
			Span[line][1] = 0;
		}
		else //1から9行目の繰り上げ
		{
			Station[line] = Station[line + 1];
			Track[line] = Track[line + 1];
			Limit[line] = Limit[line + 1];
			Limit2[line] = Limit2[line + 1];
			Span[line][0] = Span[line + 1][0];
			Span[line][1] = Span[line + 1][1];
		}

		for(int j=0;j<3;j++) //着発時刻のリセット
		{
			if(line == 9) //10行目を空にする
			{
				switch(j)
				{
				case 0:
					Arrive[line][j] = 0;
					Leave[line][j] = 0;
					break;
				case 1:
					Arrive[line][j] = 0;
					Leave[line][j] = 0;
					break;
				case 2:
					Arrive[line][j] = 0;
					Leave[line][j] = 0;
					break;
				}
				
			}
			else //1から9行目は繰り上げ
			{
				Arrive[line][j] = Arrive[line + 1][j];
				Leave[line][j] = Leave[line + 1][j];
			}
		}
	}

	//社線用駅名を更新
	void PushSESta(void)
	{
		SESta[0] = m_seSta;
	}



	// 入力されている次駅を画面に反映
	void PushNext(void)
	{
		This = m_thisName;
		Next = m_nextName;
		NextTime[0] = m_nextTime[0];
		NextTime[1] = m_nextTime[1];
		NextTime[2] = m_nextTime[2];
		NextTrack = m_nextTrack;
	}

	// 入力されている採時駅を画面に反映
	void PushTimeStation(void)
	{
		Before = m_beforeName; //直前採時駅
		After = m_afterName; //次採時駅
		RelayName = m_relayName; //降車駅

		BeforeStationTime[0] = m_beforeTime[0];
		BeforeStationTime[1] = m_beforeTime[1];
		BeforeStationTime[2] = m_beforeTime[2];

		AfterStationTimeA[0] = m_afterTimeA[0];
		AfterStationTimeA[1] = m_afterTimeA[1];
		AfterStationTimeA[2] = m_afterTimeA[2];

		AfterStationTimeL[0] = m_afterTimeL[0];
		AfterStationTimeL[1] = m_afterTimeL[1];
		AfterStationTimeL[2] = m_afterTimeL[2];

		LastStopTime[0] = m_lastStopTime[0];
		LastStopTime[1] = m_lastStopTime[1];
		LastStopTime[2] = m_lastStopTime[2];

		LastStopTimeL[0] =  m_lastStopTimeL[0];
		LastStopTimeL[1] =  m_lastStopTimeL[1];
		LastStopTimeL[2] =  m_lastStopTimeL[2];

		BeforeTrack = m_beforeTrack;
		AfterTrack = m_afterTrack;
		LastStopTrack = m_lastStopTrack;

		//20181018暫定追加
		From = m_from; //運行パターン発駅
		Destination = m_destination; //運行パターン着駅
		For = m_for; //列車行先

	}

	//20181018暫定追加
	//入力されている列車番号と種別を画面に反映
	void PushTrainNumber(void)
	{
		//現在種別列番
		Kind = m_kind % 100;
		PassMode = (m_kind >= 100) ? (m_kind / 100) : 0; // 通過設定

		Number[0] = m_number[0];
		Number[1] = m_number[1];
		Number[2] = m_number[2];
		Number[3] = m_number[3];
		Charactor = m_charactor;
	
		//次行路列番
		AfterKind = m_afterKind;
		AfterNumber[0] = m_afterNumber[0];
		AfterNumber[1] = m_afterNumber[1];
		AfterNumber[2] = m_afterNumber[2];
		AfterNumber[3] = m_afterNumber[3];
		AfterChara = m_afterChara;

		//次行路発着時刻
		AfterTime[0][0] = m_afterTime[0][0];
		AfterTime[0][1] = m_afterTime[0][1];
		AfterTime[0][2] = m_afterTime[0][2];

		AfterTime[1][0] = m_afterTime[1][0];
		AfterTime[1][1] = m_afterTime[1][1];
		AfterTime[1][2] = m_afterTime[1][2];
	}

	// 配列の初期化
	void ResetArray(void)
	{
		for(int c3=0;c3<3;c3++) // 3回ループ、乗務終了駅のリセット
		{
			LastStopTime[c3] = 0;
			LastStopTimeL[c3] = 0;
			m_nextTime[c3] = 0;

			for(int c3_2=0;c3_2<2;c3_2++) // 2回ループ、次行路時刻のリセット
			{
				AfterTime[c3_2][c3] = 0;
			}
		}

		for(int st3=0;st3<3;st3++) // 3回ループ、採時駅のリセット
		{
			BeforeStationTime[st3] = 0;
			AfterStationTimeA[st3] = 0;
			AfterStationTimeL[st3] = 0;
			m_beforeTime[st3] = 0;
			m_afterTimeA[st3] = 0;
			m_afterTimeL[st3] = 0;
		}

		for(int c4=0;c4<4;c4++) // 4回ループ、列番のリセット
		{
			Number[c4] = 10;
			AfterNumber[c4] = 10;
		}

		for(int c10=0;c10<10;c10++) // 10回ループ、時刻以外のスタフテーブルのリセット
		{
			Station[c10] = 0;
			PassSta[c10] = 0;
			Track[c10] = 0;
			Limit[c10] = 0;
			Limit2[c10] = 0;

			HiddenLine[c10] = 0;
			m_update[c10] = 0;

			TrackPath[c10] = 0;
			UnitTims[c10] = 0;
			UnitState[c10] = 0;
			m_pathSecDist[c10] = 0;
			m_passSecLine[10] = 0;

			// 時刻テーブルには 00:00:00 (0, 0, 0) を出力
			for(int c10_3=0;c10_3<3;c10_3++) // 10回それそれに3回ずつループ
			{
				switch(c10_3)
				{
				case 0:
					Arrive[c10][c10_3] = 0;
					Leave[c10][c10_3] = 0;
					Span[c10][c10_3] = 0;
					break;
				case 1:
					Arrive[c10][c10_3] = 0;
					Leave[c10][c10_3] = 0;
					Span[c10][c10_3] = 0;
					break;
				case 2:
					Arrive[c10][c10_3] = 0;
					Leave[c10][c10_3] = 0;
					break;
				}
			}
		}

		for(int c120=0;c120<120;c120++) // 120回ループ、開通情報のリセット
		{
			TrackPath2[c120] = 0;
		}
	}
};	// CTims

class CSpp
{
private:
	float m_dist; // 停止予定点距離
	bool m_played;
	bool m_halt;
	int m_haltChime;
	int m_haltChime2;
	int m_haltChime3;
	int m_passAlarm;

public:
	float *TrainSpeed; // 列車速度[km/h]
	int *DeltaT; // フレーム時間
	int *BrakeNotch; // 現在のブレーキノッチ

	int HaltChime; // 停車チャイム
	int HaltChime2; // 停車チャイムループ
	int HaltChime3; // 停車チャイムループ(減少しない)
	int PassAlarm; // 通過案内音声
	int ServiceNotch; // ATS確認ノッチ

	void RunSpp(void)
	{
		float speed = fabsf(*TrainSpeed); // 速度の絶対値[km/h]
		float def = speed / 3600 * *DeltaT; // 1フレームで動いた距離(絶対値)[m]
		m_dist -= def; // 残り距離を減算する

		// 警報を鳴らす
		if(speed * speed / SPP_DECELERATION >= m_dist - 50 && m_dist > 0 && !m_played) // 速度照査 (パターン)
		{
			m_played = true;
			if(m_halt) // 停車の場合
			{
// #if(SPP_ALARMTYPE) // 1回発音型
			m_haltChime = ATS_SOUND_PLAY;
// #else              // 繰り返し発音型
			m_haltChime3 = ATS_SOUND_PLAYLOOPING;
			if(*BrakeNotch >= ServiceNotch)
			{
				m_haltChime2 = -1200; // ブレーキを込めた場合は音量を小さくする (-12dB)
			}
			else
			{
				m_haltChime2 = 0;
			}
// #endif
			}
			else // 通過の場合
			{
				m_passAlarm = ATS_SOUND_PLAY;
			}
		}

		// サウンドを更新
		HaltChime = m_haltChime;
		HaltChime2 = m_haltChime2;
		HaltChime3 = m_haltChime3;
		PassAlarm = m_passAlarm;

		// CONTINUEにしておく
		if(m_haltChime == ATS_SOUND_PLAY){m_haltChime = ATS_SOUND_CONTINUE;}
		if(m_haltChime2 == ATS_SOUND_PLAY){m_haltChime2 = ATS_SOUND_CONTINUE;}
		if(m_haltChime3 == ATS_SOUND_PLAYLOOPING){m_haltChime3 = ATS_SOUND_CONTINUE;}
		m_passAlarm = ATS_SOUND_CONTINUE;
	}

	void InitSpp(void)
	{
		m_dist = 0;
		m_haltChime = ATS_SOUND_STOP;
		m_haltChime2 = ATS_SOUND_STOP;
		m_haltChime3 = ATS_SOUND_STOP;
	}

	void Receive(int data)
	{
		m_halt = data >= 0;
		m_dist = abs(data) - SPP_OFFSET;
		m_played = false;
		m_haltChime = ATS_SOUND_STOP;
		m_haltChime2 = ATS_SOUND_STOP;
		m_haltChime3 = ATS_SOUND_STOP;
	}

	void NotchChanged(void)
	{
		// ブレーキを込めた場合は音量を小さくする (-12dB)
		if(m_haltChime2 == 0 && *BrakeNotch >= ServiceNotch){m_haltChime2 = -1200;}
	}

	void StopChime(void)
	{
		m_haltChime = ATS_SOUND_STOP;
		m_haltChime2 = ATS_SOUND_STOP;
		m_haltChime3 = ATS_SOUND_STOP;
	}
};	// CSpp

class CDate
{
public:
	// 時間変数・構造体
	time_t t;
	struct tm *status;

	// 出力
	int Cooler; // 冷房
	int CoolerSound; // 冷房音

	// 初期化する
	void initialize(void)
	{
		m_month = 0; // 月

		Cooler = 0; // 冷房
		CoolerSound = ATS_SOUND_STOP; // 冷房音

		update();
	}

	// 日付を更新する
	void update(void)
	{
		time(&t);
		status = localtime(&t);
		m_month = status->tm_mon + 1;

		// 冷房の制御
		if(Cooler == 0)
		{
			Cooler = (m_month >= 6 && m_month <= 9);
		}
	}

private:
	int m_month; // 月(1～12)
};	// CDate
