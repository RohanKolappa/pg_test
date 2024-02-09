// ============================================================================
//
// Copyright (c) 2003-2014 Barco N.V.
//
// ============================================================================
//
// This file is part of "JBS".
//
// "JBS" is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
//
// "JBS" is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with "JBS"; if not, write to:
//                          Free Software Foundation
//                          59 Temple Place, Suite 330
//                          Boston, MA 02111-1307 USA
//
// ============================================================================
//
// Filename:
//	JBS.CppProject.cpp
//
// Description:
//	...
//
// Author(s):
//	...
//
// ============================================================================

#include "BFC.VFS.FSDir.h"
#include "BFC.VFS.FSGlobber.h"

#include "JBS.Atoms.BscAtom.h"
#include "JBS.Atoms.CppAtom.h"
#include "JBS.Atoms.DepAtom.h"
#include "JBS.Atoms.DirAtom.h"
#include "JBS.Atoms.ExeAtom.h"
#include "JBS.Atoms.HdrAtom.h"
#include "JBS.Atoms.LibAtom.h"
#include "JBS.Atoms.ListAtom.h"
#include "JBS.Atoms.ObjAtom.h"
#include "JBS.Atoms.SbrAtom.h"
#include "JBS.Atoms.UiAtom.h"
#include "JBS.Atoms.VuiAtom.h"

#include "JBS.Context.h"
#include "JBS.CppProject.h"
#include "JBS.Environment.h"
#include "JBS.ListUtils.h"

// ============================================================================

using namespace BFC;
using namespace JBS;
using namespace JBS::Atoms;

// ============================================================================

BFC_PTR_IMPL( CppProject )

// ============================================================================

CppProject::CppProject() {

	setObjectName( "JBS.CppProject" );
	setDebug( true );

	projectType	= NoType;

}

CppProject::~CppProject() {



}

// ============================================================================

void CppProject::buildFromDefs(
		UserDefsMapPtr		defs ) {

	// Interpret all definitions...

	for ( Uint32 i = 0 ; i < defs->size() ; i++ ) {

		UserDefinitionPtr def = defs->getDef( i );
		Buffer defName = def->getName();

		if ( defName == "target-type" ) {
			if ( ! def->isSingleton() ) {
				throw InternalError( "Invalid Project type!" );
			}
			if ( projectType != NoType ) {
				throw InternalError( "Duplicate Project type!" );
			}
			Buffer type = def->getSingleton();
			if ( type == "exe" ) {
				projectType = Executable;
			}
			else if ( type == "lib" ) {
				projectType = Library;
				libraryType = Static;
			}
			else if ( type == "dll" ) {
				projectType = Library;
				libraryType = Shared;
			}
			else if ( type == "plugin" ) {
				projectType = Library;
				libraryType = Plugin;
			}
		}
		else if ( defName == "target-name" ) {
			if ( ! def->isSingleton() ) {
				throw InternalError( "Invalid Project name!" );
			}
			if ( ! getProjectName().isEmpty() ) {
				throw InternalError( "Duplicate Project name!" );
			}
			setProjectName( def->getSingleton() );
			setObjectName(
				"JBS.CppProject[ "
				+ getProjectName()
				+ " ]" );
//			msgDbg( "Name: " + getProjectName() );
		}
		else if ( defName == "target-debug" ) {
//			if ( ! def->isSingleton() ) {
//				throw InternalError( "Invalid debug flag!" );
//			}
//			if ( buildConfig.dbgMode != DontKnow ) {
//				Message( Message::MsgWarning, "Duplicate debug flag!" );
//			}
//			buildConfig.dbgMode =
//				( def->getSingleton() == "yes" ? Yes : No );
		}
		else if ( defName == "target-static" ) {
			if ( ! def->isSingleton() ) {
				throw InternalError( "Invalid static flag!" );
			}
			if ( buildConfig.staMode != DontKnow ) {
				Message( Message::MsgWarning, "Duplicate static flag!" );
			}
			buildConfig.staMode =
				( def->getSingleton() == "yes" ? Yes : No );
		}
		else if ( defName == "target-ver-base" ) {
			versionBase = def->getSingleton();
		}
		else if ( defName == "target-ver-maj" ) {
			versionLocal.setMaj( def->getSingleton().toUint32() );
		}
		else if ( defName == "target-ver-med" ) {
			versionLocal.setMed( def->getSingleton().toUint32() );
		}
		else if ( defName == "target-ver-min" ) {
			versionLocal.setMin( def->getSingleton().toUint32() );
		}
		else if ( defName == "target-sources" ) {
			cppSources = def->getValues();
		}
		else if ( defName == "target-cxxdefs" ) {
			cxxDefs = def->getValues();
		}
		else if ( defName == "target-cxxflags" ) {
			cxxFlags = def->getValues();
		}
		else if ( defName == "target-deps" ) {
			targetDeps = def->getValues();
		}
		else if ( defName == "target-mocs" ) {
			mocSources = def->getValues();
		}
		else if ( defName == "target-guis" ) {
			guiSources = def->getValues();
		}
		else if ( defName == "target-vuis" ) {
			vuiSources = def->getValues();
		}
		else if ( defName == "target-quis" ) {
			quiSources = def->getValues();
		}
		else if ( defName == "target-incldirs" ) {
			for ( Uint32 i = 0 ; i < def->getValues().size() ; i++ ) {
				Buffer b = def->getValues()[ i ];
				b = context->environment->expandLine( b );
				addInclDirs += b;
			}
		}
		else if ( defName == "target-libs" ) {
			addLibNames = def->getValues();
		}
		else if ( defName == "target-libdirs" ) {
			for ( Uint32 i = 0 ; i < def->getValues().size() ; i++ ) {
				Buffer b = def->getValues()[ i ];
				b = context->environment->expandLine( b );
				addLibDirs += b;
			}
		}
		else if ( defName == "target-docdir" ) {
			docDir = def->getSingleton();
		}
		else if ( defName == "target-doxydir" ) {
			doxyDir = def->getSingleton();
		}
		else if ( defName == "target-doxyimg" ) {
			doxyImg = def->getSingleton();
		}
		else if ( defName == "target-stripped" ) {
			// No-op...
		}
		else {
			throw InternalError( "Don't know what to do with variable \""
				+ defName
				+ "\"!" );
		}

	}

	if ( projectType == NoType ) {
		throw InternalError( "No Project type!" );
	}

	if ( getProjectName().isEmpty() ) {
		throw InternalError( "No Project name!" );
	}

}

