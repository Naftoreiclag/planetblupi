// DecBlupi.cpp
//

#include <unordered_map>
#include "gettext.h"
#include "def.h"
#include "decor.h"
#include "action.h"
#include "misc.h"
#include "resource.h"
#include "decgoal.h"

// Cette table donne l'action � effectuer pour un bouton
// enfonc�.
short table_actions[] =
{
	WM_ACTION_GO,
	WM_ACTION_STOP,
	WM_ACTION_MANGE,
	WM_ACTION_CARRY,
	WM_ACTION_DEPOSE,
	WM_ACTION_ABAT1,
	WM_ACTION_ROC1,
	WM_ACTION_CULTIVE,
	WM_ACTION_BUILD1,
	WM_ACTION_BUILD2,
	WM_ACTION_BUILD3,
	WM_ACTION_BUILD4,
	WM_ACTION_BUILD5,
	WM_ACTION_BUILD6,
	WM_ACTION_MUR,
	WM_ACTION_PALIS,
	WM_ACTION_ABAT1,
	WM_ACTION_ROC1,
	WM_ACTION_PONTE,
	WM_ACTION_TOUR,
	WM_ACTION_BOIT,
	WM_ACTION_LABO,
	WM_ACTION_FLEUR1,
	WM_ACTION_FLEUR1,
	WM_ACTION_DYNAMITE,
	WM_ACTION_BATEAUE,
	WM_ACTION_DJEEP,
	WM_ACTION_DRAPEAU,
	WM_ACTION_EXTRAIT,
	WM_ACTION_FABJEEP,
	WM_ACTION_FABMINE,
	WM_ACTION_FABDISC,
	WM_ACTION_REPEAT,
	WM_ACTION_DARMURE,
	WM_ACTION_FABARMURE,
};





// Supprime tous les blupi.

void CDecor::BlupiFlush()
{
	int		i;

	memset(m_blupi, 0, sizeof(Blupi)*MAXBLUPI);

	for ( i=0 ; i<MAXBLUPI ; i++ )
	{
		m_blupi[i].bExist = false;
	}
}

// Cr�e un nouveau blupi, et retourne son rang.

int CDecor::BlupiCreate(POINT cel, int action, int direct,
						int perso, int energy)
{
	int		rank;

	if ( perso == 0 && action == ACTION_STOP &&  // blupi ?
		 energy <= MAXENERGY/4 )
	{
		action = ACTION_STOPf;
	}

	if ( perso == 1 && action == ACTION_STOP )  // araign�e ?
	{
		action = ACTION_A_STOP;
	}

	if ( perso == 2 && action == ACTION_STOP )  // virus ?
	{
		action = ACTION_V_STOP;
	}

	if ( perso == 3 && action == ACTION_STOP )  // tracks ?
	{
		action = ACTION_T_STOP;
	}

	if ( perso == 4 && action == ACTION_STOP )  // robot ?
	{
		action = ACTION_R_STOP;
	}

	if ( perso == 5 && action == ACTION_STOP )  // bombe ?
	{
		action = ACTION_B_STOP;
	}

	if ( perso == 7 && action == ACTION_STOP )  // �lectro ?
	{
		action = ACTION_E_STOP;
	}

	if ( perso == 8 && action == ACTION_STOP )  // disciple ?
	{
		action = ACTION_D_STOP;
	}

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist &&
			 m_blupi[rank].cel.x == cel.x &&
			 m_blupi[rank].cel.y == cel.y )
		{
			m_blupi[rank].aDirect = ((m_blupi[rank].aDirect/16+1)%8)*16;
			m_blupi[rank].sDirect =  m_blupi[rank].aDirect;
			m_blupi[rank].perso   = perso;
			m_blupi[rank].energy  = energy;
			m_blupi[rank].action  = action;
			BlupiActualise(rank);
			return rank;
		}
	}

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( !m_blupi[rank].bExist )
		{
			m_blupi[rank].bExist      = true;
			m_blupi[rank].bHili       = false;
			m_blupi[rank].perso       = perso;
			m_blupi[rank].energy      = energy;
			m_blupi[rank].goalAction  = 0;
			m_blupi[rank].goalCel.x   = -1;
			m_blupi[rank].goalCel.y   = -1;
			m_blupi[rank].cel         = cel;
			m_blupi[rank].fix         = cel;
			m_blupi[rank].action      = action;
			m_blupi[rank].interrupt   = 1;
			m_blupi[rank].aDirect     = direct;
			m_blupi[rank].sDirect     = direct;
			m_blupi[rank].phase       = 0;
			m_blupi[rank].step        = rank*13;  // un peu de hazard !
			m_blupi[rank].channel     = -1;
			m_blupi[rank].icon        = -1;
			m_blupi[rank].lastIcon    = -1;
			m_blupi[rank].pos.x       = 0;
			m_blupi[rank].pos.y       = 0;
			m_blupi[rank].posZ        = 0;
			m_blupi[rank].takeChannel = -1;
			m_blupi[rank].passCel.x   = -1;
			m_blupi[rank].jaugePhase  = -1;
			m_blupi[rank].jaugeMax    = -1;
			m_blupi[rank].stop        = 0;
			m_blupi[rank].bArrow      = false;
			m_blupi[rank].bRepeat     = false;
			m_blupi[rank].bMalade     = false;
			m_blupi[rank].bCache      = false;
			m_blupi[rank].vehicule    = 0;
			m_blupi[rank].busyCount   = 0;
			m_blupi[rank].busyDelay   = 0;
			m_blupi[rank].clicCount   = 0;
			m_blupi[rank].clicDelay   = 0;

			ListFlush(rank);
			FlushUsed(rank);
			BlupiDestCel(rank);
			BlupiPushFog(rank);
			BlupiActualise(rank);
			return rank;
		}
	}

	return -1;
}

// Supprime un blupi existant.
// Si perso == -1, supprime n'importe quel personnage ici.
// Si perso >= 0, supprime seulement ce personnage.

bool CDecor::BlupiDelete(POINT cel, int perso)
{
	int		rank;

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist &&
			 m_blupi[rank].cel.x == cel.x &&
			 m_blupi[rank].cel.y == cel.y &&
			 (perso == -1 || m_blupi[rank].perso == perso) )
		{
			m_blupi[rank].bExist = false;

			if ( !m_bBuild )  // phase de jeu ?
			{
				if ( perso == 6 )  // d�tonnateur invisible ?
				{
					MoveFinish(rank);  // stoppe d�compte � rebourd
				}
				if ( m_nbBlupiHili > 0 &&
					 m_rankBlupiHili == rank )  // est-ce le blupi s�lectionn� ?
				{
					m_nbBlupiHili   = 0;
					m_rankBlupiHili = -1;
				}
			}
			return true;
		}
	}

	return false;
}

// Supprime un blupi existant.

void CDecor::BlupiDelete(int rank)
{
	m_blupi[rank].bExist = false;

	if ( !m_bBuild &&    // phase de jeu ?
		 m_nbBlupiHili > 0 &&
		 m_rankBlupiHili == rank )  // est-ce le blupi s�lectionn� ?
	{
		m_nbBlupiHili   = 0;
		m_rankBlupiHili = -1;
	}
}

// Supprime tout dans un p�rim�tre donn� suite � une explosion.
//	type=0	->	explosion
//	type=1	->	�lectro

void CDecor::BlupiKill(int exRank, POINT cel, int type)
{
	int		rank, action, x, y, icon;

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		// Supprime sans condition les blupi plac�s
		// dans la cellule o� a lieu l'explosion.
		if ( rank != exRank &&
			 m_blupi[rank].bExist &&
			 m_blupi[rank].vehicule != 3 &&  // pas armure ?
			 !m_bInvincible &&
			 m_blupi[rank].cel.x >= cel.x   &&
			 m_blupi[rank].cel.x <= cel.x+1 &&
			 m_blupi[rank].cel.y >= cel.y   &&
			 m_blupi[rank].cel.y <= cel.y+1 )
		{
			GoalUnwork(rank);

			if ( type == 0 )  // explosion ?
			{
				m_blupi[rank].bExist = false;  // mort instantann�e
			}

			if ( type == 1 )  // �lectro ?
			{
				x = m_blupi[rank].cel.x;
				y = m_blupi[rank].cel.y;
				icon = m_decor[x/2][y/2].objectIcon;
				if ( m_blupi[rank].perso == 0 &&
					 m_blupi[rank].vehicule == 0 &&  // � pied ?
					 !m_bInvincible &&
					 icon != 113 &&                 // maison ?
					 icon !=  28 && icon !=  29 &&  // laboratoire ?
					 icon != 119 && icon != 120 &&  // usine ?
					 icon != 121 && icon != 122 )   // mine de fer ?
				{
					if ( m_blupi[rank].bMalade )
					{
						action = WM_ACTION_ELECTROm;
					}
					else
					{
						action = WM_ACTION_ELECTRO;
					}
					GoalStart(rank, action, m_blupi[rank].cel);
//?					BlupiChangeAction(rank, ACTION_ELECTRO);
				}
			}
		}

		// Supprime les blupi plac�s une case autour de la
		// cellule o� a lieu l'explosion, seulement s'ils
		// ne sont pas cach�s (pas dans un batiment).
		if ( type == 0 &&
			 rank != exRank &&
			 m_blupi[rank].bExist &&
			 m_blupi[rank].vehicule != 3 &&  // pas armure ?
			 !m_bInvincible &&
			 !m_blupi[rank].bCache &&  // pas dans un batiment ?
			 m_blupi[rank].cel.x >= cel.x-1 &&
			 m_blupi[rank].cel.x <= cel.x+2 &&
			 m_blupi[rank].cel.y >= cel.y-1 &&
			 m_blupi[rank].cel.y <= cel.y+2 )
		{
			GoalUnwork(rank);
			m_blupi[rank].bExist = false;
		}
	}
}



// Test si un blupi existe.

bool CDecor::BlupiIfExist(int rank)
{
	return !!m_blupi[rank].bExist;
}


// Triche pour tous les blupi.
//	#1	->	(POWER)    redonne l'�nergie maximale
//	#2	->	(LONESOME) tue toutes les araign�es/virus/etc.

void CDecor::BlupiCheat(int cheat)
{
	int		rank;

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( cheat == 1 )  // power ?
		{
			if ( m_blupi[rank].bExist &&
				 m_blupi[rank].perso == 0 )
			{
				m_blupi[rank].energy = MAXENERGY;
				m_blupi[rank].bMalade = false;
			}
		}

		if ( cheat == 2 )  // lonesome ?
		{
			if ( m_blupi[rank].bExist &&
				 m_blupi[rank].perso != 0 &&
				 m_blupi[rank].perso != 8 )  // araign�e/virus/etc. ?
			{
				m_blupi[rank].bExist = false;
			}
		}
	}
}


// Actualise un blupi pour pouvoir le dessiner dans son �tat.

void CDecor::BlupiActualise(int rank)
{
	short	sound;

	Action(m_blupi[rank].action,
		   m_blupi[rank].aDirect,
		   m_blupi[rank].phase,
		   m_blupi[rank].step,
		   m_blupi[rank].channel,
		   m_blupi[rank].icon,
		   m_blupi[rank].pos,
		   m_blupi[rank].posZ,
		   sound);
	BlupiAdaptIcon(rank);

	m_blupi[rank].lastIcon = -1;
	m_blupi[rank].phase    = 0;
	m_blupi[rank].pos.x    = 0;
	m_blupi[rank].pos.y    = 0;
	m_blupi[rank].posZ     = 0;
}

// Adapte une ic�ne.

void CDecor::BlupiAdaptIcon(int rank)
{
	int		direct;

	if ( m_blupi[rank].icon == -1 )  return;

	if ( m_blupi[rank].bMalade )  // malade ?
	{
		if ( m_blupi[rank].icon >= 69 &&
			 m_blupi[rank].icon <= 92 )
		{
			m_blupi[rank].icon += 100;  // 169..192 (tout vert)
		}
		else if ( m_blupi[rank].icon == 270 )  // �cras� ?
		{
			m_blupi[rank].icon = 271;
		}
		else if ( m_blupi[rank].icon < 318 ||  // pas t�l�transport� ?
				  m_blupi[rank].icon > 321 )
		{
			direct = GetIconDirect(m_blupi[rank].icon);
			if ( direct < 0 )
			{
				m_blupi[rank].icon = 169;  // debout direction est !
			}
			else
			{
				m_blupi[rank].icon = 169+3*(direct/16);
			}
		}
	}

	if ( m_blupi[rank].perso == 0 &&
		 m_blupi[rank].vehicule == 1 )  // en bateau ?
	{
		direct = GetIconDirect(m_blupi[rank].icon);
		if ( direct < 0 )
		{
			m_blupi[rank].icon = 234;  // bateau direction est !
		}
		else
		{
			m_blupi[rank].icon = 234+(direct/(16/2));
		}
	}

	if ( m_blupi[rank].perso == 0 &&
		 m_blupi[rank].vehicule == 2 )  // en jeep ?
	{
		direct = GetIconDirect(m_blupi[rank].icon);
		if ( direct < 0 )
		{
			m_blupi[rank].icon = 250;  // bateau direction est !
		}
		else
		{
			m_blupi[rank].icon = 250+(direct/(16/2));
		}
	}

	if ( m_blupi[rank].perso == 0 &&
		 m_blupi[rank].vehicule == 3 )  // armure ?
	{
		if ( m_blupi[rank].icon < 322 ||
			 m_blupi[rank].icon > 347 )
		{
			if ( m_blupi[rank].icon == 106 ||  // �lan pour saut ?
				 m_blupi[rank].icon == 194 )   // m�che ?
			{
				m_blupi[rank].icon = 347;
			}
			else
			{
				direct = GetIconDirect(m_blupi[rank].icon);
				if ( direct < 0 )
				{
					m_blupi[rank].icon = 322;  // armure direction est !
				}
				else
				{
					m_blupi[rank].icon = 322+3*(direct/16);
				}
			}
		}
	}

	if ( m_blupi[rank].perso == 8 )  // disciple ?
	{
		direct = GetIconDirect(m_blupi[rank].icon);
		if ( direct < 0 )
		{
			m_blupi[rank].icon = 290;  // disciple direction est !
		}
		else
		{
			m_blupi[rank].icon = 290+(direct/(16/2));
		}
	}
}


// Fait entendre un son pour un blupi.
// Si bStop=true, on stoppe le son pr�c�dent associ�
// � ce blupi (rank), si n�cessaire.

void CDecor::BlupiSound(int rank, int sound, POINT pos, bool bStop)
{
	int		newSound;

	if ( rank == -1 )
	{
		rank = m_rankBlupiHili;
	}

	if ( rank != -1 && m_blupi[rank].perso == 8 )  // disciple ?
	{
		if ( sound == SOUND_HOP    ||
			 sound == SOUND_BRULE  ||
			 sound == SOUND_TCHAO  ||
			 sound == SOUND_FLEUR  ||
			 sound == SOUND_ARROSE )
		{
			newSound = -1;
		}
		else
		{
			newSound = sound;
		}

		if ( sound == SOUND_BOING  ||
			 sound == SOUND_BOING1 ||
			 sound == SOUND_BOING2 ||
			 sound == SOUND_BOING3 )
		{
			newSound = SOUND_D_BOING;
		}
		if ( sound == SOUND_GO1 ||
			 sound == SOUND_GO2 ||
			 sound == SOUND_GO3 ||
			 sound == SOUND_GO4 ||
			 sound == SOUND_GO5 ||
			 sound == SOUND_GO6 )
		{
			newSound = SOUND_D_GO;
		}
		if ( sound == SOUND_OK1  ||
			 sound == SOUND_OK2  ||
			 sound == SOUND_OK3  ||
			 sound == SOUND_OK4  ||
			 sound == SOUND_OK5  ||
			 sound == SOUND_OK6  ||
			 sound == SOUND_OK1f ||
			 sound == SOUND_OK2f ||
			 sound == SOUND_OK3f ||
			 sound == SOUND_OK1e ||
			 sound == SOUND_OK2e ||
			 sound == SOUND_OK3e )
		{
			newSound = SOUND_D_OK;
		}
		if ( sound == SOUND_TERM1 ||
			 sound == SOUND_TERM2 ||
			 sound == SOUND_TERM3 ||
			 sound == SOUND_TERM4 ||
			 sound == SOUND_TERM5 ||
			 sound == SOUND_TERM6 )
		{
			newSound = SOUND_D_TERM;
		}

		if ( newSound == -1 )  return;
		sound = newSound;
	}

	if ( bStop )
	{
		m_pSound->PlayImage(sound, pos, rank);
	}
	else
	{
		m_pSound->PlayImage(sound, pos);
	}
}

// Sons associ�s � des actions.

static short tableSound[] =
{
	ACTION_BRULE,		SOUND_BRULE,
	ACTION_TCHAO,		SOUND_TCHAO,
	ACTION_MANGE,		SOUND_MANGE,
	ACTION_BOIT,		SOUND_BOIT,
	ACTION_GLISSE,		SOUND_GLISSE,
	ACTION_R_CHARGE,	SOUND_R_CHARGE,
	-1
};

// Effectue quelques initialisations pour une nouvelle action.

