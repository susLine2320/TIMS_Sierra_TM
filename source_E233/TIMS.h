//TIMS.h
//���̃t�@�C���ł�TIMS���u���V�~�����[�g���܂�

//�����l��`�iTIMS�\����j
#define TIMS_DECELERATION 19.5F //�����萔�i�����x[km/h/s] * 7.2�j
#define TIMS_OFFSET 0.5F //�ԏ�q�I�t�Z�b�g

#define RANDOMIZE_UNITTIMS 30 //TIMS���j�b�g�\���̍X�V�m��
#define RANDOMIZE_UNITLAMP 10 //���j�b�g�\�����̍X�V�m��
#define UPDATE_SPAN 180000 //�^�s���X�V�̍Œ�Ԋu
#define RELAY_REQUIREMENT 7.5f //���s���m�����[���삷�鑬�x

//TIMS�\����
class TIMS
{
public:
	//�ėp�I�Ȓl
	/*
	int emgNotch; //���u���[�L
	int *brakeNotch; //�u���[�L�m�b�`
	int *powerNotch; //�͍s�m�b�`
	int *reverser; //���o�[�T�[
	*/
	float BcPressure; //BC����[kPa]
	float MrPressure; //MR����[kPa]
	float Current; //�d��[A]
	double Location; //��Ԉʒu[m]

	//TIMS�\����
	int Kind; //��Ԏ��
	int Number[4]; //��Ԕԍ�
	int Charactor; //��ԋL��
	int From; //�n���w
	int For; //��ԍs��
	int Destination; //�s��w
	int This; //���w
	int Next; //���w
	int NextBlink; //���w�i�_�Łj
	int NextBlinkLamp; //���w��ԕ\����
	int NextTime[3]; //���w��������
	int NextTrack; //���w�����Ԑ�
	int Crawl; //���s���
	int CrawlLimit; //���s���x
	int AfterNumber[4]; //���^�p��Ԕԍ�
	int AfterChara; //���^�p��ԋL��
	int AfterTime[2][3]; //���^�p��������
	int AfterKind; //���^�p��Ԏ��
	int TrainArrow; //�s�H�\���
	int Station[10]; //�w���\��
	int SESta[10]; //�А��w��
	int DispSESta[10];
	int SEDirection; //�А��i�s����
	int SEArea; //�А��p���s����
	int PassSta[10]; //�ʉ߉w���\��
	int Arrive[10][3]; //��������
	int Leave[10][3]; //���Ԏ���
	int Track[10]; //�����Ԑ�
	int LimitA[10]; //�ō����x�i��j
	int LimitL[10]; //�ō����x�i���j
	int Span[10][2]; //�w�ԑ��s����
	int Before; //���O�̎��w
	int BeforeTime[3]; //���O�̎��w��������
	int BeforeTrack; //���O�̎��w�Ԑ�
	int After; //���̎��w
	int AfterTimeA[3]; //���̎��w��������
	int AfterTimeL[3]; //���̎��w���Ԏ���
	int AfterTrack; //���̎��w�Ԑ�
	int Last; //�~�ԉw
	int LastTimeA[3]; //�~�ԉw��������
	int LastTimeL[3]; //�~�ԉw���Ԏ���
	int LastTrack; //�~�ԉw�Ԑ�
	int HiddenLine[10]; //�X�V����1�s����\���ɂ���
	float StartingPoint; //�����v�Z�̋N�_
	int Distance[3]; //���s����
	int TimsSpeed[3]; //TIMS���x
	int UnitTims[10]; //TIMS���j�b�g�\��
	int UnitState[10]; //���j�b�g�\����

	int Direction; //�i�s����
	int ArrowDirection; // �i�s�������
	int PassMode; //�ʉߐݒ�

	void Load(void) //������
	{
		Kind = 0;
		Charactor = 0;
		From = 0;
		For = 0;
		Destination = 0;
		This = 0;
		Next = 0;
		NextBlink = 0;
		NextBlinkLamp = 0;
		NextTrack = 0;
		Crawl = 0;
		CrawlLimit = 0;
		AfterChara = 0;
		Last = 0;
		LastTrack = 0;
		TrainArrow = 0;

		m_dist = 0;

		m_pushUpFlag = -1;
		m_pushUpBeacon = 0;
		m_pushUpCount = 0;
		m_tmrVisible = g_time;

		m_thisName = 0;
		m_nextName = 0;
		m_nextTrack = 0;
		m_distance = 0;
		m_direction = 1;
		m_distDef = 0;

		ResetArray();

		StartingPoint = 0.0f;
		Distance[0] = 0;
		Distance[1] = 10;
		Distance[2] = 10;
		TimsSpeed[0] = 0;
		TimsSpeed[1] = 0;
		TimsSpeed[2] = 10;
	}

	void Execute(void) //���t���[�����s
	{
		float speed = fabsf(g_speed); //���x�̐�Βl[km/h]
		float def = speed / 3600 * g_deltaT; //1�t���[���œ����������i��Βl�j[m]
		float def2 = g_speed / 3600 * g_deltaT; //1�t���[���œ����������i���Βl�j[m]
		m_dist -= def; //�c�苗�������Z����

		//TIMS�\���탁�[�^�[
		m_timerTims -= abs(g_deltaT); //���t���[�����Z
		if (m_timerTims < 0)
		{
			double temp_distance = ((Location / 10) - (StartingPoint / 10)) * m_direction + m_distance + (m_distDef / 10);

			//�L����
			Distance[0] = temp_distance / 100;
			Distance[1] = (temp_distance / 10) - (Distance[0] * 10);
			Distance[2] = temp_distance - (Distance[0] * 100 + Distance[1] * 10);
			if (temp_distance >= 100 && Distance[0] == 0) { Distance[0] = 10; }
			if (temp_distance >= 10 && Distance[1] == 0) { Distance[1] = 10; }

			//���x�v
			TimsSpeed[0] = fabs(g_speed) / 100;
			TimsSpeed[1] = (fabs(g_speed) / 10) - (TimsSpeed[0] * 10);
			TimsSpeed[2] = fabs(g_speed) - (TimsSpeed[0] * 100 + TimsSpeed[1] * 10);
			if (fabs(g_speed) >= 1000 && TimsSpeed[0] == 0) { TimsSpeed[0] = 10; }
			if (fabs(g_speed) >= 100 && TimsSpeed[1] == 0) { TimsSpeed[1] = 10; }
			if (TimsSpeed[2] == 0) { TimsSpeed[2] = 10; }

			m_timerTims = 1200;
		}

		//���j�b�g�\��
		for (int i = 0; i < 10; i++) //��Ԃ̍X�V�A���j�b�g��Ԃ�10�񃋁[�v
		{
			//TIMS���j�b�g�\��
			if (Current > 0 && UnitTims[i] != 1 && rand() % RANDOMIZE_UNITTIMS == 0) { UnitTims[i] = 1; }
			else if (Current < 0 && UnitTims[i] != 2 && rand() % RANDOMIZE_UNITTIMS == 0) { UnitTims[i] = 2; }
			else if (Current == 0 && UnitTims[i] != 0 && rand() % RANDOMIZE_UNITTIMS == 0) { UnitTims[i] = 0; }

			//���j�b�g�\����
			if (Current != 0 && UnitState[i] != 1 && rand() % RANDOMIZE_UNITLAMP == 0) { UnitState[i] = 1; }
			else if (Current == 0 && UnitState[i] != 0 && rand() % RANDOMIZE_UNITLAMP == 0) { UnitState[i] = 2; }
		}

		//�i�s�������
		switch (Direction)
		{
		case 0:
		default:
			ArrowDirection = 0;
			break;
		case 1:
			switch (g_reverser)
			{
			case 1:
				ArrowDirection = 1;
				break;
			case -1:
				ArrowDirection = 2;
				break;
			case 0:
			default:
				ArrowDirection = 0;
				break;
			}
			break;
		case 2:
			switch (g_reverser)
			{
			case 1:
				ArrowDirection = 2;
				break;
			case -1:
				ArrowDirection = 1;
				break;
			case 0:
			default:
				ArrowDirection = 0;
				break;
			}
			break;
		}

		//[TIMS9N�̂�]�w���̑}��
		for (int i = 0; i < 7; i++)
		{
			if (SEDirection == 1) //�c�cA���E���c�}����
			{
				SESta[i + 1] = g_9n.SetSEStaA(SESta[i]);
			}
			else //�c�cB���E���c�}���
			{
				SESta[i + 1] = g_9n.SetSEStaB(SESta[i]);
			}
			if (i <= 4)
			{
				DispSESta[i] = SESta[i];
			}
		}

		g_9n.NowSta = -1;//1�i��

		//�~�ԉw��SESta[0]�`SESta[3]�ƈ�v����ꍇ
		for (int i = 0; i < 5; i++)
		{
			if (SESta[i] == g_9n.ConvDest2Sta(g_9n.ArrivalSta))
			{
				g_9n.NowSta = 4 - i; //�i��������
				if (i < 4)
				{
					DispSESta[4] = SESta[i]; //�����I��5�w�ڂƂ���
					for (int j = 4; j > 0; j--)//4�`0�̉w�������Ă͂�
					{
						if (SEDirection == 1) //�c�cA���E���c�}����
						{
							DispSESta[j - 1] = g_9n.SetSEStaB(DispSESta[j]);
						}
						else //�c�cB���E���c�}���
						{
							DispSESta[j - 1] = g_9n.SetSEStaA(DispSESta[j]);
						}
					}
					for (int j = 4; j < 7; j++)//5�`7�̉w�������Ă͂�
					{
						if (SEDirection == 1) //�c�cA���E���c�}����
						{
							DispSESta[j + 1] = g_9n.SetSEStaA(DispSESta[j]);
						}
						else //�c�cB���E���c�}���
						{
							DispSESta[j + 1] = g_9n.SetSEStaB(DispSESta[j]);
						}
					}
				}
				break;
			}
		}

		//���w��_�ł�����
		if (speed * speed / TIMS_DECELERATION >= m_dist - 50 && m_dist > 0 && !m_blinking) //���x�ƍ��i�p�^�[��)
		{
			m_blinking = true;

			if (m_pushUpFlag == 1) //��ԉw�̂Ƃ�
			{
				m_pushUpFlag = 2; //2 = ��2�L����������X�V
			}
		}

		//�w���\���̓_��
		NextBlink = m_pushUpFlag == 2 ? ((g_time % 750) / 375) : 1; //ini�ł������悤�ɂ���
		NextBlinkLamp = m_pushUpFlag == 2 ? 1 : 0;

		//�v�b�V���A�b�v�C�x���g
		if (g_speed != 0) //�w�W�����v�����O����
		{
			if ((m_pushUpFlag == 2 && (g_9n.McKey == 6 || m_tisFlag == 1) && (m_pushUpBeacon == 1 || g_speed < 2.0f)) || (m_pushUpFlag == -1 && (g_9n.McKey == 6 || m_tisFlag == 1) && m_dist <= 0)) //��ʃp�^�[���ɂ�������2�L�������邩�ʉ߂ŋ�����������Ǝ��s
			//if ((m_pushUpFlag == 2 && (m_pushUpBeacon == 1 || m_dist <= 2 || !g_pilotLamp)) || (m_pushUpFlag == -1 && m_dist <= 0)) //��ʃp�^�[���ɂ�������2�L�������邩�ʉ߂ŋ�����������Ǝ��s
			{
				m_pushUpFlag = 0;
				m_tisFlag = 0;

				//�X�e�b�v�X�V�̉񐔂������[�v
				for (; m_pushUpCount > 0; m_pushUpCount--)
				{
					if (m_pushUpCount > 1)
					{
						for (int i = 0; i < 10; i++)
						{
							PushUp(i);
						}
					}
					else
					{
						m_tmrVisible = g_time;
						for (int i = 0; i < 10; i++)
						{
							m_update[i] = 1;

							//3��ڂ̎��A���w���ŐV�ɂ���A�̎��w���ŐV�ɂ���A��Ԃ��X�V����
							if (i == 3)
							{
								PushNext();
								PushTimeStation();
								PushTrainNumber();
								PushSESta();
							}
						}
					}
				}

				//�y�[�W�߂��肵�Ȃ�
				if (m_option > 0)
				{					
					PushNext();
					PushTimeStation();
					PushTrainNumber();
					PushSESta();
				}
			}
		}
		//�Ȍ�w�W�����v
		else if (m_pushUpFlag == 3 || m_pushUpBeacon == 2)
		{
			m_pushUpFlag = 0;
			PushNext();
			PushTimeStation();
			PushTrainNumber();
			PushSESta();

			for (int i = 0; i < 10; i++)
			{
				PushUp(i);
			}
		}
		else if (m_pushUpFlag == -1) //�N�����̏�����
		{
			PushNext();
			PushTimeStation();
			PushTrainNumber();
			PushSESta();
		}

		//�X�e�b�v�X�V�̏���
		for (int i = 0; i < 10; i++)
		{
			if (g_time >= m_tmrVisible + (i * (LineUpdate * (2 / 3))) && g_time <= m_tmrVisible + LineUpdate * (i + 1) - (LineUpdate * (1 / 3)))
			{
				HiddenLine[i] = 1;

				if (m_update[i] == 1)
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

	int GetMDist(int flag)
	{
		return flag == 0 ? m_dist : m_tisFlag;
	}
	int GetPUFlag()
	{
		return m_pushUpFlag;
	}

	//�J�����Ɏ��s
	void DoorOpen(void)
	{
		m_door = 0; //�h�A���
		m_tmrVisible = 0; //�\���̍X�V�������Ȃ�
		m_tisFlag = 0;
	}

	//�����Ɏ��s
	void DoorClose(void)
	{
		m_door = 1; //�h�A���
	}

	//�ȉ��n��q�ʉߎ��Ɏ��s

	//���w�ڋ߁i105�j
	void Recieve(int data, int option)
	{
		m_pushUpFlag = data >= 0 ? 1 : -1;
		m_pushUpBeacon = 0;
		if (g_9n.McKey == 6)//JR�L�[�̎�
			m_pushUpCount = abs(data) > 100000 ? 0 : abs(data) / 10000 > 0 ? abs(data) / 10000 : 1; //10���ȏ�͍X�V���Ȃ��A1���̈ʂ͌���
		else//���̑��̎�
		{
			m_pushUpCount = g_9n.NowSta >= 0 ? 0 : 1;
			option = g_9n.NowSta >= 0 ? 1 : 0;
		}
		m_option = abs(option) > 0 ? 1 : 0;

		m_dist = abs(data % 10000) - TIMS_OFFSET;
		m_blinking = false;

		if (g_speed == 0) //�w�W�����v�����O����
		{
			m_pushUpFlag = abs(data) < 100000 ? 3 : 0;
			m_pushUpCount = 0;
			m_dist = 0;
		}
	}

	//���w�A���w�̐ݒ�i106�j
	void SetNext(int data)
	{
		m_thisName = data / 1000;
		m_nextName = data % 1000;
	}

	//���w�̎����ݒ�i107�j
	void SetNextTime(int data)
	{
		m_nextTime[0] = data / 10000;
		m_nextTime[1] = ((data % 10000) - (data % 100)) / 100;
		m_nextTime[2] = (data % 100) / 5;
	}

	//���w�����Ԑ��̐ݒ�i108�j
	void SetNextTrack(int data)
	{
		m_nextTrack = data;
	}

	//��Ԏ�ʂ̐ݒ�i101�j
	void SetKind(int data)
	{
		m_kind = data;
	}

	//��Ԕԍ��̐ݒ�i100�C150�j
	void SetNumber(int data, int chara)
	{
		m_charactor = chara; //�L���͉�2��
		m_Number[0] = data >= 1000 ? (data / 1000) % 10 == 0 ? 10 : (data / 1000) % 10 : 0; //1000������0�C���̑��͐��l�ʂ�
		m_Number[1] = data >= 100 ? (data / 100) % 10 == 0 ? 10 : (data / 100) % 10 : 0; //100������0�C0��10�C���̑��͐��l�ʂ�
		m_Number[2] = data >= 10 ? (data / 10) % 10 == 0 ? 10 : (data / 10) % 10 : 0; //10������0�C0��10�C���̑��͐��l�ʂ�
		m_Number[3] = data % 10 == 0 ? 10 : data % 10; //0��10�C���̑��͐��l�ʂ�
	}

	//�^�s��Ԃ̐ݒ�i104�j
	void SetLeg(int data)
	{
		m_from = data / 1000;
		m_destination = data % 1000;
	}

	//�s��̐ݒ�i109�j
	void SetFor(int data)
	{
		m_for = data;
	}

	//���s��Ԃ̐ݒ�i135�j
	void SetCrawl(int data)
	{
		if (data > 0)
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

	//���^�p�̐ݒ�i118�C119�C122�C123�j
	void SetAfteruent(int type, int data, int chara)
	{
		switch (type)
		{
		case 0://���
			m_afterChara = chara; //�L���͉�2��
			m_afterNumber[0] = data >= 1000 ? (data / 1000) % 10 == 0 ? 10 : (data / 1000) % 10 : 0;
			m_afterNumber[1] = data >= 100 ? (data / 100) % 10 == 0 ? 10 : (data / 100) % 10 : 0;
			m_afterNumber[2] = data >= 10 ? (data / 10) % 10 == 0 ? 10 : (data / 10) % 10 : 0;
			m_afterNumber[3] = data % 10 == 0 ? 10 : data % 10;
			break;
		case 1://��������
			m_afterTime[0][0] = data / 10000;
			m_afterTime[0][1] = ((data % 10000) - (data % 100)) / 100;
			m_afterTime[0][2] = (data % 100) / 5;
			break;
		case 2://���Ԏ���
			m_afterTime[1][0] = data / 10000;
			m_afterTime[1][1] = ((data % 10000) - (data % 100)) / 100;
			m_afterTime[1][2] = (data % 100) / 5;
			break;
		case 3://���
			m_afterKind = data;
			break;
		default:
			break;
		}
	}

	//�i�s�����ݒ�i102�j
	void SetDirection(int direction)
	{
		switch (direction)
		{
		case 1://��
			Direction = 1;
			break;
		case 2://�E
			Direction = 2;
			break;
		case 0://��O
		default:
			Direction = 0;
			break;
		}
	}

	//�~�ԉw�ݒ�i124�`126�j
	void SetLastStop(int type, int data)
	{
		switch (type)
		{
		case 0://��������
			m_lastTimeA[0] = data / 10000;
			m_lastTimeA[1] = ((data % 10000) - (data % 100)) / 100;
			m_lastTimeA[2] = (data % 100) / 5;
			break;
		case 1://���Ԏ���
			m_lastTimeL[0] = data / 10000;
			m_lastTimeL[1] = ((data % 10000) - (data % 100)) / 100;
			m_lastTimeL[2] = (data % 100) / 5;
			break;
		case 2://�Ԑ�
			m_lastTrack = data;
			break;
		default:
			break;
		}
	}

	//�~�ԉw���ݒ�i127�j
	void SetLastStation(int data)
	{
		m_lastName = data;
	}

	//�̎��w�����Ԑ��ݒ�i128�`132�j
	void SetTimeStationTime(int type, int data)
	{
		switch (type)
		{
		case 0://���O�̎��w����
			m_beforeTime[0] = data / 10000;
			m_beforeTime[1] = ((data % 10000) - (data % 100)) / 100;
			m_beforeTime[2] = (data % 100) / 5;
			break;
		case 1://���̎��w��������
			m_afterTimeA[0] = data / 10000;
			m_afterTimeA[1] = ((data % 10000) - (data % 100)) / 100;
			m_afterTimeA[2] = (data % 100) / 5;
			break;
		case 2://���̎��w���Ԏ���
			m_afterTimeL[0] = data / 10000;
			m_afterTimeL[1] = ((data % 10000) - (data % 100)) / 100;
			m_afterTimeL[2] = (data % 100) / 5;
			break;
		case 3://���O�̎��w�Ԑ�
			m_beforeTrack = data;
			break;
		case 4://���̎��w�Ԑ�
			m_afterTrack = data;
			break;
		default:
			break;
		}
	}

	//�̎��w�w���ݒ�i133�C134�j
	void SetTimeStationName(int type, int data)
	{
		if (type == 0)//���O�̎��w�ݒ�
			m_beforeName = data;
		else//���̎��w�ݒ�
			m_afterName = data;
	}

	//�А��p�w���̐ݒ�i10�C70�C604�j
	void SetSESta(int sta)
	{
		if (g_9n.DepartSta == -1) { g_9n.DepartSta = sta; }//104�Ԓn��q���Ȃ����A�ŏ��̉w��������
		m_seSta = sta; //�w���f�[�^������
		m_tisFlag = 1;
	}
	/*
	//�А��p�w�������݁i603�j
	void SetSEStaExtra(int data)
	{
		SESta[0] = data;
	}
	*/
	void SetSEDirection(int data, int area)
	{
		SEDirection = data == 0 ? -1 : 1; //����Ԃł͌��Z
		/*
		if (area >= 51)
			SEArea = 1;//���葽�����E���V�h�s��
		if (area >= 31)
			SEArea = 2;//����]�m����
		else
			SEArea = 0;//���菬�c�����E�����c��
			*/
	}

	//�s�H�\���̐ݒ�i117�j
	void SetArrowState(int data)
	{
		TrainArrow = data;
	}

	//���s�����̐ݒ�i103�j
	void SetDistance(float distance, int data)
	{
		StartingPoint = distance; //�����v�Z�̋N�_
		m_direction = data >= 1000000 ? -1 : 1;
		m_distance = data % 1000000; //���s����
	}

	//���s�����̐ݒ�isnp2-114�j
	void SetPositionDef(int data)
	{
		m_distDef = data; //��Ԉʒu�̕␳
	}

	//�X�^�t�e�[�u���̏������݁i110�`116�j
	void InputLine(int column, int line, int data)
	{
		switch (column)
		{
		case 0: //�w�ԑ��s����
			Span[line][0] = data / 100;
			Span[line][1] = (data % 100) / 5;
			break;
		case 9: //�w�ԑ��s����snp2
			Span[line][0] = data / 100;
			Span[line][1] = (data % 100) / 5;
			if (Span[line][0] == 0)
				Span[line][0] = 60;
			else if (Span[line][0] == 60)
				Span[line][0] = 0;
			if (Span[line][1] == 0)
				Span[line][1] = 12;
			else if (Span[line][1] == 12)
				Span[line][1] = 0;
			break;
		case 1: //�w���\��
			Station[line] = data;
			break;
		case 2: //��������
			Arrive[line][0] = data / 10000;
			Arrive[line][1] = ((data % 10000) - (data % 100)) / 100;
			Arrive[line][2] = (data % 100) / 5;
			break;
		case 3: //���Ԏ���
			Leave[line][0] = data / 10000;
			Leave[line][1] = ((data % 10000) - (data % 100)) / 100;
			Leave[line][2] = (data % 100) / 5;
			break;
		case 7: //��������snp2
			Arrive[line][0] = data / 10000;
			Arrive[line][1] = ((data % 10000) - (data % 100)) / 100;
			Arrive[line][2] = (data % 100) / 5;
			if (Arrive[line][0] == 0)
				Arrive[line][0] = 24;
			else if (Arrive[line][0] == 24)
				Arrive[line][0] = 0;
			if (Arrive[line][1] == 0)
				Arrive[line][1] = 60;
			else if (Arrive[line][1] == 60)
				Arrive[line][1] = 0;
			if (Arrive[line][2] == 0)
				Arrive[line][2] = 12;
			else if (Arrive[line][2] == 12)
				Arrive[line][2] = 0;
			break;
		case 8: //���Ԏ���snp2
			Leave[line][0] = data / 10000;
			Leave[line][1] = ((data % 10000) - (data % 100)) / 100;
			Leave[line][2] = (data % 100) / 5;
			if (Leave[line][0] == 0)
				Leave[line][0] = 24;
			else if (Leave[line][0] == 24)
				Leave[line][0] = 0;
			if (Leave[line][1] == 0)
				Leave[line][1] = 60;
			else if (Leave[line][1] == 60)
				Leave[line][1] = 0;
			if (Leave[line][2] == 0)
				Leave[line][2] = 12;
			else if (Leave[line][2] == 12)
				Leave[line][2] = 0;
			break;
		case 4: //�Ԑ�
			Track[line] = data;
			break;
		case 5: //�i������
			LimitA[line] = data / 5;
			break;
		case 6://�i�o����
			LimitL[line] = data / 5;
			break;
		}
	}

private:
	int m_door; //�h�A���
	float m_dist; //��~�\��_����
	bool m_blinking; //���w�_�ł��ǂ���

	int m_pushUpFlag; //�\���X�V�̃t���O
	int m_pushUpBeacon; //�\���X�V�̒n��q
	int m_pushUpCount; //�\���X�V�̌J��Ԃ���
	int m_tisFlag; //TIS�\���X�V�̃t���O
	int m_tmrVisible; //���j�^�̃X�e�b�v�X�V
	int m_update[7]; //�X�e�b�v�X�V�̏��
	int m_thisName; //���w�w��
	int m_nextName; //���w�w��
	int m_seSta; //�А��p�w���X�g�b�N
	int m_nextTime[3]; //���w����
	int m_nextTrack; // ���w�����Ԑ�
	int m_beforeName; //���O�̎��w��
	int m_beforeTime[3]; //���O�̎��w����
	int m_beforeTrack; // �O�̎��w�Ԑ�
	int m_afterName; //���̎��w��
	int m_afterTimeA[3]; //���̎��w������
	int m_afterTimeL[3]; //���̎��w������
	int m_afterTrack; //���̎��w�Ԑ�
	int m_lastName; //�~�ԉw�w��
	int m_lastTimeA[3]; //�~�ԉw��������
	int m_lastTimeL[3]; //�~�ԉw�o������
	int m_lastTrack; //�~�ԉw�Ԑ�
	int m_from; //�^�s�p�^�[�����w
	int m_destination; //�^�s�p�^�[�����w
	int m_for; //��ԍs��
	int m_option; //���w�ƍ̎��w�̂ݍX�V
	int m_kind; //��Ԏ��
	int m_Number[4]; //��Ԕԍ�
	int m_charactor; //��ԋL��
	int m_afterNumber[4]; //���^�p���
	int m_afterChara; //���^�p��ԋL��
	int m_afterTime[2][3]; //���^�p��������
	int m_afterKind; //���^�p���

	double m_distance; //���s����
	int m_direction; //�������Z���邩�ǂ���
	int m_distDef; //��Ԉʒu�̕␳
	int m_timerTims; //TIMS�\����X�V�^�C�}�[

	//�X�^�t�e�[�u���̃v�b�V���A�b�v
	void PushUp(int line)
	{
		if (line == 9)//10�s�ڂ̎����ȊO���Z�b�g
		{
			Station[line] = 0;
			Track[line] = 0;
			LimitA[line] = 0;
			LimitL[line] = 0;
			Span[line][0] = 0;
			Span[line][1] = 0;
		}
		else //1����9�s�ڂ̌J��グ
		{
			Station[line] = Station[line + 1];
			Track[line] = Track[line + 1];
			LimitA[line] = LimitA[line + 1];
			LimitL[line] = LimitL[line + 1];
			Span[line][0] = Span[line + 1][0];
			Span[line][1] = Span[line + 1][1];
		}

		for (int j = 0; j < 3; j++)//���������̃��Z�b�g
		{
			if (line == 9) //10�s�ڂ���ɂ���
			{
				switch (j)
				{
				case 0:
				case 1:
				case 2:
					Arrive[line][j] = 0;
					Leave[line][j] = 0;
					break;
				}
			}
			else //1����9�s�ڂ͌J��グ
			{
				Arrive[line][j] = Arrive[line + 1][j];
				Leave[line][j] = Leave[line + 1][j];
			}
		}
	}

	//�А��p�w�����X�V
	void PushSESta(void)
	{
		SESta[0] = m_seSta;
	}

	//���͂���Ă��鎟�w����ʂɔ��f
	void PushNext(void)
	{
		This = m_thisName;
		Next = m_nextName;
		NextTime[0] = m_nextTime[0];
		NextTime[1] = m_nextTime[1];
		NextTime[2] = m_nextTime[2];
		NextTrack = m_nextTrack;
	}

	//���͂���Ă���̎��w����ʂɔ��f
	void PushTimeStation(void)
	{
		Before = m_beforeName; //���O�̎��w
		After = m_afterName; //���̎��w
		Last = m_lastName; //�~�ԉw

		BeforeTime[0] = m_beforeTime[0];
		BeforeTime[1] = m_beforeTime[1];
		BeforeTime[2] = m_beforeTime[2];

		AfterTimeA[0] = m_afterTimeA[0];
		AfterTimeA[1] = m_afterTimeA[1];
		AfterTimeA[2] = m_afterTimeA[2];

		AfterTimeL[0] = m_afterTimeL[0];
		AfterTimeL[1] = m_afterTimeL[1];
		AfterTimeL[2] = m_afterTimeL[2];

		LastTimeA[0] = m_lastTimeA[0];
		LastTimeA[1] = m_lastTimeA[1];
		LastTimeA[2] = m_lastTimeA[2];

		LastTimeL[0] = m_lastTimeL[0];
		LastTimeL[1] = m_lastTimeL[1];
		LastTimeL[2] = m_lastTimeL[2];

		BeforeTrack = m_beforeTrack;
		AfterTrack = m_afterTrack;
		LastTrack = m_lastTrack;

		From = m_from;
		Destination = m_destination;
		For = m_for;
	}

	//���͂���Ă����Ԕԍ��Ǝ�ʂ���ʂɔ��f
	void PushTrainNumber(void)
	{
		//���ݎ�ʗ��
		Kind = m_kind % 100;
		PassMode = (m_kind >= 100) ? (m_kind / 100) : 0; //�ʉߐݒ�

		Number[0] = m_Number[0];
		Number[1] = m_Number[1];
		Number[2] = m_Number[2];
		Number[3] = m_Number[3];
		Charactor = m_charactor;

		//���s�H���
		AfterKind = m_afterKind;
		AfterNumber[0] = m_afterNumber[0];
		AfterNumber[1] = m_afterNumber[1];
		AfterNumber[2] = m_afterNumber[2];
		AfterNumber[3] = m_afterNumber[3];
		AfterChara = m_afterChara;

		//���s�H��������
		AfterTime[0][0] = m_afterTime[0][0];
		AfterTime[0][1] = m_afterTime[0][1];
		AfterTime[0][2] = m_afterTime[0][2];
		AfterTime[1][0] = m_afterTime[1][0];
		AfterTime[1][1] = m_afterTime[1][1];
		AfterTime[1][2] = m_afterTime[1][2];
	}

	//�z��̏�����
	void ResetArray(void)
	{
		for (int c3 = 0; c3 < 3; c3++)//3�񃋁[�v�A�~�ԉw�̃��Z�b�g
		{
			LastTimeA[c3] = 0;
			LastTimeL[c3] = 0;
			m_nextTime[c3] = 0;

			for (int c3_2 = 0; c3_2 < 2; c3_2++)//2�񃋁[�v�A���s�H�����̃��Z�b�g
			{
				AfterTime[c3_2][c3] = 0;
			}
		}

		for (int st3 = 0; st3 < 3; st3++)//3�񃋁[�v�A�̎��w�̃��Z�b�g
		{
			BeforeTime[st3] = 0;
			AfterTimeA[st3] = 0;
			AfterTimeL[st3] = 0;
			m_beforeTime[st3] = 0;
			m_afterTimeA[st3] = 0;
			m_afterTimeL[st3] = 0;
		}
		/*
		for (int o3 = 0; o3 < 3; o3++)//3�񃋁[�v�A�F�X���Z�b�g
		{

		}
		*/
		for (int c4 = 0; c4 < 4; c4++)//4�񃋁[�v�A��Ԃ̃��Z�b�g
		{
			Number[c4] = 10;
			AfterNumber[c4] = 10;
		}

		for (int c7 = 0; c7 < 7; c7++)//7�񃋁[�v�A�A�b�v�f�[�g�����Z�b�g
		{
			m_update[c7] = 0;
		}

		for (int c10 = 0; c10 < 10; c10++)//10�񃋁[�v�A�����ȊO�̃X�^�t�e�[�u���̃��Z�b�g
		{
			Station[c10] = 0;
			Track[c10] = 0;
			LimitA[c10] = 0;
			LimitL[c10] = 0;

			HiddenLine[c10] = 0;
			UnitTims[c10] = 0;
			UnitState[c10] = 0;

			Arrive[c10][0] = 0;
			Leave[c10][0] = 0;
			Span[c10][0] = 0;

			Arrive[c10][1] = 0;
			Leave[c10][1] = 0;
			Span[c10][1] = 0;

			Arrive[c10][2] = 0;
			Leave[c10][2] = 0;
		}
	}
};