// ============================================================================

void CppProject::fixDefs() {

	// Fix the version, if needed...

	if ( ! versionBase.isEmpty() ) {
		CppProjectPtr base = context->database->getProjectByName( versionBase );
		if ( ! base->versionBase.isEmpty() ) {
			throw InternalError( "Multiple indirection to compute version!" );
		}
		versionLocal = base->versionLocal;
//		msgDbg( "Fixed version: " + versionLocal.toBuffer() );
	}

	// Fix the list of dependencies, if needed...

	getDepsList( 0 );

}

// ============================================================================

const CppProjectList& CppProject::getDepsList(
	const	Uint32		level ) {

	if ( level >= context->maxDepLevel ) {
		Message( Message::MsgError, "Infinite project dependencies recursion detected!" );
		Message( Message::MsgError, "Go and fix your inclusion lists first!" );
		return depsList;
	}

	if ( ! depsList.isEmpty() || targetDeps.isEmpty() ) {
		return depsList;
	}

	// Fix the list of dependencies, if needed...

	for ( Uint32 i = 0 ; i < targetDeps.size() ; i++ ) {

		Buffer depname = targetDeps[ i ];

		ProjectPtr dep;

		try {
			dep = context->database->getProjectByName( depname );
		}
		catch ( Exception& ) {
			Message( Message::MsgWarning,
				"No such project: \""
				+ depname + "\"!" );
			continue;
		}

		CppProjectPtr cdep;

		try {
			cdep = dep;
		}
		catch ( BadCast& ) {
			Message( Message::MsgWarning,
				"Dependency \""
				+ depname
				+ "\" not a C/C++ project!" );
			continue;
		}

		// Now, make sure this dependency has an ordered list of
		// dependencies... and insert those into our own list...
		// Then insert our dependency itself... This way, our list
		// is ordered correctly for the linking!

		depsList.insertLast( cdep->getDepsList( level + 1 ) );
		depsList.insertLast( cdep );

	}

//	// Dump the list...
//
//	Buffer msg;
//
//	for ( CppProjectList::Iterator it = depsList.firstElt() ; it ; it++ ) {
//		msg += (*it)->getProjectName() + " ";
//	}
//
//	msgDbg( "BEFORE: " + msg );

	// Now, suppress redundant elements in our list!

	for ( CppProjectList::Iterator it = depsList.firstElt() ; it ; it++ ) {
		CppProjectList::Iterator it2 = it;
		it2++;
		while ( it2 ) {
			if ( *it == *it2 ) {
				depsList.pop( it2 );
			}
			else {
				it2++;
			}
		}
	}

//	msg.kill();
//
//	for ( CppProjectList::Iterator it = depsList.firstElt() ; it ; it++ ) {
//		msg += (*it)->getProjectName() + " ";
//	}
//
//	msgDbg( "AFTER : " + msg );

	return depsList;

}