void CDecor::BlupiInitAction(int rank, int action, int direct)
{
	short*		pTable = tableSound;
	POINT		pos;
	int			rand;

	while ( *pTable != -1 )
	{
		if ( pTable[0] == action )
		{
			pos = ConvCelToPos(m_blupi[rank].cel);
			BlupiSound(rank, pTable[1], pos);
			break;
		}

		pTable += 2;
	}

	m_blupi[rank].action = action;

	if ( m_blupi[rank].perso == 0 )  // blupi ?
	{
		if ( m_blupi[rank].vehicule == 1 )  // en bateau ?
		{
			if ( m_blupi[rank].action == ACTION_STOP )
			{
				m_blupi[rank].action = ACTION_STOPb;
			}

			if ( m_blupi[rank].action == ACTION_MARCHE )
			{
				m_blupi[rank].action = ACTION_MARCHEb;
			}
		}

		if ( m_blupi[rank].vehicule == 2 )  // en jeep ?
		{
			if ( m_blupi[rank].action == ACTION_STOP )
			{
				m_blupi[rank].action = ACTION_STOPj;
			}

			if ( m_blupi[rank].action == ACTION_MARCHE )
			{
				m_blupi[rank].action = ACTION_MARCHEj;
			}
		}

		if ( m_blupi[rank].vehicule == 3 )  // armure ?
		{
			if ( m_blupi[rank].action == ACTION_STOP )
			{
				m_blupi[rank].action = ACTION_STOPa;
			}

			if ( m_blupi[rank].action == ACTION_MARCHE )
			{
				m_blupi[rank].action = ACTION_MARCHEa;
				m_blupi[rank].step = (m_blupi[rank].step/2)*2;
			}
		}

		if ( m_blupi[rank].energy <= MAXENERGY/4 )  // peu de forces ?
		{
			if ( m_blupi[rank].action == ACTION_STOP )
			{
				m_blupi[rank].action = ACTION_STOPf;
			}

			if ( m_blupi[rank].action == ACTION_MARCHE )
			{
				m_blupi[rank].action = ACTION_MARCHEf;
				m_blupi[rank].step = (m_blupi[rank].step/2)*2;
			}
		}

		if ( m_blupi[rank].action == ACTION_STOP &&
			 m_blupi[rank].goalAction == 0 )  // � pied ?
		{
			rand = Random(0,400);
			if ( rand >= 10 && rand <= 15 &&
				 m_blupi[rank].takeChannel == -1 )
			{
				m_blupi[rank].action = ACTION_MISC1;  // �paules
				m_blupi[rank].step = 0;
			}
			if ( rand >= 20 && rand <= 23 )
			{
				m_blupi[rank].action = ACTION_MISC2;  // grat-grat
				m_blupi[rank].step = 0;
			}
			if ( rand == 30 &&
				 m_blupi[rank].takeChannel == -1 )
			{
				m_blupi[rank].action = ACTION_MISC3;  // yoyo
				m_blupi[rank].step = 0;
			}
			if ( rand >= 40 && rand <= 45 )
			{
				m_blupi[rank].action = ACTION_MISC4;  // ferme les yeux
				m_blupi[rank].step = 0;
			}
			if ( rand == 50 &&
				 m_blupi[rank].takeChannel == -1 )
			{
				m_blupi[rank].action = ACTION_MISC5;  // oh�
				m_blupi[rank].step = 0;
			}
			if ( rand == 60 )
			{
				m_blupi[rank].action = ACTION_MISC6;  // diabolo
				m_blupi[rank].step = 0;
			}
		}
		if ( m_blupi[rank].action == ACTION_STOPf &&
			 m_blupi[rank].goalAction == 0 )
		{
			rand = Random(0,100);
			if ( rand == 10 &&  // propabilit� 1/100
				 m_blupi[rank].takeChannel == -1 )
			{
				m_blupi[rank].action = ACTION_MISC1f;
				m_blupi[rank].step = 0;
			}
		}

		if ( direct != -1 )
		{
			m_blupi[rank].sDirect = direct;
		}

		if ( m_blupi[rank].action == ACTION_BUILD        ||
			 m_blupi[rank].action == ACTION_BUILDBREF    ||
			 m_blupi[rank].action == ACTION_BUILDSEC     ||
			 m_blupi[rank].action == ACTION_BUILDSOURD   ||
			 m_blupi[rank].action == ACTION_BUILDPIERRE  ||
			 m_blupi[rank].action == ACTION_PIOCHE       ||
			 m_blupi[rank].action == ACTION_PIOCHEPIERRE ||
			 m_blupi[rank].action == ACTION_PIOCHESOURD  ||
			 m_blupi[rank].action == ACTION_ARROSE       ||
			 m_blupi[rank].action == ACTION_BECHE        ||
			 m_blupi[rank].action == ACTION_SCIE         ||
			 m_blupi[rank].action == ACTION_TAKE         ||
			 m_blupi[rank].action == ACTION_DEPOSE       ||
			 m_blupi[rank].action == ACTION_BRULE        ||
			 m_blupi[rank].action == ACTION_TCHAO        ||
			 m_blupi[rank].action == ACTION_GRILLE1      ||
			 m_blupi[rank].action == ACTION_GRILLE2      ||
			 m_blupi[rank].action == ACTION_GRILLE3      ||
			 m_blupi[rank].action == ACTION_ELECTRO      ||
			 m_blupi[rank].action == ACTION_MANGE        ||
			 m_blupi[rank].action == ACTION_BOIT         ||
			 m_blupi[rank].action == ACTION_NAISSANCE    ||
			 m_blupi[rank].action == ACTION_SAUTE1       ||
			 m_blupi[rank].action == ACTION_SAUTE2       ||
			 m_blupi[rank].action == ACTION_SAUTE3       ||
			 m_blupi[rank].action == ACTION_SAUTE4       ||
			 m_blupi[rank].action == ACTION_SAUTE5       ||
//?			 m_blupi[rank].action == ACTION_GLISSE       ||
			 m_blupi[rank].action == ACTION_PONT         ||
			 m_blupi[rank].action == ACTION_MECHE        ||
			 m_blupi[rank].action == ACTION_A_GRILLE     ||
			 m_blupi[rank].action == ACTION_V_GRILLE     ||
			 m_blupi[rank].action == ACTION_T_ECRASE     ||
			 m_blupi[rank].action == ACTION_R_MARCHE     ||
			 m_blupi[rank].action == ACTION_R_APLAT      ||
			 m_blupi[rank].action == ACTION_R_BUILD      ||
			 m_blupi[rank].action == ACTION_R_CHARGE     ||
			 m_blupi[rank].action == ACTION_B_MARCHE     ||
			 m_blupi[rank].action == ACTION_E_MARCHE     ||
			 m_blupi[rank].action == ACTION_MARCHEb      ||
			 m_blupi[rank].action == ACTION_MARCHEj      ||
			 m_blupi[rank].action == ACTION_TELEPORTE1   ||
			 m_blupi[rank].action == ACTION_TELEPORTE2   ||
			 m_blupi[rank].action == ACTION_TELEPORTE3   ||
			 m_blupi[rank].action == ACTION_ARMUREOPEN   ||
			 m_blupi[rank].action == ACTION_ARMURECLOSE  )
		{
			m_blupi[rank].step = 0;
		}
	}

	if ( m_blupi[rank].perso == 1 )  // araign�e ?
	{
		if ( m_blupi[rank].action == ACTION_MARCHE )
		{
			m_blupi[rank].action = ACTION_A_MARCHE;
		}

		if ( m_blupi[rank].action == ACTION_STOP )
		{
			m_blupi[rank].action = ACTION_A_STOP;
			m_blupi[rank].sDirect = Random(0,7)*16;
		}

		m_blupi[rank].step = 0;
	}

	if ( m_blupi[rank].perso == 2 )  // virus ?
	{
		if ( m_blupi[rank].action == ACTION_MARCHE )
		{
			m_blupi[rank].action = ACTION_V_MARCHE;
		}

		if ( m_blupi[rank].action == ACTION_STOP )
		{
			m_blupi[rank].action = ACTION_V_STOP;
		}

		m_blupi[rank].step = 0;
	}

	if ( m_blupi[rank].perso == 3 )  // tracks ?
	{
		if ( m_blupi[rank].action == ACTION_MARCHE )
		{
			m_blupi[rank].action = ACTION_T_MARCHE;
		}

		if ( m_blupi[rank].action == ACTION_STOP )
		{
			m_blupi[rank].action = ACTION_T_STOP;
		}

		m_blupi[rank].step = 0;
	}

	if ( m_blupi[rank].perso == 4 )  // robot ?
	{
		if ( m_blupi[rank].action == ACTION_MARCHE )
		{
			m_blupi[rank].action = ACTION_R_MARCHE;
		}

		if ( m_blupi[rank].action == ACTION_STOP )
		{
			m_blupi[rank].action = ACTION_R_STOP;
		}

		m_blupi[rank].step = 0;
	}

	if ( m_blupi[rank].perso == 5 )  // bombe ?
	{
		if ( m_blupi[rank].action == ACTION_MARCHE )
		{
			m_blupi[rank].action = ACTION_B_MARCHE;
		}

		if ( m_blupi[rank].action == ACTION_STOP )
		{
			m_blupi[rank].action = ACTION_B_STOP;
		}

		m_blupi[rank].step = 0;
	}

	if ( m_blupi[rank].perso == 7 )  // �lectro ?
	{
		if ( m_blupi[rank].action == ACTION_MARCHE )
		{
			m_blupi[rank].action = ACTION_E_MARCHE;
		}

		if ( m_blupi[rank].action == ACTION_STOP )
		{
			m_blupi[rank].action = ACTION_E_STOP;
		}

		m_blupi[rank].step = 0;
	}

	if ( m_blupi[rank].perso == 8 )  // disciple ?
	{
		if ( direct != -1 )
		{
			m_blupi[rank].sDirect = direct;
		}

		if ( m_blupi[rank].action == ACTION_MARCHE )
		{
			m_blupi[rank].action = ACTION_D_MARCHE;
		}

		if ( m_blupi[rank].action == ACTION_STOP )
		{
			m_blupi[rank].action = ACTION_D_STOP;
		}

		if ( m_blupi[rank].action == ACTION_PIOCHE )
		{
			m_blupi[rank].action = ACTION_D_PIOCHE;
		}

		if ( m_blupi[rank].action == ACTION_BUILD )
		{
			m_blupi[rank].action = ACTION_D_BUILD;
		}

		if ( m_blupi[rank].action == ACTION_SCIE )
		{
			m_blupi[rank].action = ACTION_D_SCIE;
		}

		if ( m_blupi[rank].action == ACTION_TCHAO )
		{
			m_blupi[rank].action = ACTION_D_TCHAO;
		}

		if ( m_blupi[rank].action == ACTION_CUEILLE1 )
		{
			m_blupi[rank].action = ACTION_D_CUEILLE1;
		}

		if ( m_blupi[rank].action == ACTION_CUEILLE2 )
		{
			m_blupi[rank].action = ACTION_D_CUEILLE2;
		}

		if ( m_blupi[rank].action == ACTION_MECHE )
		{
			m_blupi[rank].action = ACTION_D_MECHE;
		}

		if ( m_blupi[rank].action == ACTION_ARROSE )
		{
			m_blupi[rank].action = ACTION_D_ARROSE;
		}

		if ( m_blupi[rank].action == ACTION_BECHE )
		{
			m_blupi[rank].action = ACTION_D_BECHE;
		}

		m_blupi[rank].step = 0;
	}
}

// Change l'action de blupi.

void CDecor::BlupiChangeAction(int rank, int action, int direct)
{
	if ( rank < 0 )  return;
	BlupiInitAction(rank, action, direct);
	BlupiDestCel(rank);
	m_blupi[rank].phase = 0;
	m_blupi[rank].pos.x = 0;
	m_blupi[rank].pos.y = 0;
	BlupiNextAction(rank);
}



// Vide la liste des actions.

void CDecor::ListFlush(int rank)
{
	int		i;

	for ( i=0 ; i<MAXLIST ; i++ )
	{
		m_blupi[rank].listButton[i] = -1;
	}
	m_blupi[rank].repeatLevelHope = -1;
	m_blupi[rank].repeatLevel     = -1;
}

// Retourne le param�tre associ� � une action.

int CDecor::ListGetParam(int rank, int button, POINT cel)
{
	int		icon;

	if ( button == BUTTON_CARRY ||
		 button == BUTTON_FLEUR )
	{
		return m_decor[cel.x/2][cel.y/2].objectIcon;
	}

	if ( button == BUTTON_DEPOSE )
	{
		return m_blupi[rank].takeIcon;
	}

	if ( button == BUTTON_GO &&
		 cel.x%2 == 1 && cel.y%2 == 1 )
	{
		icon = m_decor[cel.x/2][cel.y/2].objectIcon;
		if ( icon == 117 ||  // bateau ?
			 icon == 118 )   // voiture ?
		{
			return icon;
		}
	}

	return -1;
}

// Ajoute une action dans la liste.

bool CDecor::ListPut(int rank, int button, POINT cel, POINT cMem)
{
	int		i, last;

	if ( button == BUTTON_REPEAT ||
		 button == BUTTON_GO     )  return true;

	// M�morise "mange" seulement apr�s un "cultive".
	if ( button == BUTTON_MANGE &&
		 m_blupi[rank].listButton[0] != BUTTON_CULTIVE )  return true;

	// Si prend/d�pose � la suite au m�me endroit,
	// il est inutile de m�moriser !
	last = m_blupi[rank].listButton[0];
	if ( (button == BUTTON_CARRY  && last == BUTTON_DEPOSE) ||
		 (button == BUTTON_DEPOSE && last == BUTTON_CARRY ) )
	{
		if ( cel.x/2 == m_blupi[rank].listCel[0].x/2 &&
			 cel.y/2 == m_blupi[rank].listCel[0].y/2 )
		{
			ListRemove(rank);
			return true;
		}
	}

	for ( i=MAXLIST-1 ; i>0 ; i-- )
	{
		m_blupi[rank].listButton[i] = m_blupi[rank].listButton[i-1];
		m_blupi[rank].listCel[i]    = m_blupi[rank].listCel[i-1];
		m_blupi[rank].listParam[i]  = m_blupi[rank].listParam[i-1];
	}

	m_blupi[rank].listButton[0] = button;
	m_blupi[rank].listCel[0]    = cMem;
	m_blupi[rank].listParam[0]  = ListGetParam(rank, button, cel);

	return true;
}

// Enl�ve la derni�re action ajout�e dans la liste.

void CDecor::ListRemove(int rank)
{
	int		i;

	if ( m_blupi[rank].listButton[0] == BUTTON_CULTIVE )  return;

	for ( i=0 ; i<MAXLIST-1 ; i++ )
	{
		m_blupi[rank].listButton[i] = m_blupi[rank].listButton[i+1];
		m_blupi[rank].listCel[i]    = m_blupi[rank].listCel[i+1];
		m_blupi[rank].listParam[i]  = m_blupi[rank].listParam[i+1];
	}

	m_blupi[rank].listButton[MAXLIST-1] = -1;
}

// Cherche une action � r�p�ter dans la liste.
// Retourne la profondeur de la r�p�tition.
// Retourne -1 si aucune r�p�titon n'est possible.

int CDecor::ListSearch(int rank, int button, POINT cel,
					   const char *&textForButton)
{
	int		i, j, param, nb;

	static const char *errors[] = {
		/*  0 */ translate ("1: Grow tomatoes\n2: Eat"),
		/*  1 */ translate ("1: Make a bunch\n2: Transform"),
		/*  2 */ translate ("1: Take\n2: Transform"),
		/*  3 */ translate ("1: Extract iron\n2: Make a bomb"),
		/*  4 */ translate ("1: Extract iron\n2: Make a Jeep"),
		/*  5 */ translate ("1: Extract iron\n2: Make an armour"),
		/*  6 */ translate ("1: Cut down a tree \n2: Make a palisade"),
		/*  7 */ translate ("1: Take\n2: Build palisade"),
		/*  8 */ translate ("1: Cut down a tree \n2: Build a bridge"),
		/*  9 */ translate ("1: Take\n2: Build a bridge"),
		/* 10 */ translate ("1: Cut down a tree \n2: Make a boat"),
		/* 11 */ translate ("1: Take\n2: Make a boat"),
	};

	static int table_series[] =
	{
		0, // errors
		2, BUTTON_CULTIVE, BUTTON_MANGE,

		1, // errors
		4, BUTTON_FLEUR, BUTTON_CARRY, BUTTON_LABO, BUTTON_DEPOSE,

		2, // errors
		3, BUTTON_CARRY, BUTTON_LABO, BUTTON_DEPOSE,

		3, // errors
		3, BUTTON_EXTRAIT, BUTTON_FABMINE, BUTTON_DEPOSE,

		4, // errors
		3, BUTTON_EXTRAIT, BUTTON_FABJEEP, BUTTON_DJEEP,

		5, // errors
		3, BUTTON_EXTRAIT, BUTTON_FABARMURE, BUTTON_DARMURE,

		6, // errors
		4, BUTTON_ABAT, BUTTON_CARRY, BUTTON_DEPOSE, BUTTON_PALIS,

		7, // errors
		3, BUTTON_CARRY, BUTTON_DEPOSE, BUTTON_PALIS,

		8, // errors
		4, BUTTON_ABAT, BUTTON_CARRY, BUTTON_DEPOSE, BUTTON_PONT,

		9, // errors
		3, BUTTON_CARRY, BUTTON_DEPOSE, BUTTON_PONT,

		10, // errors
		4, BUTTON_ABAT, BUTTON_CARRY, BUTTON_DEPOSE, BUTTON_BATEAU,

		11, // errors
		3, BUTTON_CARRY, BUTTON_DEPOSE, BUTTON_BATEAU,

		-1,
	};

	param = ListGetParam(rank, button, cel);

	i = 0;
	while ( table_series[i] != -1 )
	{
		nb = table_series[i+1];
		if ( button == m_blupi[rank].listButton[nb-1]   &&
			 param  == m_blupi[rank].listParam[nb-1]    &&
			 cel.x  >= m_blupi[rank].listCel[nb-1].x-50 &&
			 cel.x  <= m_blupi[rank].listCel[nb-1].x+50 &&
			 cel.y  >= m_blupi[rank].listCel[nb-1].y-50 &&
			 cel.y  <= m_blupi[rank].listCel[nb-1].y+50 )
		{
			for ( j=0 ; j<nb ; j++ )
			{
				if ( table_series[i+2+j] !=
					 m_blupi[rank].listButton[nb-1-j] )
				{
					goto next;
				}
			}
			textForButton = gettext (errors[table_series[i]]);
			return nb-1;
		}

		next:
		i += nb+2;
	}

#if 0
	for ( i=0 ; i<MAXLIST ; i++ )
	{
		if ( button == m_blupi[rank].listButton[i]   &&
			 param  == m_blupi[rank].listParam[i]    &&
			 cel.x  >= m_blupi[rank].listCel[i].x-50 &&
			 cel.x  <= m_blupi[rank].listCel[i].x+50 &&
			 cel.y  >= m_blupi[rank].listCel[i].y-50 &&
			 cel.y  <= m_blupi[rank].listCel[i].y+50 )
		{
			return i;
		}
	}
#endif

	return -1;
}

// Ajuste une action � r�p�ter.

bool CDecor::RepeatAdjust(int rank, int button,
						  POINT &cel, POINT &cMem, int param, int list)
{
	int		i, channel, icon, icon1, icon2, flags;
	POINT	test;

	static int	table_object[] =
	{
		BUTTON_ABAT,	CHOBJECT,   6,  11,
		BUTTON_ROC,		CHOBJECT,  37,  43,
		BUTTON_MANGE,	CHOBJECT,  60,  60,
		BUTTON_PALIS,	CHOBJECT,  36,  36,
		BUTTON_BATEAU,	CHOBJECT,  36,  36,
		BUTTON_DEPOSE,	-1,        -1,  -1,
		BUTTON_DJEEP,	-1,        -1,  -1,
		BUTTON_DARMURE,	-1,        -1,  -1,
		0,
	};

	static int table_mur[] =
	{
		+2, 0,		// 1<<0
		 0,+2,		// 1<<1
		-2, 0,		// 1<<2
		 0,-2,		// 1<<3
	};

	if ( button == BUTTON_DEPOSE &&  // d�pose pour une palissade ?
		 list > 0 &&
		 m_blupi[rank].listButton[list-1] == BUTTON_PALIS )
	{
		icon = m_decor[cel.x/2][cel.y/2].objectIcon;

		flags = 0;
		if ( icon == 65 )  flags = (1<<0)|(1<<2);
		if ( icon == 66 )  flags = (1<<1)|(1<<3);
		if ( icon == 67 )  flags = (1<<0)|(1<<1);
		if ( icon == 68 )  flags = (1<<1)|(1<<2);
		if ( icon == 69 )  flags = (1<<2)|(1<<3);
		if ( icon == 70 )  flags = (1<<0)|(1<<3);
		if ( icon == 71 )
		{
			for ( i=0 ; i<4 ; i++ )
			{
				test.x = cel.x + table_mur[i*2+0];
				test.y = cel.y + table_mur[i*2+1];
				if ( IsValid(test) &&
					 m_decor[test.x/2][test.y/2].floorIcon == 15 &&  // dalle grise ?
					 CelOkForAction(test, WM_ACTION_DEPOSE, rank) == 0 )
				{
					cel  = test;
					cMem = test;
					goto ok;
				}
			}
			flags = (1<<0)|(1<<1)|(1<<2)|(1<<3);
		}

		if ( flags == 0 )
		{
			if ( CelOkForAction(cel, WM_ACTION_DEPOSE, rank) == 0 )
			{
				goto ok;
			}
		}

		for ( i=0 ; i<4 ; i++ )
		{
			if ( flags & (1<<i) )
			{
				test.x = cel.x + table_mur[i*2+0];
				test.y = cel.y + table_mur[i*2+1];
				if ( CelOkForAction(test, WM_ACTION_DEPOSE, rank) == 0 )
				{
					cel  = test;
					cMem = test;
					goto ok;
				}
			}
		}
		return false;
	}

	if ( button == BUTTON_DEPOSE &&  // d�pose pour un bateau ?
		 list > 0 &&
		 m_blupi[rank].listButton[list-1] == BUTTON_BATEAU )
	{
		if ( SearchOtherBateau(rank, cel, 100, test, icon) )
		{
			cel  = test;
			cMem = test;
			goto ok;
		}
		return false;
	}

//?	if ( button == BUTTON_MANGE )
//?	{
//?		cel = m_blupi[rank].cel;  // cherche l� o� est blupi !
//?	}

	i = 0;
	channel = -2;
	icon1   = -1;
	icon2   = -1;
	while ( table_object[i] != 0 )
	{
		if ( button == table_object[i] )
		{
			channel = table_object[i+1];
			icon1   = table_object[i+2];
			icon2   = table_object[i+3];
			break;
		}
		i += 4;
	}

	if ( button == BUTTON_CARRY ||
		 button == BUTTON_FLEUR )
	{
		channel = CHOBJECT;
		icon1   = param;
		icon2   = param;
	}
	if ( button == BUTTON_GO &&  // va en bateau/jeep ?
		 param != -1 )
	{
		channel = CHOBJECT;
		icon1   = param;
		icon2   = param;
	}

	if ( channel != -2 )
	{
		if ( !SearchOtherObject(rank, cel, table_actions[button],
								50*2, channel,
								icon1, icon2, -1, -1,
								cel, icon1) )
		{
			return false;
		}
	}

	if ( button == BUTTON_PALIS )
	{
		cMem = cel;
	}

	ok:
	if ( cel.x%2 == 0 )  cel.x ++;
	if ( cel.y%2 == 0 )  cel.y ++;  // sur l'objet

	m_blupi[rank].interrupt = 1;
	return true;
}



