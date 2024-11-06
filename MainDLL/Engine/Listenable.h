//Listenable class....
//Used to handle the itemadded, changed, removed events..//
//Shape -> RcadAppp(update graphics), RapidShapeListener(update in front end), SnapPtManagaer(update intersection points)..
//Measurement - > RcadApp(Update graphics), RcaddimListener(update in front end)..
//UCS -> Rcaducslistener(update in front end)..

#pragma once
#include <list>
#include "CollectionListener.h"
#include "..\MainDLL.h"

using namespace std;
class MAINDLL_API Listenable
{
	list<CollectionListener*> listeners;
public:
	Listenable();
	~Listenable();
	//Add/Remove collection listeners.. i.e. RcadAppp/RapidShapeListener/RcaddimListener/Rcaducslistener/Dxflistener etc...
	void addListener(CollectionListener* l);
	void removeListener(CollectionListener* l);
	void notifySelection(); // notify selection changed event..
	void notifyAdd(BaseItem* item); // When item added.. call all listener with item and its collectionbase as sender
	void notifyRemove(BaseItem* item);	//When item removed..call all listener with item and its collectionbase as sender
	void notifyChange(BaseItem* item); //When item changed/modified..call all listener with item and its collectionbase as sender

	// collectionbase -- > shapecollection/measurementcollection/dxf coll/ucs coll etc..
};

//Created by Rajavanya.. Modified by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!