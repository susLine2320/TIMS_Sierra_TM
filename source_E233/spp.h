//spp.h
//���̃t�@�C���ł͌�ʉߖh�~���u���V�~�����[�g���܂�

//�����l��`�i��ʉߖh�~���u�j
#define SPP_DECELERATION 19.5F //�����萔�i�����x[km/h/s] * 7.2�j
#define SPP_OFFSET 0.5F //�ԏ�q�I�t�Z�b�g[m]
#define SPP_ALARMTYPE 1

//��ʉߖh�~���u
class SPP
{
private:
	float m_dist; //��~�\��_����
	bool m_played;
	bool m_halt;
	int m_haltChime;
	int m_haltChime2;
	int m_haltChime3;
	int m_passAlarm;
public:
	int HaltChime; //��ԃ`���C��
	int HaltChime2; //��ԃ`���C�����[�v
	int HaltChime3; //��ԃ`���C�����[�v�i�������Ȃ��j
	int PassAlarm; //�ʉ߃`���C��
	int ServiceNotch; //ATS�m�F�m�b�`

	void Initialize(void) //������
	{
		m_dist = 0;
		m_haltChime = ATS_SOUND_STOP;
		m_haltChime2 = ATS_SOUND_STOP;
		m_haltChime3 = ATS_SOUND_STOP;
	}

	void Execute(void)//���t���[�����s
	{
		float speed = fabsf(g_speed);
		float def = speed / 3600 * g_deltaT;
		m_dist -= def; //�c�苗�������Z����

		//�x���炷
		if (speed * speed / SPP_DECELERATION >= m_dist - 50 && m_dist > 0 && !m_played)
		{
			m_played = true;
			if (m_halt) //��Ԃ̏ꍇ
			{
				m_haltChime = ATS_SOUND_PLAY;
				m_haltChime3 = ATS_SOUND_PLAYLOOPING;
				if (g_brakeNotch >= ServiceNotch)
					m_haltChime2 = -1200; //�u���[�L�����߂��ꍇ�͉��ʂ�����������i-12dB�j
				else
					m_haltChime2 = 0;
			}
			else //�ʉ߂̏ꍇ
				m_passAlarm = ATS_SOUND_PLAY;
		}

		//�T�E���h���X�V
		HaltChime = m_haltChime;
		HaltChime2 = m_haltChime2;
		HaltChime3 = m_haltChime3;
		PassAlarm = m_passAlarm;

		//CONTINUE�ɂ��Ă���
		if (m_haltChime == ATS_SOUND_PLAY) { m_haltChime = ATS_SOUND_CONTINUE; }
		if (m_haltChime2 == ATS_SOUND_PLAY) { m_haltChime2 = ATS_SOUND_CONTINUE; }
		if (m_haltChime3 == ATS_SOUND_PLAYLOOPING) { m_haltChime3 = ATS_SOUND_CONTINUE; }
		m_passAlarm = ATS_SOUND_CONTINUE;
	}
	
	void NotchChanged(void)
	{
		// �u���[�L�����߂��ꍇ�͉��ʂ�����������
		if (m_haltChime2 == 0 && g_brakeNotch >= ServiceNotch) { m_haltChime2 = -1200; }
	}
	

	//�J�����Ɏ��s
	void StopChime(void)
	{
		m_haltChime = ATS_SOUND_STOP;
		m_haltChime2 = ATS_SOUND_STOP;
		m_haltChime3 = ATS_SOUND_STOP;
	}

	//#105�Ŏ��s
	void Recieve(int data)
	{
		m_halt = data >= 0;
		m_dist = abs(data) - SPP_OFFSET;
		m_played = false;
		m_haltChime = ATS_SOUND_STOP;
		m_haltChime2 = ATS_SOUND_STOP;
		m_haltChime3 = ATS_SOUND_STOP;
	}
};