// D�marre une action.

void CDecor::GoalStart(int rank, int action, POINT cel)
{
	m_blupi[rank].goalHili   = cel;
	m_blupi[rank].goalAction = action;
	m_blupi[rank].goalPhase  = 0;
	m_blupi[rank].goalCel.x  = -1;
	m_blupi[rank].bRepeat    = false;

	GoalInitJauge(rank);
	FlushUsed(rank);
}

// Effectue la m�ta op�ration suivante.
// Retourne false lorsque c'est fini !

bool CDecor::GoalNextPhase(int rank)
{
	short*	pTable;
	int		i, nb;

	if ( m_blupi[rank].goalAction == 0 )  return false;

	pTable = GetTableGoal(m_blupi[rank].goalAction);
	if ( pTable == nullptr )
	{
		GoalStop(rank, true);
		return false;
	}

	for ( i=0 ; i<m_blupi[rank].goalPhase ; i++ )
	{
		if ( *pTable == 0 )  return false;
		pTable += 1+table_goal_nbop[*pTable];
	}

	if ( *pTable == GOAL_GROUP )
	{
		m_blupi[rank].goalPhase ++;
		nb = pTable[1];
		pTable += 2;

		for ( i=0 ; i<nb ; i++ )
		{
			m_blupi[rank].goalPhase ++;
			if ( !GoalNextOp(rank, pTable) )  return false;
			pTable += 1+table_goal_nbop[*pTable];
		}
	}

	m_blupi[rank].goalPhase ++;
	return GoalNextOp(rank, pTable);
}

// Initialise la jauge pour une m�ta op�ration.

void CDecor::GoalInitJauge(int rank)
{
	short*	pTable;
	int		max=0, op;

	m_blupi[rank].jaugePhase = -1;
	m_blupi[rank].jaugeMax   = -1;

	if ( m_blupi[rank].perso != 0 &&
		 m_blupi[rank].perso != 8 )  return;  // araign�e/virus/etc. ?
	if ( m_blupi[rank].goalAction == 0 )  return;

	pTable = GetTableGoal(m_blupi[rank].goalAction);
	if ( pTable == nullptr )  goto term;

	while ( true )
	{
		op = *pTable;
		if ( op == 0 ||
			 op == GOAL_TERM )  goto term;

		if ( op == GOAL_ACTION &&
			 pTable[1] != ACTION_STOP )
		{
			max ++;
		}

		pTable += 1+table_goal_nbop[*pTable];
	}

	term:
	if ( max > 0 )
	{
		m_blupi[rank].jaugePhase = 0;
		m_blupi[rank].jaugeMax   = max;
	}
}

// Permet de passer � travers certains arbres.

void CDecor::GoalInitPassCel(int rank)
{
	POINT		cel;
	int			channel, icon;

	cel.x = (m_blupi[rank].goalCel.x/2)*2;
	cel.y = (m_blupi[rank].goalCel.y/2)*2;

	GetObject(cel, channel, icon);

	if ( channel == CHOBJECT &&
		 ((icon >=  8 && icon <= 11) ||  // arbres touffus ?
		  (icon >= 30 && icon <= 35) ||  // arbres touffus sans feuilles ?
		  (icon >= 37 && icon <= 43) ||  // rochers ?
		  (icon == 81 || icon == 83 || icon == 94) ||  // fleurs ?
		  (icon >= 100 && icon <= 105) ||  // usine ?
		  (icon == 115 || icon == 116) ||  // usine ?
		  (icon ==  17 || icon ==  18) ||  // usine ?
		  (icon == 117) ||  // bateau ?
		  (icon == 118) ||  // jeep ?
		  (icon ==  16)) )  // armure ?
	{
		m_blupi[rank].passCel = m_blupi[rank].goalCel;
	}
	else
	{
		m_blupi[rank].passCel.x = -1;
	}
}

// Ajuste une coordonn�e de cellule.

void CDecor::GoalAdjustCel(int rank, int &x, int &y)
{
	if ( x == -10 && y == -10 )
	{
		if ( m_blupi[rank].goalAction == WM_ACTION_PONTEL )
		{
			x = m_blupi[rank].fix.x + m_blupi[rank].cLoop*2;
			y = m_blupi[rank].fix.y;
			return;
		}
		if ( m_blupi[rank].goalAction == WM_ACTION_PONTOL )
		{
			x = m_blupi[rank].fix.x - m_blupi[rank].cLoop*2;
			y = m_blupi[rank].fix.y;
			return;
		}
		if ( m_blupi[rank].goalAction == WM_ACTION_PONTSL )
		{
			x = m_blupi[rank].fix.x;
			y = m_blupi[rank].fix.y + m_blupi[rank].cLoop*2;
			return;
		}
		if ( m_blupi[rank].goalAction == WM_ACTION_PONTNL )
		{
			x = m_blupi[rank].fix.x;
			y = m_blupi[rank].fix.y - m_blupi[rank].cLoop*2;
			return;
		}
	}

	x += m_blupi[rank].cel.x;
	y += m_blupi[rank].cel.y;
}

// Liste des buts multiples.

int table_multi_goal[16*2] =
{
	 0, 0,
	+1, 0,
	 0,+1,
	+1,+1,
	 0,-1,
	+1,-1,
	 0,+2,
	+1,+2,
	-1,-1,
	-1, 0,
	-1,+1,
	-1,+2,
	+2,-1,
	+2, 0,
	+2,+1,
	+2,+2,
};

// Effectue une m�ta op�ration.

