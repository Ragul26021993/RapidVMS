//Class used to handle the derived update for parent change...
//Whenever the a Shape changes.. Modified / Selection Changed it calles ItemChanged..
#pragma once
#include <vector>
#include <list>
#include "..\MainDLL.h"
#include "..\Engine\ItemListener.h"

using namespace std;

class BaseItem;

class MAINDLL_API RelatedShape:
	public ItemListener
{
private:
	list<BaseItem*> parents, child, Mchild, GroupParent, GroupChilds;
	list<BaseItem*> FGNearShapes;
	list<BaseItem*> MCSparentShape, UCSChildShape;
	list<BaseItem*> siblings;
public:
	RelatedShape();
	virtual ~RelatedShape();

	//Add the parent and Child Shapes..relationship..//
	void addParent(BaseItem* parent);
	void addChild(BaseItem* chil);
	void clearFgNearShapes();
	void addGroupParent(BaseItem*  groupShape);
	void addGroupChild(BaseItem*  groupShape);
	void addMChild(BaseItem* chil);
	void addFgNearShapes(BaseItem* Fgsh);
	void adducsCopyRelation(BaseItem* parent);

	void RemoveParent(BaseItem* parent);
	void RemoveChild(BaseItem* chil);
	void RemoveMChild(BaseItem* chil);
	void RemoveGroupParent(BaseItem* Gparent);
	void RemoveGroupChild(BaseItem* Gchil);
	//Item changed event...//
	virtual void EntityParamChanged(ITEMSTATUS i, BaseItem* sender);

	virtual void UpdateForParentChange(BaseItem* sender) = 0;
		
	//Returns the parent and child shapes..//
	list<BaseItem*>& getParents();
	list<BaseItem*>& getChild();
	list<BaseItem*>& getGroupParent();
	list<BaseItem*>& getGroupChilds();
	list<BaseItem*>& getMchild();
	list<BaseItem*>& getFgNearShapes();
	list<BaseItem*>& getucsCopyParent();
	list<BaseItem*>& getucsCopyChild();

	//sibling relationship - will be useful for shapes that are in equal footing and add points on one of them needs to update parameters of all of them.
	void addSibling(BaseItem* sibling);
	void RemoveSibling(BaseItem* sibling);
	list<BaseItem*>& getSiblings();
	void clearSiblings();

	BaseItem* getMCSParentShape();

	//this property is used for restricting the shapes and measurement to write in part program...
	bool DoNotSaveInPP;
};