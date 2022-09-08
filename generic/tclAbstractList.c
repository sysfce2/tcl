/*
 * tclAbstractList.h --
 *
 *	The AbstractList Obj Type -- a psuedo List
 *
 * Copyright © 2022 by Brian Griffin. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution of
 * this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "tclAbstractList.h"


/* -------------------------- AbstractList object ---------------------------- */

/*
 * Prototypes for procedures defined later in this file:
 */

static void		DupAbstractListInternalRep (Tcl_Obj *srcPtr, Tcl_Obj *copyPtr);
static void		FreeAbstractListInternalRep (Tcl_Obj *listPtr);
static int		SetAbstractListFromAny (Tcl_Interp *interp, Tcl_Obj *objPtr);
static void		UpdateStringOfAbstractList (Tcl_Obj *listPtr);

/*
 * The structure below defines the AbstractList Tcl object type by means of
 * procedures that can be invoked by generic object code.
 *
 * The abstract list object is a special case of Tcl list represented by a set
 * of functions.
 *
 */

const Tcl_ObjType tclAbstractListType = {
    "abstractlist",			/* name */
    FreeAbstractListInternalRep,	/* freeIntRepProc */
    DupAbstractListInternalRep,		/* dupIntRepProc */
    UpdateStringOfAbstractList,		/* updateStringProc */
    SetAbstractListFromAny		/* setFromAnyProc */
};

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AbstractListLen --
 *
 * 	Compute the length of the equivalent list
 *
 * Results:
 *
 * 	The length of the list generated by the given range,
 * 	that may be zero.
 *
 * Side effects:
 *
 * 	None.
 *
 *----------------------------------------------------------------------
 */