bool CDecor::GoalNextOp(int rank, short *pTable)
{
	int			op, x, y;
	int			action, direct, channel, icon, mchannel, micon;
	int			total, step, delai, first, last, first2, last2, flag, i;
	int			button, param;
	POINT		pos, cel, cMem, destCel;
	bool		bOK, bError=true;

	pos = ConvCelToPos(m_blupi[rank].cel);

	op = *pTable++;

	if ( op == GOAL_ACTION &&
		 *pTable != ACTION_STOP )
	{
		m_blupi[rank].jaugePhase ++;
	}

	if ( op == GOAL_GOHILI )
	{
		m_blupi[rank].goalCel.x = m_blupi[rank].goalHili.x+(*pTable++);
		m_blupi[rank].goalCel.y = m_blupi[rank].goalHili.y+(*pTable++);
		flag = *pTable++;
//?		m_blupi[rank].passCel.x = -1;
		FlushUsed(rank);
		return true;
	}

	if ( op == GOAL_GOHILI2 )
	{
		cel.x = (m_blupi[rank].goalHili.x/2)*2+(*pTable++);
		cel.y = (m_blupi[rank].goalHili.y/2)*2+(*pTable++);
		flag = *pTable++;
		if ( !!flag )
		{
			m_blupi[rank].goalCel = cel;
			GoalInitPassCel(rank);
		}
		m_blupi[rank].goalCel = cel;
		FlushUsed(rank);
		return true;
	}

	if ( op == GOAL_GOBLUPI )
	{
		cel.x = m_blupi[rank].cel.x+(*pTable++);
		cel.y = m_blupi[rank].cel.y+(*pTable++);
		flag = *pTable++;
		if ( !!flag )
		{
			if ( IsBlupiHereEx(cel, rank, false) )  // destination occup�e ?
			{
				m_blupi[rank].goalPhase --;  // on attend ...
				return true;
			}
			m_blupi[rank].goalCel = cel;
			GoalInitPassCel(rank);
		}
		m_blupi[rank].goalCel = cel;
		FlushUsed(rank);
		return true;
	}

	if ( op == GOAL_TESTOBJECT )
	{
		cel.x = m_blupi[rank].cel.x+(*pTable++);
		cel.y = m_blupi[rank].cel.y+(*pTable++);
		channel  = *pTable++;
		icon     = *pTable++;
		GetObject(cel, mchannel, micon);
		if ( channel != mchannel || icon != micon )  goto error;
		return true;
	}

	if ( op == GOAL_PUTFLOOR )
	{
		x = *pTable++;
		y = *pTable++;
		GoalAdjustCel(rank, x,y);
		channel = *pTable++;
		icon    = *pTable++;
		if ( icon == -2 )  icon = m_blupi[rank].vIcon;
		PutFloor(GetCel(x,y), channel, icon);
		return true;
	}

	if ( op == GOAL_PUTOBJECT )
	{
		x = *pTable++;
		y = *pTable++;
		GoalAdjustCel(rank, x,y);
		channel = *pTable++;
		icon    = *pTable++;
		if ( channel == -3 && icon == -3 )  // l'objet transport� ?
		{
			channel = m_blupi[rank].takeChannel;
			icon    = m_blupi[rank].takeIcon;
		}
		PutObject(GetCel(x,y), channel, icon);
		return true;
	}

	if ( op == GOAL_BUILDFLOOR )
	{
		x = *pTable++;
		y = *pTable++;
		GoalAdjustCel(rank, x,y);
		cel.x = (x/2)*2;
		cel.y = (y/2)*2;
		GetFloor(cel, channel, i);
		channel  = *pTable++;
		icon     = *pTable++;
		mchannel = *pTable++;
		micon    = *pTable++;
		total    = *pTable++;
		delai    = *pTable++;
		step     = *pTable++;

		if ( i >= 19 && i <= 32 &&  // herbe fonc�e ?
			 icon == 57 )  icon = 58;  // sol tomate fonc�
		if ( i == 58 &&
			 icon == 1 )  icon = 20;  // remet herbe fonc�e

		if ( !MoveCreate(cel, rank, true,
						 channel, icon,
						 mchannel, micon,
						 total, delai, step) )  goto error;
		return true;
	}

	if ( op == GOAL_BUILDOBJECT )
	{
		x = *pTable++;
		y = *pTable++;
		GoalAdjustCel(rank, x,y);
		cel.x = (x/2)*2;
		cel.y = (y/2)*2;
		channel  = *pTable++;
		icon     = *pTable++;
		mchannel = *pTable++;
		micon    = *pTable++;
		total    = *pTable++;
		delai    = *pTable++;
		step     = *pTable++;
		if ( channel == -2 && icon == -2 )  // l'objet actuel ?
		{
			GetObject(cel, channel, icon);
		}
		ArrangeBuild(cel, channel, icon);  // arrange les murs autour
		if ( !MoveCreate(cel, rank, false,
						 channel, icon,
						 mchannel, micon,
						 total, delai, step) )  goto error;
		return true;
	}

	if ( op == GOAL_FINISHMOVE )
	{
		MoveFinish(rank);
		return true;
	}

	if ( op == GOAL_ARRANGEOBJECT )
	{
		x = *pTable++;
		y = *pTable++;
		GoalAdjustCel(rank, x,y);
		MoveFinish(GetCel(x,y));
		ArrangeObject(GetCel(x,y));
		return true;
	}

	if ( op == GOAL_EXPLOSE1 )
	{
		x = *pTable++;
		y = *pTable++;
		GoalAdjustCel(rank, x,y);
		cel = GetCel(x,y);

		BlupiKill(rank, cel, 0);
		MoveFinish(cel);

		// Faut-il d�marrer une explosion en cha�ne.
		GetObject(cel, channel, icon);
		if ( channel == CHOBJECT && icon == 85 )  // dynamite ?
		{
			rank = BlupiCreate(GetCel(cel,1,1), ACTION_STOP, DIRECT_E,
							   6, MAXENERGY);  // cr�e un d�tonnateur
			if ( rank >= 0 )
			{
				GoalStart(rank, WM_ACTION_T_DYNAMITE, cel);
				m_blupi[rank].bCache = true;
			}
		}
		else
		{
			PutObject(cel, -1,-1);  // supprime l'objet
			ArrangeObject(cel);
		}
		return true;
	}

	if ( op == GOAL_EXPLOSE2 )
	{
		x = *pTable++;
		y = *pTable++;
		GoalAdjustCel(rank, x,y);
		cel = GetCel((x/2)*2,(y/2)*2);

		GetObject(cel, channel, icon);
		if ( channel != CHOBJECT || icon != 85 )  // dynamite ?
		{
			channel = CHOBJECT;
			icon    = -1;
			ArrangeBuild(cel, channel, icon);  // arrange les murs autour
			if ( !MoveCreate(cel, rank, false,
							 CHOBJECT,-1, -1,-1,
							 10, 1, -1*100) )  goto error;
			MoveAddIcons(cel, 6);  // explosion
		}
		return true;
	}

	if ( op == GOAL_ADDMOVES )
	{
		x = *pTable++;
		y = *pTable++;
		GoalAdjustCel(rank, x,y);
		icon = *pTable++;
		MoveAddMoves(GetCel(x,y), icon);
		return true;
	}

	if ( op == GOAL_ADDICONS )
	{
		x = *pTable++;
		y = *pTable++;
		GoalAdjustCel(rank, x,y);
		icon = *pTable++;
		MoveAddIcons(GetCel(x,y), icon);
		return true;
	}

	if ( op == GOAL_ACTION )
	{
		action = *pTable++;
		direct = *pTable++;
		BlupiInitAction(rank, action, direct);
		return true;
	}

	if ( op == GOAL_ELECTRO )
	{
		x = *pTable++;
		y = *pTable++;
		GoalAdjustCel(rank, x,y);
		cel = GetCel((x/2)*2,(y/2)*2);
		icon = *pTable++;
		if ( MoveCreate(cel, rank, true,
						CHFLOOR,-1, -1,-1,
						100,1,100, false, true) )
		{
			MoveAddIcons(cel, icon);
		}
		BlupiKill(rank, cel, 1);
		return true;
	}

	if ( op == GOAL_MALADE )
	{
		m_blupi[rank].bMalade = *pTable++;
		return true;
	}

	if ( op == GOAL_WORK )
	{
		cel.x = m_blupi[rank].cel.x+(*pTable++);
		cel.y = m_blupi[rank].cel.y+(*pTable++);
		m_decor[cel.x/2][cel.y/2].workBlupi = rank;
		return true;
	}

	if ( op == GOAL_INTERRUPT )
	{
		m_blupi[rank].interrupt = *pTable++;  // change le niveau
		return true;
	}

	if ( op == GOAL_ENERGY )
	{
		if ( m_blupi[rank].energy <= *pTable++ )  goto error;
		return true;
	}

	if ( op == GOAL_ISNOMALADE )
	{
		if ( m_blupi[rank].bMalade )  goto error;
		return true;
	}

	if ( op == GOAL_TAKE )
	{
		cel.x = m_blupi[rank].cel.x+(*pTable++);
		cel.y = m_blupi[rank].cel.y+(*pTable++);
		MoveFinish(rank);
		m_blupi[rank].takeChannel = m_decor[cel.x/2][cel.y/2].objectChannel;
		m_blupi[rank].takeIcon    = m_decor[cel.x/2][cel.y/2].objectIcon;
		m_decor[cel.x/2][cel.y/2].objectChannel = -1;
		m_decor[cel.x/2][cel.y/2].objectIcon    = -1;
		return true;
	}

	if ( op == GOAL_TAKEOBJECT )
	{
		cel.x = m_blupi[rank].cel.x+(*pTable++);
		cel.y = m_blupi[rank].cel.y+(*pTable++);
		channel = *pTable++;
		icon    = *pTable++;
		m_blupi[rank].takeChannel = channel;
		m_blupi[rank].takeIcon    = icon;
		return true;
	}

	if ( op == GOAL_LABO )
	{
		m_blupi[rank].takeChannel = CHOBJECT;
		if ( m_blupi[rank].takeIcon == 82 )  // fleurs normales ?
		{
			m_blupi[rank].takeIcon = 80;  // bouteille
		}
		if ( m_blupi[rank].takeIcon == 84 )  // fleurs fonc�es ?
		{
			m_blupi[rank].takeIcon = 85;  // dynamite
		}
		if ( m_blupi[rank].takeIcon == 95 )  // fleurs vertes ?
		{
			m_blupi[rank].takeIcon = 93;  // pi�ge
		}
		if ( m_blupi[rank].takeIcon == 60 )  // tomates ?
		{
			m_blupi[rank].takeIcon = 92;  // poison
		}
		return true;
	}

	if ( op == GOAL_CACHE )
	{
		m_blupi[rank].bCache = *pTable++;
		if ( *pTable++ &&  // bDynamite ?
			 m_blupi[rank].perso == 0 &&
			 m_blupi[rank].vehicule == 3 &&  // armure ?
			 !m_bInvincible )
		{
			m_blupi[rank].bCache = false;
		}
		return true;
	}

	if ( op == GOAL_DELETE )
	{
		if ( m_blupi[rank].perso    == 0 &&  // blupi ?
			 m_blupi[rank].vehicule == 3 &&  // armure ?
			 !m_bInvincible )
		{
			return true;
		}
		BlupiDelete(rank);  // snif ...
		return true;
	}

	if ( op == GOAL_DEPOSE )
	{
		m_blupi[rank].takeChannel = -1;
		return true;
	}

	if ( op == GOAL_NEWBLUPI )
	{
		cel.x = m_blupi[rank].cel.x+(*pTable++);
		cel.y = m_blupi[rank].cel.y+(*pTable++);
		PutObject(cel, -1,-1);  // enl�ve les oeufs

		rank = BlupiCreate(cel, ACTION_STOP, DIRECT_E, 0, MAXENERGY/4);
		if ( rank >= 0 )
		{
			m_blupi[rank].energy = MAXENERGY/4;
			BlupiInitAction(rank, ACTION_NAISSANCE);
		}

		cel.x ++;
		rank = BlupiCreate(cel, ACTION_STOP, DIRECT_E, 0, MAXENERGY/4);
		if ( rank >= 0 )
		{
			m_blupi[rank].energy = MAXENERGY/4;
			BlupiInitAction(rank, ACTION_NAISSANCE);
		}

		cel.x --;
		cel.y ++;
		rank = BlupiCreate(cel, ACTION_STOP, DIRECT_E, 0, MAXENERGY/4);
		if ( rank >= 0 )
		{
			m_blupi[rank].energy = MAXENERGY/4;
			BlupiInitAction(rank, ACTION_NAISSANCE);
		}

		cel.x ++;
		rank = BlupiCreate(cel, ACTION_STOP, DIRECT_E, 0, MAXENERGY/4);
		if ( rank >= 0 )
		{
			m_blupi[rank].energy = MAXENERGY/4;
			BlupiInitAction(rank, ACTION_NAISSANCE);
		}
		return true;
	}

	if ( op == GOAL_NEWPERSO )
	{
		cel.x = m_blupi[rank].cel.x+(*pTable++);
		cel.y = m_blupi[rank].cel.y+(*pTable++);
		icon = *pTable++;

		destCel = cel;
		destCel.x ++;
		if ( IsBlupiHereEx(destCel, rank, false) )  // destination occup�e ?
		{
			m_blupi[rank].goalPhase --;  // on attend ...
			return true;
		}
		destCel.x ++;
		if ( IsBlupiHereEx(destCel, rank, false) )  // destination occup�e ?
		{
			destCel.y --;
			if ( icon == 5 ||  // bombe ?
				 IsBlupiHereEx(destCel, rank, false) )  // destination occup�e ?
			{
				m_blupi[rank].goalPhase --;  // on attend ...
				return true;
			}
		}

		rank = BlupiCreate(cel, ACTION_STOP, DIRECT_E, icon, MAXENERGY);
		if ( rank >= 0 )
		{
			m_blupi[rank].goalCel = destCel;
		}
		return true;
	}

	if ( op == GOAL_USINEBUILD )
	{
		cel.x = m_blupi[rank].cel.x+(*pTable++);
		cel.y = m_blupi[rank].cel.y+(*pTable++);
		if ( !IsUsineBuild(rank, cel) )  goto error;
		return true;
	}

	if ( op == GOAL_USINEFREE )
	{
		cel.x = m_blupi[rank].cel.x+(*pTable++);
		cel.y = m_blupi[rank].cel.y+(*pTable++);
		if ( !IsUsineFree(rank, cel) )
		{
			GoalStop(rank, true);
			m_blupi[rank].goalCel = GetCel(cel,1,-1);  // � c�t� de la porte
//?			m_blupi[rank].goalAction = 0;  // stoppe sit�t apr�s
//?			m_blupi[rank].interrupt = 1;
//?			GoalUnwork(rank);
//?			FlushUsed(rank);
		}
		return true;
	}

	if ( op == GOAL_AMORCE )
	{
		cel.x = m_blupi[rank].cel.x+(*pTable++);
		cel.y = m_blupi[rank].cel.y+(*pTable++);
		if ( IsBlupiHereEx(cel, rank, false) )  goto error;
		// Cr�e un d�tonnateur de mine (blupi invisible).
		rank = BlupiCreate(cel, ACTION_STOP, DIRECT_E, 6, MAXENERGY);
		if ( rank >= 0 )
		{
			m_blupi[rank].bCache = true;  // invisible
			m_blupi[rank].goalAction = WM_ACTION_MINE2;
			m_blupi[rank].goalPhase  = 0;
			m_blupi[rank].goalCel.x  = -1;
			m_blupi[rank].interrupt  = 1;
		}
		return true;
	}

	if ( op == GOAL_VEHICULE )
	{
		m_blupi[rank].vehicule = *pTable++;
		if ( m_blupi[rank].vehicule != 0         &&  // pas � pied ?
			 m_blupi[rank].takeChannel != -1     &&  // porte qq chose ?
			 m_blupi[rank].energy <= MAXENERGY/4 )   // faible ?
		{
			m_blupi[rank].energy = MAXENERGY/4+1;
		}
		return true;
	}

	if ( op == GOAL_ACTUALISE )
	{
		BlupiActualise(rank);
		return true;
	}

	if ( op == GOAL_SOUND )
	{
		icon = *pTable++;
		BlupiSound(rank, icon, pos);
		return true;
	}

	if ( op == GOAL_REPEAT )
	{
		icon = *pTable++;
		m_blupi[rank].bRepeat = icon;
		return true;
	}

	if ( op == GOAL_OTHER )
	{
		if ( !m_blupi[rank].bRepeat )  goto term;

		// Bouton stop press� ?
		if ( m_blupi[rank].stop == 1 )  goto term;

		channel = *pTable++;
		first   = *pTable++;
		last    = *pTable++;
		first2  = *pTable++;
		last2   = *pTable++;
		action  = *pTable++;
		if ( !SearchOtherObject(rank, m_blupi[rank].cel, action, 100,
								channel, first, last, first2, last2,
								m_blupi[rank].goalHili, icon) )  goto term;
		if ( action == WM_ACTION_ABAT1 ||
			 action == WM_ACTION_ROC1  )
		{
			action += icon-first;  // WM_ACTION_ABAT1..6
		}
		m_blupi[rank].goalAction = action;
		m_blupi[rank].goalPhase  = 0;
		m_blupi[rank].goalCel.x  = -1;
		m_blupi[rank].interrupt  = 1;
		GoalInitJauge(rank);
		GoalUnwork(rank);
		FlushUsed(rank);
		return true;
	}

	if ( op == GOAL_OTHERFIX )
	{
		if ( !m_blupi[rank].bRepeat )  goto term;

		// Bouton stop press� ?
		if ( m_blupi[rank].stop == 1 )  goto term;

		channel = *pTable++;
		first   = *pTable++;
		last    = *pTable++;
		first2  = *pTable++;
		last2   = *pTable++;
		action  = *pTable++;
		if ( !SearchOtherObject(rank, m_blupi[rank].fix, action, 100,
								channel, first, last, first2, last2,
								m_blupi[rank].goalHili, icon) )  goto term;
		if ( action == WM_ACTION_ABAT1 ||
			 action == WM_ACTION_ROC1  )
		{
			action += icon-first;  // WM_ACTION_ABAT1..6
		}
		m_blupi[rank].goalAction = action;
		m_blupi[rank].goalPhase  = 0;
		m_blupi[rank].goalCel.x  = -1;
		m_blupi[rank].interrupt  = 1;
		GoalInitJauge(rank);
		GoalUnwork(rank);
		FlushUsed(rank);
		return true;
	}

	if ( op == GOAL_OTHERLOOP )
	{
		action = *pTable++;
		if ( m_blupi[rank].cLoop < m_blupi[rank].nLoop )
		{
			m_blupi[rank].goalAction = action;
			m_blupi[rank].goalPhase  = 0;
			m_blupi[rank].goalCel.x  = -1;
			m_blupi[rank].interrupt  = 1;
			GoalInitJauge(rank);
			GoalUnwork(rank);
			FlushUsed(rank);
		}
		return true;
	}

	if ( op == GOAL_NEXTLOOP )
	{
		m_blupi[rank].cLoop ++;
		return true;
	}

	if ( op == GOAL_FIX )
	{
		m_blupi[rank].fix.x = m_blupi[rank].cel.x+(*pTable++);
		m_blupi[rank].fix.y = m_blupi[rank].cel.y+(*pTable++);
		return true;
	}

	if ( op == GOAL_FLOORJUMP )
	{
		channel = *pTable++;
		icon    = *pTable++;
		action  = *pTable++;
		GetFloor(m_blupi[rank].cel, mchannel, micon);
		if ( channel == mchannel && icon == micon )
		{
			m_blupi[rank].goalAction = action;
			m_blupi[rank].goalPhase  = 0;
			m_blupi[rank].goalCel.x  = -1;
			m_blupi[rank].interrupt  = 1;
			GoalInitJauge(rank);
			GoalUnwork(rank);
			FlushUsed(rank);
		}
		return true;
	}

	if ( op == GOAL_ADDDRAPEAU )
	{
		cel.x = m_blupi[rank].cel.x+(*pTable++);
		cel.y = m_blupi[rank].cel.y+(*pTable++);
		AddDrapeau(cel);  // cellule sond�e
		return true;
	}

	if ( op == GOAL_TELEPORTE )
	{
		pos.x = *pTable++;
		pos.y = *pTable++;

		cMem = m_blupi[rank].cel;
		GetFloor(cMem, channel, icon);
		PutFloor(cMem, -1, -1);
		bOK = SearchOtherObject(rank, m_blupi[rank].cel, WM_ACTION_GO,
								1000, CHFLOOR,80,80,-1,-1, cel, i);
		PutFloor(cMem, channel, icon);
		if ( !bOK )  goto error;

		cel.x += pos.x;
		cel.y += pos.y;
		if ( IsBlupiHereEx(cel, rank, false) ||
			 !IsFreeCel(cel, rank) )  goto error;
		m_blupi[rank].cel = cel;
		BlupiPushFog(rank);
		if ( m_blupi[rank].bHili )
		{
			SetCoin(cel, true);
		}
		return true;
	}

	if ( op == GOAL_IFTERM )
	{
		cel.x = m_blupi[rank].cel.x+(*pTable++);
		cel.y = m_blupi[rank].cel.y+(*pTable++);
		if ( !IsFreeCel(cel, rank) ||
			 IsBlupiHereEx(cel, rank, false) )  goto term;
		return true;
	}

	if ( op == GOAL_IFDEBARQUE )
	{
		cel.x = m_blupi[rank].cel.x+(*pTable++);
		cel.y = m_blupi[rank].cel.y+(*pTable++);
		m_blupi[rank].vehicule = 0;  // � pied
		bOK = IsFreeCel(cel, rank) &&
			  !IsBlupiHereEx(cel, rank, false);
		m_blupi[rank].vehicule = 1;  // en bateau
		if ( !bOK )  goto term;
		return true;
	}

	if ( op == GOAL_SKIPSKILL )
	{
		i     = (*pTable++);
		total = (*pTable++);
		if (  m_skill == i )
		{
			m_blupi[rank].goalPhase += total;  // saute qq instructions
		}
		return true;
	}

	if ( op == GOAL_TERM )
	{
		term:
		bError = false;
	}

	if ( op == GOAL_WAITFREE )
	{
		cel.x = m_blupi[rank].cel.x+(*pTable++);
		cel.y = m_blupi[rank].cel.y+(*pTable++);
		if ( IsBlupiHereEx(cel, rank, false) )  // destination occup�e ?
		{
			m_blupi[rank].goalPhase --;  // on attend ...

			rank = m_blupiHere;
			if ( m_blupi[rank].goalAction != WM_ACTION_GO      &&
				 m_blupi[rank].goalAction != WM_ACTION_ELECTRO &&
				 m_blupi[rank].goalAction != WM_ACTION_ELECTROm )
			{
				destCel.x = cel.x;
				destCel.y = cel.y-1;
				if ( !IsBlupiHereEx(destCel, rank, false) )
				{
					GoalStart(rank, WM_ACTION_GO, destCel);
					return true;
				}

				destCel.x = cel.x+1;
				destCel.y = cel.y;
				if ( !IsBlupiHereEx(destCel, rank, false) )
				{
					GoalStart(rank, WM_ACTION_GO, destCel);
					return true;
				}

				destCel.x = cel.x;
				destCel.y = cel.y+1;
				if ( !IsBlupiHereEx(destCel, rank, false) )
				{
					GoalStart(rank, WM_ACTION_GO, destCel);
					return true;
				}

				destCel.x = cel.x+1;
				destCel.y = cel.y-1;
				if ( !IsBlupiHereEx(destCel, rank, false) )
				{
					GoalStart(rank, WM_ACTION_GO, destCel);
					return true;
				}

				destCel.x = cel.x+1;
				destCel.y = cel.y+1;
				if ( !IsBlupiHereEx(destCel, rank, false) )
				{
					GoalStart(rank, WM_ACTION_GO, destCel);
					return true;
				}

				if ( m_blupi[rank].perso == 0 )
				{
					if ( m_blupi[rank].bMalade )
					{
						action = WM_ACTION_ELECTROm;
					}
					else
					{
						action = WM_ACTION_ELECTRO;
					}
					GoalStart(rank, action, m_blupi[rank].cel);
					return true;
				}
			}
		}
		return true;
	}

	error:
	i = m_blupi[rank].repeatLevel;
	GoalStop(rank, bError, i==-1);
	if ( i != -1 )  // r�p�tition en cours ?
	{
		button = m_blupi[rank].listButton[i];
		cMem   = m_blupi[rank].listCel[i];
		param  = m_blupi[rank].listParam[i];
		cel = cMem;
		if ( RepeatAdjust(rank, button, cel, cMem, param, i) )
		{
			if ( IsBlupiHereEx(cel, rank, false) )  // destination occup�e ?
			{
				m_blupi[rank].repeatLevel = i;  // on continue ...
				GoalStart(rank, WM_ACTION_GO, m_blupi[rank].cel);  // on attend ...
				return true;
			}
			if ( BlupiGoal(rank, button, cel, cMem) )
			{
				m_blupi[rank].repeatLevel = i;  // on continue ...
				return true;
			}
		}
	}
	return false;
}

// Supprime le blocage de la cellule dans laquelle
// blupi travaille.

void CDecor::GoalUnwork(int rank)
{
	int			x, y;

	for ( x=0 ; x<MAXCELX/2 ; x++ )
	{
		for ( y=0 ; y<MAXCELY/2 ; y++ )
		{
			if ( m_decor[x][y].workBlupi == rank )
			{
				m_decor[x][y].workBlupi = -1;  // d�bloque
			}
		}
	}
}

// Stoppe compl�tement une action.

void CDecor::GoalStop(int rank, bool bError, bool bSound)
{
	POINT		pos;

	static int table_sound_term[6] =
	{
		SOUND_TERM1,
		SOUND_TERM2,
		SOUND_TERM3,
		SOUND_TERM4,
		SOUND_TERM5,
		SOUND_TERM6,
	};

	static int table_sound_boing[3] =
	{
		SOUND_BOING1,
		SOUND_BOING2,
		SOUND_BOING3,
	};

	if ( bError && bSound )
	{
		ListRemove(rank);  // supprime la derni�re action m�moris�e
	}

	m_blupi[rank].goalAction  = 0;
	m_blupi[rank].goalPhase   = 0;
	m_blupi[rank].goalCel.x   = -1;
	m_blupi[rank].jaugePhase  = -1;
	m_blupi[rank].jaugeMax    = -1;
	m_blupi[rank].interrupt   = 1;  // remet le niveau normal
	m_blupi[rank].busyCount   = 0;
	m_blupi[rank].busyDelay   = 0;
	m_blupi[rank].repeatLevel = -1;  // stoppe la r�p�tition
	FlushUsed(rank);
	MoveFinish(rank);
	GoalUnwork(rank);

	// En cas d'erreur, il faut accepter de traverser la
	// construction avort�e (par exemple, le massif d'arbres
	// dans lequel blupi se trouve, mais qui n'a pas pu
	// �tre abattu).
	if ( !bError )
	{
		m_blupi[rank].passCel.x = -1;
	}

	m_blupi[rank].stop = 0;  // rel�che bouton stop

	if ( bSound &&
		 (m_blupi[rank].perso == 0 ||   // blupi ?
		  m_blupi[rank].perso == 8 ) )  // assistant ?
	{
		if ( bError )
		{
			pos.x = LXIMAGE/2;
			pos.y = LYIMAGE/2;
			BlupiSound(rank, table_sound_boing[Random(0,2)], pos, true);
		}
		else
		{
			pos = ConvCelToPos(m_blupi[rank].cel);
			BlupiSound(rank, table_sound_term[Random(0,5)], pos, true);
		}
	}
}


// Teste si une cellule est d�j� utilis�e comme but pour
// n'importe quel blupi.

bool CDecor::BlupiIsGoalUsed(POINT cel)
{
	int		rank;

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist &&
			 m_blupi[rank].goalCel.x/2 == cel.x/2 &&
			 m_blupi[rank].goalCel.y/2 == cel.y/2 )  return true;
	}

	return false;
}


// D�marre ou stoppe un rayon entre deux tours.

void CDecor::BlupiStartStopRayon(int rank, POINT startCel, POINT endCel)
{
	int		i, icon, icon2;
	POINT	cel, cel2, vector, pos;

	if ( m_blupi[rank].perso == 1 ||  // araign�e ?
		 m_blupi[rank].perso == 2 )   // virus ?
	{
		return;
	}

	// Stoppe un rayon.
	cel.x = (endCel.x/2)*2;
	cel.y = (endCel.y/2)*2;
	icon = m_decor[cel.x/2][cel.y/2].objectIcon;
	if ( (icon == 10000 || icon == 10001) &&
		 MoveIsUsed(cel) )
	{
		if ( MoveIsUsed(cel) )
		{
			MoveFinish(cel);

			pos = ConvCelToPos(cel);
			BlupiSound(rank, SOUND_RAYON2, pos);
		}

		for ( i=0 ; i<4 ; i++ )
		{
			vector = GetVector(i*2*16);
			cel.x = (endCel.x/2)*2 + vector.x*2;
			cel.y = (endCel.y/2)*2 + vector.y*2;
			icon = m_decor[cel.x/2][cel.y/2].objectIcon;
			if ( icon == 10000 || icon == 10001 )
			{
				MoveFinish(cel);
			}
		}
	}

	// D�marre un rayon.
	cel.x  = (startCel.x/2)*2;
	cel.y  = (startCel.y/2)*2;
	cel2.x = (endCel.x/2)*2;
	cel2.y = (endCel.y/2)*2;
	icon  = m_decor[cel.x/2][cel.y/2].objectIcon;
	icon2 = m_decor[cel2.x/2][cel2.y/2].objectIcon;
	if ( (icon == 10000 || icon == 10001) &&
		 icon2 != 10000 && icon2 != 10001 )
	{
		if ( MoveCreate(cel, -1, false, CHOBJECT,-1,
						-1,-1, 9999,1,0, true) )
		{
			MoveAddIcons(cel, icon==10000?4:5, true);  // �clairs
		}

		pos = ConvCelToPos(cel);
		BlupiSound(rank, SOUND_RAYON1, pos);

		for ( i=0 ; i<4 ; i++ )
		{
			vector = GetVector(i*2*16);
			cel.x = (startCel.x/2)*2 + vector.x*2;
			cel.y = (startCel.y/2)*2 + vector.y*2;
			icon = m_decor[cel.x/2][cel.y/2].objectIcon;
			if ( icon == 10000 || icon == 10001 )
			{
				if ( MoveCreate(cel, -1, false, CHOBJECT,-1,
								-1,-1, 9999,1,0, true) )
				{
					MoveAddIcons(cel, icon==10000?4:5, true);  // �clairs
				}
			}
		}
	}
}


// Tourne un blupi, si n�cessaire.
// Retourne false si ce n'est pas n�cessaire.

