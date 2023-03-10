/*	$NetBSD: GXditview-ad.h,v 1.1.1.1 2001/04/19 12:52:30 wiz Exp $	*/

"GXditview.height: 840",
"GXditview.paned.allowResize:		true",
"GXditview.paned.viewport.allowVert:	true",
"GXditview.paned.viewport.allowHoriz:	true",
"GXditview.paned.viewport.skipAdjust:	false",
"GXditview.paned.viewport.width:		600",
"GXditview.paned.viewport.height:	800",
"GXditview.paned.viewport.showGrip:	false",
"GXditview.paned.label.skipAdjust:	true",
"GXditview.paned.viewport.dvi.translations: #augment \
	<Btn1Down>:	XawPositionSimpleMenu(menu) MenuPopup(menu)\\n\
	<Key>Next:	NextPage()\\n\
	<Key>n:		NextPage()\\n\
	<Key>space:	NextPage()\\n\
	<Key>Return:	NextPage()\\n\
	<Key>Prior:	PreviousPage()\\n\
	<Key>p:		PreviousPage()\\n\
	<Key>BackSpace:	PreviousPage()\\n\
	<Key>Delete:	PreviousPage()\\n\
	<Key>Select:	SelectPage()\\n\
	<Key>Find:	OpenFile()\\n\
	<Key>r:		Rerasterize()\\n\
	<Key>q:		Quit()",
"GXditview.paned.label.translations: #augment \
	<Btn1Down>:	XawPositionSimpleMenu(menu) MenuPopup(menu)\\n\
	<Key>Next:	NextPage()\\n\
	<Key>n:		NextPage()\\n\
	<Key>space:	NextPage()\\n\
	<Key>Return:	NextPage()\\n\
	<Key>Prior:	PreviousPage()\\n\
	<Key>p:		PreviousPage()\\n\
	<Key>BackSpace:	PreviousPage()\\n\
	<Key>Delete:	PreviousPage()\\n\
	<Key>Select:	SelectPage()\\n\
	<Key>Find:	OpenFile()\\n\
	<Key>r:		Rerasterize()\\n\
	<Key>q:		Quit()",
"GXditview.menu.nextPage.label:		Next Page",
"GXditview.menu.previousPage.label:	Previous Page",
"GXditview.menu.selectPage.label:	Select Page",
"GXditview.menu.print.label:		Print",
"GXditview.menu.openFile.label:		Open",
"GXditview.menu.quit.label:		Quit",
"GXditview.promptShell.allowShellResize:	true",
"GXditview.promptShell.promptDialog.value.translations: #override \
	<Key>Return:	Accept()",
"GXditview.promptShell.promptDialog.accept.label: Accept",
"GXditview.promptShell.promptDialog.accept.translations: #override \
	<BtnUp>:	Accept() unset()",
"GXditview.promptShell.promptDialog.cancel.label: Cancel",
"GXditview.promptShell.promptDialog.cancel.translations: #override \
	<BtnUp>:	Cancel() unset()",
