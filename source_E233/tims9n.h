//tims9n.h
//�}�X�R���L�[�A���ȂǁA�����v���݊��@�\��ݒ肵�܂�

#define TIMS_DECELERATION 19.5F //�����萔�i�����x[km/h/s] * 7.2�j
#define TIMS_OFFSET 0.5F //�ԏ�q�I�t�Z�b�g

class tims9N
{
public:
	int McKey; //�}�X�R���L�[
	int TrainType; //���
	int m_DepartSta; //��ԉw�ϊ��O
	int DepartSta; //��ԉw
	int ArrivalSta; //�~�ԉw
	int SEArea; //�А��p���s����
	int m_Location; //���w������
	int Location; //���w������
	int Array;
	int NowSta; //���݉w�i���i����0�j
	int SESta[10]; //�А��w��
	int DispSESta[10];
	int SEDirection; //�А��i�s����
	//int SEArea; //�А��p���s����
	int HiddenLine[10]; //�X�V����1�s����\���ɂ���

	//������
	void Init()
	{
		m_Location = 0;
		Location = 0;
		m_DepartSta = -1;

		m_dist = 0;
		m_array = 0;
		m_array2 = 0;

		m_pushUpFlag = -1;
		m_pushUpBeacon = 0;
		m_pushUpCount = 0;
		m_tmrVisible = g_time;
	}