bool CDecor::BlupiRotate(int rank)
{
	int		aDirect, sDirect, ip, in, sens;
	bool	bOK;
	POINT	pos;

	aDirect = m_blupi[rank].aDirect;
	sDirect = m_blupi[rank].sDirect;

	if ( aDirect == sDirect )  return false;

	if ( sDirect > aDirect )  ip = sDirect+0*16-aDirect;
	else                      ip = sDirect+8*16-aDirect;

	if ( aDirect > sDirect )  in = aDirect+0*16-sDirect;
	else                      in = aDirect+8*16-sDirect;

	if ( ip == 0 || in == 0 )
	{
		m_blupi[rank].aDirect = m_blupi[rank].sDirect;
		return false;
	}

	if ( m_blupi[rank].perso == 0 &&  // blupi ?
		 m_blupi[rank].vehicule == 1 )  // en bateau ?
	{
		if ( ip == in )  sens = Random(0,1)?8:8*16-8;
		if ( ip <  in )  sens = 8;
		if ( ip >  in )  sens = 8*16-8;
		aDirect = (aDirect+sens)%(8*16);
	}
	else if ( m_blupi[rank].perso == 0 &&  // blupi ?
		 m_blupi[rank].vehicule == 2 )  // en jeep ?
	{
		if ( ip == in )  sens = Random(0,1)?8:8*16-8;
		if ( ip <  in )  sens = 8;
		if ( ip >  in )  sens = 8*16-8;
		aDirect = (aDirect+sens)%(8*16);
	}
	else if ( m_blupi[rank].perso == 0 &&  // blupi ?
		 m_blupi[rank].vehicule == 3 )  // armure ?
	{
		if ( ip == in )  sens = Random(0,1)?4:8*16-4;
		if ( ip <  in )  sens = 4;
		if ( ip >  in )  sens = 8*16-4;
		aDirect = (aDirect+sens)%(8*16);
	}
	else if ( m_blupi[rank].perso == 3 )  // tracks ?
	{
		if ( ip == in )  sens = Random(0,1)?4:8*16-4;
		if ( ip <  in )  sens = 4;
		if ( ip >  in )  sens = 8*16-4;
		aDirect = (aDirect+sens)%(8*16);
	}
	else if ( m_blupi[rank].perso == 4 )  // robot ?
	{
		pos = ConvCelToPos(m_blupi[rank].cel);
		BlupiSound(rank, SOUND_R_ROTATE, pos);

		if ( ip == in )  sens = Random(0,1)?2:8*16-2;
		if ( ip <  in )  sens = 2;
		if ( ip >  in )  sens = 8*16-2;
		aDirect = (aDirect+sens)%(8*16);
	}
	else if ( m_blupi[rank].perso == 8 )  // disciple ?
	{
		if ( ip == in )  sens = Random(0,1)?8:8*16-8;
		if ( ip <  in )  sens = 8;
		if ( ip >  in )  sens = 8*16-8;
		aDirect = (aDirect+sens)%(8*16);
	}
	else
	{
		if ( ip == in )  sens = Random(0,1)?1:7;
		if ( ip <  in )  sens = 1;
		if ( ip >  in )  sens = 7;
		aDirect = ((aDirect/16+sens)%8)*16;
	}

	m_blupi[rank].lastIcon = m_blupi[rank].icon;
	bOK = Rotate(m_blupi[rank].icon, aDirect);
	if ( bOK )
	{
		m_blupi[rank].aDirect = aDirect;
		return true;
	}
	else
	{
		m_blupi[rank].aDirect = m_blupi[rank].sDirect;
		return false;
	}
}

// Avance un blupi existant.

bool CDecor::BlupiNextAction(int rank)
{
	bool		bOK;
	POINT		pos, iCel;
	int			a, min;
	short		sound;

	if ( !m_blupi[rank].bExist )  return false;

	if ( m_blupi[rank].clicDelay > 0 )
	{
		m_blupi[rank].clicDelay --;
	}
	if ( m_blupi[rank].clicDelay == 0 )
	{
		m_blupi[rank].clicCount = 0;
	}

	bOK = true;
	if ( !BlupiRotate(rank) )  // si rotation pas n�cessaire
	{
		m_blupi[rank].lastIcon = m_blupi[rank].icon;

		bOK = Action(m_blupi[rank].action,
					 m_blupi[rank].aDirect,
					 m_blupi[rank].phase,
					 m_blupi[rank].step,
					 m_blupi[rank].channel,
					 m_blupi[rank].icon,
					 m_blupi[rank].pos,
					 m_blupi[rank].posZ,
					 sound);
		BlupiAdaptIcon(rank);

		if ( sound != -1 )
		{
			pos = ConvCelToPos(m_blupi[rank].cel);
			BlupiSound(rank, sound, pos);
		}
	}

	a = GetAmplitude(m_blupi[rank].action);

	iCel = m_blupi[rank].cel;

	if ( m_blupi[rank].pos.x ==  (DIMCELX/2)*a &&
		 m_blupi[rank].pos.y ==  (DIMCELY/2)*a )
	{
		m_blupi[rank].cel.x += a;
		BlupiPushFog(rank);
		BlupiStartStopRayon(rank, iCel, m_blupi[rank].cel);
	}

	if ( m_blupi[rank].pos.x == -(DIMCELX/2)*a &&
		 m_blupi[rank].pos.y == -(DIMCELY/2)*a )
	{
		m_blupi[rank].cel.x -= a;
		BlupiPushFog(rank);
		BlupiStartStopRayon(rank, iCel, m_blupi[rank].cel);
	}

	if ( m_blupi[rank].pos.x == -(DIMCELX/2)*a &&
		 m_blupi[rank].pos.y ==  (DIMCELY/2)*a )
	{
		m_blupi[rank].cel.y += a;
		BlupiPushFog(rank);
		BlupiStartStopRayon(rank, iCel, m_blupi[rank].cel);
	}

	if ( m_blupi[rank].pos.x ==  (DIMCELX/2)*a &&
		 m_blupi[rank].pos.y == -(DIMCELY/2)*a )
	{
		m_blupi[rank].cel.y -= a;
		BlupiPushFog(rank);
		BlupiStartStopRayon(rank, iCel, m_blupi[rank].cel);
	}

	if ( m_blupi[rank].pos.x ==  0          &&
		 m_blupi[rank].pos.y ==  DIMCELY*a )
	{
		m_blupi[rank].cel.x += a;
		m_blupi[rank].cel.y += a;
		BlupiPushFog(rank);
		BlupiStartStopRayon(rank, iCel, m_blupi[rank].cel);
	}

	if ( m_blupi[rank].pos.x ==  0          &&
		 m_blupi[rank].pos.y == -DIMCELY*a )
	{
		m_blupi[rank].cel.x -= a;
		m_blupi[rank].cel.y -= a;
		BlupiPushFog(rank);
		BlupiStartStopRayon(rank, iCel, m_blupi[rank].cel);
	}

	if ( m_blupi[rank].pos.x ==  DIMCELX*a &&
		 m_blupi[rank].pos.y ==  0          )
	{
		m_blupi[rank].cel.x += a;
		m_blupi[rank].cel.y -= a;
		BlupiPushFog(rank);
		BlupiStartStopRayon(rank, iCel, m_blupi[rank].cel);
	}

	if ( m_blupi[rank].pos.x == -DIMCELX*a &&
		 m_blupi[rank].pos.y ==  0          )
	{
		m_blupi[rank].cel.x -= a;
		m_blupi[rank].cel.y += a;
		BlupiPushFog(rank);
		BlupiStartStopRayon(rank, iCel, m_blupi[rank].cel);
	}

	// Blupi perd de l'�nergie s'il fait qq chose.
	if ( m_blupi[rank].action != ACTION_STOP    &&
		 m_blupi[rank].action != ACTION_STOPf   &&
		 m_blupi[rank].action != ACTION_STOPb   &&
		 m_blupi[rank].action != ACTION_STOPj   &&
		 m_blupi[rank].action != ACTION_MISC1   &&
		 m_blupi[rank].action != ACTION_MISC2   &&
		 m_blupi[rank].action != ACTION_MISC3   &&
		 m_blupi[rank].action != ACTION_MISC4   &&
		 m_blupi[rank].action != ACTION_MISC5   &&
		 m_blupi[rank].action != ACTION_MISC6   &&
		 m_blupi[rank].action != ACTION_MISC1f  )
	{
		if ( m_blupi[rank].energy > 0 &&
			 m_blupi[rank].perso == 0 &&    // blupi ?
			 m_blupi[rank].vehicule == 0 )  // � pied ?
		{
			if ( m_bSuper )  a = 0;
			else             a = 1;
			min = 0;
			if ( m_blupi[rank].goalAction == WM_ACTION_MUR ||
				 m_blupi[rank].goalAction == WM_ACTION_TOUR )
			{
				a = 5;
				min = 1;
			}
			if ( m_blupi[rank].action == ACTION_GLISSE )
			{
				if ( m_bSuper )  a = 0;
				else             a = 40;
			}
			m_blupi[rank].energy -= a;  // blupi se fatigue +/-
			if ( m_blupi[rank].energy < min )
			{
				m_blupi[rank].energy = min;
			}
		}
	}

	// Blupi prend de l'�nergie s'il mange.
	if ( m_blupi[rank].action == ACTION_MANGE )
	{
		if ( m_blupi[rank].energy < MAXENERGY )
		{
			m_blupi[rank].energy += MAXENERGY/(40*3);
		}
	}

	// Le robot perd de l'�nergie s'il fait qq chose.
	if ( m_blupi[rank].action != ACTION_R_STOP )
	{
		if ( m_blupi[rank].energy > 0 &&
			 m_blupi[rank].perso == 4 )
		{
			m_blupi[rank].energy -= 3;  // le robot se fatigue
			if ( m_blupi[rank].energy < 1 )
			{
				m_blupi[rank].energy = 1;
			}
		}
	}

	// Le robot prend de l'�nergie s'il se recharge.
	if ( m_blupi[rank].action == ACTION_R_CHARGE )
	{
		if ( m_blupi[rank].energy < MAXENERGY )
		{
			m_blupi[rank].energy = MAXENERGY;
		}
	}

	// Blupi gu�rrit s'il boit.
	if ( m_blupi[rank].action == ACTION_BOIT )
	{
		m_blupi[rank].bMalade = false;
		if ( m_blupi[rank].energy < MAXENERGY )
		{
			m_blupi[rank].energy += MAXENERGY/(40*3);
		}
	}

	// Si blupi est presque compl�tement �puis�, il stoppe.
	if ( !bOK &&
		 m_blupi[rank].perso == 0 &&
		 m_blupi[rank].energy < 50 &&
		 m_blupi[rank].energy != 0 &&
		 m_blupi[rank].action == ACTION_MARCHEf )
	{
		BlupiInitAction(rank, ACTION_STOP);
		GoalStop(rank, true);
	}

	return bOK;
}

// Action suivante pour un blupi existant.

