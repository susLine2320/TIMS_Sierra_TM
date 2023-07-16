//Meter.h
//���̃t�@�C���ł̓��[�^�[�\������V�~�����[�g���܂�

//�����l��`�i���[�^�[�\����j


//���[�^�[�\����
class Meter
{
private:
	int m_timerUpdate; //���x�v�̍X�V�^�C�}�[
public:
	float BcPressure; //BC����[kPa]
	float MrPressure; //MR����[kPa]
	float Current; //�d��[A]
	double Location; //��Ԉʒu[m]

	int BrakeDelay; //�u���[�L�w��
	int AccelDelay; //�͍s�w��
	int BcCaution; //200kPa�x��
	int Speed; //���x�v
	int SpeedD[3]; //�f�W�^�����x�v
	int BCMeter[4]; //BC���[�^
	int BCPress; //BC
	int BCPressD[3]; //�f�W�^��BC
	int MRMeter[6]; //MR���[�^
	int MRPress; //MR
	int MRPressD[3]; //�f�W�^��MR
	int AMMeter[2]; //AM���[�^
	int AMMeterA[2]; //�d���v
	int AMMeterD[5]; //�f�W�^���d���v

	int AccelCutting; //�͍s�x��

	void Load(void) //������
	{
		BrakeDelay = 0;
		AccelDelay = 0;
		BcCaution = 0;
		Speed = 0;
		SpeedD[0] = 10;
		SpeedD[1] = 10;
		SpeedD[2] = 0;
		BCMeter[0] = 10;
		BCMeter[1] = 10;
		BCMeter[2] = 10;
		BCMeter[3] = 10;
		BCPress = 0;
		BCPressD[0] = 10;
		BCPressD[1] = 10;
		BCPressD[2] = 0;
		MRMeter[0] = 10;
		MRMeter[1] = 10;
		MRMeter[2] = 10;
		MRMeter[3] = 10;
		MRMeter[4] = 10;
		MRMeter[5] = 10;
		MRPress = 0;
		MRPressD[0] = 10;
		MRPressD[1] = 10;
		MRPressD[2] = 0;
		AMMeter[0] = 0;
		AMMeter[1] = 0;
		AMMeterA[0] = 0;
		AMMeterA[1] = 0;
		AMMeterD[0] = 0;
		AMMeterD[1] = 0;
		AMMeterD[2] = 0;
		AMMeterD[3] = 0;
		AMMeterD[4] = 10;
	}

	void Execute(void) //���t���[�����s
	{
		//�O���X�R�b�N�s�b�g
		m_timerUpdate -= abs(g_deltaT); //���t���[�����Z
		if (m_timerUpdate < 0)
		{
			//SPEED
			Speed = fabs(g_speed);
			SpeedD[0] = fabs(g_speed) / 100;
			SpeedD[1] = (fabs(g_speed) / 10) - (SpeedD[0] * 10);
			SpeedD[2] = fabs(g_speed) - (SpeedD[0] * 100 + SpeedD[1] * 10);
			if (fabs(g_speed) < 1000 && SpeedD[0] == 0) { SpeedD[0] = 10; }
			if (fabs(g_speed) < 100 && SpeedD[1] == 0) { SpeedD[1] = 10; }

			BrakeDelay = g_brakeNotch;//�u���[�L�w��
			AccelDelay = g_powerNotch;//�͍s�w��
			BcCaution = BcPressure < 200 && !g_pilotLamp;

			//������
			BCMeter[0] = 10;
			BCMeter[1] = 10;
			BCMeter[2] = 10;
			BCMeter[3] = 10;
			MRMeter[0] = 10;
			MRMeter[1] = 10;
			MRMeter[2] = 10;
			MRMeter[3] = 10;
			MRMeter[4] = 10;
			MRMeter[5] = 10;

			//BC
			if (BcPressure < 200) { BCMeter[0] = BcPressure / 20; }
			else if (BcPressure < 400) { BCMeter[1] = (BcPressure - 200) / 20; }
			else if (BcPressure < 600) { BCMeter[2] = (BcPressure - 400) / 20; }
			else if (BcPressure < 800) { BCMeter[3] = (BcPressure - 600) / 20; }

			BCPress = fabs(BcPressure);
			BCPressD[0] = fabs(BcPressure) / 100;
			BCPressD[1] = (fabs(BcPressure) / 10) - (BCPressD[0] * 10);
			BCPressD[2] = fabs(BcPressure) - (BCPressD[0] * 100 + BCPressD[1] * 10);
			if (BcPressure < 1000 && BCPressD[0] == 0) { BCPressD[0] = 10; }
			if (BcPressure < 100 && BCPressD[1] == 0) { BCPressD[1] = 10; }

			//MR
			if (MrPressure > 750 && MrPressure < 795) { MRMeter[0] = (MrPressure - 750) / 5; }
			else if (MrPressure > 750 && MrPressure < 845) { MRMeter[1] = (MrPressure - 800) / 5; }
			else if (MrPressure > 750 && MrPressure < 895) { MRMeter[2] = (MrPressure - 850) / 5; }
			if (MrPressure > 700 && MrPressure < 790) { MRMeter[3] = (MrPressure - 700) / 10; }
			else if (MrPressure > 700 && MrPressure < 890) { MRMeter[4] = (MrPressure - 800) / 10; }
			else if (MrPressure > 700 && MrPressure < 990) { MRMeter[5] = (MrPressure - 900) / 10; }

			MRPress = fabs(MrPressure);
			MRPressD[0] = fabs(MrPressure) / 100;
			MRPressD[1] = (fabs(MrPressure) / 10) - (MRPressD[0] * 10);
			MRPressD[2] = fabs(MrPressure) - (MRPressD[0] * 100 + MRPressD[1] * 10);
			if (MrPressure < 1000 && MRPressD[0] == 0) { MRPressD[0] = 10; }
			if (MrPressure < 100 && MRPressD[1] == 0) { MRPressD[1] = 10; }

			//AM
			AMMeter[0] = Current > 0 ? Current / 50 : 0;
			AMMeter[1] = Current < 0 ? Current / 50 * (-1) : 0;

			AMMeterA[0] = Current;
			AMMeterA[1] = fabs(Current);
			AMMeterD[0] = fabs(Current) / 1000;
			AMMeterD[1] = (fabs(Current) / 100) - (AMMeterD[0] * 10);
			AMMeterD[2] = (fabs(Current) - (AMMeterD[0] * 1000 + AMMeterD[1] * 100)) / 10;
			AMMeterD[3] = fabs(Current) - (AMMeterD[0] * 1000 + AMMeterD[1] * 100 + AMMeterD[2] * 10);
			if (fabs(Current) < 1000 && AMMeterD[0] == 0) { AMMeterD[0] = 10; }
			if (fabs(Current) < 100 && AMMeterD[1] == 0) { AMMeterD[1] = 10; }
			if (fabs(Current) < 10 && AMMeterD[2] == 0) { AMMeterD[2] = 10; }
			AMMeterD[4] = Current >= 0 ? 10 : 12;

			m_timerUpdate = 200 + (g_time % 50) * 5;
		}

	}
};
