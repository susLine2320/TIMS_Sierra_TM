//sub.h
//�u���[�L�����Ȃǂ̋@�\���������܂�

class Sub
{
private:
	int m_tmrBcPressCut;
	int m_relayD; //���s���m�����[
	int m_lbInit; //����N�����f���퉹
	int m_airHigh; //���u���[�L�ɉ���
	int m_airApply; //�u���[�L������
	float m_bcPrsDelta;
	int m_emgAnnounce; //���u���[�L����
	int m_airApplyEmg; //�u���[�L�������i���j
	int m_updateInfo; //�^�s���X�V
	int m_deltaUpdate; //�O��^�s���X�V
public:
	int BcPressCut; //�u���[�L��������
	int RelayState; //���s���m�����[

	int AccelCutting; //�͍s�x��
	int RelayD; //���s���m�����[
	int LbInit; //����N�����f���퉹
	int AirHigh; //���u���[�L�ɉ���
	int AirApply; //�u���[�L������
	int EmgAnnounce; //���u���[�L����
	int AirApplyEmg; //�u���[�L�������i���j
	int UpdateInfo; //�^�s���X�V

	int event_lbInit; //����N�����͍s�x��
	int BcPressure;

	void load(void)
	{
		BcPressCut = 0;
		RelayState = 0;
		AccelCutting = 0;
		m_tmrBcPressCut = 0;
		m_relayD = ATS_SOUND_STOP;
		m_lbInit = ATS_SOUND_STOP;
		m_airHigh = ATS_SOUND_STOP;
		AirApply = ATS_SOUND_STOP;
		m_bcPrsDelta = 0.0F;
		m_emgAnnounce = ATS_SOUND_STOP;
		m_airApplyEmg = ATS_SOUND_STOP;
		m_updateInfo = ATS_SOUND_STOP;
		m_deltaUpdate = g_time - UPDATE_SPAN;
	}

	void Execute(void)
	{
		//����N�����͍s�x��
		if (event_lbInit > 0 && g_powerNotch > 0)
		{
			event_lbInit = 0;
			AccelCutting = g_time + 500;
			m_lbInit = ATS_SOUND_PLAY;
		}

		//����N�����f���퉹
		LbInit = m_lbInit;
		m_lbInit = ATS_SOUND_CONTINUE;

		//�u���[�L����
		if (m_tmrBcPressCut < g_time)
		{
			BcPressCut = 0;
			m_tmrBcPressCut = 0;
		}
		else
		{
			BcPressCut = 1;
		}

		//�ɉ�x���^�C�}�[�̖h��
		if (fabs(g_speed) > 3.0f)
		{
			m_tmrBcPressCut = 0;
			BcPressCut = 0;
		}

		//���s���m�����[
		if (RelayState == 0 && fabs(g_speed) >= RELAY_REQUIREMENT)
		{
			RelayState = 1;
			m_relayD = ATS_SOUND_PLAY;
		}
		else if (RelayState == 1 && fabs(g_speed) <= RELAY_REQUIREMENT)
		{
			RelayState = 0;
			m_relayD = ATS_SOUND_PLAY;
		}

		RelayD = m_relayD;
		m_relayD = ATS_SOUND_CONTINUE;

		//���u���[�L�ɉ���
		AirHigh = m_airHigh;
		m_airHigh = ATS_SOUND_CONTINUE;

		//�u���[�L������
		if (g_brakeNotch > 0 && BcPressure > 40.0F && BcPressure > m_bcPrsDelta)
			AirApply = ATS_SOUND_PLAYLOOPING;
		else
			AirApply = ATS_SOUND_STOP;

		m_bcPrsDelta = BcPressure; //BC����[kPa]

		//���u���[�L����
		EmgAnnounce = m_emgAnnounce;
		m_emgAnnounce = ATS_SOUND_CONTINUE;

		//�u���[�L�������i���j
		AirApplyEmg = m_airApplyEmg;
		m_airApplyEmg = ATS_SOUND_CONTINUE;

		//�^�s���X�V
		if (g_speed > 10.0f && g_time > m_deltaUpdate + UPDATE_SPAN)
		{
			m_deltaUpdate = g_time;
			
			if (rand() % 200 == 0)
				m_updateInfo = ATS_SOUND_PLAY;
		}
		UpdateInfo = m_updateInfo;//�^�s���X�V
		m_updateInfo = ATS_SOUND_CONTINUE;
	}

	//���u���[�L�ɉ������Đ�
	void PlaySoundAirHigh(int state, int newState)
	{
		if (state == g_emgBrake && newState < g_emgBrake && BcPressure > 340)
		{
			m_airHigh = ATS_SOUND_PLAY;
			m_tmrBcPressCut = g_time + EbCut; //�u���[�L�����̃^�C�}�[
		}
	}

	//���u���[�L�������Đ�����
	void PlaySoundEmgAnnounce(int state, int newState)
	{
		if (state != g_emgBrake && newState == g_emgBrake && g_speed > 5.0F)//���u���[�L�������x
		{
			m_emgAnnounce = ATS_SOUND_PLAY;
		}
	}

	//����N�����f
	void SetLbInit(int pos, int enable)
	{
		if (pos == 0 && enable == 1)
		{
			event_lbInit = 1;
		}
	}

	//�u���[�L�������i���j���Đ�����
	void PlaySoundAirApplyEmg(int state, int newState)
	{
		if (state != g_emgBrake && newState == g_emgBrake && BcPressure < 150.0f)
		{
			m_airApplyEmg = ATS_SOUND_PLAY;
		}
	}
};