void CDecor::BlupiNextGoal(int rank)
{
	int			direct, action, channel, icon, min, lg, fRank, i;
	POINT		pos, cel, vector;

	if ( !m_blupi[rank].bExist )  return;

	pos = ConvCelToPos(m_blupi[rank].cel);

	// Si blupi termine une action "mort", il doit dispara�tre.
	if ( m_blupi[rank].action == ACTION_BRULE    ||
		 m_blupi[rank].action == ACTION_TCHAO    ||
		 m_blupi[rank].action == ACTION_A_GRILLE ||
		 m_blupi[rank].action == ACTION_V_GRILLE )
	{
		BlupiDelete(rank);  // snif ...
		return;
	}

	// Si blupi passe trop pr�t du feu, aie aie aie ...
	if ( IsFireCel(m_blupi[rank].cel) )  // blupi se brule les ailes ?
	{
		if ( m_blupi[rank].perso == 0 &&
			 m_blupi[rank].vehicule != 3 &&  // pas armure ?
			 !m_bInvincible &&
			 m_blupi[rank].goalAction != WM_ACTION_GRILLE )
		{
			BlupiDeselect(rank);
			GoalStart(rank, WM_ACTION_GRILLE, m_blupi[rank].cel);
			goto goal;
//?			BlupiInitAction(rank, ACTION_BRULE);
//?			goto init;
		}
		if ( m_blupi[rank].perso == 1 ||  // araign�e ?
			 m_blupi[rank].perso == 2 )   // virus ?
		{
			BlupiDelete(rank);  // la bestiole meurt
			return;
		}
	}

	// Si blupi passe trop pr�t d'un virus ...
	if ( m_blupi[rank].perso == 0 &&
		 m_blupi[rank].vehicule != 1 &&  // pas en bateau ?
		 m_blupi[rank].vehicule != 3 &&  // pas armure ?
		 !m_blupi[rank].bMalade &&  // en bonne sant� ?
		 m_blupi[rank].goalAction != WM_ACTION_GRILLE &&
		 m_blupi[rank].goalAction != WM_ACTION_ELECTRO &&
		 m_blupi[rank].goalAction != WM_ACTION_ELECTROm &&
		 m_blupi[rank].goalAction != WM_ACTION_BATEAUDE &&
		 m_blupi[rank].goalAction != WM_ACTION_BATEAUDS &&
		 m_blupi[rank].goalAction != WM_ACTION_BATEAUDO &&
		 m_blupi[rank].goalAction != WM_ACTION_BATEAUDN &&
		 m_blupi[rank].goalAction != WM_ACTION_BATEAUAE &&
		 m_blupi[rank].goalAction != WM_ACTION_BATEAUAS &&
		 m_blupi[rank].goalAction != WM_ACTION_BATEAUAO &&
		 m_blupi[rank].goalAction != WM_ACTION_BATEAUAN &&
		 !m_bInvincible &&
		 IsVirusCel(m_blupi[rank].cel) )  // blupi chope un virus ?
	{
		m_blupi[rank].bMalade = true;

		if ( m_blupi[rank].energy > MAXENERGY/4 )
		{
			m_blupi[rank].energy = MAXENERGY/4;
		}

		BlupiSound(rank, SOUND_VIRUS, pos);

		if ( m_blupi[rank].vehicule == 2 )  // en jeep ?
		{
			GoalStart(rank, WM_ACTION_DJEEP, m_blupi[rank].cel);
			goto goal;
		}
	}

	// Si blupi est compl�tement �puis�, il meurt.
	if ( m_blupi[rank].perso == 0 &&
		 m_blupi[rank].energy == 0 )
	{
		BlupiDeselect(rank);
		BlupiInitAction(rank, ACTION_TCHAO);
		goto init;
	}

	// Assigne un but s'il s'agit d'une araign�e.
	if ( m_blupi[rank].perso == 1 &&  // araign�e ?
		 m_blupi[rank].goalAction != WM_ACTION_A_MORT )
	{
		cel = m_blupi[rank].cel;
		cel.x = (cel.x/2)*2;
		cel.y = (cel.y/2)*2;
		GetObject(cel, channel, icon);
		if ( channel == CHOBJECT &&
			 (icon == 10000 || icon == 10001) &&  // rayon ?
			 MoveIsUsed(cel) )  // enclench� ?
		{
			BlupiInitAction(rank, ACTION_A_GRILLE);
			goto init;
		}

		cel = m_blupi[rank].cel;
		if ( cel.x%2 != 0 && cel.y%2 != 0 )
		{
			cel.x = (cel.x/2)*2;
			cel.y = (cel.y/2)*2;
			GetObject(cel, channel, icon);
			if ( channel == CHOBJECT &&
				 icon    == 60 )  // tomates ?
			{
				PutObject(cel, -1,-1);  // plus de tomates
				BlupiSound(rank, SOUND_A_HIHI, pos);
			}
			if ( channel == CHOBJECT &&
				 icon    == 92 )  // poison ?
			{
				PutObject(cel, -1,-1);  // plus de poison
				BlupiInitAction(rank, ACTION_STOP);
				GoalStart(rank, WM_ACTION_A_MORT, m_blupi[rank].cel);
				m_blupi[rank].goalCel = m_blupi[rank].cel;
				goto goal;
//?				BlupiSound(rank, SOUND_A_POISON, pos);
//?				BlupiInitAction(rank, ACTION_A_POISON);
//?				goto init;
			}
			if ( channel == CHOBJECT &&
				 icon    == 93 )  // pi�ge ?
			{
				BlupiSound(rank, SOUND_PIEGE, pos);
				PutObject(cel, CHOBJECT, 96);  // araign�e pi�g�e
				BlupiDelete(rank);  // supprime araign�e
				return;
			}
		}

		m_blupi[rank].bExist = false;
		if ( m_time%5 == rank%5 &&  // pas trop souvent !
			 SearchSpiderObject(rank, m_blupi[rank].cel, 100, cel, icon) )
		{
			m_blupi[rank].goalCel = cel;
			FlushUsed(rank);
//			direct = DirectSearch(m_blupi[rank].cel, cel);
//			if ( direct != -1 )
//			{
//				vector = GetVector(direct);
//				m_blupi[rank].goalCel.x = m_blupi[rank].cel.x + vector.x;
//				m_blupi[rank].goalCel.y = m_blupi[rank].cel.y + vector.y;
//				FlushUsed(rank);
//			}
		}
		m_blupi[rank].bExist = true;
	}

	// Assigne un but s'il s'agit d'un virus.
	if ( m_blupi[rank].perso == 2 )  // virus ?
	{
		cel = m_blupi[rank].cel;
		cel.x = (cel.x/2)*2;
		cel.y = (cel.y/2)*2;
		GetObject(cel, channel, icon);
		if ( channel == CHOBJECT &&
			 (icon == 10000 || icon == 10001) &&  // rayon ?
			 MoveIsUsed(cel) )  // enclench� ?
		{
			BlupiInitAction(rank, ACTION_V_GRILLE);
			goto init;
		}

		min = 50;  // ignore si trop loin !
		fRank = -1;
		for ( i=0 ; i<MAXBLUPI ; i++ )
		{
			if ( m_blupi[i].bExist &&
				 m_blupi[i].perso == 0 &&
				 !m_blupi[i].bMalade )
			{
				lg = abs(m_blupi[rank].cel.x-m_blupi[i].cel.x)+
					 abs(m_blupi[rank].cel.y-m_blupi[i].cel.y);
				if ( lg < min )
				{
					min = lg;
					fRank = i;
				}
			}
		}
		if ( fRank != -1 )
		{
			direct = DirectSearch(m_blupi[rank].cel, m_blupi[fRank].cel);
			if ( direct != -1 )
			{
				vector = GetVector(direct);
				m_blupi[rank].goalCel.x = m_blupi[rank].cel.x + vector.x;
				m_blupi[rank].goalCel.y = m_blupi[rank].cel.y + vector.y;
				FlushUsed(rank);
			}
		}
	}

	// Assigne un but s'il s'agit d'un tracks.
	if ( m_blupi[rank].perso == 3 &&  // tracks ?
		 m_blupi[rank].goalAction != WM_ACTION_T_DYNAMITE )
	{
		cel = m_blupi[rank].cel;
		if ( cel.x%2 != 0 && cel.y%2 != 0 )
		{
			cel.x = (cel.x/2)*2;
			cel.y = (cel.y/2)*2;
			GetObject(cel, channel, icon);
			if ( channel == CHOBJECT &&
				 IsTracksObject(icon) )
			{
				if ( icon == 85 )  // dynamite ?
				{
					BlupiInitAction(rank, ACTION_STOP);
					GoalStart(rank, WM_ACTION_T_DYNAMITE, cel);
					goto goal;
				}
				if ( icon == 125 ||  // mine ?
					 icon == 127 )
				{
					// Supprime le d�tonnateur.
					BlupiDelete(GetCel(cel.x+1,cel.y+1), 6);
				}
				if ( icon == 93 )  // pi�ge ?
				{
					BlupiSound(rank, SOUND_PIEGE, pos);
					PutObject(cel, CHOBJECT, 97);  // tracks pi�g�
					BlupiDelete(rank);  // supprime tracks
					return;
				}

				PutObject(cel, -1,-1);  // plus d'objet

				BlupiSound(rank, SOUND_T_ECRASE, pos);
				BlupiInitAction(rank, ACTION_T_ECRASE);
				goto init;
			}
		}
		cel = m_blupi[rank].cel;
		m_blupi[rank].bExist = false;
		if ( IsBlupiHere(cel, false) &&
			 m_blupi[m_blupiHere].perso == 0 &&
			 m_blupi[m_blupiHere].vehicule == 0 )  // � pied ?
		{
			m_blupi[rank].bExist = true;
			// Blupi �cras� au sol.
			if ( MoveCreate(cel, rank, true, CHFLOOR,-1, -1,-1,
							100,1,100, false, true) )
			{
				if ( m_blupi[m_blupiHere].bMalade )  MoveAddIcons(cel, 10);
				else                                 MoveAddIcons(cel, 9);
			}
			BlupiDelete(m_blupiHere);  // plus de blupi !
			BlupiSound(rank, SOUND_AIE, pos);
			BlupiInitAction(rank, ACTION_T_ECRASE);  // �crase blupi
			goto init;
		}
		m_blupi[rank].bExist = true;

//		if ( m_blupi[rank].goalCel.x != -1 )
//		{
//			GetObject(m_blupi[rank].goalCel, channel, icon);
//			if ( IsTracksObject(icon) )  goto action;
//		}
		m_blupi[rank].bExist = false;
		if ( m_time%5 == rank%5 &&  // pas trop souvent !
			 SearchTracksObject(rank, m_blupi[rank].cel, 25, cel, icon) )
		{
			m_blupi[rank].goalCel = cel;
			FlushUsed(rank);
		}
		m_blupi[rank].bExist = true;
	}

	// Assigne un but s'il s'agit d'un robot.
	if ( m_blupi[rank].perso == 4 &&  // robot ?
		 m_blupi[rank].goalAction != WM_ACTION_T_DYNAMITE )
	{
		cel = m_blupi[rank].cel;
		if ( cel.x%2 != 0 && cel.y%2 != 0 )
		{
			cel.x = (cel.x/2)*2;
			cel.y = (cel.y/2)*2;
			GetObject(cel, channel, icon);
			if ( channel == CHOBJECT &&
				 IsRobotObject(icon) )
			{
				if ( icon == 85 )  // dynamite ?
				{
					BlupiInitAction(rank, ACTION_STOP);
					GoalStart(rank, WM_ACTION_T_DYNAMITE, cel);
					goto goal;
				}
				if ( icon == 125 ||  // mine ?
					 icon == 127 )
				{
					// Supprime le d�tonnateur.
					BlupiDelete(GetCel(cel.x+1,cel.y+1), 6);
				}
				if ( icon == 93 )  // pi�ge ?
				{
					BlupiSound(rank, SOUND_PIEGE, pos);
					PutObject(cel, CHOBJECT, 98);  // robot pi�g�
					BlupiDelete(rank);  // supprime robot
					return;
				}

				PutObject(cel, -1,-1);  // plus d'objet

				BlupiSound(rank, SOUND_T_ECRASE, pos);
				BlupiInitAction(rank, ACTION_R_ECRASE);
				goto init;
			}
		}
		cel = m_blupi[rank].cel;
		if ( m_blupi[rank].goalAction == 0 &&
			 m_time%17 == rank%17 &&  // pas trop souvent !
			 SearchRobotObject(rank, m_blupi[rank].fix, 50, cel, icon, action) )
		{
			if ( action == -1 )
			{
				m_blupi[rank].goalCel = cel;
				FlushUsed(rank);
			}
			else
			{
				BlupiInitAction(rank, ACTION_STOP);
				GoalStart(rank, action, cel);
				goto goal;
			}
		}
	}

	// Assigne un but s'il s'agit d'une bombe.
	if ( m_blupi[rank].perso == 5 &&  // bombe ?
		 m_blupi[rank].goalAction != WM_ACTION_T_DYNAMITE )
	{
		cel = m_blupi[rank].cel;
		if ( cel.x%2 != 0 && cel.y%2 != 0 )
		{
			cel.x = (cel.x/2)*2;
			cel.y = (cel.y/2)*2;
			GetObject(cel, channel, icon);
			if ( channel == CHOBJECT &&
				 icon == 93 )  // pi�ge ?
			{
				BlupiSound(rank, SOUND_PIEGE, pos);
				PutObject(cel, CHOBJECT, 114);  // bombe pi�g�e
				BlupiDelete(rank);  // supprime bombe
				return;
			}
		}
		for ( i=0 ; i<4 ; i++ )
		{
			vector = GetVector(i*2*16);
			cel.x = ((m_blupi[rank].cel.x + vector.x*2)/2)*2;
			cel.y = ((m_blupi[rank].cel.y + vector.y*2)/2)*2;
			GetObject(cel, channel, icon);
			if ( channel == CHOBJECT &&
				 IsBombeObject(icon) &&  // cabane, palissade, etc. ?
				 icon != 93 )  // pas pi�ge ?
			{
				BlupiInitAction(rank, ACTION_STOP);
				GoalStart(rank, WM_ACTION_T_DYNAMITE, m_blupi[rank].cel);
				goto goal;
			}
		}
		cel = m_blupi[rank].cel;
		if ( m_blupi[rank].goalAction == 0 &&
			 m_time%17 == rank%17 &&  // pas trop souvent !
			 SearchBombeObject(rank, cel, 100, cel, icon) )
		{
			m_blupi[rank].goalCel = cel;
			FlushUsed(rank);
		}
	}

	// Assigne un but s'il s'agit d'un �lectro.
	if ( m_blupi[rank].perso == 7 )  // �lectro ?
	{
		cel = m_blupi[rank].cel;
		if ( cel.x%2 != 0 && cel.y%2 != 0 )
		{
			cel.x = (cel.x/2)*2;
			cel.y = (cel.y/2)*2;
			GetObject(cel, channel, icon);
			if ( channel == CHOBJECT &&
				 icon == 93 )  // pi�ge ?
			{
				BlupiSound(rank, SOUND_PIEGE, pos);
				PutObject(cel, CHOBJECT, 19);  // �lectro pi�g�e
				BlupiDelete(rank);  // supprime �lectro
				return;
			}
		}
		cel = m_blupi[rank].cel;
		if ( m_blupi[rank].goalAction == 0 &&
			 m_blupi[rank].goalCel.x == -1 &&
			 m_time%37 == rank%37 &&  // pas trop souvent !
			 SearchElectroObject(rank, cel, 100, cel, icon) )
		{
			if ( icon == -1 )  // sur un blupi ?
			{
				BlupiInitAction(rank, ACTION_STOP);
				GoalStart(rank, WM_ACTION_E_RAYON, cel);
				m_blupi[rank].fix = cel;
				goto goal;
			}
			m_blupi[rank].goalCel = cel;
			FlushUsed(rank);
		}
	}

	BlupiInitAction(rank, ACTION_STOP);

	goal:
	if ( m_blupi[rank].goalCel.x != -1 )  // y a-t-il un but ?
	{
		direct = DirectSearch(m_blupi[rank].cel,
							  m_blupi[rank].goalCel);

		if ( direct == -1 )  // but atteint ?
		{
			m_blupi[rank].goalCel.x = -1;
			FlushUsed(rank);
		}
		else
		{
			// Si blupi a peu d'�nergie et qu'il transporte
			// qq chose, il doit stopper !
			if ( m_blupi[rank].energy <= MAXENERGY/4 &&
				 m_blupi[rank].takeChannel != -1 &&
				 (m_blupi[rank].vehicule == 0 ||  // � pied ?
				  m_blupi[rank].vehicule == 3) )  // armure ?
			{
				// Si blupi est en train de descendre de la jeep
				// et qu'il est malade tout en transportant qq
				// chose, il ne faut pas stopper !!!
				cel = m_blupi[rank].cel; 
				if ( cel.x%2 != 0 && cel.y%2 != 0 &&
					 m_decor[cel.x/2][cel.y/2].objectIcon == 118 &&  // jeep
					 m_blupi[rank].bMalade &&
					 m_blupi[rank].takeChannel != -1 )  goto search;
				GoalStop(rank, true);
			}
			else
			{
				search:
//-				BlupiInitAction(rank, ACTION_MARCHE, direct);
				if ( SearchBestPass(rank, action) )
				{
//-					if ( (action >= ACTION_SAUTE2 &&
//-						  action <= ACTION_SAUTE5) ||
//-						 action == ACTION_GLISSE )
//-					{
						BlupiInitAction(rank, action);
//-					}
				}
				else
				{
					BlupiInitAction(rank, ACTION_STOP);
					if ( m_blupi[rank].perso == 0 ||  // blupi ?
						 m_blupi[rank].perso == 8 )   // disciple ?
					{
						if ( m_blupi[rank].busyCount == 0 )  // derni�re tentative ?
						{
							GoalStop(rank, true);
							m_blupi[rank].goalCel.x = -1;
							m_blupi[rank].goalPhase = 0;
							m_blupi[rank].interrupt = 1;
						}
					}
					else		// perso ennemi ?
					{
						// On cherchera un autre but !
						GoalStop(rank, true);
//?						m_blupi[rank].goalCel.x = -1;
//?						m_blupi[rank].goalPhase = 0;
//?						m_blupi[rank].interrupt = 1;
					}
				}
			}
		}
	}
	else
	{
		GoalNextPhase(rank);  // m�ta op�ration suivante
	}

	init:
	BlupiDestCel(rank);

	m_blupi[rank].phase = 0;
	m_blupi[rank].pos.x = 0;
	m_blupi[rank].pos.y = 0;

	BlupiNextAction(rank);
}

// Calcule la cellule de destination.

void CDecor::BlupiDestCel(int rank)
{
	int			a;
	POINT		vector;

	m_blupi[rank].destCel = m_blupi[rank].cel;

	if ( m_blupi[rank].action == ACTION_MARCHE   ||
		 m_blupi[rank].action == ACTION_MARCHEf  ||
		 m_blupi[rank].action == ACTION_MARCHEb  ||
		 m_blupi[rank].action == ACTION_MARCHEj  ||
		 m_blupi[rank].action == ACTION_MARCHEa  ||
		 m_blupi[rank].action == ACTION_A_MARCHE ||
		 m_blupi[rank].action == ACTION_V_MARCHE ||
		 m_blupi[rank].action == ACTION_T_MARCHE ||
		 m_blupi[rank].action == ACTION_R_MARCHE ||
		 m_blupi[rank].action == ACTION_B_MARCHE ||
		 m_blupi[rank].action == ACTION_E_MARCHE ||
		 m_blupi[rank].action == ACTION_D_MARCHE )
	{
		vector = GetVector(m_blupi[rank].sDirect);

		m_blupi[rank].destCel.x += vector.x;
		m_blupi[rank].destCel.y += vector.y;
	}

	a = GetAmplitude(m_blupi[rank].action);
	if ( a > 1 )
	{
		vector = GetVector(m_blupi[rank].sDirect);

		m_blupi[rank].destCel.x += vector.x*a;
		m_blupi[rank].destCel.y += vector.y*a;
	}
}

// Avance tous les blupis.

void CDecor::BlupiStep(bool bFirst)
{
	int		rank;

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist )
		{
			if ( !BlupiNextAction(rank) )
			{
				BlupiNextGoal(rank);
			}
		}
	}

	if ( bFirst )
	{
		m_timeConst ++;  // avance le temps absolu global constant

		if ( m_timeConst == m_timeFlipOutline )
		{
			m_bOutline = false;  // supprime le mode "outline"
		}
	}
	m_time ++;  // avance le temps absolu global
}


// Retourne le rectangle occup� par un blupi,
// pour les s�lections (pas exact).

void CDecor::BlupiGetRect(int rank, RECT &rect)
{
	POINT		pos;

	pos = ConvCelToPos(m_blupi[rank].cel);
	pos.x += m_blupi[rank].pos.x;
	pos.y += m_blupi[rank].pos.y-(DIMBLUPIY-DIMCELY)-SHIFTBLUPIY;

	rect.left   = pos.x+16;
	rect.top    = pos.y+10;
	rect.right  = pos.x+DIMBLUPIX-16;
	rect.bottom = pos.y+DIMBLUPIY;
}

// Retourne le blupi vis� par la souris.

int CDecor::GetTargetBlupi(POINT pos)
{
#if 1
	int			rank, found, prof;
	POINT		test, rel, cel;

	cel = ConvPosToCel(pos);

	found = -1;
	prof  = 0;
	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist &&
			 (m_blupi[rank].perso == 0 ||  // blupi ?
			  m_blupi[rank].perso == 8) )  // disciple ?
		{
			test = ConvCelToPos(m_blupi[rank].cel);
			test.x += m_blupi[rank].pos.x;
			test.y += m_blupi[rank].pos.y-(DIMBLUPIY-DIMCELY)-SHIFTBLUPIY;

			if ( pos.x >= test.x           &&
				 pos.x <= test.x+DIMBLUPIX &&
				 pos.y >= test.y           &&
				 pos.y <= test.y+DIMBLUPIY )
			{
				rel.x = pos.x-test.x;
				rel.y = pos.y-test.y;
				if ( (cel.x == m_blupi[rank].cel.x &&
					  cel.y == m_blupi[rank].cel.y ) ||
					 (cel.x == m_blupi[rank].destCel.x &&
					  cel.y == m_blupi[rank].destCel.y ) ||
					 m_pPixmap->IsIconPixel(m_blupi[rank].channel,
											m_blupi[rank].icon,
											rel) )
				{
					if ( found != -1 &&
						 test.y < prof )  continue;

					found = rank;
					prof  = test.y;
				}
			}
		}
	}

	return found;
#else
	int			rank, found, prof;
	RECT		rect;
	POINT		cel;

	found = -1;
	prof  = 0;
	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist &&
			 (m_blupi[rank].perso == 0 ||  // blupi ?
			  m_blupi[rank].perso == 8) )  // disciple ?
		{
			BlupiGetRect(rank, rect);

			if ( pos.x >= rect.left    &&
				 pos.x <= rect.right   &&
				 pos.y >= rect.top     &&
				 pos.y <= rect.bottom  )
			{
				if ( found != -1 &&
					 rect.top < prof )  continue;

				found = rank;
				prof  = rect.top;
			}
		}
	}

	if ( found != -1 )  return found;

	cel = ConvPosToCel(pos);

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist &&
			 (m_blupi[rank].perso == 0 ||  // blupi ?
			  m_blupi[rank].perso == 8) )  // disciple ?
		{
			if ( cel.x == m_blupi[rank].cel.x &&
				 cel.y == m_blupi[rank].cel.y )  return rank;

			if ( cel.x == m_blupi[rank].destCel.x &&
				 cel.y == m_blupi[rank].destCel.y )  return rank;
		}
	}

	return -1;
#endif
}


// D�slectionne tous les blupi.

void CDecor::BlupiDeselect()
{
	int		rank;

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		m_blupi[rank].bHili  = false;
		m_blupi[rank].bArrow = false;
	}

	m_nbBlupiHili   = 0;
	m_rankBlupiHili = -1;
}

// D�slectionne un blupi.

void CDecor::BlupiDeselect(int rank)
{
	m_blupi[rank].bHili  = false;
	m_blupi[rank].bArrow = false;

	if ( m_nbBlupiHili > 0 &&
		 m_rankBlupiHili == rank )  // est-ce le blupi s�lectionn� ?
	{
		m_nbBlupiHili   = 0;
		m_rankBlupiHili = -1;
	}
}

// Met ou enl�ve une fl�che au blupi s�lectionn� blupi.

void CDecor::BlupiSetArrow(int rank, bool bArrow)
{
	m_celArrow.x = -1;

	if ( bArrow )
	{
		m_blupi[rank].bArrow = true;
	}
	else
	{
		for ( rank=0 ; rank<MAXBLUPI ; rank++ )
		{
			m_blupi[rank].bArrow = false;
		}
	}
}

// Initialise la zone outline en fonction du rectangle de s�lection.

void CDecor::InitOutlineRect()
{
	if ( !m_bOutline && m_bHiliRect )
	{
		m_celOutline1.x = m_p1Hili.x<m_p2Hili.x ? m_p1Hili.x : m_p2Hili.x;
		m_celOutline1.y = m_p1Hili.y<m_p2Hili.y ? m_p1Hili.y : m_p2Hili.y;
		m_celOutline2.x = m_p1Hili.x>m_p2Hili.x ? m_p1Hili.x : m_p2Hili.x;
		m_celOutline2.y = m_p1Hili.y>m_p2Hili.y ? m_p1Hili.y : m_p2Hili.y;
		m_celOutline1.x = (m_celOutline1.x/2)*2;
		m_celOutline1.y = (m_celOutline1.y/2)*2;
		m_celOutline2.x += 2;
		m_celOutline2.y += 2;
	}
	else
	{
		m_celOutline1.x = -1;
		m_celOutline2.x = -1;
	}
}

// S�lectionne un blupi lorsque le bouton est press�.

void CDecor::BlupiHiliDown(POINT pos, bool bAdd)
{
	if ( MapMove(pos) )  return;

	if ( !bAdd )  BlupiDeselect();

	m_p1Hili = ConvPosToCel(pos);
	m_p2Hili = ConvPosToCel(pos);
	m_bHiliRect = true;
	m_celHili.x = -1;

	InitOutlineRect();
}

// S�lectionne un blupi lorsque la souris est d�plac�e.

void CDecor::BlupiHiliMove(POINT pos, bool bAdd)
{
	if ( m_bHiliRect )  // rectangle de s�lection existe ?
	{
		m_p2Hili = ConvPosToCel(pos);
		InitOutlineRect();
	}
}

// S�lectionne un blupi lorsque le bouton est rel�ch�.
// Retourne false si la s�lection n'a pas chang� !