// ============================================================================

Bool CppProject::canBuildConfig(
	const	BuildConfig&		cfg ) const {

	return tgtRoots.contains( cfg );

}

AtomPtr CppProject::getTargetRoot(
	const	BuildConfig&		cfg ) const {

	return tgtRoots[ cfg ];

}

// ============================================================================

void CppProject::doBuild() {

	doBuild(
		BuildConfig(
			( context->forceDbg || buildConfig.dbgMode == Yes ),
			( context->forceSta || buildConfig.staMode == Yes ) ) );

}

void CppProject::doBuild(
	const	BuildConfig&	doCfg ) {

	createTree();

	// Find the corresponding Atom. If it doesn't exist, it means the
	// requested config is not supported!

	if ( ! tgtRoots.contains( doCfg ) ) {
		Message( Message::MsgWarning, "Building mode not supported!" );
		return;
	}

	AtomPtr tgtRoot = tgtRoots[ doCfg ];

	if ( ! tgtRoot ) {
		throw InternalError( "NULL entry in tgtroot map!" );
	}

	// Bring all project dependencies now. This is redundant (it should
	// have been done automatically below), but is kept to get better
	// output ;-)

	AtomList tmpList;

	if ( projectType == Library ) {
		LibAtomPtr libAtom = tgtRoot;
		tmpList = libAtom->getDepList();
	}
	else {
		ExeAtomPtr exeAtom = tgtRoot;
		tmpList = exeAtom->getDepList();
	}

	for ( AtomList::Iterator prit = tmpList.firstElt() ; prit ; prit++ ) {
		LibAtomPtr subLib = *prit;
		BuildConfig subCfg( subLib->getDbgMode() , subLib->getStaMode() );
		CppProjectPtr subProj = subLib->getProject();
		subProj->doBuild(
			BuildConfig(
				subLib->getDbgMode(),
				subLib->getStaMode() )
		);
	}

	// Time to display a nice banner ;-)

	if ( tgtRoot->isUpToDate() == Yes ) {
		return;
	}

	Message( Message::Building,
		"\""
		+ getProjectName()
		+ "\" (in "
		+ getProjectPath().toBuffer()
		+ ")" );

	// Bring the target up to date now! This will rebuild all project
	// dependencies as needed (already done above)!

//	tempDir->doUpdate();

	try {
		tgtRoot->doUpdate();
	}
	catch ( Exception& e ) {
		Message( Message::MsgError, "Failed to build project:" );
		Message( Message::MsgError, "... name: " + getProjectName() );
		Message( Message::MsgError, "... path: " + getProjectPath().toBuffer() );
		Message( Message::MsgDebug, "Exception was:" );
		Message( Message::MsgDebug, e.what() );
		if ( ! context->contOnErr ) {
			throw Failed();
		}
	}

}

void CppProject::doClean(
	const	Bool		distclean ) {

	Message( Message::Cleaning,
		"\""
		+ getProjectName()
		+ "\" (in "
		+ getProjectPath().toBuffer()
		+ ")" );

	createTree();

	for ( Uint32 i = 0 ; i < tgtRoots.size() ; i++ ) {
		AtomPtr tgtRoot = tgtRoots.value( i );
		tgtRoot->doClean( distclean );
	}

	tempDir->doClean( distclean );

}