Tcl_WideInt
Tcl_AbstractListObjLength(Tcl_Obj *abstractListObjPtr)
{
    return AbstractListObjLength(abstractListObjPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_NewAbstractListObj()
 *
 *	Creates a new AbstractList object. The returned object has
 *	refcount = 0.
 *
 * Results:
 *
 * 	A Tcl_Obj pointer to the created AbstractList object.
 * 	A NULL pointer of the range is invalid.
 *
 * Side Effects:
 *
 * 	None.
 *----------------------------------------------------------------------
 */

Tcl_Obj*
Tcl_NewAbstractListObj(Tcl_Interp *interp, const Tcl_AbstractListType* vTablePtr)
{
    Tcl_Obj *objPtr;
    Tcl_ObjInternalRep itr;
    (void)interp;
    TclNewObj(objPtr);
    itr.twoPtrValue.ptr1 = (void*)vTablePtr; /* dispatch table for concrete type */
    itr.twoPtrValue.ptr2 = NULL;
    Tcl_StoreInternalRep(objPtr, &tclAbstractListType, &itr);
    Tcl_InvalidateStringRep(objPtr);
    return objPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AbstractListObjIndex --
 *
 *	Returns the element with the specified index in the list
 *	represented by the specified Abstract List object.
 *	If the index is out of range, TCL_ERROR is returned,
 *	otherwise TCL_OK is returned and the integer value of the
 *	element is stored in *element.
 *
 * Results:
 *
 * 	Element Tcl_Obj is returned on succes, NULL on index out of range.
 *
 *----------------------------------------------------------------------
 */

Tcl_Obj*
Tcl_AbstractListObjIndex(Tcl_Obj *abstractListObjPtr, Tcl_WideInt index)
{
    Tcl_AbstractListType *typePtr;
    Tcl_Obj *elementObjPtr;

    typePtr = Tcl_AbstractListGetType(abstractListObjPtr);
    /*
     * The general assumption is that the obj is assumed first to be a List,
     * and only ends up here because it has been determinded to be an
     * AbstractList.  If that's not the case, then a mistake has been made. To
     * attempt to try a List call (e.g. shimmer) could potentially loop(?)
     * So: if called from List code, then something has gone wrong; if called
     * from user code, then user has made a mistake.
     */
    if (typePtr == NULL) {
	Tcl_Panic("Tcl_AbstractListObjIndex called without and AbstractList Obj.");
    }
    elementObjPtr = typePtr->indexProc(abstractListObjPtr, index);

    return elementObjPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * FreeAbstractListInternalRep --
 *
 *	Deallocate the storage associated with an abstract list object's
 *	internal representation.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Frees abstractListPtr's AbstractList* internal representation and
 *	sets listPtr's	internalRep.twoPtrValue.ptr1 to NULL.
 *
 *----------------------------------------------------------------------
 */

void
FreeAbstractListInternalRep(Tcl_Obj *abstractListObjPtr)
{
    Tcl_AbstractListType *typePtr = Tcl_AbstractListGetType(abstractListObjPtr);

    if (TclAbstractListHasProc(abstractListObjPtr, TCL_ABSL_FREEREP)) {
        /* call the free callback for the concrete rep */
        typePtr->freeRepProc(abstractListObjPtr);
    }
    abstractListObjPtr->internalRep.twoPtrValue.ptr1 = NULL;
    abstractListObjPtr->internalRep.twoPtrValue.ptr2 = NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * DupAbstractListInternalRep --
 *
 *	Initialize the internal representation of a AbstractList Tcl_Obj to a
 *	copy of the internal representation of an existing abstractlist object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	We set "copyPtr"s internal rep to a pointer to a
 *	newly allocated AbstractList structure.
 *----------------------------------------------------------------------
 */

static void
DupAbstractListInternalRep(
    Tcl_Obj *srcPtr,		/* Object with internal rep to copy. */
    Tcl_Obj *copyPtr)		/* Object with internal rep to set.
				 * Internal rep must be clear, it is stomped */
{
    Tcl_AbstractListType *typePtr;
    typePtr = AbstractListGetType(srcPtr);
    copyPtr->internalRep.twoPtrValue.ptr1 = typePtr;
    copyPtr->internalRep.twoPtrValue.ptr2 = NULL;

    /* Now do concrete type dup. It is responsible for calling
       Tcl_AbstractListSetConcreteRep to initialize ptr2 */

    if (typePtr->dupRepProc) {
	typePtr->dupRepProc(srcPtr, copyPtr);
    } else {
	/* TODO - or set it to NULL instead? */
	copyPtr->internalRep.twoPtrValue.ptr2 =
	    srcPtr->internalRep.twoPtrValue.ptr2;
    }

    copyPtr->typePtr = &tclAbstractListType;
}

/*
 *----------------------------------------------------------------------
 *
 * UpdateStringOfAbstractList --
 *
 *	Update the string representation for an abstractlist object.
 *	Note: This procedure does not invalidate an existing old string rep
 *	so storage will be lost if this has not already been done.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object's string is set to a valid string that results from the
 *	listlike-to-string conversion. This string will be empty if the
 *	AbstractList is empty.
 *
 * Notes:
 *      This simple approach is costly in that it forces a string rep for each
 *      element, which is then tossed.  Improving the performance here may
 *      require implementing a custom size-calculation function for each
 *      subtype of AbstractList.
 *
 *----------------------------------------------------------------------
 */

static void
UpdateStringOfAbstractList(Tcl_Obj *abstractListObjPtr)
{
    Tcl_AbstractListType *typePtr;
    char *p, *str;
    Tcl_Obj *eleObj;
    Tcl_WideInt length = 0;
    int llen, slen, i;

    typePtr = AbstractListGetType(abstractListObjPtr);

    /*
     * If concrete type has a better way to generate the string,
     * let it do it.
     */
    if (TclAbstractListHasProc(abstractListObjPtr, TCL_ABSL_TOSTRING)) {
	typePtr->toStringProc(abstractListObjPtr);
	return;
    }

    /*
     * Pass 1: estimate space.
     */
    llen = typePtr->lengthProc(abstractListObjPtr);
    if (llen <= 0) {
	Tcl_InitStringRep(abstractListObjPtr, NULL, 0);
	return;
    }
    for (i = 0; i < llen; i++) {
	eleObj = typePtr->indexProc(abstractListObjPtr, i);
	Tcl_GetStringFromObj(eleObj, &slen);
	length += slen + 1; /* one more for the space char */
	Tcl_DecrRefCount(eleObj);
    }

    /*
     * Pass 2: generate the string repr.
     */

    p = Tcl_InitStringRep(abstractListObjPtr, NULL, length);
    for (i = 0; i < llen; i++) {
	eleObj = typePtr->indexProc(abstractListObjPtr, i);
	str = Tcl_GetStringFromObj(eleObj, &slen);
	strcpy(p, str);
	p[slen] = ' ';
	p += slen+1;
	Tcl_DecrRefCount(eleObj);
    }
    if (length > 0) abstractListObjPtr->bytes[length-1] = '\0';
    abstractListObjPtr->length = length-1;
}

/*
 *----------------------------------------------------------------------
 *
 * SetAbstractListFromAny --
 *
 * 	The AbstractList object is just a way to optimize
 * 	Lists space complexity, so no one should try to convert
 * 	a string to an AbstractList object.
 *
 * 	This function is here just to populate the Type structure.
 *
 * Results:
 *
 * 	The result is always TCL_ERROR. But see Side Effects.
 *
 * Side effects:
 *
 * 	Tcl Panic if called.
 *
 *----------------------------------------------------------------------
 */

static int
SetAbstractListFromAny(
    Tcl_Interp *interp,		/* Used for error reporting if not NULL. */
    Tcl_Obj *objPtr)		/* The object to convert. */
{
    (void)interp;
    (void)objPtr;
    /* TODO - at some future point, should just shimmer to a traditional
     * Tcl list (but only when those are implemented under the AbstractList)
     * interface.
     */
    Tcl_Panic("SetAbstractListFromAny: should never be called");
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * TclAbstractListObjCopy --
 *
 *	Makes a "pure AbstractList" copy of an AbstractList value. This
 *	provides for the C level a counterpart of the [lrange $list 0 end]
 *	command, while using internals details to be as efficient as possible.
 *
 * Results:
 *
 *	Normally returns a pointer to a new Tcl_Obj, that contains the same
 *	abstractList value as *abstractListPtr does. The returned Tcl_Obj has a
 *	refCount of zero. If *abstractListPtr does not hold an AbstractList,
 *	NULL is returned, and if interp is non-NULL, an error message is
 *	recorded there.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Tcl_Obj *
TclAbstractListObjCopy(
    Tcl_Interp *interp,		 /* Used to report errors if not NULL. */
    Tcl_Obj *abstractListObjPtr) /* List object for which an element array is
				  * to be returned. */
{
    Tcl_Obj *copyPtr;

    if (!TclHasInternalRep(abstractListObjPtr, &tclAbstractListType)) {
	if (SetAbstractListFromAny(interp, abstractListObjPtr) != TCL_OK) {
	    /* We know this is going to panic, but it's the message we want */
	    return NULL;
	}
    }

    TclNewObj(copyPtr);
    TclInvalidateStringRep(copyPtr);
    DupAbstractListInternalRep(abstractListObjPtr, copyPtr);
    return copyPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AbstractListObjRange --
 *
 *	Makes a slice of an AbstractList value.
 *      *abstractListObjPtr must be known to be a valid AbstractList.
 *
 * Results:
 *	Returns a pointer to the sliced array.
 *      This may be a new object or the same object if not shared.
 *
 * Side effects:
 *
 *	?The possible conversion of the object referenced by
 *	abstractListObjPtr to a list object.?
 *
 *----------------------------------------------------------------------
 */

Tcl_Obj *
Tcl_AbstractListObjRange(
    Tcl_Obj *abstractListObjPtr, /* List object to take a range from. */
    Tcl_WideInt fromIdx,	 /* Index of first element to include. */
    Tcl_WideInt toIdx)		 /* Index of last element to include. */
{
    Tcl_AbstractListType *typePtr;
    if (!TclHasInternalRep(abstractListObjPtr, &tclAbstractListType)) {
	if (SetAbstractListFromAny(NULL, abstractListObjPtr) != TCL_OK) {
	    /* We know this is going to panic, but it's the message we want */
	    return NULL;
	}
    }
    typePtr = Tcl_AbstractListGetType(abstractListObjPtr);
    /*
     * sliceProc can be NULL, then revert to List.  Note: [lrange]
     * command also checks for NULL sliceProc, and won't call AbstractList
     */
    if (typePtr->sliceProc) {
	return typePtr->sliceProc(abstractListObjPtr, fromIdx, toIdx);
    } else {
	/* TODO ?shimmer avoided? */
	Tcl_Obj *newObj = TclListObjCopy(NULL, abstractListObjPtr);
	return newObj ? TclListObjRange(newObj, (ListSizeT)fromIdx, (ListSizeT)toIdx) : NULL;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AbstractListObjReverse --
 *
 *	Reverses the order of an AbstractList value.
 *      *abstractListObjPtr must be known to be a valid AbstractList.
 *
 * Results:
 *	Returns a pointer to the reversed array.
 *      This may be a new object or the same object if not shared.
 *
 * Side effects:
 *
 *	?The possible conversion of the object referenced by
 *	abstractListObjPtr to a list object.?
 *
 *----------------------------------------------------------------------
 */

Tcl_Obj *
Tcl_AbstractListObjReverse(
    Tcl_Obj *abstractListObjPtr) /* List object to take a range from. */
{
    Tcl_AbstractListType *typePtr;
    if (!TclHasInternalRep(abstractListObjPtr, &tclAbstractListType) ||
	!TclAbstractListHasProc(abstractListObjPtr, TCL_ABSL_REVERSE)) {
	if (SetAbstractListFromAny(NULL, abstractListObjPtr) != TCL_OK) {
	    /* We know this is going to panic, but it's the message we want */
	    return NULL;
	}
    }
    typePtr = Tcl_AbstractListGetType(abstractListObjPtr);
    return typePtr->reverseProc(abstractListObjPtr);
}


/*
 *----------------------------------------------------------------------
 *
 * Tcl_AbstractListObjGetElements --
 *
 *	This function returns an (objc,objv) array of the elements in a list
 *	object.
 *
 * Results:
 *	The return value is normally TCL_OK; in this case *objcPtr is set to
 *	the count of list elements and *objvPtr is set to a pointer to an
 *	array of (*objcPtr) pointers to each list element. If listPtr does not
 *	refer to an Abstract List object and the object can not be converted
 *	to one, TCL_ERROR is returned and an error message will be left in the
 *	interpreter's result if interp is not NULL.
 *
 *	The objects referenced by the returned array should be treated as
 *	readonly and their ref counts are _not_ incremented; the caller must
 *	do that if it holds on to a reference. Furthermore, the pointer and
 *	length returned by this function may change as soon as any function is
 *	called on the list object; be careful about retaining the pointer in a
 *	local data structure.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_AbstractListObjGetElements(
    Tcl_Interp *interp,		/* Used to report errors if not NULL. */
    Tcl_Obj *objPtr,		/* AbstractList object for which an element
				 * array is to be returned. */
    int *objcPtr,		/* Where to store the count of objects
				 * referenced by objv. */
    Tcl_Obj ***objvPtr)		/* Where to store the pointer to an array of
				 * pointers to the list's objects. */
{

    if (TclHasInternalRep(objPtr,&tclAbstractListType)) {
	Tcl_AbstractListType *typePtr  = Tcl_AbstractListGetType(objPtr);

        if (TclAbstractListHasProc(objPtr, TCL_ABSL_GETELEMENTS)) {
            int status = typePtr->getElementsProc(interp, objPtr, objcPtr, objvPtr);
            /* TODO -- Add error message here, or propagate interp down */
            return status;
        } else {
            if (interp) {
                Tcl_SetObjResult(
		    interp,
                    Tcl_NewStringObj("GetElements not supported!", -1));
		    Tcl_SetErrorCode(interp, "TCL", "MEMORY", NULL);
            }
        }
        return TCL_ERROR;
    } else {
	if (interp != NULL) {
	    Tcl_SetObjResult(
		interp,
		Tcl_ObjPrintf("value is not an abstract list"));
	    Tcl_SetErrorCode(interp, "TCL", "VALUE", "UNKNOWN", NULL);
	}
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * fill-column: 78
 * End:
 */