void CDecor::BlupiHiliUp(POINT pos, bool bAdd)
{
	int			rank, r, nb, sound;
	bool		bEnerve = false;
	POINT		c1, c2;

	static int table_sound_ok[6] =
	{
		SOUND_OK1,
		SOUND_OK2,
		SOUND_OK3,
		SOUND_OK4,
		SOUND_OK5,
		SOUND_OK6,
	};

	static int table_sound_okf[3] =  // si fatigu�
	{
		SOUND_OK1f,
		SOUND_OK2f,
		SOUND_OK3f,
	};

	static int table_sound_oke[3] =  // si �nerv�
	{
		SOUND_OK1e,
		SOUND_OK2e,
		SOUND_OK3e,
	};

	if ( m_bHiliRect )  // rectangle de s�lection existe ?
	{
		nb = 0;

		if ( m_p1Hili.x == m_p2Hili.x &&
			 m_p1Hili.y == m_p2Hili.y )
		{
			rank = GetTargetBlupi(pos);  // rank <- blupi vis� par la souris
			if ( rank != -1 )
			{
				m_blupi[rank].bHili = !m_blupi[rank].bHili;
				if ( m_blupi[rank].bHili )
				{
					if ( m_blupi[rank].clicDelay > 0 )
					{
						m_blupi[rank].clicDelay = 80;
						m_blupi[rank].clicCount ++;
						if ( m_blupi[rank].clicCount > 4 )
						{
							bEnerve = true;
						}
					}
					else
					{
						m_blupi[rank].clicDelay = 40;
					}
					nb = 1;
				}
			}
		}
		else
		{
			if ( m_p1Hili.x < m_p2Hili.x )
			{
				c1.x = m_p1Hili.x;
				c2.x = m_p2Hili.x+1;
			}
			else
			{
				c1.x = m_p2Hili.x;
				c2.x = m_p1Hili.x+1;
			}

			if ( m_p1Hili.y < m_p2Hili.y )
			{
				c1.y = m_p1Hili.y;
				c2.y = m_p2Hili.y+1;
			}
			else
			{
				c1.y = m_p2Hili.y;
				c2.y = m_p1Hili.y+1;
			}

			for ( r=0 ; r<MAXBLUPI ; r++ )
			{
				if ( m_blupi[r].bExist &&
					 (m_blupi[r].perso == 0 ||  // blupi ?
					  m_blupi[r].perso == 8) )  // disciple ?
				{
					if ( m_blupi[r].cel.x >= c1.x &&
						 m_blupi[r].cel.x <  c2.x &&
						 m_blupi[r].cel.y >= c1.y &&
						 m_blupi[r].cel.y <  c2.y )
					{
						m_blupi[r].bHili = true;
						nb ++;
						rank = r;
					}
				}
			}
		}

		m_bHiliRect = false;  // plus de rectangle
		InitOutlineRect();

		if ( nb > 0 )
		{
			if ( nb > 1 )  // s�lection multiple ?
			{
				sound = table_sound_ok[Random(0,5)];
			}
			else
			{
				if ( m_blupi[rank].energy <= MAXENERGY/4 )
				{
					sound = table_sound_okf[Random(0,2)];
				}
				else
				{
					sound = table_sound_ok[Random(0,5)];
				}
				if ( bEnerve )  // d�j� s�lectionn� y'a peu ?
				{
					sound = table_sound_oke[Random(0,2)];
				}
			}
			BlupiSound(rank, sound, pos, true);
		}
	}

	m_nbBlupiHili   = 0;
	m_rankBlupiHili = -1;
	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		m_blupi[rank].bArrow = false;

		if ( m_blupi[rank].bExist &&
			 m_blupi[rank].bHili  )
		{
			m_nbBlupiHili ++;
			m_rankBlupiHili = rank;
		}
	}
}

// Dessine le rectangle de s�lection, si n�cessaire.

void CDecor::BlupiDrawHili()
{
	POINT		c1, c2, cc;
	POINT		p1, p2, p3, p4;
	POINT		start, pos;
	RECT		rect;
	int			shift;

	if ( !m_bHiliRect )  return;

	if ( m_p1Hili.x < m_p2Hili.x )
	{
		c1.x = m_p1Hili.x;
		c2.x = m_p2Hili.x+1;
	}
	else
	{
		c1.x = m_p2Hili.x;
		c2.x = m_p1Hili.x+1;
	}

	if ( m_p1Hili.y < m_p2Hili.y )
	{
		c1.y = m_p1Hili.y;
		c2.y = m_p2Hili.y+1;
	}
	else
	{
		c1.y = m_p2Hili.y;
		c2.y = m_p1Hili.y+1;
	}

	p1 = ConvCelToPos(c1);  // p1 en haut
	p2 = ConvCelToPos(c2);  // p2 en bas

	cc.x = c1.x;
	cc.y = c2.y;
	p3 = ConvCelToPos(cc);  // p3 � gauche

	cc.x = c2.x;
	cc.y = c1.y;
	p4 = ConvCelToPos(cc);  // p4 � droite

	p1.x += DIMCELX/2;
	p2.x += DIMCELX/2;
	p3.x += DIMCELX/2;
	p4.x += DIMCELX/2;

	shift = m_shiftHili%(64/2);

	start.x = p1.x-shift*2;
	start.y = p1.y-shift-1;
	while ( start.x < p4.x )
	{
		pos = start;
		rect.left   = 0;
		rect.right  = 64;
		rect.top    = 0;
		rect.bottom = 66/2;
		if ( pos.x+rect.right > p4.x )
		{
			rect.right = p4.x-pos.x;
		}
		if ( pos.x < p1.x )
		{
			rect.left +=  p1.x-pos.x;
			rect.top  += (p1.x-pos.x)/2;
			pos.x = p1.x;
			pos.y = p1.y-1;
		}
		m_pPixmap->DrawPart(-1, CHHILI, pos, rect);  // ligne p1-p4

		start.x += 64;
		start.y += 64/2;
	}

	start.x = p3.x-shift*2;
	start.y = p3.y-shift-1;
	while ( start.x < p2.x )
	{
		pos = start;
		rect.left   = 0;
		rect.right  = 64;
		rect.top    = 0;
		rect.bottom = 66/2;
		if ( pos.x+rect.right > p2.x )
		{
			rect.right = p2.x-pos.x;
		}
		if ( pos.x < p3.x )
		{
			rect.left +=  p3.x-pos.x;
			rect.top  += (p3.x-pos.x)/2;
			pos.x = p3.x;
			pos.y = p3.y-1;
		}
		m_pPixmap->DrawPart(-1, CHHILI, pos, rect);  // ligne p3-p2

		start.x += 64;
		start.y += 64/2;
	}

	start.x = p3.x-shift*2;
	start.y = p3.y+shift-66/2;
	while ( start.x < p1.x )
	{
		pos = start;
		rect.left   = 0;
		rect.right  = 64;
		rect.top    = 66/2;
		rect.bottom = 66;
		if ( pos.x+rect.right > p1.x )
		{
			rect.right = p1.x-pos.x;
		}
		if ( pos.x < p3.x )
		{
			rect.left   +=  p3.x-pos.x;
			rect.bottom -= (p3.x-pos.x)/2;
			pos.x = p3.x;
		}
		m_pPixmap->DrawPart(-1, CHHILI, pos, rect);  // ligne p3-p1

		start.x += 64;
		start.y -= 64/2;
	}

	start.x = p2.x-shift*2;
	start.y = p2.y+shift-66/2;
	while ( start.x < p4.x )
	{
		pos = start;
		rect.left   = 0;
		rect.right  = 64;
		rect.top    = 66/2;
		rect.bottom = 66;
		if ( pos.x+rect.right > p4.x )
		{
			rect.right = p4.x-pos.x;
		}
		if ( pos.x < p2.x )
		{
			rect.left   +=  p2.x-pos.x;
			rect.bottom -= (p2.x-pos.x)/2;
			pos.x = p2.x;
		}
		m_pPixmap->DrawPart(-1, CHHILI, pos, rect);  // ligne p2-p4

		start.x += 64;
		start.y -= 64/2;
	}

	m_shiftHili += 3;
}


// Retourne le bouton par d�faut � un endroit donn�.
// Est utilis� pour trouver que faire lors d'un clic
// avec le bouton de droite.

int CDecor::GetDefButton(POINT cel)
{
	int		button, rank, channel, icon;
	POINT	iCel;

	iCel = cel;
	cel.x = (cel.x/2)*2;
	cel.y = (cel.y/2)*2;
	GetObject(cel, channel, icon);

	if ( m_nbBlupiHili == 0 )  return -1;
	if ( m_nbBlupiHili >  1 )  return BUTTON_GO;
	rank = m_rankBlupiHili;

	button = BUTTON_GO;

	if ( channel == CHOBJECT )
	{
		if ( icon >=  7 &&
			 icon <= 11 )  button = BUTTON_ABAT;
		if ( icon >= 37 &&
			 icon <= 43 )  button = BUTTON_ROC;

		if ( icon ==  61 )  button = BUTTON_CULTIVE; // cabane
		if ( icon == 122 )  button = BUTTON_EXTRAIT; // extrait

		if ( iCel.x%2 == 1 && iCel.y%2 == 1 )
		{
			if ( icon ==  14 )  button = BUTTON_CARRY;   // m�tal
			if ( icon ==  36 )  button = BUTTON_CARRY;   // planches
			if ( icon ==  44 )  button = BUTTON_CARRY;   // pierres
			if ( icon ==  60 )  button = BUTTON_MANGE;   // tomates
			if ( icon ==  63 )  button = BUTTON_CARRY;   // oeufs
			if ( icon ==  80 )  button = BUTTON_BOIT;    // bouteille
			if ( icon ==  82 )  button = BUTTON_CARRY;   // fleurs
			if ( icon ==  84 )  button = BUTTON_CARRY;   // fleurs
			if ( icon ==  95 )  button = BUTTON_CARRY;   // fleurs
			if ( icon ==  85 )  button = BUTTON_CARRY;   // dynamite
			if ( icon ==  92 )  button = BUTTON_CARRY;   // poison
			if ( icon ==  93 )  button = BUTTON_CARRY;   // pi�ge
			if ( icon == 123 )  button = BUTTON_CARRY;   // fer
			if ( icon == 125 )  button = BUTTON_CARRY;   // mine
		}

		if ( icon == 28 &&  // laboratoire ?
			 m_blupi[rank].energy > MAXENERGY/4 &&
			 m_blupi[rank].takeChannel == CHOBJECT &&
			 (m_blupi[rank].takeIcon == 82 ||  // porte fleurs ?
			  m_blupi[rank].takeIcon == 84 ||
			  m_blupi[rank].takeIcon == 95 ||
			  m_blupi[rank].takeIcon == 60) )  // porte tomates ?
		{
			button = BUTTON_LABO;  // transforme
		}
	}

	if ( !m_blupi[rank].bMalade &&
		 button == BUTTON_BOIT )
	{
		button = BUTTON_CARRY;  // porte la bouteille si pas malade
	}

	if ( (m_blupi[rank].energy <= MAXENERGY/4 ||
		  m_blupi[rank].takeChannel != -1     ) &&
		 (button == BUTTON_ABAT    ||
		  button == BUTTON_CARRY   ||
		  button == BUTTON_ROC     ||
		  button == BUTTON_CULTIVE ) )
	{
		return -1;
	}

	if ( m_blupi[rank].energy > (MAXENERGY/4)*3 &&
		 button == BUTTON_MANGE )
	{
		button = BUTTON_CARRY;
	}

	if ( m_buttonExist[button] == 0 )  // bouton existe ?
	{
		return -1;
	}

	return button;
}

// Indique un but vis� � long terme, pour un blupi donn�.

bool CDecor::BlupiGoal(int rank, int button, POINT cel, POINT cMem)
{
	POINT		goalHili, goalHili2, goal, test;
	int			i, action, channel, icon, error, direct, step;
	bool		bRepeat = false;

	// Si plusieurs blupi sont s�lectionn�s, ils ne vont pas
	// tous � la m�me destination.
	if ( button == BUTTON_GO )
	{
		step = 0;
		for ( i=0 ; i<rank ; i++ )
		{
			if ( m_blupi[i].bExist &&
				 m_blupi[i].bHili  )  step ++;
		}
		if ( step > 15 )  step = 15;
		cel.x  += table_multi_goal[step*2+0];
		cel.y  += table_multi_goal[step*2+1];
		cMem.x += table_multi_goal[step*2+0];
		cMem.y += table_multi_goal[step*2+1];
	}

	if ( !IsCheminFree(rank, cel, button) )  return false;

	goal        = cel;
	goalHili    = cel;
	goalHili2.x = (cel.x/2)*2;
	goalHili2.y = (cel.y/2)*2;

	if ( button == BUTTON_GO &&
		 m_decor[goalHili.x/2][goalHili.y/2].objectIcon == 113 )  // maison ?
	{
		goalHili.x = (goalHili.x/2)*2+1;
		goalHili.y = (goalHili.y/2)*2+1;
	}

	if ( button == BUTTON_ABATn )
	{
		button = BUTTON_ABAT;
		bRepeat = true;
	}
	if ( button == BUTTON_ROCn )
	{
		button = BUTTON_ROC;
		bRepeat = true;
	}
	if ( button == BUTTON_FLEURn )
	{
		button = BUTTON_FLEUR;
		bRepeat = true;
	}
	action = table_actions[button];

	if ( action == WM_ACTION_STOP )
	{
		if ( m_blupi[rank].goalAction != 0 &&
			 m_blupi[rank].interrupt <= 0 )
		{
			m_blupi[rank].stop = 1;  // faudra stopper
		}
		else
		{
			m_blupi[rank].goalCel = m_blupi[rank].destCel;
			m_blupi[rank].goalAction = 0;
		}
		m_blupi[rank].repeatLevel = -1;  // stoppe la r�p�tition
		return false;
	}

	// Action prioritaire en cours ?
	if ( m_blupi[rank].goalAction != 0 &&
		 m_blupi[rank].interrupt <= 0 )  return false;

	error = CelOkForAction(goalHili, action, rank);
	if ( error != 0 && error != ERROR_TOURISOL )  return false;

	if ( action == WM_ACTION_GO &&
		 m_blupi[rank].energy <= MAXENERGY/4 &&
		 m_blupi[rank].takeChannel != -1 )  return false;

	if ( action == WM_ACTION_GO )
	{
		GetObject(goalHili2, channel, icon);
		if ( channel == CHOBJECT &&
			 icon == 120 &&  // usine ?
			 goalHili.x%2 == 0 &&  // au fond ?
			 goalHili.y%2 == 1 )
		{
			return false;  // action refus�e
		}
		if ( m_blupi[rank].perso != 8 &&  // pas disciple ?
			 channel == CHOBJECT &&
			 icon == 118 &&  // jeep ?
			 goalHili.x%2 == 1 &&  // sur la jeep ?
			 goalHili.y%2 == 1 )
		{
			action = WM_ACTION_MJEEP;
		}
		if ( m_blupi[rank].perso != 8 &&  // pas disciple ?
			 m_blupi[rank].takeChannel == -1 &&  // ne porte rien ?
			 channel == CHOBJECT &&
			 icon == 16 &&  // armure ?
			 goalHili.x%2 == 1 &&  // sur l'armure ?
			 goalHili.y%2 == 1 )
		{
			action = WM_ACTION_MARMURE;
		}
		if ( m_blupi[rank].perso != 8 &&  // pas disciple ?
			 channel == CHOBJECT &&
			 icon == 113 )  // maison ?
		{
			action = WM_ACTION_MAISON;
		}
		GetFloor(goalHili2, channel, icon);
		if ( m_blupi[rank].perso == 0 &&
			 m_blupi[rank].vehicule == 0 &&  // � pied ?
			 m_blupi[rank].takeChannel == -1 &&  // ne porte rien ?
			 channel == CHFLOOR &&
			 icon == 80 )  // t�l�porteur ?
		{
			if ( cel.x%2 == 0 && cel.y%2 == 0 )
			{
				action = WM_ACTION_TELEPORTE00;
			}
			if ( cel.x%2 == 1 && cel.y%2 == 0 )
			{
				action = WM_ACTION_TELEPORTE10;
			}
			if ( cel.x%2 == 0 && cel.y%2 == 1 )
			{
				action = WM_ACTION_TELEPORTE01;
			}
			if ( cel.x%2 == 1 && cel.y%2 == 1 )
			{
				action = WM_ACTION_TELEPORTE11;
			}
		}
		IsFreeCelEmbarque(goalHili, rank, action, goal);
		IsFreeCelDebarque(goalHili, rank, action, goal);
	}

	if ( action == WM_ACTION_DEPOSE &&
		 m_blupi[rank].energy <= MAXENERGY/4 )
	{
		// Energie juste pour d�poser l'objet transport�.
		m_blupi[rank].energy = MAXENERGY/4+20;
	}

	if ( action == WM_ACTION_ABAT1 )
	{
		GetObject(goalHili2, channel, icon);
		if ( channel == CHOBJECT &&
			 icon >= 6 && icon <= 11 )  // arbre ?
		{
			action += icon-6;  // WM_ACTION_ABAT1..6
		}
	}

	if ( action == WM_ACTION_ROC1 )
	{
		GetObject(goalHili2, channel, icon);
		if ( channel == CHOBJECT &&
			 icon >= 37 && icon <= 43 )  // rochers ?
		{
			action += icon-37;  // WM_ACTION_ROC1..7
		}
	}

	if ( action == WM_ACTION_FLEUR1 )
	{
		GetObject(goalHili2, channel, icon);
		if ( channel == CHOBJECT &&
			 icon == 83 )  // fleurs fonc�es ?
		{
			action = WM_ACTION_FLEUR2;
		}
		if ( channel == CHOBJECT &&
			 icon == 94 )  // fleurs vertes ?
		{
			action = WM_ACTION_FLEUR3;
		}
	}

	if ( action == WM_ACTION_PONTE )
	{
		cel  = goalHili2;
		test = goalHili2;
		if ( IsBuildPont(test, icon) != 0 )  return false;

		m_blupi[rank].nLoop = static_cast<short> (abs((test.x-cel.x)+(test.y-cel.y))/2);
		m_blupi[rank].cLoop = 0;
		m_blupi[rank].vIcon = icon;
		m_blupi[rank].fix   = cel;

		if ( test.x-cel.x < 0 )  action = WM_ACTION_PONTO;
		if ( test.y-cel.y > 0 )  action = WM_ACTION_PONTS;
		if ( test.y-cel.y < 0 )  action = WM_ACTION_PONTN;
	}

	if ( action == WM_ACTION_BATEAUE )
	{
		if ( !IsBuildBateau(goalHili2, direct) )  return false;

		if ( direct == DIRECT_S )  action = WM_ACTION_BATEAUS;
		if ( direct == DIRECT_O )  action = WM_ACTION_BATEAUO;
		if ( direct == DIRECT_N )  action = WM_ACTION_BATEAUN;
	}

	if ( action == WM_ACTION_CARRY )
	{
		if ( IsBlupiHereEx(GetCel(goalHili2,0,1), rank, true) )
		{
			action = WM_ACTION_CARRY2;
		}
	}

	if ( action == WM_ACTION_DEPOSE )
	{
		GetFloor(goalHili2, channel, icon);
		if ( channel == CHFLOOR && icon == 52 &&  // nurserie ?
			 m_blupi[rank].takeChannel == CHOBJECT &&
			 m_blupi[rank].takeIcon    == 63 )  // oeufs ?
		{
			action = WM_ACTION_NEWBLUPI;
		}
		if ( !IsFreeCelDepose(GetCel(goalHili2,0,1), rank) ||
			 IsBlupiHereEx(GetCel(goalHili2,0,1), rank, true) )
		{
			action = WM_ACTION_DEPOSE2;
		}
	}

	if ( action == WM_ACTION_MANGE )
	{
		if ( IsBlupiHereEx(GetCel(goalHili2,0,1), rank, true) )
		{
			action = WM_ACTION_MANGE2;
		}
	}

	if ( action == WM_ACTION_BOIT )
	{
		if ( IsBlupiHereEx(GetCel(goalHili2,0,1), rank, true) )
		{
			action = WM_ACTION_BOIT2;
		}
	}

	if ( action == WM_ACTION_DYNAMITE )
	{
		GetObject(goalHili2, channel, icon);
		if ( channel == CHOBJECT &&
			 icon == 125 )  // mine ?
		{
			action = WM_ACTION_MINE;
		}
		else
		{
			if ( m_blupi[rank].takeChannel == CHOBJECT &&
				 m_blupi[rank].takeIcon == 85 )  // porte dynamite ?
			{
				action = WM_ACTION_DYNAMITE2;
			}
		}
	}

	GoalStart(rank, action, goal);
	m_blupi[rank].bRepeat   = bRepeat;
	m_blupi[rank].busyCount = 5;  // 5 tentatives au maximum
	m_blupi[rank].busyDelay = 0;

	if ( action == WM_ACTION_REPEAT )
	{
		m_blupi[rank].repeatLevel = m_blupi[rank].repeatLevelHope;
		m_blupi[rank].listCel[m_blupi[rank].repeatLevel] = goal;
	}
	else
	{
		ListPut(rank, button, goal, cMem);
	}

	return true;
}

