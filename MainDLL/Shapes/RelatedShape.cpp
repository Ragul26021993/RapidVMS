#include "stdafx.h"
#include "RelatedShape.h"
#include "..\Engine\BaseItem.h"
#include "..\Engine\RCadApp.h"
#include <process.h>

RelatedShape::RelatedShape()
{
	DoNotSaveInPP = false;
}

RelatedShape::~RelatedShape()
{
	parents.clear();
	child.clear();
	Mchild.clear();
	FGNearShapes.clear();
}

void RelatedShape::EntityParamChanged(ITEMSTATUS i, BaseItem* sender)
{
	try
	{
		if(i == DATACHANGED)
		{
			this->UpdateForParentChange(sender);
			if(dynamic_cast<Shape*>(this))
				((Shape*)this)->notifyAll(ITEMSTATUS::DATACHANGED, ((Shape*)this));
			else
				((DimBase*)this)->notifyAll(ITEMSTATUS::DATACHANGED, ((DimBase*)this));
		}
	}
	//catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS0002", __FILE__, __FUNCSIG__); }
	catch (std::exception &ex)
	{
		std::string tempString = __FUNCSIG__;
		tempString.append(ex.what());
		MAINDllOBJECT->SetAndRaiseErrorMessage("RS0002", __FILE__, tempString);
	}
}

void RelatedShape::addParent(BaseItem* parent)
{
	try
	{
		bool Addrelation = true;
		for each(BaseItem* bi in parents)
		{
			if(bi->getId() == parent->getId())
			{
				Addrelation = false;
				break;
			}
		}
		if(Addrelation)
		{
			parents.push_back(parent);
			parent->addListener(this);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS0003", __FILE__, __FUNCSIG__); }
}

void RelatedShape::addGroupParent(BaseItem* groupParentShape)
{
	try
	{
		bool Addrelation = true;
		for each(BaseItem* bi in GroupParent)
		{
			if(bi->getId() == groupParentShape->getId())
			{
				Addrelation = false;
				break;
			}
		}
		if(Addrelation)
			GroupParent.push_back(groupParentShape);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS0003", __FILE__, __FUNCSIG__); }
}

void RelatedShape::addGroupChild(BaseItem* groupChildShape)
{
	try
	{
		bool Addrelation = true;
		for each(BaseItem* bi in GroupChilds)
		{
			if(bi->getId() == groupChildShape->getId())
			{
				Addrelation = false;
				break;
			}
		}
		if(Addrelation)
			GroupChilds.push_back(groupChildShape);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS0003", __FILE__, __FUNCSIG__); }
}

void RelatedShape::addChild(BaseItem* chil)
{
	try
	{
		bool Addrelation = true;
		for each(BaseItem* bi in child)
		{
			if(bi->getId() == chil->getId())
			{
				Addrelation = false;
				break;
			}
		}
		if(Addrelation)
			child.push_back(chil);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS0004", __FILE__, __FUNCSIG__); }
}

void RelatedShape::clearFgNearShapes()
{
	try
	{
		FGNearShapes.clear();
	}
	catch(...) { MAINDllOBJECT->SetAndRaiseErrorMessage("RS0006", __FILE__, __FUNCSIG__); }
}


void RelatedShape::addMChild(BaseItem* chill)
{
	try
	{
		bool Addrelation = true;
		for each(BaseItem* bi in child)
		{
			if(bi->getId() == chill->getId())
			{
				Addrelation = false;
				break;
			}
		}
		if(Addrelation)
			Mchild.push_back(chill);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS0005", __FILE__, __FUNCSIG__); }
}

void RelatedShape::addFgNearShapes(BaseItem* Fgsh)
{
	try
	{
		bool Addrelation = true;
		for each(BaseItem* bi in FGNearShapes)
		{
			if(bi->getId() == Fgsh->getId())
			{
				Addrelation = false;
				break;
			}
		}
		if(Addrelation)
			FGNearShapes.push_back(Fgsh);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS0006", __FILE__, __FUNCSIG__); }
}

void RelatedShape::adducsCopyRelation(BaseItem* parent)
{
	try
	{
		MCSparentShape.push_back(parent);
		//((RelatedShape*)parent)->UCSChildShape.push_back((BaseItem*)this);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS0006", __FILE__, __FUNCSIG__); }
}

void RelatedShape::RemoveParent(BaseItem* parent)
{
	try{ parents.remove(parent); parent->removeListener(this); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS0007", __FILE__, __FUNCSIG__); }
}

void RelatedShape::RemoveChild(BaseItem* chil)
{
	try{ child.remove(chil); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS0008", __FILE__, __FUNCSIG__); }
}

void RelatedShape::RemoveMChild(BaseItem* chil)
{
	try{ Mchild.remove(chil); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS0008a", __FILE__, __FUNCSIG__); }
}

void RelatedShape::RemoveGroupParent(BaseItem* Gparent)
{
	try{ GroupParent.remove(Gparent); Gparent->removeListener(this); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS0008b", __FILE__, __FUNCSIG__); }
}

void RelatedShape::RemoveGroupChild(BaseItem* Gchil)
		{
	try{ GroupChilds.remove(Gchil); Gchil->removeListener(this); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS0008c", __FILE__, __FUNCSIG__); }
}

list<BaseItem*>& RelatedShape::getParents()
{
	return parents;
}

list<BaseItem*>& RelatedShape::getChild()
{
	return child;
}

list<BaseItem*>& RelatedShape::getGroupParent()
{
	return GroupParent;
}

list<BaseItem*>& RelatedShape::getGroupChilds()
{
	return GroupChilds;
}

list<BaseItem*>& RelatedShape::getMchild()
{
	return Mchild;
}

list<BaseItem*>& RelatedShape::getFgNearShapes()
{
	return FGNearShapes;
}

list<BaseItem*>& RelatedShape::getucsCopyParent()
{
	return MCSparentShape;
}

list<BaseItem*>& RelatedShape::getucsCopyChild()
{
	return UCSChildShape;
}

BaseItem* RelatedShape::getMCSParentShape()
{
	if((int)MCSparentShape.size() > 0)
		return (*MCSparentShape.begin());
	else
		return NULL;
}

void RelatedShape::addSibling(BaseItem* sibling)
{
	try
	{
		bool Addrelation = true;
		for each(BaseItem* bi in siblings)
		{
			if(bi->getId() == sibling->getId())
			{
				Addrelation = false;
				break;
			}
		}
		if(Addrelation)
		{
			siblings.push_back(sibling);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS0009", __FILE__, __FUNCSIG__); }
}

void RelatedShape::RemoveSibling(BaseItem* sibling)
{
	try
	{
		bool RemoveRelation = false;
		for each(BaseItem* bi in siblings)
		{
			if (bi->getId() == sibling->getId())
			{
				RemoveRelation = true;
				break;
			}
		}
		if (RemoveRelation)
		{
			siblings.remove(sibling);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RS00010", __FILE__, __FUNCSIG__); }
}

list<BaseItem*>& RelatedShape::getSiblings()
{
	return siblings;
}

void RelatedShape::clearSiblings()
{
	try
	{
		siblings.clear();
	}
	catch(...) { MAINDllOBJECT->SetAndRaiseErrorMessage("RS00011", __FILE__, __FUNCSIG__); }
}