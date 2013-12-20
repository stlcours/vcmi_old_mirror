#pragma once

#include "../../lib/VCMI_Lib.h"
#include "../../lib/CObjectHandler.h"
#include "../../lib/CBuildingHandler.h"
#include "../../lib/CCreatureHandler.h"
#include "../../lib/CTownHandler.h"
#include "AIUtility.h"

/*
 * Goals.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
struct HeroPtr;
class VCAI;
class FuzzyHelper;

namespace Goals
{
	struct AbstractGoal;
	class VisitTile;
	typedef std::shared_ptr<Goals::AbstractGoal> TSubgoal;
	typedef std::vector<TSubgoal> TGoalVec;

	enum EGoals
{
	INVALID = -1,
	WIN, DO_NOT_LOSE, CONQUER, BUILD, //build needs to get a real reasoning
	EXPLORE, GATHER_ARMY, BOOST_HERO,
	RECRUIT_HERO,
	BUILD_STRUCTURE, //if hero set, then in visited town
	COLLECT_RES,
	GATHER_TROOPS, // val of creatures with objid

	OBJECT_GOALS_BEGIN,
	GET_OBJ, //visit or defeat or collect the object
	FIND_OBJ, //find and visit any obj with objid + resid //TODO: consider universal subid for various types (aid, bid)
	VISIT_HERO, //heroes can move around - set goal abstract and track hero every turn

	GET_ART_TYPE,

	//BUILD_STRUCTURE,
	ISSUE_COMMAND,

	VISIT_TILE, //tile, in conjunction with hero elementar; assumes tile is reachable
	CLEAR_WAY_TO,
	DIG_AT_TILE //elementar with hero on tile
};

	//method chaining + clone pattern
#define VSETTER(type, field) virtual AbstractGoal & set ## field(const type &rhs) = 0;
#define OSETTER(type, field) CGoal<T> & set ## field(const type &rhs) override { field = rhs; return *this; };

#if 0
	#define SETTER
#endif // _DEBUG

enum {LOW_PR = -1};

class AbstractGoal
{
public:
	bool isElementar; VSETTER(bool, isElementar)
	bool isAbstract; VSETTER(bool, isAbstract)
	int priority; VSETTER(bool, priority)
	int value; VSETTER(int, value)
	int resID; VSETTER(int, resID)
	int objid; VSETTER(int, objid)
	int aid; VSETTER(int, aid)
	int3 tile; VSETTER(int3, tile)
	HeroPtr hero; VSETTER(HeroPtr, hero)
	const CGTownInstance *town; VSETTER(CGTownInstance *, town)
	int bid; VSETTER(int, bid)

	AbstractGoal (EGoals goal = INVALID) : goalType (goal)
	{
		priority = 0;
		isElementar = false;
		isAbstract = false;
		value = 0;
		aid = -1;
		resID = -1;
		tile = int3(-1, -1, -1);
		town = nullptr;
	}
	virtual ~AbstractGoal(){};
	virtual AbstractGoal * clone() const = 0;

	EGoals goalType;

	std::string name() const;
	virtual std::string completeMessage() const {return "This goal is unspecified!";};

	bool invalid() const;

	static TSubgoal goVisitOrLookFor(const CGObjectInstance *obj); //if obj is nullptr, then we'll explore
	static TSubgoal lookForArtSmart(int aid); //checks non-standard ways of obtaining art (merchants, quests, etc.)
	static TSubgoal tryRecruitHero();

	virtual TGoalVec getAllPossibleSubgoals() = 0;
	virtual TSubgoal whatToDoToAchieve() = 0;
	virtual float importanceWhenLocked() const {return -1e10;}; //how much would it cost to interrupt the goal
	//probably could use some sophisticated fuzzy evalluation for it as well

	///Visitor pattern
	//TODO: make accept work for shared_ptr... somehow
	virtual void accept (VCAI * ai); //unhandled goal will report standard error
	virtual float accept (FuzzyHelper * f);

	virtual bool operator== (AbstractGoal &g)
	{
		return false;
	}
	virtual bool fulfillsMe (Goals::TSubgoal goal)
	{
		return false;
	}
	virtual bool fulfillsMe (shared_ptr<VisitTile> goal)
	{
		return false;
	}


	//template <typename Handler> void serialize(Handler &h, const int version)
	//{
	//	h & goalType & isElementar & isAbstract & priority;
	//	h & value & resID & objid & aid & tile & hero & town & bid;
	//}
};

template <typename T> class CGoal : public AbstractGoal
{
public:
	CGoal<T> (EGoals goal = INVALID) : AbstractGoal (goal)
	{
		priority = 0;
		isElementar = false;
		isAbstract = false;
		value = 0;
		aid = -1;
		resID = -1;
		tile = int3(-1, -1, -1);
		town = nullptr;
	}
	//virtual TSubgoal whatToDoToAchieve() override; //can't have virtual and template class at once

	OSETTER(bool, isElementar)
	OSETTER(bool, isAbstract)
	OSETTER(bool, priority)
	OSETTER(int, value)
	OSETTER(int, resID)
	OSETTER(int, objid)
	OSETTER(int, aid)
	OSETTER(int3, tile)
	OSETTER(HeroPtr, hero)
	OSETTER(CGTownInstance *, town)
	OSETTER(int, bid)

	void accept (VCAI * ai) override;
	float accept (FuzzyHelper * f) override;

	CGoal<T> * clone() const override
	{
		return new T(static_cast<T const&>(*this)); //casting enforces template instantiation
	}
	TSubgoal iAmElementar()
	{
		setisElementar(true);
		shared_ptr<AbstractGoal> ptr;
		ptr.reset(clone());
		return ptr;
	}
	template <typename Handler> void serialize(Handler &h, const int version)
	{
		h & goalType & isElementar & isAbstract & priority;
		h & value & resID & objid & aid & tile & hero & town & bid;
	}
};

TSubgoal sptr(const AbstractGoal & tmp);

class Invalid : public CGoal<Invalid>
{
	public:
	Invalid() : CGoal (Goals::INVALID){};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	//float importanceWhenLocked() const override;
};
class Win : public CGoal<Win>
{
	public:
	Win() : CGoal (Goals::WIN){};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	//float importanceWhenLocked() const override; //can't be locked, doesn't make much sense anyway
};
class NotLose : public CGoal<NotLose>
{
	public:
	NotLose() : CGoal (Goals::DO_NOT_LOSE){};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	float importanceWhenLocked() const override;
};
class Conquer : public CGoal<Conquer>
{
	public:
	Conquer() : CGoal (Goals::CONQUER){};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	float importanceWhenLocked() const override;
};
class Build : public CGoal<Build>
{
	public:
	Build() : CGoal (Goals::BUILD){};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	float importanceWhenLocked() const override;
};
class Explore : public CGoal<Explore>
{
	public:
	Explore() : CGoal (Goals::EXPLORE){};
	Explore(HeroPtr h) : CGoal (Goals::EXPLORE){hero = h;};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	std::string completeMessage() const override;
	float importanceWhenLocked() const override;
};
class GatherArmy : public CGoal<GatherArmy>
{
private:
	GatherArmy() : CGoal (Goals::GATHER_ARMY){};
public:
	GatherArmy(int val) : CGoal (Goals::GATHER_ARMY){value = val;};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	std::string completeMessage() const override;
	float importanceWhenLocked() const override;
};
class BoostHero : public CGoal<BoostHero>
{
	public:
	BoostHero() : CGoal (Goals::INVALID){}; //TODO
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	float importanceWhenLocked() const override;
};
class RecruitHero : public CGoal<RecruitHero>
{
	public:
	RecruitHero() : CGoal (Goals::RECRUIT_HERO){};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	//float importanceWhenLocked() const override;
};
class BuildThis : public CGoal<BuildThis>
{
private:
	BuildThis() : CGoal (Goals::BUILD_STRUCTURE){};
public:
	BuildThis(BuildingID Bid, const CGTownInstance *tid) : CGoal (Goals::BUILD_STRUCTURE) {bid = Bid; town = tid;};
	BuildThis(BuildingID Bid) : CGoal (Goals::BUILD_STRUCTURE) {bid = Bid;};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	float importanceWhenLocked() const override;
};
class CollectRes : public CGoal<CollectRes>
{
private:	
	CollectRes() : CGoal (Goals::COLLECT_RES){};
public:
	CollectRes(int rid, int val) : CGoal (Goals::COLLECT_RES) {resID = rid; value = val;};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	float importanceWhenLocked() const override;
};
class GatherTroops : public CGoal<GatherTroops>
{
private:
	GatherTroops() : CGoal (Goals::GATHER_TROOPS){};
public:
	GatherTroops(int type, int val) : CGoal (Goals::GATHER_TROOPS){objid = type; value = val;};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	float importanceWhenLocked() const override;
};
class GetObj : public CGoal<GetObj>
{
private:
	GetObj() {}; // empty constructor not allowed
public:
	GetObj(int Objid) : CGoal(Goals::GET_OBJ) {objid = Objid;};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	bool operator== (GetObj &g) {return g.objid ==  objid;}
	bool fulfillsMe (shared_ptr<VisitTile> goal) override;
	std::string completeMessage() const override;
	float importanceWhenLocked() const override;
};
class FindObj : public CGoal<FindObj>
{
private:
	FindObj() {}; // empty constructor not allowed
public:
	FindObj(int ID) : CGoal(Goals::FIND_OBJ) {objid = ID;};
	FindObj(int ID, int subID) : CGoal(Goals::FIND_OBJ) {objid = ID; resID = subID;};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	float importanceWhenLocked() const override;
};
class VisitHero : public CGoal<VisitHero>
{
private:
	VisitHero() : CGoal (Goals::VISIT_HERO){};
public:
	VisitHero(int hid) : CGoal (Goals::VISIT_HERO){objid = hid;};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	bool operator== (VisitHero &g) {return g.objid == objid;}
	bool fulfillsMe (shared_ptr<VisitTile> goal) override;
	std::string completeMessage() const override;
	float importanceWhenLocked() const override;
};
class GetArtOfType : public CGoal<GetArtOfType>
{
private:
	GetArtOfType() : CGoal (Goals::GET_ART_TYPE){};
public:
	GetArtOfType(int type) : CGoal (Goals::GET_ART_TYPE){aid = type;};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	float importanceWhenLocked() const override;
};
class VisitTile : public CGoal<VisitTile>
	//tile, in conjunction with hero elementar; assumes tile is reachable
{
private:
	VisitTile() {}; // empty constructor not allowed
public:
	VisitTile(int3 Tile) : CGoal (Goals::VISIT_TILE) {tile = Tile;};
	TGoalVec getAllPossibleSubgoals() override;
	TSubgoal whatToDoToAchieve() override;
	bool operator== (VisitTile &g) {return g.tile == tile;}
	std::string completeMessage() const override;
	float importanceWhenLocked() const override;
}; 
class ClearWayTo : public CGoal<ClearWayTo>
{
public:
	ClearWayTo(int3 Tile) : CGoal (Goals::CLEAR_WAY_TO) {tile = Tile;};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	bool operator== (ClearWayTo &g) {return g.tile == tile;}
	float importanceWhenLocked() const override;
};
class DigAtTile : public CGoal<DigAtTile>
	//elementar with hero on tile
{
private:
	DigAtTile() : CGoal (Goals::DIG_AT_TILE){};
public:
	DigAtTile(int3 Tile) : CGoal (Goals::DIG_AT_TILE) {tile = Tile;};
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	bool operator== (DigAtTile &g) {return g.tile == tile;}
	float importanceWhenLocked() const override;
};

class CIssueCommand : public CGoal<CIssueCommand>
{
	std::function<bool()> command;

	public:
	CIssueCommand(std::function<bool()> _command): CGoal(ISSUE_COMMAND), command(_command) {}
	TGoalVec getAllPossibleSubgoals() override {return TGoalVec();};
	TSubgoal whatToDoToAchieve() override;
	//float importanceWhenLocked() const override; //unsupported yet, but shoudl be highest
};

}