	//���t���[�����s
	void Execute()
	{
		float speed = fabsf(g_speed); //���x�̐�Βl[km/h]
		float def = speed / 3600 * g_deltaT; //1�t���[���œ����������i��Βl�j[m]
		m_dist -= def; //�c�苗�������Z����
		m_array -= def;
		Location = m_array;
		if (m_array != 0 && m_array >= m_array2 * 2 / 3) //�c����2/3�ȏ�
			Array = 1;
		else if (m_array != 0 && m_array >= m_array2 / 3)
			Array = 2;
		else if (m_array > 0)
			Array = 3;
		else
			Array = 0;
		Array += max(NowSta, 0) * 4;
		if (Array > 16) { Array = 16; }

		//[TIMS9N�̂�]�w���̑}��
		for (int i = 0; i < 7; i++)
		{
			if (SEDirection == 1) //�c�cA���E���c�}����
			{
				SESta[i + 1] = SetSEStaA(SESta[i]);
			}
			else //�c�cB���E���c�}���
			{
				SESta[i + 1] = SetSEStaB(SESta[i]);
			}
			if (i <= 4)
			{
				DispSESta[i] = SESta[i];
			}
		}

		NowSta = -1;//1�i��

		//�~�ԉw��SESta[0]�`SESta[3]�ƈ�v����ꍇ
		for (int i = 0; i < 5; i++)
		{
			if (SESta[i] == ConvDest2Sta(ArrivalSta))
			{
				NowSta = 4 - i; //�i��������
				if (i < 4)
				{
					DispSESta[4] = SESta[i]; //�����I��5�w�ڂƂ���
					for (int j = 4; j > 0; j--)//4�`0�̉w�������Ă͂�
					{
						if (SEDirection == 1) //�c�cA���E���c�}����
						{
							DispSESta[j - 1] = SetSEStaB(DispSESta[j]);
						}
						else //�c�cB���E���c�}���
						{
							DispSESta[j - 1] = SetSEStaA(DispSESta[j]);
						}
					}
					for (int j = 4; j < 7; j++)//5�`7�̉w�������Ă͂�
					{
						if (SEDirection == 1) //�c�cA���E���c�}����
						{
							DispSESta[j + 1] = SetSEStaA(DispSESta[j]);
						}
						else //�c�cB���E���c�}���
						{
							DispSESta[j + 1] = SetSEStaB(DispSESta[j]);
						}
					}
				}
				break;
			}
		}
		//���w��_�ł�����
		if (speed * speed / TIMS_DECELERATION >= m_dist - 50 && m_dist > 0) //���x�ƍ��i�p�^�[��)
		{
			//m_blinking = true;

			if (m_pushUpFlag == 1) //��ԉw�̂Ƃ�
			{
				m_pushUpFlag = 2; //2 = ��2�L����������X�V
			}
		}

		//�v�b�V���A�b�v�C�x���g
		if (g_speed != 0) //�w�W�����v�����O����
		{
			if ((m_pushUpFlag == 2 && m_tisFlag == 1 && (m_pushUpBeacon == 1 || g_speed < 2.0f)) || (m_pushUpFlag == -1 && m_tisFlag == 1 && m_dist <= 0)) //��ʃp�^�[���ɂ�������2�L�������邩�ʉ߂ŋ�����������Ǝ��s
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
							//PushUp(i);
						}
					}
					else
					{
						m_tmrVisible = g_time;
						for (int i = 0; i < 10; i++)
						{
							//m_update[i] = 1;

							//3��ڂ̎��A���w���ŐV�ɂ���A�̎��w���ŐV�ɂ���A��Ԃ��X�V����
							if (i == 3)
							{
								PushSESta();
							}
						}
					}
				}

				//�y�[�W�߂��肵�Ȃ�
				if (m_option > 0)
				{
					PushSESta();
				}
			}
		}
		//�Ȍ�w�W�����v
		else if (m_pushUpFlag == 3 || m_pushUpBeacon == 2)
		{
			m_pushUpFlag = 0;
			PushSESta();
		}
		else if (m_pushUpFlag == -1) //�N�����̏�����
		{
			PushSESta();
		}
		//�X�e�b�v�X�V�̏���
		for (int i = 0; i < 10; i++)
		{
			if (g_time >= m_tmrVisible + (i * (LineUpdate * (2 / 3))) && g_time <= m_tmrVisible + LineUpdate * (i + 1) - (LineUpdate * (1 / 3)))
			{
				HiddenLine[i] = 1;
				/*
				if (m_update[i] == 1)
				{
					m_update[i] = 0;
					PushUp(i);
				}*/
			}
			else
			{
				HiddenLine[i] = 0;
			}
		}
	}

	//���w�ڋ�SE�i622�j
	void RecieveSE(int data, int option)
	{
		m_pushUpFlag = data >= 0 ? 1 : -1;
		m_pushUpBeacon = 0;
		{
			m_pushUpCount = NowSta >= 0 ? 0 : 1;
			option = NowSta >= 0 ? 1 : 0;
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

	//�А��p�w���̐ݒ�i10�C70�C604�j
	void SetSESta(int sta)
	{
		if (m_DepartSta == -1) {m_DepartSta = sta; }//104�Ԓn��q���Ȃ����A�ŏ��̉w��������
		m_seSta = sta; //�w���f�[�^������
		m_tisFlag = 1;
	}

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

	//���t���[�����s�~�ԉw
	void SetArrivalSta(int ats172)
	{
		ArrivalSta = ats172;
		if (McKey == 7)
		{
			//���c�}�L�[�Ń��g���ȉ��̍s��̍ۂ͑�X�؏㌴�~��
			if ((ats172 >= 15 && ats172 <= 19) || (ats172 >= 42 && ats172 <= 48) || ats172 == 50 || ats172 == 51 || ats172 == 57)
			{
				ArrivalSta = 41;
			}
		}
		if (McKey == 1)
		{
			//���g���L�[�ŏ��c�}�̍s��̍ۂ͑�X�؏㌴�~��
			if ((ats172 >= 1 && ats172 <= 8) || (ats172 >= 10 && ats172 <= 14) || (ats172 >= 21 && ats172 <= 35))
			{
				ArrivalSta = 41;
			}
			//���g���L�[��JR�̍s��̍ۂ͈����~��
			else if (ats172 >= 15 && ats172 <= 19)
			{
				ArrivalSta = 42;
			}
		}
		if (ats172 == 21)//�V�h�s����1
			SEArea = 1;
		else if (ats172 == 24)//���ؓc�s����3
			SEArea = 3;
		else if (ats172 == 14 || ats172 == 4 || ats172 == 5 || ats172 == 27 || ats172 == 28 || ats172 == 35)//��a�E����E�]�m���E����s����2
			SEArea = 2;
		else//����0
			SEArea = 0;
	}

	//���t���[�����s��ԉw
	void SetDepartSta()
	{
		DepartSta = ConvSta2Dest(m_DepartSta);
		if (McKey == 7)
		{
			//���c�}�L�[�Ń��g���ȉ��̎n���w�̏ꍇ�͑�X�؏㌴���
			if ((DepartSta >= 15 && DepartSta <= 19) || (DepartSta >= 42 && DepartSta <= 48) || DepartSta == 50 || DepartSta == 51 || DepartSta == 57)
			{
				DepartSta = 41;
			}
		}
		if (McKey == 1)
		{
			//���g���L�[�ŏ��c�}�̍s��̍ۂ͑�X�؏㌴�~��
			if ((DepartSta >= 1 && DepartSta <= 8) || (DepartSta >= 10 && DepartSta <= 14) || (DepartSta >= 21 && DepartSta <= 35))
			{
				DepartSta = 41;
			}
			//���g���L�[��JR�̍s��̍ۂ͈����~��
			else if (DepartSta >= 15 && DepartSta <= 19)
			{
				DepartSta = 42;
			}
		}
		if (ConvSta2Dest(m_DepartSta) == 21)//�V�h�n����1
		{
			if (SEArea == 2 || SEArea == 3)
				SEArea += 2;//4or5
			else
				SEArea = 3;
		}
		else if (ConvSta2Dest(m_DepartSta) == 24)//���ؓc�n����3
		{
			if (SEArea == 1)
				SEArea = 5;
			else
				SEArea = 2;
		}
		else if (ConvSta2Dest(m_DepartSta) == 14 || ConvSta2Dest(m_DepartSta) == 4 || ConvSta2Dest(m_DepartSta) == 5 || ConvSta2Dest(m_DepartSta) == 27 || ConvSta2Dest(m_DepartSta) == 28 || ConvSta2Dest(m_DepartSta) == 35)//��a�E����E�]�m���E����n����2
		{
			if (SEArea == 1)
				SEArea = 4;
			else
				SEArea = 1;
		}
		else//����0
		{
			if (SEArea == 2 || SEArea == 3)
				SEArea -= 1;//1or2
			else if (SEArea == 1)
				SEArea = 3;
			else
				SEArea = 0;
		}

	}

	//A���E1�w����
	int SetSEStaA(int sta0)
	{
		int sta1 = sta0;
		switch (sta0)
		{
			/*
		case 56: //�k����
			sta1 = 101;
			break;
		case 101: //��������
			sta1 = 57;
			break;
		case 58: //�k��Z
			sta1 = 102;
			break;
		case 102: //��Z�ܕ�
			sta1 = 59;
			break;
		case 62: //����
			sta1 = 103;
			break;
		case 103: //����Y��
			sta1 = 63;
			break;
		case 64: //�V�䒃�m��
			sta1 = 104;
			break;
		case 104: //���ܕ�
			sta1 = 65;
			break;
		case 67: //����J
			sta1 = 105;
			break;
		case 105: //���֐ܕ�
			sta1 = 68;
			break;
		case 68: //���P��
			sta1 = 106;
			break;
		case 106: //����Y��
			sta1 = 69;
			break;
		case 71: //�T�؍�
			sta1 = 107;
			break;
		case 107: //�Q���ܕ�
			sta1 = 72;
			break;
		case 73: //�����_�{
			sta1 = 108;
			break;
		case 108: //��������
			sta1 = 74;
			break;
			*/
		case 119: //��X�،���
			sta1 = 31;
			break;
		case 120: //��X�؏㌴
			sta1 = 32;
			break;
		case 49: //�V�S�����u
		case 84:
			sta1 = SEArea % 3 == 2 ? 85 : 50;
			break;
			/*
		case 53: //���c
			sta1 = 13;
			break;
			*/
		case 13: //���͑��
		case 54: //���͑��
			sta1 = SEArea % 3 == 1 ? 12 : 55;
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
			sta1 = sta0 - 1; //�]�m�����ʏ�
			break;
		case 1: //����{��
			sta1 = 79; //����
			break;
		case 0: //�����Ȃ����͂��̂܂܃��[�v�����Ă���
			sta1 = 0;
			break;
		default:
			sta1 = sta0 + 1;//�ʏ펞
			break;
		}
		return sta1;
	}

	//B���E1�w����
	int SetSEStaB(int sta0)
	{
		int sta1 = sta0;
		switch (sta0)
		{
			/*
		case 57: //����
			sta1 = 101;
			break;
		case 101: //��������
			sta1 = 56;
			break;
		case 59: //����
			sta1 = 102;
			break;
		case 102: //��Z�ܕ�
			sta1 = 58;
			break;
		case 63: //����
			sta1 = 103;
			break;
		case 103: //����Y��
			sta1 = 62;
			break;
		case 65: //��蒬
			sta1 = 104;
			break;
		case 104: //���ܕ�
			sta1 = 64;
			break;
		case 68: //���P��
			sta1 = 105;
			break;
		case 105: //���֐ܕ�
			sta1 = 67;
			break;
		case 69: //����c�����O
			sta1 = 106;
			break;
		case 106: //����Y��
			sta1 = 68;
			break;
		case 72: //�\�Q��
			sta1 = 107;
			break;
		case 107: //�Q���ܕ�
			sta1 = 71;
			break;
		case 74: //��X�،���
			sta1 = 108;
			break;
		case 108: //��������
			sta1 = 73;
			break;
			*/
		case 31: //��X�؏㌴
		case 120:
			sta1 = SEArea / 3 == 0 ? 119 : 30;
			break;
		case 85: //�܌���
			sta1 = 49;
			break;
		case 84: //�V�S��
			sta1 = 48;
			break;
			/*
		case 75: //���c�}���͌�
			sta1 = 13;
			break;
			*/
		case 13: //���͑��
			sta1 = 53;
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
			sta1 = sta0 + 1; //�]�m�����ʏ�
			break;
		case 79: //����
			sta1 = 1;
			break;
		case 0: //�����Ȃ����͂��̂܂܃��[�v�����Ă���
			sta1 = 0;
			break;
		default:
			sta1 = sta0 - 1;//�ʏ펞
			break;
		}
		return sta1;
	}

	//�w�ԍ����s��ԍ��ɕϊ�
	int ConvSta2Dest(int sta)
	{
		int dest = sta;
		switch (sta)
		{
		case 3:
			dest = 2;
			break;
		case 13:
			dest = 1;
			break;
		case 60:
			dest = 6;
			break;
		case 73:
			dest = 7;
			break;
		case 45:
			dest = 8;
			break;
		case 31:
			dest = 9;
			break;
		case 37:
			dest = 10;
			break;
		case 58:
			dest = 11;
			break;
		case 62:
			dest = 12;
			break;
		case 65:
			dest = 13;
			break;
		case 8:
			dest = 14;
			break;
		case 95:
			dest = 42;
			break;
		case 93:
			dest = 43;
			break;
		case 119:
			dest = 44;
			break;
		case 96:
			dest = 45;
			break;
		case 117:
			dest = 46;
			break;
		case 110:
			dest = 47;
			break;
		case 103:
			dest = 48;
			break;
		case 116:
			dest = 50;
			break;
		case 106:
			dest = 51;
			break;
		case 101:
			dest = 57;
			break;
		default:
			dest = 0;
			break;
		}
		return dest;
	}

	//�s��ԍ����w�ԍ��ɕϊ�
	int ConvDest2Sta(int dest)
	{
		int sta = dest;
		switch (dest)
		{
		case 1:
		case 26:
			sta = 13;
			break;
		case 2:
		case 25:
			sta = 53;
			break;
		case 6:
		case 29:
			sta = 60;
			break;
		case 7:
		case 31:
			sta = 73;
			break;
		case 8:
		case 22:
			sta = 45;
			break;
		case 9:
		case 41:
			sta = 31;
			break;
		case 10:
			sta = 37;
			break;
		case 11:
			sta = 58;
			break;
		case 12:
			sta = 62;
			break;
		case 13:
			sta = 65;
			break;
		case 14:
			sta = 8;
			break;
		case 42:
			sta = 95;
			break;
		case 43:
			sta = 93;
			break;
		case 44:
			sta = 119;
			break;
		case 45:
			sta = 96;
			break;
		case 46:
			sta = 117;
			break;
		case 47:
			sta = 110;
			break;
		case 48:
			sta = 103;
			break;
		case 50:
			sta = 116;
			break;
		case 51:
			sta = 106;
			break;
		case 57:
			sta = 101;
			break;
		default:
			break;
		}
		return sta;
	}

	//�����v���w����ϊ�
	int ConvUsao2TIMS(int sta)
	{
		int nsta = sta;
		switch (sta)
		{
		case 13:
			return 95;
		case 56:
			return 93;
		case 57:
			return 95;
		case 58:
			return 96;
		case 59:
			return 98;
		case 60:
			return 99;
		case 61:
			return 100;
		case 62:
			return 101;
		case 63:
			return 103;
		case 64:
			return 104;
		case 65:
			return 106;
		case 66:
			return 107;
		case 67:
			return 108;
		case 68:
			return 110;
		case 69:
			return 112;
		case 70:
			return 113;
		case 71:
			return 114;
		case 72:
			return 116;
		case 73:
			return 117;
		case 74:
			return 119;
		case 75:
			return 120;
		default:
			return sta + 150;
		}
	}


	//�����������ʉ߉w�ݒ�
	int SetTrainPass(int sta)
	{
#define PASS 62
#define STOP 63
		int ret = STOP;
		switch (TrainType)
		{
		case 5: //A����
			if (sta == 36)
				ret = PASS;
		case 3: //B����
		case 7:
			if (sta == 33 || sta == 34 || sta == 35)
				ret = PASS;
		case 4: //�ʋΉ���
			if (sta == 37 || sta == 38 || sta == 39 || sta == 40)
				ret = PASS;
			if (TrainType != 7)//���t�����ȊO
				break;
			if (sta == 42 || sta == 43 || sta == 45 || sta == 47 || sta == 48)
				ret = PASS;
			break;
		}
		return ret;
	}

	//�s�H�\���
	void SetLocation(int loc)
	{
		//m_Location = Location;
		//Location = loc;
		if (g_speed != 0) {//�w�W�����v�ȊO
			m_array = loc;
			m_array2 = loc;
		}
	}

private:
	float m_dist; //��~�\��_����
	bool m_blinking; //���w�_�ł��ǂ���

	float m_array;
	float m_array2;

	int m_pushUpFlag; //�\���X�V�̃t���O
	int m_pushUpBeacon; //�\���X�V�̒n��q
	int m_pushUpCount; //�\���X�V�̌J��Ԃ���

	int m_tisFlag; //TIS�\���X�V�̃t���O
	int m_seSta; //�А��p�w���X�g�b�N

	int m_tmrVisible; //���j�^�̃X�e�b�v�X�V
	int m_update[7]; //�X�e�b�v�X�V�̏��
	int m_option; //���w�ƍ̎��w�̂ݍX�V

	//�А��p�w�����X�V
	void PushSESta(void)
	{
		SESta[0] = m_seSta;
	}
};