void CppProject::doGenPacks() {

	Message( Message::GenPacks,
		"\""
		+ getProjectName()
		+ "\" (in "
		+ getProjectPath().toBuffer()
		+ ")" );

	createTree();
#if 0
	AtomPtr tgtRoot = tgtRoots[ doCfg ];

	if ( ! tgtRoot ) {
		throw InternalError( "NULL entry in tgtroot map!" );
	}

	// Bring the target up to date now! This will rebuild all project
	// dependencies as needed!

//	tempDir->doUpdate();

	try {
		tgtRoot->doUpdate();
	}
	catch ( Exception& e ) {
		Message( Message::MsgError, "Failed to build project:" );
		Message( Message::MsgError, "... name: " + getProjectName() );
		Message( Message::MsgError, "... path: " + getProjectPath().toBuffer() );
		Message( Message::MsgDebug, "Exception was:" );
		Message( Message::MsgDebug, e.what() );
		if ( ! context->contOnErr ) {
			throw Failed();
		}
	}
#endif
}

void CppProject::doGenDoc() {

	// No-op!

}

// ============================================================================

void CppProject::createTree() {

	if ( tempDir ) {
		return;
	}

	Message( Message::Entering, getProjectPath().toBuffer() );

	// Determine the supported configurations... The order can slightly
	// improve the performances, so we start with the shared/release
	// config, then the shared/debug, and so on...

	Map< BuildConfig, AtomList >	configs;

	configs.add( BuildConfig( false, false ) );
	configs.add( BuildConfig( true , false ) );
	configs.add( BuildConfig( false, true  ) );
	configs.add( BuildConfig( true , true  ) );

	// Remove configs that the user has refused explicitly in Make.target!

	Uint32 i;

	for ( i = 0 ; i < configs.size() ; ) {
		if ( configs.key( i ).isCompatibleWith( buildConfig ) ) {
			i++;
		}
		else {
			configs.pop( i );
		}
	}

	// Now, the inter-project dependencies!

	CppProjectList::Iterator cit;

	for ( cit = depsList.firstElt() ; cit ; cit++ ) {

		// If there are no more configs left, abort!

		if ( configs.isEmpty() ) {
			Message( Message::MsgWarning,
				"No config to build this project!" );
			return;
		}

		// First, rebuild the dependency internal tree, if needed...

		(*cit)->createTree();

		// Now, make sure all our configs are compatible with this
		// dependency...

		for ( Uint32 i = 0 ; i < configs.size() ; ) {

			BuildConfig cfg = configs.key( i );

			if ( (*cit)->canBuildConfig( cfg ) ) {
				configs.getValue( i ).insertLast(
					(*cit)->getTargetRoot( cfg ) );
				i++;
				continue;
			}

			Message( Message::MsgWarning, "Dependency \""
				+ (*cit)->getProjectName()
				+ "\" can't be built in mode \""
				+ cfg.toBuffer()
				+ "\"!" );

			if ( cfg.staMode == No ) {
				cfg.staMode = Yes;
				if ( (*cit)->canBuildConfig( cfg ) ) {
					Message( Message::MsgWarning,
						"... but can't be built in compatible mode \""
						+ cfg.toBuffer()
						+ "\"!" );
					configs.getValue( i ).insertLast(
						(*cit)->getTargetRoot( cfg ) );
					i++;
					continue;
				}
			}

			configs.pop( i );

		}

	}

	// Let's start by converting all wildcards to relative paths... We
	// will get lists of all possible files (temporary or precious) that
	// we will have to handle later.

	Message( Message::Analyzing, getProjectPath().toBuffer() );

	FSPathList allCppPaths;
	FSPathList allHdrPaths;
	FSPathList allMocPaths;
	FSPathList allGuiPaths;
	FSPathList allVuiPaths;
	FSPathList allQuiPaths;

	FSDir::setCurrentDir( getProjectPath() );

	unglob( cppSources, allCppPaths ); allHdrPaths = allCppPaths;
	unglob( mocSources, allMocPaths );
	unglob( guiSources, allGuiPaths );
	unglob( vuiSources, allVuiPaths );
	unglob( quiSources, allQuiPaths );

	ListUtils::keepExtension( CppAtom::fileExt, allCppPaths );
	ListUtils::keepExtension( HdrAtom::fileExt, allHdrPaths );
	ListUtils::keepExtension( HdrAtom::fileExt, allMocPaths );
	ListUtils::keepExtension( UiAtom ::fileExt, allGuiPaths );
	ListUtils::keepExtension( VuiAtom::fileExt, allVuiPaths );
	ListUtils::keepExtension( UiAtom ::fileExt, allQuiPaths );

	// Now that we have our lists of files, let's start to populate our
	// tree.

	// All Atom objects keep a pointer to their parent Project. Let's
	// give them a Slave pointer instead of a Master, which would prevent
	// destruction because of loops!

	ProjectPtr proj = this;

	proj.setMaster( false );

	// All temp files we will build go to a temp folder now :-)
	// Listen and repeat : "JBS 5 is great and we love it."
	// ... and send an email to the author to let him know he has a new
	// user who reads source code comments ;-)

	tempDir = new DirAtom( proj );

	// All DepAtoms need the temporary headers to be built first, before
	// trying to update themselves! Therefor, we create a list of temporary
	// headers, that we use as dependency for each DepAtom!

	ListAtomPtr tmpHdrAtoms = new ListAtom;

	// We will progressively populate lists of specific Atoms...

#if defined( PLATFORM_WIN32 )
	AtomList	objDbgStaList;
	AtomList	objDbgDynList;
	AtomList	objRelStaList;
	AtomList	objRelDynList;
#else
	AtomList	objDbgList;
	AtomList	objRelList;
#endif
	AtomList	depList;
	AtomList	cppList;
	/* AtomList	hdrList */;
	AtomList	vuiList;
	AtomList	uiList;

	// Some *.ui files are used to generate mocifiable .h files and .cpp
	// files...

	FSPathList::Iterator pit;

	for ( pit = allGuiPaths.firstElt() ; pit ; pit++ ) {
		AtomPtr uiAtom = new UiAtom( proj, *pit );
		AtomPtr hdrAtom = new UicDeclHdrAtom( uiAtom );
		AtomPtr cppAtom = new UicImplCppAtom( uiAtom, hdrAtom );
		AtomPtr mocAtom = new MocCppAtom( hdrAtom );
		hdrAtom->addExternalDep( tempDir );
		cppAtom->addExternalDep( tempDir );
		mocAtom->addExternalDep( tempDir );
		uiList.insertLast( uiAtom );
		hdrList.insertLast( hdrAtom );
		cppList.insertLast( cppAtom );
		cppList.insertLast( mocAtom );
		tmpHdrAtoms->addInternalDep( hdrAtom );
	}

	// Other *.ui files are translated into *.vui (VMP UI files)...

	for ( pit = allQuiPaths.firstElt() ; pit ; pit++ ) {
		AtomPtr uiAtom = new UiAtom( proj, *pit );
		AtomPtr vuiAtom = new Ui2VuiAtom( uiAtom );
		vuiAtom->addExternalDep( tempDir );
		uiList.insertLast( uiAtom );
		vuiList.insertLast( vuiAtom );
	}

	// Additional "mocifiable" files produce additional .cpp files...

	for ( pit = allMocPaths.firstElt() ; pit ; pit++ ) {
		AtomPtr hdrAtom = new UserHdrAtom( proj, *pit );
		AtomPtr mocAtom = new MocCppAtom( hdrAtom );
		mocAtom->addExternalDep( tempDir );
		hdrList.insertLast( hdrAtom );
		cppList.insertLast( mocAtom );
	}

	// Append all user-specified .vui files now...

	for ( pit = allVuiPaths.firstElt() ; pit ; pit++ ) {
		AtomPtr vuiAtom = new UserVuiAtom( proj, *pit );
		vuiList.insertLast( vuiAtom );
	}

	// All *.vui files are processed to give a pair of .h and .cpp files...

	AtomList::Iterator ait;

	for ( ait = vuiList.firstElt() ; ait ; ait++ ) {
		AtomPtr hdrAtom = new VuiDeclHdrAtom( *ait );
		AtomPtr cppAtom = new VuiImplCppAtom( *ait, hdrAtom );
		hdrAtom->addExternalDep( tempDir );
		cppAtom->addExternalDep( tempDir );
		hdrList.insertLast( hdrAtom );
		cppList.insertLast( cppAtom );
		tmpHdrAtoms->addInternalDep( hdrAtom );
	}

	// Append all user-specified .h files now...

	for ( pit = allHdrPaths.firstElt() ; pit ; pit++ ) {
		if ( ! foundIn( hdrList, *pit ) ) {
			hdrList.insertLast( new UserHdrAtom( proj, *pit ) );
		}
	}

	for ( pit = allCppPaths.firstElt() ; pit ; pit++ ) {
		if ( ! foundIn( cppList, *pit ) ) {
			cppList.insertLast( new UserCppAtom( proj, *pit ) );
		}
	}

	// All source files will get compiled, and generate an independent
	// dependencies file...

	for ( ait = cppList.firstElt() ; ait ; ait++ ) {
		AtomPtr depAtom = new DepAtom( *ait );
		depAtom->addPreBuildDep( tmpHdrAtoms );
		depAtom->addExternalDep( tempDir );
		depList.insertLast( depAtom );
#if defined( PLATFORM_WIN32 )
		AtomPtr objDbgStaAtom = new ObjAtom( *ait, depAtom, true , true  );
		AtomPtr objDbgDynAtom = new ObjAtom( *ait, depAtom, true , false );
		AtomPtr objRelStaAtom = new ObjAtom( *ait, depAtom, false, true  );
		AtomPtr objRelDynAtom = new ObjAtom( *ait, depAtom, false, false );
		objDbgStaAtom->addExternalDep( tempDir );
		objDbgDynAtom->addExternalDep( tempDir );
		objRelStaAtom->addExternalDep( tempDir );
		objRelDynAtom->addExternalDep( tempDir );
		objDbgStaList.insertLast( objDbgStaAtom );
		objDbgDynList.insertLast( objDbgDynAtom );
		objRelStaList.insertLast( objRelStaAtom );
		objRelDynList.insertLast( objRelDynAtom );
#else
		AtomPtr objDbgAtom = new ObjAtom( *ait, depAtom, true );
		AtomPtr objRelAtom = new ObjAtom( *ait, depAtom, false );
		objDbgAtom->addExternalDep( tempDir );
		objRelAtom->addExternalDep( tempDir );
		objDbgList.insertLast( objDbgAtom );
		objRelList.insertLast( objRelAtom );
#endif
	}

#if defined( PLATFORM_WIN32 )

	// All source files will generate browsing info, when compiled...

	AtomList	sbrStaList;
	AtomList	sbrDynList;

	for ( ait = objDbgStaList.firstElt() ; ait ; ait++ ) {
		ObjAtomPtr objAtom = *ait;
		SbrAtomPtr sbrAtom = new SbrAtom( objAtom );
		sbrAtom->addExternalDep( tempDir );
		objAtom->setSbrAtom( sbrAtom ); // back pointer!
		sbrStaList.insertLast( sbrAtom );
	}

	for ( ait = objDbgDynList.firstElt() ; ait ; ait++ ) {
		ObjAtomPtr objAtom = *ait;
		SbrAtomPtr sbrAtom = new SbrAtom( objAtom );
		sbrAtom->addExternalDep( tempDir );
		objAtom->setSbrAtom( sbrAtom ); // back pointer!
		sbrDynList.insertLast( sbrAtom );
	}

	// All .sbr are collected into a big .bsc file, placed at the root of
	// the project...

// FIXME: BSC & SBR handling!
//	AtomPtr dbgRoot = new BscAtom( sbrList );

#endif

	// All .obj are linked together to form an exe or a lib!

	// We've got our final list of supported configs!

	for ( i = 0 ; i < configs.size() ; i++ ) {
		const BuildConfig& cfg = configs.key( i );
//		msgDbg( "Supported config: " + cfg.toBuffer() );
		AtomPtr tgtRoot;
		if ( projectType == Executable ) {
			tgtRoot = new ExeAtom(
				proj,
#if defined( PLATFORM_WIN32 )
				cfg.dbgMode == Yes ? objDbgStaList : objRelStaList,
#else
				cfg.dbgMode == Yes ? objDbgList : objRelList,
#endif
				configs.value( i ),
				cfg.dbgMode == Yes,
				cfg.staMode == Yes );
		}
		else {	// projectType == Library!
			tgtRoot = new LibAtom(
				proj,
#if defined( PLATFORM_WIN32 )
				cfg.dbgMode == Yes
					? ( cfg.staMode == Yes ? objDbgStaList : objDbgDynList )
					: ( cfg.staMode == Yes ? objRelStaList : objRelDynList ),
#else
				cfg.dbgMode == Yes ? objDbgList : objRelList,
#endif
				configs.value( i ),
				cfg.dbgMode == Yes,
				cfg.staMode == Yes );
		}
		tgtRoot->addExternalDep( tempDir );
		tgtRoots.add( cfg, tgtRoot );
	}

	// Prepare a list of directories to use when compiling using our
	// local headers...

	for ( ait = hdrList.firstElt() ; ait ; ait++ ) {
		localInclDirs.insertLast( (*ait)->getAtomPath().getDirPath() );
	}

	ListUtils::removeDuplicates( localInclDirs );

	for ( pit = localInclDirs.firstElt() ; pit ; pit++ ) {
		if ( pit->isRelative() ) {
			*pit = getProjectPath() + *pit;
		}
	}

	// Debug the whole shit...

	if ( context->verbosity >= Message::Debug ) {
#if defined( PLATFORM_WIN32 )
		dumpAtomList( sbrStaList, "sbrStaList" );
		dumpAtomList( sbrDynList, "sbrDynList" );
		dumpAtomList( objDbgStaList, "objDbgStaList" );
		dumpAtomList( objRelStaList, "objRelStaList" );
		dumpAtomList( objDbgDynList, "objDbgDynList" );
		dumpAtomList( objRelDynList, "objRelDynList" );
#else
		dumpAtomList( objDbgList, "objDbgList" );
		dumpAtomList( objRelList, "objRelList" );
#endif
		dumpAtomList( depList, "depList" );
		dumpAtomList( cppList, "cppList" );
		dumpAtomList( hdrList, "hdrList" );
		dumpAtomList( vuiList, "vuiList" );
		dumpAtomList( uiList, "uiList" );
		dumpPathList( localInclDirs, "localInclDirs" );
		dumpBufferArray( addLibNames, "addLibNames" );
	}

}