// Indique un but vis� � long terme, pour tous les blupi
// s�lectionn�s.

void CDecor::BlupiGoal(POINT cel, int button)
{
	POINT		bPos, avg;
	int			rank, nb, nbHili;

	static int table_sound_go[6] =
	{
		SOUND_GO1,
		SOUND_GO2,
		SOUND_GO3,
		SOUND_GO4,
		SOUND_GO5,
		SOUND_GO6,
	};

	static int table_sound_gom[3] =
	{
		SOUND_GO4,
		SOUND_GO5,
		SOUND_GO6,
	};

	static int table_sound_boing[3] =
	{
		SOUND_BOING1,
		SOUND_BOING2,
		SOUND_BOING3,
	};

	if ( button == -1 )
	{
		avg = ConvCelToPos(cel);
		m_pSound->PlayImage(table_sound_boing[Random(0,2)], avg);
		return;
	}

	avg.x  = 0;
	avg.y  = 0;
	nb     = 0;
	nbHili = 0;
	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist &&
			 m_blupi[rank].bHili  )
		{
			bPos = ConvCelToPos(m_blupi[rank].cel);
			avg.x += bPos.x;
			avg.y += bPos.y;
			nbHili ++;

			if ( BlupiGoal(rank, button, cel, cel) )
			{
				nb  ++;
			}
		}
	}

	if ( button == BUTTON_STOP )  return;

	if ( nbHili > 0 )
	{
		avg.x /= nbHili;
		avg.y /= nbHili;
	}
	if ( avg.x < 0       )  avg.x = 0;
	if ( avg.x > LXIMAGE )  avg.x = LXIMAGE;
	avg.y = LYIMAGE/2;

	if ( nb == 0 && nbHili > 0 )
	{
		if ( nbHili == 1 )
		{
			BlupiSound(m_rankBlupiHili, table_sound_boing[Random(0,2)], avg, true);
		}
		else
		{
			m_pSound->PlayImage(table_sound_boing[Random(0,2)], avg);
		}
	}

	if ( nb > 0 )
	{
		if ( nbHili == 1 )
		{
			BlupiSound(m_rankBlupiHili, table_sound_go[Random(0,5)], avg, true);
		}
		else
		{
			m_pSound->PlayImage(table_sound_gom[Random(0,2)], avg);
		}
	}
}


// Indique si une cellule est occup�e pour un tracks.
// La cellule est consid�r�e libre uniquement si elle
// contient un blupi � pied ou un d�tonnateur de mine
// (personnage invisible).

bool CDecor::IsTracksHere(POINT cel, bool bSkipInMove)
{
	int			rank;

	if ( !IsValid(cel) )  return false;

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist &&
			 (m_blupi[rank].perso != 0    ||  // blupi ?
			  m_blupi[rank].vehicule != 0 ||  // � pied ?
			  m_bInvincible) &&
			 m_blupi[rank].perso != 6 )  // d�tonnateur ?
		{
			if ( bSkipInMove && m_blupi[rank].goalCel.x != -1 )  continue;

			if ( cel.x == m_blupi[rank].cel.x &&
				 cel.y == m_blupi[rank].cel.y )
			{
				m_blupiHere = rank;
				return true;
			}

			if ( cel.x == m_blupi[rank].destCel.x &&
				 cel.y == m_blupi[rank].destCel.y )
			{
				m_blupiHere = rank;
				return true;
			}
		}
	}

	return false;
}

// Indique si une cellule est occup�e par un blupi.
// Le blupi donn� dans exRank est ignor� !

bool CDecor::IsBlupiHereEx(POINT cel1, POINT cel2, int exRank, bool bSkipInMove)
{
	int			rank;

	if ( !IsValid(cel1) )  return false;
	if ( !IsValid(cel2) )  return false;

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist &&
			 m_blupi[rank].perso != 6 &&  // pas le d�tonnateur de mine
			 rank != exRank )
		{
			if ( bSkipInMove && m_blupi[rank].goalCel.x != -1 )  continue;

			if ( cel1.x <= m_blupi[rank].cel.x &&
				 cel2.x >= m_blupi[rank].cel.x &&
				 cel1.y <= m_blupi[rank].cel.y &&
				 cel2.y >= m_blupi[rank].cel.y )
			{
				m_blupiHere = rank;
				return true;
			}

			if ( cel1.x <= m_blupi[rank].destCel.x &&
				 cel2.x >= m_blupi[rank].destCel.x &&
				 cel1.y <= m_blupi[rank].destCel.y &&
				 cel2.y >= m_blupi[rank].destCel.y )
			{
				m_blupiHere = rank;
				return true;
			}
		}
	}

	return false;
}

// Indique si une cellule est occup�e par un blupi.
// Le blupi donn� dans exRank est ignor� !

bool CDecor::IsBlupiHereEx(POINT cel, int exRank, bool bSkipInMove)
{
	int			rank;

	if ( !IsValid(cel) )  return false;

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist &&
			 m_blupi[rank].perso != 6 &&  // pas le d�tonnateur de mine
			 rank != exRank )
		{
			if ( bSkipInMove && m_blupi[rank].goalCel.x != -1 )  continue;

			if ( cel.x == m_blupi[rank].cel.x &&
				 cel.y == m_blupi[rank].cel.y )
			{
				m_blupiHere = rank;
				return true;
			}

			if ( cel.x == m_blupi[rank].destCel.x &&
				 cel.y == m_blupi[rank].destCel.y )
			{
				m_blupiHere = rank;
				return true;
			}
		}
	}

	return false;
}

// Indique si une cellule est occup�e par un blupi.

bool CDecor::IsBlupiHere(POINT cel, bool bSkipInMove)
{
	return IsBlupiHereEx(cel, -1, bSkipInMove);
}

// Indique si une cellule future (dans une direction donn�e)
// est d�j� occup�e par un blupi.

bool CDecor::IsBlupiHere(POINT cel, int direct, bool bSkipInMove)
{
	POINT	vector;

	vector = GetVector(direct);

	cel.x += vector.x;
	cel.y += vector.y;

	return IsBlupiHereEx(cel, -1, bSkipInMove);
}


// Retourne les niveaux des jauges.

void CDecor::GetLevelJauge(int *pLevels, int *pTypes)
{
	int			rank;

	pLevels[0] = -1;
	pLevels[1] = -1;

	rank = m_rankBlupiHili;

	if ( m_nbBlupiHili == 1 )  // un seul blupi s�lectionn� ?
	{
		pLevels[0] = (m_blupi[rank].energy*100)/MAXENERGY;

		pTypes[0] = 1;  // rouge
		if ( m_blupi[rank].energy > MAXENERGY/4 )
		{
			pTypes[0] = 2;  // bleu
		}
	}

	if ( m_blupi[rank].interrupt == 0 &&
		 m_blupi[rank].jaugeMax > 0 )
	{
		pLevels[1] = (m_blupi[rank].jaugePhase*100)/
					  m_blupi[rank].jaugeMax;
		pTypes[1] = 3;  // jaune
	}
}


// Retourne true si un blupi est d�j� s�lectionn� et qu'il
// effectue une action prioritaire. Dans ce cas, il faut tout
// de suite mettre le menu "stoppe" s'il est cliqu�.

bool CDecor::IsWorkBlupi(int rank)
{
	if ( m_blupi[rank].bHili &&
		 m_blupi[m_rankBlupiHili].goalAction != 0 &&
		 m_blupi[m_rankBlupiHili].interrupt <= 0 )  return true;

	return false;
}


// Retourne les boutons possibles � un endroit donn�,
// pour le blupi s�lectionn�.

void CDecor::BlupiGetButtons(POINT pos, int &nb,
							 int *pButtons, int *pErrors,
							 std::unordered_map<int, const char *> &texts, int &perso)
{
	int*		pB = pButtons;
	int*		pE = pErrors;
	POINT		cel, cel2;
	int			i, rank, button, error, channel, icon;
	bool		bBuild = false;
	bool		bPut;
	const char *textForButton;

	static int table_buttons[] =
	{
		BUTTON_GO,			0,
		BUTTON_DJEEP,		0,
		BUTTON_DARMURE,		0,
		BUTTON_MANGE,		0,
		BUTTON_BOIT,		0,
		BUTTON_CARRY,		0,
		BUTTON_DEPOSE,		0,
		BUTTON_LABO,		0,
		BUTTON_ABAT,		0,
		BUTTON_ABATn,		0,
		BUTTON_ROC,			0,
		BUTTON_ROCn,		0,
		BUTTON_CULTIVE,		0,
		BUTTON_FLEUR,		0,
		BUTTON_FLEURn,		0,
		BUTTON_DYNAMITE,	0,
		BUTTON_DRAPEAU,		0,
		BUTTON_EXTRAIT,		0,
		BUTTON_FABJEEP,		0,
		BUTTON_FABMINE,		0,
		BUTTON_FABDISC,		0,
		BUTTON_FABARMURE,	0,
		BUTTON_BUILD1,		36,  // si planches (cabane)
		BUTTON_BUILD2,		36,  // si planches (nurserie)
		BUTTON_BUILD4,		36,  // si planches (mine)
		BUTTON_PALIS,		36,  // si planches
		BUTTON_PONT,		36,  // si planches
		BUTTON_BATEAU,		36,  // si planches
		BUTTON_BUILD6,		36,  // si planches (t�l�porteur)
		BUTTON_BUILD3,		44,  // si pierres (laboratoire)
		BUTTON_BUILD5,		44,  // si pierres (usine)
		BUTTON_MUR,			44,  // si pierres
		BUTTON_TOUR,		44,  // si pierres
		BUTTON_STOP,		0,
		-1
	};

	nb    = 0;
	perso = 0;

	cel  = ConvPosToCel(pos);
	cel2 = ConvPosToCel2(pos);

	if ( m_nbBlupiHili == 0 )  return;

	if ( m_nbBlupiHili > 1 )  // s�lection multiple ?
	{
		error = CelOkForAction(cel, table_actions[BUTTON_GO], m_rankBlupiHili);
		if ( error == 0 )
		{
			*pB++ = BUTTON_GO;
			*pE++ = 0;
			nb ++;
		}

		for ( rank=0 ; rank<MAXBLUPI ; rank++ )
		{
			if ( m_blupi[rank].bExist &&
				 m_blupi[rank].goalAction != 0 )
			{
				*pB++ = BUTTON_STOP;
				*pE++ = 0;
				nb ++;
				break;
			}
		}

		return;
	}

	if ( m_nbBlupiHili != 1 )  return;

	perso = m_blupi[m_rankBlupiHili].perso;

	// Si action prioritaire en cours -> seulement stoppe.
	if ( m_blupi[m_rankBlupiHili].goalAction != 0 &&
		 m_blupi[m_rankBlupiHili].interrupt <= 0 )
	{
		if ( abs(m_blupi[m_rankBlupiHili].cel.x-cel.x) <= 3 &&
			 abs(m_blupi[m_rankBlupiHili].cel.y-cel.y) <= 3 &&
			 CelOkForAction(cel, table_actions[BUTTON_STOP], m_rankBlupiHili) == 0 )
		{
			*pB++ = BUTTON_STOP;
			*pE++ = 0;
			nb ++;
		}
		return;
	}

	// V�rifie si le blupi s�lectionn� peut construire.
	if ( m_rankBlupiHili >= 0 )
	{
		if ( m_blupi[m_rankBlupiHili].energy > MAXENERGY/4 &&
			 m_blupi[m_rankBlupiHili].takeChannel == -1 &&
			 m_blupi[m_rankBlupiHili].vehicule == 0 )  // � pied ?
		{
			bBuild = true;
		}
	}

	// Met les diff�rentes actions.
	i = 0;
	while ( table_buttons[i] != -1 )
	{
		button = table_buttons[i];

		if ( m_buttonExist[button] == 0 )  goto next;

		error = CelOkForAction(cel, table_actions[button], m_rankBlupiHili);

		if ( error == 0 )  bPut = true;
		else               bPut = false;

		if ( bBuild &&
			 table_buttons[i+1] != 0 &&  // toujours pr�sent si mati�re ?
			 (m_rankBlupiHili < 0 ||
			  m_blupi[m_rankBlupiHili].perso != 8 ||  // pas disciple ?
			  table_buttons[i+1] != 44) )  // ni pierres ?
		{
			GetObject(cel2, channel, icon);
			if ( channel == CHOBJECT &&
				 icon == table_buttons[i+1] &&  // mati�re ?
				 cel.x%2 == 1 && cel.y%2 == 1 )
			{
				bPut = true;  // bouton pr�sent, mais disable !
			}
		}

		if ( bPut )
		{
			*pB++ = button;
			*pE++ = error;
			nb ++;
		}

		next:
		i += 2;
	}

	// Si le premier bouton est "abat", ajoute "va" devant !
	if ( pButtons[0] == BUTTON_ABAT )
	{
		for ( i=nb ; i>0 ; i-- )
		{
			pButtons[i] = pButtons[i-1];
			pErrors[i]  = pErrors[i-1];
		}

		pButtons[0] = BUTTON_GO;
		pErrors[0]  = ERROR_MISC;
		nb ++;
	}

	// Regarde s'il faut ajouter le bouton "r�p�te".
	if ( m_blupi[m_rankBlupiHili].repeatLevel != -1 ||
		 m_blupi[m_rankBlupiHili].energy <= MAXENERGY/4 ||
		 m_buttonExist[BUTTON_REPEAT] == 0 )
	{
		return;
	}

	for ( i=0 ; i<nb ; i++ )
	{
		rank = ListSearch(m_rankBlupiHili, pButtons[i], cel, textForButton);
		if ( rank > 0 )  // au moins 2 actions � r�p�ter ?
		{
			m_blupi[m_rankBlupiHili].repeatLevelHope = rank;

			pButtons[nb] = BUTTON_REPEAT;
			pErrors[nb]  = 500;
			texts[nb]    = textForButton;
			nb ++;
			return;
		}
	}
}


// Initialise les conditions de fin.

void CDecor::TerminatedInit()
{
	m_winCount = 50;

	m_winLastHachBlupi   = 0;
	m_winLastHachPlanche = 0;
	m_winLastHachTomate  = 0;
	m_winLastHachMetal   = 0;
	m_winLastHachRobot   = 0;
	m_winLastHome        = 0;
	m_winLastHomeBlupi   = 0;
	m_winLastRobots      = 0;
}

// V�rifie si la partie est termin�e.
// Retourne 0 si la partie n'est pas termin�e.
// Retourne 1 si la partie est perdue.
// Retourne 2 si la partie est gagn�e.

int CDecor::IsTerminated()
{
	int		nb, count, out;
	POINT	pos;

	pos.x = LXIMAGE/2;
	pos.y = LYIMAGE/2;

	count = m_winCount;
	m_winCount = 50;

	if ( m_winLastHome > m_nbStatHome )  // une maison en moins ?
	{
		out = 1;  // perdu
		goto delay;
	}
	m_winLastHome = m_nbStatHome;

	nb = StatisticGetBlupi();
	if ( nb < m_term.nbMinBlupi )
	{
		out = 1;  // perdu
		goto delay;
	}
	if ( nb < m_term.nbMaxBlupi )  return 0;  // continue

	if ( m_term.bStopFire )
	{
		nb = StatisticGetFire();
		if ( nb > 0 )  return 0;  // continue;
	}

	if ( m_term.bHachBlupi )
	{
		if ( m_winLastHachBlupi < m_nbStatHachBlupi )
		{
			m_pSound->PlayImage(SOUND_BUT, pos);
		}
		m_winLastHachBlupi = m_nbStatHachBlupi;

		if ( m_nbStatHachBlupi < m_nbStatHach*4 )  return 0;  // continue;
	}

	if ( m_term.bHachPlanche )
	{
		if ( m_winLastHachPlanche < m_nbStatHachPlanche )
		{
			m_pSound->PlayImage(SOUND_BUT, pos);
		}
		m_winLastHachPlanche = m_nbStatHachPlanche;

		if ( m_nbStatHachPlanche < m_nbStatHach )  return 0;  // continue;
	}

	if ( m_term.bHachTomate )
	{
		if ( m_winLastHachTomate < m_nbStatHachTomate )
		{
			m_pSound->PlayImage(SOUND_BUT, pos);
		}
		m_winLastHachTomate = m_nbStatHachTomate;

		if ( m_nbStatHachTomate < m_nbStatHach )  return 0;  // continue;
	}

	if ( m_term.bHachMetal )
	{
		if ( m_winLastHachMetal < m_nbStatHachMetal )
		{
			m_pSound->PlayImage(SOUND_BUT, pos);
		}
		m_winLastHachMetal = m_nbStatHachMetal;

		if ( m_nbStatHachMetal < m_nbStatHach )  return 0;  // continue;
	}

	if ( m_term.bHachRobot )
	{
		if ( m_winLastRobots > m_nbStatRobots )
		{
			out = 1;  // perdu
			goto delay;
		}
		m_winLastRobots = m_nbStatRobots;

		if ( m_winLastHachRobot < m_nbStatHachRobot )
		{
			m_pSound->PlayImage(SOUND_BUT, pos);
		}
		m_winLastHachRobot = m_nbStatHachRobot;

		if ( m_nbStatHachRobot < m_nbStatHach )  return 0;  // continue;
	}

	if ( m_term.bHomeBlupi )
	{
		if ( m_winLastHomeBlupi < m_nbStatHomeBlupi )
		{
			m_pSound->PlayImage(SOUND_BUT, pos);
		}
		m_winLastHomeBlupi = m_nbStatHomeBlupi;

		if ( m_nbStatHomeBlupi < m_nbStatHome )  return 0;  // continue;
	}

	if ( m_term.bKillRobots )
	{
		if ( m_winLastRobots > m_nbStatRobots )
		{
			m_pSound->PlayImage(SOUND_BUT, pos);
		}
		m_winLastRobots = m_nbStatRobots;

		if ( m_nbStatRobots > 0 )  return 0;  // continue;
	}

	out = 2;  // gagn�

	delay:
	m_winCount = count;
	if ( m_winCount == 0 )
	{
		if ( out == 1 )  // perdu ?
		{
			if ( !m_pSound->PlayImage(SOUND_LOST, pos) )
			{
				m_pSound->PlayImage(SOUND_BUT, pos);
			}
		}
		else
		{
			if ( !m_pSound->PlayImage(SOUND_WIN, pos) )
			{
				m_pSound->PlayImage(SOUND_BUT, pos);
			}
		}

		return out;  // perdu/gagn�
	}
	m_winCount --;
	return 0;  // continue
}

// Retourne la structure pour terminer une partie.

Term* CDecor::GetTerminated()
{
	return &m_term;
}
