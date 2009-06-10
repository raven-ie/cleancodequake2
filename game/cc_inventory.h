class CInventory
{
private:
	int		Array[MAX_CS_ITEMS];

public:
	int		SelectedItem;

	void	operator += (CBaseItem *Item);
	void	operator += (int Index);
	void	operator -= (CBaseItem *Item);
	void	operator -= (int Index);

	CInventory ();
	// Note: All of these functions are for directly
	// adding items. This will NOT call the pickup
	// function! You need to add your own checks in for
	// ammo and such if you use these functions.
	void	Add (CBaseItem *Item, int Num);
	void	Remove (CBaseItem *Item, int Num);
	void	Add (int Index, int Num);
	void	Remove (int Index, int Num);

	// Draw the inventory
	void	Draw (CPlayerEntity *ent);

	// Validate items selected with functions below
	void	ValidateSelectedItem ();

	void	SelectNextItem (EItemFlags Flags);
	void	SelectPrevItem (EItemFlags Flags);

	int		Has	(int Index);
	int		Has	(CBaseItem *Item);

	void	Set	(CBaseItem *Item, int Num);
	void	Set	(int Index, int Num);
};

void Cmd_Use_f (CPlayerEntity *ent);
void Cmd_UseList_f (CPlayerEntity *ent);
void Cmd_Drop_f (CPlayerEntity *ent);
void Cmd_Inven_f (CPlayerEntity *ent);
void Cmd_InvUse_f (CPlayerEntity *ent);
void Cmd_WeapPrev_f (CPlayerEntity *ent);
void Cmd_WeapNext_f (CPlayerEntity *ent);
void Cmd_WeapLast_f (CPlayerEntity *ent);
void Cmd_InvDrop_f (CPlayerEntity *ent);
void Cmd_SelectNextItem_f (CPlayerEntity *ent);
void Cmd_SelectPrevItem_f (CPlayerEntity *ent);
void Cmd_SelectNextWeapon_f (CPlayerEntity *ent);
void Cmd_SelectPrevWeapon_f (CPlayerEntity *ent);
void Cmd_SelectNextPowerup_f (CPlayerEntity *ent);
void Cmd_SelectPrevPowerup_f (CPlayerEntity *ent);
void Cmd_Give (CPlayerEntity *ent);
void Cmd_Give_f (CPlayerEntity *ent);