// ============================================================================

void CppProject::unglob(
	const	BufferArray&		in,
		FSPathList&		out ) const {

	out.kill();

	for ( Uint32 i = 0 ;  i < in.size() ; i++ ) {
		FSGlobber::findMatches( in[ i ], out );
	}

}

// ============================================================================

void CppProject::dumpBufferArray(
	const	BufferArray&		fileList,
	const	Buffer&			listName ) const {

	if ( fileList.isEmpty() ) {
		return;
	}

	Buffer msg;

	msg = "[ " + listName + " ]:";

	for ( Uint32 i = 0 ; i < fileList.size() ; i++ ) {
		msg += " " + fileList[ i ];
	}

	Message( Message::MsgDebug, msg );

}

void CppProject::dumpAtomList(
	const	AtomList&		fileList,
	const	Buffer&			listName ) const {

	if ( fileList.isEmpty() ) {
		return;
	}

	Buffer msg;

	msg = "[ " + listName + " ]:";

	for ( AtomList::CstIterator it = fileList.firstElt() ; it ; it++ ) {
		msg += " " + (*it)->getAtomPath().toBuffer();
	}

	Message( Message::MsgDebug, msg );

}

void CppProject::dumpPathList(
	const	FSPathList&		fileList,
	const	Buffer&			listName ) const {

	if ( fileList.isEmpty() ) {
		return;
	}

	Buffer msg;

	msg = "[ " + listName + " ]";

	for ( FSPathList::CstIterator it = fileList.firstElt() ; it ; it++ ) {
		msg += " " + it->toBuffer();
	}

	Message( Message::MsgDebug, msg );

}

// ============================================================================

Bool CppProject::foundIn(
	const	AtomList&		list,
	const	FSPath&			path ) const {

	for ( AtomList::CstIterator it = list.firstElt() ; it ; it++ ) {
		if ( (*it)->getAtomPath() == path ) {
			return true;
		}
	}

	return false;

}

// ============================================================================

