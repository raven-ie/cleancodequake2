#define MAX_COMMANDS 128
#define MAX_CMD_HASH (MAX_COMMANDS/4)

enum ECmdTypeFlags
{
	CMD_NORMAL		= 0, // Doesn't do anything at all, just here for show.
	CMD_SPECTATOR	= 1,
	CMD_CHEAT		= 2
};

class CCmd
{
public:
	uint32			hashValue;
	CCmd			*hashNext;
	char			*cmdName;

	ECmdTypeFlags	CmdFlags;
	void			(*RunFunction) (edict_t *ent);

	CCmd (char *name, void (*Func) (edict_t *ent), ECmdTypeFlags Flags);
	~CCmd();
	void Run (edict_t *ent);
};